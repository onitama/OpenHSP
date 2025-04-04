//
//	hsp3cnv(3.31b2) generated source
//	[block3.ax]
//
#include "hsp3r.h"

#define _HSP3CNV_DATE "2012/07/08"
#define _HSP3CNV_TIME "15:45:40"
#define _HSP3CNV_MAXVAR 38
#define _HSP3CNV_MAXHPI 0
#define _HSP3CNV_VERSION 0x301
#define _HSP3CNV_BOOTOPT 4096

/*-----------------------------------------------------------*/

static PVal *Var_x1;
static PVal *Var_y1;
static PVal *Var_x2;
static PVal *Var_y2;
static PVal *Var_clrflag;
static PVal *Var_blsize;
static PVal *Var_blspd;
static PVal *Var_blx;
static PVal *Var_bly;
static PVal *Var_bpx;
static PVal *Var_bpy;
static PVal *Var_bk;
static PVal *Var_mblsize;
static PVal *Var_barsize;
static PVal *Var_barx;
static PVal *Var_bary;
static PVal *Var_barhalf;
static PVal *Var_blhalf;
static PVal *Var_wx;
static PVal *Var_wy;
static PVal *Var_wsx;
static PVal *Var_wsy;
static PVal *Var_wpx;
static PVal *Var_wpy;
static PVal *Var_wall;
static PVal *Var_score;
static PVal *Var_gameflag;
static PVal *Var_key;
static PVal *Var_coly;
static PVal *Var_colx;
static PVal *Var_x;
static PVal *Var_i;
static PVal *Var_bsize;
static PVal *Var_left;
static PVal *Var_cy;
static PVal *Var_y;
static PVal *Var_cx;
static PVal *Var_j;

/*-----------------------------------------------------------*/

void __HspEntry( void ) {
	// Var initalize
	Var_x1 = &mem_var[0];
	Var_y1 = &mem_var[1];
	Var_x2 = &mem_var[2];
	Var_y2 = &mem_var[3];
	Var_clrflag = &mem_var[4];
	Var_blsize = &mem_var[5];
	Var_blspd = &mem_var[6];
	Var_blx = &mem_var[7];
	Var_bly = &mem_var[8];
	Var_bpx = &mem_var[9];
	Var_bpy = &mem_var[10];
	Var_bk = &mem_var[11];
	Var_mblsize = &mem_var[12];
	Var_barsize = &mem_var[13];
	Var_barx = &mem_var[14];
	Var_bary = &mem_var[15];
	Var_barhalf = &mem_var[16];
	Var_blhalf = &mem_var[17];
	Var_wx = &mem_var[18];
	Var_wy = &mem_var[19];
	Var_wsx = &mem_var[20];
	Var_wsy = &mem_var[21];
	Var_wpx = &mem_var[22];
	Var_wpy = &mem_var[23];
	Var_wall = &mem_var[24];
	Var_score = &mem_var[25];
	Var_gameflag = &mem_var[26];
	Var_key = &mem_var[27];
	Var_coly = &mem_var[28];
	Var_colx = &mem_var[29];
	Var_x = &mem_var[30];
	Var_i = &mem_var[31];
	Var_bsize = &mem_var[32];
	Var_left = &mem_var[33];
	Var_cy = &mem_var[34];
	Var_y = &mem_var[35];
	Var_cx = &mem_var[36];
	Var_j = &mem_var[37];

	// x1 =0
	PushInt(0); 
	VarSet(Var_x1,0);
	// y1 =0
	PushInt(0); 
	VarSet(Var_y1,0);
	// x2 =320
	PushInt(320); 
	VarSet(Var_x2,0);
	// y2 =480
	PushInt(480); 
	VarSet(Var_y2,0);
	// screen 0, x2, y2
	PushVAP(Var_y2,0); 
	PushVAP(Var_x2,0); 
	PushInt(0); 
	Extcmd(42,3);
	// title "ブロック崩し"
	PushStr("ブロック崩し"); 
	Extcmd(16,1);
	TaskSwitch(0);
}

