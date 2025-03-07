;
;	HSP help manager用 HELPソースファイル
;	(先頭が「;」の行はコメントとして処理されます)
;
%type
拡張命令
%ver
3.7
%note
hgimg4.asまたはhgimg4dx.asをインクルードすること。
%date
2024/07/04
%author
onitama
%dll
hgimg4
%url
https://hsp.tv/make/hgimg4.html
%port
Win
%portinfo
Windows/Linux/android/iOS等HGIMG4をサポートするシステム上で動作します。
Windowsの場合は、OpenGL3.1以降/DirectX9が必要です。



%index
gpreset
HGIMG4の初期化
%group
拡張画面制御命令
%prm
option
option(0) : 初期化オプション
%inst
HGIMG4の初期化を行ないます。
シーンを再構築する場合に使用してください。
optionで設定した内容によって初期化の内容が変わります。
^p
	オプション値  内容
	---------------------------------------------------------------
	    0         すべてのオブジェクトを破棄して初期状態に戻す
	    1         シーンに存在するオブジェクトのみ破棄する
^p
%href


%index
gpdraw
シーン内の全オブジェクトを描画する
%group
拡張画面制御命令
%prm
option,debug
option($ffff) : 描画オプション
debug(0)      : デバッグオプション
%inst
シーン内のオブジェクトをすべて描画します。
optionパラメーターを指定することで、限られた項目の描画だけを行なうことができます。
^p
        マクロ名                      内容
    -------------------------------------------------------------------------------
	GPDRAW_OPT_OBJUPDATE          オブジェクトの自動移動処理/アニメーション処理
	GPDRAW_OPT_DRAWSCENE          3Dシーン描画処理
	GPDRAW_OPT_DRAWSCENE_LATE     3Dシーン描画処理(OBJ_LATE)
	GPDRAW_OPT_DRAW2D             2Dスプライト描画処理
	GPDRAW_OPT_DRAW2D_LATE        2Dスプライト描画処理(OBJ_LATE)
^p
optionパラメーターを省略した場合は、すべての項目が選択されている状態になります。
通常は、optionパラメーターを指定しなくても問題ありません。
また、2Dの直接描画命令だけを使用して、オブジェクトによる描画が不要な場合は、gpdraw命令を記述する必要もありません。
OBJ_LATEのモードフラグ値が付加されたもの(半透明α値が設定されているものを含む)は、不透明のオブジェクトよりも後に描画するため、項目が分けられています。
複数の項目を「|」で区切って指定することもできます。
^p
	例:
	gpdraw GPDRAW_OPT_DRAW2D|GPDRAW_OPT_DRAW2D_LATE
^p
上の例では、2Dスプライト描画処理、2Dスプライト描画処理(OBJ_LATE)だけを実行します。
これを利用すると、3Dシーン描画と2Dスプライト描画の間にgcopy命令など別な描画命令による表示を行なうことが可能になります。
debugパラメーターを指定することで、デバッグのための描画を追加することができます。
通常は、debugパラメーターを指定しなくても問題ありません。
^p
        マクロ名                      内容
    -------------------------------------------------------------------------------
	GPDRAW_OPT_DEBUG_PHY          物理オブジェクトのデバッグ表示
^p
GPDRAW_OPT_DEBUG_PHYは、物理設定された3Dオブジェクトのデバッグ表示を有効にします。

%href


%index
gpusescene
シーンの切り替え
%group
拡張画面制御命令
%prm
id
id(0) : 選択するシーンID
%inst
(今後のバージョンでサポートするための予約キーワードです。機能は現在まだ実装されていません。)
%href


%index
gpsetprm
オブジェクトのコアパラメーター設定
%group
拡張画面制御命令
%prm
objid,prmid,value
objid(0) : オブジェクトID
prmid(0) : コアパラメーターID
value(0) : 設定する値(整数値)
%inst
オブジェクトごとに保持されているコアパラメーターにvalueで指定された値を上書き設定します。
値を上書きではなく、ビット単位にON,OFFを行ないたい場合は、gpsetprmon/gpsetprmoff命令を使用してください。

