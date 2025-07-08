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
var,objid,mode,eventid
var         : 複製されたオブジェクトのIDが代入される変数名
objid(0)    : 複製元のオブジェクトID
mode(0)     : 追加モード値
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
mode値を設定することで、クローン後のオブジェクトが持つモード値に値を追加することができます(setobjmode命令と同様の指定になります)。mode値を省略するか0を指定した場合は、モードは追加されません。
イベントIDをeventidパラメーターに指定することで、クローン後のオブジェクトにイベントを適用することができます。eventidがマイナス値か、省略された場合はイベントは設定されません。
※現在のバージョンでは、シーン内に存在する表示可能なオブジェクト以外はクローンすることができませんので注意してください。
%href
gpdraw
setobjmode 


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
matobjパラメーターが省略された場合は、標準のマテリアルが使用されます。
^
生成されたオブジェクト(ノード)の描画はgpdraw命令によりまとめて行なわれます。
^
ノードに物理設定を行なう場合は、gppbind命令を使用してください。
(床モデルは標準で、同じ場所に固定された物理特性が設定されます)
ノードの接触判定は、もとのモデルと同じ形状が使用されます。
%href
gpdraw
gppbind


%index
gpbox
箱ノードを生成
%group
拡張画面制御命令
%prm
var,size,color,matobj
var     : 生成されたオブジェクトIDが代入される変数名
size(1)    : 生成されるサイズ
color(-1)  : マテリアルカラー(24bitRGB値)
matobj(-1) : マテリアルID
%inst
指定サイズの立方体を3Dモデルデータとしてノードを生成します。
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
gpspr
2Dスプライトノード生成
%group
拡張画面制御命令
%prm
var,bufid,celid,gmode
var      : 生成されたオブジェクトIDが代入される変数名
bufid(0) : バッファID
celid(0) : セルID
gmode(3) : コピーモード
%inst
2Dスプライトを表示するためのノードを生成します。
varで指定された変数に、生成されたオブジェクトのIDが代入されます。
生成されたノードは2Dスクリーンの中で自由に操作することが可能になります。
bufidパラメーターで表示元の画像が読み込まれているバッファIDを、celidパラメーターで表示するセルIDを指定します。
また、gmodeパラメーターでgcopy命令で指定するものと同様のコピーモードを指定します。
^p
	 gmode値  |  内容
	---------------------------------------------------
	    0,1      アルファチャンネル無効
	    2        アルファチャンネル有効
	    5        色加算・アルファチャンネル有効
^p
生成されたオブジェクト(ノード)の描画はgpdraw命令によりまとめて行なわれます。
^p
	; スプライト1個あたりの描画
	gmode [gmode設定値]
	pos [ノードX座標],[ノードY座標]
	celput [バッファID], [セルID], [ノードXスケール], [ノードYスケール], [ノードZ回転]
^p
つまり、2Dスプライトノードは上のような処理をあらかじめ登録した設定をまとめて行なう機能と考えることができます。
また、getcoli命令による2Dスプライトノード同士の接触判定を手軽に行なうことが可能です。
%href
gpdraw


%index
gplight
ライトノードを設定
%group
拡張画面制御命令
%prm
id,opt,range,inner,outer
id(0)     : オブジェクトのID
opt(0)    : ライト生成オプション
range(1)  : 影響範囲パラメーター
inner(0.5): 内側の減衰パラメーター
outer(1)  : 外側の減衰パラメーター
%inst
生成済みのノードにライトとしての機能を追加します。
idパラメーターで、ノードのオブジェクトIDを指定します。
optパラメーターでライト生成オプション値を指定することができます。
^p
	  opt値               |  内容
	--------------------------------------------------------------
	  GPOBJ_LGTOPT_NORMAL    平行光源(ディレクショナルライト)
	  GPOBJ_LGTOPT_POINT     点光源(ポイントライト)
	  GPOBJ_LGTOPT_SPOT      スポットライト
^p
rangeパラメーターは、ポイントライト及びスポットライトの影響範囲に関する設定を行ないます。
また、inner,outerパラメーターは、スポットライトが減衰する内側・外側のパラメーターを設定します。
^p
	例:
	gpnull id_alight			; ヌルノードを生成する
	gplight id_alight, GPOBJ_LGTOPT_NORMAL	; ライトとして登録する
	gpuselight id_alight			; デフォルトのライトに設定
^p
gplight命令は、あくまでもオブジェクトにライトとしての機能を設定するだけです。
設定されたライトは、gpuselight命令によりカレントライトとして登録することで、以降に生成されるモデル・マテリアルに反映されます。

%href
gpresetlight
gpuselight
gpnull

%index
gpcamera
カメラノードを設定
%group
拡張画面制御命令
%prm
id,fov,aspect,near,far,sw
id(0)      : オブジェクトのID
fov(45)    : 視野(FOV)パラメーター
aspect(1.5): アスペクト比
near(0.5)  : ニアクリップZ値
far(768)   : ファークリップZ値
sw(0)      : カメラタイプ値(0,1)
%inst
生成済みのノードにカメラとしての機能を追加します。
idパラメーターで、ノードのオブジェクトIDを指定します。
fovパラメーターで視野(FOV)を設定します。45を指定した場合は、45度の視野となります。
aspectパラメーターでアスペクト比(縦横比)を指定します。
また、near,farパラメーターにより近くと遠くのZ座標(クリッピングに使われるZ座標)を指定することができます。
^p
	例:
	gpnull id_camera			; ヌルノードを生成する
	gpcamera id_camera, 45, 1.5, 0.5, 768	; カメラとして設定する
	gpusecamera id_camera			; 使用するカメラを選択する
	setpos id_camera, 0,20,20		; カメラ位置を設定する
^p
カメラタイプ値(sw)に1を指定することにより、平行投影(Orthographic)を行うカメラを設定することが可能です。その場合は、fov値はズーム値(1.0が標準)として反映されます。

シーン内に配置されたカメラは、gpusecamera命令により切り替えることができます。

%href
gpusecamera
gpnull


%index
gplookat
指定座標に向けてノードを回転
%group
拡張画面制御命令
%prm
objid,x,y,z
objid(0)  : オブジェクトID
x(0)      : 目標となるX座標
y(0)      : 目標となるY座標
z(0)      : 目標となるZ座標
%inst
3Dノードを、指定した座標に向けて回転させます。
カメラに適用した場合は、指定座標を注視する角度に設定されます。
%href


%index
gppbind
ノードに標準的な物理特性を設定
%group
拡張画面制御命令
%prm
objid,mass,friction,option
objid(0)      : オブジェクトID
mass(1)       : 重さ
friction(0.5) : 摩擦係数
option(0)     : 設定オプション
%inst
ノードオブジェクトに対して、基本的な物理設定を行ないます。
massパラメーターは物体の重さ。frictionパラメーターは摩擦係数です。値を省略した場合は、デフォルト値が使用されます。
重さ(mass)を0にすることで静的剛体(衝突はするが、動かない)として設定されます。
それ以外の場合は、物理法則に従って床のある場所まで落下します。
^
gppbind命令は基本的な物理特性だけを設定します。詳細な物理パラメーターを設定する場合には、gppset命令を使用して設定を行ってください。
^
※物理挙動を設定する場合は、必ず床(地面)となる部分を作成しておいてください。
gpfloor命令によって生成された床に標準的な物理特性を設定することで、固定された床面を生成することができます。
床が存在しなかった場合、物体は永遠に落下を続けることになります。
^
物理設定を行なったノードオブジェクトは、それ以降自立して動作するようになり、setposなどの座標変更は無効となります。
物体に力を加えたい場合は、gppapply命令を使用してください。
^
optionパラメーターにより設定時のオプションを付加します。以下のマクロを指定することができます。
^p
	  opt値               |  内容
	--------------------------------------------------------------

	  GPPBIND_NOSCALE        コリジョンにノードのスケールを反映させない
	  GPPBIND_MESH           コリジョンとしてノードのモデルを反映させる
^p
ノードオブジェクトに物理設定を行なうと、接触判定を行う情報(コリジョン)が作成されます。

