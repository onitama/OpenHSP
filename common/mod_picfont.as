#ifndef __MOD_PICFONT
#define __MOD_PICFONT

;0〜9、A〜Zなどの基本的な文字フォントを画像で持ち、
;それを表示するためのモジュールです。
;自作の文字フォントを利用する際に使用することができます。
;既存のフォントを画像に変換するサンプル(mkfont.hsp)も用意しています。

#module mod_picfont
#deffunc picfont int _p1, int _p2, int _p3, int _p4, int _p5

	;	picfont id, cx, cy, mode, offset
	;	(画像を使用したフォント表示準備)
	;	id : フォント画像のあるバッファID
	;	cx,cy : フォント１文字あたりのXYサイズ
	;	mode : 表示時のモード(gmodeのモード値)
	;	(文字は半角英数字のみ)
	;
	sx=_p2:if sx=0 : sx=16
	sy=_p3:if sy=0 : sy=16
	mode=_p4:id=_p1
	ofsx=_p5
	return

#deffunc picfprt str _p1

	;	picfprt "message"
	;	(画像を使用したフォント表示を行ないます)
	;	"message" : 表示するメッセージ
	;	表示座標は、posで指定した位置から
	;
	x=ginfo_cx:xs=x
	y=ginfo_cy
	i=0:gmode mode,sx,sy
	st=_p1

	repeat
	a1=peek(st,i):i++:if a1=0 : break
	if a1=13 {
		a1=peek(st,i)
		if a1=10 : i++
		x=xs : y+=sy : continue	; 改行
	} else {
		by=a1/16*sy:bx=a1\16*sx
		pos x,y:gcopy id,bx,by
	}
	x+=sx+ofsx
	loop

	pos xs,y+sy
	return

#global
#endif

