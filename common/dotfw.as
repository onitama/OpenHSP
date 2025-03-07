#ifndef __DOT_FW
#define __DOT_FW

#include "hsp3dish.as"
#include "layer_fade.as"
#include "mod_joystick2.as"
#include "mod_vpad.as"

;
;
;============================================================
; 珠音ドットフレームワーク
;  ドット絵を使った2Dゲーム向けのフレームワーク
;  (HSP3汎用ライブラリ)

	randomize
	_dotfw_flag = 0					; 初期化フラグ
	_dotfw_opt = 0					; 設定オプション
	_dotfw_style = 0				; ゲームスタイル
	_dotfw_sx = 320					; 横サイズ
	_dotfw_sy = 224					; 縦サイズ
	_dotfw_dir = 0					; 横長=0,縦長=1
	_dotfw_zoom = 300				; ウインドウ表示拡大率
	_dotfw_aspect = 100				; アスペクト比
	_dotfw_fps = 60					; フレームレート
	_dotfw_fullscr = 0				; フルスクリーン
	_dotfw_noresize = 0				; 画面リサイズなし
	_dotfw_sprmax = 256				; エフェクトスプライト最大数
	_dotfw_essmax = 1024			; HSP3Dishスプライト最大数
	_dotfw_bgpic_max = 6			; 背景BG最大数
	_dotfw_frame = 0				; フレーム
	_dotfw_enemy = 0				; 敵表示数
	_dotfw_player = 0				; プレイヤー表示数
	_dotfw_effect = 0				; エフェクト表示数
	_dotfw_cursp = 0				; カレントスプライト
	_dotfw_hitsp = 0				; 接触スプライト
	_dotfw_curemode = 0				; カレント敵モード
	_dotfw_curemis = 0				; カレント敵ミサイル
	_dotfw_interval = 8				; 連射インターバル
	_dotfw_contkey = 15				; 連続入力キー
	_dotfw_myx =0					; プレイヤーX
	_dotfw_myy =0					; プレイヤーY
	_dotfw_myani =0					; プレイヤーアニメーションカウンタ
	_dotfw_mydir =0					; プレイヤー方向ID
	_dotfw_myjmp =0					; プレイヤージャンプフラグ
	_dotfw_cx =0					; 敵X
	_dotfw_cy =0					; 敵Y
	_dotfw_movex =0					; 移動成分X
	_dotfw_movey =0					; 移動成分Y
	_dotfw_enemy_turn =0			; 敵設定(ターン数)
	_dotfw_enemy_timer =0			; 敵設定(タイマー値)
	_dotfw_enemy_speed =0			; 敵設定(速度%)
	_dotfw_enemy_dir =0				; 敵設定(方向)
	_dotfw_enemy_exprm =0			; 敵設定(exprm)
	_dotfw_update_flag =0			; 更新フラグ(UPDATE_*)
	_dotfw_curmapx = 0				; カレントマップX
	_dotfw_curmapy = 0				; カレントマップY
	_dotfw_curmapcel = 0			; カレントマップセルNo.
	_dotfw_curmapattr = 0			; カレントマップアトリビュート
	_dotfw_bgcolor = 0				; 背景色(RGB)
	_dotfw_device = ""				; デバイス名
	_dotfw_joystick = -1			; ジョイスティック有効フラグ
	_dotfw_vpad = 0					; バーチャルパッド有効フラグ
	_dotfw_vpadopt = 0				; バーチャルパッドオプション

	_dotfw_soundflag = 0			; サウンド初期化フラグ(0=none/1=OK)
	_dotfw_music = 0				; サウンド(音楽)対応
	_dotfw_se = 0					; サウンド(効果音)対応

	dim _dotfw_res, 16
	_dotfw_resmax = 0
	sdim _dotfw_cfghash,256
	sdim s1,256
	sdim s2,256
	sdim s3,256
	key = 0							; キーコード(stickの値)

#module HSP3MOD_DOTFW

#define global DOTFW_AFONT (0)
#define global DOTFW_BFONT (1)
#define global DOTFW_CFONT (2)
#define global DOTFW_DFONT (3)

#define global DOTFW_FONTID (3)
#define global DOTFW_TAMAID (4)
#define global DOTFW_STARID (5)
#define global DOTFW_TAMARES "tamane16.png"
#define global DOTFW_VPADID (6)

#const global DOTFW_CHRX 16
#const global DOTFW_CHRY 16

#const global STYLE_DEFAULT (0)
#const global STYLE_SHOOT (1)
#const global STYLE_JUMP (2)

#const global UPDATE_NOWAIT (1)
#const global UPDATE_PAUSE (2)
#const global UPDATE_NOCONTROL (4)

#const global TYPE_NONE (0)
#const global TYPE_PLAYER (1)
#const global TYPE_NPC (2)
#const global TYPE_ITEM (4)
#const global TYPE_PMISSLE (8)
#const global TYPE_PBOMB (16)
#const global TYPE_EMISSLE (32)
#const global TYPE_ENEMY (64)
#const global TYPE_EXTENEMY (128)
#const global TYPE_NCENEMY (256)

#const global DIR_UP (128)
#const global DIR_DOWN (0)
#const global DIR_LEFT (192)
#const global DIR_RIGHT (64)
#const global DIR_AIM (-1)

#const global CHR_TAMANE (0)
#const global CHR_TAMANE2 (4)
#const global CHR_TAMANE3 (8)
#const global CHR_TAMANE4 (12)
#const global CHR_MONSTER1 (16)
#const global CHR_MONSTER2 (18)
#const global CHR_MONSTER3 (20)
#const global CHR_MONSTER4 (22)
#const global CHR_BOM (24)
#const global CHR_MISSILE (30)
#const global CHR_MISSILE2 (31)
#const global CHR_DOT1 (32)
#const global CHR_DOT2 (33)
#const global CHR_DOT3 (34)
#const global CHR_DOT4 (35)
#const global CHR_BALL1 (36)
#const global CHR_BALL2 (37)
#const global CHR_BALL3 (38)
#const global CHR_BALL4 (39)
#const global CHR_WAVE (40)
#const global CHR_SHIP1 (41)
#const global CHR_SHIP2 (42)
#const global CHR_SHIP3 (43)
#const global CHR_SOUPCUP (44)
#const global CHR_HSED (45)
#const global CHR_ONION (46)
#const global CHR_LIGHT (47)
#const global CHR_CIRCLE (48)
#const global CHR_UFO (50)
#const global CHR_VADER (51)
#const global CHR_STONE1 (52)
#const global CHR_STONE2 (53)
#const global CHR_STONE3 (54)
#const global CHR_STONE4 (55)
#const global CHR_COIN (56)
#const global CHR_BOX (57)
#const global CHR_MEAT (58)
#const global CHR_SWORD (59)
#const global CHR_ITEM1 (60)
#const global CHR_ITEM2 (61)
#const global CHR_ITEM3 (62)
#const global CHR_ITEM4 (63)
#const global CHR_ITEM5 (64)

