;
;	HSP help manager用 HELPソースファイル
;	(先頭が「;」の行はコメントとして処理されます)
;
%type
拡張命令
%ver
3.6
%note
hgimg4dx.asまたはhgimg4.asをインクルードすること。
%author
onitama
%dll
hgimg4
%date
2019/03/14
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
ベクトル回転
%group
拡張画面制御命令
%prm
fv,x,y,z
fv      = FV値が代入されている変数名
(x,y,z) = 回転角度(実数値)
%inst
fvで指定された変数に格納されているFV値をX,Y,Z角度として、
小数値(X,Y,Z)で指定されたベクトルを回転させた結果を、変数fvに代入します。
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
ベクトル最小値
%group
拡張画面制御命令
%prm
fv,x,y,z
fv      = FV値が代入されている変数名
(x,y,z) = 比較値(実数値)
%inst
fvで指定された変数に格納されているFV値と、小数値(X,Y,Z)を比較して、値の大きいものを代入します。
FV値の各要素を最小値までに切り詰める場合に使用します。
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
ベクトル最大値
%group
拡張画面制御命令
%prm
fv,x,y,z
fv      = FV値が代入されている変数名
(x,y,z) = 比較値(実数値)
%inst
fvで指定された変数に格納されているFV値と、小数値(X,Y,Z)を比較して、値の小さいものを代入します。
FV値の各要素を最大値までに切り詰める場合に使用します。
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
角度の単位は整数で0～255で一周する値を使用します。
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
角度の単位は整数で0～255で一周する値を使用します。
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
モード値は、regobj命令で指定するものと同様です。
swは、以下のように動作します。
^p
	sw = 0 : 指定したモード値を追加
	sw = 1 : 指定したモード値を削除
	sw = 2 : 指定したモード値だけを設定
^p
%href
regobj
setobjmodel


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
val,id,distance
val      : 結果が代入される変数名
id       : オブジェクトID
distance : 衝突を検出する範囲(実数値)
%inst
指定したオブジェクトが持つコリジョン情報をもとに、そのオブジェクトが衝突している別なオブジェクトのIDを調べます。
distanceは、衝突する範囲(半径)を実数値で指定します。
衝突が検出された場合は、変数にオブジェクトIDが代入されます。
何も衝突が検出されなかった場合は、-1が代入されます。
^
HGIMG4では、distanceにマイナス値を指定した場合、3Dモデルが持つ衝突範囲(バウンディングボックス)にdistanceを掛けた値をもとに衝突検出を行ないます。
たとえば、-1.5を指定した場合は、衝突範囲を1.5倍に拡大した状態で、衝突検出が行なわれます。


%href
setcoli
findobj
nextobj


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
^p

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
オブジェクト検索
%group
拡張画面制御命令
%prm
exmode,group
exmode(0) : 検索を除外するモード
group(0)  : 検索対象コリジョングループ値
%inst
有効なオブジェクトを列挙します。
コリジョングループ値を指定した場合は、特定のコリジョングループに属するオブジェクトだけを検索します。
コリジョングループ値が0の場合は、すべてのオブジェクトが検索対象となります。
最初にfindobjを実行して、次にnextobj命令で該当するオブジェクトを検索することができます。
また、exmodeで指定したモード(regobjで指定するモード値と同じ)は検索から除外されます。
%href
setcoli
nextobj


%index
nextobj
次のオブジェクト検索
%group
拡張画面制御命令
%prm
val
val      : 結果が代入される変数名
%inst
findobj命令で指定された条件をもとにオブジェクトを検索します。
検索されると、変数にオブジェクトIDが代入されます。
検索対象がなくなった時には-1が代入されます。
%href
setcoli
findobj