箱ノード、床ノード、板ノードはそれぞれの形状をコリジョンとして扱います。optionに、GPPBIND_NOSCALEを指定した場合は、スケールが反映されていないもともとの形状がコリジョンとなります。
3Dモデルノードは、標準ではモデル全体を覆うスフィア(球体)をコリジョンとして作成します。ただし、optionにGPPBIND_MESHを指定した場合は、モデルの形状そのものをコリジョンとして扱います。ただし、複雑なモデル形状をコリジョンにした場合、接触判定にかかる負荷が増大します。
また、モデルの形状をコリジョンとして扱う際には凹んだ部分を考慮する必要があります。凹型の部分は正しく判定されない場合があります。
コリジョンは、モデルの形状とは独立した情報となります。コリジョンが作成された後は、ノードのスケール値は反映されませんので注意してください。


%href
gppapply
gppset


%index
getwork2
ノードワーク値2を取得
%group
拡張画面制御命令
%prm
id,x,y,z
id      : オブジェクトID
(x,y,z) : 取得する変数
%inst
オブジェクトの持つwork2パラメーターを取得します。
(x,y,z)は、実数型の変数として設定されます。
命令の最後に「i」を付加することで、整数値として値を取得することができます。
%href
getwork2i


%index
getwork2i
ノードワーク値2を取得(整数値)
%group
拡張画面制御命令
%prm
id,x,y,z
id      : オブジェクトID
(x,y,z) : 取得する変数
%inst
オブジェクトの持つwork2パラメーターを取得します。
(x,y,z)は、整数型の変数として設定されます。
getwork2命令を使用することで、実数値として値を取得することができます。
%href
getwork2


%index
getcolor
オブジェクトカラー値を取得
%group
拡張画面制御命令
%prm
id,x,y,z
id      : オブジェクトID
(x,y,z) : 取得する変数
%inst
オブジェクトの持つcolorパラメーターを取得します。
(x,y,z)は、実数型の変数として設定されます。
命令の最後に「i」を付加することで、整数値として値を取得することができます。
%href
getcolori


%index
getcolori
オブジェクトカラー値を取得(整数値)
%group
拡張画面制御命令
%prm
id,x,y,z
id      : オブジェクトID
(x,y,z) : 取得する変数
%inst
オブジェクトの持つcolorパラメーターを取得します。
(x,y,z)は、整数型の変数として設定されます。
getcolor命令を使用することで、実数値として値を取得することができます。
%href
getcolor


%index
setcolor
オブジェクトカラー値を設定
%group
拡張画面制御命令
%prm
id,x,y,z
id      : オブジェクトID
(x,y,z) : 設定する値 (デフォルト=0)
%inst
オブジェクトの持つパラメーターを設定します。
colorグループ(カラー値)に(x,y,z)で指定された値を設定します。
(x,y,z)には、実数または整数値を指定することができます。
%href
getcolor
addcolor


%index
addcolor
オブジェクトカラー値を加算
%group
拡張画面制御命令
%prm
id,x,y,z
id      : オブジェクトID
(x,y,z) : 設定する値 (デフォルト=0)
%inst
オブジェクトの持つパラメーターを加算します。
colorグループ(カラー値)に(x,y,z)で指定された値を加算します。
(x,y,z)には、実数または整数値を指定することができます。
%href
getcolor
setcolor




%index
selquat
オブジェク回転情報をMOC情報に設定
%group
拡張画面制御命令
%prm
id
id(0) : オブジェクトID
%inst
MOC設定命令の対象となるMOCグループをquat(クォータニオン回転情報)に設定します
idは、オブジェクトIDとなります。
%href


%index
selwork2
オブジェクトワーク2をMOC情報に設定
%group
拡張画面制御命令
%prm
id
id(0) : オブジェクトID
%inst
MOC設定命令の対象となるMOCグループをwork2(ワーク2)に設定します
idは、オブジェクトIDとなります。
%href


%index
setwork2
work2グループ情報を設定
%group
拡張画面制御命令
%prm
id,x,y,z
id      : オブジェクトID
(x,y,z) : 設定する値 (デフォルト=0)
%inst
オブジェクトの持つパラメーターを設定します。
work2グループ(ワーク2値)に(x,y,z)で指定された値を設定します。
(x,y,z)には、実数または整数値を指定することができます。
%href
setwork


%index
addwork2
work2グループ情報を加算
%group
拡張画面制御命令
%prm
id,x,y,z
id      : オブジェクトID
(x,y,z) : 設定する値 (デフォルト=0)
%inst
オブジェクトの持つパラメーターを設定します。
work2グループ(ワーク2値)に(x,y,z)で指定された値を加算します。
(x,y,z)には、実数または整数値を指定することができます。
%href
addwork


%index
gpcnvaxis
3D座標の変換を行なう
%group
拡張画面制御命令
%prm
var_x,var_y,var_z,x,y,z,mode
var_x   : X値が代入される変数
var_y   : Y値が代入される変数
var_z   : Z値が代入される変数
x(0.0)  : 変換元のX値
y(0.0)  : 変換元のY値
z(0.0)  : 変換元のZ値
mode(0) : 変換モード
%inst
決められたモードに従って、(x,y,z)の3D座標を変換します。
結果は、var_x,var_y,var_zで指定された変数に実数型で代入されます。(変数型は自動的に設定されます)
モード値による変換の内容は以下の通りです。
^p
モード  内容
-----------------------------------------------
0       描画される2D座標(X,Y)位置+Zバッファ値
1       0と同じだが(X,Y)座標が正規化されたもの
2       ビュー(カメラから見た状態)変換を行なった(X,Y,Z)座標
3       ワールド(カメラを含まない)変換を行なった(X,Y,Z)座標
^p
モード0と1は、スクリーン上に2D投影を行なった際のX,Y座標、及びZバッファ値に変換します。
モード1の場合は、(X,Y)座標が0〜1.0の値に正規化されます。モード0で計算される(X,Y)座標はスクリーン全体の座標ではなく、gpviewportで設定された座標系に変換されますので注意してください。
モード2では、カメラ位置を考慮したビュー変換を行なったX,Y,Z座標値に変換します。
モード3では、カメラ位置を考慮しないワールド変換を行なったX,Y,Z座標値に変換します。
%href
gpviewport


%index
gppset
ノードの物理パラメーターを設定
%group
拡張画面制御命令
%prm
objid,prmid,x,y,z
objid(0)  : オブジェクトID
prmid(0)  : パラメーターID
x(0.0)    : 設定パラメーターX値
y(0.0)    : 設定パラメーターY値
z(0.0)    : 設定パラメーターZ値
%inst
objidで指定されたノードの詳細な物理パラメーターを設定します。
prmidパラメーターで、設定する項目を指定し、X,Y,Zに指定された値が、それぞれの項目に対応した値として適用されます。
^p
        マクロ名                          内容
    ----------------------------------------------------------------------------
	GPPSET_ENABLE                     X=物理挙動のON/OFF(0=無効)
	GPPSET_FRICTION                   X=摩擦係数, Y=弾力設定
	GPPSET_DAMPING                    X=linear反発(0〜1.0), Y=angular反発(0〜1.0)
	GPPSET_KINEMATIC                  X=Kinematic設定のON/OFF(0=無効)
	GPPSET_ANISOTROPIC_FRICTION       X,Y,Z=異方性の摩擦
	GPPSET_GRAVITY                    X,Y,Z=重力
	GPPSET_LINEAR_FACTOR              X,Y,Z=ベクトルに沿った移動の設定
	GPPSET_ANGULAR_FACTOR             X,Y,Z=ベクトルに沿った回転の設定
	GPPSET_ANGULAR_VELOCITY           X,Y,Z=回転ベロシティー
	GPPSET_LINEAR_VELOCITY            X,Y,Z=リニアベロシティー
	GPPSET_MASS_CENTER                X,Y,Z=中心座標のオフセット
^p
必ずgppbind命令で基本的な物理特性を設定した後で詳細設定を行なうようにしてください。
設定値が正しくない場合は、エラーが発生します。
%href
gppbind


%index
gpobjpool
オブジェクトID生成の設定
%group
拡張画面制御命令
%prm
start,num
start(0) : 割り当て開始番号
num(-1)  : 最大割り当て数
%inst
オブジェクトIDの割り当て方法を変更します。
オブジェクトIDは、あらかじめ決められた最大数の中で、使用されていないID番号が割り当てられます。
オブジェクト最大数は、デフォルトで1024個となっており、sysreq命令によって拡張することが可能です。
^p
	例:
	setreq SYSREQ_MAXOBJ,4096	; オブジェクト最大数を4096に拡張する
