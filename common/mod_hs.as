
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

*ihelp_init
	;
	;	hsファイルライブラリ初期化
	;	(参照用グローバル変数)
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


#module "ihelp"
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
	if strsize<0 : dialog "ファイル["+ans_name+"]がありません." : lnidx=-1 : return

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
	sdim ln,256
	sdim cate,256
	sdim tmp,$8000				; text一時バッファ(32K)
	sdim refsel,$4000			; リファレンス情報2(16K)
	;
	noteget tmp,0
	if tmp!="%index" : dialog "Helpデータが異常です\nindexを再構築してください" : lnidx=-1 : return
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
	ih_refsel@="関連"+i+"項目\n"
	ih_refsel@+=refsel
	;
	return


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
	if strsize<0 : dialog "ファイル["+ans_name+"]がありません." : return
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
	ih_opt@="[種別] "+iopt.0+"\n[グループ]"+ih_group@+"[Version] "+iopt.1+"\n[作成日] "+iopt.2+"\n[作成者] "+iopt.3+"\n[URL]"+iopt.5+"\n[備考]\n"+iopt.4
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
	fname2="hsphelp.idx"			; idxファイル名
	exist fname2
	if strsize<1 : dialog "idxファイルがありません.\n「index作成」でインデックスファイルを作成してください。" : return
	;
	sdim wrt,strsize+4			; idxバッファを確保
	bload fname2,wrt
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

