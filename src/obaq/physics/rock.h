#ifndef ROCK_H

//
// OBAQ physics library
// Copyright   : Zener Works, Inc/kuni/onitama
// License     : CC BY-NC-ND 4.0
// See OBAQlicense.txt for information
// This is a part of HSP related product.
// http://hsp.tv/
//
//

#include <stdio.h>
#include <float.h>
#include <math.h>
#include <string.h>

#include "ptttypes.h"
#include "gamePhysics.h"

/*
#define MAX_ROCKBASE_VERTEX		256
#define MAX_ROCKBASE_VERTEXW	1024

#define MAX_ROCK			64
#define MAX_ROCK_AREA		8
#define MAX_ROCK_WIRE		2

#define MAX_WIRE			8
#define MAX_WIRE_POINT		32

const float margin_default = 2.0f;
const float margin_collect = 4.0f;
const float margin_attract = 8.0f;

const float maxRockSpeed = 9.9f;
*/

class ROCK {
	public:
		enum {
			stat_reserve	= 0x00,	// 空き
			stat_sleep		= 0x01,	// パラメータ定義はしたけど出現していない。
			stat_active		= 0x02,	// 出現している。
		};

		enum {
			collisionCheck_none		= 0,	// 出現時コリジョンチェック無し
			collisionCheck_active	= 1,	// 出現時active状態の岩との接触を避ける
			collisionCheck_all		= 2,	// 出現時reserve状態以外の岩との接触を避ける
			collisionCheck_nest		= 3,	// nestフラグの立っている岩から出現させる
		};

		enum {
			addFail_overflow		= -1,
			addFail_collision		= -2,
		};

		enum {
			// 確定
			type_normal		= 0x0000,
			type_inner		= 0x0001,	// 裏表を逆として判定（ほぼ外周専用）
			type_puppet		= 0x0002,	// 制御の一部をゲーム側に移し、移動処理をキャンセル。
										// ※加わった外力は force, torque に入っているため、 weight, momentで割った値と重力加速度を speed, rSpeed に加算すれば通常の物理挙動。
										// ※座標の更新後には、calcVertexW()を呼び、頂点ごとのワールド座標も更新する必要があるので注意。
			type_explode	= 0x0004,	// 機雷や他の爆発、type_igniteが設定されたROCKに触れると、新たに爆発が発生する。
			type_ignite		= 0x0008,	// 接触により、type_explodeが設定されたROCKを爆発させる。

			// 検証中
			type_cancelShot	= 0x0010,	// ショットとの衝突判定を行わない。
		//	type_sponge		= 0x0020,	// 水を吸う。

			type_bindX		= 0x0040,	// x軸移動を固定する。
			type_bindY		= 0x0080,	// y軸移動を固定する。
			type_bindXY		= 0x00c0,	// xy軸移動を固定する。
			type_bindR		= 0x0100,	// 回転を固定する。
			type_bind		= 0x01c0,	// 完全に固定する。

			type_attract	= 0x0600,
			type_attractW	= 0x0200,
			type_attractB	= 0x0400,

			type_item		= 0x0800,

			type_quake		= 0x1000,

			type_float		= 0x2000,

			type_boss		= 0x4000,

			type_ceiling	= 0x8000,

			type_freeze		= 0x10000,

			type_nest		= 0x20000,

			type_bar		= 0x40000,

			type_autowipe	= 0x100000,		// 接触で自動的に消去する
			type_rewind		= 0x80000,		// 角度を自動的に復帰させる
			type_autoguide	= 0x200000,		// 目的地を有効にする

			type_debug		= 0x80000000,
		};

		unsigned int	type;

		unsigned short	sleepCount;		// デクリメントされ、ゼロになったらactiveに状態変更。最初からゼロの場合はデクリメントされずsleep継続。

		unsigned char	stat;

		char			nTotalVertex;
		char			nArea;

		float			damage;
		float			damageLimit;	// 許容ダメージ, マイナスは無敵, ゼロは即死, 破壊判定はゲーム側への移管が妥当
		float			damageShave;	// 放水によるダメージ増加量

		bool			shave;			// 削られていればtrue

		short			startAreaVertex;
		short			startVertex;
		short			startVertexW;

		char			wirenum[MAX_ROCK_WIRE];

		FVECTOR2		center;
		float			angle;

		float			weight;
		float			moment;
		float			buoyancy;

		float			radius;
		float			radiusSqrt;

		float			cost, sint;

		FVECTOR2		speed;
		float			rSpeed;

		FVECTOR2		force;
		float			torque;

		float			torqueV;
		unsigned int	tSign;

		FVECTOR2		tmpCenter;
		float			tmpAngle;

		short			parentRock;
		float			parentAngle;
		float			parentDistance;

