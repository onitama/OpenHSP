;
;	HSP help manager用 HELPソースファイル
;	(先頭が;の行はコメントとして処理されます)
;

%type
内蔵命令
%ver
3.7
%note
ver3.7標準命令
%date
2023/08/25
%author
onitama
%url
http://hsp.tv/
%port
Win
Cli
Let




%index
getkey
キー入力チェック
%group
基本入出力制御命令
%prm
p1,p2
p1=変数   : 読み込むための変数
p2=1〜(1) : キーコード

%inst
キーボード及びマウスボタンの状態をチェックして変数に代入します。指定したボタンが押されていれば、1を代入し、押されていなければ0が代入されます。
キーコードで指定する値の詳細は、以下の通りです。
^p
  キーコード : 実際のキー
 ------------------------------------------
        1    : マウスの左ボタン
        2    : マウスの右ボタン
        3    : キャンセル（[CTRL]+[BREAK]）
        4    : ３ボタンマウスのまん中のボタン
        8    : [BACKSPACE]（PC98の[BS]）
        9    : [TAB]
       13    : [ENTER]
       16    : [SHIFT]
       17    : [CTRL]
       18    : [ALT]（PC98の[GRPH]）
       20    : [CAPSLOCK]
       27    : [ESC]
       32    : スペースキー
       33    : [PAGEUP]（PC98の[ROLLDOWN]）
       34    : [PAGEDOWN]（PC98の[ROLLUP]）
       35    : [END]（PC98の[HELP]）
       36    : [HOME]（PC98の[HOMECLR]）
       37    : カーソルキー[←]
       38    : カーソルキー[↑]
       39    : カーソルキー[→]
       40    : カーソルキー[↓]
   48〜57    : [0]〜[9]（メインキーボード）
   65〜90    : [A]〜[Z]
  96〜105    : [0]〜[9]（テンキー）
 112〜121    : ファンクションキー [F1]〜[F10]
^p
この表に載っているキー以外でも、取得できる場合があります。(サンプルを実行するとキーコードを調べることができます。)


%sample
title "キー入力してください(^^)v"
onkey *inkey
onclick *inkey
stop
*inkey
if lparam>>30:stop
mes wparam
stop


%href
stick




%index
mouse
マウスカーソル座標設定
%group
基本入出力制御命令
%prm
p1,p2,p3
p1,p2 : 設定するX,Y座標
p3(0) : 設定モード値
%inst
マウスカーソルをp1,p2で指定した座標に変更します。
指定する座標は、  ウィンドウ内の座標ではなくディスプレイでの座標(通常は、X=0〜ginfo_dispx/Y=0〜ginfo_dispy)を指定します。
p1とp2の指定を省略した場合は、現在の座標がそのまま適用されます。
p3の値で、マウスカーソルの表示・非表示に関する設定モードを決めます。p3の値は、以下のように動作します。
^p
モード値  内容
-------------------------------------------------------------------------
    0     p1またはp2の値がマイナスの時にマウスカーソルを非表示に設定する
          それ以外の時は、(p1,p2)の座標にマウスカーソルを移動して表示する
   -1     (p1,p2)の座標にマウスカーソルを移動し、非表示に設定する
    1     (p1,p2)の座標にマウスカーソルを移動し、表示・非表示の設定は維持する
    2     (p1,p2)の座標にマウスカーソルを移動し、非表示設定の場合は表示する
^p
p3の値が0(デフォルト値)の場合は、p1または、p2がマイナス値の場合に、HSPウィンドウ上でマウスカーソル表示をOFF(非表示)に設定します。
マウスカーソルの表示をOFFにした後は、有効な座標を設定することで再び表示されるようになります。
p3の値が0以外の場合は、(p1,p2)の値とマウスカーソルのON/OFFに関係なく座標を指定することができます。これは、マルチモニタ環境などでマイナス値の座標を扱う必要がある場合などに使用します。

%port-
Let




%index
randomize
乱数発生の初期化
%group
基本入出力制御命令
%prm
p1
p1=0〜(不定) : 乱数の初期化パラメーター

%inst
rnd関数で発生させる乱数のパターンを初期化します。
^
p1に同じ値を指定して初期化された乱数は、常に同じパターンで乱数を発生させられます。
p1を省略した場合は、 Windowsのタイマから得られた不定な値を使って初期化します。これによって、毎回まったく違う乱数を発生させることができます。

%href
rnd




%index
stick
キー入力情報取得
%group
基本入出力制御命令
%prm
p1,p2,p3
p1=変数    : 読み込むための変数
p2=0〜(0)  : 非トリガータイプキー指定
p3=0〜1(1) : ウィンドウアクティブチェックON/OFF

