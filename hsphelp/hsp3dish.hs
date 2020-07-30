;
;	HSP help manager用 HELPソースファイル
;	(先頭が「;」の行はコメントとして処理されます)
;

%type
拡張命令
%ver
3.6
%note
hsp3dish.asをインクルードすること。

%date
2020/06/10
%author
onitama
%dll
HSP3Dish
%url
http://hsp.tv/
%port
Win
%group
拡張入出力制御命令


%index
setreq
システムリクエスト設定
%prm
type,val
type(0) : 設定タイプ(SYSREQ_???)
val(0)  : 設定する値
%inst
HSP3Dishランタイムに対して様々なシステム設定を行ないます。
設定タイプ名で指定できるのは以下のシンボルです。
^p
	設定タイプ名      読み書き(*)   内容
------------------------------------------------------------------------
	SYSREQ_CLSMODE        RW        背景のクリアフラグ
	SYSREQ_CLSCOLOR       RW        背景クリア色
	SYSREQ_TIMER          R         ミリ秒単位のタイマー値
	SYSREQ_PLATFORM       R         HSP3Dishを実行しているプラットフォーム

	(*)項目ごとに、読み(R)書き(W)の可否が異なります
^p

・SYSREQ_CLSMODE、SYSREQ_CLSCOLOR

redraw 0による描画開始時に画面をクリアするかどうかを設定します。 SYSREQ_CLSMODEに1を設定した場合はクリア、0の場合はクリアを行ないません。 スクリプト内で画面のクリアを行なっている場合は、システム側で背景クリアをしない方が 処理速度の向上につながります。 SYSREQ_CLSCOLORでクリアする際の色コードを設定することができます。 色コードは、RGB24bit($rrggbb)を1つにした数値で設定します。

・SYSREQ_TIMER

ミリ秒単位のタイマー値を返します。 プラットフォームごとに返される値は異なりますが、1ミリ秒につき1ずつ値が加算されていきます。

・SYSREQ_PLATFORM

以下のHSP3Dishプラットフォーム値を返します。
^p
	マクロ名            値  内容
	-------------------------------------------
	PLATFORM_WINDOWS    0   Windows版
	PLATFORM_IOS        1   iOS版
	PLATFORM_ANDROID    2   android版
	PLATFORM_WEBGL      3   hsp3dish.js(WebGL/JavaScript)版
	PLATFORM_LINUX      4   Linux版
	PLATFORM_RASPBIAN   5   Raspberry Pi(raspbian)版
^p

%href
getreq


%index
getreq
システムリクエスト取得
%prm
var,type
var  : 結果が代入される変数名
type : 設定タイプ(SYSREQ_???)
%inst
HSP3Dishランタイムが保持するシステム設定を取得して変数に代入します。
設定タイプ名で指定できる名称は、setreq命令と同様です。

%href
setreq


%index
gfilter
テクスチャ補間の設定
%prm
type
type : 設定タイプ(FILTER_???)
%inst
HSP3Dishランタイムが描画を行なう際の、テクスチャ補間(フィルタリング)の設定を行ないます。
設定タイプ名で指定できる名称は、以下の通りです。
^p
	マクロ名        値      内容
	-------------------------------------------
	FILTER_NONE     0       補間なし(NEAREST)
	FILTER_LINEAR   1       直線補間(LINEAR)
	FILTER_LINEAR2  2       エリア補間(高度な補間処理)
^p
フィルタリングの設定により、画像の拡大・縮小時のドット補間方法を 選ぶことができます。FILTER_NONEを指定することで、補間なし(ニアレスト) の状態になります。FILTER_LINEARは、直線(リニア)補間、FILTER_LINEAR2は より高度な補間方法(バイリニアなど)を選択します。 gfilter命令の設定は、以降のすべての描画命令に適用されます。 また、「redraw 0」による画面クリア時には、フィルタリング設定は FILTER_NONEにリセットされます。

%href
gcopy
grotate
gsquare
celput


