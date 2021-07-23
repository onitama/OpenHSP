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

#define BIND_PHYSICS_NOSCALE (1)
#define BIND_PHYSICS_MESH (2)

class gpobj;

//  HGIMG4 Collision Info
class gppinfo {
public:
	gppinfo();
	~gppinfo();
	int _objid;			// 衝突したObjID
	Vector3 _pos;		// 位置
	float _force;		// 力の強さ
};

//  HGIMG4 Physics Object
class gpphy {
public:
	gpphy();
	~gpphy();
	void reset( gpobj *parent );
	int setParameter(int prmid, Vector3 *prm);
	void setCollision(PhysicsCollisionObject *obj);
	void bindNodeAsBox( Node *node, Vector3 &size, PhysicsRigidBody::Parameters *rigParams );
	void bindNodeAsSphere( Node *node, float radius, Vector3 &center, PhysicsRigidBody::Parameters *rigParams );
	void bindNodeAsMesh(Node *node, Mesh *mesh, PhysicsRigidBody::Parameters *rigParams);
	int contactTest(btCollisionWorld::ContactResultCallback *callback);

	short _flag;						// 存在フラグ
	short _mark;						// マーク処理用
	int _option;						// 作成オプション
	int _id;							// フィジックスオブジェクトID
	PhysicsRigidBody *_colObj;			// コリジョンオブジェクト
	gpobj *_parent;						// 親のgpobj
	std::vector<gppinfo> _ainfo;		// gppinfo配列

};


#endif
