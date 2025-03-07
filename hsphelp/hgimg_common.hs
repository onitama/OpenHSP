;
;	HSP help manager用 HELPソースファイル
;	(先頭が「;」の行はコメントとして処理されます)
;
%type
拡張命令
%ver
3.7
%note
hgimg3.as/hgimg4.as/hgimg4dx.asのいずれかをインクルードすること。
%author
onitama
%dll
HGIMG3/HGIMG4共通
%date
2022/06/07
%author
onitama
%url
http://www.onionsoft.net/
%port
Win
%portinfo
HGIMG4DXはDirectX9、HGIMG4はOpenGL3.1環境で動作


%index
fvseti
整数値からベクトル設定
%group
拡張画面制御命令
%prm
fv,x,y,z
fv      = FV値が代入される変数名
(x,y,z) = 整数値
%inst
(x,y,z)で指定された整数値をベクトルとしてFV値に代入する。
%href
fvset
fvadd
fvsub
fvmul
fvdiv
fvdir
fvmin
fvmax
fvouter
fvinner
fvface
fvunit


%index
fvset
ベクトル設定
%group
拡張画面制御命令
%prm
fv,x,y,z
fv      = FV値が代入される変数名
(x,y,z) = 計算値(実数値)
%inst
(x,y,z)で指定された小数値(X,Y,Z)をベクトルとしてFV値に代入する。
%href
fvseti
fvadd
fvsub
fvmul
fvdiv
fvdir
fvmin
fvmax
fvouter
fvinner
fvface
fvunit


%index
fvadd
ベクトル加算
%group
拡張画面制御命令
%prm
fv,x,y,z
fv      = FV値が代入されている変数名
(x,y,z) = 計算値(実数値)
%inst
(x,y,z)で指定された小数値(X,Y,Z)をFV値に加算する。
%href
fvseti
fvset
fvsub
fvmul
fvdiv
fvmin
fvmax


%index
fvsub
ベクトル減算
%group
拡張画面制御命令
%prm
fv,x,y,z
fv      = FV値が代入されている変数名
(x,y,z) = 計算値(実数値)
%inst
(x,y,z)で指定された小数値(X,Y,Z)をFV値から減算する。
%href
fvseti
fvset
fvadd
fvmul
fvdiv
fvmin
fvmax


%index
fvmul
ベクトル乗算
%group
拡張画面制御命令
%prm
fv,x,y,z
fv      = FV値が代入されている変数名
(x,y,z) = 計算値(実数値)
%inst
(x,y,z)で指定された小数値(X,Y,Z)をFV値に並列で乗算する。
%href
fvseti
fvset
fvadd
fvsub
fvdiv
fvmin
fvmax


%index
fvdiv
ベクトル除算
%group
拡張画面制御命令
%prm
fv,x,y,z
fv      = FV値が代入されている変数名
(x,y,z) = 計算値(実数値)
%inst
(x,y,z)で指定された小数値(X,Y,Z)をFV値に並列で除算する。
%href
fvseti
fvset
fvadd
fvsub
fvmul
fvmin
fvmax


%index
fvdir
計算が適用されたベクトルを取得する
%group
拡張画面制御命令
%prm
fv,x,y,z,type
fv      = FV値が代入されている変数名
(x,y,z) = ベクトル値(実数値)
type(0) = 取得されるベクトルの計算タイプ
%inst
fvで指定された変数に格納されているFV値(X,Y,Z)をもとに、実数値で指定されたベクトル(X,Y,Z)に対して計算を行ったものを、FV値として変数fvに代入します。
typeパラメーターで指定する値は、以下のものになります。
^p
    マクロ名              値    内容
    ---------------------------------------------------------------------------------------------
	FVDIR_ROTORDER_ZYX     0    FV値(角度)をもとにベクトル値を回転させる(Z->Y->X)
	FVDIR_ROTORDER_XYZ     1    FV値(角度)をもとにベクトル値を回転させる(X->Y->Z)
	FVDIR_ROTORDER_YXZ     2    FV値(角度)をもとにベクトル値を回転させる(Y->X->Z)
	FVDIR_HALFVECTOR       4    FV値(ベクトル)とベクトル値のハーフベクトルを求める
	FVDIR_REFRECTION       5    FV値(入射ベクトル)とベクトル値(法線)をもとに反射ベクトルを求める
	FVDIR_MIRROR           6    FV値(入射ベクトル)とベクトル値(法線)をもとに鏡面ベクトルを求める