%index
setborder
オブジェクト有効範囲設定
%group
拡張画面制御命令
%prm
fx,fy,fz,option
( fx,fy,fz ) : ボーダー領域の設定値(実数値)
option(0) : 設定オプション(0～2)
%inst
ボーダー領域(オブジェクト有効範囲)を設定します。
optionパラメーターにより、( fx,fy,fz )に設定する内容が変わります。
optionパラメーターを省略するか、または0の場合は、
( 0,0,0 )を中心にした、( fx,fy,fz )サイズの立方体がボーダー領域となります。
optionパラメーターが1の場合は、( fx,fy,fz )の座標を数値が小さい側のボーダー領域として設定します。
optionパラメーターが2の場合は、( fx,fy,fz )の座標を数値が大きい側のボーダー領域として設定します。

%href
regobj
setobjmode


%index
selmoc
MOC情報を設定
%group
拡張画面制御命令
%prm
id, mocofs
id     : オブジェクトID
mocofs : MOCのグループ指定
%inst
MOC設定命令の対象となるMOCグループを指定します。
idは、オブジェクトIDとなります。
通常は、selpos,selang,selscale,seldir命令をお使いください。
%href
selpos
selang
selscale
seldir
selcam
selcpos
selcang
selcint


%index
objgetfv
MOC情報を取得
%group
拡張画面制御命令
%prm
fv
fv      = FV値が代入される変数名
%inst
MOCに設定されている値を変数fvに代入します。
%href
objsetfv
fvset
fvadd
fvsub
fvmul
fvdiv


%index
objsetfv
MOC情報を設定
%group
拡張画面制御命令
%prm
fv
fv      = FV値が代入されている変数名
%inst
変数fvの内容をMOCに設定します。
%href
objgetfv
fvset
fvadd
fvsub
fvmul
fvdiv


%index
objaddfv
MOC情報を加算
%group
拡張画面制御命令
%prm
fv
fv      = FV値が代入されている変数名
%inst
変数fvの内容をMOCに加算します。
%href
objgetfv
fvset
fvadd
fvsub
fvmul
fvdiv


%index
objexist
オブジェクトIDが有効か調べる
%group
拡張画面制御命令
%prm
p1
p1(0) : オブジェクトID
%inst
p1で指定されたオブジェクトIDが有効であるか調べます。
オブジェクトIDが有効(登録済み)の場合は、システム変数statに0が代入されます。
オブジェクトIDが無効(未登録)の場合は、システム変数statに-1が代入されます。

%href
regobj
delobj



%index
event_wait
待ち時間イベントを追加
%group
拡張画面制御命令
%prm
id,p1
id      : イベントID
p1(0)   : 待ち時間(フレーム)
%inst
idで指定しているイベントIDに、待ち時間イベントを追加します。
待ち時間イベントは、p1で指定されたフレーム数だけ次のイベントに進むことを保留します。

%href
newevent
setevent


%index
event_jump
ジャンプイベントを追加
%group
拡張画面制御命令
%prm
id,p1,p2
id      : イベントID
p1(0)   : ジャンプ先のイベント番号
p2(0)   : ジャンプ無視の確率(%)
%inst
idで指定しているイベントIDに、ジャンプイベントを追加します。
ジャンプイベントは、指定されたイベント番号から実行を続けることを指示します。
イベントリストの中でのgoto命令にあたります。
p1で指定するイベント番号は、イベントに追加された順番に0,1,2…と数えたものになります。
p2で、ジャンプ無視の確率(%)を設定することができます。
0または省略された場合は、必ず(無条件)でジャンプを行ないます。
それ以外の場合は、乱数をもとに1～100%の確率でジャンプを行ない、
ジャンプしなかった場合は次のイベントに進みます。

%href
newevent
setevent


%index
event_prmset
パラメーター設定イベントを追加
%group
拡張画面制御命令
%prm
id,p1,p2
id      : イベントID
p1(0)   : パラメーターID(PRMSET_*)
p2(0)   : 設定される値
%inst
idで指定しているイベントIDに、パラメーター設定イベントを追加します。
パラメーター設定イベントは、p1で指定されたパラメーターIDにp2の値を設定します。
(それまでに設定されていた内容は消去されます)
パラメーターIDには、以下の名前を使用することができます。
^p
パラメーターID   内容
---------------------------------------
PRMSET_MODE      動作モード
PRMSET_FLAG      存在フラグ
PRMSET_SHADE     シェーディングモード
PRMSET_TIMER     タイマー
PRMSET_MYGROUP   コリジョングループ値
PRMSET_COLGROUP  対象グループ値
^p