%inst
よく使われるキーボード及びマウスボタンの状態をまとめてチェックして変数に代入します。
^
stick命令が実行されると以下のような複数のボタン情報が１つの数値として、p1で指定した変数に代入されます。
^p
値(10進)  値(16進) キー
-------------------------------------
     1  : $00001 : カーソルキー左(←)
     2  : $00002 : カーソルキー上(↑)
     4  : $00004 : カーソルキー右(→)
     8  : $00008 : カーソルキー下(↓)
    16  : $00010 : スペースキー
    32  : $00020 : Enterキー
    64  : $00040 : Ctrlキー
   128  : $00080 : ESCキー
   256  : $00100 : マウスの左ボタン
   512  : $00200 : マウスの右ボタン
  1024  : $00400 : TABキー
  2048  : $00800 : [Z]キー
  4096  : $01000 : [X]キー
  8192  : $02000 : [C]キー
 16384  : $04000 : [A]キー
 32768  : $08000 : [W]キー
 65536  : $10000 : [D]キー
131072  : $20000 : [S]キー
^p
何もボタンが押されていない場合には0が代入されます。
^
もし複数のボタンが同時に押されていた場合には、それらの数値がすべて加算されます。
たとえば、カーソルキーの右とスペースキーが同時に押されている場合には、4+16の20が変数に読み込まれます。
この数値をif命令などでチェックする場合には、演算子「&」が役に立ちます。
^p
例 :
	stick a,0            ; 変数aにキー状態を読み出し
	if a&16 : goto *spc  ; スペースが押されたか?
	if a&32 : goto *ent  ; Enterが押されたか?
^p
このように「変数&キー情報」で複数のボタン情報が入った数値から、 １つだけのキー情報を取り出すことができます。
^
stick命令は、 getkey命令によく似ていますが、最大の違いはボタンが押された瞬間だけを検出する点です。つまり、ボタンが押された時に１度だけ押されている情報を返し、あとは押されているボタンをはなすまでは、押されていることになりません。
^
ただし、p2の非トリガータイプキー指定により、押しっぱなしであっても情報を検出することができるようになります。
^
p2に押しっぱなしでも検出されるキーのコード(上の表にあるコードです。 複数の場合はそれぞれの数値を加えます)を指定すると、 そのキーだけはボタンが押されている間ずっと検出されるようになります。
^
この命令は、非常に複雑に見えますがキーを使ったスクリプトを作る際にとても便利な機能となるでしょう。
たとえば、シューティングゲームを思い浮かべてみてください。自分の機体は上下左右の方向にボタンが押されている間ずっと移動しなければなりません。しかし、ミサイルを発射するボタンは１回押してはなすまでは次の弾は発射されません。
このような場合には、上下左右のキーだけは非トリガータイプのキーに指定して、それ以外は１度だけ押された情報を返すようにすればいいわけです。
^
また、p3でウィンドウがアクティブでない場合は入力を無効にする機能をON/OFFすることができます。
p3が1か省略された場合は、HSPウィンドウがアクティブでない場合にはキー入力が無効になります。
p3が0の場合は、すべての状況下でキー入力を行ないます。


%href
getkey
jstick



%index
logmes
デバッグメッセージ送信
%group
HSPシステム制御命令
%prm
"message"
"message" : ログに記録するメッセージ

%inst
デバッグウィンドウ表示時に、"message"の内容をデバッグログに記録します。
ある時点での変数の内容や、通過チェックなどに利用できます。
スクリプトエディタからデバッグウィンドウの表示モードを設定するか、assert命令によってデバッグウィンドウを表示させておく必要があります。
変数の代入を行った際にログを記録する場合は、logmesv、logmesva命令を使用することができます。
実行ファイル作成時は、この命令は無効になります。

%href
logmesv
logmesva
assert
varprop



%index
logmesv
変数デバッグメッセージ表示設定
%group
HSPシステム制御命令
%prm
var
var   : デバッグメッセージを設定する変数名
%inst
変数デバッグメッセージの表示設定を行います。
これにより、デバッグウィンドウ表示時に、指定された変数のデバッグメッセージを記録することができます。
varパラメーターに変数を指定することでデバッグメッセージの表示が有効になります。この場合、配列変数の要素1以降は表示されません。配列変数のすべての要素でデバッグメッセージの表示を有効にする場合は、logmesva命令を使用してください。
^
以下の例では、変数aにデバッグメッセージ表示を設定しています。
^p
例 :
	logmesv a		; 変数aの代入時にデバッグメッセージが表示される