^p
計算タイプが0〜2の場合は、FV値にあらかじめ設定されている(X,Y,Z)値を角度値(ラジアン)としてパラメーターで指定されたベクトル値に対して回転を適用します。計算タイプにより、回転順序を変更することができます。計算タイプを省略するか、0が指定された場合は通常の回転順序(Z->Y->X)となります。
計算タイプが4以上の場合は、FV値にあらかじめ設定されている(X,Y,Z)値を元のベクトルとして、パラメーターで指定されたベクトル値をもとに計算を適用します。この場合にあらかじめ設定されているベクトル値は、正規化されているものとして扱われます。
%href
fvset
fvdir
fvface


%index
fvface
座標から角度を得る
%group
拡張画面制御命令
%prm
fv,x,y,z
fv      = FV値が代入されている変数名
(x,y,z) = X,Y,Z座標値(実数値)
%inst
fvで指定された変数に格納されているベクトル(FV値)を基点とするX,Y,Z座標から、指定されたX,Y,Z座標を直線で見るためのX,Y,Z回転角度を求めて変数fvに代入します。
%href
fvset
fvdir


%index
fvmin
ベクトルの要素を最小値で切り詰める
%group
拡張画面制御命令
%prm
fv,x,y,z
fv      = FV値が代入されている変数名
(x,y,z) = 比較値(実数値)
%inst
fvで指定された変数に格納されているFV値の各要素を、パラメーターで指定された値(X,Y,Z)が最小値になるように切り詰めます。
FV値の各要素に対して最小値を適用する場合に使用します。
%href
fvseti
fvset
fvadd
fvsub
fvmul
fvdiv
fvmax


%index
fvmax
ベクトルの要素を最大値で切り詰める
%group
拡張画面制御命令
%prm
fv,x,y,z
fv      = FV値が代入されている変数名
(x,y,z) = 比較値(実数値)
%inst
fvで指定された変数に格納されているFV値の各要素を、パラメーターで指定された値(X,Y,Z)が最大値になるように切り詰めます。
FV値の各要素に対して最大値を適用する場合に使用します。
%href
fvseti
fvset
fvadd
fvsub
fvmul
fvdiv
fvmin


%index
fvouter
ベクトル外積
%group
拡張画面制御命令
%prm
fv,x,y,z
fv      = FV値が代入されている変数名
(x,y,z) = 演算するベクトル値(実数値)
%inst
fvで指定された変数に格納されているFV値と、小数値(X,Y,Z)で指定するベクトルの外積を求めて代入します。
%href
fvseti
fvset
fvinner


%index
fvinner
ベクトル内積
%group
拡張画面制御命令
%prm
fv,x,y,z
fv      = FV値が代入されている変数名
(x,y,z) = 演算するベクトル値(実数値)
%inst
fvで指定された変数に格納されているFV値と、小数値(X,Y,Z)で指定するベクトルの内積を求めてfv.0に代入します。
%href
fvseti
fvset
fvouter


%index
fvunit
ベクトル正規化
%group
拡張画面制御命令
%prm
fv
fv      = FV値が代入されている変数名
%inst
fvで指定された変数に格納されているベクトル(FV値)を正規化します。
%href
fvseti
fvset


%index
fsin
サインを求める
%group
拡張画面制御命令
%prm
fval,frot
fval    = 実数値が代入される変数名
frot    = 回転角度(ラジアン)
%inst
frotで指定された角度のサイン値をfvalで指定した変数に代入します。
角度の単位はラジアン(2π=360度)になります。
%href
fcos
fsqr
froti



%index
fcos
コサインを求める
%group
拡張画面制御命令
%prm
fval,frot
fval    = 実数値が代入される変数名
frot    = 回転角度(ラジアン)
%inst
frotで指定された角度のコサイン値をfvalで指定した変数に代入します。
角度の単位はラジアン(2π=360度)になります。
%href
fsin
fsqr
froti