%index
mtlist
ポイントIDリスト取得
%prm
var
var  : 結果が代入される変数名
%inst
現在タッチされている情報を持つポイントIDリストを取得します。
ポイントIDリストは、複数のポイントIDを返すもので、マルチタッチによる複数のポイントを含む情報になります。
mtlist命令が実行されると、その時点で押されている(触れている)すべてのポイントID(整数値)を、varで指定した変数に配列として代入します。
たとえば、ID0とID2、ID3の３つの情報があれば、var(0)に0、var(1)に2、var(2)に3という値が代入されることになります。
代入されたポイントIDの数は、システム変数statに返されます。
この命令で得られたポイントIDを使用して、mtinfo命令で詳細なタッチ情報を取得することができます。
マルチタッチの情報を取得するためには、それぞれのデバイスでマルチタッチの設定を有効にする必要があります。
また、Windows上では、Windows7以降のマルチタッチに対応したデバイスが必要になります。

%href
mtinfo


%index
mtinfo
タッチ情報取得
%prm
var,id
var  : 結果が代入される変数名
id(0): ポイントID
%inst
指定されたポイントIDに関連付けられたタッチ情報を取得します。
タッチ情報は、varで指定した変数に配列として代入されます。
^p
	要素値  内容
	-------------------------------------------
	  0     タッチ状態(1=ON/0=OFF)
	  1     タッチされたX座標
	  2     タッチされたY座標
	  3     タッチ識別用ID
^p
つまり、var(1)にはタッチされたX座標が、var(0)にはタッチの状態(1ならば押されている)が代入されます。
varで指定された変数は必ず整数型の配列変数として初期化されます。
var(3)のタッチ識別用IDは、他のタッチと識別するための整数値です。この値は、デバイスごとに異なります。

%href
mtlist


%index
devinfo
デバイス情報文字列取得
%prm
var,"name"
var    : 結果が代入される変数名
"name" : デバイス情報名
%inst
特定のOS、及びデバイスに依存した情報を取得します。
文字列"name"で示された情報を、varで指定した変数に代入します。(代入される情報は文字列となります。)
実行後に、正しく情報が取得された場合は、システム変数statが0以上になります。
情報が存在しないか、エラーが発生した場合はシステム変数statがマイナスの値となります。

%href
devinfoi


%index
devinfoi
デバイス情報値取得
%prm
var,"name"
var    : 結果が代入される変数名
"name" : デバイス情報名
%inst
特定のOS、及びデバイスに依存した情報を取得します。
文字列"name"で示された情報を、varで指定した変数に代入します。(代入される情報は整数型の配列となります。)
実行後に、正しく情報が取得された場合は、システム変数statが0以上になります。
情報が存在しないか、エラーが発生した場合はシステム変数statがマイナスの値となります。

%href
devinfo


%index
devprm
デバイス制御用のパラメーター設定
%prm
"name","value"
"name" : パラメーター名を示す文字列
"value" : 設定する文字列
%inst
devcontrol命令によりデバイス制御する際のパラメーター(付加情報)を設定します。
実行後に、正しく実行が行なわれた場合は、システム変数statが0以上の値になります。
パラメーターが存在しないか、エラーが発生した場合はシステム変数statがマイナスの値となります。

%href
devcontrol


%index
devcontrol
デバイス制御を実行する
%prm
"command",p1,p2,p3
"command" : コマンド名を示す文字列
p1(0)     : 制御パラメーター1
p2(0)     : 制御パラメーター2
p3(0)     : 制御パラメーター3
%inst
特定のOS、及びデバイスに依存したデバイス制御を実行します。
実行後に、正しく実行が行なわれた場合は、システム変数statが0以上の値になります。
パラメーターが存在しないか、エラーが発生した場合はシステム変数statがマイナスの値となります。

%href
devprm


%index
setcls
画面クリア設定
%group
拡張マルチメディア制御命令
%prm
mode,color,tex
mode(0)  : 画面クリアのモード
color(0) : 画面クリア色(24bitRGBコード)
%inst
redraw 0による画面初期化時に行なわれる画面クリアのための設定を行ないます。
mode値の設定により、画面クリアを行なうためのモードを設定します。
1を設定した場合はクリア、0の場合はクリアを行ないません。
スクリプト内で画面のクリアを行なっている場合は、システム側で背景クリアをしない方が処理速度の向上につながります。
^p
	モード値  内容
	------------------------------------------------------
	    0     画面クリアを行なわない
	    1     画面クリアを行なう
