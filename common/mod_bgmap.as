
#module "bgmap"

;
;	簡易マップデータ管理モジュール
;	(mapeditツールで作成したマップデータを読み込んで使用できます)
;

#deffunc mapinit int _p1, int _p2

	;	マップ初期化
	;	mapinit vx,vy
	;	(マップを初期化します)
	;	vx=表示Xサイズ、vy=表示Yサイズ
	;
	cx=64:cy=64	; パーツXYサイズ
	mapsx=32	; マップ全体Xサイズ
	mapsy=32	; マップ全体Yサイズ
	celname="mapbg.bmp"			; パーツ画像ファイル
	csrc=ginfo_newid			; パーツのあるウィンドウID
	buffer csrc
	mapvx=_p1:mapvy=_p2			; 表示XYチップ数
	vx=mapvx*cx:vy=mapvy*cy			; 画面内のマップ表示サイズ
	return


#deffunc mapload str _p1
	;	マップ読み込み
	;	(ファイルからマップを読み込みます)
	;	mapload "filename"
	;	(*.map)のファイルを読み込みます。
	;	付加情報(*.txt)がある場合は、それも読み込みます。
	;
	fn=_p1
	minf=getpath(fn,1)+".txt"
	exist minf
	if strsize>0 : gosub *load_minf
	dim map,mapsx,mapsy
	mapx=0:mapy=0				; マップ表示位置
	bload fn,map
	return


*load_minf
	;	付属情報ファイル(txt)をロード
	;
	notesel buf
	noteload minf
	i=0
	noteget s1,i:i++
	if s1!="$mapedit data" : dialog "付属情報が無効です" : return

	repeat
		if i>=notemax : break
		noteget s1,i:i++
		noteget s2,i
		if s1="%mapsx" : mapsx = 0+s2 : i++
		if s1="%mapsy" : mapsy = 0+s2 : i++
		if s1="%celname" : celname = s2 : i++
		if s1="%cx" : cx = 0+s2 : i++
		if s1="%cy" : cy = 0+s2 : i++
	loop
	return


#deffunc mapsave str _p1
	;	マップ保存
	;	(ファイルにマップを保存します)
	;	mapsave "filename"
	;	(*.map)のファイルを保存します。
	;	付加情報(*.txt)も同時に保存します。
	;
	fn=_p1
	bsave fn,map
	minf=getpath(fn,1)+".txt"
	;	付属情報ファイル(txt)を保存
	buf="$mapedit data\n"
	buf+="%mapsx\n"+mapsx+"\n"
	buf+="%mapsy\n"+mapsy+"\n"
	buf+="%celname\n"+celname+"\n"
	buf+="%cx\n"+cx+"\n"
	buf+="%cy\n"+cy+"\n"
	notesel buf
	notesave minf
	return


#deffunc mapdraw int _p1, int _p2

	;	マップ描画
	;	(マップを描画します)
	;	mapdraw x,y
	;	(x,y=描画開始位置)
	;
	gmode 0
	xx=_p1:yy=_p2			; マップを描き始める位置
	repeat mapvy
	y=cnt
	pos xx,y*cy+yy
	repeat mapvx
		celput csrc,map(mapx+cnt,mapy+y)
	loop
	loop
	return




#global

