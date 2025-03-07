#ifndef __MOD_TAMAKEISD
#define __MOD_TAMAKEISD
;
;
#module "_tamakeisd"

#define global SDCHR_BUFID_BASE (3)
#define global SDCHR_BUFID_UPICON (11)
#define global SDCHR_BUFID_SANTA (12)

	;	珠音ちゃん・プロ生ちゃん SD 上半身コラボ素材
	;	HSP3用スクリプトサンプル( https://hsp.tv/ )
	;	(実際の画像素材データはpngフォルダに入っています)
	;
	;	暮井 慧（プロ生ちゃん）(C) Pronama LLC
	;	珠音ちゃん (C) オガワコウサク(チームグリグリ)/HSPTV!/onion software
	;	※このスクリプト及びサンプルデータは、ガイドラインのもと自由にお使い頂けます
	;	https://github.com/onitama/tamakei_ragho
	;
	fflag=0
	santaflag=0
	upicon=0
	dim chrflip,16
	dim chrbuf,16

#deffunc sdchr_load int _p1, int _p2, int _p3

	;	SDキャラの準備
	;	(画像バッファ3〜10を使用します、SDCHR_BUFID_BASEで定義)
	;	sdchr_load p1, p2, p3
	;
	;	p1(0) : キャラID(0=慧/1=珠音/2=フィネス/3=千由莉)
	;	p2(0) : 向き(0=右/1=左)
	;	p3(0) : サイズ%(0=100%)
	;
	fsx=640:fsy=1024		; 素材サイズ
	fox=512:foy=256			; 追加表情サイズ
	fpx=384:fpy=128:fpx2=fpx/2	; アニメパーツサイズ
	fofs_eye=fpy*4
	fofs_mouth=fpy*7
	frate=_p3
	if _p3<=0 : frate=100
	;
	chrid=_p1
	if chrid<0|chrid>3 : return

	bname="ketafiti"
	name=strmid(bname,chrid*2,2)
	if _p2=0 {
		name+="r"
		chrflip(chrid)=1
	} else {
		name+="l"
		chrflip(chrid)=0
	}
	;
	fid_up=SDCHR_BUFID_BASE+chrid*2
	fid_opt=SDCHR_BUFID_BASE+1+chrid*2
	chrbuf(chrid)=fid_up
	fname="up_"+name+".png"
	celload fname,fid_up
	fname="face_"+name+".png"
	celload fname,fid_opt
	celdiv fid_opt, fox,foy
	;
	fflag=1
	;
	if santaflag=0 {
		; サンタ帽を読み込み
		celload "santa_l.png",SDCHR_BUFID_SANTA
		celload "santa_r.png",SDCHR_BUFID_SANTA+1
		santaflag=1
	}
	return

#deffunc sdchr_put int _p1, int _p2, int _p3, int _p4

	;	SDキャラの表示(パーツ変化あり)
	;	(あらかじめsdchr_loadでキャラの準備をしておく必要があります)
	;	(posで指定されたカレントポジションから表示します)
	;	sdchr_put p1,p2,p3,p4
	;
	;	p1(0) : キャラID(0=慧/1=珠音/2=フィネス/3=千由莉)
	;	p2(0) : 眉(0=普通/1=悲しみ/2=怒り/3=驚き)
	;	p3(0) : 目(0=普通/1=中間/2=閉じる)
	;	p4(0) : 口(0=閉じる/1=開く)
	;
	if fflag=0 : return

	chrid=_p1
	gosub *sdchr_prepare

	xx=ginfo_cx:yy=ginfo_cy
	gmode 2,fsx,fsy,255
	pos xx,yy
	sx=fsx*frate/100:sy=fsy*frate/100
	gzoom sx,sy,fid_up,0,0,,,1

	gmode 2,fpx,fpy,255
	sx=fpx*frate/100:sy=fpy*frate/100
	x=axdata(2)*frate/100:y=axdata(3)*frate/100
	pos xx+x,yy+y:gzoom sx,sy,fid_up,fsx,_p2*fpy,,,1
	x=axdata(4)*frate/100:y=axdata(5)*frate/100
	pos xx+x,yy+y:gzoom sx,sy,fid_up,fsx,_p3*fpy+fofs_eye,,,1
	sx=fpx2*frate/100
	x=axdata(6)*frate/100:y=axdata(7)*frate/100
	pos xx+x,yy+y:gzoom sx,sy,fid_up,fsx+_p4*fpx2,fofs_mouth,fpx2,fpy,1

	pos xx,yy
	return


#deffunc sdchr_putex int _p1, int _p2

	;	SDキャラの表示(表情あり)
	;	(あらかじめsdchr_loadでキャラの準備をしておく必要があります)
	;	(posで指定されたカレントポジションから表示します)
	;	sdchr_putex p1
	;
	;	p1(0) : キャラID(0=慧/1=珠音/2=フィネス/3=千由莉)
	;	p2(0) : 表情(1=泣き/2=どや/3=丸目/4=にっこり/5=怒り/6=嬉しい/7=じと目)
	;
	if fflag=0 : return

	chrid=_p1
	gosub *sdchr_prepare

	xx=ginfo_cx:yy=ginfo_cy
	gmode 2,fsx,fsy,255
	pos xx,yy
	sx=fsx*frate/100:sy=fsy*frate/100
	gzoom sx,sy,fid_up,0,0,,,1

	sx=fox*frate/100:sy=foy*frate/100
	x=axdata(0)*frate/100:y=axdata(1)*frate/100
	pos xx+x,yy+y:gzoom sx,sy,fid_opt,(_p2&1)*fox,(_p2/2)*foy,fox,foy,1
	pos xx,yy
	return


*sdchr_prepare
	fid_up=0
	if chrid<length(chrbuf) : fid_up=chrbuf(chrid)
	if fid_up=0 : dialog "Character not loaded.(ID"+chrid+")" : end	; ロードしていない
	fid_opt=fid_up+1
	chrss=chrid*2+chrflip(chrid)
	if chrss=0 : axdata=64,386,90,331,90,419,183,536	;kel
	if chrss=1 : axdata=64,386,169,334,170,421,261,540	;ker
	if chrss=2 : axdata=64,384,116,332,115,427,207,544	;tal
	if chrss=3 : axdata=64,384,129,332,132,432,222,544	;tar
	if chrss=4 : axdata=64,384,83,333,88,421,176,532	;fil
	if chrss=5 : axdata=64,384,198,326,196,425,292,533	;fir
	if chrss=6 : axdata=64,384,98,363,101,448,197,555	;til
	if chrss=7 : axdata=64,384,173,363,160,450,262,555	;tir
	return


#deffunc sdchr_putsanta int _p1, int _p2

	;	直前に描画したSDキャラの上にサンタ帽を表示
	;	(あらかじめキャラの表示をしておく必要があります)
	;	(posで指定されたカレントポジションを基準に表示します)
	;	sdchr_putsanta p1, p2
	;
	;	p1(0) : X位置調整
	;	p2(0) : Y位置調整
	;
	if fflag=0 : return
	x=ginfo_cx:y=ginfo_cy
	if chrid<0 : return
	i=SDCHR_BUFID_SANTA+chrflip(chrid)
	yy=70:px=20
	if chrid=2 : yy-=20 : px=0
	if chrflip(chrid) {
		if chrid=1 : px=-10
		xx=-60+px
	} else {
		xx=346-px
	}

	gmode 2,384,300,255
	sx=384*frate/100:sy=300*frate/100
	xx+=_p1:yy+=_p2
	xx=xx*frate/100:yy=yy*frate/100
	pos x+xx,y+yy
	gzoom sx,sy,i,0,0,,,1
	pos x,y
	return


#deffunc sdchr_loadicon int _p1

	;	アイコン(漫符)の準備
	;	(画像バッファ11を使用します、SDCHR_BUFID_UPICONで定義)
	;	sdchr_loadicon p1
	;
	;	p1(0) : オプション値(未使用)
	;
	upiconsx=256
	upiconsy=256
	celload "up_icon.png",SDCHR_BUFID_UPICON
	celdiv SDCHR_BUFID_UPICON,upiconsx,upiconsy
	upicon=1
	return


#deffunc sdchr_puticon int _p1, int _p2, int _p3

	;	直前に描画したSDキャラの上にアイコン(漫符)を表示
	;	(あらかじめsdchr_iconloadで準備をしておく必要があります)
	;	sdchr_puticon p1
	;
	;	p1(0) : アイコンID(1〜7)
	;	p2(0) : X位置調整
	;	p3(0) : Y位置調整
	;
	x=ginfo_cx:y=ginfo_cy
	if upicon=0|chrid<0 : dialog "Icon not loaded." : end
	i=_p1
	if i<=0 : return
	gosub *sdchr_icons
	xx+=_p2:yy+=_p3

	gmode 2,upiconsx,upiconsy,255
	sx=upiconsx*frate/100:sy=upiconsy*frate/100
	xx=xx*frate/100:yy=yy*frate/100
	pos x+xx,y+yy
	gzoom sx,sy,SDCHR_BUFID_UPICON,bx,by,,,1
	pos x,y
	return

*sdchr_icons
	if i=1 : xx=10:yy=80	;kira
	if i=2 : xx=-50:yy=100	;yo1
	if i=3 : xx=-50:yy=100	;yo2
	if i=4 : xx=-50:yy=100	;heart
	if i=5 : xx=-50:yy=100	;moya
	if i=6 : xx=30:yy=30	;bikk
	if i=7 : xx=0:yy=100	;ikari
	a=i&1
	if chrflip(chrid) {
		a=3-a
		xx=fsx-xx-upiconsx
	}
	bx=a*256:by=(i>>1)*256
	return


#global
#endif

