;
; HSP3Dish sprite function define
;
#ifndef __essprite__
#define __essprite__

#regcmd 9
#cmd es_ini $200
#cmd es_screen $201
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
#cmd es_arot $22d
#cmd es_bgparam $22e
#cmd es_bgattr $22f
#cmd es_bghitpos $230
#cmd es_getbghit $231
#cmd es_getbgattr $232
#cmd es_bglink $233
#cmd es_stick $234
#cmd es_bghit $235
#cmd es_regdeco $236
#cmd es_setdeco $237
#cmd es_bgfetch $238
#cmd es_sizeex $239
#cmd es_spropt $23a
#cmd es_nearobj $23b
#cmd es_setlife $23c


#define global es_fmes mes
#define global es_bye

#define global ESI_FLAG 0
#define global ESI_POSX 1
#define global ESI_POSY 2
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
#define global ESI_SPLINK 20
#define global ESI_TIMER 21
#define global ESI_TIMERBASE 22
#define global ESI_PROTZ 23
#define global ESI_PZOOMX 24
#define global ESI_PZOOMY 25
#define global ESI_MAPHIT 26
#define global ESI_STICKSP 27
#define global ESI_STICKX 28
#define global ESI_STICKY 29
#define global ESI_MOVERES 30
#define global ESI_XREVCHR 31
#define global ESI_YREVCHR 32
#define global ESI_MULCOLOR 33
#define global ESI_LIFE 34
#define global ESI_LIFEMAX 35
#define global ESI_POWER 36

#define global ESSPFLAG_NONE (0)
#define global ESSPFLAG_STATIC (0x100)
#define global ESSPFLAG_MOVE (0x200)
#define global ESSPFLAG_GRAVITY (0x400)
#define global ESSPFLAG_SPLINK (0x800)
#define global ESSPFLAG_NOWIPE (0x1000)
#define global ESSPFLAG_XBOUNCE (0x2000)
#define global ESSPFLAG_YBOUNCE (0x4000)
#define global ESSPFLAG_BLINK (0x8000)
#define global ESSPFLAG_NODISP (0x10000)
#define global ESSPFLAG_FADEIN (0x20000)
#define global ESSPFLAG_FADEOUT (0x40000)
#define global ESSPFLAG_TIMERWIPE (0x80000)
#define global ESSPFLAG_BLINK2 (0x100000)
#define global ESSPFLAG_EFADE (0x200000)
#define global ESSPFLAG_EFADE2 (0x400000)
#define global ESSPFLAG_MOVEROT (0x800000)
#define global ESSPFLAG_DECORATE (0x1000000)

#define global ESSPPAT_1SHOT (0x1000)

#define global ESSPSET_POS (0)
#define global ESSPSET_ADDPOS (1)
#define global ESSPSET_FALL (2)
#define global ESSPSET_BOUNCE (3)
#define global ESSPSET_ZOOM (4)
#define global ESSPSET_ADDZOOM (5)
#define global ESSPSET_CENTER (6)
#define global ESSPSET_PUTPOS (7)
#define global ESSPSET_ADDPOS2 (8)
#define global ESSPSET_DIRECT (0x1000)
#define global ESSPSET_MASKBIT (0x2000)

#define global ESSPRES_XBLOCK (0x100)
#define global ESSPRES_YBLOCK (0x200)
#define global ESSPRES_GROUND (0x400)
#define global ESSPRES_EVENT (0x800)

#define global ESDRAW_NORMAL (0)
#define global ESDRAW_NOMOVE (1)
#define global ESDRAW_NOANIM (2)
#define global ESDRAW_NOCALLBACK (4)
#define global ESDRAW_NODISP (8)
#define global ESDRAW_NOSORT (16)
#define global ESDRAW_DEBUG (32)

#define global ESSPF_TIMEWIPE (1)
#define global ESSPF_BLINK (2)
#define global ESSPF_BLINKWIPE (3)
#define global ESSPF_BLINK2 (4)
#define global ESSPF_BLINKWIPE2 (5)
#define global ESSPF_FADEOUT (6)
#define global ESSPF_FADEOUTWIPE (7)
#define global ESSPF_FADEIN (8)
#define global ESSPF_FADEINWIPE (9)
#define global ESSPF_EFADE (10)
#define global ESSPF_EFADEWIPE (11)
#define global ESSPF_EFADE2 (12)
#define global ESSPF_EFADEWIPE2 (13)

#define global ESBGOPT_NOTRANS (1)
#define global ESBGOPT_USEMASK (0x100)

#define global ESMAP_ATTR_MAX (0x1000)
#define global ESMAP_ATTR_NONE (0)		//	侵入可能な場所(デフォルト)
#define global ESMAP_ATTR_EVENT (64)	//	侵入可能でヒット判定(アイテム)
#define global ESMAP_ATTR_HOLD (128)	//	侵入可能だが足場になる
#define global ESMAP_ATTR_WALL (192)	//	侵入不可の壁
#define global ESMAP_ATTR_ANIM (32)		//	アニメーション有効
#define global ESMAP_ATTR_NOTICE (16)	//	通知アイテム
#define global ESMAP_ATTR_GROUP (15)	//	グループビット

#define global ESMAP_OPT_WIPENOTICE (1)
#define global ESMAP_OPT_GETEVENT (2)
#define global ESMAP_OPT_WIPEEVENT (4)

#define global ESSPMAPHIT_BGHIT (0x100)
#define global ESSPMAPHIT_BLOCKBIT (0x400)
#define global ESSPMAPHIT_HITWIPE (0x800)
#define global ESSPMAPHIT_WIPEEVENT (0x1000)
#define global ESSPMAPHIT_BGOBJ (0x10000)
#define global ESSPMAPHIT_STICKSP (0x20000)

#define global ESMAP_PRM_GMODE (0)
#define global ESMAP_PRM_ANIM (1)
#define global ESMAP_PRM_GROUP (2)
#define global ESMAP_PRM_NOTICE (3)
#define global ESMAP_PRM_WIPEDECO (4)
#define global ESMAP_PRM_BLOCKBIT (5)
#define global ESMAP_PRM_GRAVITY (6)
#define global ESMAP_PRM_OPTION (16)

#define global ESMAPHIT_NONE (0)
#define global ESMAPHIT_HITX (1)
#define global ESMAPHIT_HITY (2)
#define global ESMAPHIT_HIT (3)
#define global ESMAPHIT_EVENT (4)
#define global ESMAPHIT_NOTICE (5)
#define global ESMAPHIT_SPHIT (6)

#define global ESDECO_FRONT (1)
#define global ESDECO_MAPHIT (2)
#define global ESDECO_GRAVITY (4)
#define global ESDECO_ZOOM (8)
#define global ESDECO_ROTATE (16)
#define global ESDECO_BOOST (32)
#define global ESDECO_SCATTER (64)
#define global ESDECO_MULTI4 (0x100)
#define global ESDECO_MULTI8 (0x200)
#define global ESDECO_MULTI16 (0x400)
#define global ESDECO_CHR2 (0x1000)
#define global ESDECO_CHR4 (0x2000)
#define global ESDECO_EPADD (0x4000)
#define global ESDECO_FADEOUT (0x8000)


#endif