^p
gpobjpool命令は、ID番号の割り当て範囲を任意に変更することができます。
startパラメーターで指定された番号を割り当て開始番号に設定します。
numパラメーターで指定された番号を割り当てられる最大数に設定します。
numパラメーターがマイナス値か省略されていた場合は、割り当て開始番号から最大限取れる範囲が設定されます。
^p
	例:
	gpobjpool 100,50	; オブジェクトID生成の設定変更
^p
上の例では、ID番号100〜150のみを割り当て対象に変更します。
これ以降のオブジェクト生成命令(gpboxやgpnullなど)では、指定された範囲内のオブジェクトIDのみが割り当てられます。
オブジェクト全体の中で特定の個数までしか生成されないように制限したい場合や、
表示順序をある程度コントロールしたい場合に利用できます。
gpobjpool命令によって、あらかじめ設定されたオブジェクト最大数を拡張することはできません。
必ず最大数の範囲内で設定を行なうよう注意してください。
%href
delobj


%index
gppapply
ノードに物理的な力を適用する
%group
拡張画面制御命令
%prm
objid,action,x,y,z
objid(0)  : オブジェクトID
action(0) : 力の種類(タイプ)
x(0.0)    : 設定パラメーターX値
y(0.0)    : 設定パラメーターY値
z(0.0)    : 設定パラメーターZ値
%inst
ノードに対して(x,y,z)で指定したベクトルの物理的な力を適用します。
actionパラメーターで設定できるタイプは、以下の通りです。
^p
        マクロ名                     内容
    ----------------------------------------------------------------------------
	GPPAPPLY_FORCE               移動しようとする力を加算する
	GPPAPPLY_IMPULSE             瞬間的な衝撃を与える
	GPPAPPLY_TORQUE              トルク(ねじる)力を与える
	GPPAPPLY_TORQUE_IMPULSE      トルク+衝撃を与える
	GPPAPPLY_FORCE_POS           移動しようとする力を加算する(座標指定あり)
	GPPAPPLY_IMPULSE_POS         瞬間的な衝撃を与える(座標指定あり)
^p
GPPAPPLY_FORCE_POS、GPPAPPLY_IMPULSE_POSを指定した場合はオブジェクトIDで指定されたモデルが持つWORK値(setwork命令などで設定される)を座標値として使用して力を適用します。
この命令は、必ずgppbind命令で基本的な物理特性を設定した後で力を適用するようにしてください。
設定値が正しくない場合は、エラーが発生します。
%href
gppbind


%index
gpmatprm1
マテリアルのパラメーター設定(1)
%group
拡張画面制御命令
%prm
id,"name",value
id(0)      : マテリアルID/オブジェクトID
"name"     : パラメーター名
value(0.0) : Xの設定値(実数値)
%inst
生成されたマテリアルのシェーダーパラメーターを設定します。
gpmatprm命令と基本的に同じ機能です。
gpmatprm1命令は、1項目のみの実数パラメーターを指定します。
%href
gpmat
gpmatprm
gpmatprm2
gpmatprm4
gpmatprm16


%index
gpmatprm2
マテリアルのパラメーター設定(2)
%group
拡張画面制御命令
%prm
id,"name",value,value2
id(0)      : マテリアルID/オブジェクトID
"name"     : パラメーター名
value(0.0) : Xの設定値(実数値)
value2(0.0) : Yの設定値(実数値)
%inst
生成されたマテリアルのシェーダーパラメーターを設定します。
gpmatprm命令と基本的に同じ機能です。
gpmatprm2命令は、2項目の実数パラメーターを指定します。
%href
gpmat
gpmatprm
gpmatprm1
gpmatprm4
gpmatprm16


%index
gpmatprm4
マテリアルのパラメーター設定(4)
%group
拡張画面制御命令
%prm
id,"name",x,y,z,w
id(0)  : マテリアルID/オブジェクトID
"name" : パラメーター名
x(0.0) : Xの設定値(実数値)
y(0.0) : Yの設定値(実数値)
z(0.0) : Zの設定値(実数値)
w(0.0) : Wの設定値(実数値)
%inst
生成されたマテリアルのシェーダーパラメーターを設定します。
gpmatprm命令と基本的に同じ機能です。
gpmatprm4命令は、4項目(x,y,z,w)の実数パラメーターを指定します。
%href
gpmat
gpmatprm
gpmatprm1
gpmatprm2
gpmatprm16


%index
setalpha
オブジェクトの透明度(α値)設定
%group
拡張画面制御命令
%prm
objid,value
objid(0)   : オブジェクトID
value(255) : 設定する値(整数値)(0〜255)
%inst
オブジェクトの透明度(α値)を設定します。
valueパラメーターで指定された値を、objidパラメーターのオブジェクトに設定します。
valueパラメーターで指定する値は、透明度を示す0〜255の整数値(α値)となります。
0は完全な透明、255は不透明が適用されます。
α値が255以外(半透明)の状態となったオブジェクトは、描画順序が前面に変更されます。
(OBJ_LATEのモードフラグ値が指定されたのと同じ状態)
%href
gpsetprm


%index
gpgetlog
HGIMG4エラーログを取得
%group
拡張画面制御命令
%prm
var
var   : エラーログが代入される変数
%inst
HGIMG4のエラーログを取得して変数に代入します。変数は文字列型として初期化されます。
エラーログは、gpreset、gpload命令などの実行時に発生したエラーの詳細を記録するもので、原因の調査などに利用することができます。

%href
gpload
gpreset


%index
gpaddanim
アニメーションクリップを追加
%group
拡張画面制御命令
%prm
objid,"name",start,end,option
objid(0)  : オブジェクトID
"name"    : アニメーションクリップ名
start(0)  : スタートフレーム(ミリ秒)
end(-1)   : 終了フレーム(ミリ秒)
option(0) : 追加オプション
%inst
読み込まれた3Dモデルが持つアニメーションデータをもとに、新規のアニメーションクリップを追加します。
アニメーションクリップは、3Dモデルのアニメーションデータから一部のフレームを切り出したものに名前を付けて管理するものです。
あらかじめ、gpload命令によりgpbファイルからアニメーションデータを含むモデルデータを読み込んでおく必要があります。
"name"パラメーターで、アニメーションクリップの名前を指定します。既に追加されている名前は指定できません。
start,endパラメーターでアニメーションクリップのフレーム範囲を指定します。これらはミリ秒単位の時間指定となります。
endパラメーターを省略するかマイナス値を指定した場合は、アニメーション全体の最後にあたるフレームが適用されます。

処理が正常に終了した場合は、システム変数statに0が代入されます。エラーが発生した場合は、システム変数statはマイナス値が代入されます。

%href
gpgetanim
gpsetanim
gpact


%index
gpact
アニメーションクリップを再生/停止
%group
拡張画面制御命令
%prm
objid,"name",option
objid(0)  : オブジェクトID
"name"(""): アニメーションクリップ名
option(1) : 再生オプション
%inst
指定された名前のアニメーションクリップの再生/停止などをコントロールします。
アニメーションが設定されているモデルのオブジェクトIDを指定し、"name"でアニメーションクリップを選択します。
アニメーションクリップはあらかじめgpaddanim命令で設定しておくことができます。また、"name"の指定を省略するか、""(空文字)の場合はデフォルトのアニメーションクリップとなります。
optionの値で再生のコントロールを行ないます。
^p
        マクロ名              値          内容
    ----------------------------------------------------------------------------
	GPACT_STOP            0           停止
	GPACT_PLAY            1           開始
	GPACT_PAUSE           2           一時停止
^p
アニメーションの再生は、アニメーションクリップごとに制御されます。
1つのモデルに対して同時に複数のアニメーションクリップを再生することが可能です。
アニメーションクリップ再生の状態を取得・設定するための命令として、gpgetanim,gpsetanimが用意されています。
処理が正常に終了した場合は、システム変数statに0が代入されます。エラーが発生した場合は、システム変数statはマイナス値が代入されます。

%href
gpaddanim
gpgetanim
gpsetanim