%index
fsqr
平方根を求める
%group
拡張画面制御命令
%prm
fval,fprm
fval    = 実数値が代入される変数名
fprm    = 演算に使われる値(実数)
%inst
fprmで指定された値の平方根をfvalで指定した変数に代入します。
%href
fsin
fcos
froti



%index
froti
整数値角度を小数値に変換
%group
拡張画面制御命令
%prm
fval,prm
fval    = 実数値が代入される変数名
prm(0)  = 0〜1023の角度値(整数)
%inst
prmで指定された整数値(0〜1023)を角度を示す値として、fvalに-π〜+πのラジアン角度値に変換して代入します。
%href
fsin
fcos
fsqr



%index
str2fv
文字列をベクトルに変換
%group
拡張画面制御命令
%prm
fv,"x,y,z"
fv      = FV値が代入される変数名
"x,y,z" = 「,」で区切られた実数値が格納された文字列
%inst
"x,y,z"で指定された文字列情報を「,」で区切られたX,Y,Z小数値として読み出し、fvで指定された変数に格納します。
それぞれの項目が正しく数値として認識できない(不正な)文字列があった場合には、それ以降の項目も含めて0.0が代入されます。
%href
fv2str
str2f
f2str
f2i


%index
fv2str
ベクトルを文字列に変換
%group
拡張画面制御命令
%prm
fv
fv      = FV値が代入されている変数名
%inst
fvで指定された変数に格納されているベクトル(FV値)を文字列に変換してシステム変数refstrに結果を返します。
%href
str2fv
str2f
f2str
f2i


%index
str2f
文字列を小数値に変換
%group
拡張画面制御命令
%prm
fval,"fval"
fval    = 実数値が代入される変数名
"fval"  = 実数値が格納された文字列
%inst
"fval"で指定された文字列情報を小数値として読み出し、fvalで指定された変数に格納します。
%href
fv2str
str2fv
f2str
f2i


%index
f2str
小数値を文字列に変換
%group
拡張画面制御命令
%prm
sval,fval
sval    = 文字列が代入される変数名
fval    = 変換元の実数値
%inst
fvalで指定された小数値を文字列に変換して、valで指定された文字列型の変数に結果を返します。
%href
fv2str
str2fv
str2f
f2i


%index
delobj
オブジェクトの削除
%group
拡張画面制御命令
%prm
ObjID
ObjID  : オブジェクトID
%inst
指定されたオブジェクトを削除します。
%href
regobj


%index
setpos
posグループ情報を設定
%group
拡張画面制御命令
%prm
id,x,y,z
id      : オブジェクトID
(x,y,z) : 設定する値 (デフォルト=0)

%inst
オブジェクトの持つパラメーターを設定します。
posグループ(表示座標)に(x,y,z)で指定された値を設定します。
(x,y,z)には、実数または整数値を指定することができます。

%href
setang
setangr
setscale
setdir
setefx
setwork


%index
setang
angグループ情報を設定
%group
拡張画面制御命令
%prm
id,x,y,z
id      : オブジェクトID
(x,y,z) : 設定する値 (デフォルト=0)

%inst
オブジェクトの持つパラメーターを設定します。
angグループ(表示角度)に(x,y,z)で指定された値を設定します。
(x,y,z)には、実数または整数値を指定することができます。
角度の単位はラジアンになります。
(回転する順番はX->Y->Zとなります。他の順番で回転させるための、setangy、setangz命令が用意されています。)
整数で角度を設定するためのsetangr命令も用意されています。

%href
setpos
setangr
setscale
setdir
setefx
setwork


%index
setangr
angグループ情報を設定
%group
拡張画面制御命令
%prm
id,x,y,z
id      : オブジェクトID
(x,y,z) : 設定する値 (デフォルト=0)

%inst
オブジェクトの持つパラメーターを設定します。
angグループ(表示角度)に(x,y,z)で指定された値を設定します。
(x,y,z)には、実数または整数値を指定することができます。
角度の単位は整数で0〜255で一周する値を使用します。
ラジアンで角度を設定するためのsetang命令も用意されています。