#const global PACTION_CONTROL (0)
#const global PACTION_HIT (1)
#const global PACTION_KEY (2)
#const global PACTION_BUTTON (3)
#const global PACTION_BUTTON2 (4)
#const global PACTION_BUTTON3 (5)
#const global PACTION_MISS (6)
#const global PACTION_ITEM (7)
#const global PACTION_MAPITEM (8)
#const global PACTION_MAPNOTICE (9)

#const global DACTION_SPROVER (0)
#const global DACTION_TOPMOST (1)

#const global PLAYER_BUTTON_NORMAL (0)
#const global PLAYER_BUTTON_INT1 (1)
#const global PLAYER_BUTTON_INT2 (2)
#const global PLAYER_BUTTON_INT4 (4)
#const global PLAYER_BUTTON_INT8 (8)

#const global PLAYER_KEY_BUTTON1 ($1050)
#const global PLAYER_KEY_BUTTON2 ($800)
#const global PLAYER_KEY_BUTTON3 ($2000)
#const global PLAYER_KEY_ESC ($80)

#const global EACTION_CONTROL (0)
#const global EACTION_HIT (1)
#const global EACTION_SHOT (2)
#const global EACTION_DESTROY (3)

#const global EMODE_SEQ ($10000)

#const global EMODE_XAIM (1)
#const global EMODE_YAIM (2)

#define global DOTFW_USE_STARFIELD (4)
#define global DOTFW_USE_BGPIC (8)
#define global DOTFW_USE_BGMAP (16)

#const global DOTFW_SPRSCALE 16
#const global DOTFW_SPRSHIFT 4

#define global DOTFW_BGID_TEXT (0)
#define global DOTFW_BGID_BGMAP (1)
#define global DOTFW_BGID_MAPMAX (4)

#define global DOTFW_PACTTYPE_NORMAL (0)
#define global DOTFW_PACTTYPE_JUMP (1)
#define global DOTFW_PACTTYPE_2DMAP (2)


#const SPR_NONE 0
#const SPR_TIMER $fff
#const SPR_OK $1000
#const SPR_MES $2000
#const SPR_FMES $4000
#const SPR_MOVE $8000
#const SPR_ANIM4 $10000
#const SPR_ANIM8 $20000
#const SPR_ANIM16 $30000

#const BGID_MAP $10000