%index
gpgetanim
アニメーションクリップ設定を取得
%group
拡張画面制御命令
%prm
var,objid,index,prmid
var   : 情報が代入される変数
objid(0) : オブジェクトID
index(0) : アニメーションクリップのインデックス(0〜)
prmid(0) : パラメーターID
%inst
指定されたアニメーションクリップの設定を取得してvarで指定された変数に代入します。
objidで設定を取得するオブジェクトIDを、indexにアニメーションクリップのインデックスを指定します。
アニメーションクリップのインデックスは、オブジェクトが保持しているアニメーションクリップに順番に割り振られる番号です。0,1,2,3…のような0から始まる整数値で、存在しないインデックスが指定された場合はシステム変数に-1(エラー)が代入されます。
prmidでどのような情報を取得するかを指定します。prmidで指定できる値は以下の通りです。
^p
        マクロ名                   値          内容
    ----------------------------------------------------------------------------
	GPANIM_OPT_START_FRAME     0           開始フレーム(ミリ秒単位)
	GPANIM_OPT_END_FRAME       1           終了フレーム(ミリ秒単位)
	GPANIM_OPT_DURATION        2           再生の長さ(ミリ秒単位)
	GPANIM_OPT_ELAPSED         3           経過時間(ミリ秒単位)
	GPANIM_OPT_BLEND           4           ブレンド係数(%単位)
	GPANIM_OPT_PLAYING         5           再生中フラグ(0=停止/1=再生)
	GPANIM_OPT_SPEED           6           再生スピード(%単位)
	GPANIM_OPT_NAME            16          アニメーションクリップ名
^p
取得される変数は適切な型で初期化されます。GPANIM_OPT_NAMEの場合は、文字列型となります。
処理が正常に終了した場合は、システム変数statに0が代入されます。エラーが発生した場合は、システム変数statはマイナス値が代入されます。
%href
gpaddanim
gpsetanim


%index
gpsetanim
アニメーションクリップ設定を更新
%group
拡張画面制御命令
%prm
objid,index,prmid,value
objid(0) : オブジェクトID
index(0) : アニメーションクリップのインデックス(0〜)
prmid(0) : パラメーターID
value(0) : 設定される値(整数値)
%inst
指定されたアニメーションクリップの設定をvalueで指定される新しい値で更新します。
objidで設定を取得するオブジェクトIDを、indexにアニメーションクリップのインデックスを指定します。
アニメーションクリップのインデックスは、オブジェクトが保持しているアニメーションクリップに順番に割り振られる番号です。0,1,2,3…のような0から始まる整数値で、存在しないインデックスが指定された場合はシステム変数に-1(エラー)が代入されます。
prmidでどのような情報を設定するかを指定します。prmidで指定できる値は以下の通りです。
^p
        マクロ名                   値          内容
    ----------------------------------------------------------------------------
	GPANIM_OPT_DURATION        2           再生の長さ(ミリ秒単位)
	GPANIM_OPT_BLEND           4           ブレンド係数(%単位)
	GPANIM_OPT_SPEED           6           再生スピード(%単位)
^p
処理が正常に終了した場合は、システム変数statに0が代入されます。エラーが発生した場合は、システム変数statはマイナス値が代入されます。
%href
gpaddanim
gpgetanim


%index
gpmatprm16
マテリアルのパラメーター設定(マトリクス)
%group
拡張画面制御命令
%prm
id,"name",var
id(0)      : マテリアルID/オブジェクトID
"name"     : パラメーター名
var        : 値が代入された配列変数名(実数型)
count(1)   : 設定されるマトリクスの個数
%inst
生成されたマテリアルのシェーダーパラメーターを設定します。
gpmatprm命令と基本的に同じ機能ですが、gpmatprm16命令は、varで指定された実数型の配列変数に格納された数値を4×4のマトリクス行列として設定します。
var(0)〜var(15)の16個の実数がマトリクス行列となります。countを指定した場合は、その個数だけ配列変数から値を取り出します。
マトリクスの個数を2以上に設定する場合は、必ずマテリアルIDを指定するようにしてください。
また、現在のバージョンでは2個以上のマトリクスは1つのマテリアルにつき1種類までの制限がありますのでご注意ください。マトリクスの個数が1であれば、制限なくパラメーターを設定することができます。
%href
gpmatprm
gpmatprm1
gpmatprm2
gpmatprm4


%index
gpmatprmt
マテリアルのパラメーター設定(テクスチャファイル)
%group
拡張画面制御命令
%prm
id,"name","filename"
id(0)      : マテリアルID/オブジェクトID
"name"     : パラメーター名
"filename" : 画像ファイル名
opt(0)     : マテリアルオプション値
%inst
生成されたマテリアルのシェーダーパラメーターを設定します。
gpmatprmt命令は、"filename"で指定されたファイルをテクスチャ画像としてシェーダーパラメーターに設定します。
あらかじめテクスチャが割り当てられているマテリアルを後から変更する際に使用してください。
この命令は、sampler2D型のパラメーターをシェーダーに渡す場合に使用することができます。これにより、マルチテクスチャを独自のシェーダーで扱うことが可能です。
パラメーター名を省略、または空文字("")にした場合は、HGIMG4の標準的なシェーダーパラメーター名「u_diffuseTexture」が使用されます。
optパラメーターに、GPOBJ_MATOPT_NOMIPMAPを指定した場合は、MIPMAPを生成しません。
また、optパラメーターに、GPOBJ_MATOPT_CUBEMAPを指定した場合は、テクスチャをキューブマップとして扱います。
キューブマップは、6面方向の画像をまとめた特殊な形式で環境マップやスカイボックスなどに利用することができます。
(キューブマップ画像は、+X,-X,+Y,-Y,+Z,-Z放送の6画像を縦に連結した1枚の.PNG形式を使用してください。)
既に読み込まれているマテリアルIDを使用してパラメーター設定を行う場合は、gpmatprmp命令をご使用ください。
また、テクスチャ設定を行うgpmatprmf命令も用意されています。
%href
gpmatprm
gpmatprm1
gpmatprm2
gpmatprm4
gpmatprm16
gpmatprmp
gpmatprmf


%index
gpmatprmp
マテリアルのパラメーター設定(マテリアルID)
%group
拡張画面制御命令
%prm
id,"name",matobj
id(0)      : マテリアルID/オブジェクトID
"name"     : パラメーター名
matobj(0)  : マテリアルID
%inst
生成されたマテリアルのシェーダーパラメーターを設定します。
gpmatprmp命令は、matobjで指定されたマテリアルIDが持つテクスチャをシェーダーパラメーターとして設定します。
この命令は、sampler2D型のパラメーターをシェーダーに渡す場合に使用することができます。これにより、マルチテクスチャを独自のシェーダーで扱うことが可能です。
パラメーター名を省略、または空文字("")にした場合は、HGIMG4の標準的なシェーダーパラメーター名「u_diffuseTexture」が使用されます。
マテリアルIDの替わりにオフスクリーンとして登録されている画面バッファIDを指定することができます。「GPOBJ_ID_SRCFLAG+画面バッファID」の形で指定することで画面バッファをテクスチャとして渡すことが可能です。

テクスチャファイルを使用してパラメーター設定を行う場合は、gpmatprmt命令をご使用ください。
%href
gpmatprm
gpmatprm1
gpmatprm2
gpmatprm4
gpmatprm16
gpmatprmt
gpmatprmf


%index
gpusershader
ユーザーシェーダーの指定
%group
拡張画面制御命令
%prm
"vsh","fsh","defs"
"vsh"     : バーテックスシェーダーファイル名
"fsh"     : フラグメントシェーダーファイル名
"defs"    : 追加のラベル定義
%inst
ユーザーが独自に設定するためのシェーダーを指定します。
バーテックスシェーダー、フラグメントシェーダー及び追加のラベル定義を記述して、使用することができます。
指定されたシェーダーは、gpusermatのデフォルト値として、buffer命令によるスクリーンバッファのカスタムシェーダー指定時に参照されます。
%href
buffer
gpusermat


