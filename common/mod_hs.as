
	;
	;	hsファイルを扱うためのモジュール
	;	HSP3用( onion software/onitama 1999-2020 )
	;
	;   *このモジュールは自由に組み込んで頂いて構いません
	;
	;	ihelp_init
	;		変数の初期化を行ないます。
	;		スクリプトの最初に「#include "mod_hs.as"」を入れて
	;		必ず始めにihelp_initを通るようにしてください。
	;
	;	ihelp_find "keyword"
	;		( keyword = 検索キーワード )
	;		指定されたキーワードを検索します。
	;		検索に該当する項目数がシステム変数statに、
	;		検索に該当したキーワード一覧がih_ansbufに返ります。
	;		検索のためには、インデックスファイル(hsphelp.idx)が
	;		カレントディレクトリにある必要があります。
	;
	;	ihelp_open n
	;		( n=候補No. )
	;		キーワード検索の中から、n番目(0〜)の項目を取得します。
	;		取得した情報は、ih_infoを始めとする各種変数に代入されます。
	;
	;	ihelp_info
	;		ihelp_openだけでは取得されない、拡張情報を得ます。
	;		結果は、ih_optに代入されます。
	;

*ihelp_boot
	;
	;	hsファイルライブラリ初期化
	;	(参照用グローバル変数)
	;
	ih_file="hsphelp.idx"			; indexファイル
	;
	sdim ih_ansbuf,$8000			; 検索候補バッファ
	sdim ih_info,$8000				; メッセージ表示バッファ(32K)
	sdim ih_refinf,$4000			; 関連項目表示バッファ(16K)
	sdim ih_refsel,$4000			; 関連項目表示バッファ(16K)
	sdim ih_group,256
	sdim ih_ans_key,64
	sdim ih_ans_name,64
	sdim ih_ans_title,256
	sdim ih_ans_dll,64
	sdim ih_prgsmp,$4000			; サンプル情報(16K)
	sdim ih_prminf,$4000			; パラメータ情報(16K)
	sdim ih_fncprm,$1000			; パラメータ情報2(4K)
	sdim ih_opt,$4000				; 詳細情報表示バッファ(16K)

	ih_htmopt = 0					; html出力 ON/OFF flag

	sdim cb_group,512

	if sysinfo(3)=1 {
		cb_group={"命令概要
プリプロセッサ命令
代入命令
特殊代入命令
プログラム制御命令
プログラム制御マクロ
基本入出力制御命令
拡張入出力制御命令
オブジェクト制御命令
画面制御命令
拡張画面制御命令
文字列操作命令
メモリ管理命令
マルチメディア制御命令
ファイル操作命令
拡張ファイル操作命令
通信制御命令
OSシステム制御命令
HSPシステム制御命令
その他の命令
"}
	} else {
		cb_group={"Abstract
Preprocess command
Substitution command
Special assignment command
Program control command
Program control macro
Basic I/O control command
Extended I/O control command
Object control command
Screen control command
Extended screen control command
String manipulation command
Memory management command
Multimedia control command
File operation command
Extended file operation command
Communication control command
OS system control command
HSP system control command
Other commands
"}
}




#module "ihelp"
#deffunc ihelp_init var _p1, int _p2
	;
	;	hsphelpライブラリ初期化
	;		ihelp_init msg, flag
	;		( msg=結果文字列が代入されるバッファ, flag=1の場合はindex再構築 )
	;
	ih_msg="Init HSPhelp.\n"

	idxerror=0				; index構築エラーフラグ

	sdim fl,$4000
	dirlist fl,"*.hs",1
	notesel fl
	flmax=notemax 
	if flmax=0 : goto *failidx

	fname2=ih_file@				; 書き出しファイル
	if _p2 : goto *initidx

	exist fname2
	if strsize>0 : goto *loadidx

*initidx
	ih_msg+="Rebuild index.\n"
	if idxerror>0 : goto *failidx
	gosub *mkidx
	idxerror++
*loadidx
	fname2=ih_file@				; idxファイル名
	exist fname2
	if strsize<1 : goto *failidx
	;
	sdim tmp,$8000				; text一時バッファ(32K)
	sdim wrt,strsize+4			; idxバッファを確保
	sdim ln,256
	hsfilemax=0
	hsfileok=0
	notesel wrt
	noteload fname2
	repeat notemax
		noteget ln,cnt
		if peek(ln,0)!='@' : break
		i=1
		getstr fname1,ln,i,','
		i+=strsize
		getstr tmp,ln,i,','
		flsize=0+tmp
		exist fname1
		if strsize!=flsize : hsfileok=1
		hsfilemax++
	loop
	if hsfilemax!=flmax : hsfileok=2

	if hsfileok>0 {
		goto *initidx		; .HSファイルが更新されている
	}
	maxkw=(notemax-hsfilemax)/2
	ih_msg+="Total "+maxkw+" keywords.\n"
	_p1=ih_msg
	return 0
*failidx
	sdim wrt,64
	ih_msg+="No index file.\n"
	_p1=ih_msg
	return 1

*mkidx
	;	idxファイル再構築main
	;
	ih_msg="idxファイルの更新中...\n"

	fname2=ih_file@				; 書き出しファイル
	sdim tidx1,$10000			; idx一時バッファ(64K)
	sdim tidx2,$10000			; idx一時バッファ(64K)
	sdim hsidx,$10000			; hsidx一時バッファ(64K)
	sdim wrt,$18000				; idx書き出しバッファ(96K)

	dim flsize,flmax
	flnum=0

*lp2
	;	"*.hs"をすべて解析
	;
	notesel fl
	noteget fname1,flnum
	gosub *docstart

	flnum++
	if flnum<flmax : goto *lp2

	;	tidxをソートしてwrtを作成
	;
	sdim ln,256
	sortnote tidx1
	;
	notesel tidx1
	i=notemax
	c=0
	wrt=""
	repeat i
	  notesel tidx1
	  noteget ln,cnt
	  c=peek(ln,0)
	  if c=0 : goto *tchkov
	  wrt+=ln+"\n"
	  sortget c,cnt
	  notesel tidx2
	  noteget ln,c
	  wrt+=ln+"\n"
*tchkov
	loop

	;	idxファイルをセーブ
	;
	wrt = hsidx + wrt
	;
	notesel wrt
	notesave fname2
	ih_msg+="["+fname2+"] saved.\n"



*docstart
	;	hsファイルを解析してidxを作成
	;
	exist fname1
	if strsize<0 : dialog "fatal error" : end
	hsfsize=strsize
	sdim buf,hsfsize+4			; hs読み込みバッファ
	flsize(flnum)=hsfsize
	hsidx+="@"+fname1+","+hsfsize+"\n"

	sdim a,256
	sdim ln,256
	sdim dllname,64

	ih_msg+="file ["+fname1+"] を読み込みます.\n"
	bload fname1,buf
	bufmax=strlen(buf)

	i=0:idn=0
	c=0
*doc1
	idn=i					; 行IDを保存
	getstr ln,buf,i,0:i+=strsize
	c=peek(ln,0)
	if c!=$25 : goto *doc2			; '%' check
	;
	a=strmid(ln,1,64)
	if a="dll" : goto *doc3			; "%dll" check
	if a!="index" : goto *doc2		; "%index" check
	;
	getstr a,buf,i,0:i+=strsize		; 次の行を取得
	getstr ln,buf,i,0:i+=strsize		; その次の行を取得
	if i>=bufmax : goto *doc2		; error check
	;
	tidx1+=a+"\n"
	tidx2+=""+idn+","+fname1+","+dllname+","+hsfsize+"\n"
*doc2
	if i<bufmax : goto *doc1
	return
*doc3
	getstr dllname,buf,i,0:i+=strsize	; 次の行を取得
	if dllname="none" : dllname=""
	goto *doc2


#deffunc ihelp_open int _p1
*gethelp
	;	hsファイル読み込み
	;		ihelp_findの結果変数をもとにhelpデータ取得
	;		ihelp_open n ( n=サーチテーブルindex )
	;
	lnidx=ansln._p1
	gosub *getidx
	;
	ih_ans_key@ = ans_key
	ih_ans_name@ = ans_name
	ih_ans_title@ = ans_title
	ih_ans_dll@ = ans_dll
	;
	exist ans_name
	if strsize<0 : dialog "No file ["+ans_name+"]." : lnidx=-1 : return 1

	;	Helpのnote項目を取得
	;
	ih_opt@=""
	sdim ln,256
	sdim buf,$8000				; hs読み込みバッファ
	notesel buf
	noteload ans_name			; 先頭からロード

	bufmax=strlen(buf)
	i=0:c=0
*getnopt
	if i>bufmax : goto *gnlp1
	getstr ln,buf,i,0:i+=strsize
	if ln="%note" : goto *gnlp0
	if ln="%index" : goto *gnlp1
	goto *getnopt
*gnlp0
	if i>bufmax : goto *gnlp1
	getstr ln,buf,i,0:i+=strsize
	c=peek(ln,0):if c='%' : goto *gnlp1
	ih_opt@+=ln
	goto *gnlp0
*gnlp1
	;	Help本文を取得
	;
	sdim buf,$8000				; hs読み込みバッファ
	bload ans_name,buf,$8000,lnidx		; 指定オフセットからロード
	notesel buf
	bufmax=notemax
	;
	noteget tmp,0
	if tmp!="%index" : lnidx=-1 : return 1
	noteget tmp,1
	if tmp!=ans_key : lnidx=-1 : return 1
	noteget ans_title,2
	ih_ans_title@ = ans_title
	;
	sdim ln,256
	sdim cate,256
	sdim tmp,$8000				; text一時バッファ(32K)
	sdim refsel,$4000			; リファレンス情報2(16K)
	;
	c=0
	a=""
	ih_info@=""
	ih_group@=""
	ih_refinf@=""
	ih_prgsmp@=""
	ih_prminf@=""
	ih_fncprm@=""
	i=1
	tagmode=0

	;	hsファイル文字列取得
	;
*ghlp0
	tmp=""
*ghlp1
	noteget ln,i
*ghlp1x
	c=peek(ln,0)
	if c=0 : goto *ghlp2		; NULL check
	if c=';' : goto *ghlp2		; ';' comment check
	if c=$25 : goto *ghlp3		; '%' tag check
	if c=$5e : goto *ghlp4		; '^' html tag check
	;
	if tagmode : tmp+="    "
	tmp+=ln+"\n"
*ghlp2
	i++
	if i<bufmax : goto *ghlp1
	;
	if tmp="" : goto *helpov
	goto *ghlp3

*ghlp4
	;	html特殊tag
	;
	c=peek(ln,1)
	if c='p' : goto *ghlp6
	if c='P' : goto *ghlp6
	ln=strmid(ln,1,256)
*ghlp5
	if ih_htmopt@ : tmp+="<BR><BR>"
	tmp+="\n"
	goto *ghlp2
*ghlp6
	tagmode=tagmode^1
	if ih_htmopt@=0 : goto *ghlp5
	;
	if tagmode : tmp+="<UL><PRE>\n" : goto *ghlp2
	tmp+="</UL></PRE>\n"
	goto *ghlp2

*ghlp3
	;	カテゴリー判別
	;
	if cate="prm" : ih_prminf@=tmp
	if cate="inst" : ih_info@=tmp
	if cate="href" : gosub *getref
	if cate="group" : ih_group@=tmp
	if cate="sample" : ih_prgsmp@=tmp
	if cate="index" : goto *helpov
	;
	tagmode=0
	cate=strmid(ln,1,64)
	i++
	if i<bufmax : goto *ghlp0
	;
*helpov
	fncprm=""
	getstr ih_fncprm@,ih_prminf@,0,0		; 関数パラメータを取得	
	ih_prminf@=strmid(ih_prminf@,strsize,4096)	; 残りをパラメータ説明とする
	;
	notesel refsel : i=notemax
	ih_refsel@="Related "+i+" keys.\n"
	ih_refsel@+=refsel
	;
	return 0


*getref
	;	リファレンス情報を取得
	;		tmp -> refinf & refsel
	;
	sdim rs,128
	sdim rs2,128
	ridx=0:ridx2=0:rchr=0
	ih_refinf@=""
	refsel=""
*gref1
	getstr rs,tmp,ridx,0
	ridx+=strsize
	if strsize=0 : return
	;
	rchr=peek(rs,0)
	if rchr!=$22 : goto *gref3
	;
	ridx2=instr(rs,1,"\"")
	if ridx2<1 : goto *gref1
	poke rs,ridx2+1,0
	rs2=peek(rs,1)
	refsel+=rs2+"\n"
	rs2=peek(rs,ridx2+2)
	ih_refinf@+=rs2+"\n"
	goto *gref1
*gref3
	ih_refinf@+=rs+"\n"
	refsel+=rs+"\n"
	goto *gref1

*getidx
	;
	;	idx内容取得(lnidx=行index)
	;		ans_name : hsファイル名
	;		ans_title : 項目タイトル
	;
	if lnidx<0 : return
	;
	sdim ln2,256
	sdim ans_key,64
	sdim ans_name,64
	sdim ans_title,256
	sdim ans_dll,64
	;
	i=lnidx
	getstr ans_key,wrt,i			; indexを取得
	i+=strsize
	getstr ln2,wrt,i			; indexの次行を取得
	;
	i=0
	getstr a,ln2,i,','			; offsetを取得
	i+=strsize:lnidx=0+a
	getstr ans_name,ln2,i,','		; ans_nameを取得
	i+=strsize
	getstr ans_dll,ln2,i,','		; ans_dllを取得
	i+=strsize
	getstr ans_title,ln2,i			; ans_titleを取得
	;
	return




#deffunc ihelp_info
*getprop
	;	hsファイル読み込み
	;		lnidxまでの詳細データ取得
	;
	ih_opt=""
	exist ans_name
	if strsize<0 : dialog "No file ["+ans_name+"]." : return
	;
	sdim buf,strsize+4			; hs読み込みバッファ
	sdim a,256
	sdim ln,256
	sdim iopt,256,6				; 詳細データ格納バッファ

	;	 詳細情報table
	;
	;
	bload ans_name,buf
	bufmax=strlen(buf)
	if lnidx>bufmax : return

	i=0
*prlp1
	if i>=lnidx : goto *prfin		; 終了チェック
	;
	getstr ln,buf,i,0:i+=strsize
	c=peek(ln,0)
	if c!=$25 : goto *prlp1			; '%' check
	;
	j=-1
	a=strmid(ln,1,64)
	if a="type"   : j=0
	if a="ver"    : j=1
	if a="date"   : j=2
	if a="author" : j=3
	if a="note"   : j=4
	if a="url"    : j=5
	;
	if j<0 : goto *prlp1
	;
	getstr ln,buf,i,0:i+=strsize
	iopt.j=ln
	goto *prlp1

*prfin
	ih_opt@="[Type] "+iopt.0+"\n[Group]"+ih_group@+"[Version] "+iopt.1+"\n[Date] "+iopt.2+"\n[Author] "+iopt.3+"\n[URL]"+iopt.5+"\n[misc]\n"+iopt.4
	return



#deffunc ihelp_find str _p1
*chkidx
	;	idxサーチ(in:検索キー文字列)
	;		 (statにlnnumが返る)
	;		lnnum   : match数 ( 0 = not found )
	;		ansln.x : lnidx値
	;		ansbuf  : 結果index (note形式)
	;
	mref _stat,64
	a=_p1					; 変数a = パラメータ1
	;
	dim ansln,$1000				; サーチ結果lineバッファ(4096)
	sdim ansbuf,$8000			; サーチ結果バッファ
	;
	sdim ln,256
	sdim ln2,256
	i=0:c=0
	lnnum=0
	c=strlen(a)
*idsrc
	getstr ln,wrt,i				; lnidxを取得
	if strsize=0 : goto *chkfin
	if peek(ln,0)='@' {			; file情報行
		i+=strsize
		goto *idsrc
	}
	ln2=ln
	poke ln2,c,0				; ln2をaと同じ長さに合わせる

	if ln2=a : ansln.lnnum=i : ansbuf+=ln+"\n" : lnnum+

	i+=strsize
	getstr ln,wrt,i:i+=strsize		; 1行を読み飛ばす

	goto *idsrc

*chkfin
	_stat = lnnum
	ih_ansbuf@ = ansbuf
	return



#global