#deffunc df_reset int _p1

	;	ドットフレームワークの初期化
	;
	if _p1>0 : _dotfw_opt@|=_p1
	;
	if _dotfw_flag@ = 0 {
		_dotfw_flag@=$10000+_dotfw_opt@
		sx = _dotfw_sx@
		sy = _dotfw_sy@
		zoomx = double(_dotfw_zoom@)/100.0
		zoomy = zoomx * _dotfw_aspect@ / 100.0
		wx = int( zoomx * sx )
		wy = int( zoomy * sy )
		if _dotfw_fullscr@ {
			bgscr 0,wx,wy,256
		} else {
			if _dotfw_noresize@=0 {
				screen 0,wx,wy
			}
		}
	}
	;
	devinfo _dotfw_device@,"name"
	if _dotfw_device@="emscripten" : _dotfw_joystick@ = 0
	if _dotfw_device@="Androiddev" : _dotfw_joystick@ = 0
	if _dotfw_device@="iOSdev" : _dotfw_joystick@ = 0
	;
	setcls CLSMODE_NONE
	cls
	viewcalc vptype_2d, zoomx, zoomy
	_dotfw_dir@ = 0
	if wy>wx : _dotfw_dir@ = 1
	;
	redraw 0
	rgbcolor _dotfw_bgcolor@:boxf
	redraw 1
	;
	es_ini _dotfw_essmax@,1024,256		; スプライト初期化
	_dotfw_rotbase@ = 6.28318530718 / 256
	es_area  -DOTFW_CHRX,-DOTFW_CHRY,sx+DOTFW_CHRX,sy+DOTFW_CHRY
	_dotfw_frame@ = 0

	if _dotfw_soundflag@ = 0 {
		_dotfw_soundflag@=1
	}

	;	フェード登録
	layer_fade

	;	8x8フォント
	;
	fntfile="afont8.png"
	poke fntfile, 0, (_dotfw_flag@ & 3)+'a'
	celload fntfile, DOTFW_FONTID
	celdiv DOTFW_FONTID,8,8
	texbg=0
	texsx=sx/8:texsy=sy/8		; テキストのマップサイズ
	dim tvram,texsx*texsy		; マップデータ変数
	es_bgmap texbg,tvram,texsx,texsy,texsx,texsy,DOTFW_FONTID	; マップ登録

	;	珠音16dot
	;
	buffer DOTFW_TAMAID
	picload DOTFW_TAMARES
	celdiv DOTFW_TAMAID,DOTFW_CHRX,DOTFW_CHRY
	es_size DOTFW_CHRX,DOTFW_CHRY,60
	aniframe=4
	;	珠音ちゃん
	es_patanim 0, 4, 0,0, aniframe
	es_patanim 4, 4, 4*DOTFW_CHRX,0, aniframe
	es_patanim 8, 4, 0, DOTFW_CHRX, aniframe
	es_patanim 12, 4, 4*DOTFW_CHRX, DOTFW_CHRY, aniframe
	;	敵モンスター
	no=CHR_MONSTER1
	repeat 4
	es_patanim no, 2, 2*DOTFW_CHRX*cnt, 2*DOTFW_CHRY, 8
	no+=2
	loop
	es_patanim CHR_BOM, 6, 0, 3*DOTFW_CHRY, 4
	repeat 4
	es_pat CHR_STONE1+cnt, (4+cnt)*DOTFW_CHRX,6*DOTFW_CHRY
	es_pat CHR_COIN+cnt, cnt*DOTFW_CHRX,7*DOTFW_CHRY
	es_pat CHR_ITEM1+cnt, (4+cnt)*DOTFW_CHRX,7*DOTFW_CHRY
	loop
	repeat 8
	es_pat CHR_WAVE+cnt, cnt*DOTFW_CHRX,5*DOTFW_CHRY
	loop
	es_patanim CHR_CIRCLE, 2, 0, 6*DOTFW_CHRY, 8
	es_pat CHR_UFO, 2*DOTFW_CHRX,6*DOTFW_CHRY
	es_pat CHR_VADER, 3*DOTFW_CHRX,6*DOTFW_CHRY
	es_patanim CHR_ITEM5, 2, 0, 3*DOTFW_CHRY, 8

	es_size DOTFW_CHRX,DOTFW_CHRY,80
	es_pat CHR_MISSILE, 6*DOTFW_CHRX,3*DOTFW_CHRY
	es_pat CHR_MISSILE2, 7*DOTFW_CHRX,3*DOTFW_CHRY
	repeat 8
	es_pat CHR_DOT1+cnt, cnt*DOTFW_CHRX,4*DOTFW_CHRY
	loop
	gsel 0
	es_regdeco CHR_BOM,0, 0,0, 24						; ダミーの登録


	;	星(Star Field)
	;
	if _dotfw_flag@ & DOTFW_USE_STARFIELD {
		buffer DOTFW_STARID,256,256,screen_offscreen
		dim bitmap,256*256
		celdiv DOTFW_STARID,256,256,128,128
		gsel 0
		star_zx = 1.0
		star_zy = 1.25
		star_rot = 0
		star_rate = 1
		if sy>sx {
			star_zx = 1.25
			star_zy = 1.0
			star_rot = 1
		}
	}

	;	プレイヤー設定
	;
	sp_player_mode=-1
	sp_player_acttype=0
	sp_player_map=0
	sp_player_shtmap=0
	sp_player=0
	sp_player_tamane=0
	sp_player_speedx=2
	sp_player_speedy=2
	sp_player_x1=0
	sp_player_y1=0
	sp_player_x2=sx-DOTFW_CHRX
	sp_player_y2=sy-DOTFW_CHRY
	sp_player_bgtrack=0
	sp_player_bgxrate=0
	sp_player_bgyrate=0
	sp_player_bgx1=0
	sp_player_bgy1=0
	sp_player_bgx2=0
	sp_player_bgy2=0

	dupdate0_lb = *actdefault
	dupdate1_lb = *actdefault

	player_actlb=*actdefault
	player_keylb=*pcont_normal
	player_btn1lb=*player_act_shot
	player_btn2lb=*actdefault
	player_btn3lb=*actdefault
	player_misslb=*player_act_miss
	player_itemlb=*actdefault
	player_mapitemlb=*actdefault
	player_mapnoticelb=*actdefault

	player_button_cnt=0
	player_button_int=_dotfw_interval@
	_dotfw_contkey@ = 15

	gmp_map1=0
	gmp_map2=0
	gmp_map3=0
	gmp_map4=0
	myx=0:myy=0
	sp_player_bgxadj=0
	sp_player_bgyadj=0
	res=0

	;	敵設定
	;
	dim enemy_mode,_dotfw_essmax@
	dim enemy_cnt,_dotfw_essmax@
	dim enemy_interval,_dotfw_essmax@
	dim enemy_misrate,_dotfw_essmax@
	dim enemy_seqid,_dotfw_essmax@
	dim enemy_speed,_dotfw_essmax@
	dim enemy_dir,_dotfw_essmax@
	dim enemy_timer,_dotfw_essmax@
	dim enemy_turn,_dotfw_essmax@
	dim enemy_exprm,_dotfw_essmax@
	;
	enemy_modemax=1
	ldim enemy_modelb,16
	;
	enemy_addrate=10
	enemy_addinterval=8
	enemy_actlb=*enemy_act_move
	enemy_hitlb=*enemy_act_hit
	enemy_shotlb=*enemy_act_shot
	enemy_destlb=*actdefault
	enemy_def_mode=0
	enemy_def_interval=8
	enemy_def_misrate=10
	enemy_def_chr=CHR_MONSTER1
	enemy_def_speed=200
	enemy_def_exprm=0
	enemy_def_timer=0
	enemy_def_opt=0
	enemy_x1=-DOTFW_CHRX
	enemy_y1=-DOTFW_CHRX
	enemy_x2=sx
	enemy_y2=sy
	enemy_misrange=0
	;
	dim eseq_mode,16
	dim eseq_timer,16
	dim eseq_exprm,16
	eseq_max=0
	etype=0
	numinfo=0
	;
	dim bgp_id,_dotfw_bgpic_max@
	dim bgp_gx,_dotfw_bgpic_max@
	dim bgp_gy,_dotfw_bgpic_max@
	dim bgp_px,_dotfw_bgpic_max@
	dim bgp_py,_dotfw_bgpic_max@
	dim bgp_sx,_dotfw_bgpic_max@
	dim bgp_sy,_dotfw_bgpic_max@
	;
	df_emodesub *esub_xaim,EMODE_XAIM
	df_emodesub *esub_yaim,EMODE_YAIM
	dim hitinfo,10


	;	エフェクト用内部スプライト
	;
	sprmax=_dotfw_sprmax@
	dim sprflag,sprmax
	dim sprx,sprmax
	dim spry,sprmax
	dim sprchr,sprmax
	dim sprid,sprmax
	dim sprgx,sprmax
	dim sprgy,sprmax
	dim sprgxp,sprmax
	dim sprcnt,sprmax
	dim spranim,sprmax
	sdim sprmes,64,sprmax

	;	バーチャルパッド
	;
	if _dotfw_vpad@ : hspvpad_init _dotfw_vpad@, _dotfw_vpadopt@		; バーチャルパッドの初期化

	redraw 0
	gosub *df_bgput
	return

