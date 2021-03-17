%type
システム定義マクロ
%ver
3.6
%date
2019/04/09
%note
このマクロはhspdef.as内で定義されています。
%group
標準定義マクロ
%author
onitama
%port
Win
Mac
Cli


%index
gmode_sub
色減算合成コピーモード

%inst
gmodeの第1引数に指定することで、画面コピーモードを色減算合成コピーモードに設定できます。

%sample
	screen 1 : picload dir_exe + "/sample/demo/logop.bmp"
	img_width = ginfo_winx
	img_height = ginfo_winy
	gsel 0
	color       : boxf img_width / 5, 0, img_width  * 2 / 5, ginfo_winy
	color 255   : boxf img_width * 2 / 5, 0, img_width * 3 / 5, ginfo_winy
	color ,255  : boxf img_width * 3 / 5, 0, img_width * 4 / 5, ginfo_winy
	color ,,255 : boxf img_width * 4 / 5, 0, img_width, ginfo_winy
	gmode gmode_sub, img_width, img_height, 256
	gcopy 1, 0, 0
	stop

%href
gmode
gmode_rgb0
gmode_mem
gmode_alpha
gmode_add
gmode_gdi
gmode_rgb0alpha
gmode_pixela
%index
gmode_add
色加算合成コピーモード

%inst
gmodeの第1引数に指定することで、画面コピーモードを色加算合成コピーモードに設定できます。

%sample
	screen 1 : picload dir_exe + "/sample/demo/logop.bmp"
	img_width = ginfo_winx
	img_height = ginfo_winy
	gsel 0
	color       : boxf img_width / 5, 0, img_width  * 2 / 5, ginfo_winy
	color 255   : boxf img_width * 2 / 5, 0, img_width * 3 / 5, ginfo_winy
	color ,255  : boxf img_width * 3 / 5, 0, img_width * 4 / 5, ginfo_winy
	color ,,255 : boxf img_width * 4 / 5, 0, img_width, ginfo_winy
	gmode gmode_add, img_width, img_height, 256
	gcopy 1, 0, 0
	stop

%href
gmode
gmode_rgb0
gmode_mem
gmode_alpha
gmode_sub
gmode_gdi
gmode_rgb0alpha
gmode_pixela
%index
gmode_gdi
通常のコピーモード

%inst
gmodeの第1引数に指定することで、画面コピーモードを通常のコピーモードに設定できます。
省略しても同じ結果が得られますので、省略しても構いません。

%sample
	buffer 1 : picload dir_exe + "/sample/demo/logop.bmp"
	img_width = ginfo_winx
	img_height = ginfo_winy
	gsel 0
	gmode gmode_gdi
	gcopy 1, 0, 0, img_width, img_height
	stop

%href
gmode
gmode_rgb0
gmode_mem
gmode_alpha
gmode_add
gmode_sub
gmode_rgb0alpha
gmode_pixela
%index
gmode_rgb0
透明色付きコピーモード

%inst
gmodeの第1引数に指定することで、画面コピーモードを透明色付きコピーモードに設定できます。

%sample
	screen 1 : picload dir_exe + "/sample/demo/logop.bmp"
	img_width = ginfo_winx
	img_height = ginfo_winy
	gsel 0
	gmode gmode_rgb0, img_width, img_height
	gcopy 1, 0, 0
	stop

%href
gmode
gmode_sub
gmode_mem
gmode_alpha
gmode_add
gmode_gdi
gmode_rgb0alpha
gmode_pixela
%index
gmode_mem
メモリ間コピーモード

%inst
gmodeの第1引数に指定することで、画面コピーモードをメモリ間コピーモードに設定できます。

%sample
	screen 1 : picload dir_exe + "/sample/demo/logop.bmp"
	img_width = ginfo_winx
	img_height = ginfo_winy
	gsel 0
	gmode gmode_mem, img_width, img_height
	gcopy 1, 0, 0
	stop