%href
event_prmon
event_prmoff
newevent
setevent


%index
event_prmon
パラメータービット設定イベントを追加
%group
拡張画面制御命令
%prm
id,p1,p2
id      : イベントID
p1(0)   : パラメーターID(PRMSET_*)
p2(0)   : 設定されるビット
%inst
idで指定しているイベントIDに、パラメータービット設定イベントを追加します。
パラメータービット設定イベントは、p1で指定されたパラメーターIDにp2のビットを設定します。
(それまでに設定されていた内容は保持されたまま、新しい値のビットだけが有効になります)
パラメーターIDの詳細については、event_prmset命令を参照してください。

%href
event_prmset
event_prmoff
newevent
setevent


%index
event_prmoff
パラメータービット消去イベントを追加
%group
拡張画面制御命令
%prm
id,p1,p2
id      : イベントID
p1(0)   : パラメーターID(PRMSET_*)
p2(0)   : 消去されるビット
%inst
idで指定しているイベントIDに、パラメータービット消去イベントを追加します。
パラメータービット消去イベントは、p1で指定されたパラメーターIDから、
p2のビットだけを消去します。
パラメーターIDの詳細については、event_prmset命令を参照してください。

%href
event_prmset
event_prmon
newevent
setevent


%index
event_setpos
posグループ設定イベントを追加
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
event_setang
event_setangr
event_setscale
event_setdir
event_setefx
event_setwork
newevent
setevent


%index
event_setang
angグループ設定イベントを追加
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
event_setangr
event_setscale
event_setdir
event_setefx
event_setwork
newevent
setevent


%index
event_setangr
angグループ設定イベントを追加
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
event_setscale
event_setdir
event_setefx
event_setwork
newevent
setevent


%index
event_setscale
scaleグループ設定イベントを追加
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
event_setdir
event_setefx
event_setwork
newevent
setevent


%index
event_setdir
dirグループ設定イベントを追加
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
event_setefx
event_setwork
newevent
setevent


%index
event_setwork
workグループ設定イベントを追加
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
newevent
setevent


%index
event_pos
posグループ変化イベントを追加
%group
拡張画面制御命令
%prm
id,frame,x1,y1,z1,sw
id         : イベントID
frame      : 変化までのフレーム数
(x1,y1,z1) : 設定される値
sw(1)      : 補間オプション
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
swを省略した場合には、絶対値スプラインが設定されます。
swの値に16を加算した場合は、(x1,y1,z1)で設定される値の替わりにオブジェクトが持つworkグループの値を使用して変化を設定します。

%href
event_ang
event_angr
event_scale
event_dir
event_efx
event_work
newevent
setevent


%index
event_ang
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
swの値に16を加算した場合は、(x1,y1,z1)で設定される値の替わりにオブジェクトが持つworkグループの値を使用して変化を設定します。
(角度の指定は、setang命令と同様で、回転する順番はX->Y->Zとなります。他の順番で回転させるための、event_angy、event_angz命令が用意されています。)


%href
event_pos
event_angr
event_scale
event_dir
event_efx
event_work
newevent
setevent


%index
event_angr
angグループ変化イベントを追加
%group
拡張画面制御命令
%prm
id,frame,x1,y1,z1
id         : イベントID
frame      : 変化までのフレーム数
(x1,y1,z1) : 設定される値
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
swの値に16を加算した場合は、(x1,y1,z1)で設定される値の替わりにオブジェクトが持つworkグループの値を使用して変化を設定します。

%href
event_pos
event_ang
event_scale
event_dir
event_efx
event_work
newevent
setevent


%index
event_scale
scaleグループ変化イベントを追加
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
swの値に16を加算した場合は、(x1,y1,z1)で設定される値の替わりにオブジェクトが持つworkグループの値を使用して変化を設定します。

%href
event_pos
event_ang
event_angr
event_dir
event_efx
event_work
newevent
setevent


