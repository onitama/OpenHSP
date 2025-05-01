#ifndef	__ESSPRITE_H__
#define	__ESSPRITE_H__

#include "hspwnd.h"

#if defined(_LANGUAGE_C_PLUS_PLUS)||defined(__cplusplus)||defined(c_plusplus)
extern "C" {
#endif

/*---------------------------------------------------------------------------*/

#define ESSPFLAG_NONE (0)
#define ESSPFLAG_STATIC (0x100)
#define ESSPFLAG_MOVE (0x200)
#define ESSPFLAG_GRAVITY (0x400)
#define ESSPFLAG_SPLINK (0x800)
#define ESSPFLAG_NOWIPE (0x1000)
#define ESSPFLAG_XBOUNCE (0x2000)
#define ESSPFLAG_YBOUNCE (0x4000)
#define ESSPFLAG_BLINK (0x8000)
#define ESSPFLAG_NODISP (0x10000)
#define ESSPFLAG_FADEIN (0x20000)
#define ESSPFLAG_FADEOUT (0x40000)
#define ESSPFLAG_TIMERWIPE (0x80000)
#define ESSPFLAG_BLINK2 (0x100000)
#define ESSPFLAG_EFADE (0x200000)
#define ESSPFLAG_EFADE2 (0x400000)
#define ESSPFLAG_MOVEROT (0x800000)
#define ESSPFLAG_DECORATE (0x1000000)

#define ESSPMAPHIT_NONE (0)
#define ESSPMAPHIT_BGHIT (0x100)
#define ESSPMAPHIT_BLOCKBIT (0x400)
#define ESSPMAPHIT_HITWIPE (0x800)
#define ESSPMAPHIT_WIPEEVENT (0x1000)
#define ESSPMAPHIT_BGOBJ (0x10000)
#define ESSPMAPHIT_STICKSP (0x20000)

#define ESSPBOUND_BOUNDX (1)
#define ESSPBOUND_BOUNDY (2)
#define ESSPBOUND_FOLLOW (4)

#define ESSPSET_POS (0)
#define ESSPSET_ADDPOS (1)
#define ESSPSET_FALL (2)
#define ESSPSET_BOUNCE (3)
#define ESSPSET_ZOOM (4)
#define ESSPSET_ADDZOOM (5)
#define ESSPSET_CENTER (6)
#define ESSPSET_PUTPOS (7)
#define ESSPSET_ADDPOS2 (8)
#define ESSPSET_POS2 (9)
#define ESSPSET_DIRECT (0x1000)
#define ESSPSET_MASKBIT (0x2000)

#define ESSPRES_XBLOCK (0x100)
#define ESSPRES_YBLOCK (0x200)
#define ESSPRES_GROUND (0x400)
#define ESSPRES_EVENT (0x800)

#define ESDRAW_NORMAL (0)
#define ESDRAW_NOMOVE (1)
#define ESDRAW_NOANIM (2)
#define ESDRAW_NOCALLBACK (4)
#define ESDRAW_NODISP (8)
#define ESDRAW_NOSORT (16)
#define ESDRAW_DEBUG (32)

#define ESSPLINK_BGMAP (0x10000)

#define ESMAP_OPT_NOTRANS (1)
#define ESMAP_OPT_USEMASK (0x100)

#define ESMAP_ATTR_MAX (0x1000)
#define ESMAP_ATTR_NONE (0)			//	侵入可能な場所(デフォルト)
#define ESMAP_ATTR_EVENT (64)		//	侵入可能でヒット判定(アイテム)
#define ESMAP_ATTR_HOLD (128)		//	侵入可能だが足場になる
#define ESMAP_ATTR_WALL (192)		//	侵入不可の壁
#define ESMAP_ATTR_ANIM (32)		//	アニメーション有効
#define ESMAP_ATTR_NOTICE (16)		//	通知アイテム
#define ESMAP_ATTR_GROUP (15)		//	グループビット
#define ESMAP_CEL_IGNORE (0x80000000)	//	通知無視ビット

#define ESMAP_OPT_WIPENOTICE (1)
#define ESMAP_OPT_GETEVENT (2)
#define ESMAP_OPT_WIPEEVENT (4)

#define ESMAP_PRM_GMODE (0)
#define ESMAP_PRM_ANIM (1)
#define ESMAP_PRM_GROUP (2)
#define ESMAP_PRM_NOTICE (3)
#define ESMAP_PRM_WIPEDECO (4)
#define ESMAP_PRM_BLOCKBIT (5)
#define ESMAP_PRM_GRAVITY (6)
#define ESMAP_PRM_OPTION (16)

#define ESMAPHIT_NONE (0)
#define ESMAPHIT_HITX (1)
#define ESMAPHIT_HITY (2)
#define ESMAPHIT_HIT (3)
#define ESMAPHIT_EVENT (4)
#define ESMAPHIT_NOTICE (5)
#define ESMAPHIT_SPHIT (6)

#define ESSPF_TIMEWIPE (1)
#define ESSPF_BLINK (2)
#define ESSPF_BLINKWIPE (3)
#define ESSPF_BLINK2 (4)
#define ESSPF_BLINKWIPE2 (5)
#define ESSPF_FADEOUT (6)
#define ESSPF_FADEOUTWIPE (7)
#define ESSPF_FADEIN (8)
#define ESSPF_FADEINWIPE (9)
#define ESSPF_EFADE (10)
#define ESSPF_EFADEWIPE (11)
#define ESSPF_EFADE2 (12)
#define ESSPF_EFADEWIPE2 (13)

#define ESDECO_FRONT (1)
#define ESDECO_MAPHIT (2)
#define ESDECO_GRAVITY (4)
#define ESDECO_ZOOM (8)
#define ESDECO_ROTATE (16)
#define ESDECO_BOOST (32)
#define ESDECO_SCATTER (64)
#define ESDECO_MULTI4 (0x100)
#define ESDECO_MULTI8 (0x200)
#define ESDECO_MULTI16 (0x400)
#define ESDECO_CHR2 (0x1000)
#define ESDECO_CHR4 (0x2000)
#define ESDECO_EPADD (0x4000)
#define ESDECO_FADEOUT (0x8000)

//
//	for rotate expression
//
class ESVec2 {
public:
	double x, y;

	ESVec2() { x = 0.0; y = 0.0; }
	ESVec2(double _x, double _y) : x(_x), y(_y) {}

	double dot(const ESVec2 &other) const {
		return x * other.x + y * other.y;
	}

	ESVec2 normalize() const {
		double len = sqrt(x * x + y * y);
		if (len > 0) {
			return ESVec2(x / len, y / len);
		}
		return ESVec2(0, 0);
	}
};

typedef struct ESHitRect
{
	ESVec2 center;
	ESVec2 halfSize;
	ESVec2 offset;
} ESHitRect;

class ESRectAxis {
public:
	ESRectAxis();
	void setRect( ESHitRect *rect );
	void setRect(  int x, int y, int sx, int sy, int ofsx=0, int ofsy=0 );
	void setRotation(double rotation);
	ESVec2* getCorner(int index);
	ESVec2 getAxis(int index);
	bool checkAxisOverlap(ESRectAxis* rect1, ESRectAxis* rect2, ESVec2* axis);
	bool checkRotatedCollision(ESRectAxis* rect1, ESRectAxis* rect2);

	ESHitRect _master;
	ESHitRect *_rect;
	ESVec2 _corner[4];
	double _rotation;
};

//
//	sprite move flag (fl) condition :
//			( fl=0 : no sprite )
//
//			bit 0-7 : countdown wipe (0=none)
//
//			bit 8 :  $100 static sprite
//			bit 9 :  $200 move (px,py) every
//			bit10 :  $400 gravity move
//			bit11 :  $800 background link move
//			bit12 : $1000 no border wipe
//			bit13 : $2000 X-border bounce
//			bit14 : $4000 Y-border bounce
//
//
//		info number :
//			0,1 = flag (32bit)
//			2,3 = X axis (32bit)
//			4,5 = Y axis (32bit)
//			6,7 = PX prm (32bit)
//			8,9 = PY prm (32bit)
//			 10 = progress counter
//			 11 = Animation counter
//			 12 = CHR code
//			 13 = TYPE code
//		  14,15 = Exec ptr.
//

typedef struct CHRREF
{
	short wid;			//  buffer ID
	short tpflag;		//  Transparent flag
	short lktime, lknum;	//	chr link information
	short bx, by;		//	chr base X,Y
	short bsx, bsy;		//	chr base size of X,Y
	int colx, coly;		//	Collision X,Y offset
	int colsx, colsy;	//	Collision size of X,Y
	int putofsx, putofsy;	//	chr put offset X,Y
} CHRREF;

typedef struct BGHITINFO
{
	int result;			//	hit result
	int celid;			//	hit Cel ID
	int attr;			//	hit Attribute
	int myx, myy;		//	hit Map axis
	int x, y;			//	last Player axis (pixel)
} BGHITINFO;

typedef struct SPDECOINFO
{
	int chr;			//	chr code
	int option;			//	option value
	int direction;		//	move direction
	int speed;			//	move speed
	int life;			//	life count
	int rotp;			//	rotate prm
	int zoomxp;			//	ZOOMX prm
	int zoomyp;			//	ZOOMY prm
} SPDECOINFO;

typedef struct BGMAP
{
	int* varptr;			//	Map reference ptr.
	int* maskptr;			//	Map Mask reference ptr.
	int mapsx, mapsy;		//	Map alloc size
	int sizex, sizey;		//	Map view size (cel)
	int viewx, viewy;		//	Map view axis
	int divx, divy;			//	Cel Div size
	int buferid;			//	Map parts buffer ID
	int tpflag;				//	合成パラメーター
	int animation;			//	Animation Index
	int group;				//	group mask
	int bgoption;			//	BG option
	unsigned short *attr;	//	Cel Attribute
	int notice;				//	Notice Index
	int maphit_cnt;			//	Map hit count
	int sx, sy;				//	Whole Map Size
	int spwipe_chr;			//	Sprite wipe DECO
	int blockbit;			//	attr bit to Block (work)
	int blockbit_org;		//	attr bit to Block
	int def_gravity;		//	default gravity
	std::vector<BGHITINFO> *mem_bghitinfo;	//	Map hit info
} BGMAP;

typedef struct SPOBJ
{
	int	fl;				//	exist flag (ESSPFLAG_*)
	int xx;				//  X axis (16bit固定少数)
	int yy;				//	Y axis (16bit固定少数)
	int px;				//	Gravity/Move X parameters
	int py;				//	Gravity/Move Y parameters
	int progress;		//	Move progress counter
	int ani;			//	chr anim counter
	int chr;			//	chr code
	int type;			//	Attribute type
	int fspx,fspy;		//  落下速度
	int bound;			//  固さ
	int boundflag;		//  バウンドさせるフラグ(1=X 2=Y)
	int option;			//	ユーザーオプション値
	int priority;		//	優先順位
	int tpflag;			//	α値(0-255)+effectパラメーター
	int fadeprm;		//	αフェードパラメーター
	int zoomx;			//	X方向倍率(16bit固定少数)
	int zoomy;			//	Y方向倍率(16bit固定少数)
	int	rotz;			//	回転角度
	int	splink;			//	link to other sprite
	int timer;			//	カウントダウンタイマー値
	int timer_base;		//	カウントダウンタイマー初期値
	int protz;			//	Move RotZ parameter
	int pzoomx, pzoomy;	//	Move ZoomX,ZoomY parameters
	int maphit;			//	マップヒット用フラグ
	int spstick;		//	他のスプライトに吸着(-1=無効)
	int spst_x, spst_y;	//	吸着オフセット
	int moveres;		//	移動結果フラグ
	int xrevchr;		//	X方向リバースキャラクタオフセット
	int yrevchr;		//	Y方向リバースキャラクタオフセット
	int mulcolor;		//	エフェクトカラー(-1=通常)
	int life;			//	ライフカウント(ユーザー使用)
	int lifemax;		//	ライフカウントMAX(ユーザー使用)
	int power;			//	パワーカウント(ユーザー使用)
	unsigned short *sbr;//	callback

} SPOBJ;

class essprite {
public:
	essprite();
	~essprite();
	void reset(void);
	int init(int maxsprite=512, int maxchr=1024, int rotrate=64, int maxmap=16);
	void setSpriteScaleOption( int scaleopt, int clipopt );
	void updateFrame(void);
	int setResolution(HspWnd* wnd, int sx, int sy, int bufferid = 0);
	void setArea(int x, int y, int sx, int sy );
	void setSize(int p1, int p2, int p3, int p4);
	void setSizeEx(int xsize, int ysize, int tpflag, int colsizex=0, int colsizey=0, int colx=0, int coly=0, int putx=0, int puty=0 );
	void setLand(int p1, int p2);
	int setGravity(int p1, int p2, int p3);
	int setBound(int p1, int p2, int p3);
	void setOffset(int p1, int p2);
	int setPattern(int p1, int p2, int p3, int p4, int window_id);
	int setLink(int p1, int p2);

	void clear(int spno);
	void clear(int p1, int p2);
	void setTransparentMode(int tp);
	SPOBJ* resetSprite(int spno);
	int put(int x, int y, int chr, int tpflag = -1, int zoomx = 0x10000, int zoomy = 0x10000, int rotz = 0, int mulcolor = -1, int mode = 0);
	int put2(int x, int y, int chr, int tpflag = -1, int zoomx = 0x10000, int zoomy = 0x10000, int rotz = 0, int mulcolor = -1, int mode = 0);
	int draw(int start, int num, int mode, int start_pri, int end_pri);
	int execSingle(int id);
	int find(int chktype, int spno, int endspno = -1, int step = 0);
	int checkCollisionSub(SPOBJ* sp);
	int checkCollisionSub2(SPOBJ* sp);
	int checkCollision(int spno, int chktype, int rotflag = 0, int startno = 0, int endno = -1);
	int checkCollisionRotateSub(SPOBJ* sp, ESRectAxis* rect, ESRectAxis* rect2);
	int checkCollisionRotate(int spno, int chktype, int rotflag = 0, int startno = 0, int endno = -1);
	int getNear(int spno, int chktype, int range );

	void initMap(void);
	void resetMap(void);
	void deleteMap(int id);
	void deleteMapMask(int id);
	void deleteMapAttribute(int id);
	int putMap(int x, int y, int id);
	int fetchMap(int bgno, int dir, int size, int evtype);
	int setMap(int bgno, int* varptr, int mapsx, int mapsy, int sx, int sy, int buffer, int option);
	int setMapPos(int bgno, int x, int y);
	int setMapMes(int bgno, int x, int y, char *msg, int offset=0);
	int setMapParam(int bgno, int tp, int option);
	int setSpriteMapLink(int bgno, int hitoption);
	void resetMapHitInfo(int bgno);
	BGHITINFO* getMapHitInfo(int bgno, int index);
	BGHITINFO* addMapHitInfo(int bgno, int result, int celid, int attr, int x, int y, int myx, int myy);
	void resetMapAttribute(BGMAP *map);
	int setMapAttribute(int bgno, int start, int end, int attribute);
	int getMapAttribute(int bgno, int celid);
	int updateMapMask(int bgno);
	int getMapMask(BGMAP *map, int x, int y);
	int getMapMaskHit(int bgno,int x,int y, int sizex, int sizey, int px, int py);
	int getMapMaskHit32(int bgno, int x, int y, int sizex, int sizey, int px, int py);
	int getMapMaskHitSub( int bgno, int x, int y, int sizex, int sizey, bool wallonly=false, bool downdir=false );
	int getMapMaskHitSprite(int bgno, int spno, int px, int py);
	int getSpriteAttrHit( int xx, int yy, int xsize, int ysize, bool plane=false );
	int getMapAttrFromPos(int bgno, int x, int y);

	int setSpriteFlag(int spno, int flag, int op=0);
	int setSpritePosChr(int spno, int xx, int yy, int chrno, int option, int pri);
	int registSpriteDecoration(int chr, int opt, int direction, int speed, int life, int entry=-1);
	int setSpriteDecoration(int x, int y, int decoid);
	int setSpriteDecorationSub(int x, int y, int direction, SPDECOINFO *info);
	int setSpritePos(int spno, int xx, int yy, int opt=0);
	int setSpriteAddPos(int spno, int xx, int yy, bool realaxis = false);
	int setSpriteAddPosRate(int spno, int xx, int yy, int rate);
	int setSpriteAddRotZoom(int spno, int rotz, int zoomx, int zoomy);
	int setSpriteChr(int spno, int chrno);
	int setSpriteType(int spno, int type);
	int setSpriteAddDir(int spno, int direction, int dirrate);
	int setSpriteAim(int spno, int xx, int yy, int dirrate);
	int getSpritePos(int* xpos, int* ypos, int spno, int option);
	int modifySpriteAxis(int spno, int endspno, int type, int x, int y, int option);

	int setSpriteParent(int spno, int parent, int option);
	int setSpriteFade(int p1, int p2, int p3);
	void resetTimer(SPOBJ* sp);
	int setSpriteEffect(int id, int tpflag, int mulcolor);
	int setSpriteRotate(int id, int angle, int zoomx, int zoomy, int rate);
	void setSpritePriority(int id, int pri);
	void setSpriteCallback(int p1, unsigned short *callback = NULL);
	int setSpriteStick(SPOBJ *sp, int targetsp);
	int setSpriteStick(int spno, int targetsp);

	SPOBJ* getObj(int id);
	BGMAP* getMap(int id);
	CHRREF* getChr(int id);
	void getSpriteAxis(SPOBJ* sp, int& xx, int& yy);
	void getSpriteCenterAxis(SPOBJ* sp, int& xx, int& yy);
	int getSpriteParentAxis(SPOBJ *sp, int &xx, int &yy, int depth);
	int setSpriteLife(int spno, int life, int option );

	int getEmptyChrNo(void);
	int getEmptySpriteNo(void);
	int getEmptySpriteNo(int sp_from, int sp_to, int step);
	int getParameter(int spno, int prmid, int *value);
	int setParameter(int spno, int prmid, int value, int op=0);
	int getMaxSprites(void) { return spkaz; };
	int getMaxCharacters(void) { return chrkaz; };
	void getDefaultPatternSize(int* xsize, int* ysize);

	int utilGetAngle(int p1, int p2, int p3, int p4);
	int utilGetSin(int p1);
	int utilGetCos(int p1);
	int utilGetDistance(int x1, int y1, int x2, int y2);
	double utilGetDistanceFloat(int x1, int y1, int x2, int y2);

	//	Open Parameters
	//
	bool	sprite_enable;
	int		sprite_opt;			// Sprite Draw Type
	bool	sprite_newfunc;		// Sprite New function
	bool	sprite_clip;		// Sprite Clipping

protected:
	void execTimerFade(SPOBJ* sp);
	void execTimerEndFade(SPOBJ* sp);
	int drawSubMove(SPOBJ* sp, int mode);
	int drawSubPut(SPOBJ* sp, int mode);
	int drawSubMoveGravity(SPOBJ* sp);
	int drawSubMoveVector(SPOBJ* sp);
	void setDebugColor(int num, int color) { debugcolor_line[num] = color; };

private:
	//	Parameters
	//
	HspWnd* hspwnd;	// Parent Window Handle
	Bmscr* bmscr;

	int		spkaz;	// Max sprite object
	SPOBJ* mem_sp;
	int		chrkaz;	// Max pattern object
	CHRREF* mem_chr;
	CHRREF* chr;
	int		mapkaz;	// Max map object
	BGMAP* mem_map;
	std::vector<SPDECOINFO> mem_deco;
	ESRectAxis rectaxis;

	int		main_sx, main_sy;	// default window size
	int		ofsx, ofsy;			// for sprite offset
	int		ox1, oy1, ox2, oy2;	// valid area
	int		land_x, land_y;		// common ground axis
	double	pi, pans;
	int		resox, resoy;		// 画面解像度
	int		rrate;				// 周の分解能

	int* vpx, *vpy;				// sin,cosテーブル

	int		dotshift;			// 座標シフト値
	int		dotshift_base;		// 座標シフト値(1.0)

	int		df_bsx, df_bsy, df_colx, df_coly;
	int		df_colsx, df_colsy, df_tpflag;
	int		df_putofsx, df_putofsy;
	int		def_fspx, def_fspy;
	int		def_bound;
	int		def_boundflag;

	int		def_maplink;
	int		def_maphitflag;

	int		colx, coly, colex, coley;
	int		fade_mode, fade_upd, fade_val, fade_tar;

	int		framecount;			// frame count
	int		hitattr;			// Map hit attr backup (8bit)
	int		hitattr_org;		// Map hit attr backup (16bit)
	int		hitcelid;			// Map hit cel backup
	int		hitmapx, hitmapy;	// Map hit axis backup
	int		bak_hitmapx;		// Map hit axis backup (previous)
	int		bak_hitmapy;		// Map hit axis backup (previous)

	int		debugcolor_line[4];
};

//	sprite pack info ( for sort )
typedef struct {
	int ikey;
	int info;
} SPRDATA;


/*---------------------------------------------------------------------------*/

#if defined(_LANGUAGE_C_PLUS_PLUS)||defined(__cplusplus)||defined(c_plusplus)
}
#endif

#endif
