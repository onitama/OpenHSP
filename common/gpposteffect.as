
#ifndef __gpposteffect__
#define __gpposteffect__

goto *skip_module

;-----------------------------------------------------------------
; 変数初期化(hsp3.7b7以降用)
#var crt_mat, cont_mat, blur_mat, blur_mat2, sobel_mat, oldfilm_mat, glow_mat, etime

#deffunc gppost_setup int _p1

	;	ポストエフェクト初期化
	postid = _p1
	postid_max = 10

	postname="None"
	post_prmsub = *setup_postsub
	sx=ginfo_sx:sy=ginfo_sy

	if postid=0 : gosub *setup_none
	if postid=1 : gosub *setup_crt
	if postid=2 : gosub *setup_contrast
	if postid=3 : gosub *setup_blur2
	if postid=4 : gosub *setup_mosaic
	if postid=5 : gosub *setup_sobel
	if postid=6 : gosub *setup_oldfilm
	if postid=7 : gosub *setup_glow
	if postid=8 : gosub *setup_sepia
	if postid=9 : gosub *setup_gray

	gsel 0
	return

#deffunc gppost_exec

	;	ポストエフェクト実行
	;
	gosub post_prmsub		; ポストエフェクト描画開始
	return


*setup_postsub
	return

*exec_copybuf
	;	buffer1 -> メイン画面にシェーダー描画
	gsel 0
	redraw 0			; 描画開始
	pos 0,0:gmode 0
	celput 1
	return

*setup_none
	;	フィルターなし
	buffer 1,sx,sy,screen_offscreen
	post_prmsub = *exec_copybuf
	return

*setup_sepia
	;	セピアフィルター
	postname="Sepia Filter"
	gpusershader "res/shaders/sprite.vert", "res/shaders/p_sepia.frag", ""
	buffer 1,sx,sy,screen_offscreen + screen_usergcopy
	post_prmsub = *exec_copybuf
	return

*setup_gray
	;	白黒フィルター
	postname="Gray Filter"
	gpusershader "res/shaders/sprite.vert", "res/shaders/p_grayscale.frag", ""
	buffer 1,sx,sy,screen_offscreen + screen_usergcopy
	post_prmsub = *exec_copybuf
	return

*setup_crt
	;	ブラウン管フィルター
	postname="CRT Filter"
	celload "res/crtmask.png",4	; オーバーレイ画像
	gpusershader "res/shaders/sprite.vert", "res/shaders/p_crtmonitor.frag", ""
	buffer 1,sx/2,sy/2,screen_offscreen + screen_usergcopy
	post_prmsub = *exec_crt
	gpgetmat crt_mat, 1, GPGETMAT_OPT_SCRMAT
	crt_curvex=0.5		; X方向の湾曲率
	crt_curvey=0.4		; Y方向の湾曲率
	gpmatprm1 crt_mat, "u_curvex", crt_curvex
	gpmatprm1 crt_mat, "u_curvey", crt_curvey
	return

*exec_crt
	;	buffer1 -> メイン画面にシェーダー描画
	gsel 0
	redraw 0			; 描画開始
	pos 0,0:gmode 0
	gzoom sx,sy,1,0,0,sx/2,sy/2
	;gosub *exec_copybuf
	;	オーバーレイを描画
	pos 0,0:gmode 1
	celput 4
	return

*setup_contrast
	;	コントラストフィルター
	postname="Contrast Filter"
	gpusershader "res/shaders/sprite.vert", "res/shaders/p_contrast.frag", ""
	buffer 1,sx,sy,screen_offscreen + screen_usergcopy
	post_prmsub = *exec_copybuf
	gpgetmat cont_mat, 1, GPGETMAT_OPT_SCRMAT
	contrast_level=2.0
	contrast_bright=-0.2
	gpmatprm1 cont_mat, "u_contrast", contrast_level
	gpmatprm1 cont_mat, "u_brightness", contrast_bright
	return

*setup_blur2
	;	ガウスぼかしフィルター
	postname="Blur Filter"
	gpusershader "res/shaders/p_blur2.vert", "res/shaders/p_blur2.frag", ""
	buffer 1,sx,sy,screen_offscreen + screen_usergcopy
	gpusershader "res/shaders/p_blur2.vert", "res/shaders/p_blur2.frag", ""
	;gpusershader "res/shaders/sprite.vert", "res/shaders/p_blur.frag", ""
	buffer 2,sx,sy,screen_offscreen + screen_usergcopy
	post_prmsub = *exec_blur2
	gpgetmat blur_mat, 1, GPGETMAT_OPT_SCRMAT
	gpgetmat blur_mat2, 2, GPGETMAT_OPT_SCRMAT

	blur_level=6.0			; ぼかし強度(1.0〜10.0程度)
	blur_stepx=1.0/sx
	blur_stepy=1.0/sy
	gpmatprm1 blur_mat2, "u_length", 0.0
	gpmatprm1 blur_mat2, "u_length2", blur_level
	
	return