%index
gpgetmat
マテリアルIDの取得
%group
拡張画面制御命令
%prm
var,id,opt
var    : マテリアルIDが代入される変数
id(0)  : 参照されるID
opt(0) : 取得オプション
%inst
指定されたオブジェクト、及び画面バッファで使用されているカスタムマテリアルのIDを取得します。
optパラメーターで取得オプションを指定します。optで指定できる値は以下の通りです。
^p
    マクロ名                   値     内容
    ----------------------------------------------------------------------------
	GPGETMAT_OPT_OBJMAT        0      オブジェクトが持つカスタムマテリアル
	GPGETMAT_OPT_SCRMAT        1      画面バッファが持つカスタムマテリアル
^p
GPGETMAT_OPT_OBJMATを指定した場合は、idパラメーターにオブジェクトIDを指定することで、オブジェクトが持つカスタムマテリアルのIDを取得することができます。
GPGETMAT_OPT_SCRMATを指定した場合は、idパラメーターに画面バッファID(ウインドウID)を指定することで、それぞれの画面バッファが持つカスタムマテリアルのIDを取得することができます。


%href
buffer
gpmatprm
gpmatprm1
gpmatprm2
gpmatprm4
gpmatprm16


%index
gpmatprmf
マテリアルのテクスチャ詳細設定
%group
拡張画面制御命令
%prm
id,"name","value","prmname"
id(0)       : マテリアルID/オブジェクトID
"name"("")  : 設定名
"value"("") : 設定する値
"prmname"("") : パラメーター名
%inst
マテリアルに設定されているテクスチャ設定を変更します。
既にテクスチャが設定されているマテリアルID、またはオブジェクトIDを指定してフィルターモードやテクスチャラップモードを変更することが可能です。
idパラメーターにテクスチャ設定を行うマテリアルID、またはオブジェクトIDを指定します。マテリアルID、やオブジェクトIDが間違っていたり、テクスチャが設定されていないマテリアルを指定した場合はエラーが発生します。
"name"で指定した設定名に、"value"で指定した値を設定します。これらの指定は文字列で行います。
設定は、以下の文字列が認識されます。(大文字小文字は同一に扱われます)
^p
    設定名           値          内容
    ----------------------------------------------------------------------------
    filter           nearest     フィルターモードをNEARESTに設定する
    filter           linear      フィルターモードをLINEARに設定する
    warp             repeat      ラップモードをREPEATに設定する
    warp             clamp       ラップモードをCLAMPに設定する
^p
filterという設定名は、テクスチャ拡大縮小時のフィルターを設定します。nearestは最も近いピクセルを使用し、linearはリニア補間が適用されます。
wrapという設定名は、テクスチャUV値が0〜1を超えた時の設定になります。repeatは繰り返し、clampは切り抜きが適用されます。
^
"prmname"でシェーダーパラメーター名を指定することができます。パラメーター名を省略、または空文字("")にした場合は、HGIMG4の標準的なシェーダーパラメーター名「u_diffuseTexture」が使用されます。
設定が正しく行われた場合は、システム変数に0が代入されます。何らかの問題が発生し設定されなかった場合は、マイナス値が代入されます。
マテリアルのステート情報を設定する場合は、gpmatstate命令を使用してください。
%href
gpmatprm
gpmatprm1
gpmatprm2
gpmatprm4
gpmatprm16
gpmatprmp
gpmatprmf
gpmatstate


%index
setquat
quatグループ情報を設定
%group
拡張画面制御命令
%prm
id,x,y,z,w
id      : オブジェクトID
(x,y,z,w) : 設定する値 (デフォルト=0)

%inst
オブジェクトの持つ回転パラメーターを設定します。
quatグループ(クォータニオン)に(x,y,z,w)で指定された値を設定します。
(x,y,z,w)は、クォータニオン(四元数)による設定を行ないます。HGIMG4内部では、すべての回転情報は、クォータニオンにより保持されています。

%href
getquat


%index
getquat
quatグループ情報を取得
%group
拡張画面制御命令
%prm
id,x,y,z,w
id      : オブジェクトID
(x,y,z,w) : 取得する変数

%inst
オブジェクトの持つ回転パラメーターを取得します。
quatグループ(クォータニオン)の内容が(x,y,z,w)で指定された変数に代入されます。
(x,y,z,w)は、実数型の変数として設定されます。
HGIMG4内部では、すべての回転情報は、クォータニオンにより保持されています。
角度情報に変換する場合は、getangまたはgetangr命令をご使用ください。
ただし、回転情報を完全に再現できない場合がありますので、角度情報に変換する場合は注意してください。

%href
getang
getangr
setquat



%index
event_suicide
オブジェクト破棄イベントを追加
%group
拡張画面制御命令
%prm
p1
p1    : イベントID
%inst
※この命令は今後廃止される予定です、event_delobjと置き換えてご使用ください
idで指定しているイベントIDに、オブジェクト破棄イベントを追加します。
この命令は、event_delobjと同等に使用することができます。
%href
event_delobj


%index
gpsetprmon
オブジェクトのコアパラメーター設定(ビット追加)
%group
拡張画面制御命令
%prm
objid,prmid,value
objid(0) : オブジェクトID
prmid(0) : コアパラメーターID
value(0) : 設定する値(整数値)
%inst
オブジェクトごとに保持されているコアパラメーターの値を、valueで設定されているビットのみONに設定します。
(もともとのコアパラメーター値にvalueの値をORで合成します)
これは、モードフラグ値などビット単位で設定が必要なパラメーターに、新しいビットを追加する場合などに使用することができます。

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
%href
gpgetprm
gpsetprm
gpsetprmoff



%index
gpsetprmoff
オブジェクトのコアパラメーター設定(ビット削除)
%group
拡張画面制御命令
%prm
objid,prmid,value
objid(0) : オブジェクトID
prmid(0) : コアパラメーターID
value(0) : 設定する値(整数値)
%inst
オブジェクトごとに保持されているコアパラメーターの値を、valueで設定されているビットのみOFFに設定します。
(もともとのコアパラメーター値からvalueで指定されたビットを消去します)
これは、モードフラグ値などビット単位で設定が必要なパラメーターから、特定のビットを削除する場合などに使用することができます。

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
%href
gpgetprm
gpsetprm
gpsetprmon



%index
setangy
angグループ情報を設定
%group
拡張画面制御命令
%prm
id,x,y,z
id      : オブジェクトID
(x,y,z) : 設定する値 (デフォルト=0)

%inst
setang命令と同様の機能を持っていますが、回転の順番がY->X->Zとなります。

%href
setang
setangz
setangr


%index
setangz
angグループ情報を設定
%group
拡張画面制御命令
%prm
id,x,y,z
id      : オブジェクトID
(x,y,z) : 設定する値 (デフォルト=0)

%inst
setang命令と同様の機能を持っていますが、回転の順番がZ->Y->Xとなります。

%href
setang
setangy
setangr


%index
event_angy
angグループ変化イベントを追加
%group
拡張画面制御命令
%prm
id,frame,x1,y1,z1,sw
id         : イベントID
frame      : 変化までのフレーム数
(x1,y1,z1) : 設定される値
sw(0)      : 補間オプション

%inst
event_ang命令と同様の機能を持っていますが、回転の順番がY->X->Zとなります。

%href
event_ang
event_angz
event_angr


%index
event_angz
angグループ変化イベントを追加
%group
拡張画面制御命令
%prm
id,frame,x1,y1,z1,sw
id         : イベントID
frame      : 変化までのフレーム数
(x1,y1,z1) : 設定される値
sw(0)      : 補間オプション

%inst
event_ang命令と同様の機能を持っていますが、回転の順番がZ->Y->Xとなります。

%href
event_ang
event_angy
event_angr


%index
event_setangy
angグループ設定イベントを追加
%group
拡張画面制御命令
%prm
id,x1,y1,z1,x2,y2,z2
id         : イベントID
(x1,y1,z1) : 設定される値(下限値)
(x2,y2,z2) : 設定される値(上限値)
%inst
event_setang命令と同様の機能を持っていますが、回転の順番がY->X->Zとなります。

%href
event_setang
event_setangz
event_setangr


%index
event_setangz
angグループ設定イベントを追加
%group
拡張画面制御命令
%prm
id,x1,y1,z1,x2,y2,z2
id         : イベントID
(x1,y1,z1) : 設定される値(下限値)
(x2,y2,z2) : 設定される値(上限値)
%inst
event_setang命令と同様の機能を持っていますが、回転の順番がZ->Y->Xとなります。