%href
gmode
gmode_rgb0
gmode_sub
gmode_alpha
gmode_add
gmode_gdi
gmode_rgb0alpha
gmode_pixela
%index
gmode_alpha
半透明合成コピーモード

%inst
gmodeの第1引数に指定することで、画面コピーモードを半透明合成コピーモードに設定できます。

%sample
	screen 1 : picload dir_exe + "/sample/demo/logop.bmp"
	img_width = ginfo_winx
	img_height = ginfo_winy
	gsel 0
	color       : boxf img_width / 5, 0, img_width  * 2 / 5, ginfo_winy
	color 255   : boxf img_width * 2 / 5, 0, img_width * 3 / 5, ginfo_winy
	color ,255  : boxf img_width * 3 / 5, 0, img_width * 4 / 5, ginfo_winy
	color ,,255 : boxf img_width * 4 / 5, 0, img_width, ginfo_winy
	gmode gmode_alpha, img_width, img_height, 128
	gcopy 1, 0, 0
	stop

%href
gmode
gmode_rgb0
gmode_mem
gmode_sub
gmode_add
gmode_gdi
gmode_rgb0alpha
gmode_pixela
%index
gmode_rgb0alpha
透明色付き半透明合成コピーモード

%inst
gmodeの第1引数に指定することで、画面コピーモードを透明色付き半透明合成コピーモードに設定できます。

%sample
	screen 1 : picload dir_exe + "/sample/demo/logop.bmp"
	img_width = ginfo_winx
	img_height = ginfo_winy
	gsel 0
	color   0,   0,   0 : boxf img_width / 5, 0, img_width  * 2 / 5, ginfo_winy
	color 255,   0,   0 : boxf img_width * 2 / 5, 0, img_width * 3 / 5, ginfo_winy
	color   0, 255,   0 : boxf img_width * 3 / 5, 0, img_width * 4 / 5, ginfo_winy
	color   0,   0, 255 : boxf img_width * 4 / 5, 0, img_width, ginfo_winy
	gmode gmode_rgb0alpha, img_width, img_height, 128
	color 0, 0, 0
	gcopy 1, 0, 0
	stop

%href
gmode
gmode_rgb0
gmode_mem
gmode_alpha
gmode_add
gmode_gdi
gmode_sub
gmode_pixela
%index
gmode_pixela
ピクセルアルファブレンドコピーモード

%inst
gmodeの第1引数に指定することで、画面コピーモードをピクセルアルファブレンドコピーモードに設定できます。

%sample
	screen 1 : picload dir_exe + "/sample/demo/logop.bmp"
	img_width = ginfo_winx
	img_height = ginfo_winy
	screen 1, img_width * 2, img_height : picload dir_exe + "/sample/demo/logop.bmp", 1
	repeat img_width
		hsvcolor cnt * 192 / ( img_width  ), 255, 255
		line img_width + cnt, img_height, img_width + cnt, 0
	loop
	gsel 0
	gmode gmode_pixela, img_width, img_height, 128
	gcopy 1, 0, 0
	stop

%href
gmode
gmode_rgb0
gmode_mem
gmode_alpha
gmode_add
gmode_gdi
gmode_rgb0alpha
gmode_sub
%index
objinfo_mode
モードおよびオプションデータを取得

%prm
(p1)
p1=0〜 : ウィンドウオブジェクトID

%inst
指定したウィンドウオブジェクトのモードおよびオプションデータを返します。

%sample
	button goto "sample", *dummy
	info = objinfo_mode( stat )
	mes "mode : " + ( info & 0xffff )
	mes "option : " + ( info >> 16 & 0xffff )

*dummy
	stop

%href
objinfo
objinfo_hwnd
objinfo_bmscr
%index
objinfo_bmscr
オブジェクトが配置されているBMSCR構造体のポインタを取得

%prm
(p1)
p1=0〜 : ウィンドウオブジェクトID

%inst
指定したウィンドウオブジェクトが配置されているBMSCR構造体のポインタを返します。