		unsigned int	group;				// 所属するグループのビットがONになる
		unsigned int	exceptGroup;		// 衝突判定を除外するグループのビットがONになる
		unsigned int	recordGroup;		// 衝突ログを記録するグループのビットがONになる

		char			ground;				// 接地（他剛体に上側から接触）している場合に|=1, 方向問わず接触で |=2
		char			bombarded;			// 爆発によってダメージを受けた際に立つフラグ

		unsigned char	mineral;			// 破壊時に放出される爆薬数

		unsigned char	stop;

		char			round;

		int				num;
		class ROCKBASE* base;

		float			inertia;			// 惰性(0.0f <= inertia <= 1.0f, default 0.999f), 0.0fに近づくほど謎抵抗でねっとりした動きになる
		float			friction;			// 摩擦(0.0f <= friction <= 1.0f, default - 0.7f), 接触する物体のfrictionと乗算した数値が参照される
		float			damper;				// 吸振(0.0f <= damper <= 1.0f, default - 0.7f), 接触する物体のdamperと乗算した数値が参照される

		float			gravity;

		FVECTOR2		minPos, maxPos;
		float			marginOrg;

		// ゲーム側で利用するパラメータへのポインタ
		class GAMEROCK*	pGameRock;

		short			target_id;				// ターゲットID
		short			target_opt;				// ターゲットoption
		short			target_timer;			// ターゲットtimer
		short			target_int;				// ターゲットインターバル
		float			target_x,target_y;		// ターゲットX,Y
		float			target_px,target_py;	// ターゲット更新値X,Y
		float			target_x1,target_x2;	// ターゲットX設定範囲
		float			target_y1,target_y2;	// ターゲットY設定範囲
		float			target_maxsp;			// ターゲット接近最大速度

		ROCK() {
			init();
		}
		void init(void);

		void setSide(float margin);
		void deleteSide(void);
		void calcVertexW(void);

		void setDamage(float limit_, float shave_ = 0.0f, int mineral_ = 0) {
			damageLimit = limit_;
			damageShave = shave_;
			mineral = mineral_;
		}
		void setWeight(float weight_, float moment_, float buoyancy_ = 0.0f) {
			weight = weight_;
			moment = moment_;
			buoyancy = buoyancy_;
		}

		// 位置を決定
		void setPosition(float x, float y) {
			center.x = x;
			center.y = y;
		}
		// 角度を決定
		void setAngle(float r) {
			angle = r;
		}
		// 位置を移動（速度も設定）
		void movePosition(float x, float y) {
			FVECTOR2 oc = center;
			center.x = x;
			center.y = y;
			speed.x = x - oc.x;
			speed.y = y - oc.y;
		}
		// 角度を回転（角速度も設定）
		void turnAngle(float r) {
			float oa = angle;
			angle = r;
			rSpeed = r - oa;
		}

		// type_innerを後から変更した際のリセット用
		void reset(void) {
			setSide(marginOrg);
			calcVertexW();
		}

		float collision_rock(ROCK* pR2);
		float check_rock(ROCK* pR2);
		float check_rock_rev(ROCK* pR2);
		float collision_point_n(float xw, float yw, ROCK* pR, int &nvv, FVECTOR2* pVertex = NULL);
		float collision_fluid(float xw, float yw, FVECTOR2* pFr);
		float collision_circle(float xw, float yw);

		void push(float xw, float yw, float ax, float ay);
		void blast(float xw, float yw, float power, float nearClip = 1.0f, float farCLip = FLT_MAX);

		void pushMove(float xw, float yw, float ax, float ay);
		void blastMove(float xw, float yw, float power, float nearClip = 1.0f, float farClip = FLT_MAX);
};

class WIRE {
	public:
		short		vertex;		// 接続元の頂点番号
		short		rock;		// 接続先の岩番号（マイナスで背景に固定）
		float		length;		// ワイヤー長さ(マイナスで未登録)
		float		tension;
		FVECTOR2	position;	// 接続先の座標

		short		nPoint;
		char		unfix;
		float		jointLength;

		FVECTOR2	pointPosition[MAX_WIRE_POINT];
		FVECTOR2	pointSpeed[MAX_WIRE_POINT];

		WIRE() {
			init();
		}
		void init(void) {
			length = -1.0f;
		}
};

struct COLLISIONLOG {
	unsigned int	rockNum;	// 接触したrockの番号。下位16bitと上位16bitで別の番号が入っているので注意。
	FVECTOR2*		vertexID;	// 内部処理用。

	FVECTOR2		position;	// 接触位置。 
	FVECTOR2		normal;		// rockNumの値が小さい側から見た、接触面の法線方向。
	float			depth;		// 接触面にめりこんだ距離。
	float			bound;		// 接触面と垂直（はねる）方向の速度。プラス側がめりこみ方向。
	float			slide;		// 接触面と水平（ひきずる）方向の速度。
};