%href
event_setang
event_setangy
event_setangr


%index
gpresetlight
カレントライトの初期化
%group
拡張画面制御命令
%prm
p1,p2,p3
p1=1〜9(1) : ディレクショナルライトの個数
p2=0〜9(0) : ポイントライトの個数
p3=0〜9(0) : スポットライトの個数
%inst
使用するライトの種別とともにカレントライトを初期化します。
カレントライトを設定しておくことで、以降に生成されたモデルやマテリアルが受けるライトの詳細を決めることができます。
シーンが初期化された状態では、ディレクショナルライト1個だけを設定できる状態になっています。複数のライトや、ポイントライト、スポットライトなどを使用する場合は、gpresetlight命令によってカレントライトの個数をあらかじめ決めておく必要があります。
p1,p2,p3パラメーターで、それぞれのライト種別が使用するライトの最大個数を指定します。ディレクショナルライトは最低でも1つ使用する必要があります。
それ以外のポイントライト、スポットライトは必要に応じて指定してください。GPUの計算負荷につながるため、できるだけ不要なライトは指定しないようにしてください。
カレントライトが初期化された後は、gpuselight命令により登録することができます。

%href
gpuselight
gplight


%index
setobjlight
オブジェクトにカレントライトを設定
%group
拡張画面制御命令
%prm
id
id      : オブジェクトID
%inst
idで指定したオブジェクトにカレントライトの設定を行います。
idで指定したオブジェクトは、光源計算を行う3Dモデルである必要があります。
それまでオブジェクトに設定されていたライトの設定は破棄され、新しくカレントライトの設定が上書きされます。
ただし、3Dモデルが生成された時点のカレントライトの設定(ディレクショナルライト、ポイントライト、スポットライトの個数)は変更されませんので注意してください。(必ず、カレントライトに含まれるライトの個数が同一のものを設定するようにしてください。)
%href
gpuselight
gpresetlight


%index
gppcontact
オブジェクトの物理衝突情報を作成する
%group
拡張画面制御命令
%prm
var,objid
var      : 結果が代入される変数
objid(0) : オブジェクトID
%inst
物理設定を行なった3Dオブジェクトの正確な衝突の情報を作成します。
objidで指定された3Dオブジェクトが他のオブジェクトに衝突した際の情報をすべて作成します。
objidで指定された3Dオブジェクトは、必ずgppbind命令により物理設定を行っておく必要があります。
また、setcoli命令により衝突を検知する対象となるオブジェクトのコリジョングループを適切に設定しておく必要があります。
varで指定された変数には衝突情報が作成された個数が整数値として代入されます。
エラーが発生した場合は、マイナス値が代入されます。
衝突情報は複数作成されることがあります。これは、複数のオブジェクトと同時に衝突することがあるためです。変数に0が代入された場合は、衝突しているオブジェクトがないことを示します。
実際の衝突情報は、gppinfo命令により取得することができます。衝突情報として、衝突した対象のオブジェクトID、衝突した座標、衝突の強さなどを取得することができます。
衝突情報はオブジェクトIDごとに作成され、新たにgppcontact命令を実行するまでは保持されます。
gppcontact命令は、物理挙動の過程で発生した衝突の詳細を得るための命令です。簡易的な衝突を知るだけであれば、getcoli命令を代替で利用することがてきます。
また、あくまでも物理挙動で移動を行った際に衝突した(他のオブジェクトにめり込んだ)という情報をもとにしているため、完全に動きのない状態(単に接しているだけの状態)では衝突情報が作成されないので注意してください。
%href
gppbind
gppinfo
getcoli
setcoli
gppraytest
gppsweeptest


%index
gppinfo
オブジェクトの物理衝突情報を取得する
%group
拡張画面制御命令
%prm
fv,var,objid,index
fv       : FV値が代入される変数名
var      : 衝突したオブジェクトID値が代入される変数名
objid(0) : オブジェクトID
index(0) : インデックス値
%inst
gppcontact命令によって作成されたオブジェクトの物理衝突情報を取得します。必ず、gppcontact命令を先に実行しておく必要があります。
objidにより指定された3Dオブジェクトの物理衝突情報をvarとfvで指定した変数に代入します。変数には以下の内容が格納されます。
^p
    変数                 | 代入される内容
    --------------------------------------------------------------
	var                    衝突したオブジェクトID
    fv(0)                  衝突したX座標
    fv(1)                  衝突したY座標
    fv(2)                  衝突したZ座標
    fv(3)                  衝突した強さ(未実装)
^p
gppcontact命令により検出された衝突情報が複数ある場合は、0から始まるインデックス値によって識別します。
3つの衝突情報がある場合は、インデックス値は0〜2を指定することができます。
処理が正常に終了した場合は、システム変数statに0が代入されます。エラーが発生した場合は、システム変数statにマイナス値が代入されます。
^
現在のバージョンでは、gppcontactの動作時は衝突した強さが代入されませんので注意してください。

%href
gppcontact


%index
gppraytest
ベクトル上の物理衝突情報を取得する
%group
拡張画面制御命令
%prm
var,objid,distance,group
var      : 検出したオブジェクトID値が代入される変数名
objid(0)      : オブジェクトID
distance(100) : ベクトルの長さ
group(0)      : 対象となるコリジョングループ(0=すべて)
%inst
物理設定を行なった3Dオブジェクトの中で、指定した線(ベクトル)に衝突するオブジェクトの情報を取得します。
objidで指定した3Dノードを持つオブジェクトを基点として、オブジェクトの向いている方向からdistanceで指定された距離の間で最初に衝突する3DオブジェクトのオブジェクトIDを調査します。
objidで指定されるオブジェクトは、物理設定されている必要はありません。カメラやnullノード等であっても問題ありません。
groupパラメーターで、衝突の対象となるコリジョングループを指定することができます。objidで指定したオブジェクトに設定されたコリジョン設定は参照されないので注意してください。
groupパラメーターを省略するか0が指定された場合は、すべての物理設定オブジェクトが対象となります。
^
varで指定された変数に結果が代入されます。何らかのエラーが発生した場合は、マイナス値が代入されます。
線分に衝突するオブジェクトがあった場合は、そのオブジェクトIDが代入されます。何も衝突するオブジェクトがなかった場合は、0が代入されます。
衝突があった場合は、さらにobjidで指定されたオブジェクトのノードワーク値(work,work2)に詳細な情報が格納されます。
ノードワーク値は、getwork及びgetwork2命令により取得することができます。それぞれに設定される内容は以下の通りです。
^p
	ノードワーク値       | 設定される内容
	--------------------------------------------------------------
	work                   衝突が発生したX,Y,Z座標
	work2                  衝突した面の法線ベクトルX,Y,Z値
^p
板ノード(gpplateで生成)との衝突は正しく判定されないことがあるため、衝突のテストを行う際には厚みのある物体(箱など)を使用するようにしてください。
単純な線(ベクトル)ではなく、3Dモデルとの衝突をテストしたい場合は、gppsweeptest命令によって調べることが可能です。
%href
gppcontact
getcoli
getwork
getwork2
gppsweeptest


%index
gppsweeptest
モデルを指定ベクトルに移動させた際の物理衝突情報を取得する
%group
拡張画面制御命令
%prm
var,objid,x,y,z,group
var     : 検出したオブジェクトID値が代入される変数名
objid(0): オブジェクトID
x(0)    : X座標(実数)
y(0)    : Y座標(実数)
z(0)    : Z座標(実数)
group(0): コリジョングループ適用スイッチ(0=なし)
%inst
objidパラメーターで指定した3Dノードを持つオブジェクトの現在位置を基点として、(X,Y,Z)で指定された座標に移動させた場合に最初に衝突する3DオブジェクトのオブジェクトIDを調査します。
objidパラメーターで指定されるオブジェクトに設定された物理情報をもとに、衝突判定のテストを行います(実際には移動されません)。
groupパラメーターで、衝突の対象となるコリジョングループを考慮するかどうかを設定します。groupパラメーターを省略するか0以外が設定された場合は、setcoli命令で指定されたコリジョングループが考慮されます。0が指定された場合は、すべての物理設定オブジェクトが対象となります。
^
varで指定された変数に結果が代入されます。何らかのエラーが発生した場合は、マイナス値が代入されます。
線分に衝突するオブジェクトがあった場合は、そのオブジェクトIDが代入されます。何も衝突するオブジェクトがなかった場合は、0が代入されます。
衝突があった場合は、さらにobjidで指定されたオブジェクトのノードワーク値(work,work2)に詳細な情報が格納されます。
ノードワーク値は、getwork及びgetwork2命令により取得することができます。それぞれに設定される内容は以下の通りです。
^p
	ノードワーク値       | 設定される内容
	--------------------------------------------------------------
	work                   衝突が発生したX,Y,Z座標
	work2                  衝突した面の法線ベクトルX,Y,Z値