%href
setpos
setang
setscale
setdir
setefx
setwork


%index
setscale
scaleグループ情報を設定
%group
拡張画面制御命令
%prm
id,x,y,z
id      : オブジェクトID
(x,y,z) : 設定する値 (デフォルト=0)

%inst
オブジェクトの持つパラメーターを設定します。
scaleグループ(表示倍率)に(x,y,z)で指定された値を設定します。
(x,y,z)には、実数または整数値を指定することができます。

%href
setpos
setang
setangr
setdir
setefx
setwork


%index
setdir
dirグループ情報を設定
%group
拡張画面制御命令
%prm
id,x,y,z
id      : オブジェクトID
(x,y,z) : 設定する値 (デフォルト=0)

%inst
オブジェクトの持つパラメーターを設定します。
dirグループ(移動ベクトル)に(x,y,z)で指定された値を設定します。
(x,y,z)には、実数または整数値を指定することができます。
移動ベクトルに登録された値は、オブジェクトの自動移動モード(OBJ_MOVE)時に参照されます。

%href
setpos
setang
setangr
setscale
setefx
setwork


%index
setwork
workグループ情報を設定
%group
拡張画面制御命令
%prm
id,x,y,z
id      : オブジェクトID
(x,y,z) : 設定する値 (デフォルト=0)

%inst
オブジェクトの持つパラメーターを設定します。
workグループ(ワーク値)に(x,y,z)で指定された値を設定します。
(x,y,z)には、実数または整数値を指定することができます。

%href
setpos
setang
setangr
setscale
setdir
setefx
setwork2


%index
addpos
posグループ情報を加算
%group
拡張画面制御命令
%prm
id,x,y,z
id      : オブジェクトID
(x,y,z) : 加算する値 (デフォルト=0)

%inst
オブジェクトの持つパラメーターを設定します。
posグループ(表示座標)に(x,y,z)で指定された値を設定します。
(x,y,z)には、実数または整数値を指定することができます。

%href
addang
addangr
addscale
adddir
addefx
addwork


%index
addang
angグループ情報を加算
%group
拡張画面制御命令
%prm
id,x,y,z
id      : オブジェクトID
(x,y,z) : 加算する値 (デフォルト=0)

%inst
オブジェクトの持つパラメーターを設定します。
angグループ(表示角度)に(x,y,z)で指定された値を加算します。
(x,y,z)には、実数または整数値を指定することができます。
角度の単位はラジアンになります。
整数で角度を設定するためのsetangr命令も用意されています。

%href
addpos
addangr
addscale
adddir
addefx
addwork


%index
addangr
angグループ情報を加算
%group
拡張画面制御命令
%prm
id,x,y,z
id      : オブジェクトID
(x,y,z) : 加算する値 (デフォルト=0)

%inst
オブジェクトの持つパラメーターを設定します。
angグループ(表示角度)に(x,y,z)で指定された値を加算します。
(x,y,z)には、実数または整数値を指定することができます。
角度の単位は整数で0〜255で一周する値を使用します。
ラジアンで角度を設定するためのsetang命令も用意されています。

%href
addpos
addang
addscale
adddir
addefx
addwork


%index
addscale
scaleグループ情報を加算
%group
拡張画面制御命令
%prm
id,x,y,z
id      : オブジェクトID
(x,y,z) : 加算する値 (デフォルト=0)

%inst
オブジェクトの持つパラメーターを設定します。
scaleグループ(表示倍率)に(x,y,z)で指定された値を加算します。
(x,y,z)には、実数または整数値を指定することができます。

%href
addpos
addang
addangr
adddir
addefx
addwork


%index
adddir
dirグループ情報を加算
%group
拡張画面制御命令
%prm
id,x,y,z
id      : オブジェクトID
(x,y,z) : 加算する値 (デフォルト=0)

%inst
オブジェクトの持つパラメーターを設定します。
dirグループ(移動ベクトル)に(x,y,z)で指定された値を加算します。
(x,y,z)には、実数または整数値を指定することができます。

%href
addpos
addang
addangr
addscale
addefx
addwork