^p
colorでクリアする際の色を設定することができます。色は、24bitRGBコード($rrggbb)の数値で設定します。


%href
redraw


%index
celputm
複数のセルをまとめて描画
%group
拡張マルチメディア制御命令
%prm
vxpos,vypos,vcelid,bufid,count
vxpos    : 表示X座標が格納された整数型配列変数名
vypos    : 表示Y座標が格納された整数型配列変数名
vcelid   : セルIDが格納された整数型配列変数名
bufid(0) : セルが格納されているバッファID
count(1) : 配列変数から取り出す項目数
%inst
整数型の配列変数に代入された座標、セルIDをもとにcelputと同様の表示を配列要素ごとに実行します。
配列に保存された複数の項目について、一度に処理が行なわれるため効率化と高速化につながります。
countで配列変数から取り出す項目の数を指定します。配列変数からは、要素0から順番に値が取り出されます。
vxposにはX座標、vyposにはY座標、vcelidにはセルIDが代入されている必要があります。
セルIDがマイナス値の場合は、描画は行なわれません。
celputm命令の実行後、実際に表示されたセルの数がシステム変数statに代入されます。

%href
celput


%index
httpload
http通信の開始
%group
拡張入出力制御命令
%prm
"url","postdata"
"url" : リクエストを行なうファイル名
"postdata" : POSTを行なう際の文字列データ