*df_bgput
	;	背景表示
	;
	gfilter FILTER_NONE
	;
	rgbcolor _dotfw_bgcolor@:boxf
	;
	gmode 1
	;gfilter FILTER_LINEAR2

	if _dotfw_flag@ & DOTFW_USE_STARFIELD {
		if star_rate>=0 {
			celbitmap DOTFW_STARID,bitmap,$1000+star_rate
			gmode 0
			pos _dotfw_sx@/2,_dotfw_sy@/2
			rot = 4.71239 * star_rot
			celput DOTFW_STARID,0,star_zx,star_zy,rot
		}
	}
	gmp_mode=1
	repeat _dotfw_bgpic_max@
	gmp_id = bgp_id(cnt)
	if gmp_id=0 : continue
	pos 0,0

		x=bgp_gx(cnt)>>10
		y=bgp_gy(cnt)>>10

		if gmp_id&BGID_MAP {
			gmp_id=gmp_id & (BGID_MAP-1)
			es_bgparam gmp_id, 0, ESMAP_PRM_GROUP
			es_bgparam gmp_id, (_dotfw_frame@>>3)&3, ESMAP_PRM_ANIM
			es_putbg gmp_id,0,0,x,y				; マップBGを描画
			continue
		}

		gmode gmp_mode,_dotfw_sx@,_dotfw_sy@,255:gcopy gmp_id,x,y
		bgi=bgp_sx(cnt)-x
		if bgi<_dotfw_sx@ {
			pos bgi-1,0:gcopy gmp_id,0,y
			bgj=bgp_sy(cnt)-y
			if bgj<_dotfw_sy@ {
				pos 0,bgj-1:gcopy gmp_id,x,0
				pos bgi-1,bgj-1:gcopy gmp_id,0,0
			}
		} else {
			bgj=bgp_sy(cnt)-y
			if bgj<_dotfw_sy@ {
				pos 0,bgj-1:gcopy gmp_id,x,0
			}
		}
		gmp_mode=3
	loop
	;
	return

*df_bgput2
	;	背景表示(手前)
	;
	gmp_mode=1
	repeat _dotfw_bgpic_max@
		gmp_id = bgp_id(cnt)
		if gmp_id=0 : continue
		pos 0,0
		if gmp_id&BGID_MAP {
			x=bgp_gx(cnt)>>10
			y=bgp_gy(cnt)>>10
			gmp_id=gmp_id & (BGID_MAP-1)
			es_bgparam gmp_id, 1+16, ESMAP_PRM_GROUP
			es_bgparam gmp_id, (_dotfw_frame@>>3)&3, ESMAP_PRM_ANIM
			es_putbg gmp_id,0,0,x,y				; マップBGを描画
		}
	loop
	;
	return

*df_bgupdate
	;	背景更新
	;
	repeat _dotfw_bgpic_max@
	gmp_id = bgp_id(cnt)
	if gmp_id=0 : continue

		if sp_player_bgtrack=0 {
			bgp_gx(cnt)+=bgp_px(cnt)
			bgp_gy(cnt)+=bgp_py(cnt)
		}
		x=bgp_gx(cnt)>>10
		y=bgp_gy(cnt)>>10
		if x>=bgp_sx(cnt) {
			x=0
			bgp_gx(cnt)=0
		}
		if (x<0) {
			x=bgp_sx(cnt)-1
			bgp_gx(cnt)=x<<10
		}
		if y>=bgp_sy(cnt) {
			y=0
			bgp_gy(cnt)=0
		}
		if (y<0) {
			y=bgp_sy(cnt)-1
			bgp_gy(cnt)=y<<10
		}
	loop
	;
	return


*actdefault
	;	アクション用デフォルトラベル(何もしない)
	return


;------------------------------------------------------------
;	内部用スプライト
;------------------------------------------------------------

#deffunc dfi_sprdel int _p1

	;	(内部用)スプライト削除
	;	id
	;
	sprflag(_p1)=SPR_NONE
	return

#deffunc dfi_sprtimer int _p1, int _p2

	;	(内部用)削除タイマー設定
	;	id, timer
	;
	flg=sprflag(_p1)&$fffff000
	sprflag(_p1)=flg+_p2
	return

#deffunc dfi_sprmove int _p1, int _p2, int _p3

	;	(内部用)スプライト座標設定
	;	id, x,y
	;
	sprx(_p1)=_p2<<DOTFW_SPRSHIFT
	spry(_p1)=_p3<<DOTFW_SPRSHIFT
	return

#deffunc dfi_sprchr int _p1, int _p2, int _p3

	;	(内部用)スプライトキャラクター設定
	;	id, chr, bufid
	;
	sprid(_p1)=_p3
	if sprid(_p1) <= 0 : sprid(_p1)=DOTFW_TAMAID
	sprchr(_p1)=_p2
	return

#deffunc dfi_sprset int _p1, int _p2, int _p3, int _p4, int _p5

	;	(内部用)スプライト設置
	;	id, x,y, chr, bufid
	;
	sprflag(_p1)=SPR_OK
	sprx(_p1)=_p2<<DOTFW_SPRSHIFT
	spry(_p1)=_p3<<DOTFW_SPRSHIFT
	dfi_sprchr _p1, _p4, _p5
	sprgx(_p1)=0
	sprgy(_p1)=0
	sprgxp(_p1)=0
	sprcnt(_p1)=0
	spranim(_p1)=0
	return

#deffunc dfi_move int _p1, int _p2, int _p3

	;	(内部用)スプライト拡張パラメーター設定
	;	id, gx,gy
	;
	sprflag(_p1)|=SPR_MOVE
	sprgx(_p1)=_p2
	sprgy(_p1)=_p3
	return

#deffunc dfi_rotmove int _p1, int _p2, int _p3

	;	(内部用)スプライト拡張パラメーター設定
	;	id, rot, factor(16=1.0)
	;
	sprflag(_p1)|=SPR_MOVE
	rot=_dotfw_rotbase@*_p2
	px=int(sin(rot)*_p3)
	py=int(cos(rot)*_p3)
	sprgx(_p1)=px
	sprgy(_p1)=py
	return

#deffunc dfi_sprstr int _p1, str _p2, int _p3, int _p4

	;	(内部用)スプライト文字設定
	;	id, "str", color, wait
	;
	sprflag(_p1)=SPR_MES
	sprmes(_p1)=_p2
	sprgx(_p1)=_p3
	spranim(_p1)=_p4
	return

#deffunc dfi_spranim int _p1, int _p2, int _p3, local i

	;	(内部用)スプライトアニメーション設定
	;	id, count, animpat
	;
	spranim(_p1)=_p2
	i=0
	if _p3>=4 : i=SPR_ANIM4
	if _p3>=8 : i=SPR_ANIM8
	if _p3>=16 : i=SPR_ANIM16
	sprflag(_p1)|=i
	return

#deffunc dfi_sprnew

	;	(内部用)スプライト新規ID
	;
	spid=sprmax-1
	repeat sprmax
		if sprflag(spid)=SPR_NONE : break
		spid--
	loop
	if spid<0 : spid=sprmax-1
	return spid

