%dll
hspmath
%ver
3.4
%date
2011/05/10
%group
数学関数
%port
Win
Mac
Cli
Let
%author
HSPWiki
%note
hspmath.asをインクルードすること。



%type
ユーザー定義マクロ
%index
M_LOG2E
2を底とするネイピア数の対数

%inst
2を底とするネイピア数の対数を表す定数です。
2を底とするネイピア数の対数とは、log2(m_e)のことを指します。

%group
数学定数

%sample
#include "hspmath.as"
	a = 10.0
	mes "2を底とするaの対数は" + log2(a) + "です。"
	mes "これはlogf(a) * m_log2eとしても求められます：" + (logf(a) * m_log2e) + "\n"

	mes "このようにm_log2eを「eを底とする対数」にかけることで、"
	mes "「2を底とする対数」を得ることができます。"
	stop

%href
M_LOG10E
M_E
log2

%type
ユーザー定義マクロ
%index
M_LOG10E
10を底とするネイピア数の対数

%inst
10を底とするネイピア数の対数を表す定数です。
10を底とするネイピア数の対数とは、log10(m_e)のことを指します。

%group
数学定数

%sample
#include "hspmath.as"
	a = m_e
	mes "10を底とするaの対数は" + log10(a) + "です。"
	mes "これはlogf(a) * m_log10eとしても求められます：" + (logf(a) * m_log10e) + "\n"

	mes "このようにm_log10eを「eを底とする対数」にかけることで、"
	mes "「10を底とする対数」を得ることができます。"
	stop

%href
M_LOG2E
log10
M_E


%type
ユーザー定義マクロ
%index
M_LN2
ネイピア数を底とした2の対数

%inst
ネイピア数を底とした2の対数を表す定数です。
ネイピア数を底とした2の対数とは、logf(2)のことを指します。

%group
数学定数

%href
M_LN10



%type
ユーザー定義マクロ
%index
M_LN10
ネイピア数を底とした10の対数

%inst
ネイピア数を底とした10の対数を表す定数です。
ネイピア数を底とした10の対数とは、logf(10)のことを指します。

%group
数学定数

%href
M_LN2


%type
ユーザー定義マクロ
%index
M_E
ネイピア数

%inst
ネイピア数（自然対数の底）eを表す定数です。

%href
M_PI
M_LOG2E
M_LOG10E

%group
数学定数

%url
http://ja.wikipedia.org/wiki/%E3%83%8D%E3%82%A4%E3%83%94%E3%82%A2%E6%95%B0


%type
ユーザー定義マクロ
%index
M_SQRTPI
円周率の平方根

%inst
円周率の平方根を表す定数です。

%href
sqrt
M_PI
M_SQRT2
M_SQRT3

%group
数学定数


%type
ユーザー定義マクロ
%index
M_SQRT2
2の平方根

%inst
2の平方根を表す定数です。

%href
sqrt
M_SQRTPI
M_SQRT3

%group
数学定数



%type
ユーザー定義マクロ
%index
M_SQRT3
3の累乗根

%inst
3の累乗根を表す定数です。

%href
sqrt
M_SQRTPI
M_SQRT2

%group
数学定数


%type
ユーザー定義マクロ
%index
DBL_DIG
10進数で有効な桁数

%inst
10進数で有効な桁数を表す定数です。

%group
doubleの限度



%type
ユーザー定義マクロ
%index
DBL_EPSILON
1.0とその次に大きい値との差

%inst
1.0とその次に大きい値との差を表す定数です。

%group
doubleの限度



%type
ユーザー定義マクロ
%index
DBL_MANT_DIG
仮数部のbit数

%inst
実数型の数値を表す際に用いる仮数部のbit数を表す定数です。

%group
doubleの限度


%type
ユーザー定義マクロ
%index
DBL_MAX
実数の最大値

%inst
実数で表現できる最大値を表す定数です。

%group
doubleの限度

%href
INT_MAX



%type
ユーザー定義マクロ
%index
DBL_MAX_10_EXP
10進数での指数部の最大値

%inst
指数部の最大値を表す定数です。

%href
DBL_MIN_10_EXP
DBL_MAX_EXP

%group
doubleの限度