コアパラメーターは、32bit整数値で、様々な情報を管理しています。
コアパラメーターIDとして指定可能な項目は以下の通りです。
^p
	マクロ名             | 内容
	--------------------------------------------------------------
	PRMSET_FLAG            オブジェクト登録フラグ(*)
	PRMSET_MODE            モードフラグ値
	PRMSET_ID              オブジェクトID(*)
	PRMSET_ALPHA           透明度(α値)
	PRMSET_TIMER           タイマー値
	PRMSET_MYGROUP         自身のコリジョングループ
	PRMSET_COLGROUP        衝突検出するコリジョングループ
	PRMSET_SHAPE           形状ID(*)
	PRMSET_USEGPMAT        マテリアルID
	PRMSET_USEGPPHY        物理設定ID(*)
	PRMSET_COLILOG         コリジョンログID(*)
	PRMSET_FADE            フェードパラメーター
	PRMSET_SPRID           ソースバッファID(スプライトのみ)
	PRMSET_SPRCELID        ソースのセルID(スプライトのみ)
	PRMSET_SPRGMODE        コピーモード(スプライトのみ)

	(*)の項目は読み出しのみ
^p
たとえば、PRMSET_FADEパラメーターを変更することでフェードイン・アウトなどの制御を行うことができます。
^p
	例:
	gpsetprm id, PRMSET_FADE, -4	; フェードアウト
^p
PRMSET_MODEで設定されるモードフラグ値では、ビットごとにオブジェクトの挙動を設定することができます。
特定のモードフラグ値だけを設定・消去するためのsetobjmode命令も用意されています。

%href
gpgetprm
gpsetprmon
gpsetprmoff
setobjmode


%index
gpgetprm
オブジェクトのコアパラメーター取得
%group
拡張画面制御命令
%prm
var,objid,prmid
var      : 値が代入される変数名
objid(0) : オブジェクトID
prmid(0) : コアパラメーターID
%inst
オブジェクトごとに保持されているコアパラメーターに値を取得します。
コアパラメーターは、32bit整数値で、様々な情報を管理しています。
コアパラメーターIDとして指定可能な項目は以下の通りです。
^p
	マクロ名             | 内容
	--------------------------------------------------------------
	PRMSET_FLAG            オブジェクト登録フラグ(*)
	PRMSET_MODE            モードフラグ値
	PRMSET_ID              オブジェクトID(*)
	PRMSET_ALPHA           透明度(α値)
	PRMSET_TIMER           タイマー値
	PRMSET_MYGROUP         自身のコリジョングループ
	PRMSET_COLGROUP        衝突検出するコリジョングループ
	PRMSET_SHAPE           形状ID(*)
	PRMSET_USEGPMAT        マテリアルID
	PRMSET_COLILOG         コリジョンログID(*)
	PRMSET_FADE            フェードパラメーター
	PRMSET_SPRID           ソースバッファID(スプライトのみ)
	PRMSET_SPRCELID        ソースのセルID(スプライトのみ)
	PRMSET_SPRGMODE        コピーモード(スプライトのみ)

	(*)の項目は読み出しのみ
^p
%href
gpsetprm



%index
gppostefx
ポストエフェクト生成
%group
拡張画面制御命令
%prm
var
var     : エフェクトIDが代入される変数名
%inst
(今後のバージョンでサポートするための予約キーワードです。機能は現在まだ実装されていません。)
%href