^p
デバッグメッセージ表示が設定された変数は、代入などで内容が変更された際にデバッグメッセージが記録されます。
変数デバッグメッセージは、以下のように表示されます。
^p
#set[a=0] line:17 (test_logmes.hsp)
#set[b=456] line:22 (test_logmes.hsp)
#set[b(1)=123] line:22 (test_logmes.hsp)
#set[b(2)=789] line:22 (test_logmes.hsp)
#set[c="abcdefgh..."] line:23 (test_logmes.hsp)
^p
#setに続いて設定された「変数名=内容」が表示されます。内容表示は、あくまでも簡易的なものになります。文字列は先頭の16文字までが表示されます。
配列変数は、「変数名(要素)」として表示されますが、2次元以上の配列要素は1次元として扱われますので注意してください。
末尾には、「line:行番号 (スクリプトファイル)」という形で、どの行とスクリプトファイルで値が設定されたかという情報が記録されます。
変数デバッグメッセージにより、スクリプト内で意図せずに内容が変更されてしまったり、正しく代入されないといったミスをチェックすることができます。
^
変数に設定されたデバッグメッセージの表示を無効にする場合は、varprop命令を使用してください。varprop命令によって デバッグメッセージの設定を変更することが可能です。
^
デバッグウィンドウの表示は、スクリプトエディタからデバッグウィンドウの表示モードを設定するか、assert命令によってデバッグウィンドウを表示させる必要があります。
実行ファイル化したスクリプトや、プラットフォーム変換された環境(iOS,android)では、この命令は無効になりますので注意してください。
%href
logmes
logmesva
assert
varprop


%index
logmesva
配列変数デバッグメッセージ表示設定
%group
HSPシステム制御命令
%prm
var
var   : デバッグメッセージを設定する変数名
%inst
変数デバッグメッセージの表示設定を行います。
これにより、デバッグウィンドウ表示時に、指定された変数のデバッグメッセージを記録することができます。
varパラメーターに変数を指定することでデバッグメッセージの表示が有効になります。
この命令は、logmesv命令と同様に動作します。logmesv命令との違いは、配列変数のすべての要素でデバッグメッセージの表示が有効になる点です。
配列を使用しない変数のデバッグメッセージを記録する場合は、logmesv命令をご使用ください。
デバッグウィンドウの表示は、スクリプトエディタからデバッグウィンドウの表示モードを設定するか、assert命令によってデバッグウィンドウを表示させる必要があります。
実行ファイル化したスクリプトや、プラットフォーム変換された環境(iOS,android)では、この命令は無効になりますので注意してください。
%href
logmes
logmesva
assert
varprop


%index
assert
デバッグウィンドウ表示
%group
HSPシステム制御命令
%prm
p1
p1(0) : デバッグ時の条件式

%inst
プログラムを一時的に中断してデバッグウィンドウを表示します。
p1に条件式が指定された場合には、p1の条件が正しくない場合にのみデバッグウィンドウを表示します。
(p1には、通過する時の条件式を書くことになるので注意してください。)
^p
	assert a>5  ; aが5以下の時にデバッグする
^p
実行ファイル作成時は、この命令は無効になります。

%href
logmes
logmesv
%port-
Let



%index
mcall
メソッドの呼び出し
%group
基本入出力制御命令
%prm
p1,p2,p3…
p1 : 変数名
p2 : メソッド名
p3 : パラメーター

%inst
p1で指定された変数の型に応じてメソッドを呼び出します。
p1の変数にCOMオブジェクト型を指定することで、COMオートメーションのメソッドを呼び出すことが可能です。
p2でメソッド名(文字列)または、ディスパッチID(DISPID)を指定して、p3以降に引数を指定します。
p3以降のパラメーター数や、型はそのまま適切に変換されメソッドに渡されます。
メソッドを実行した結果の返値は、comres命令で設定された変数に代入されます。
また、メソッド実行が成功した場合にはシステム変数 statは0になり、実行エラーが起こった場合には、 システム変数statに結果コード(HRESULT)が代入されます。

mcall命令は、拡張された変数の型を用意することで、 新しい機能を提供することが可能です。標準では、COMオブジェクト型のみに対応しています。

%href
#usecom
newcom
delcom
querycom
comres
%port-
Let