%index
addwork
workグループ情報を加算
%group
拡張画面制御命令
%prm
id,x,y,z
id      : オブジェクトID
(x,y,z) : 加算する値 (デフォルト=0)

%inst
オブジェクトの持つパラメーターを設定します。
workグループ(ワーク値)に(x,y,z)で指定された値を加算します。
(x,y,z)には、実数または整数値を指定することができます。

%href
addpos
addang
addangr
addscale
adddir
addefx


%index
getpos
posグループ情報を取得
%group
拡張画面制御命令
%prm
id,x,y,z
id      : オブジェクトID
(x,y,z) : 取得する変数

%inst
オブジェクトの持つパラメーターを取得します。
posグループ(表示座標)の内容が(x,y,z)で指定された変数に代入されます。
(x,y,z)は、実数型の変数として設定されます。
命令の最後に「i」を付加することで、整数値として値を取得することができます。

%href
getposi
getang
getangr
getscale
getdir
getefx
getwork


%index
getscale
scaleグループ情報を取得
%group
拡張画面制御命令
%prm
id,x,y,z
id      : オブジェクトID
(x,y,z) : 取得する変数

%inst
オブジェクトの持つパラメーターを取得します。
scaleグループ(表示倍率)の内容が(x,y,z)で指定された変数に代入されますます。
(x,y,z)は、実数型の変数として設定されます。
命令の最後に「i」を付加することで、整数値として値を取得することができます。

%href
getscalei
getpos
getang
getangr
getdir
getefx
getwork


%index
getdir
dirグループ情報を取得
%group
拡張画面制御命令
%prm
id,x,y,z
id      : オブジェクトID
(x,y,z) : 取得する変数

%inst
オブジェクトの持つパラメーターを取得します。
dirグループ(移動ベクトル)の内容が(x,y,z)で指定された変数に代入されます。
(x,y,z)は、実数型の変数として設定されます。
命令の最後に「i」を付加することで、整数値として値を取得することができます。

%href
getdiri
getpos
getang
getangr
getscale
getefx
getwork


%index
getwork
workグループ情報を取得
%group
拡張画面制御命令
%prm
id,x,y,z
id      : オブジェクトID
(x,y,z) : 取得する変数

%inst
オブジェクトの持つパラメーターを取得します。
workグループ(ワーク値)の内容が(x,y,z)で指定された変数に代入されます。
(x,y,z)は、実数型の変数として設定されます。
命令の最後に「i」を付加することで、整数値として値を取得することができます。

%href
getworki
getpos
getang
getangr
getscale
getdir
getefx


%index
getposi
posグループ情報を整数で取得
%group
拡張画面制御命令
%prm
id,x,y,z
id      : オブジェクトID
(x,y,z) : 取得する変数

%inst
オブジェクトの持つパラメーターを取得します。
posグループ(表示座標)の内容が(x,y,z)で指定された変数に代入されます。
(x,y,z)は、整数型の変数として設定されます。

%href
getpos
getangi
getangri
getscalei
getdiri
getefxi
getworki


%index
getscalei
scaleグループ情報を整数で取得
%group
拡張画面制御命令
%prm
id,x,y,z
id      : オブジェクトID
(x,y,z) : 取得する変数

%inst
オブジェクトの持つパラメーターを取得します。
scaleグループ(表示倍率)の内容が(x,y,z)で指定された変数に代入されますます。
(x,y,z)は、整数型の変数として設定されます。

%href
getscale
getposi
getangi
getangri
getdiri
getefxi
getworki


%index
getdiri
dirグループ情報を整数で取得
%group
拡張画面制御命令
%prm
id,x,y,z
id      : オブジェクトID
(x,y,z) : 取得する変数

%inst
オブジェクトの持つパラメーターを取得します。
dirグループ(移動ベクトル)の内容が(x,y,z)で指定された変数に代入されます。
(x,y,z)は、整数型の変数として設定されます。

%href
getdir
getposi
getangi
getangri
getscalei
getefxi
getworki


%index
getworki
workグループ情報を整数で取得
%group
拡張画面制御命令
%prm
id,x,y,z
id      : オブジェクトID
(x,y,z) : 取得する変数