#deffunc dfi_sprupdate

	;	(内部用)スプライト更新
	;
	gmode 2,16,16
	repeat sprmax
	flg=sprflag(cnt)
	if flg=SPR_NONE : continue

	i=flg&SPR_TIMER
	if i>0 {
		sprflag(cnt)=flg-1
		i--:if i=0 : sprflag(cnt)=0
	}
	sprcnt(cnt)++

	if flg&SPR_OK {
		if flg&SPR_MOVE {
			sprx(cnt)+=sprgx(cnt)
			spry(cnt)+=sprgy(cnt)
		}
		if spranim(cnt)>0 {
			if sprcnt(cnt)>=spranim(cnt) {
				sprcnt(cnt)=0
				i=2
				if flg&SPR_ANIM4 : i*=2
				if flg&SPR_ANIM8 : i*=4
				sprgxp(cnt)++
				if sprgxp(cnt)>=i {
					sprgxp(cnt)=0
				}
			}
		}
		continue
	}
	if flg&SPR_MES {
		if spranim(cnt)>0 {
			i=sprgxp(cnt)
			if sprcnt(cnt)>=spranim(cnt) {
				sprcnt(cnt)=0
				sprgxp(cnt)++
			}
		}
		continue
	}
	if flg&SPR_FMES {
		if spranim(cnt)>0 {
			if sprcnt(cnt)>=spranim(cnt) {
				sprcnt(cnt)=0
				sprgxp(cnt)++
			}
		}
		continue
	}
	loop
	return

#deffunc dfi_sprdraw

	;	(内部用)スプライト描画
	;
	_dotfw_effect@=0
	gmode 2,16,16
	repeat sprmax
	flg=sprflag(cnt)
	if flg=SPR_NONE : continue

	_dotfw_effect@++
	x=sprx(cnt)>>DOTFW_SPRSHIFT:y=spry(cnt)>>DOTFW_SPRSHIFT

	if flg&SPR_OK {
		pos x,y
		celput sprid(cnt),sprchr(cnt)+sprgxp(cnt)
		continue
	}
	if flg&SPR_MES {
		s1=sprmes(cnt)
		nstr_end=1
		if spranim(cnt)>0 {
			i=sprgxp(cnt)
			a1=peek(s1,i)
			if a1!=0 : nstr_end=0
			poke s1,i,0
		}
		es_bgmes texbg,x,y,s1,256*sprgx(cnt)
		if nstr_end {
			sprflag(cnt)=SPR_NONE		; 全部出し終わった
		}
		continue
	}
	if flg&SPR_FMES {
		s1=sprmes(cnt)
		if spranim(cnt)>0 {
			i=sprgxp(cnt)
			gosub *nstr_wipe
		}
		pos x,y:rgbcolor sprgx(cnt):mes s1
		continue
	}
	loop
	return

*nstr_wipe
	;	1文字ずつ出す(iのカウントで区切る)
	j=0
	nstr_end=0
	skipbyte=2
	if hspstat&$20000 : skipbyte=3		; UTF8の簡易的な対応
	repeat i
		a1=peek(s1,j)
		if a1=0 {
			nstr_end=1
			break
		}
		if a1>=128 {
			j+=skipbyte : continue
		}
		j++
	loop
	poke s1,j,0
	return



;------------------------------------------------------------
;	外部用(システム)
;------------------------------------------------------------

#deffunc df_screen int _p1, int _p2, int _p3, int _p4

	;	解像度設定
	;	sx,sy,zoom,fps
	;
	if _p1>0 : _dotfw_sx@ = _p1
	if _p2>0 : _dotfw_sy@ = _p2
	if _p3>0 : _dotfw_zoom@ = _p3
	if _p4>0 : _dotfw_fps@ = _p4
	_dotfw_flag@ = 0
	return


#deffunc df_zoom int _p1, int _p2

	;	ズーム設定
	;	zoom, aspect
	;
	_dotfw_zoom@ = 300
	if _p1>0 : _dotfw_zoom@ = _p1
	_dotfw_aspect@ = 100
	if _p2>0 : _dotfw_aspect@ = _p2
	_dotfw_flag@ = 0
	return


#deffunc df_config str _p1

	;	タグ名による設定
	;	"tag1 tag2..."
	;
	_dotfw_opt@=0
	_dotfw_cfghash@=_p1
	index=0
	errflg=0
	repeat
		getstr s1,_dotfw_cfghash@,index,32,64
		if strsize=0 : break
		index+=strsize
		gosub *hashchk
		if errflg : dialog "#Unknown Tag : "+s1 : break
	loop
	return