%type
ユーザー定義マクロ
%index
DBL_MAX_EXP
2進数での指数部の最大値

%inst
指数部の最大値を表す定数です。

%group
doubleの限度

%href
DBL_MAX_10_EXP
INT_MAX


%type
ユーザー定義マクロ
%index
DBL_MIN
0を超える最小の値

%inst
0を超える最小の値を表す定数です。

%group
doubleの限度



%type
ユーザー定義マクロ
%index
DBL_MIN_10_EXP
10進数での指数部の最小値

%inst
指数部の最小値を表す定数です。

%href
DBL_MAX_10_EXP
DBL_MIN_EXP

%group
doubleの限度



%type
ユーザー定義マクロ
%index
DBL_MIN_EXP
2進数での指数部の最小値

%inst
指数部の最小値を表す定数です。

%href
DBL_MIN_10_EXP

%group
doubleの限度



%type
ユーザー定義マクロ
%index
INT_DIGIT
2進数で有効な桁数

%inst
2進数で有効な桁数を表す定数です。

%href
INT_DIGIT10

%group
intの限度



%type
ユーザー定義マクロ
%index
INT_DIGIT10
10進数で有効な桁数

%inst
10進数で有効な桁数を表す定数です。

%href
INT_DIGIT

%group
intの限度



%type
ユーザー定義マクロ
%index
INT_MAX
最大値

%inst
整数で表現できる最大値を表す定数です。

%href
DBL_MAX
INT_MIN

%group
intの限度



%type
ユーザー定義マクロ
%index
INT_MIN
最小値

%inst
整数で表現できる最小の値を表す定数です。

%href
INT_MAX

%group
intの限度





%index
pow
累乗（べき乗）を求める

%prm
(p1, p2)
p1 : 底（0以上）
p2 : 指数

%inst
p1をp2乗した値を求めます。結果は実数で与えられます。

p1は必ず正でなければなりません。負の場合はエラーにはなりませんが、非数（-1.#IND00）が返ります。

p2は正負どちらでも構いません。また、実数を指定することも可能です。

%sample
#include "hspmath.as"
	repeat 5, -2
		mes "10の" + cnt + "乗は" + pow(10, cnt) + "です。"
	loop
	stop
%href
powf
logf


%index
log10
10を底とした対数（常用対数）

%prm
(p1)
p1 : 真数

%inst
10を底とするp1の対数を求めます。結果は実数で与えられます。

%href
M_LOG10E
logf
log2
%index
log2
2を底とした対数

%prm
(p1)
p1 : 真数

%inst
2を底とするp1の対数を求めます。結果は実数で与えられます。

%href
M_LOG2E
logf
log10
%index
asin
サインの逆関数（アークサイン）

%prm
(p1)
p1 : 角度値（ラジアン）

%inst
p1のアークサイン（逆正弦）値を返します。
結果は実数型かつラジアン、-M_PI/2〜M_PI/2（度数法で-90°〜90°）の範囲で与えられます。

%href
acos
atan

%url
http://ja.wikipedia.org/wiki/%E9%80%86%E4%B8%89%E8%A7%92%E9%96%A2%E6%95%B0
%index
acos
コサインの逆関数（アークコサイン）

%prm
(p1)
p1 : 角度値（ラジアン）

%inst
p1のアークコサイン（逆余弦）値を返します。
結果は実数型かつラジアン、0〜M_PI（度数法で0°〜180°）の範囲で与えられます。

%href
asin
atan

%url
http://ja.wikipedia.org/wiki/%E9%80%86%E4%B8%89%E8%A7%92%E9%96%A2%E6%95%B0
%index
sinh
双曲線正弦関数（ハイパボリックサイン）

%prm
(p1)
p1 : 角度値（ラジアン）

%inst
p1のハイパボリックサイン（双曲線正弦）値を返します。
結果は実数で与えられます。

%href
cosh
tanh
asinh

%url
http://ja.wikipedia.org/wiki/%E5%8F%8C%E6%9B%B2%E7%B7%9A%E9%96%A2%E6%95%B0
%index
cosh
双曲線余弦関数（ハイパボリックコサイン）

%prm
(p1)
p1 : 角度値（ラジアン）