%index
gpuselight
ライトオブジェクトの登録
%group
拡張画面制御命令
%prm
id,index
id(0) : 登録するライトオブジェクトのID
index=0〜9(0) : 登録するライトオブジェクトのインデックス
%inst
指定したライトオブジェクトをカレントライトに登録します。
必ず、ライトとして初期化されているライトオブジェクトを指定する必要があります。
ライトオブジェクトは、gplight命令によって初期化することができます。
カレントライトには、ディレクショナルライト・ポイントライト・スポットライトの3種類をそれぞれ最大10個まで登録することができます。
初期状態では、カレントライトに設定ができるライトは、ディレクショナルライト1個のみとなっています。ポイントライト、スポットライトなど複数のライトを使用する場合は、gpresetlight命令によってカレントライトの個数をあらかじめ決めておく必要があります。
indexの値は、複数のライトをカレントライトに登録する場合に使用します。1個目のライトは0、2個目のライトは1…という形で1つの種類につき最大10個のライトを設定することが可能です。
カレントライトに登録することで、以降に生成されるモデル・マテリアルはカレントライトの影響を受けることになります。
既にオブジェクトに設定されているライトの設定を変更する場合は、setobjlight命令を使用してください。

%href
gpresetlight
gplight
gpusecamera
setobjlight


%index
gpusecamera
カメラオブジェクトの切り替え
%group
拡張画面制御命令
%prm
id
id(0) : 選択するカメラオブジェクトのID
%inst
指定したノードオブジェクトを有効なカメラオブジェクトとして選択します。
必ず、カメラとして初期化されているノードオブジェクトを指定する必要があります。
以降は、切り替えられたカメラを視点としてシーンの描画が行なわれます。
%href
gpuselight


%index
gpmatprm
マテリアルのパラメーター設定
%group
拡張画面制御命令
%prm
id,"name",x,y,z
id(0)  : マテリアルID/オブジェクトID
"name" : パラメーター名
x(0.0) : Xの設定値(実数値)
y(0.0) : Yの設定値(実数値)
z(0.0) : Zの設定値(実数値)
%inst
生成されたマテリアルのシェーダーパラメーターを設定します。
idがオブジェクトIDの場合は、オブジェクトに設定されているマテリアルが直接設定されます。
idがマテリアルIDの場合は、マテリアルが保持している設定が変更されます。
^p
	例:
	gpmatprm objid,"u_diffuseColor",1,0,0.5 ; マテリアルのパラメーター設定
^p
上の例では、u_diffuseColorという名称のパラメーターに( 1.0, 0.0, 0.5 )のベクトル値を設定します。
(シェーダーには、u_diffuseColorという名前で設定されたパラメーターが渡されます。)
シェーダーパラメーターに設定する項目の数に応じて、gpmatprm1、gpmatprm2、gpmatprm4、gpmatprm16命令が用意されています。
(x,y,z,w)の4項目を設定する場合は、gpmatprm4を使用してください。
また、テクスチャ(Sampler)パラメーターをシェーダーに渡すためのgpmatprmt、gpmatprmp命令、テクスチャ設定を行うgpmatprmf命令も用意されています。
%href
gpmat
gpmatprm1
gpmatprm2
gpmatprm4
gpmatprm16
gpmatprmt
gpmatprmtf
gpmatprmp


