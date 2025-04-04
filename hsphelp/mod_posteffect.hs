;
;	HSP help manager用 HELPソースファイル
;	(先頭が「;」の行はコメントとして処理されます)
;
%ver
3.7
%date
2021/05/31
%group
入出力制御命令

%type
ユーザー拡張命令

%note
hgimg4.as(hgimg4dx.as)およびmod_posteffect.asをインクルードすること。

%author
onitama
%dll
mod_posteffect
%url
https://hsp.tv/make/hgimg4.html
%port
Win
%portinfo
Windows/Linux/android/iOS等HGIMG4をサポートするシステム上で動作します。


%index
post_reset
ポストエフェクト初期化
%prm
option
option(0) : 初期化オプション
%inst
ポストエフェクトの初期化を行ないます。ポストエフェクトの初期化は、画面初期化時に１回だけ行います。
mod_posteffectモジュールによるポストエフェクトは、必ずHGIMG4ランタイム上で実行する必要があります。
optionで設定した内容によって初期化の内容が変わります。
^p
	オプション値  内容
	-------------------------------------------------------------------------
	POST_RESET_OWNDRAW         ポストエフェクトの描画をユーザー自身で行う
	POST_RESET_POSTLAYER       GUIレイヤーをポストエフェクトに含める
^p
通常は、初期化オプションは省略しておいて問題ありません。
ポストエフェクトの初期化が完了すると、ウインドウID1をポストエフェクト用の描画ウインドウとしてシステムが確保します。(また、エフェクト用のバッファとしてID64以降が使用されます。)
通常は3Dで描画される画面(gpdraw命令で描画される画面)がポストエフェクトの対象となり、2D直接描画(mes命令など)は対象となりません。オプションにPOST_RESET_POSTLAYERを指定した場合は、2D直接描画も対象となります。
ポストエフェクトを使用する際には、redraw 0〜redraw 1という画面更新の代わりに、post_drawstart〜post_drawendを使用して更新を行ってください。
post_drawendが実行された際に、自動的にそれまで描画された画面に対してポストエフェクトを反映させます。
オプションにPOST_RESET_OWNDRAWを指定した場合は、post_drawend実行時にポストエフェクトの描画を行いません。この場合は、ユーザー自身でウインドウID0を更新してポストエフェクトを描画を行う必要があります。
%href
post_select
post_drawstart
post_drawend


%index
post_select
ポストエフェクトIDを指定
%prm
id
id(0) : ポストエフェクトID
%inst
ポストエフェクトIDを指定して、画面の効果を有効にします。 
必ずpost_reset命令によりポストエフェクトの初期化を行なっておく必要があります。
ポストエフェクトIDは、以下のような値を指定することができます。
^p
	ID               内容                オプション
	-------------------------------------------------------------------------
	POSTID_NONE      なし
	POSTID_SEPIA     セピア
	POSTID_GRAY      グレースケール
	POSTID_BLUR      ぼかし              level(4.0)
	POSTID_BLUR2     ぼかし(高品質)      level(6.0)
	POSTID_MOSAIC    モザイク
	POSTID_CONTRAST  コントラスト        level(2.0),bright(-0.2)
	POSTID_SOBEL     輪郭抽出
	POSTID_CRT       ブラウン管          湾曲率X(0.5),湾曲率Y(0.4)
	POSTID_OLDFILM   古いフィルム
	POSTID_GLOW      グロー              rate(64),bright(0.3)
	POSTID_GLOW2     グロー(高品質/加算) rate(128),cutoff(0.5),level(10)
	POSTID_GLOW3     グロー(高品質)      rate(128),cutoff(0.5),level(10)
	POSTID_CUTOFF    カットオフ          ratio(0.8)
^p
ポストエフェクトIDを有効にした後は、post_setprm命令により効果ごとのオプション値を設定することができます。(上表のオプション項目にあるパラメーターを指定できます。カッコ内はデフォルト値です。)
%href
post_reset
post_setprm


%index
post_setprm
ポストエフェクトオプションを指定
%prm
p1,p2,p3
p1(0.0) : ポストエフェクトパラメーター1(実数)
p2(0.0) : ポストエフェクトパラメーター2(実数)
p3(0)   : ポストエフェクトパラメーター3(整数)
%inst
post_select命令で設定されたポストエフェクトに付随するオプション値を設定します。
オプション値は、設定されたポストエフェクトの効果を調整することができるもので、ポストエフェクトの種類により設定可能なオプションが異なります。
%href
post_select


%index
post_drawstart
ポストエフェクト用の画面描画開始
%inst
ポストエフェクトモジュール使用時に画面描画開始のタイミングを指定します。
必ずpost_reset命令によりポストエフェクトの初期化を行なっておく必要があります。
ポストエフェクトを使用する際には、redraw 0〜redraw 1という画面更新の代わりに、post_drawstart〜post_drawendを使用して更新を行ってください。
%href
post_drawend


%index
post_drawend
ポストエフェクト用の画面描画終了
%inst
ポストエフェクトモジュール使用時に画面描画終了のタイミングを指定します。
必ずpost_reset命令によりポストエフェクトの初期化を行なっておく必要があります。
ポストエフェクトを使用する際には、redraw 0〜redraw 1という画面更新の代わりに、post_drawstart〜post_drawendを使用して更新を行ってください。
%href
post_drawstart


%index
post_getname
ポストエフェクト名を取得
%prm
var
var : 結果が代入される変数名
%inst
現在有効になっているポストエフェクトの名称をvarで指定された変数に代入します。
ポストエフェクト名は文字列として代入されます。
%href
post_getmaxid
post_select


%index
post_getmaxid
ポストエフェクトID最大値を取得
%prm
var
var : 結果が代入される変数名
%inst
varで指定された変数にポストエフェクトIDとして使用できる最大値を代入します。
ポストエフェクトIDは、post_selectで指定するポストエフェクトの種別になります。
%href
post_getname
post_select