%inst
p1のハイパボリックコサイン（双曲線余弦）値を返します。
結果は実数で与えられます。

%href
sinh
tanh
acosh

%url
http://ja.wikipedia.org/wiki/%E5%8F%8C%E6%9B%B2%E7%B7%9A%E9%96%A2%E6%95%B0
%index
tanh
双曲線正接関数（ハイパボリックタンジェント）

%prm
(p1)
p1 : 角度値（ラジアン）

%inst
p1のハイパボリックタンジェント（双曲線正接）値を返します。
結果は実数で与えられます。

%href
sinh
cosh
atanh

%url
http://ja.wikipedia.org/wiki/%E5%8F%8C%E6%9B%B2%E7%B7%9A%E9%96%A2%E6%95%B0
%index
asinh
双曲線正弦関数の逆関数（アークハイパボリックサイン）

%prm
(p1)
p1 : 角度値（ラジアン）

%inst
p1のアークハイパボリックサイン（逆双曲線正弦）値を返します。
結果は実数で与えられます。

%href
acosh
atanh
sinh

%url
http://ja.wikipedia.org/wiki/%E5%8F%8C%E6%9B%B2%E7%B7%9A%E9%96%A2%E6%95%B0
%index
acosh
双曲線余弦関数の逆関数（アークハイパボリックコサイン）

%prm
(p1)
p1 : 角度値（ラジアン）

%inst
p1のアークハイパボリックコサイン（逆双曲線余弦）値を返します。
結果は実数で与えられます。

%href
asinh
atanh
cosh

%url
http://ja.wikipedia.org/wiki/%E5%8F%8C%E6%9B%B2%E7%B7%9A%E9%96%A2%E6%95%B0
%index
atanh
双曲線正接関数の逆関数（アークハイパボリックタンジェント）

%prm
(p1)
p1 : 角度値（ラジアン）

%inst
p1のアークハイパボリックタンジェント（逆双曲線正接）値を返します。
結果は実数で与えられます。

%href
asinh
acosh
tanh

%url
http://ja.wikipedia.org/wiki/%E5%8F%8C%E6%9B%B2%E7%B7%9A%E9%96%A2%E6%95%B0
%index
isfinite
有限／無限・非数の判定

%prm
(p1)
p1 : 判定する値

%inst
p1が有限の値ならば1を、p1が無限または非数の時は0を返します。

非数とは数値以外の型（文字列やラベル、モジュール型など）を指すものではありません。変数の型を調べる時はvartype関数を使用してください。

%sample
#include "hspmath.as"
	mes isfinite(10)            // 有限の値
	mes isfinite(sqrt(-1))      // 非数
	mes isfinite(expf(100))     // 有限の値
	mes isfinite(expf(1000))    // 無限大
	stop

%href
isnan
vartype
%index
isnan
非数の判定

%prm
(p1)
p1 : 判定する値

%inst
p1が非数の時は1を、それ以外の時は0を返します。

非数とは数値以外の型（文字列やラベル、モジュール型など）を指すものではありません。変数の型を調べる時はvartype関数を使用してください。

%sample
#include "hspmath.as"
	mes isnan(10)               // 有限の値
	mes isnan(sqrt(-1))         // 非数
	mes isnan(expf(100))        // 有限の値
	mes isnan(expf(1000))       // 無限大
	stop

%href
isfinite
vartype

%url
http://ja.wikipedia.org/wiki/%E9%9D%9E%E6%95%B0
%index
round
四捨五入

%prm
(p1)
p1 : 四捨五入する値（実数）

%inst
p1の小数点以下を四捨五入し、その結果を実数で返します。

%sample
#include "hspmath.as"
	repeat 10
		tmp = 1.0 + 0.1 * cnt
		mes strf("%0.1fを四捨五入すると", tmp) + strf("%0.1fになります。", round(tmp))
	loop
	stop

%href
intf
floor
ceil

%url
http://ja.wikipedia.org/wiki/%E7%AB%AF%E6%95%B0%E5%87%A6%E7%90%86
%index
sgn
符号

%prm
(p1)
p1 : 符号を判定する値（数値）

%inst
数値の符号を判定し、正ならば1を・負ならば-1を・ゼロならば0を返します。