%index
gpmatstate
マテリアルのステート設定
%group
拡張画面制御命令
%prm
objid,"name","value"
id(0)   : マテリアルID/オブジェクトID
"name"  : 設定項目名(文字列)
"value" : 設定値(文字列)
%inst
生成されたマテリアルの表示ステート設定を変更します。
文字列としてnameで指定された項目に対して、valueで指定された内容を設定します。
項目名と設定内容はすべて文字列で行なうので注意してください。
項目名と設定内容はすべて文字列で行なうので注意してください。(項目名及び設定値は大文字小文字を区別しません。)
^p
	name             value
    --------------------------------------------------------------------
	"blend"          プレンドの有効/無効切り替え
	                         (true または false を指定)
	"blendSrc"       ブレンド元を選択(下記参照)
	"blendDst"       ブレンド先を選択(下記参照)
	"cullFace"       隠面除去(カリング)の有効/無効切り替え
	                         (true または false を指定)
	"cullFaceSide"   隠面除去(カリング)の面指定
	"depthTest"      true または false
	                         (true または false を指定)
	"depthWrite"     Zバッファ書き込みの有効/無効切り替え
	                         (true または false を指定)
	"depthFunc"      Z値比較方法を選択(下記参照)

	(*)blendSrc,blendDstで設定できる文字列

	"ZERO"                     即値(0)
	"ONE"                      即値(1)
	"SRC_COLOR"                書き込み元カラー
	"ONE_MINUS_SRC_COLOR"      書き込み元カラー(反転値)
	"DST_COLOR"                書き込み先カラー
	"ONE_MINUS_DST_COLOR"      書き込み先カラー(反転値)
	"SRC_ALPHA"                書き込み元α
	"ONE_MINUS_SRC_ALPHA"      書き込み元α(反転値)
	"DST_ALPHA"                書き込み先α
	"ONE_MINUS_DST_ALPHA"      書き込み先α(反転値)
	"CONSTANT_ALPHA"           α固定値
	"ONE_MINUS_CONSTANT_ALPHA" α固定値(反転値)
	"SRC_ALPHA_SATURATE"       書き込み元α反転値

	(*)cullFaceSideで設定できる文字列

	BACK             裏面をカリング
	FRONT            表面をカリング
	FRONT_AND_BACK   両面をカリング

	(*)depthFuncで設定できる文字列

	NEVER            常に拒否
	LESS             小さい値の時のみ許可
	EQUAL            同一値のみ許可
	LEQUAL           同一か小さい値の時のみ許可
	GREATER          大きい値の時のみ許可
	NOTEQUAL         同一でない値のみ許可
	GEQUAL           同一か大きい値の時のみ許可
	ALWAYS           常に許可
^p
%href


%index
gpviewport
ビューポート設定
%group
拡張画面制御命令
%prm
x,y,sx,sy
x(0)  : ビューポート基点のX座標
y(0)  : ビューポート基点のY座標
sx    : ビューポートのXサイズ
sy    : ビューポートのYサイズ
%inst
HGIMG4が描画を行なう領域(ビューポート)を設定します。
スクリーン座標の(x,y)を基点として(sx,sy)で指定したサイズの領域に対して描画が行なわれます。(X,Yで指定される座標は、(0,0)を左下とするスクリーン全体のピクセル数を指定します)
sx,syを省略すると、現在の表示X,Yサイズがデフォルトとして指定されます。
ビューポート設定
%href
viewcalc
gpcnvaxis


%index
setobjname
オブジェクトのノード名を設定する
%group
拡張画面制御命令
%prm
objid,"name"
objid(0) : オブジェクトのID
"name"   : 設定される名称(文字列)
%inst
ノードオブジェクトに指定された名称を設定します。
すべてのノードオブジェクトは、任意に指定した名称を設定することが可能です。
%href
getobjname


%index
getobjname
オブジェクトのノード名を取得する
%group
拡張画面制御命令
%prm
var,objid
var      : 結果が代入される変数名
objid(0) : オブジェクトのID
%inst
ノードオブジェクトに指定された名称を取得します。
varで指定された変数に名称が文字列型として代入されます。
すべてのノードオブジェクトは、任意に指定した名称が設定されています。
%href
setobjname


%index
gpcolormat
カラーマテリアルの生成
%group
拡張画面制御命令
%prm
var,color,opt
var      : 生成されたマテリアルIDが代入される変数名
color(0) : マテリアルカラー(24bitRGB値)
opt(0)   : マテリアルオプション値
%inst
指定された色を持つカラーマテリアルの生成を行ないます。
colorパラメーターは、24bitRGB値(0xRRGGBBで示される値)を指定します。省略した場合は、白色(0xffffff)が設定されます。
マテリアルを独自に生成することにより、詳細な質感の設定が可能になります。