static void L0000( void ) {
	// clrflag =0
	PushInt(0); 
	VarSet(Var_clrflag,0);
	// blsize =16
	PushInt(16); 
	VarSet(Var_blsize,0);
	// blspd =10
	PushInt(10); 
	VarSet(Var_blspd,0);
	// blx =200
	PushInt(200); 
	VarSet(Var_blx,0);
	// bly =200
	PushInt(200); 
	VarSet(Var_bly,0);
	// bpx =4
	PushInt(4); 
	VarSet(Var_bpx,0);
	// bpy =4
	PushInt(4); 
	VarSet(Var_bpy,0);
	// bk =0
	PushInt(0); 
	VarSet(Var_bk,0);
	// mblsize =blsize*-1
	PushVar(Var_blsize,0); PushInt(-1); CalcMulI(); 
	VarSet(Var_mblsize,0);
	// barsize =64
	PushInt(64); 
	VarSet(Var_barsize,0);
	// barx =240
	PushInt(240); 
	VarSet(Var_barx,0);
	// bary =440
	PushInt(440); 
	VarSet(Var_bary,0);
	// barhalf =barsize/2
	PushVar(Var_barsize,0); PushInt(2); CalcDivI(); 
	VarSet(Var_barhalf,0);
	// blhalf =blsize/2
	PushVar(Var_blsize,0); PushInt(2); CalcDivI(); 
	VarSet(Var_blhalf,0);
	// wx =0
	PushInt(0); 
	VarSet(Var_wx,0);
	// wy =80
	PushInt(80); 
	VarSet(Var_wy,0);
	// wsx =10
	PushInt(10); 
	VarSet(Var_wsx,0);
	// wsy =6
	PushInt(6); 
	VarSet(Var_wsy,0);
	// wpx =x2/wsx
	PushVar(Var_x2,0); PushVar(Var_wsx,0); CalcDivI(); 
	VarSet(Var_wpx,0);
	// wpy =16
	PushInt(16); 
	VarSet(Var_wpy,0);
	// dim wall, wsx, wsy
	PushVAP(Var_wsy,0); 
	PushVAP(Var_wsx,0); 
	PushVAP(Var_wall,0); 
	Prgcmd(9,3);
	// score =0
	PushInt(0); 
	VarSet(Var_score,0);
	// gameflag =0
	PushInt(0); 
	VarSet(Var_gameflag,0);
	TaskSwitch(1);
}

static void L0001( void ) {
	// redraw 0
	PushInt(0); 
	Extcmd(27,1);
	// color 0, 0, 128
	PushInt(128); 
	PushInt(0); 
	PushInt(0); 
	Extcmd(24,3);
	// boxf 
	Extcmd(49,0);
	// color 0, 255, 255
	PushInt(255); 
	PushInt(255); 
	PushInt(0); 
	Extcmd(24,3);
	// pos 8, 8
	PushInt(8); 
	PushInt(8); 
	Extcmd(17,2);
	// mes "SCORE:"+score
	PushStr("SCORE:"); PushVar(Var_score,0); CalcAddI(); 
	Extcmd(15,1);
	// barx =barsize/2-mousex
	PushFuncEnd(); PushExtvar(0,0); PushVar(Var_barsize,0); PushInt(2); CalcDivI(); CalcSubI(); 
	VarSet(Var_barx,0);
	// color 255, 255, 255
	PushInt(255); 
	PushInt(255); 
	PushInt(255); 
	Extcmd(24,3);
	// boxf barx, bary, barx+barsize, bary+16
	PushVar(Var_bary,0); PushInt(16); CalcAddI(); 
	PushVar(Var_barx,0); PushVar(Var_barsize,0); CalcAddI(); 
	PushVAP(Var_bary,0); 
	PushVAP(Var_barx,0); 
	Extcmd(49,4);
	// if gameflag
	PushVAP(Var_gameflag,0); 
	if (HspIf()) { TaskSwitch(8); return; }
	// gosub
	PushLabel(2); 
	PushLabel(9); Prgcmd(1,2); return;
}

static void L0009( void ) {
	// stick key
	PushVAP(Var_key,0); 
	Extcmd(52,1);
	// if key&256
	PushVar(Var_key,0); PushInt(256); CalcAndI(); 
	if (HspIf()) { TaskSwitch(10); return; }
	// if mousey<300
	PushFuncEnd(); PushExtvar(1,0); PushInt(300); CalcLtI(); 
	if (HspIf()) { TaskSwitch(11); return; }
	// goto *L0000
	TaskSwitch(0);
	return;
	TaskSwitch(11);
}

static void L0011( void ) {
	TaskSwitch(10);
}

static void L0010( void ) {
	// goto *L0003
	TaskSwitch(3);
	return;
	TaskSwitch(8);
}