%sample
#include "hspmath.as"
	tmp = 10
	sign = sgn(tmp)
	if sign == 1 {
		mes str(tmp) + "は正です。"
	} else : if sign == -1 {
		mes str(tmp) + "は負です。"
	} else {
		mes str(tmp) + "はゼロです。"
	}
	stop
%index
intf
0の方向へ丸め

%prm
(p1)
p1 : 丸める数値

%inst
p1を0の方向に丸めた数値を返します。結果は実数で返されます。

ここで「p1を0の方向に丸める」とは、「p1の小数点以下を切り捨てる」ことを意味します。

%sample
#include "hspmath.as"
	tmp = -2.5
	while(tmp <= 2.5)
		mes strf("%4.1f", tmp) + "を0の方向へ丸めると、" + strf("%4.1f", intf(tmp)) + "になります。"
		tmp += 0.5
	wend
	stop

%href
round
floor
ceil

%url
http://ja.wikipedia.org/wiki/%E7%AB%AF%E6%95%B0%E5%87%A6%E7%90%86
%index
floor
負の方向へ丸め

%prm
(p1)
p1 : 丸める数値

%inst
p1を負の方向に丸めた数値を返します。結果は実数で返されます。

ここで「p1を負の方向に丸める」とは、
html{
<ul><li>p1が正ならば小数点以下を切り捨てる</li><li>p1が負ならば小数点以下を切り上げる</ul>
}html
ことを意味します。

%sample
#include "hspmath.as"
	tmp = -2.5
	while(tmp <= 2.5)
		mes strf("%4.1f", tmp) + "を負の方向へ丸めると、" + strf("%4.1f", floor(tmp)) + "になります。"
		tmp += 0.5
	wend
	stop

%href
round
intf
ceil

%url
http://ja.wikipedia.org/wiki/%E7%AB%AF%E6%95%B0%E5%87%A6%E7%90%86
%index
ceil
正の方向へ丸め

%prm
(p1)
p1 : 丸める数値

%inst
p1を正の方向に丸めた数値を返します。結果は実数で返されます。

ここで「p1を正の方向に丸める」とは、
html{
<ul><li>p1が正ならば小数点以下を切り上げる</li><li>p1が負ならば小数点以下を切り捨てる</ul>
}html
ことを意味します。

%sample
#include "hspmath.as"
	tmp = -2.5
	while(tmp <= 2.5)
		mes strf("%4.1f", tmp) + "を正の方向へ丸めると、" + strf("%4.1f", ceil(tmp)) + "になります。"
		tmp += 0.5
	wend
	stop

%href
round
intf
floor

%url
http://ja.wikipedia.org/wiki/%E7%AB%AF%E6%95%B0%E5%87%A6%E7%90%86
%index
fmod
モジュロ

%prm
(p1, p2)
p1 : 割られる数
p2 : 割る数

%inst
p1をp2で割ったときの余りを実数で返します。
\記号による演算とは異なり、実数に対しても有効です。

%sample
#include "hspmath.as"
	mes 5 \ 2
	mes fmod(5, 2)      // 戻り値は実数となる
	mes fmod(3.5, 1.2)  // 実数を実数で割ったときの余りも求められる
	stop

%url
http://ja.wikipedia.org/wiki/%E3%83%A2%E3%82%B8%E3%83%A5%E3%83%AD
%index
distance2
2点の距離を求める

%prm
(p1, p2)
p1, p2 : 点の座標を代入した数値型配列変数

%inst
2点A，B間の距離を求めます。

p1とp2には点の座標を直交座標系で代入しておきます。
html{
<ul><li>p1(0)に点AのX座標</li><li>p1(1)に点AのY座標</li><li>p2(0)に点BのX座標</li><li>p2(1)に点BのY座標</li></ul>
}html
を代入した状態で呼び出してください。
結果は実数で返されます。

%sample
#include "hspmath.as"
	point_a = 100, 50       // 点A
	point_b = 500, 400      // 点B
	mes "2点間の距離は" + distance2(point_a, point_b) + "です。"
	line point_a(0), point_a(1), point_b(0), point_b(1)
	color 255  : pset point_a(0), point_a(1)    // 点A
	color ,255 : pset point_b(0), point_b(1)    // 点B
	stop