^p
	例:
	gpcolormat mat_id,0xff00ff,GPOBJ_MATOPT_NOLIGHT
	gpbox id_model, 1, , mat_id
^p
上の例では、RGBカラーが0xff00ff(紫)の、ライティングを行なわないマテリアルを持った、立方体のノードオブジェクトが生成されます。
optパラメーターにより、マテリアルの設定を変更することができます。
これらの設定は、gpmatstate命令で別途設定することも可能です。
^p
        マクロ名                内容
    -------------------------------------------------------------------
	GPOBJ_MATOPT_NOLIGHT    ライティングを行なわない
	GPOBJ_MATOPT_NOMIPMAP   MIPMAPを生成しない
	GPOBJ_MATOPT_NOCULL     カリングを無効にする
	GPOBJ_MATOPT_NOZTEST    Zテストを無効にする
	GPOBJ_MATOPT_NOZWRITE   Zバッファ書き込みを無効にする
	GPOBJ_MATOPT_BLENDADD   プレンドモードを加算に設定する
^p
正常にマテリアルが生成できなかった場合は、エラー3(パラメータの値が異常です)が発生します。
正しく生成された場合は、varで指定された変数にマテリアルID(整数値)が代入されます。

%href
gptexmat
gpusermat


%index
gptexmat
テクスチャマテリアルの生成
%group
拡張画面制御命令
%prm
var,"file",opt
var     : 生成されたマテリアルIDが代入される変数名
"file"  : 読み込まれるテクスチャファイル名
opt(0)  : マテリアルオプション値
%inst
テクスチャ(画像)マテリアルの生成を行ないます。
マテリアルを独自に生成することにより、詳細な質感の設定が可能になります。
^p
	例:
	gptexmat id_texmat, "res/qbox.png"	; テクスチャマテリアル作成
	gpbox id_model, 1, , id_texmat		; 箱ノードを追加
^p
上の例では、resフォルダ内のqbox.pngをテクスチャとして持ったマテリアルを持った、立方体のノードオブジェクトが生成されます。
optパラメーターにより、マテリアルの設定を変更することができます。
これらの設定は、gpmatstate命令で別途設定することも可能です。
^p
        マクロ名                内容
    -------------------------------------------------------------------
	GPOBJ_MATOPT_NOLIGHT    ライティングを行なわない
	GPOBJ_MATOPT_NOMIPMAP   MIPMAPを生成しない
	GPOBJ_MATOPT_NOCULL     カリングを無効にする
	GPOBJ_MATOPT_NOZTEST    Zテストを無効にする
	GPOBJ_MATOPT_NOZWRITE   Zバッファ書き込みを無効にする
	GPOBJ_MATOPT_BLENDADD   プレンドモードを加算に設定する
	GPOBJ_MATOPT_SPECULAR   光源計算時にスペキュラー(u_specularExponent)を適用します
	GPOBJ_MATOPT_MIRROR     反転した画像として表示する
	GPOBJ_MATOPT_CUBEMAP    キューブマップとして設定する
	GPOBJ_MATOPT_NODISCARD  αチャンネルによるピクセル破棄を無効にする
	GPOBJ_MATOPT_UVOFFSET	UVオフセット値(u_textureOffset)を指定可能にする
	GPOBJ_MATOPT_UVREPEAT	UVリピート値(u_textureRepeat)を指定可能にする
^p
正常にマテリアルが生成できなかった場合は、エラー3(パラメータの値が異常です)が発生します。
正しく生成された場合は、varで指定された変数にマテリアルID(整数値)が代入されます。

%href
gpcolormat
gpusermat
gpscrmat