%inst
オブジェクトの持つパラメーターを取得します。
workグループ(ワーク値)の内容が(x,y,z)で指定された変数に代入されます。
(x,y,z)は、整数型の変数として設定されます。

%href
getwork
getposi
getangi
getangri
getscalei
getdiri
getefxi


%index
selpos
移動座標をMOC情報に設定
%group
拡張画面制御命令
%prm
id
id     : オブジェクトID
%inst
MOC設定命令の対象となるMOCグループをpos(座標)に設定します
idは、オブジェクトIDとなります。
%href
selmoc
selang
selscale
seldir
selefx
selcam
selcpos
selcang
selcint


%index
selang
回転角度をMOC情報に設定
%group
拡張画面制御命令
%prm
id
id     : オブジェクトID
%inst
MOC設定命令の対象となるMOCグループをang(回転角度)に設定します
idは、オブジェクトIDとなります。
%href
selmoc
selpos
selscale
seldir
selefx
selcam
selcpos
selcang
selcint


%index
selscale
スケールをMOC情報に設定
%group
拡張画面制御命令
%prm
id
id     : オブジェクトID
%inst
MOC設定命令の対象となるMOCグループをscale(スケール)に設定します
idは、オブジェクトIDとなります。
%href
selmoc
selpos
selang
selefx
seldir
selcam
selcpos
selcang
selcint


%index
seldir
移動量をMOC情報に設定
%group
拡張画面制御命令
%prm
id
id     : オブジェクトID
%inst
MOC設定命令の対象となるMOCグループをdir(移動量)に設定します
idは、オブジェクトIDとなります。
%href
selmoc
selpos
selang
selscale
selefx
selcam
selcpos
selcang
selcint


%index
selwork
オブジェクトワークをMOC情報に設定
%group
拡張画面制御命令
%prm
id
id     : オブジェクトID
%inst
MOC設定命令の対象となるMOCグループをwork(ワーク)に設定します
idは、オブジェクトIDとなります。
%href
selmoc
selpos
selang
selscale
selefx
selcam
selcpos
selcang
selcint


%index
objset3
MOC情報を設定
%group
拡張画面制御命令
%prm
x,y,z
x   : 設定する値
y   : 設定する値2
z   : 設定する値3

%inst
MOC情報を設定します。
オフセット番号0から3つのパラメータが対象になります。
%href
objset3
objadd3
objset3r
objsetf3
objaddf3


%index
objsetf3
MOC情報を設定
%group
拡張画面制御命令
%prm
fx,fy,fz
fx  : 設定する値(実数値)
fy  : 設定する値2(実数値)
fz  : 設定する値3(実数値)

%inst
MOC情報を設定します。
オフセット番号0から3つのパラメータが対象になります。
%href
objset3
objadd3
objset3r
objsetf3
objaddf3


%index
objadd3
MOC情報を加算
%group
拡張画面制御命令
%prm
x,y,z
x   : 加算する値
y   : 加算する値2
z   : 加算する値3

%inst
MOC情報に設定されている値にx,y,zを加算します。
オフセット番号0から3つのパラメータが対象になります。
%href
objset3
objadd3r
objset3r
objsetf3
objaddf3


%index
objaddf3
MOC情報を加算
%group
拡張画面制御命令
%prm
fx,fy,fz
fx  : 加算する値(実数値)
fy  : 加算する値2(実数値)
fz  : 加算する値3(実数値)

%inst
MOC情報に設定されている値にfx,fy,fzを加算します。
オフセット番号0から3つのパラメータが対象になります。
%href
objset3
objadd3
objset3r
objsetf3
objaddf3


%index
objadd3r
MOC情報を加算
%group
拡張画面制御命令
%prm
ofs,fx,fy,fz
ofs : MOCのオフセット番号
fx  : 加算する値(整数角度値)
fy  : 加算する値2(整数角度値)
fz  : 加算する値3(整数角度値)
%inst
MOC情報に設定されている値にfx,fy,fzを加算します。
ただし整数値(256で１回転)をラジアン単位に変換したパラメーターを加算します。
角度を指定するパラメーター以外では正常な値にならないので注意してください。
%href
objset3
objadd3
objset3r
objsetf3
objaddf3