%index
event_dir
dirグループ変化イベントを追加
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
swの値に16を加算した場合は、(x1,y1,z1)で設定される値の替わりにオブジェクトが持つworkグループの値を使用して変化を設定します。

%href
event_pos
event_ang
event_angr
event_scale
event_efx
event_work
newevent
setevent


%index
event_work
workグループ変化イベントを追加
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
newevent
setevent


%index
event_addpos
posグループ加算イベントを追加
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
event_addang
event_addangr
event_addscale
event_adddir
event_addefx
event_addwork
newevent
setevent


%index
event_addang
angグループ加算イベントを追加
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
event_addangr
event_addscale
event_adddir
event_addefx
event_addwork
newevent
setevent


%index
event_addangr
angグループ加算イベントを追加
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
event_addscale
event_adddir
event_addefx
event_addwork
newevent
setevent


%index
event_addscale
scaleグループ加算イベントを追加
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
event_adddir
event_addefx
event_addwork
newevent
setevent


%index
event_adddir
dirグループ加算イベントを追加
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
event_addefx
event_addwork
newevent
setevent


%index
event_addwork
workグループ加算イベントを追加
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
newevent
setevent


%index
setevent
オブジェクトにイベントを設定
%group
拡張画面制御命令
%prm
p1,p2,p3
p1(0)  : オブジェクトID
p2(0)  : イベントID
p3(-1) : イベントスロットID
%inst
p1で指定したオブジェクトにp2のイベントを適用します。
あらかじめ、決まった流れの処理(イベント)を登録したイベントリストを用意しておく必要があります。
^
setevent命令によって設定されるイベントは、オブジェクト１つあたり４つまで同時に適用することが可能です。
p3にイベントを設定するためのイベントスロットID(0から3まで)を指定することができます。
p3を省略するか、-1を指定した場合には0から順番に空いているイベントスロットIDが使用されます。
オブジェクトに設定されたイベントを削除する場合には、p3にイベントスロットIDを指定して、p2をマイナス値にしてください。
^
イベントの設定に成功した場合には、システム変数statに設定されたイベントスロットIDが代入されます。
イベントの設定に失敗すると、システム変数statに-1が代入されます。


%href
newevent


%index
delevent
イベントリストを削除
%group
拡張画面制御命令
%prm
p1
p1 : イベントID
%inst
p1で指定したイベントリストを削除します。

%href
newevent


%index
newevent
イベントリストを作成
%group
拡張画面制御命令
%prm
p1
p1 : イベントIDが代入される変数名
%inst
新しいイベントIDを取得し、p1で指定した変数に代入します。
^
新しくイベントを作成する場合には、必ずnewevent命令でイベントIDを取得しておく必要があります。
次に、「event_」で始まるイベントリスト追加命令によって多彩な動作を登録しておくことができます。
一度取得されたイベントIDは、シーンのリセット(hgreset命令)が行なわれるか、
またはdelevent命令によってイベントリストが削除されるまでは保持されます。
^
こうしてできたイベントは、setevent命令によっていつでもオブジェクトに対して適用することができます。


%href
delevent
setevent


%index
getang
angグループ情報を取得
%group
拡張画面制御命令
%prm
id,x,y,z
id      : オブジェクトID
(x,y,z) : 取得する変数

%inst
オブジェクトの持つパラメーターを取得します。
angグループ(表示角度)の内容が(x,y,z)で指定された変数に代入されます。
(x,y,z)は、実数型の変数として設定されます。
命令の最後に「i」を付加することで、整数値として値を取得することができます。

%href
getangi
getpos
getangr
getscale
getdir
getefx
getwork


%index
getangr
angグループ情報を取得
%group
拡張画面制御命令
%prm
id,x,y,z
id      : オブジェクトID
(x,y,z) : 取得する変数

%inst
オブジェクトの持つパラメーターを取得します。
angグループ(表示角度)の内容が(x,y,z)で指定された変数に代入されます。
(x,y,z)は、整数型の変数として設定されます。
角度の単位は整数で0～255で一周する値を使用します。

%href
getpos
getang
getscale
getdir
getefx
getwork