%index
gpscrmat
オフスクリーンテクスチャマテリアルの生成
%group
拡張画面制御命令
%prm
var,id,opt
var     : 生成されたマテリアルIDが代入される変数名
id      : 参照されるオフスクリーンバッファID
opt(0)  : マテリアルオプション値
%inst
オフスクリーンテクスチャバッファを参照するマテリアルを生成します。
マテリアルを独自に生成することにより、オフスクリーンにレンダリングされた画像イメージをテクスチャとして再利用することが可能になります。
^p
	例:
	buffer id_render,512,512,screen_offscreen
	gpscrmat id_texmat, id_render, GPOBJ_MATOPT_NOLIGHT|GPOBJ_MATOPT_NOMIPMAP	; テクスチャマテリアル作成
	gpbox id_model, 1, , id_texmat		; 箱ノードを追加
^p
上の例では、buffer命令で作成されたオフスクリーンテクスチャバッファを参照する立方体のノードオブジェクトが生成されます。
optパラメーターにより、マテリアルの設定を変更することができます。
これらの設定は、gpmatstate命令で別途設定することも可能です。
^p
        マクロ名                内容
    -------------------------------------------------------------------
	GPOBJ_MATOPT_NOLIGHT    ライティングを行なわない
	GPOBJ_MATOPT_NOMIPMAP   MIPMAPを生成しない
	GPOBJ_MATOPT_NOCULL     カリングを無効にする
	GPOBJ_MATOPT_NOZTEST    Zテストを無効にする
	GPOBJ_MATOPT_NOZWRITE   Zバッファ書き込みを無効にする
	GPOBJ_MATOPT_BLENDADD   プレンドモードを加算に設定する
	GPOBJ_MATOPT_SPECULAR   光源計算時にスペキュラーを適用します
	GPOBJ_MATOPT_MIRROR     反転した画像として表示する
	GPOBJ_MATOPT_NODISCARD  αチャンネルによるピクセル破棄を無効にする
^p
正常にマテリアルが生成できなかった場合は、エラー3(パラメータの値が異常です)が発生します。
正しく生成された場合は、varで指定された変数にマテリアルID(整数値)が代入されます。

%href
gpcolormat
gpusermat


%index
gpusermat
カスタムマテリアルの生成
%group
拡張画面制御命令
%prm
var,"vsh","fsh","defs",color,opt
var       : 生成されたマテリアルIDが代入される変数名
"vsh"     : バーテックスシェーダーファイル名
"fsh"     : フラグメントシェーダーファイル名
"defs"    : 追加のラベル定義
color(-1) : マテリアルカラー(24bitRGB値)
opt(0)    : マテリアルオプション値
%inst
カスタムマテリアルを生成します。
カスタムマテリアルは、バーテックスシェーダー及びフラグメントシェーダーを指定した独自の描画を行ないます。
OpenGLのシェーダー言語(GLSL)で記述されたシェーダーファイル名を、"vsh"、"fsh"パラメーターに指定します。
"defs"パラメーターには、シェーダーコンパイル時に追加されるラベル定義を記述します。
colorパラメーターでデフォルトのカラーを設定することができます。省略した場合は、白色(0xffffff)が設定されます。
"vsh","fsh","defs"のパラメーターを省略した場合は、gpusershaderで設定された内容が設定されます。
optパラメーターにより、マテリアルの設定を変更することができます。
これらの設定は、gpmatstate命令で別途設定することも可能です。
^p
        マクロ名                内容
    -------------------------------------------------------------------
	GPOBJ_MATOPT_NOLIGHT    ライティングを行なわない
	GPOBJ_MATOPT_NOMIPMAP   MIPMAPを生成しない
	GPOBJ_MATOPT_NOCULL     カリングを無効にする
	GPOBJ_MATOPT_NOZTEST    Zテストを無効にする
	GPOBJ_MATOPT_NOZWRITE   Zバッファ書き込みを無効にする
	GPOBJ_MATOPT_BLENDADD   プレンドモードを加算に設定する
	GPOBJ_MATOPT_SPECULAR   光源計算時にスペキュラーを適用します
	GPOBJ_MATOPT_MIRROR     反転した画像として表示する
	GPOBJ_MATOPT_CUBEMAP    キューブマップとして設定する
	GPOBJ_MATOPT_NODISCARD  αチャンネルによるピクセル破棄を無効にする