static void L0008( void ) {
	// blx =blx+bpx
	PushVar(Var_blx,0); PushVar(Var_bpx,0); CalcAddI(); 
	VarSet(Var_blx,0);
	// if blx<=x1
	PushVar(Var_blx,0); PushVar(Var_x1,0); CalcLtEqI(); 
	if (HspIf()) { TaskSwitch(12); return; }
	// blx =x1
	PushVar(Var_x1,0); 
	VarSet(Var_blx,0);
	// bpx =bpx*-1
	PushVar(Var_bpx,0); PushInt(-1); CalcMulI(); 
	VarSet(Var_bpx,0);
	TaskSwitch(12);
}

static void L0012( void ) {
	// if x2-blsize>=blx
	PushVar(Var_blx,0); PushVar(Var_x2,0); PushVar(Var_blsize,0); CalcSubI(); CalcGtEqI(); 
	if (HspIf()) { TaskSwitch(13); return; }
	// blx =x2-blsize
	PushVar(Var_x2,0); PushVar(Var_blsize,0); CalcSubI(); 
	VarSet(Var_blx,0);
	// bpx =bpx*-1
	PushVar(Var_bpx,0); PushInt(-1); CalcMulI(); 
	VarSet(Var_bpx,0);
	TaskSwitch(13);
}

static void L0013( void ) {
	// bly =bly+bpy
	PushVar(Var_bly,0); PushVar(Var_bpy,0); CalcAddI(); 
	VarSet(Var_bly,0);
	// if bly<=y1
	PushVar(Var_bly,0); PushVar(Var_y1,0); CalcLtEqI(); 
	if (HspIf()) { TaskSwitch(14); return; }
	// bly =y1
	PushVar(Var_y1,0); 
	VarSet(Var_bly,0);
	// bpy =blspd
	PushVar(Var_blspd,0); 
	VarSet(Var_bpy,0);
	// bk =1
	PushInt(1); 
	VarSet(Var_bk,0);
	// blspd =16
	PushInt(16); 
	VarSet(Var_blspd,0);
	TaskSwitch(14);
}

static void L0014( void ) {
	// if y2-blsize>=bly
	PushVar(Var_bly,0); PushVar(Var_y2,0); PushVar(Var_blsize,0); CalcSubI(); CalcGtEqI(); 
	if (HspIf()) { TaskSwitch(15); return; }
	// gameflag =1
	PushInt(1); 
	VarSet(Var_gameflag,0);
	TaskSwitch(15);
}

static void L0015( void ) {
	// coly =bly+blsize
	PushVar(Var_bly,0); PushVar(Var_blsize,0); CalcAddI(); 
	VarSet(Var_coly,0);
	// if (coly>=barybary+16<bly)&
	PushVar(Var_coly,0); PushVar(Var_bary,0); CalcGtEqI(); PushVar(Var_bly,0); PushVar(Var_bary,0); PushInt(16); CalcAddI(); CalcLtI(); CalcAndI(); 
	if (HspIf()) { TaskSwitch(16); return; }
	// colx =blx+blhalf
	PushVar(Var_blx,0); PushVar(Var_blhalf,0); CalcAddI(); 
	VarSet(Var_colx,0);
	// x =barx+barhalf
	PushVar(Var_barx,0); PushVar(Var_barhalf,0); CalcAddI(); 
	VarSet(Var_x,0);
	// if barhalf+blhalf<abs(colx-x)
	PushFuncEnd(); 	PushVar(Var_colx,0); PushVar(Var_x,0); CalcSubI(); 
	PushIntfunc(16,1); PushVar(Var_barhalf,0); PushVar(Var_blhalf,0); CalcAddI(); CalcLtI(); 
	if (HspIf()) { TaskSwitch(17); return; }
	// bly =bary-blsize
	PushVar(Var_bary,0); PushVar(Var_blsize,0); CalcSubI(); 
	VarSet(Var_bly,0);
	// bpy =blspd*-1
	PushVar(Var_blspd,0); PushInt(-1); CalcMulI(); 
	VarSet(Var_bpy,0);
	// i =(colx-x)/4
	PushVar(Var_colx,0); PushVar(Var_x,0); CalcSubI(); PushInt(4); CalcDivI(); 
	VarSet(Var_i,0);
	// if i!=0
	PushVar(Var_i,0); PushInt(0); CalcNeI(); 
	if (HspIf()) { TaskSwitch(18); return; }
	// bpx =i
	PushVar(Var_i,0); 
	VarSet(Var_bpx,0);
	TaskSwitch(18);
}

