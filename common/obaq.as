;
;	obaq.dll header
;
#ifndef __obaq__
#define __obaq__

#uselib "obaq.dll"
#func qreset qreset $202
#func qterm qterm 0
#func qexec qexec 0
#func qdraw qdraw 2

#func qview qview $202
#func qsetreq qsetreq $202
#func qgetreq qgetreq $202
#func qgetptr qgetptr $202

#func qborder qborder $202
#func qgravity qgravity $202
#func qcnvaxis qcnvaxis $202
#func qgetaxis qgetaxis $202

#func qdel qdel $202
#func qfind qfind $202
#func qnext qnext $202
#func qcollision qcollision $202
#func qgetcol qgetcol $202
#func qgetcol2 qgetcol2 $202
#func qgetcol3 qgetcol3 $202

#func qaddpoly qaddpoly $202
#func qaddmodel qaddmodel $202
#func qpos qpos $202
#func qgetpos qgetpos $202
#func qweight qweight $202
#func qgetweight qgetweight $202
#func qspeed qspeed $202
#func qgetspeed qgetspeed $202
#func qgroup qgroup $202
#func qgetgroup qgetgroup $202
#func qtype qtype $202
#func qgettype qgettype $202
#func qstat qstat $202
#func qgetstat qgetstat $202
#func qmat qmat $202
#func qgetmat qgetmat $202
#func qmat2 qmat2 $202
#func qgetmat2 qgetmat2 $202
#func qmat3 qmat3 $202
#func qgetmat3 qgetmat3 $202
#func quser quser $202
#func qgetuser qgetuser $202
#func quser2 quser2 $202
#func qgetuser2 qgetuser2 $202
#func qdamper qdamper $202
#func qgetdamper qgetdamper $202
#func qinertia qinertia $202
#func qgetinertia qgetinertia $202
#func qgetversion qgetversion $202

#func qpush qpush $202
#func qblast qblast $202
#func qinner qinner $202


;
;	type define
;
#const global type_normal 0
#const global type_inner 1
#const global type_bindX 0x40
#const global type_bindY 0x80
#const global type_bindR 0x100
#const global type_bind 0x1c0
#const global type_nest 0x20000
#const global type_autowipe 0x100000

;
;	stat define
;
#const global stat_reserve 0
#const global stat_sleep 1
#const global stat_active 2

;
;	collision check
;
#const global colsw_none 0
#const global colsw_active 1
#const global colsw_all 2
#const global colsw_nest 3

;
;	material type
;
#const global mat_none 0
#const global mat_spr 1
#const global mat_spr2 2
#const global mat_wire 3
#const global mat_wire2 4
#const global mat_delay 0x1000

;
;	system request
;
#define global REQ_NONE 0
#define global REQ_PHYSICS_RATE 1
#define global REQ_MAXOBJ 2
#define global REQ_MAXLOG 3
#define global REQ_DEFAULT_WEIGHT 4
#define global REQ_DEFAULT_MOMENT 5
#define global REQ_DEFAULT_DAMPER 6
#define global REQ_DEFAULT_FRICTION 7


#endif