*hashchk
	value=0+strmid(s1,1,64)
	a1=peek(s1,0)
	if a1='Z' {
		_dotfw_zoom@ = value
		_dotfw_flag@ = 0
		return
	}
	if a1='A' {
		_dotfw_aspect@ = value
		_dotfw_flag@ = 0
		return
	}
	if a1='X' {
		_dotfw_sx@ = value
		_dotfw_flag@ = 0
		return
	}
	if a1='Y' {
		_dotfw_sy@ = value
		_dotfw_flag@ = 0
		return
	}
	if a1='F' {
		_dotfw_fps@ = value
		return
	}
	if a1='K' {
		_dotfw_interval@ = value
		return
	}
	if a1='P' {
		_dotfw_contkey@ = 15
		if value&1 : _dotfw_contkey@ += PLAYER_KEY_BUTTON2
		if value&2 : _dotfw_contkey@ += PLAYER_KEY_BUTTON3
		return
	}
	if a1='V' {
		_dotfw_vpadopt@ = value
		return
	}

	if s1="star" : _dotfw_opt@ |= DOTFW_USE_STARFIELD : return
	if s1="afont" : _dotfw_opt@ |= DOTFW_AFONT : return
	if s1="bfont" : _dotfw_opt@ |= DOTFW_BFONT : return
	if s1="cfont" : _dotfw_opt@ |= DOTFW_CFONT : return
	if s1="dfont" : _dotfw_opt@ |= DOTFW_DFONT : return

	if s1="fullscr" : _dotfw_fullscr@ = 1 : return

	if s1="pce" : df_screen 320,224 : return
	if s1="md" : df_screen 320,224 : return
	if s1="fc" : df_screen 256,224 : _dotfw_aspect@=85 : return
	if s1="sfc" : df_screen 256,224 : _dotfw_aspect@=85 : return
	if s1="gb" : df_screen 160,144 : return
	if s1="gba" : df_screen 240,160 : return
	if s1="nds" : df_screen 256,192 : return
	if s1="3ds" : df_screen 320,240 : return
	if s1="n64" : df_screen 320,240 : return
	if s1="ngc" : df_screen 640,480 : return
	if s1="ss" : df_screen 320,224 : return
	if s1="dc" : df_screen 640,480 : return
	if s1="ps1" : df_screen 320,240 : return
	if s1="ps2" : df_screen 640,480 : return
	if s1="psp" : df_screen 480,272 : return
	if s1="vita" : df_screen 960,544 : return
	if s1="msx" : df_screen 256,192 : return
	if s1="msx2" : df_screen 512,424 : return
	if s1="appleii" : df_screen 280,192 : return
	if s1="c64" : df_screen 320,200 : _dotfw_aspect@=120 : return
	if s1="pc60" : df_screen 256,192 : return
	if s1="pc80" : df_screen 320,200 : _dotfw_aspect@=120 : return
	if s1="pc88" : df_screen 640,200 : _dotfw_aspect@=240 : return
	if s1="pc98" : df_screen 640,400 : _dotfw_aspect@=120 : return
	if s1="x68k" : df_screen 768,512 : return
	if s1="arcade" : df_screen 224,320 : return
	if s1="neogeo" : df_screen 320,224 : return

	if s1="se" : _dotfw_se@ = 1 : return
	if s1="music" : _dotfw_music@ = 1 : return
	if s1="vpad" : _dotfw_vpad@ = DOTFW_VPADID : return

	if s1="shoot" : _dotfw_style@ = STYLE_SHOOT : return
	if s1="jump" : _dotfw_style@ = STYLE_JUMP : return

	if s1="noresize" : _dotfw_noresize@=1 : return

	errflg++
	return


#deffunc df_star int _p1, int _p2, int _p3, int _p4

	;	星表示パラメーター設定
	;
	star_rate = _p1
	if _p1<0 : return
	x=100:if _p2>0 : x=_p2
	y=100:if _p3>0 : y=_p3
	star_zx = double(x)/100.0
	star_zy = double(y)/100.0
	star_rot = _p4
	return


#deffunc df_mes str _p1, int _p2, int _p3, int _p4

	;	テキスト表示
	;
	es_bgmes texbg,_p2,_p3,_p1,256*_p4
	return


#deffunc df_vpoke int _p1, int _p2, int _p3, int _p4

	;	テキストにキャラクタコード書き込み
	;	x,y,code,length
	x=_p1:i=texsx*_p2+x:z=_p4:a=_p3
	if z<=0 : z=1
	repeat z
		tvram(i)=a
		x++:if x>=texsx : break
		i++
	loop
	return


#defcfunc df_vpeek int _p1, int _p2

	;	テキストのキャラクタコードを返す
	;
	if _p1<0|_p1>=texsx : return -1
	if _p2<0|_p2>=texsy : return -1
	return tvram(texsx*_p2+_p1)


#deffunc df_vshift int _p1

	;	テキストを任意の方向にシフト
	;
	i=texsx*(texsy-1)*4
	a=texsx*4
	if _p1=DIR_UP {
		memcpy tvram,tvram,i,0,a
		memset tvram,0,a,i
		return
	}
	if _p1=DIR_DOWN {
		repeat texsy-1
		memcpy tvram,tvram,a,i,i-a
		i-=a
		loop
		memset tvram,0,a,0
		return
	}
	if _p1=DIR_LEFT {
		memcpy tvram,tvram,i+a-4,0,4
		i=texsx-1
		repeat texsy
			tvram(i)=0:i+=texsx
		loop
		return
	}
	if _p1=DIR_RIGHT {
		memcpy tvram,tvram,i+a-4,4,0
		i=0
		repeat texsy
			tvram(i)=0:i+=texsx
		loop
		return
	}
	return


#deffunc df_celload var _p1, str _p2

	;	画像読み込み
	;		var,"file"
	;
	exist _p2
	if strsize<0 {
		dialog "No File Error ["+_p2+"]"
		return
	}
	;
	celload _p2
	_p1 = stat
	return


#deffunc df_setbgpic int _p1, int _p2

	;	背景画像を指定
	;		BGNo., ウインドウID
	;
	i=_p2
	a=_p1
	if a<0 | a>_dotfw_bgpic_max@ : dialog "Invalid BG#"+a : return

	gsel i
	bgp_id(a)=i
	bgp_gx(a)=0
	bgp_gy(a)=0
	bgp_px(a)=0
	bgp_py(a)=0
	bgp_sx(a)=ginfo_sx
	bgp_sy(a)=ginfo_sy
	gsel 0
	return


#deffunc df_setbgpicfile int _p1, str _p2

	;	背景画像を指定
	;		BGNo., "filename"
	;
	a=_p1
	if a<0 | a>_dotfw_bgpic_max@ : dialog "Invalid BG#"+a : return

	df_celload i, _p2
	gsel i
	bgp_id(a)=i
	bgp_gx(a)=0
	bgp_gy(a)=0
	bgp_px(a)=0
	bgp_py(a)=0
	bgp_sx(a)=ginfo_sx
	bgp_sy(a)=ginfo_sy
	gsel 0
	return


#deffunc df_loadbgmap int _p1, str _p2, int _p3

	;	背景マップ読み込み
	;		BGNo. , "file", option
	;
	a=_p1
	if a<0 | a>_dotfw_bgpic_max@ : dialog "Invalid BG#"+a : return
	gmp_id= _p1+ DOTFW_BGID_BGMAP

	headsize=128
	sdim header,headsize
	bload _p2,header,headsize
	if header!="TMAP" : dialog "Invalid tmap file ["+_p2+"]." : return

	msx = lpeek(header,8)
	msy = lpeek(header,12)
	celsizex = lpeek(header,16)
	celsizey = lpeek(header,20)
	headsize = lpeek(header,32)
	attrmax = lpeek(header,36)/2

	dim labx,8:dim laby,8:a=64
	repeat 4
		labx(cnt)=lpeek(header,a):laby(cnt)=lpeek(header,a+4)
		a+=8
	loop

	sdim celfile,256
	memcpy celfile,header,32,0,96
	exist celfile
	if strsize<0 {
		dialog "No File Error ["+celfile+"]"
		return
	}
	celload celfile
	bufid = stat

	a=_p1
	df_setbgmap a,bufid, msx, msy, celsizex, _p3

	gmp_id= _p1+ DOTFW_BGID_BGMAP

	sdim attr,attrmax*2
	bload _p2,attr,attrmax*2,headsize
	repeat attrmax
		es_bgattr gmp_id,cnt,cnt,wpeek(attr,cnt*2)
		if stat : dialog "ERR"
	loop

	;map=0
	if gmp_id=1 : dup map, gmp_map1
	if gmp_id=2 : dup map, gmp_map2
	if gmp_id=3 : dup map, gmp_map3
	if gmp_id=4 : dup map, gmp_map4

	bload _p2,map,msx*msy*4,headsize+attrmax*2
	_dotfw_curmapx@ = msx;
	_dotfw_curmapy@ = msy;

	return


