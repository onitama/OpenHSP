;----------------------------------------------------------------
; 麻雀牌管理モジュール / onitama 2022
; (簡易的な麻雀の処理をまとめたものです、完全なルールと点数計算には対応していません)
;  使用に関する制限はありません。ご自由にお使いください。
;----------------------------------------------------------------

#ifndef __MOD_MAHJONG
#define __MOD_MAHJONG

#module "mod_mahjong"

#pack "janpai.png"

#define global MJID_WAN 1
#define global MJID_PIN 11
#define global MJID_SOU 21
#define global MJID_KAZE 31
#define global MJID_SANGEN 35
#define global MJID_URA 38
#define global MJID_FUSE 39
#define global MJID_MAX 40

#define global MJP_SX 36
#define global MJP_SY 54

#const global MJ_PMAX (34*4)	; 牌のID最大値
#define global MJ_HAND 14		; 手配の数


#deffunc mj_load int _p1

	;	mj_load id
	;
	;	牌の画像読み込み(idに読み込み画面バッファID)
	;
	mj_pcg=_p1
	if mj_pcg<=0 : mj_pcg=3
	celload "janpai.png",mj_pcg
	celdiv mj_pcg,MJP_SX,MJP_SY
	return

#deffunc mj_reset

	;	mj_reset
	;
	;	牌のリセット(シャッフル)
	;
	dim pai,MJ_PMAX:pnum=0
	repeat 4
	  repeat 9
	    pai.pnum=cnt+MJID_WAN:pnum++
	    pai.pnum=cnt+MJID_PIN:pnum++
	    pai.pnum=cnt+MJID_SOU:pnum++
	  loop
	  repeat 7
	    pai.pnum=cnt+MJID_KAZE:pnum++
	  loop
	loop
	repeat MJ_PMAX
	a=rnd(MJ_PMAX):b=pai(cnt)
	pai(cnt)=pai(a):pai(a)=b
	loop
	pnum=0
	return

#deffunc mj_put int _p1

	;	mj_put id
	;
	;	牌の表示(id=牌のID)
	;
	if mj_pcg<=0 : return
	if _p1<=0 : return
	celput mj_pcg,_p1
	return

#deffunc mj_draw var _p1

	;	mj_draw var
	;
	;	牌を1枚引く(varの変数に牌のIDが入る)
	;
	if pnum>=MJ_PMAX : _p1=0 : return
	_p1=pai(pnum):pnum++
	return

#deffunc mj_handcopy array _p1,array _p2

	;	mj_handcopy var, var2
	;
	;	手牌のコピー(var=var2)
	;
	memcpy _p1,_p2,4*MJ_HAND
	return


#deffunc mj_handsort array _p1

	;	mj_handsort var
	;
	;	手牌のソート(varの変数に手牌が入る)
	;
	dim hand2,MJ_HAND
	dim hand3,MJ_HAND
	dim table,MJID_MAX
	a=0
	memcpy hand2,_p1,4*MJ_HAND
	repeat MJID_URA:n=cnt
	  repeat MJ_HAND
	  if hand2(cnt)=n {
	    hand3(a)=n
	    hand2(cnt)=-1
	    table(n)+
	    a+
	  }
	  loop
	  if a>=MJ_HAND : break
	loop
	memcpy _p1,hand3,4*MJ_HAND
	return

#deffunc mj_handchk var _p1

	;	mj_handchk var
	;
	;	手牌の上がりチェック(varに結果、mj_handsortをすること)
	;	(結果:0=なし/1=和了)
	;
	win=0
	_p1=0
	gosub *agari1
	if win : _p1=win : return
	gosub *agari2
	if win : _p1=win : return
	return

*agari1
	;	基本形チェック
	atama=0
*ag1
	dim ctable,38
	memcpy ctable,table,38*4
*ag2
	if atama>=38 : return
	if ctable(atama)<2 : goto *ag3
	ctable(atama)-=2
	check=0:koutu=0:syuntu=0
*ag4
	if ctable(check)>=3 {
	  ctable(check)-=3:koutu++
	}
	a=check
	if ctable(a)=0 : goto *ag5
	if a>=MJID_KAZE : goto *ag6
	a+:if ctable(a)=0 : goto *ag6
	a+:if ctable(a)=0 : goto *ag6
	a=check:ctable(a)-
	a+:ctable(a)-
	a+:ctable(a)-
	syuntu++:goto *ag4
*ag5
	check+:if check<38 : goto *ag4
	win=1:return
*ag6
	atama+:goto *ag1
*ag3
	atama+:goto *ag2


*agari2
	;	和了チェック(七対子)
	toitu=0
	repeat 38
	if table(cnt)>=2 : toitu++
	loop
	if toitu>=7 : win=2
	return


#deffunc mj_getwin var _p1, var _p2, var _p3

	;	mj_getwin var,var2,var3
	;
	;	上がり結果を取得(mj_handchkで和了していること)
	;	var  = 役
	;	var2 = 順子
	;	var3 = 刻子
	;
	sdim yakulist,$4000
	if win=1 : yakulist+="和了\n"
	if win=2 : yakulist+="七対子\n"

	_p1=yakulist
	_p2=syuntu
	_p3=koutu

	return


#global
#endif