*exec_blur2
	;	パラメーター更新
	dd=blur_level

*exec_blur_loop
	;	buffer1 -> buffer2にコピー
	gpmatprm1 blur_mat, "u_length", blur_stepx*dd
	gpmatprm1 blur_mat, "u_length2", 0.0
	gsel 2:redraw 0:pos 0,0:gmode 0:celput 1:redraw 1
	dd-=1.0

	if dd<=1.0 : goto *exec_blur_done

	;	buffer2 -> buffer1にコピー
	gpmatprm1 blur_mat2, "u_length", 0.0
	gpmatprm1 blur_mat2, "u_length2", blur_stepy*dd
	gsel 1:redraw 0:pos 0,0:gmode 0:celput 2:redraw 1
	dd-=1.0

	if dd<=1.0 : goto *exec_blur_done2

	goto *exec_blur_loop

*exec_blur_done2
	;	buffer1 -> buffer0にコピー
	gpmatprm1 blur_mat, "u_length", blur_stepx*dd
	gpmatprm1 blur_mat, "u_length2", 0.0
	gsel 0:redraw 0:pos 0,0:gmode 0:celput 1
	return

*exec_blur_done
	;	buffer2 -> buffer0にコピー
	gpmatprm1 blur_mat2, "u_length", 0.0
	gpmatprm1 blur_mat2, "u_length2", blur_stepy*dd
	gsel 0:redraw 0:pos 0,0:gmode 0:celput 2
	return

*setup_mosaic
	;	モザイクフィルター
	postname="Mosaic Filter"
	gpusershader "res/shaders/sprite.vert", "res/shaders/p_mosaic.frag", ""
	buffer 1,sx,sy,screen_offscreen + screen_usergcopy
	post_prmsub = *exec_copybuf
	return

*setup_sobel
	;	輪郭抽出フィルター
	postname="Sobel Filter"
	gpusershader "res/shaders/sprite.vert", "res/shaders/p_sobel.frag", ""
	buffer 1,sx,sy,screen_offscreen + screen_usergcopy
	post_prmsub = *exec_copybuf
	gpgetmat sobel_mat, 1, GPGETMAT_OPT_SCRMAT
	gpmatprm1 sobel_mat, "u_width", 1.0/sx/2
	gpmatprm1 sobel_mat, "u_height", 1.0/sy/2
	return

*setup_oldfilm
	;	古いフィルムフィルター
	postname="Old Film Filter"
	gpusershader "res/shaders/sprite.vert", "res/shaders/p_oldfilm.frag", ""
	buffer 1,sx,sy,screen_offscreen + screen_usergcopy
	post_prmsub = *exec_oldfilm
	gpgetmat oldfilm_mat, 1, GPGETMAT_OPT_SCRMAT
	gpmatprm1 oldfilm_mat, "u_sepiaValue", 0.8
	gpmatprm1 oldfilm_mat, "u_noiseValue", 0.4
	gpmatprm1 oldfilm_mat, "u_scratchValue", 0.4
	gpmatprm1 oldfilm_mat, "u_innerVignetting", 0.9
	gpmatprm1 oldfilm_mat, "u_outerVignetting", 0.9
	return

*exec_oldfilm
	;	パラメーター更新
	getreq etime,SYSREQ_TIMER
	gpmatprm1 oldfilm_mat, "u_elapsedTime", etime
	dd=double(rnd(32768))
	gpmatprm1 oldfilm_mat, "u_random", dd / 32768
	gosub *exec_copybuf
	return

*setup_glow
	;	グローフィルター
	postname="Glow Filter"
	buffer 1,sx,sy,screen_offscreen
	gpusershader "res/shaders/sprite.vert", "res/shaders/p_blur.frag", ""
	buffer 2,sx/2,sy/2,screen_offscreen + screen_usergcopy
	post_prmsub = *exec_glow
	gpgetmat glow_mat, 2, GPGETMAT_OPT_SCRMAT
	glow_base=1.0/sx*2
	gpmatprm1 glow_mat, "u_length", glow_base
	return

*exec_glow
	;	buffer1 -> buffer2に縮小してコピー
	gsel 2
	redraw 0
	pos 0,0:gmode 0
	gzoom sx/2,sy/2, 1, 0,0, sx,sy
	redraw 1
	;	buffer1と2を合成して描画
	gsel 0
	redraw 0			; 描画開始
	pos 0,0:gmode 1,,,128
	gzoom sx,sy, 2, 0,0, sx/2,sy/2
	pos 0,0:gmode 5,,,128
	celput 1
	return

;-----------------------------------------------------------------

*skip_module

#endif