%index
setease
イージング関数の計算式を設定
%group
基本入出力制御命令
%prm
p1,p2,p3
p1 : 出力される最小値(実数値)
p2 : 出力される最大値(実数値)
p3 : 計算式のタイプ値
%inst
決められた範囲の数値を任意の計算式で補間するイージング関数の設定を行ないます。
ここで指定された設定は、イージング関数(getease,geteasef)で値を取得する際に反映されます。
計算式のタイプ値には、以下を指定することができます。
^p
	マクロ名                補間内容
	------------------------------------------------------------
	ease_linear		リニア(直線補間)
	ease_quad_in		加速(Quadratic)
	ease_quad_out		減速(Quadratic)
	ease_quad_inout		加速→減速(Quadratic)
	ease_cubic_in		加速(Cubic)
	ease_cubic_out		減速(Cubic)
	ease_cubic_inout	加速→減速(Cubic)
	ease_quartic_in		加速(Quartic)
	ease_quartic_out	減速(Quartic)
	ease_quartic_inout	加速→減速(Quartic)
	ease_bounce_in		バウンス効果(入)
	ease_bounce_out		バウンス効果(出)
	ease_bounce_inout	バウンス効果(入出)
	ease_shake_in		シェイク効果(入)
	ease_shake_out		シェイク効果(出)
	ease_shake_inout	シェイク効果(入出)
	ease_loop		補間のループ(*)

(*)で示されたタイプは、他のタイプに付加することができます。
^p
計算式のタイプ値が省略された場合は、以前に設定された値がそのまま使用されます。

イージング関数は、自然な動きのアニメーションを得るための基本的な計算をサポートします。
たとえば、X座標が100だった物体を、X座標200まで50フレームのアニメーションで移動させるとします。
通常であれば、1フレームごとにX座標を100,102,104,106…というように2ずつ加算して新しい座標を得ることでアニメーションとなります。
しかし、これは直線的な動きにしかなりません。イージング関数は、フレームごとの座標を特定の計算式から取得することができます。
計算式の設定によって、ゆっくり移動を開始して、加速しながら移動、目標の前でまた減速するといった有機的なアニメーションを実現できるほか、放物線を描いてバウンドするような動き、振り回す(シェイクする)ような動きなど様々な用途に使用することができます。
イージング関数を使用するには、まずsetease命令で値が変化する範囲と、計算式を指定します。
^p
	;	イージング関数の設定
	setease 100,200,ease_cubic_inout
^p
上の例では、100から200までの値を得るためのイージング関数をease_cubic_inoutの計算式で設定します。
次に、getease または geteasef関数により実際の値を取得します。
geteaseと、geteasefは基本的に同じもので、取得される値が整数値か、実数値かが異なります。
通常の座標を扱う場合は、整数値として取得しても問題ありません。(イージング関数の内部ではどちらも、実数による計算が行なわれています)
^p
	;	イージング関数の設定
	setease 100,200,ease_cubic_inout
	i=0
	repeat
		redraw 0
		color 0,0,0:boxf	; 画面をクリア
		x = getease(i,50)	; イージング値の取得(整数)
		color 255,255,255
		pos x,100 : mes "●"
		redraw 1
		await 30
		i=i+1
	loop
^p
getease関数の引数は、getease(時間経過値,最大値)となります。
時間経過値は、0から始まる整数値で、最大値で指定された値までを指定します。
つまり上の例で言えば、getease(0,50)は、setease命令で設定した100から200までの範囲でのスタート値、つまり100が返されます。
時間経過値が増えるごとに、100から200に向けて返される値も増加していきます。そして、getease(50,50)になった時に200が返るような計算式になっています。
^p
	時間経過値が0の時 = setease命令で指定した出力最小値が返される
	時間経過値が最大値の時 = setease命令で指定した出力最大値が返される
^p
最大値のパラメーターを省略した場合は、4096が使用されます。
geteasef関数の場合も、引数はgeteasef(時間経過値,最大値)と変わりません。
ただし、時間経過値、最大値ともに実数を使用することができ、より細かい精度でイージング関数を利用できます。また、geteasef関数で最大値のパラメーターを省略した場合は、1.0が使用されます。

通常は、時間経過値がマイナスの値だった場合は、0とみなされます。また、時間経過値が最大値を超えた場合も、最大値として扱われます。
ただし、setease命令による計算式のタイプ設定で、ease_loop(補間のループ)を加算した場合は、範囲外の値も含めて補間のループ(繰り返し)を行ないます。補間のループでは、時間経過値にしたがって出力最小値、出力最大値を往復するような動きになります。

イージング関数は、馴れないうちは結果が想像しにくいところがありますが、使いこなすことで高度なアニメーションを手軽に利用することができる便利な機能です。
また標準で内蔵されている命令のため、HSP3DishやHGIMG4などあらゆるランタイムでも同様に呼び出すことができます。

%href
getease
geteasef
%port-
Let