static void L0018( void ) {
	// bk =1
	PushInt(1); 
	VarSet(Var_bk,0);
	// if clrflag
	PushVAP(Var_clrflag,0); 
	if (HspIf()) { TaskSwitch(19); return; }
	// dim wall, wsx, wsy
	PushVAP(Var_wsy,0); 
	PushVAP(Var_wsx,0); 
	PushVAP(Var_wall,0); 
	Prgcmd(9,3);
	// clrflag =0
	PushInt(0); 
	VarSet(Var_clrflag,0);
	TaskSwitch(19);
}

static void L0019( void ) {
	TaskSwitch(17);
}

static void L0017( void ) {
	TaskSwitch(16);
}

static void L0016( void ) {
	TaskSwitch(3);
}

static void L0003( void ) {
	// colx =wpx+bsize
	PushVar(Var_wpx,0); PushVar(Var_bsize,0); CalcAddI(); 
	VarSet(Var_colx,0);
	// coly =wpy+bsize
	PushVar(Var_wpy,0); PushVar(Var_bsize,0); CalcAddI(); 
	VarSet(Var_coly,0);
	// left =0
	PushInt(0); 
	VarSet(Var_left,0);
	// repeat
	PushVAP(Var_wsy,0); 
	PushLabel(4); 
	PushLabel(20); Prgcmd(4,3); return;
	TaskSwitch(20);
}

static void L0020( void ) {
	// cy =cnt
	PushSysvar(4,0); 
	VarSet(Var_cy,0);
	// y =(cnt*wpy)+wy
	PushSysvar(4,0); PushVar(Var_wpy,0); CalcMulI(); PushVar(Var_wy,0); CalcAddI(); 
	VarSet(Var_y,0);
	// hsvcolor cnt*10, 255, 255
	PushInt(255); 
	PushInt(255); 
	PushSysvar(4,0); PushInt(10); CalcMulI(); 
	Extcmd(34,3);
	// repeat
	PushVAP(Var_wsx,0); 
	PushLabel(5); 
	PushLabel(21); Prgcmd(4,3); return;
	TaskSwitch(21);
}

static void L0021( void ) {
	// cx =cnt
	PushSysvar(4,0); 
	VarSet(Var_cx,0);
	// x =(cnt*wpx)+wx
	PushSysvar(4,0); PushVar(Var_wpx,0); CalcMulI(); PushVar(Var_wx,0); CalcAddI(); 
	VarSet(Var_x,0);
	// if wall(cx, cy)=0
		PushVAP(Var_cy,0); 
		PushVAP(Var_cx,0); 
	PushVar(Var_wall,2); PushInt(0); CalcEqI(); 
	if (HspIf()) { TaskSwitch(22); return; }
	// left ++
	VarInc(Var_left,0);
	// boxf x, y, (x+wpx)-2, (y+wpy)-2
	PushVar(Var_y,0); PushVar(Var_wpy,0); CalcAddI(); PushInt(2); CalcSubI(); 
	PushVar(Var_x,0); PushVar(Var_wpx,0); CalcAddI(); PushInt(2); CalcSubI(); 
	PushVAP(Var_y,0); 
	PushVAP(Var_x,0); 
	Extcmd(49,4);
	// i =blx-x
	PushVar(Var_blx,0); PushVar(Var_x,0); CalcSubI(); 
	VarSet(Var_i,0);
	// j =bly-y
	PushVar(Var_bly,0); PushVar(Var_y,0); CalcSubI(); 
	VarSet(Var_j,0);
	// if ((((i>=mblsizei<colx)&j>=mblsize)&j<coly)&)&bk
	PushVar(Var_i,0); PushVar(Var_mblsize,0); CalcGtEqI(); PushVar(Var_i,0); PushVar(Var_colx,0); CalcLtI(); CalcAndI(); PushVar(Var_j,0); PushVar(Var_mblsize,0); CalcGtEqI(); CalcAndI(); PushVar(Var_j,0); PushVar(Var_coly,0); CalcLtI(); CalcAndI(); PushVar(Var_bk,0); CalcAndI(); 
	if (HspIf()) { TaskSwitch(23); return; }
	// wall (cx, cy)=1
	PushInt(1); 
	PushVAP(Var_cy,0); 
	PushVAP(Var_cx,0); 
	VarSet(Var_wall,2);
	// bpy =bpy*-1
	PushVar(Var_bpy,0); PushInt(-1); CalcMulI(); 
	VarSet(Var_bpy,0);
	// bk =0
	PushInt(0); 
	VarSet(Var_bk,0);
	// score +=wsy-cy
	PushVar(Var_wsy,0); PushVar(Var_cy,0); CalcSubI(); 
	VarCalc(Var_score,0,0);
	TaskSwitch(23);
}

