;
;	hsptvapp_dish header
;
#ifndef __hsptvapp_dish__
#define __hsptvapp_dish__

#module hsptvapp
#define global HSPTV_RANK_MAX 50
#define EMU_FILE "__hsptvrank.txt"

#deffunc hsptv_start str _p1, int _p2

	hsptv_code=_p1
	netmode=2
	gameid=_p2
	sdim buf,$4000
	dim high,HSPTV_RANK_MAX
	username="MY"
	devinfo devname,"name"
	return


#deffunc hsptv_user str _p1

	username=_p1
	return


#deffunc hsptv_getmode var _v1

	_v1 = netmode
	return


#deffunc hsptv_netproc

	if netmode=0 : return

	notesel buf
	repeat HSPTV_RANK_MAX
		noteget s1,cnt*3
		lowscore=0+s1
		high(cnt)=lowscore
	loop
	return


#deffunc hsptv_up_sub int _p1, str _p2, int _p3

	;	HSPTVデータを更新します
	;	hsptv_up score,"comment",option
	;	score,comment情報を反映させて最新データを取得します。
	;	(scoreがマイナス値の場合は最新データのみ取得します)
	;
	if netmode=0 : return

	score=_p1
	comment=_p2
	option=_p3

	if netmode=0 : return

	;	emu mode
	;
	if score>=0 : goto *emu_update
	exist EMU_FILE
	if strsize>=0 {
		notesel buf
		noteload EMU_FILE
	}
	goto *comp2

*emu_update

	i=0
	notesel buf
	repeat HSPTV_RANK_MAX
		if score>high(cnt) : i=cnt : break
	loop
	if option & 0x1000 : i=score : score=0
	s1=username
	if option & 0x2000 : s1=""

	i=i*3
	noteadd ""+score+"\n"+s1+"\n"+comment,i
	notesave EMU_FILE

	score=-1

	if score>=0 : goto *send_ok

	notesel buf
	repeat HSPTV_RANK_MAX
		noteget s1,cnt*3
		lowscore=0+s1
		high(cnt)=lowscore
	loop
	return

*comp2
*send_ok
	return


#deffunc hsptv_getrank var _p1, var _p2, var _p3, int _p4

	;	HSPTVデータを取得します
	;	hsptv_getrank var1,var2,var3,rank
	;	(変数var1にrankで指定した順位のスコア情報を代入します)
	;	(変数var2にrankで指定した順位のユーザー名を代入します)
	;	(変数var3にrankで指定した順位のコメント情報を代入します)
	;	(rankは0が1位、29が30位となる)
	;	(var1は数値型、var2,var3は文字列型となります)
	;	(最新データの更新はhsptv_upで行なってください)
	;
	notesel buf
	i=_p4*3
	noteget _p2, i
	_p1=0+_p2
	noteget _p2, i+1
	noteget _p3, i+2
	noteunsel
	return

#global

#define hsptv_up(%1=-1,%2="",%3=0) hsptv_up_sub %1,%2,%3


#endif

