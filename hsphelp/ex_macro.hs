%ver
3.3

%date
2011/05/10

%author
onitama

%url
http://hsp.tv/

%type
マクロ

%group
標準定義マクロ

%port
Win
Cli
Let

%index
_debug
デバッグモード時

%inst
デバッグモード時のみ定義されるマクロです。
#ifdef、#ifndef命令などで判別して分岐させることが可能です。

%sample
#ifdef _debug
	mes "デバッグモードです。"
#else
	mes "デバッグモードではありません。"
#endif
	stop
%href
hspstat

%index
__hsp30__
ver3.0以降使用時

%inst
HSP3.0以降で、「HSP拡張マクロを使用する」にチェックが入っている時に定義されます。

%sample
#ifdef __hsp30__
	mes {"HSP3.0以降で、「HSP拡張マクロを使用する」に
	チェックが入っています。"}
#else
	mes {"HSP3.0より古いか、「HSP拡張マクロを使用する」に
	チェックが入っていません。"}
#endif
	stop

%index
__file__
使用時点で解析されているファイル名

%inst
使用時点で解析されているファイル名に置き換えられます。
ファイルが保存されていない場合は"???"に置き換わります。

%sample
	mes "使用時点で解析されているファイル名は" + __file__
	stop

%href
__line__

%index
__line__
使用時点で解析されている行番号

%inst
使用時点で解析されている行番号に置き換わります。行番号は1から始まる整数値です。

%sample
	mes "使用時点で解析されている行番号は" + __line__
	stop

%href
__file__

%index
__date__
使用時点の日付

%inst
使用時点（コンパイルした時点）の日付に置き換わります。日付はYYYY/MM/DD形式で表されます。

「HSP拡張マクロを使用する」にチェックが入っていないと使用できません。

%sample
	mes "使用時点の日付は" + __date__
	stop

%href
__time__

%index
__time__
使用時点の時刻

%inst
使用時点（コンパイルした時点）の時刻に置き換わります。時刻はHH:MM:SS形式で表されます。

「HSP拡張マクロを使用する」にチェックが入っていないと使用できません。

%sample
	mes "使用時点の時刻は" + __time__
	stop

%href
__date__

%index
__hspver__
HSPバージョン番号

%inst
HSPのバージョン番号に置き換わります。上位8bit、下位8bitがそれぞれメジャーバージョン・マイナーバージョンを示します。 

「HSP拡張マクロを使用する」にチェックが入っていないと使用できません。

%sample
	mes "HSPのバージョン番号は" + strf("%4x", __hspver__)
	stop

%href
hspver