#deffunc df_bgmaplabel var _p1, var _p2, int _p3

	;	読み込んだマップのラベル値を取得
	;		var1, var2, id
	;
	_p1=labx(_p3)
	_p2=laby(_p3)
	return

#deffunc df_setbgmap int _p1, int _p2, int _p3, int _p4, int _p5, int _p6

	;	背景マップを指定
	;		BGNo., bufid, xsize, ysize, celsize, option
	;
	a=_p1
	if a<0 | a>_dotfw_bgpic_max@ : dialog "Invalid BG#"+a : return
	gmp_size=16
	if _p5>0 : gmp_size=_p5

	gmp_id= _p1+ DOTFW_BGID_BGMAP
	gmp_buf = _p2
	mapsx=sx/gmp_size
	mapsy=sy/gmp_size
	mapvx=_dotfw_sx@/gmp_size
	mapvy=_dotfw_sy@/gmp_size
	if (_dotfw_sx@ \ gmp_size)>0 : mapvx++
	if (_dotfw_sy@ \ gmp_size)>0 : mapvy++
	if _p3>0 : mapsx=_p3
	if _p4>0 : mapsy=_p4
	celdiv gmp_buf, gmp_size, gmp_size
	if gmp_id=1 : dim gmp_map1,mapsx*mapsy : es_bgmap gmp_id, gmp_map1, mapsx, mapsy, mapvx, mapvy, gmp_buf : goto *bgmap_ent
	if gmp_id=2 : dim gmp_map2,mapsx*mapsy : es_bgmap gmp_id, gmp_map2, mapsx, mapsy, mapvx, mapvy, gmp_buf : goto *bgmap_ent
	if gmp_id=3 : dim gmp_map3,mapsx*mapsy : es_bgmap gmp_id, gmp_map3, mapsx, mapsy, mapvx, mapvy, gmp_buf : goto *bgmap_ent
	if gmp_id=4 : dim gmp_map4,mapsx*mapsy : es_bgmap gmp_id, gmp_map4, mapsx, mapsy, mapvx, mapvy, gmp_buf : goto *bgmap_ent
	dialog "Invalid BG#"+a
	return
*bgmap_ent
	;dialog ""+mapvx+"x"+mapvy
	bgp_id(a)=gmp_id | BGID_MAP
	bgp_gx(a)=0
	bgp_gy(a)=0
	bgp_px(a)=0
	bgp_py(a)=0
	bgp_sx(a)=gmp_size*mapsx
	bgp_sy(a)=gmp_size*mapsy
	return


#deffunc df_getbgmap var _p1, int _p2

	;	背景マップ変数を取得
	;		var, BGNo.
	;
	a=_p2
	if a<0 | a>_dotfw_bgpic_max@ | (bgp_id(_p2)&BGID_MAP)=0 : dialog "Invalid BG#"+a : return
	gmp_id= _p2+ DOTFW_BGID_BGMAP
	if gmp_id=1 : dup _p1, gmp_map1 : return
	if gmp_id=2 : dup _p1, gmp_map2 : return
	if gmp_id=3 : dup _p1, gmp_map3 : return
	if gmp_id=4 : dup _p1, gmp_map4 : return
	return


#deffunc df_setbgmapfile int _p1, str _p2

	;	背景マップを指定
	;		BGNo., "MAPfile"
	;
	if a<0 | a>_dotfw_bgpic_max@ : dialog "Invalid BG#"+a : return
	return


#deffunc df_bgpoint int _p1, int _p2, int _p3

	;	背景参照ポイントを指定
	;		BGNo., x, y
	;
	a=_p1
	bgp_gx(a)=_p2<<10
	bgp_gy(a)=_p3<<10
	bgp_px(a)=0
	bgp_py(a)=0
	return


#deffunc df_getbgpoint var _p1, var _p2, int _p3

	;	背景参照ポイントを取得
	;		var1, var2, BGNo.
	;
	a=_p3
	_p1=bgp_gx(a)>>10
	_p2=bgp_gy(a)>>10
	return


#deffunc df_bgsize int _p1, int _p2, int _p3, int _p4

	;	背景最大サイズを指定
	;		BGNo., x, y, padj
	;
	a=_p1
	bgp_sx(a)=_p2
	bgp_sy(a)=_p3
	sp_player_x2 = bgp_sx(a)-_p4
	sp_player_y2 = bgp_sy(a)-_p4
	return


#deffunc df_bgscroll int _p1, int _p2, int _p3

	;	背景自動スクロールを指定
	;		BGNo., dir, speed
	;
	a=_p1
	bgp_gx(a)=0
	bgp_gy(a)=0
	es_cos _p2:x=stat
	es_sin _p2:y=stat
	x=x*_p3/100
	y=y*_p3/100
	bgp_px(a)=-x
	bgp_py(a)=-y
	return


#deffunc df_update0 int _p1

	;	更新処理(1)
	;
	_dotfw_update_flag@=_p1
	if _dotfw_update_flag@&UPDATE_PAUSE {
		es_draw	,,ESDRAW_NOMOVE|ESDRAW_NOANIM	; スプライト描画
	} else {
		es_draw					; スプライト描画
		_dotfw_frame@++
	}
	if (_dotfw_update_flag@&UPDATE_PAUSE)=0 {
		dfi_sprupdate				; 内部スプライト更新
	}
	dfi_sprdraw					; 内部スプライト描画
	;
	gosub *df_bgput2			; 前面マップ
	gosub *player_maptrack
	gosub *df_bgupdate
	;
	gosub dupdate0_lb			; DACTION_SPROVER
	;
	gmode 1
	es_putbg texbg				; テキストVRAMを描画
	;
	gosub dupdate1_lb			; DACTION_TOPMOST
	;
	if _dotfw_vpad@ {
			viewcalc vptype_2d, 1.0, 1.0
			hspvpad_put key@
	}
	;
	return