%sample
	button goto "sample", *dummy
	p_bmscr = objinfo_bmscr( stat )
	mes "objectが配置されているBMSCR構造体のポインタ : " + p_bmscr
	mref bmscr, 67
	mes "mrefで得られる値（" + varptr( bmscr ) + "）と同等"

*dummy
	stop

%href
objinfo
objinfo_mode
objinfo_hwnd
%index
objinfo_hwnd
ウィンドウオブジェクトのハンドルを取得

%prm
(p1)
p1=0〜 : ウィンドウオブジェクトID

%inst
指定したウィンドウオブジェクトのハンドルを返します。

%sample
	button goto "sample", *dummy
	obj_hwnd = objinfo_hwnd( stat )
	mes "ウィンドウオブジェクトのハンドル : " + obj_hwnd

*dummy
	stop

%href
objinfo
objinfo_mode
objinfo_bmscr
%index
screen_normal
通常のウィンドウを作成

%inst
screen命令の第4引数に指定することで、通常のウィンドウを作成できます。
省略しても同じ結果が得られますので、省略しても構いません。

%sample
// ウィンドウID0の通常のウィンドウを作成
	screen 0, 640, 480, screen_normal

// 省略しても同じ結果が得られる
	screen 1, 320, 240
	stop

%href
screen
screen_palette
screen_hide
screen_fixedsize
screen_tool
screen_frame

%index
screen_palette
パレットモードのウィンドウを作成

%inst
screen命令の第4引数に指定することで、パレットモードのウィンドウを作成できます。
他のマクロと組み合わせて指定する場合は、その和または論理和を指定してください。

%sample
// ウィンドウID0のウィンドウをパレットモードで作成
	screen 0, 640, 480, screen_palette
	stop

%href
screen
screen_normal
screen_hide
screen_fixedsize
screen_tool
screen_frame

%index
screen_hide
非表示のウィンドウを作成

%inst
screen命令の第4引数に指定することで、非表示のウィンドウを作成できます。
他のマクロと組み合わせて指定する場合は、その和または論理和を指定してください。

%sample
// ウィンドウID0のウィンドウを非表示で作成
	screen 0, 640, 480, screen_hide
	stop

%href
screen
screen_normal
screen_palette
screen_fixedsize
screen_tool
screen_frame
%index
screen_fixedsize
サイズ固定ウィンドウを作成

%inst
screen命令の第4引数に指定することで、サイズ固定のウィンドウを作成できます。
他のマクロと組み合わせて指定する場合は、その和または論理和を指定してください。

%sample
// ウィンドウID0のウィンドウをサイズ固定で作成
	screen 0, 640, 480, screen_fixedsize
	stop

%href
screen
screen_normal
screen_palette
screen_hide
screen_tool
screen_frame
%index
screen_tool
ツールウィンドウを作成

%inst
screen命令の第4引数に指定することで、ツールウィンドウを作成できます。
他のマクロと組み合わせて指定する場合は、その和または論理和を指定してください。

%sample
// ウィンドウID0のツールウィンドウを作成
	screen 0, 640, 480, screen_tool
	stop

%href
screen
screen_normal
screen_palette
screen_hide
screen_fixedsize
screen_frame

%index
screen_frame
深い縁のあるウィンドウを作成

%inst
screen命令の第4引数に指定することで、深い縁のあるウィンドウを作成できます。
他のマクロと組み合わせて指定する場合は、その和または論理和を指定してください。

%sample
// ウィンドウIDの深い縁のあるウィンドウを作成
	screen 0, 640, 480, screen_frame
	stop

%href
screen
screen_normal
screen_palette
screen_hide
screen_fixedsize
screen_tool

%index
font_normal
通常のスタイルでフォント設定

%inst
font命令の第3引数に指定することで、通常のスタイルでフォントを設定することができます。
省略しても同じ結果が得られますので、省略しても構いません。

%sample
// サイズ12のMSゴシックを設定
	font msgothic, 24, font_normal
	mes "サイズ24のMSゴシック（通常のスタイル）"