^p
板ノード(gpplateで生成)との衝突は正しく判定されないことがあるため、衝突のテストを行う際には厚みのある物体(箱など)を使用するようにしてください。
モデルではなく単純な線(ベクトル)に衝突するかどうかは、gppraytest命令によって調べることが可能です。
%href
gppcontact
setcoli
getcoli
getwork
getwork2
gppraytest


%index
event_fade
フェード設定イベントを追加
%group
拡張画面制御命令
%prm
id,fade
id         : イベントID
fade(0)    : 設定される値
%inst
オブジェクトのフェード値を設定するイベントを追加します。
フェードにマイナス値を指定することでα値を0に向けてフェードアウト、プラス値を指定することでα値を255に向けてフェードインさせることができます。
event_prmset命令でPRMSET_FADEパラメーターに設定することと等価です。
%href
event_prmset


%index
gpmesh
3Dメッシューノードを生成
%group
拡張画面制御命令
%prm
var,color,matobj
var     : 生成されたオブジェクトIDが代入される変数名
color(-1)  : マテリアルカラー(24bitRGB値)
matobj(-1) : マテリアルID
%inst
自由な形状を3Dモデルとして定義したものをノードオブジェクトとして生成します。
あらかじめ、gpmeshclear、gpmeshadd、gpmeshpolygon等の命令を使用して、カスタム3Dメッシュと呼ばれるモデル情報を構築する必要があります。
gpboxやgpplateのような単純なモデルではなく、自由に複雑な形状を定義することができますが、頂点座標や法線ベクトルの1つ1つを定義する必要があるため、上級者向けの機能と考えてください。
通常は3Dモデルデータを.gpbファイルとして用意したものをご利用ください。
^
カスタム3Dメッシュは、以下のような手順で作成することができます。
^p
	1. gpmeshclear命令でカスタム3Dメッシュを初期化する
	2. gpmeshadd命令で必要な頂点情報を登録する
	3. gpmeshpolygon命令により頂点情報を組み合わせて3角形または4角形の面を構成する
	4. 必要な面だけ以上の登録を繰り返す
^p
gpmesh命令は、作成されたカスタム3Dメッシュの情報をもとに、登録されているすべての面を使用したモデルを生成します。
頂点情報には、X,Y,Z座標、法線ベクトル、テクスチャUV座標などを登録することが可能です。これらを適切に設定することで、自由な形状を定義することができます。
カスタム3Dメッシュは、何度でも再定義して異なるモデル形状を登録することが可能です。
^
varで指定された変数に、生成されたオブジェクトのIDが代入されます。
生成されたオブジェクトはgpload命令により読み込まれた3Dモデルと同様に操作することができます。
colorパラメーターでモデル全体の色(マテリアルカラー)を24bitRGB値(0xRRGGBBで示される値)により指定します。
colorパラメーターが省略された場合は、白色(0xffffff)が使用されます。
matobjパラメーターで、マテリアルIDを指定することができます。
gpcolormat命令などにより、ユーザーが生成したマテリアルを使用する場合は指定してください。
matobjパラメーターが省略された場合は、標準のマテリアルが使用されます。

%href
gpmeshclear
gpmeshpolygon
gpmeshadd


%index
gpmeshclear
カスタム3Dメッシュを初期化
%group
拡張画面制御命令
%inst
カスタム3Dメッシュの登録情報を初期化します。
カスタム3Dメッシュは、自由な3D形状を定義するためのデータを構築する仕組みです。
実際のデータ登録は、gpmeshadd、gpmeshpolygon命令を使用します。
構築したデータは、gpmesh命令でノードオブジェクトとして登録することができます。
%href
gpmesh
gpmeshpolygon
gpmeshadd


%index
gpmeshadd
カスタム3Dメッシュに頂点情報を追加
%group
拡張画面制御命令
%prm
var,x,y,z,nx,ny,nz,u,v
var     : 生成されたメッシュ頂点IDが代入される変数名
x(0)    : X座標(実数)
y(0)    : Y座標(実数)
z(0)    : Z座標(実数)
nx(0)   : 法線ベクトルX(実数)
ny(0)   : 法線ベクトルY(実数)
nz(0)   : 法線ベクトルZ(実数)
u(0)    : テクスチャ頂点座標X(実数)
v(0)    : テクスチャ頂点座標Y(実数)
%inst
カスタム3Dメッシュに頂点情報を追加登録します。
頂点情報は、X,Y,Z座標、法線ベクトル、テクスチャUV座標を1つのセットとして登録します。
varで指定された変数に、登録されたメッシュ頂点IDが代入されます。メッシュ頂点IDは、面(3角形,4角形)を構成する際に使用します。
既に登録されている頂点情報の場合は、過去に登録されたメッシュ頂点IDが代入され、重複して登録は行われません。頂点情報は最大で32767まで登録することが可能です。
^
(X,Y,Z)座標は3D空間上の頂点を示します。
法線ベクトル(X,Y,Z)は頂点が向いている方向(ベクトル)を正規化したものを示します。法線ベクトルは、光源計算を行う際に必要となります。
テクスチャ頂点座標(X(u),Y(v))は、テクスチャを貼る際に参照される座標を指定します。原点(0.0, 0.0)から両端(1.0, 1.0)までの正規化された値となります。マテリアルとしてテクスチャを貼る際に必要となります。
^
カスタム3Dメッシュを定義する場合は、最初に必ずgpmeshclear命令で登録情報をクリアしておく必要があります。
実際に頂点情報を組み合わせて面(3角形,4角形)を登録する場合は、gpmeshpolygon命令を使用します。
構築したデータは、gpmesh命令でノードオブジェクトとして登録することができます。
%href
gpmeshclear
gpmeshpolygon
gpmesh


%index
gpmeshpolygon
カスタム3Dメッシュに面情報を追加
%group
拡張画面制御命令
%prm
p1,p2,p3,p4
p1(0)   : メッシュ頂点ID1
p2(0)   : メッシュ頂点ID2
p3(0)   : メッシュ頂点ID3
p4(-1)  : メッシュ頂点ID4
%inst
カスタム3Dメッシュに面(ポリゴン)情報を追加登録します。
あらかじめ、gpmeshadd命令により必要な頂点情報を登録しておく必要があります。
頂点情報のインデックスを示すメッシュ頂点IDを組み合わせて、面(3角形,4角形)を構成します。
3角形の場合は、p1,p2,p3パラメーター、4角形の場合は、p1,p2,p3,p4パラメーターにメッシュ頂点IDを指定してください。頂点を指定する順番は、p1,p2,p3の3角形が時計回りに並ぶように配置してください。(4角形の場合は、残りの1頂点を最後に追加してください)
(指定する座標は、(-1,-1,0)が左上、(1,1,0)が右下となるような座標系になります。
OpenGLの座標系ではなく、あくまでも座標値の大小でモデル座標を考慮しています。
また、テクスチャ(UV)座標は、縦の0〜1は上下が逆になっていますのでご注意ください。)
^
構築したデータは、gpmesh命令でノードオブジェクトとして登録することができます。
%href
gpmeshclear
gpmeshadd
gpmesh


