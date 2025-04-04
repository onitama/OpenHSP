#ifndef __MOD_RES
#define __MOD_RES
;
;
;============================================================
; リソース管理モジュール
;  アプリが使用する画像リソースを1元管理するマネージャーです
;  (HSP3汎用ライブラリ)

	sdim _hsp3mod_resfile,64,16
	_hsp3mod_resstart = 10							; 管理リソーススタートID
	_hsp3mod_resmax = 0

#module HSP3MOD_RES

#deffunc hsp3res_reset int _p1

	;	リソース管理モジュールの初期化
	;
	repeat _hsp3mod_resmax@
		hsp3res_del cnt+_hsp3mod_resstart@
	loop
	return


#defcfunc hsp3res_id str _p1, local i

	;	リソースが読み込まれたバッファIDを取得する関数
	;
	i=-1
	repeat _hsp3mod_resmax@
	if _hsp3mod_resfile@(cnt)=_p1 {
		i=cnt+_hsp3mod_resstart@
		break
	}
	loop
	return i


#deffunc hsp3res_load str _p1, int _p2

	;	リソース読み込み
	;
	fname=_p1
	id=_p2
	if id>0 : goto *reschk

	id = hsp3res_id(fname)
	if id>0 : return id								; 読み込み済みリソース

	;	新規リソーススロット探索
	id = hsp3res_id("")
	if id<0 {
		id = _hsp3mod_resmax@ + _hsp3mod_resstart@
		_hsp3mod_resmax@++
	}
	goto *resadd

*reschk
	id = hsp3res_id(fname)
	if id>0 {
		if id=_p2 {
			return id								; 読み込み済みリソース
		}
		dialog "Resource ID Error ["+fname+"]=ID"+id
		return -1
	}
	id = _p2
	goto *resadd

*resadd
	slot = id -_hsp3mod_resstart@
	if (slot<0)|(slot>=_hsp3mod_resmax@) {
		dialog "Resource ID Error ["+id+"]"
		return -1
	}
	exist fname
	if strsize<0 {
		dialog "No File Error ["+fname+"]"
		return -1
	}
	_hsp3mod_resfile@(slot)=fname
	celload fname,id
	return id


#deffunc hsp3res_del int _p1

	;	リソース破棄
	;
	id = _p1
	slot = _p1-_hsp3mod_resstart@
	if slot<0 : return
	if slot>=_hsp3mod_resmax@ : return
	if _hsp3mod_resfile@(slot)="" : return

	_hsp3mod_resfile@(slot)=""
	i=ginfo_sel
	buffer id,32,32
	gsel i
	return

#global
#endif

