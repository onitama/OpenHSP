;----------------------------------------------------------------
; バーチャルパッドモジュール / onitama 2023
;  使用に関する制限はありません。ご自由にお使いください。
;----------------------------------------------------------------

#module "_vpad"

#deffunc hspvpad_init int _p1, int _p2, int _p3, int _p4

	;	バーチャルパッドの初期化
	;	hspvpad_init p1,p2
	;	p1 : パッド画像を格納するバッファID
	;	p2 : パッド配置(1=左PAD/2=上PAD/4=移動のみ)
	;	p3 : X方向オフセット
	;	p4 : Y方向オフセット
	;
	vpbuf=_p1
	if vpbuf<=0 : vpbuf=ginfo(25)
	celload "vpad.png",vpbuf
	celdiv vpbuf,64,64
	vppos=_p2
	sx=ginfo_sx:sy=ginfo_sy
	vpadx=32:if vppos&1 : vpadx=sx-223
	i=22:if vppos&2 : i=4
	vpady=sy*i/30
	vpadx += _p3 : vpady += _p4
	;
	vpkey=0
	vpmax=0
	dim vp_key,16
	dim vp_id,16
	dim vp_x,16
	dim vp_y,16
	;
	hspvpad_addkey vpadx,vpady,1,7
	hspvpad_addkey vpadx+128,vpady,4,5
	hspvpad_addkey vpadx+64,vpady-64,2,4
	hspvpad_addkey vpadx+64,vpady+64,8,6
	;
	vpadx=sx-223:if vppos&1 : vpadx=32
	;
	if (vppos&4)=0 {
		hspvpad_addkey vpadx,vpady,16,3
		hspvpad_addkey vpadx+128,vpady,2048,0
		hspvpad_addkey vpadx+64,vpady-64,4096,2
		hspvpad_addkey vpadx+64,vpady+64,8192,1
	}
	;
	return


#deffunc hspvpad_addkey int _p1, int _p2, int _p3, int _p4

	;	ボタンの配置(内部用)
	vp_x(vpmax)=_p1
	vp_y(vpmax)=_p2
	vp_key(vpmax)=_p3
	vp_id(vpmax)=_p4
	vpmax++
	return	


#deffunc hspvpad_set int _p1, int _p2, int _p3

	;	バーチャルパッドのボタン位置調整
	;	hspvpad_set p1,p2,p3
	;	p1 : ボタンID(0=上/1=下/2=左/3=右/4=チェック/5=〇/6=△/7=×)
	;	p2 : X位置(絶対座標) (マイナス値の場合は表示なし)
	;	p3 : Y位置(絶対座標)
	;
	vp_x(_p1)=_p2
	vp_y(_p1)=_p3
	return


#deffunc hspvpad_stick int _p1, int _p2

	;	バーチャルパッドのキー置き換え
	;	hspvpad_stick p1,p2
	;	p1 : ボタンID(0=上/1=下/2=左/3=右/4=チェック/5=〇/6=△/7=×)
	;	p2 : キー値(stick命令のキー情報値)
	;
	vp_key(_p1)=_p2
	return


#deffunc hspvpad_key var _p1, int _p2

	;	バーチャルパッドのキー処理
	;	hspvpad_key var
	;	var = stickで値を取得した変数名
	;
	if _p1&$100 {

	x=mousex:y=mousey
	    repeat vpmax
		x1=vp_x(cnt):x2=x1+64
		if x1>=0 {
			if x>=x1&&x<x2 {
				y1=vp_y(cnt):y2=y1+64
				if y>=y1&&y<y2 {
					_p1|=vp_key(cnt)
				}
			}
		}
	    loop
	}
	vpkey=_p1
	return

#deffunc hspvpad_put int _p1, int _p2

	;	バーチャルパッドの表示処理
	;	hspvpad_put p1
	;	p1 : 強制ONを行うkey値
	;
	i=vpkey|_p1
	gmode 3,,,128
	repeat vpmax
		x=vp_x(cnt)
		if x>=0 {
			pos x,vp_y(cnt)
			j=vp_id(cnt):if i&vp_key(cnt) : j+=8
			celput vpbuf,j
		}
	loop
	return

#global