// 省略しても同じ結果が得られる
	font msgothic, 24
	mes "サイズ24のMSゴシック（通常のスタイル）"
	stop

%href
font
font_bold
font_italic
font_underline
font_strikeout
font_antialias
%index
font_bold
太文字でフォント設定

%inst
font命令の第3引数に指定することで、太文字のフォントを設定することができます。
他のマクロと組み合わせて指定する場合は、その和または論理和を指定してください。

%sample
// サイズ24のMSゴシックを設定
	font msgothic, 24, font_normal
	mes "サイズ24のMSゴシック（通常のスタイル）"

// サイズ24で太文字のMSゴシックを設定
	font msgothic, 24, font_bold
	mes "サイズ24のMSゴシック（太文字）"
	stop

%href
font
font_normal
font_italic
font_underline
font_strikeout
font_antialias
%index
font_italic
イタリック体でフォント設定

%inst
font命令の第3引数に指定することで、イタリック体のフォントを設定することができます。
他のマクロと組み合わせて指定する場合は、その和または論理和を指定してください。

%sample
// サイズ24のMSゴシックを設定
	font msgothic, 24, font_normal
	mes "サイズ24のMSゴシック（通常のスタイル）"

// サイズ24でイタリック体MSmsゴシックを設定
	font msgothic, 24, font_italic
	mes "サイズ24のMSゴシック（イタリック体）"
	stop

%href
font
font_normal
font_bold
font_underline
font_strikeout
font_antialias
%index
font_underline
下線付きでフォント設定

%inst
font命令の第3引数に指定することで、下線付きのフォントを設定することができます。
他のマクロと組み合わせて指定する場合は、その和または論理和を指定してください。

%sample
// サイズ24のMSゴシックを設定
	font msgothic, 24, font_normal
	mes "サイズ24のMSゴシック（通常のスタイル）"

// サイズ24で下線付きのMSゴシックを設定
	font msgothic, 24, font_underline
	mes "サイズ24のMSゴシック（下線付き）"
	stop

%href
font
font_normal
font_bold
font_italic
font_strikeout
font_antialias
%index
font_strikeout
打ち消し線付きでフォント設定

%inst
font命令の第3引数に指定することで、打ち消し線付きのフォントを設定することができます。
他のマクロと組み合わせて指定する場合は、その和または論理和を指定してください。

%sample
// サイズ24のMSゴシックを設定
	font msgothic, 24, font_normal
	mes "サイズ24のMSゴシック（通常のスタイル）"

// サイズ24で打ち消し線付きのMSゴシックを設定
	font msgothic, 24, font_strikeout
	mes "サイズ24のMSゴシック（打ち消し線付き）"
	stop

%href
font
font_normal
font_bold
font_italic
font_underline
font_antialias
%index
font_antialias
アンチエイリアスでフォント設定

%inst
font命令の第3引数に指定することで、アンチエイリアスのフォントを設定することができます。
他のマクロと組み合わせて指定する場合は、その和または論理和を指定してください。

%sample
// サイズ24のMSゴシックを設定
	font msgothic, 24, font_normal
	mes "サイズ24のMSゴシック（通常のスタイル）"

// サイズ24でアンチエイリアスのMSゴシックを設定
	font msgothic, 24, font_antialias
	mes "サイズ24のMSゴシック（アンチエイリアス）"
	stop

%href
font
font_normal
font_bold
font_italic
font_underline
font_strikeout


%index
objmode_normal
HSP標準フォントを設定

%inst
objmode命令の第1引数に指定することで、オブジェクト制御命令で使用されるフォントをHSP標準フォントに設定することができます。
省略しても同じ結果が得られますので、省略しても構いません。

%sample
	s = "オブジェクト制御命令で使用されるフォントのサンプル"

// オブジェクト制御命令で使用されるフォントをHSP標準フォントに設定
	objmode objmode_normal
	mesbox s, ginfo_winx, ginfo_winy / 2

// 省略しても同じ結果が得られる
	objmode objmode_normal
	mesbox s, ginfo_winx, ginfo_winy / 2

	stop