%index
gpnodeinfo
3Dモデルの階層情報を取得する
%group
拡張画面制御命令
%prm
var,objid,option,"name"
var       : 結果が代入される変数名
objid(0)  : オブジェクトID
option(0) : 取得オプション値
name      : 階層ノード名
%inst
gpload命令で読み込まれた3Dモデルの階層情報を取得して、varで指定された変数に結果を代入します。
objidで指定した3Dモデルが持つ階層を名前によって指定することで個別のノードを特定します。
(階層の名前(ノード名)は、3Dモデル作成時にあらかじめ付けられています。すべての階層を取得する際には、mod_gputilモジュールのgptree_get命令をご使用ください。)
option値により、代入される結果を選択することができます。option値に指定できる値は、以下の通りです。
^p
	オプション値          | 取得される内容
	--------------------------------------------------------------
	GPNODEINFO_NODE         該当するノードを示すオブジェクトID
	GPNODEINFO_MODEL        該当するモデルノードを示すオブジェクトID
	GPNODEINFO_MATNUM       ノードが保持しているマテリアルの数
	GPNODEINFO_MATERIAL     ノードのマテリアルID
	GPNODEINFO_NAME         該当するノード名(*)
	GPNODEINFO_CHILD        階層が持つ子のノード名(*)
	GPNODEINFO_SIBLING      同じ階層にある隣のノード名(*)
	GPNODEINFO_SKINROOT     スキンメッシュが持っているノード名(*)
	GPNODEINFO_MATNAME      ノードに設定されたマテリアル名(*)

    (*) 結果の文字列が代入されます
^p
GPNODEINFO_NODEを指定した場合は、指定された名前を持つ階層を示す特別なオブジェクトIDの値が代入されます。
このオブジェクトIDを使用して、getpos、getang、getquatなどそれぞれの階層が持つ座標や回転情報を取得することができます。(このオブジェクトIDは、gpnodeinfo命令で取得した直後しか使用できません。他の階層をgpnodeinfo命令で取得した場合は、以前に取得したオブジェクトIDは無効になります)
GPNODEINFO_MODELを指定した場合は、描画するモデルを持った階層のみオブジェクトIDが取得されます。
GPNODEINFO_CHILD、GPNODEINFO_SIBLING、GPNODEINFO_SKINROOTのオプションは、他の階層との繋がりを取得します。これにより、ノードの階層を調べることができます。
処理が正常に終了した場合は、システム変数statに0が代入されます。エラーが発生した場合は、システム変数statにマイナス値が代入されます。
^
GPNODEINFO_MATNUMを指定した場合は、ノードが保持しているマテリアルの数を取得します。メッシュモデルではないノードを指定した場合は、-1が返されます。
GPNODEINFO_MATNAMEを指定した場合は、ノードに設定されたマテリアル名を取得します。メッシュモデルではないノードやgploadで読み込まれていない3Dノードを指定した場合は、空の文字列("")が返されます。
1つのノードに複数のマテリアルが設定されている場合は、GPNODEINFO_MATNAME+1、GPNODEINFO_MATNAME+2というようにインデックスの値を加算してください。複数のマテリアルが設定されているかは、GPNODEINFO_MATNUMのオプションで調べることができます。
^
GPNODEINFO_MATERIALを指定した場合は、ノードに設定されたマテリアルを参照するためのマテリアルIDを生成します。メッシュモデルではないノードやgploadで読み込まれていない3Dノードを指定した場合は、-1が返されます。
取得されたマテリアルIDは、gpmatprm系,gpmatstate系の命令で独自に設定を追加変更することが可能です。
1つのノードに複数のマテリアルが設定されている場合は、GPNODEINFO_MATERIAL+1、GPNODEINFO_MATERIAL+2というようにインデックスの値を加算してください。複数のマテリアルが設定されているかは、GPNODEINFO_MATNUMのオプションで調べることができます。
GPNODEINFO_MATERIALによって取得されたマテリアルIDは、ノード内の情報を参照するために生成される特別なIDとなります。毎フレーム取得するような処理を行うと、フレームごとに新規のマテリアルIDが取得されてしまいますので、モデルが読み込まれた後に1回だけマテリアルIDを取得して使用するようにしてください。
取得されたマテリアルIDは、gpdelobj命令によって破棄された場合でも元のマテリアル情報は維持されますので、不要になったマテリアルIDは破棄することができます。
%href
gpload


%index
event_work2
work2グループ変化イベントを追加
%group
拡張画面制御命令
%prm
id,frame,x1,y1,z1,sw
id         : イベントID
frame      : 変化までのフレーム数
(x1,y1,z1) : 設定される値
sw(0)      : 補間オプション
%inst
idで指定しているイベントIDに、グループ変化イベントを追加します。
グループ変化イベントは、オブジェクトが持っているパラメーターの時間による変化を設定します。
frameで指定したフレーム数が経過した時に(x1,y1,z1)の値になります。
swの補間オプションは、以下の値を指定することができます。
^p
	sw = 0 : リニア補間(絶対値)
	sw = 1 : スプライン補間(絶対値)
	sw = 2 : リニア補間(相対値)
	sw = 3 : スプライン補間(相対値)
^p
swを省略した場合には、絶対値リニアが設定されます。

%href
event_pos
event_ang
event_angr
event_scale
event_dir
event_efx
event_work
newevent
setevent


%index
event_setwork2
work2グループ設定イベントを追加
%group
拡張画面制御命令
%prm
id,x1,y1,z1,x2,y2,z2
id         : イベントID
(x1,y1,z1) : 設定される値(下限値)
(x2,y2,z2) : 設定される値(上限値)
%inst
idで指定しているイベントIDに、グループ設定イベントを追加します。
グループ設定イベントは、オブジェクトが持っているパラメーターを設定します。
(x1,y1,z1)と(x2,y2,z2)を指定すると、それぞれの範囲内にある値を乱数で作成します。
(x2,y2,z2)を省略して、(x1,y1,z1)だけを指定した場合はそのまま値が設定されます。

%href
event_setpos
event_setang
event_setangr
event_setscale
event_setdir
event_setefx
event_setwork
newevent
setevent


%index
event_addwork2
work2グループ加算イベントを追加
%group
拡張画面制御命令
%prm
id,x,y,z
id      : イベントID
(x,y,z) : 加算される値
%inst
idで指定しているイベントIDに、グループ加算イベントを追加します。
グループ加算イベントは、オブジェクトが持っているパラメーターに(x,y,z)の値を加算します。

%href
event_addpos
event_addang
event_addangr
event_addscale
event_adddir
event_addefx
event_addwork
newevent
setevent


%index
event_prmadd
パラメーター加算イベントを追加
%group
拡張画面制御命令
%prm
id,p1,p2,p3,p4
id      : イベントID
p1(0)   : パラメーターID(PRMSET_*)
p2(0)   : 設定される値
p3(0)   : 設定される最小値
p4(0)   : 設定される最大値
%inst
idで指定しているイベントIDに、パラメーター加算イベントを追加します。
パラメーター設定イベントは、p1で指定されたパラメーターIDにp2の値を加算します。(マイナス値も設定できます)
パラメーターIDには、以下の名前を使用することができます。
^p
パラメーターID   内容
---------------------------------------
		PRMSET_FLAG            オブジェクト登録フラグ(*)
		PRMSET_MODE            モードフラグ値
		PRMSET_ID              オブジェクトID(*)(HGIMG4のみ)
		PRMSET_ALPHA           透明度(α値)(HGIMG4のみ)
		PRMSET_SHADE           シェーディングモード(HGIMG3のみ)
		PRMSET_TIMER           タイマー値
		PRMSET_MYGROUP         自身のコリジョングループ
		PRMSET_COLGROUP        衝突検出するコリジョングループ
		PRMSET_SHAPE           形状ID(*)(HGIMG4のみ)
		PRMSET_USEGPMAT        マテリアルID(HGIMG4のみ)
		PRMSET_USEGPPHY        物理設定ID(*)(HGIMG4のみ)
		PRMSET_COLILOG         コリジョンログID(*)(HGIMG4のみ)
		PRMSET_FADE            フェードパラメーター(HGIMG4のみ)
		PRMSET_SPRID           ソースバッファID(スプライトのみ)(HGIMG4のみ)
		PRMSET_SPRCELID        ソースのセルID(スプライトのみ)(HGIMG4のみ)
		PRMSET_SPRGMODE        コピーモード(スプライトのみ)(HGIMG4のみ)

		(*)の項目は読み出しのみ
^p
p3,p4パラメーターにより、加算後に設定される最小値、最大値を指定することができます。
特定の範囲内のみに収まるように加算を行うことが可能です。
%href
event_prmset
event_prmon
event_prmoff
newevent
setevent