#deffunc df_update1 int _p1

	;	更新処理(2)
	;
	redraw 1

	if (_dotfw_update_flag@&UPDATE_NOWAIT)=0 {
		await 1000/_dotfw_fps@
	}

	i=_dotfw_contkey@
	if player_button_int>0 : i|=PLAYER_KEY_BUTTON1
	i|=256

	if _dotfw_joystick@ {
		jstick key@,i
	} else {
		stick key@,i
	}
	if _dotfw_vpad@ : hspvpad_key key@

	redraw 0
	viewcalc vptype_2d, zoomx, zoomy
	gosub *df_bgput

	;	ゲームコントロール
	if (_dotfw_update_flag@&UPDATE_NOWAIT)=0 {
		if (_dotfw_update_flag@&UPDATE_NOCONTROL)=0 {
			gosub *player_main
			gosub *enemy_main
		}
	}
	return

#deffunc df_update int _p1

	;	画面更新
	;
	df_update0 _p1
	df_update1 _p1
	return


#deffunc df_control int _p1

	;	ゲーム処理更新
	;
	gosub *player_main
	gosub *enemy_main
	return


#deffunc df_daction label _p1, int _p2

	;	描画のアクション動作ラベルを設定
	;		label, mode
	;
	if _p2=DACTION_SPROVER {
		dupdate0_lb=_p1
	}
	if _p2=DACTION_TOPMOST {
		dupdate1_lb=_p1
	}
	return


*player_maptrack
	;	プレイヤーを画面内に入れる
	;
	if sp_player_bgtrack=0 : return
	if sp_player_map=0 : return
	if sp_player_mode<0 : return
	;
	a = sp_player_map - DOTFW_BGID_BGMAP
	es_getpos sp_player,myx,myy,ESSPSET_CENTER

	if sp_player_bgtrack=2 : goto *player_maptrack_fix

	myx+=sp_player_bgxadj
	myy+=sp_player_bgyadj

	myx-=bgp_gx(a)>>10
	myy-=bgp_gy(a)>>10

	px=1
	x = sp_player_bgx1 - myx
	if x<=0 {
		px=-1
		x = myx - sp_player_bgx2
		if x<=0 : goto *player_maptrack_y
	}
	setease 0,sp_player_bgxrate*1024,ease_cubic_in

	myx=(bgp_sx(a)-_dotfw_sx@)<<10:if myx<0 : myx=0
	i=bgp_gx(a)
	i-=getease(x,sp_player_bgxrate)*px
	if i<0 : i=0
	if i>myx : i=myx
	bgp_gx(a)=i

*player_maptrack_y
	py=1
	y = sp_player_bgy1 - myy
	if y<=0 {
		py=-1
		y = myy - sp_player_bgy2
		if y<=0 : goto *player_maptrack_done
	}
	setease 0,sp_player_bgyrate*1024,ease_cubic_in

	myy=(bgp_sy(a) - _dotfw_sy@)<<10:if myy<0 : myy=0
	i=bgp_gy(a)
	i-=getease(y,sp_player_bgyrate)*py
	if i<0 : i=0
	if i>myy : i=myy
	bgp_gy(a)=i
	goto *player_maptrack_done

*player_maptrack_fix
	myx=(bgp_sx(a)-_dotfw_sx@)<<10:if myx<0 : myx=0
	i=bgp_gx(a)
	i+=(bgp_px(a)-i)/32
	if i<0 : i=0
	if i>myx : i=myx
	bgp_gx(a)=i

	myy=(bgp_sy(a) - _dotfw_sy@)<<10:if myy<0 : myy=0
	i=bgp_gy(a)
	i+=(bgp_py(a)-i)/32
	if i<0 : i=0
	if i>myy : i=myy
	bgp_gy(a)=i
	;dialog ""+(bgp_gx(a)>>10)+","+(bgp_gy(a)>>10)
	goto *player_maptrack_done

*player_maptrack_done
	return


*player_main
	;	プレイヤーコントロールメイン
	;
	_dotfw_player@=0
	if sp_player_mode<0 : return
	;
	_dotfw_cursp@ = sp_player
	_dotfw_player@++

	gosub player_keylb
	if sp_player_mode<0 : return

	i=0
	es_getpos sp_player,myx,myy
	if myy<sp_player_y1 : myy=sp_player_y1 : i++
	if myx<sp_player_x1 : myx=sp_player_x1 : i++
	if myy>sp_player_y2 : myy=sp_player_y2 : i++
	if myx>sp_player_x2 : myx=sp_player_x2 : i++
	if i : es_pos sp_player,myx,myy

	i=key@&PLAYER_KEY_BUTTON1
	player_button_cnt++
	if player_button_int>0 {
		if player_button_cnt<player_button_int : i=0
	}
	if i {
		_dotfw_cursp@ = sp_player
		player_button_cnt=0
		gosub player_btn1lb
	}
	i=key@&PLAYER_KEY_BUTTON2
	if i {
		_dotfw_cursp@ = sp_player
		gosub player_btn2lb
	}
	i=key@&PLAYER_KEY_BUTTON3
	if i {
		_dotfw_cursp@ = sp_player
		gosub player_btn3lb
	}

	;	やられチェック
	es_check res, sp_player, TYPE_ENEMY|TYPE_EMISSLE|TYPE_EXTENEMY
	if res>=0 {
		_dotfw_cursp@ = res
		gosub player_misslb
		if sp_player_mode<0 : return
	}
	;	アイテムチェック
	es_check res, sp_player, TYPE_ITEM
	if res>=0 {
		_dotfw_cursp@ = res
		gosub player_itemlb
	}

	_dotfw_cursp@ = sp_player
	gosub player_actlb
	return

*player_act_shot
	;	ボタンを押した時の処理(デフォルト)
	df_getplayer
	if _dotfw_dir@ {
		df_addpmis _dotfw_myx@,_dotfw_myy@, DIR_UP, 400, CHR_MISSILE			; ミサイル発射
	} else {
		df_addpmis _dotfw_myx@,_dotfw_myy@, DIR_RIGHT, 400, CHR_MISSILE2		; ミサイル発射
	}
	�0����