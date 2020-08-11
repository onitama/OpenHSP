;
; HSP3DISH Default define and macros
;
#ifndef __hsp3dish__
#define __hsp3dish__
#define global _HSP3DISH

#runtime "hsp3dish"

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

;	system request
;
#enum global SYSREQ_MAXMODEL=1
#enum global SYSREQ_MAXOBJ
#enum global SYSREQ_MAXTEX
#enum global SYSREQ_MAXMOC
#enum global SYSREQ_DXMODE
#enum global SYSREQ_DXHWND
#enum global SYSREQ_DXWIDTH
#enum global SYSREQ_DXHEIGHT
#enum global SYSREQ_COLORKEY
#enum global SYSREQ_RESULT
#enum global SYSREQ_RESVMODE
#enum global SYSREQ_PKTSIZE
#enum global SYSREQ_MAXEVENT
#enum global SYSREQ_PTRD3D
#enum global SYSREQ_PTRD3DDEV
#enum global SYSREQ_MDLANIM
#enum global SYSREQ_CALCNORMAL
#enum global SYSREQ_2DFILTER
#enum global SYSREQ_3DFILTER
#enum global SYSREQ_OLDCAM
#enum global SYSREQ_QUATALG
#enum global SYSREQ_DXVSYNC
#enum global SYSREQ_DEFTIMER
#enum global SYSREQ_NOMIPMAP
#enum global SYSREQ_DEVLOST
#enum global SYSREQ_MAXEMITTER
#enum global SYSREQ_THROUGHFLAG
#enum global SYSREQ_OBAQMATBUF
#enum global SYSREQ_2DFILTER2
#enum global SYSREQ_FPUPRESERVE
#enum global SYSREQ_CLSMODE
#enum global SYSREQ_CLSCOLOR
#enum global SYSREQ_CLSTEX
#enum global SYSREQ_TIMER
#enum global SYSREQ_PLATFORM
#enum global SYSREQ_FPS
#enum global SYSREQ_VSYNC
#enum global SYSREQ_MAXMATERIAL
#enum global SYSREQ_PHYSICS_RATE
#enum global SYSREQ_MAXOBAQ
#enum global SYSREQ_MAXLOG
#enum global SYSREQ_DEFAULT_WEIGHT
#enum global SYSREQ_DEFAULT_MOMENT
#enum global SYSREQ_DEFAULT_DAMPER
#enum global SYSREQ_DEFAULT_FRICTION
#enum global SYSREQ_MESCACHE_MAX

#const global PLATFORM_WINDOWS (0)
#const global PLATFORM_IOS (1)
#const global PLATFORM_ANDROID (2)
#const global PLATFORM_WEBGL (3)
#const global PLATFORM_LINUX (4)
#const global PLATFORM_RASPBIAN (5)

#define global SYSREQ_DEBUG $10000

#define global ginfo_accx ginfo(0x100)
#define global ginfo_accy ginfo(0x101)
#define global ginfo_accz ginfo(0x102)

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

;
;	es sprite header for HSP3Dish
;
#ifndef __essprite__
#define __essprite__

#regcmd 9
#cmd es_ini $200
#cmd es_window $201
#cmd es_area $202
#cmd es_size $203
#cmd es_pat $204
#cmd es_link $205
#cmd es_clear $206
#cmd es_new $207
#cmd es_get $208
#cmd es_setp $209
#cmd es_find $20a
#cmd es_check $20b
#cmd es_offset $20c
#cmd es_set $20d
#cmd es_flag $20e
#cmd es_chr $20f
#cmd es_type $210
#cmd es_kill $211
#cmd es_pos $212
#cmd es_setrot $213
#cmd es_apos $214
#cmd es_setgosub $215
#cmd es_adir $216
#cmd es_aim $217
#cmd es_draw $218
#cmd es_gravity $219
#cmd es_bound $21a
#cmd es_fade $21b
#cmd es_effect $21c
#cmd es_move $21d
#cmd es_setpri $21e
#cmd es_put $21f
#cmd es_ang $220
#cmd es_sin $221
#cmd es_cos $222
#cmd es_dist $223
#cmd es_opt $224
#cmd es_exnew $225
#cmd es_patanim $226
#cmd es_getpos $227
#cmd es_bgmap $228
#cmd es_putbg $229
#cmd es_bgmes $22a
#cmd es_setparent $22b
#cmd es_modaxis $22c

#define global es_fmes mes
#define global es_bye

#define global ESI_FLAG 0
#define global ESI_POSX 1
#define global ESI_POSY_2
#define global ESI_SPDX 3
#define global ESI_SPDY 4
#define global ESI_PRGCOUNT 5
#define global ESI_ANIMECOUNT 6
#define global ESI_CHRNO 7
#define global ESI_TYPE 8
#define global ESI_ACCELX 9
#define global ESI_ACCELY 10
#define global ESI_BOUNCEPOW 11
#define global ESI_BOUNCEFLAG 12
#define global ESI_OPTION 13
#define global ESI_PRIORITY 14
#define global ESI_ALPHA 15
#define global ESI_FADEPRM 16
#define global ESI_ZOOMX 17
#define global ESI_ZOOMY 18
#define global ESI_ROTZ 19

#define global ESSPFLAG_NONE (0)
#define global ESSPFLAG_STATIC (0x100)
#define global ESSPFLAG_MOVE (0x200)
#define global ESSPFLAG_GRAVITY (0x400)
#define global ESSPFLAG_BGLINK (0x800)
#define global ESSPFLAG_NOWIPE (0x1000)
#define global ESSPFLAG_XBOUNCE (0x2000)
#define global ESSPFLAG_YBOUNCE (0x4000)
#define global ESSPFLAG_BLINK (0x8000)
#define global ESSPFLAG_NODISP (0x10000)

#define global ESSPPAT_1SHOT (0x1000)

#define global ESSPSET_POS (0)
#define global ESSPSET_ADDPOS (1)
#define global ESSPSET_FALL (2)
#define global ESSPSET_BOUNCE (3)
#define global ESSPSET_ZOOM (4)
#define global ESSPSET_DIRECT (0x1000)
#define global ESSPSET_MASKBIT (0x2000)

#define global ESSPOPT_NONE (0)
#define global ESSPOPT_EXTDISP (1)
#define global ESSPOPT_FADEIN (4)
#define global ESSPOPT_FADEOUT (8)
#define global ESSPOPT_ADDCOLOR (16)
#define global ESSPOPT_SUBCOLOR (32)

#define global ESDRAW_NORMAL (0)
#define global ESDRAW_NOMOVE (1)
#define global ESDRAW_NOANIM (2)
#define global ESDRAW_NOCALLBACK (4)
#define global ESDRAW_NODISP (8)
#define global ESDRAW_NOSORT (16)

#endif

