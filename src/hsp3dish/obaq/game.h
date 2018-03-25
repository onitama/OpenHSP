#ifndef GAME_H

#include "../../obaq/physics/ptttypes.h"
#include "../../obaq/physics/vessel.h"
#include "../random.h"
#include "../sysreq.h"

const int target_optposx	= 1;	// 目的地(X)を自動設定する
const int target_optposy	= 2;	// 目的地(Y)を自動設定する
const int target_optincy	= 4;	// 目的地(Y)を自動更新する
const int target_optflipx	= 8;	// 目的地(X)を自動反転する
const int target_optflipy	= 16;	// 目的地(Y)を自動反転する
const int target_optdirect	= 32;	// 目的地へのダイレクト移動
const int target_nowipe		= 64;	// 自動消去を無効にする
const int target_addpos		= 128;	// スピード加算のみ(guideなし)
const int target_addpos2	= 256;	// スピード加算のみ(相対)(guideなし)

class GAMEROCK {
	public:
		enum {
			mattype_none		= 0,
			mattype_sprite_fit	= 1,
			mattype_sprite		= 2,
			mattype_wire		= 3,
			mattype_wire2		= 4,
			mattype_basetype	= 0xfff,
			mattype_delay		= 0x1000,		// あとで描画
		};

		// 管理用
		class GAMEROCK*	next;
		class GAMEROCK*	prev;

		// 対応するROCKを示す
		class ROCK*		pRock;

		// ワーク類
		int		material_type;
		int		material_id;
		int		material_subid;
		int		material_offsetx;
		int		material_offsety;
		float	material_zoomx;
		float	material_zoomy;
		short	material_gmode;
		short	material_grate;

		// ユーザー利用データ
		int		user1,user2,user3;
		float	user4,user5,user6;

		// モデルのサイズ情報
		float	minx,miny,maxx,maxy;

		GAMEROCK() {
			memset(this, 0, sizeof(GAMEROCK));
			user4 = user5 = user6 = 0.0f;
			material_zoomx = material_zoomy = 1.0f;
		};
		void UpdateMinMaxInfo( void );
};

class GAME {
	public:
		enum {
			objtype_normal		= 0x80000000,
			objtype_player		= 0x00000001,	// プレイヤー
			objtype_enemy		= 0x00000002,	// 敵
			objtype_deathobj	= 0x00000004,	// 敵を爆発させる物体
			objtype_target		= 0x00000008,	// ターゲット
			objtype_wall		= 0x00000010,	// 壁類
			objtype_item		= 0x00000020,	// アイテム
			objtype_targetenemy	= 0x00000040,	// ターゲット敵
			objtype_helper		= 0x00000080,	// プレイヤー
			objtype_rock		= 0x00000100,	// 岩
			objtype_sponge		= 0x00000200,	// 爆薬供給源
		};

		enum {
			shapetype_polygon	= 0,

			shapetype_triangle	= 3,
			shapetype_square	= 4,
			shapetype_pentagon	= 5,
			shapetype_hexagon	= 6,
			shapetype_heptagon	= 7,
			shapetype_octagon	= 8,

			shapetype_testobj	= 100,
		};

		VESSEL*	pVessel;


		// ゲーム進行用
		int		gameTick;
		float	border_x1, border_y1, border_x2, border_y2;

		RAND	random2;

		// GAMEROCK
		class GAMEROCK *startGameRock;
		class GAMEROCK *endGameRock;

		// test
		int		rocknum_wall;
		COLLISIONLOG *curlog;
		int		collision_id;
		int		collision_target;

		GAME() {
			startGameRock = NULL;
			endGameRock = NULL;
		};
		~GAME() {
			GAMEROCK* pG = startGameRock;
			while(pG) {
				pG = delGameRock(pG);
			}
		};

		void init(VESSEL* pVessel_);
		void proc(void);

		int addRock(int stat, int type, float x, float y, float angle, float xsize, float ysize, int nvertex, const FVECTOR2* pVec, float margin, int collisionCheck, unsigned int group=0, unsigned int exceptGroup=0);

		class GAMEROCK* delRock(GAMEROCK* pG);
		int addShape(int shape, int stat, int type, float x, float y, float r, float xsize, float ysize, int coll, int mygroup, int exgroup, int loggroup );
		int setBorder( float x1, float y1, float x2, float y2, float cen_x, float cen_y );

		class GAMEROCK* addGameRock(void);
		class GAMEROCK* delGameRock(GAMEROCK* pG);

		//		Service Functions
		//
		class ROCK* getRock( int num );
		class GAMEROCK* getGameRock( int num );
		void getCollision( int num, int target );
		COLLISIONLOG* getCollisionLog( void );

		void setRockWeight( int num, float weight, float moment );
		void setRockDamper( int num, float damper, float friction );
		void setRockInertia( int num, float inertia, float gravity );

		void setRockAngle( int num, float angle );
		void setRockCenter( int num, float x, float y );
		void setRockSpeed( int num, float px, float py, float pr, int option );
		void setRockGroup( int num, int group, int exgroup, int loggroup );
		void setRockType( int num, unsigned int type, int calcop );
		void setRockStat( int num, unsigned int stat, int sleepcount );
		void setRockMaterial( int num, int type, int matid, int subid );
		void setRockMaterial2( int num, int offsetx, int offsety, float zoomx, float zoomy );
		void setRockMaterial3( int num, int gmode, int rate );
		void setRockUserData( int num, int user1, int user2, int user3 );
		void setRockUserData2( int num, float user4, float user5, float user6 );

		void BlastAll( float ax, float ay, float power, float p_near, float p_far, int sw );
		int CheckInsidePosition( int num, float chkx, float chky );

		float moveRockToTarget( ROCK *pR, float x, float y, float angle, float maxspeed, float maxrotsp, int opt );
		void ExecuteAutoWipe( void );

		void resetRockTarget( int id, int opt, int target_id, float targetx, float targety, float speed );
		void setRockTargetAdd( int id, float px, float py );
		void setRockTargetArea( int id, float x1, float y1, float x2, float y2 );
		int moveRockAutoTarget( ROCK *pR );
		void updateRockAutoTarget( ROCK *pR );
		void setTargetBorder( float centerx, float centery, float y1, float y2 );
		void setTargetDefault( float x1, float y1, float x2, float y2 );
		void setTargetDefaultAdd( float px, float py );
		void setRockTargetTimer( int id, int timer );

		float	target_cenx, target_ceny;
		float	target_px, target_py;
		float	target_y1, target_y2;
		float	target_defx1, target_defy1;
		float	target_defx2, target_defy2;

	private:
};

#endif
#define GAME_H
