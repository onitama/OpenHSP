;
; HSP3DISH Default define and macros
;
#ifndef __hsp3dish__
#define __hsp3dish__
#define global _HSP3DISH

#runtime "hsp3dish"
#addition "dish_enhance.as"
#addition "dish_sysreq.as"

#const global FILTER_NONE 0
#const global FILTER_LINEAR 1
#const global FILTER_LINEAR2 2

#const global CLSMODE_NONE 0
#const global CLSMODE_SOLID 1
#const global CLSMODE_TEXTURE 2

#define global screen_offscreen (32)
#define global screen_usergcopy (64)

#define global celbitmap_bgr (0)
#define global celbitmap_rgb (1)
#define global celbitmap_capture (16)

#define global vptype_off (0)
#define global vptype_translate (1)
#define global vptype_rotate (2)
#define global vptype_scale (3)
#define global vptype_3dmatrix (4)
#define global vptype_2d (5)
#define global vptype_3d (6)

;
;	http value
;
#enum global HTTPMODE_NONE=0
#enum global HTTPMODE_READY
#enum global HTTPMODE_REQUEST
#enum global HTTPMODE_SEND
#enum global HTTPMODE_DATAWAIT
#enum global HTTPMODE_DATAEND
#define global HTTPMODE_ERROR (-1)

#define global HTTPINFO_MODE 0		; 現在のモード
#define global HTTPINFO_SIZE 1		; データサイズ
#define global HTTPINFO_DATA 16		; 取得データ
#define global HTTPINFO_ERROR 17	; エラー文字列

#endif

;
;	obaq header for HSP3Dish
;
#ifndef __obaq__
#define __obaq__

#regcmd 18
#cmd qreset $00							// qreset
#cmd qterm $01							// qterm
#cmd qexec $02							// qexec
#cmd qdraw $03							// qdraw
#cmd qview $04							// qview
#cmd qsetreq $05						// qsetreq
#cmd qgetreq $06						// qgetreq
#cmd qborder $07						// qborder
#cmd qgravity $08						// qgravity
#cmd qcnvaxis $09						// qcnvaxis
#cmd qgetaxis $0a						// qgetaxis
#cmd qaddpoly $0b						// qaddpoly
#cmd qdel $0c							// qdel
#cmd qpos $0d							// qpos
#cmd qgetpos $0e						// qgetpos
#cmd qweight $0f						// qweight
#cmd qgetweight $10						// qgetweight
#cmd qspeed $11							// qspeed
#cmd qgetspeed $12						// qgetspeed
#cmd qgroup $13							// qgroup
#cmd qgetgroup $14						// qgetgroup
#cmd qtype $15							// qtype
#cmd qgettype $16						// qgettype
#cmd qstat $17							// qstat
#cmd qgetstat $18						// qgetstat
#cmd qcollision $19						// qcollision
#cmd qgetcol $1a						// qgetcol
#cmd qgetcol2 $1b						// qgetcol2
#cmd qgetcol3 $1c						// qgetcol3
#cmd qmat $1d							// qmat
#cmd qmat2 $1e							// qmat2
#cmd qmat3 $1f							// qmat3
#cmd qgetmat $20						// qgetmat
#cmd qgetmat2 $21						// qgetmat2
#cmd qgetmat3 $22						// qgetmat3
#cmd quser $23							// quser
#cmd quser2 $24							// quser2
#cmd qgetuser $25						// qgetuser
#cmd qgetuser2 $26						// qgetuser2
#cmd qdamper $27						// qdamper
#cmd qgetdamper $28						// qgetdamper
#cmd qpush $29							// qpush
#cmd qblast $2a							// qblast
#cmd qaddmodel $2b						// qaddmodel
#cmd qfind $2c							// qfind
#cmd qnext $2d							// qnext
#cmd qinertia $2e						// qinertia
#cmd qgetinertia $2f						// qgetinertia
#cmd qgetversion $30						// qgetversion
#cmd qinner $31							// qinner


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
#define global REQ_PHYSICS_RATE (SYSREQ_PHYSICS_RATE)
#define global REQ_MAXOBJ (SYSREQ_MAXOBAQ)
#define global REQ_MAXLOG (SYSREQ_MAXLOG)
#define global REQ_DEFAULT_WEIGHT (SYSREQ_DEFAULT_WEIGHT)
#define global REQ_DEFAULT_MOMENT (SYSREQ_DEFAULT_MOMENT)
#define global REQ_DEFAULT_DAMPER (SYSREQ_DEFAULT_DAMPER)
#define global REQ_DEFAULT_FRICTION (SYSREQ_DEFAULT_FRICTION)

#endif

#addition "dish_sprite.as"