%href
objmode
objmode_guifont
objmode_usefont
objmode_usecolor

%index
objmode_guifont
デフォルトGUIフォントを設定

%inst
objmode命令の第1引数に指定することで、オブジェクト制御命令で使用されるフォントをデフォルトGUIフォントに設定することができます。

%sample
	s = "オブジェクト制御命令で使用されるフォントのサンプル"

// オブジェクト制御命令で使用されるフォントをデフォルトGUIフォントに設定
	objmode objmode_guifont
	mesbox s, ginfo_winx, ginfo_winy

	stop

%href
objmode
objmode_normal
objmode_usefont
objmode_usecolor


%index
objmode_usefont
font命令で選択されているフォントを設定

%inst
objmode命令の第1引数に指定することで、オブジェクト制御命令で使用されるフォントをfont命令で選択されているフォントに設定することができます。

%sample
	s = "オブジェクト制御命令で使用されるフォントのサンプル"

// オブジェクト制御命令で使用されるフォントをfont命令で選択されているフォントに設定
	objmode objmode_usefont

	font msmincho, 24
	mesbox s, ginfo_winx, ginfo_winy / 2

	font msgothic, 18, font_italic
	mesbox s, ginfo_winx, ginfo_winy / 2

	stop

%href
objmode
objmode_normal
objmode_guifont
objmode_usecolor


%index
objmode_usecolor
objcolor命令で選択されている色を設定

%inst
objmode命令の第1引数に指定することで、オブジェクト制御命令で使用される色をcolor命令、objcolor命令で指定されている色に設定することができます。

%href
objmode
objmode_normal
objmode_guifont
objmode_usefont



%index
msgothic
MSゴシックフォント

%group
システム変数

%inst
MSゴシックを示すフォントを示すキーワードです。
font命令で指定するフォント名として使用することができます。

%href
msmincho

;---------------------------------------------------------------------
%index
msmincho
MS明朝フォント

%group
システム変数

%inst
MS明朝を示すフォントを示すキーワードです。
font命令で指定するフォント名として使用することができます。

%href
msgothic

;---------------------------------------------------------------------
%index
and
論理積(演算子)

%group
標準定義マクロ

%inst
論理積を示す演算子「&」と同様に使用することができるマクロです。

%href
or
xor
not

;---------------------------------------------------------------------
%index
or
論理和(演算子)

%group
標準定義マクロ

%inst
論理和を示す演算子「|」と同様に使用することができるマクロです。

%href
and
xor
not

;---------------------------------------------------------------------
%index
xor
排他的論理和(演算子)

%group
標準定義マクロ

%inst
排他的論理和を示す演算子「^」と同様に使用することができるマクロです。

%href
and
or
not

;---------------------------------------------------------------------
%index
not
否定(演算子)

%group
標準定義マクロ

%inst
否定を示す演算子「!」と同様に使用することができるマクロです。

%href
and
or
xor

;---------------------------------------------------------------------
%index
M_PI
円周率

%inst
円周率を表す定数です。3.14159265358979323846が定義されています。

%group
数学定数

%href
rad2deg
deg2rad


;---------------------------------------------------------------------
%index
rad2deg
ラジアンを度に変換

%prm
(p1)
p1 : 度に変換する角度（ラジアン）

%inst
角度の単位をラジアンから度へ変換します。
弧度法で表された角度を度数法での角度に変換するとも言えます。

%sample
	tmp = M_PI
	mes str(tmp) + "ラジアンは" + rad2deg(tmp) + "°です。"
	stop

%href
M_PI
deg2rad


;---------------------------------------------------------------------
%index
deg2rad
度をラジアンに変換

%prm
(p1)
p1 : ラジアンに変換する角度（度）

%inst
角度の単位を度からラジアンへ変換します。
度数法で表された角度を弧度法での角度に変換するとも言えます。

%sample
	tmp = 90
	mes str(tmp) + "°は" + deg2rad(tmp) + "ラジアンです。"
	stop

%href
M_PI
rad2deg