%index
objset3r
MOC情報を設定
%group
拡張画面制御命令
%prm
x,y,z
x   : 設定する値
y   : 設定する値2
z   : 設定する値3

%inst
MOC情報に角度情報を設定します。
オフセット番号0から3つのパラメータが対象になります。
整数値(256で１回転)をラジアン単位に変換してパラメーターを書き込みます。
角度を指定するパラメーター以外では正常な値にならないので注意してください。
%href
objset3
objadd3
objset3r
objsetf3
objaddf3


%index
setobjmode
オブジェクトのモード設定
%group
拡張画面制御命令
%prm
ObjID,mode,sw
ObjID    : オブジェクトID
mode     : モード値
sw       : 設定スイッチ
%inst
指定されたオブジェクトのモードを変更します。
オブジェクトのモードは、あらかじめデフォルトのモードがモデルの種類に応じて設定されているので、通常は特に設定する必要はありません。
モードは以下の中から選択します。
^p
	ラベル             |        内容
	--------------------------------------------------------------
	OBJ_HIDE             非表示(画面から消す)
	OBJ_CLIP             3Dクリッピングを有効にする(HGIMG4のみ)
	OBJ_XFRONT           正面属性(常に画面に正面を向く)
	OBJ_WIRE             ワイヤーフレームで描画する(HGIMG4のみ)
	OBJ_MOVE             自動移動を行なう(XYZ移動量を参照する)
	OBJ_FLIP             ボーダー領域で反転する
	OBJ_BORDER           ボーダー領域を有効にする
	OBJ_2D               2Dスプライト
	OBJ_TIMER            タイマーを有効にする(HGIMG4のみ)
	OBJ_LATE             後から描画される(半透明オブジェクト用)

	OBJ_FIRST            常に最初に描かれる(HGIMG3のみ)
	OBJ_SORT             自動的に奥から描かれる(3Dオブジェクト用)(HGIMG3のみ)
	OBJ_LOOKAT           特定オブジェクトの方向を常に向く(HGIMG3のみ)
	OBJ_LAND             Y座標を常に一定に保つ(HGIMG3のみ)
	OBJ_GROUND           地面として認識される(HGIMG3のみ)
	OBJ_STAND            地面の上に配置する(HGIMG3のみ)
	OBJ_GRAVITY          重力計算を有効にする(HGIMG3のみ)
	OBJ_STATIC           障害物として認識される(HGIMG3のみ)
	OBJ_BOUND            地面で反発する(メッシュマップコリジョン用)(HGIMG3のみ)
	OBJ_ALIEN            ターゲットに向ける(メッシュマップコリジョン用)(HGIMG3のみ)
	OBJ_WALKCLIP         移動の制限を受ける(メッシュマップコリジョン用)(HGIMG3のみ)
	OBJ_EMITTER          エミッター発生オブジェクトになる(HGIMG3のみ)

^p
HGIMG3の場合は、regobj命令で指定するモード値と同様になります。
OBJ_2Dのモードは、自動的に設定されるもので、途中で変更しないようにしてください。
複数の項目を同時に選択する場合は、「OBJ_LATE|OBJ_MOVE」のように「|」で区切って指定してください。何も指定しない場合は、0にするか省略して構いません。

swは、以下のように動作します。
^p
	sw = 0 : 指定したモード値を追加
	sw = 1 : 指定したモード値を削除
	sw = 2 : 指定したモード値だけを設定
^p
%href


%index
setcoli
オブジェクトのコリジョン設定
%group
拡張画面制御命令
%prm
id,mygroup,enegroup
id       : オブジェクトID
mygroup  : 自分が属するグループ値
enegroup : 衝突を検出する対象となるグループ値
%inst
オブジェクトに対してコリジョングループ情報を設定します。
コリジョングループ値は、1,2,4,8,16,32,64,128,256,512,1024,2048,4096,8192,16384,32768の中から1つだけを選択可能です。
%href
getcoli
findobj
nextobj