%inst
指定されたURLに対して、http通信を開始します。
この命令は、通信のリクエストを開始するだけで、すぐに結果は取得されません。
通信の結果は、httpinfo命令を使用して正しくデータを受信したことを確認した上で、取得する必要があります。
^
httploadは、通常は"url"の指定でリクエストを行なうURLスキーム(「http://www.onionsoft.net/about.html」のような文字列)を指定することで、HTTP通信を開始します。
^p
;例:
;	URLを指定して通信を開始する
httpload "http://www.onionsoft.net/about.html"
^p
CGIやWebAPIなどにアクセスするために、POST形式で通信を行なう場合は、"postdata"にパラメーター文字列を指定してください。
("postdata"の指定を省略した場合は、通常のGET形式によるHTTP通信が行なわれます。)
^p
;例:
;	URLを指定してPOST形式の通信を開始する
httpload "http://www.onionsoft.net/hsp/beta/betabbs.cgi","mode=find&cond=and&log=0&word=script"
^p
POST形式の通信は、"postdata"の形式や仕様については、CGIやhttpリクエストについての知識がある人に向けて用意されているものです。
httpload命令が実行された後は、システム変数statにリクエスト処理が行なわれたかを示す数値が代入されます。
システム変数statが、0の場合は正しくリクエストが完了しています。システム変数statが、0以外の場合は、エラーによりリクエストができなかったことを示しています。
httpload命令で正常にhttp通信を開始した後は、httpinfo命令による受信確認とデータ取得処理をスクリプト側で行なう必要があります。
。
^p
;例:
	;	URLを指定して通信を開始する
	httpload "http://www.onionsoft.net/about.html"
	if stat : goto *bad	; 正しくリクエストができなかった
*main
	;	結果待ちのためのループ
	httpinfo res,HTTPINFO_MODE		; 現在のモードを取得
	if res = HTTPMODE_READY : goto *ok	; 通信が終了した
	if res <= HTTPMODE_NONE : goto *bad	; エラーが発生した
	await 50				; 時間待ちを行なう
	goto *main
*bad
	;	エラー
	httpinfo estr,HTTPINFO_ERROR		; エラー文字列を取得する
	dialog "ERROR "+estr
	stop
*ok
	;	完了
	httpinfo buf,HTTPINFO_DATA		; 結果データを取得する
	httpinfo size,HTTPINFO_SIZE		; データサイズを取得する
^p
http通信によって取得されるデータは、変数が持つメモリに格納されます。
大きなファイルを取得した場合は、そのサイズだけメモリが消費するので注意してください。
あくまでも、メモリ上に取得できる範囲のサイズを想定した簡易的な通信の仕組みであることをご理解の上使用してください。
Windows上であれば、HSPINETプラグインにより、さらに詳細なHTTP通信を行なうことが可能です。

%href
httpinfo


%index
httpinfo
http通信の情報を取得
%group
拡張入出力制御命令
%prm
p1,p2
p1    : データが代入される変数
p2(0) : データの種別ID

%inst
http通信に関する情報を取得します。
httpload命令による、http通信を行なった結果などを取得することができます。
p2で指定された取得モードに対応する内容を、p1で指定された変数に代入します。
取得モードの値は、以下の通りです。
^p
	  種別ID              内容
	------------------------------------------------------
	HTTPINFO_MODE 0       現在の通信モード
	HTTPINFO_SIZE 1       データサイズ
	HTTPINFO_DATA 16      取得データ(*)
	HTTPINFO_ERROR 17     エラー文字列(*)

(*)の項目は、文字列型変数として代入されます。
^p
受信データ(HTTPINFO_DATA)の読み出しを行なうと、受信データの内容はクリアされるので注意してください。
無効な種別IDが指定された場合は、0または""(空の文字列)が返されます。
^
通信モードの値は、以下の内容になっています。
^p
	  通信モード値        内容
	------------------------------------------------------
	HTTPMODE_NONE         通信初期化エラー
	HTTPMODE_READY        通信可能状態
	HTTPMODE_REQUEST      リクエスト通信準備
	HTTPMODE_SEND         リクエスト送信
	HTTPMODE_DATAWAIT     通信結果待機中
	HTTPMODE_DATAEND      通信終了
	HTTPMODE_ERROR        エラー発生
^p

%href
httpload



%index
gmulcolor
テクスチャ乗算値の設定
%prm
p1,p2,p3
p1,p2,p3=0～255(255) : 色コード（R,G,Bの輝度）
%inst
画像(テクスチャ)コピーを行なう際のカラー乗算値を指定します。
p1,p2,p3がそれぞれ、R,G,Bの輝度になります。
gcopy,gzoom,celput,grotate(画像使用時)などでコピーされる画像に対して、指定したR,G,B値が乗算されます。
初期値は、RGBそれぞれ255が指定されています。テクスチャ乗算値を設定することにより、元の画像の色を変化させることができます。
画像コピーの際に、gmode命令で指定されるコピーモードが0または1(アルファチャンネルを反映しない)の場合は、テクスチャ乗算値は無効になるので注意してください。
テクスチャ乗算値は、mes命令による文字の描画、及びgsquare命令には反映されません。また、boxf,line命令など単色の描画には反映されません。

%href
color
gcopy
gzoom
grotate
gsquare
celput




%index
viewcalc
描画時の座標変換を設定
%prm
p1,p2,p3,p4,p5
p1(0)   : 設定モード
p2(0.0) : パラメーター1(実数)
p3(0.0) : パラメーター2(実数)
p4(0.0) : パラメーター3(実数)
p5(0.0) : パラメーター4(実数)
%inst
2D描画時のX,Y座標に任意の計算を適用します。
これにより、描画される表示物全体のスケーリングや移動、回転などを行うことができます。
p1パラメーターで設定モードを指定します。
設定モードにより、以降の設定される内容が変わります。
^p
	マクロ名            値   内容
	-----------------------------------------------------
	vptype_off           0   座標変換なし(デフォルト)
	vptype_translate     1   移動パラメーターを指定
	vptype_rotate        2   回転パラメーターを指定
	vptype_scale         3   スケールパラメーターを指定
	vptype_3dmatrix      4   4×4マトリクスによる座標変換
	vptype_2d            5   2D座標変換を設定
	vptype_3d            6   3D座標変換を設定
^p
vptype_2dは、基本的な2D座標変換を適用します。
パラメーター1～3で、X,Yのスケール(倍率)、回転角度(ラジアン単位)を設定することができます。
^p
	viewcalc vptype_translate, 移動X, 移動Y
	viewcalc vptype_2d, スケールX, スケールY, 回転角度(ラジアン)
^p
vptype_3dは、3D空間上にマッピングして描画を適用します。
移動、回転、スケールそれぞれのX,Y,Z値を指定することができます。
^p
	viewcalc vptype_translate, 移動X, 移動Y, 移動Z
	viewcalc vptype_rotate, 回転X, 回転Y, 回転Z
	viewcalc vptype_scale, スケールX, スケールY, スケールZ
	viewcalc vptype_3d, カメラ角度(FOV), NearZ値, FarZ値
^p
vptype_3dmatrixは、4×4の行列(マトリクス)を指定して座標変換を行います。
ユーザー自身でマトリクスを作成する場合に使用することができます。
4×4のm00～m33までのパラメーターを以下のように指定します。
^p
	viewcalc vptype_translate, m00, m01, m02, m03
	viewcalc vptype_rotate, m10, m11, m12, m13
	viewcalc vptype_scale, m20, m21, m22, m23
	viewcalc vptype_3dmatrix, m30, m31, m32, m33
^p
viewcalc命令を設定した以降の描画に、座標変換が適用されます。
画面の初期化時は、座標変換なし(vptype_off)が設定されています。
vptype_3d及びvptype_3dmatrixにより変換設定した場合は、マウスの座標を正しく取得することができなくなるので注意してください。

%href
pos
mes


%index
celbitmap
変数バッファを画像データとして適用する
%prm
p1,var,p2
p1(0)   : ウインドウID
var     : イメージを置き換えるための配列変数
p2(0)   : 動作オプション
%inst
変数バッファの内容を画像データとして指定されたオフスクリーンバッファを置き換えます。
あらかじめ、buffer命令により作成したオフスクリーンバッファと、置き換えるための変数バッファを初期化する必要があります。
オフスクリーンバッファは、buffer命令でscreen_offscreenのオプションを指定して作成してください。
^p
	buffer 2,256,256,screen_offscreen
^p
その後、バッファを置き換えるための変数バッファを作成します。
dim命令により、１次元配列を初期化して作成してください。作成するサイズは、オフスクリーンバッファのXサイズ×Yサイズになります。
^p
	dim bitmap, 256*256
^p
これで、celbitmap命令を使用する準備が整いました。
celbitmap命令により、オフスクリーンバッファのIDと配列変数を指定することで、変数バッファの内容がそのまま画像イメージとして適用されます。
変数バッファは、1要素あたり1ドットとなります。1ドットは、32bitの要素をR,G,B,Aそれぞれ8bit単位で格納されるRGB形式のデータとなります。
たとえば、「$ff204080」(16進数)を指定した場合は、A=$ff(255)、B=$20(32)、G=$40(64)、R=$80(128)となります。(カッコ内は10進数)
画像データの置き換えは、コストの高い処理になるため、大きなサイズのイメージを置き換える場合、フレームレートが下がることがありますので注意してください。
p2パラメーターにより、置き換えの形式や動作を指定することができます。
^p
 値  マクロ             動作
---------------------------------------------------------
  0  celbitmap_rgb      $aabbggrr形式でデータを保持する
  1  celbitmap_bgr      $aarrggbb形式でデータを保持する
  16 celbitmap_capture  描画画面の内容を変数に読み出す
^p

通常は、OpenGLの標準に合わせたcelbitmap_rgbの形式が使用されます。オプションにcelbitmap_bgrを指定した場合は、DirectX標準に合わせたARGBの並びになります。
celbitmap_captureを指定した場合は、描画先の画面上のデータを配列変数に読み出します。(読み出しのサイズはオフスクリーンバッファのサイズになります)
※celbitmap_captureオプションは、実験的な実装です。現在は、Windows(DirectX版HGIMG4を除く)、Linux版でのみ使用できますのでご了承ください。


%sample
#include "hsp3dish.as"

	buffer 2,256,256,screen_offscreen
	gsel 0
	dim bitmap,256*256
	repeat 256*256
	bitmap(cnt)=$ff00ffff
	loop
*main
	redraw 0
	celbitmap 2,bitmap
	pos 0,0
	celput 2
	redraw 1
	await 1000/30
	goto *main
%href
buffer