^p
正常にマテリアルが生成できなかった場合は、エラー3(パラメータの値が異常です)が発生します。
正しく生成された場合は、varで指定された変数にマテリアルID(整数値)が代入されます。
^
カスタムマテリアルにより、任意のシェーダーを使って描画のすべてをコントロールすることができるようになります。
これは、シェーダーについての知識がある上級者向けの拡張機能ですので、通常は使用する必要はありません。
カスタムマテリアルを生成した場合は、gpmatprm命令によってシェーダーパラメーターに渡すための値を設定することができます。

%href
gpmatprm
gpcolormat
gptexmat
gpusershader
gpscrmat


%index
gpclone
ノードを複製
%group
拡張画面制御命令
%prm
var,objid,eventid
var         : 複製されたオブジェクトのIDが代入される変数名
objid(0)    : 複製元のオブジェクトID
eventid(-1) : イベントID
%inst
既に存在するオブジェクトと同じ設定で、新しいオブジェクトを生成します。
varで指定された変数に、複製されたオブジェクトのIDが代入されます。
同一の形状、パラメーター設定(物理設定を除く)を持ちますが座標や角度などを新しく設定することができます。
同一の物体を複数生成する場合に、設定や読み込みを何度も行なう手間を軽減し高速化にもつながります。
^p
	例:
	gpload id_model,"res/duck"	; モデル読み込み
	gpclone i, id_model		; 同じものをクローン
^p
上の例では、res/duck.gpbファイルによる3Dモデルノード(id_model)をクローンして変数iに新しいオブジェクトIDを代入します。
クローンやIDの割り当てに失敗した場合は、varで指定された変数に-1が代入されます。
イベントIDをeventidパラメーターに指定することで、クローン後のオブジェクトにイベントを適用することができます。eventidがマイナス値か、省略された場合はイベントは設定されません。
※現在のバージョンでは、シーン内に存在する表示可能なオブジェクト以外はクローンすることができませんので注意してください。
%href
gpdraw


%index
gpnull
ヌルノードを生成
%group
拡張画面制御命令
%prm
var
var : 生成されたオブジェクトIDが代入される変数名
%inst
空のノードオブジェクト(ヌルノード)を生成します。
varで指定された変数に、複製されたオブジェクトのIDが代入されます。
ヌルノードは、カメラやライトなどモデルを持つ必要のないノードを作成する場合などに使用します。
生成されたノードは、他のノードと同様に3D空間の中で自由に操作することが可能になります。
%href
gpcamera
gplight


%index
gpload
3Dモデルノードを生成
%group
拡張画面制御命令
%prm
var,"file","name"
var     : 生成されたオブジェクトIDが代入される変数名
"file"  : 読み込みを行なうgpbファイル名
"name"  : 読み込み対象となるノード名
"defs"  : 追加のラベル定義
%inst
ファイル化された3Dモデルデータ(.gpbファイル)をノードとして生成します。
varで指定された変数に、生成されたオブジェクトのIDが代入されます。
生成されたノードは3D空間の中で自由に操作することが可能になります。
"file"でフォルダ及びファイル名を指定します。「.gpb」などの拡張子は指定する必要ありません。
ファイルの中で特定の名前を持ったノードだけを読み込みたい場合は、"name"で指定することができます。
"name"の指定を省略した場合は、ファイルに含まれている階層構造をすべて読み込みノードとして生成します。
マテリアルに関する情報は、「.material」ファイルから読み込まれます。必ず「.gpb」ファイルとセットで用意するようにしてください。
"defs"パラメーターが指定された場合には、すべてのシェーダーコンパイル時に指定されたラベル定義が追加されます。
^
ノードに物理設定を行なう場合は、gppbind命令を使用してください。
3Dモデルデータの接触判定は、全体を包む球(バウンディング球)が使用されます。
^
生成されたオブジェクト(ノード)の描画はgpdraw命令によりまとめて行なわれます。
^p
	例:
	gpload id_model,"res/duck"		; モデル読み込み