%index
sortval
配列変数を数値でソート
%group
基本入出力制御命令
%prm
p1,p2
p1 : 数値型の配列変数名
p2 : 並び順 (0=小さい順/1=大きい順)
%inst
数値が格納された配列変数を指定された並び順でソート(並び替え)します。
p1で指定された配列変数を直接並べ替えます。

並び替えに関する情報は、sortget命令で得ることができます。
%href
sortget
%port-
Let


%index
sortstr
配列変数を文字列でソート
%group
基本入出力制御命令
%prm
p1,p2
p1 : 文字列型の配列変数名
p2 : 並び順 (0=小さい順/1=大きい順)
%inst
文字列が格納された配列変数を指定された並び順でソート(並び替え)します。
p1で指定された配列変数を直接並べ替えます。
ソートの並び順は、 ASCIIコードの大きい小さいで比較されるので、 ABC順、あいうえお順に並べ替えられます。

並び替えに関する情報は、sortget命令で得ることができます。
%href
sortget
%port-
Let



%index
sortnote
メモリノート文字列をソート
%group
基本入出力制御命令
%prm
p1,p2
p1 : メモリノート形式の文字列型の変数名
p2 : 並び順 (0=小さい順/1=大きい順)
%inst
メモリノートパッド形式の 文字列が格納された変数を、 指定された並び順でソート(並び替え)します。
p1で指定された変数を直接並べ替えます。
ソートの並び順は、 ASCIIコードの大きい小さいで比較されるので、 ABC順、あいうえお順に並べ替えられます。

並び替えに関する情報は、sortget命令で得ることができます。
メモリノートパッド形式は、 notesel,  noteget命令などで使用することのできる「\n」(改行コード)で区切られたデータ列のことです。
%href
notesel
sortget
%port-
Let



%index
sortget
ソート元のインデックスを取得
%group
基本入出力制御命令
%prm
p1,p2
p1 : 結果が代入される変数名
p2 : インデックスNo.
%inst
sortstr, sortval, sortnote命令を実行後の配列において、格納されているデータは、ソートする前はどのインデックスに置かれていたものかを調べて結果を返します。 
たとえば、配列変数aをソートした後、 sortget n,4という命令で、1という値が返ってきたとすると、a(4)という配列変数に現在入っている値は、ソートする前には、a(1)に入っていたことを示します。
この命令は、データの一部だけをソートして、その情報をもとにほかのデータの並び替えも行なうような時に有効です。
%href
sortstr
sortval
sortnote
%port-
Let



%index
varprop
変数のデバッグ設定を行う
%group
HSPシステム制御命令
%prm
var,p1
var   : デバッグ設定を行う変数名
p1(0) : デバッグ設定値(varprop_*)
%inst
変数のデバッグを行うための設定を行います。
これは、指定された変数に対して型の固定、値の固定、デバッグログの記録などを個別に指定することで、より安全な変数の運用と不具合の発見を支援するものです。
varパラメーターに変数名を指定し、p1パラメーターでデバッグ設定値を指定します。(既に設定されているデバッグ設定値は維持されます。)
デバッグ設定値は以下のマクロを使用することができます。
^p
	マクロ名         値     設定内容
	------------------------------------------------------------
	varprop_xtype     1     変数型を固定する
	varprop_xvalue    2     変数値を固定する
	varprop_log       4     変数デバッグ記録を有効にする
	varprop_logarray  8     配列変数デバッグ記録を有効にする
^p
それぞれの値は、加算することで同時に指定することが可能です。
「varprop_xtype」を設定した場合は、変数の型が固定されます。これ以降は、変数の型が禁止された状態になり、実行中に変数型が変更された場合はエラーが発生します。
これは、プリプロセッサ命令「#varint」などで変数型を固定した場合と同様の動作になります。
「varprop_xvalue」を設定した場合は、その時点で代入されていた値に変数値が固定されます。これ以降は、実行中に変数値が変更された場合はエラーが発生します。
これにより変数値や変数型を固定し、意図しない変数の初期化や値の上書きを発見することができます。
「varprop_log」「varprop_logarray」を設定した場合は、デバッグウィンドウのログに変数代入時の変数デバッグメッセージの表示を行います。これは、「logmesv」「logmesva」命令と同等の機能になります。変数デバッグメッセージについての詳細は、logmesv命令のヘルプを参照してください。
p1パラメーターを省略するか0を指定した場合は、すべての変数デバッグ設定を解除します。
^
varprop命令は、assert命令などと同様にデバッグ実行時の支援を行うものです。実行ファイル化したスクリプトや、プラットフォーム変換された環境(iOS,android)では、この命令は無効になりますので注意してください。
%href
logmesv
logmesva
#varint
assert