class ROCKBASE {
	public:
		ROCK		rock[MAX_ROCK];
		WIRE		wire[MAX_WIRE];

		char		areaVertex[MAX_ROCKBASE_AREAVERTEX];
		int			nAreaVertex;

		FVECTOR2	vertex[MAX_ROCKBASE_VERTEX];
		FSEQ		side[MAX_ROCKBASE_VERTEX];
		int			nVertex;

		FVECTOR2	vertexW[MAX_ROCKBASE_VERTEXW];
		int			nVertexW;

		FVECTOR2	gravity;
		float		maxBombRadius;

		COLLISIONLOG*	pCollisionLog;
		int				nCollisionLog;
		int				maxCollisionLog;
		int				overflowCollisionLog;

		int				getCollisionParam[2];
		COLLISIONLOG*	getCollisionTmp;

		ROCKBASE() {
			maxCollisionLog = 0;
			init();
		}
		~ROCKBASE() {
			if(maxCollisionLog) {
				delete pCollisionLog;
			}
		}

		// 初期化
		void init(int _maxCollisionLog = 0) {
			nAreaVertex = 0;
			nVertex = 0;
			nVertexW = 0;
			gravity.x = 0.0f;
			gravity.y = 0.01f / 4.0f;

			if(maxCollisionLog) {
				delete pCollisionLog;
			}
			maxCollisionLog = _maxCollisionLog;
			if(maxCollisionLog) {
				pCollisionLog = new COLLISIONLOG[maxCollisionLog];
			}
			resetCollisionLog();
		}

		void setGravity(float x, float y) {
			gravity.x = x;
			gravity.y = y;
		}

		// コリジョンログのリセット
		void resetCollisionLog(void) {
			nCollisionLog = 0;
			overflowCollisionLog = 0;
			getCollisionParam[0] = -1;
			getCollisionParam[1] = -1;
		}

		// rocknumAとrocknumBが衝突している場合に、COLLISIONLOGへのポインタを返す。
		// ※ 衝突は複数箇所で起こることもあるため、呼ばれるごとに次の衝突へと対象が移る。
		// ※ NULLが返るようになったら、それ以上の衝突は存在しない。
		// ※ rocknumBがマイナスの場合は、rocknumAに関する全ての衝突を取得するワイルドカードになる。
		// ※ rocknumAかrocknumBの値が前回の呼び出しと異なる場合は、読み出しのポインタがリセットされる。
		// ※ 手動でリセット後に呼び出しを行いたい場合は、resetをtrueにして呼び出す。
		// ※ COLLISIONLOGの法線方向は、rocknum*の値小さい側から見た法線方向。
		COLLISIONLOG* getCollisionLog(int rocknumA, int rocknumB = -1, bool reset = false) {
			if(rocknumA != getCollisionParam[0] || rocknumB != getCollisionParam[1] || reset == true) {
				getCollisionParam[0] = rocknumA;
				getCollisionParam[1] = rocknumB;
				getCollisionTmp = pCollisionLog;
			}

			if(rocknumB >= 0) {
				unsigned int rocknum;
				if(rocknumA < rocknumB) {
					rocknum = (rocknumA << 16) | rocknumB;
				} else{
					rocknum = (rocknumB << 16) | rocknumA;
				}

				while(getCollisionTmp != (pCollisionLog + nCollisionLog)) {
					COLLISIONLOG* r = getCollisionTmp ++;
					if(r->rockNum == rocknum) {
						return r;
					}
				}
			} else {
				while(getCollisionTmp != (pCollisionLog + nCollisionLog)) {
					COLLISIONLOG* r = getCollisionTmp ++;
					if((r->rockNum & 0x0000ffff) == rocknumA || (r->rockNum >> 16) == rocknumA) {
						return r;
					}
				}
			}

			return NULL;
		}

		void proc(int procCount, bool wake);
		void proc2(void);

	//	void sync(ROCKBASE* pSrc, int cont);
	//	void makeRockMap(unsigned char* pMap, int width, int height);

		int addRock(int stat, int type, float weight, float moment, float buoyancy, float x, float y, float angle, float xsize, float ysize, int nVertex, const FVECTOR2* pVec, float margin, int collisionCheck, unsigned int group, unsigned int exceptGroup);
		int adjustRock(ROCK* pR, int stat, int type, float weight, float moment, float buoyancy, float x, float y, float angle, float xsize, float ysize, int nVertex, const FVECTOR2* pVec, float margin, int collisionCheck);
		void delRock(ROCK* pR);
};

#endif
#define ROCK_H