%index
getcoli
オブジェクトのコリジョン判定
%group
拡張画面制御命令
%prm
val,id,distance,startid,numid
val           : 結果が代入される変数名
id(0)         : オブジェクトID
distance(1.0) : 衝突を検出する範囲(実数値)
startid(0)    : 検出を開始するオブジェクトID
numid(-1)     : 検出の対象となるオブジェクトIDの数(-1=すべて)
%inst
指定したオブジェクトが持つコリジョン情報をもとに、そのオブジェクトが衝突している別なオブジェクトのIDを調べます。
distanceは、衝突する範囲(半径)を実数値で指定します。
衝突が検出された場合は、変数にオブジェクトIDが代入されます。
何も衝突が検出されなかった場合は、-1が代入されます。
^
startidとnumidパラメーターにより、衝突を検出するオブジェクトIDの範囲を指定することができます。
startidに検出を開始するオブジェクトIDを指定して、numidにオブジェクトIDの個数を指定します。numidを省略するかマイナス値の場合は、開始オブジェクトID以降すべてが対象になります。
たとえば、startidが100、numidが50の場合は、オブジェクトIDが100〜149までの範囲にあるオブジェクトが判定の対象となります。通常は、パラメーターを省略することですべてのオブジェクトが検出の対象となります。指定された範囲のオブジェクトIDだけに限定したい時に使用してください。
HGIMG4では、ライトやカメラなどもオブジェクトIDとして扱われますが、IDの範囲が設定できるものは3Dモデルや2Dスプライトのオブジェクトのみに限定されますのでご注意ください。
^
HGIMG4では、distanceにマイナス値を指定した場合、3Dモデルが持つ衝突範囲(バウンディングボックス)にdistanceを掛けた値をもとに衝突検出を行ないます。
たとえば、-1.5を指定した場合は、衝突範囲を1.5倍に拡大した状態で、衝突検出が行なわれます。また、より正確な衝突の情報を作成するために、gppcontact命令を使用することが可能です。
%href
setcoli
findobj
nextobj
gppcontact


%index
getobjcoli
オブジェクトのグループ取得
%group
拡張画面制御命令
%prm
var,id,group
var      : 結果が代入される変数名
id(0)    : オブジェクトID
group(0) : グループID
%inst
指定したオブジェクトが所属するグループ値(コリジョングループなど)を取得し、varで指定された変数に代入します。
グループ値は、以下のものになります。
^p
グループID   内容
---------------------------------------
  0          コリジョングループ(setcoliで設定)
  1          衝突対象グループ(setcoliで設定)
  2          レンダリンググループ(setobjrenderで設定)
  3          ライティンググループ(setobjrenderで設定)
  4          バウンディングスフィアのサイズを取得(HGIMG4のみ)(*)

(*)の項目はdouble値で代入されます
^p
グループIDに4を指定した場合は、該当する3Dモデルの外周を囲むためのバウンディングスフィア(球体)の半径サイズが代入されます。これは、HGIMG4でのみ有効です。

%href
setcoli
getcoli
setobjrender


%index
setobjrender
オブジェクトのレンダリンググループ設定
%group
拡張画面制御命令
%prm
id,rendergroup,lightgroup
id(0)           : オブジェクトID
rendergroup(1)  : レンダリンググループ値
lightgroup(1)   : ライティンググループ値
%inst
オブジェクトに対してレンダリンググループ、ライティンググループ情報を設定します。
レンダリンググループ値は、カメラからレンダリングした際に表示のON/OFFを設定するための値です。カメラが持つレンダリンググループ値と同一である場合は、表示が有効となります。
通常は、オブジェクト、カメラともにグループ1が設定されています。特定のカメラからの表示のみ表示を無効にしたい場合などに利用できます。
ライティンググループ値は、特定のライトに対して有効/無効を切り替えるものです。ライトが持つライティンググループ値と異なる場合は、ライトが無効となります。

それぞれのグループ値は、1,2,4,8,16,32,64,128,256,512,1024,2048,4096,8192,16384,32768の中から任意のビットを組み合わせて指定可能です。

%href
setcoli
getobjcoli


%index
findobj
�0����