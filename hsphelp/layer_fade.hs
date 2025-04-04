;
;	HSP help manager用 HELPソースファイル
;	(先頭が「;」の行はコメントとして処理されます)
;
%ver
3.7
%date
2022/06/01
%group
入出力制御命令

%type
ユーザー拡張命令

%note
layer_fade.asをインクルードすること。

%author
onitama
%dll
layer_fade
%url
https://hsp.tv/
%port
Win
%portinfo
HSP3標準ランタイム/HSP3Dish/HGIMG4上で動作します。


%index
layer_fade
フェードインアウトモジュールの初期化
%inst
フェードインアウトモジュール(layer_fade)の初期化を行います。
これ以降は、fadein/fadeout命令によるフェードイン・アウトが利用できます。
フェードインアウトモジュールは、レイヤーオブジェクトの1つとして動作します。
(stop命令やredraw命令を使用していないスクリプトでは正しく動作しませんのでご注意ください)
また、cls命令などで画面初期化が行われた際には、再度初期化を行う必要があります。
フェードインアウトモジュールは、HSP3標準ランタイム及びHSP3Dish/HGIMG4上で動作します。
ただし、HSP3標準ランタイムでは仕様により、ボタンなどの配置オブジェクトに対してフェードは適用されませんのでご注意ください。
%sample
#include "layer_fade.as"	; フェードモジュールを使用

	screen 0,640,480
	layer_fade		; フェードモジュールを初期化
	fadecolor $000000	; フェード色を設定
	fadespeed 8		; フェード速度を設定
	fadein			; フェードイン
*main
	redraw 0
	rgbcolor $ffffff
	boxf
	color 255,0,0
	font msgothic,60,1
	pos 100,180:mes "フェードします",mesopt_outline
	redraw 1
	await 1000/30

	stick key
	if key&32 : fadeout	; [Enter]キーでフェードアウト

	goto *main
%href
fadein
fadeout
fadespeed
fadecolor



%index
fadein
画面のフェードインを行う
%inst
フェードインアウトモジュールによる画面のフェードインを行います。
必ず、layer_fade命令による初期化を行っておく必要があります。
%href
layer_fade


%index
fadeout
画面のフェードアウトを行う
%inst
フェードインアウトモジュールによる画面のフェードアウトを行います。
必ず、layer_fade命令による初期化を行っておく必要があります。
%href
layer_fade


%index
fadespeed
フェードインアウトの速度を設定する
%prm
p1
p1(0) : フェード速度(1〜128)
%inst
フェードインアウトモジュールによる画面のフェード速度を設定します。
フェード速度として設定した値が、1フレームあたりの変化量となります。
値は、1〜128程度を指定します。値が少ないほどゆっくりとフェードします。値が大きくなるほどフェードが完了するまでの時間は早くなります。
必ず、layer_fade命令による初期化を行っておく必要があります。
%href
layer_fade


%index
fadecolor
フェードインアウトの色を設定する
%prm
color
color(0) : フェード色コード(RGB)
%inst
フェードインアウトモジュールによる画面のフェード色を設定します。
色コードは、rgbcolor命令と同様のRGB形式になります。
必ず、layer_fade命令による初期化を行っておく必要があります。
%href
layer_fade