^p
上の例では、resフォルダ内にあるduck.gpbファイルを読み込みます。
その際に、マテリアル情報が書かれているduck.materialファイルが参照されます。
さらに、テクスチャが必要な場合は、同じフォルダ内の画像ファイルも読み込まれます。ファイル読み込み時にエラーが発生した場合には、オブジェクトIDとしてマイナス値が代入されます。
さらに詳細な読み込み時のエラーやワーニングを調べたい場合は、gpgetlog命令によってログを収集することが可能です。

^
gpload命令で読み込むためのファイル(.gpb形式)は、gameplay3Dで標準的に使用されているデータ形式です。
HGIMG4では、.gpb形式を生成するためのGPBコンバーター(gpbconv.exe)を用意しています。
詳しくは、HGIMG4マニュアルを参照してください。
%href
gpgetlog
gpdraw
gppbind


%index
gpplate
板ノードを生成
%group
拡張画面制御命令
%prm
var,sizex,sizey,color,matobj
var        : 生成されたオブジェクトIDが代入される変数名
sizex(1)   : 生成されるXサイズ
sizey(1)   : 生成されるYサイズ
color(-1)  : マテリアルカラー(24bitRGB値)
matobj(-1) : マテリアルID
%inst
垂直に立った1枚の板(四角形)を3Dモデルデータとしてノードを生成します。
varで指定された変数に、生成されたオブジェクトのIDが代入されます。
生成されたノードは3D空間の中で自由に操作することが可能になります。
モデルのX,Yサイズをsizex,sizeyパラメーターで指定することができます。
colorパラメーターでモデルの色(マテリアルカラー)を24bitRGB値(0xRRGGBBで示される値)により指定します。
colorパラメーターが省略された場合は、白色(0xffffff)が使用されます。
matobjパラメーターで、マテリアルIDを指定することができます。
gpcolormat命令などにより、ユーザーが生成したマテリアルを使用する場合は指定してください。
matobjパラメーターが省略された場合は、標準のマテリアルが使用されます。
^
生成されたオブジェクト(ノード)の描画はgpdraw命令によりまとめて行なわれます。
^
ノードに物理設定を行なう場合は、gppbind命令を使用してください。
ノードの接触判定は、もとのモデルと同じ形状が使用されます。
%href
gpdraw
gppbind


%index
gpfloor
床ノードを生成
%group
拡張画面制御命令
%prm
var,sizex,sizey,color,matobj
var     : 生成されたオブジェクトIDが代入される変数名
sizex(1)   : 生成されるXサイズ
sizey(1)   : 生成されるYサイズ
color(-1)  : マテリアルカラー(24bitRGB値)
matobj(-1) : マテリアルID
%inst
1枚の床(四角形)を3Dモデルデータとしてノードを生成します。
varで指定された変数に、生成されたオブジェクトのIDが代入されます。
生成されたノードは3D空間の中で自由に操作することが可能になります。
モデルのX,Yサイズをsizex,sizeyパラメーターで指定することができます。
colorパラメーターでモデルの色(マテリアルカラー)を24bitRGB値(0xRRGGBBで示される値)により指定します。
colorパラメーターが省略された場合は、白色(0xffffff)が使用されます。
matobjパラメーターで、マテリアルIDを指定することができます。
gpcolormat命令などにより、ユーザーが生成したマテリアルを使用する場合は指定してください。
matobjパラメーターが省略された場合は、標準のマテリアルが使�0����