static void L0023( void ) {
	TaskSwitch(22);
}

static void L0022( void ) {
	// loop 
	Prgcmd(5,0);
	return;
	TaskSwitch(5);
}

static void L0005( void ) {
	// loop 
	Prgcmd(5,0);
	return;
	TaskSwitch(4);
}

static void L0004( void ) {
	// pos blx, bly
	PushVAP(Var_bly,0); 
	PushVAP(Var_blx,0); 
	Extcmd(17,2);
	// color 255, 255, 255
	PushInt(255); 
	PushInt(255); 
	PushInt(255); 
	Extcmd(24,3);
	// circle blx, bly, blx+blsize, bly+blsize
	PushVar(Var_bly,0); PushVar(Var_blsize,0); CalcAddI(); 
	PushVar(Var_blx,0); PushVar(Var_blsize,0); CalcAddI(); 
	PushVAP(Var_bly,0); 
	PushVAP(Var_blx,0); 
	Extcmd(18,4);
	// redraw 1
	PushInt(1); 
	Extcmd(27,1);
	// await 33
	PushInt(33); 
	Prgcmd(8,1);
	TaskSwitch(24);
}

static void L0024( void ) {
	// if left=0
	PushVar(Var_left,0); PushInt(0); CalcEqI(); 
	if (HspIf()) { TaskSwitch(25); return; }
	// clrflag =1
	PushInt(1); 
	VarSet(Var_clrflag,0);
	TaskSwitch(25);
}

static void L0025( void ) {
	// goto *L0001
	TaskSwitch(1);
	return;
	TaskSwitch(2);
}

static void L0002( void ) {
	// color 255, 255, 255
	PushInt(255); 
	PushInt(255); 
	PushInt(255); 
	Extcmd(24,3);
	// pos 120, 280
	PushInt(280); 
	PushInt(120); 
	Extcmd(17,2);
	// mes "GAME OVER"
	PushStr("GAME OVER"); 
	Extcmd(15,1);
	// return 
	Prgcmd(2,0);
	return;
	TaskSwitch(6);
}

static void L0006( void ) {
	// end 
	Prgcmd(16,0);
	return;
	TaskSwitch(7);
}

static void L0007( void ) {
	// stop 
	Prgcmd(17,0);
	return;
	// goto 
	Prgcmd(0,0);
	return;
}

//-End of Source-------------------------------------------

CHSP3_TASK __HspTaskFunc[]={
(CHSP3_TASK) L0000,
(CHSP3_TASK) L0001,
(CHSP3_TASK) L0002,
(CHSP3_TASK) L0003,
(CHSP3_TASK) L0004,
(CHSP3_TASK) L0005,
(CHSP3_TASK) L0006,
(CHSP3_TASK) L0007,
(CHSP3_TASK) L0008,
(CHSP3_TASK) L0009,
(CHSP3_TASK) L0010,
(CHSP3_TASK) L0011,
(CHSP3_TASK) L0012,
(CHSP3_TASK) L0013,
(CHSP3_TASK) L0014,
(CHSP3_TASK) L0015,
(CHSP3_TASK) L0016,
(CHSP3_TASK) L0017,
(CHSP3_TASK) L0018,
(CHSP3_TASK) L0019,
(CHSP3_TASK) L0020,
(CHSP3_TASK) L0021,
(CHSP3_TASK) L0022,
(CHSP3_TASK) L0023,
(CHSP3_TASK) L0024,
(CHSP3_TASK) L0025,
(CHSP3_TASK) 0,

};

/*-----------------------------------------------------------*/


/*-----------------------------------------------------------*/

void __HspInit( Hsp3r *hsp3 ) {
	hsp3->Reset( _HSP3CNV_MAXVAR, _HSP3CNV_MAXHPI );
	hsp3->SetDataName( 0 );
	hsp3->SetFInfo( 0, 0 );
	hsp3->SetLInfo( 0, 0 );
	hsp3->SetMInfo( 0, 0 );
}

/*-----------------------------------------------------------*/

