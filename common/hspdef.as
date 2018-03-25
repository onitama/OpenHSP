;
; HSP3.0 Default define and macros
;
#ifdef __hsp30__
#ifndef __hspdef__
#define __hspdef__
#addition "userdef.as"

#define global and &
#define global or |
#define global xor ^
#define global not !

#define global screen_normal (0)
#define global screen_palette (1)
#define global screen_hide (2)
#define global screen_fixedsize (4)
#define global screen_tool (8)
#define global screen_frame (16)

#define global gmode_gdi (0)
#define global gmode_mem (1)
#define global gmode_rgb0 (2)
#define global gmode_alpha (3)
#define global gmode_rgb0alpha (4)
#define global gmode_add (5)
#define global gmode_sub (6)
#define global gmode_pixela (7)

#define global ginfo_mx ginfo(0)
#define global ginfo_my ginfo(1)
#define global ginfo_act ginfo(2)
#define global ginfo_sel ginfo(3)
#define global ginfo_wx1 ginfo(4)
#define global ginfo_wy1 ginfo(5)
#define global ginfo_wx2 ginfo(6)
#define global ginfo_wy2 ginfo(7)
#define global ginfo_vx ginfo(8)
#define global ginfo_vy ginfo(9)
#define global ginfo_sizex ginfo(10)
#define global ginfo_sizey ginfo(11)
#define global ginfo_winx ginfo(12)
#define global ginfo_winy ginfo(13)
#define global ginfo_mesx ginfo(14)
#define global ginfo_mesy ginfo(15)
#define global ginfo_r ginfo(16)
#define global ginfo_g ginfo(17)
#define global ginfo_b ginfo(18)
#define global ginfo_paluse ginfo(19)
#define global ginfo_dispx ginfo(20)
#define global ginfo_dispy ginfo(21)
#define global ginfo_cx ginfo(22)
#define global ginfo_cy ginfo(23)
#define global ginfo_intid ginfo(24)
#define global ginfo_newid ginfo(25)
#define global ginfo_sx ginfo(26)
#define global ginfo_sy ginfo(27)

#define global ginfo_accelx ginfo(256)
#define global ginfo_accely ginfo(257)
#define global ginfo_accelz ginfo(258)
#define global ginfo_gyrox ginfo(259)
#define global ginfo_gyroy ginfo(260)
#define global ginfo_gyroz ginfo(261)

#define global ctype objinfo_mode(%1) objinfo(%1,0)
#define global ctype objinfo_bmscr(%1) objinfo(%1,1)
#define global ctype objinfo_hwnd(%1) objinfo(%1,2)

#define global notemax noteinfo(0)
#define global notesize noteinfo(1)

#define global dir_cur dirinfo(0)
#define global dir_exe dirinfo(1)
#define global dir_win dirinfo(2)
#define global dir_sys dirinfo(3)
#define global dir_cmdline dirinfo(4)
#define global dir_desktop dirinfo($10000)
#define global dir_mydoc dirinfo($10005)
#define global dir_tv dirinfo(5)

#define global font_normal (0)
#define global font_bold (1)
#define global font_italic (2)
#define global font_underline (4)
#define global font_strikeout (8)
#define global font_antialias (16)

#define global objmode_normal (0)
#define global objmode_guifont (1)
#define global objmode_usefont (2)

#define global gsquare_grad (-257)

#define global msgothic "ＭＳ ゴシック"
#define global msmincho "ＭＳ 明朝"

#define global do  %tbreak %i0 %tcontinue %i0 %tdo *%i
#define global until(%1=1) %tcontinue *%o : %tdo if (%1)=0 { goto *%o } %tbreak *%o
#define global while(%1=1) %tcontinue %i0 %twhile *%i :%tbreak if (%1)=0 { goto *%i }
#define global wend %tcontinue *%o : %twhile goto *%o: %tbreak *%o
#define global for(%1,%2=0,%3=0,%4=1) %tcontinue %i0 %tfor %1=%2:*%i %s4 %s1 :%tbreak exgoto %1,%4,%3,*%i
#define global next %tcontinue *%o :%tfor %o +=%o :goto *%o :%tbreak *%o
#define global _break %tbreak goto *%p
#define global _continue %tcontinue goto *%p

#define global switch(%1) %tswitch %i0 %s1 _switch_val=%p : if 0 {
#define global case(%1) %tswitch _switch_sw++} if _switch_val == (%1) | _switch_sw { _switch_sw = 0

#define global default %tswitch } if 1 {
#define global swbreak %tswitch goto *%p1
#define global swend %tswitch %o0 } *%o

#define global ddim(%1,%2,%3=0,%4=0,%5=0) dimtype %1,3,%2,%3,%4,%5
#define global ldim(%1,%2,%3=0,%4=0,%5=0) dimtype %1,1,%2,%3,%4,%5
#define global alloc sdim

#define global M_PI	3.14159265358979323846
#define global ctype rad2deg(%1)       (57.295779513082320877*(%1))
#define global ctype deg2rad(%1)       (0.017453292519943295769*(%1))

#define global ease_linear (0)
#define global ease_quad_in (1)
#define global ease_quad_out (2)
#define global ease_quad_inout (3)
#define global ease_cubic_in (4)
#define global ease_cubic_out (5)
#define global ease_cubic_inout (6)
#define global ease_quartic_in (7)
#define global ease_quartic_out (8)
#define global ease_quartic_inout (9)
#define global ease_bounce_in (10)
#define global ease_bounce_out (11)
#define global ease_bounce_inout (12)
#define global ease_shake_in (13)
#define global ease_shake_out (14)
#define global ease_shake_inout (15)
#define global ease_loop (4096)

#define global notefind_match (0)
#define global notefind_first (1)
#define global notefind_instr (2)


#ifndef _DEBUG
#undef assert
#define global assert(%1) :
#undef logmes
#define global logmes(%1) :
#endif

#endif

#else
#addition "hsp261cmp.as"

#endif
