#ifndef gpphy_H_
#define gpphy_H_

#include "gameplay.h"

using namespace gameplay;

#define GPPHY_FLAG_NONE (0)
#define GPPHY_FLAG_ENTRY (1)

#define GPPAPPLY_FORCE (0)
#define GPPAPPLY_IMPULSE (1)
#define GPPAPPLY_TORQUE (2)
#define GPPAPPLY_TORQUE_IMPULSE (3)


//  HGIMG4 Physics Object
class gpphy {
public:
	gpphy();
	~gpphy();
	void reset( int id );
	int setParameter( int prmid, Vector3 *prm );
	void bindNodeAsBox( Node *node, Vector3 &size, PhysicsRigidBody::Parameters *rigParams );
	void bindNodeAsSphere( Node *node, float radius, Vector3 &center, PhysicsRigidBody::Parameters *rigParams );

	short _flag;						// 存在フラグ
	short _mark;						// マーク処理用
	int _mode;							// モード
	int _id;							// フィジックスオブジェクトID
	PhysicsRigidBody *_colObj;	// コリジョンオブジェクト

};


#endif
