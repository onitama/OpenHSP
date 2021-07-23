#include "gamehsp.h"

#include "../../hsp3/hsp3config.h"
#include "../supio.h"
#include "../sysreq.h"

/*------------------------------------------------------------*/
/*
		gameplay Physics Obj
*/
/*------------------------------------------------------------*/

gpphy::gpphy()
{
	// コンストラクタ
	_flag = GPPHY_FLAG_NONE;
	_colObj = NULL;
	_parent = NULL;
}

gpphy::~gpphy()
{
}

void gpphy::reset( gpobj *parent )
{
	_option = 0;
	_mark = 0;
	_colObj = NULL;
	_parent = parent;
	_flag = GPPHY_FLAG_ENTRY;
	_id = parent->_id;
}


int gpphy::setParameter( int prmid, Vector3 *prm )
{
	if ( _colObj == NULL ) return -1;

	switch( prmid ) {

	case GPPSET_ENABLE:
		_colObj->setEnabled( prm->x != 0.0f );
		break;
	case GPPSET_FRICTION:
		_colObj->setFriction( prm->x );
		_colObj->setRestitution( prm->y );
		break;
	case GPPSET_DAMPING:
		_colObj->setDamping( prm->x, prm->y );
		break;
	case GPPSET_KINEMATIC:
		_colObj->setKinematic( prm->x != 0.0f );
		break;
	case GPPSET_ANISOTROPIC_FRICTION:
		_colObj->setAnisotropicFriction( *prm );
		break;
	case GPPSET_GRAVITY:
		_colObj->setGravity( *prm );
		break;
	case GPPSET_LINEAR_FACTOR:
		_colObj->setLinearFactor( *prm );
		break;
	case GPPSET_ANGULAR_FACTOR:
		_colObj->setAngularFactor( *prm );
		break;
	case GPPSET_ANGULAR_VELOCITY:
		_colObj->setAngularVelocity( *prm );
		break;
	case GPPSET_LINEAR_VELOCITY:
		_colObj->setLinearVelocity( *prm );
		break;
	case GPPSET_MASS_CENTER:
		_colObj->setCenterOfMassOffset( *prm );
		break;
	default:
		return -1;

	}
	return 0;
}


void gpphy::setCollision(PhysicsCollisionObject *obj)
{
	obj->setUserPtr( _parent );
	_colObj = dynamic_cast<PhysicsRigidBody*>(obj);
}


void gpphy::bindNodeAsBox( Node *node, Vector3 &size, PhysicsRigidBody::Parameters *rigParams )
{
	PhysicsCollisionObject *pco;
	pco =node->setCollisionObject(PhysicsCollisionObject::RIGID_BODY, 
			PhysicsCollisionShape::box( size ), rigParams );
	setCollision(pco);
}


void gpphy::bindNodeAsSphere(Node *node, float radius, Vector3 &center, PhysicsRigidBody::Parameters *rigParams)
{
	PhysicsCollisionObject *pco;
	pco = node->setCollisionObject(PhysicsCollisionObject::RIGID_BODY,
		PhysicsCollisionShape::sphere(radius, center), rigParams);
	setCollision(pco);
}


void gpphy::bindNodeAsMesh(Node *node, Mesh *mesh, PhysicsRigidBody::Parameters *rigParams)
{
	PhysicsCollisionObject *pco;
	pco = node->setCollisionObject(PhysicsCollisionObject::RIGID_BODY,
		PhysicsCollisionShape::mesh(mesh), rigParams);
	setCollision(pco);
}

int gpphy::contactTest(btCollisionWorld::ContactResultCallback *callback)
{
	if (_colObj == NULL) return -1;

	_ainfo.clear();

	PhysicsController *pc = Game::getInstance()->getPhysicsController();
	bool res = pc->execContactTest(_colObj, callback);
	if (res == false) return -1;

	return (int)_ainfo.size();
}


/*------------------------------------------------------------*/
/*
		Physics process
*/
/*------------------------------------------------------------*/

gpphy *gamehsp::getPhy( int objid )
{
	gpobj *obj;
	gpphy *phy;
	obj = getObj( objid );
	if ( obj == NULL ) return NULL;

	phy = obj->_phy;
	return phy;
}


gppinfo::gppinfo()
{
	_objid = -1;
}
gppinfo::~gppinfo()
{
}

btScalar gamehsp::CollisionCallback::addSingleResult(btManifoldPoint& cp, const btCollisionObjectWrapper* a, int partIdA, int indexA, const btCollisionObjectWrapper* b, int partIdB, int indexB)
{
	// Get pointers to the PhysicsCollisionObject objects.
	PhysicsCollisionObject* objectA = _pc->getCollisionObject(a->m_collisionObject);
	gpobj *objA = (gpobj *)objectA->getUserPtr();

	int colgroup = objA->_colgroup;
	if (colgroup == 0) return 0.0;

	PhysicsCollisionObject* objectB = _pc->getCollisionObject(b->m_collisionObject);
	gpobj *objB = (gpobj *)objectB->getUserPtr();

	if ((objB->_mygroup & colgroup) == 0) return 0.0;		// Check collision group

	gpphy *phy = objA->_phy;
	if (phy) {
		gppinfo info;
		btVector3 pos;
		info._objid = objB->_id;
		pos = cp.getPositionWorldOnA();
		info._pos.set(pos.m_floats[0], pos.m_floats[1], pos.m_floats[2]);
		info._force = (float)cp.getAppliedImpulse();

		phy->_ainfo.push_back(info);
	}

//	Alertf("%d:%d (%f)",objA->_id, objB->_id, dist);

	return 0.0;
}


int gamehsp::getPhysicsContact(int objid)
{
	gpphy *phy = getPhy(objid);
	if (phy == NULL) return -1;
	return phy->contactTest(_collision_callback);
}


gppinfo *gamehsp::getPhysicsContactInfo(int objid, int index)
{
	gpphy *phy = getPhy(objid);
	if (phy == NULL) return NULL;
	if ((index < 0) || (index >= (int)phy->_ainfo.size())) return NULL;
	return &phy->_ainfo[index];
}


gpphy *gamehsp::setPhysicsObjectAuto( gpobj *obj, float mass, float friction, int option )
{
	gpphy *phy;
	Node *node = obj->_node;
	if ( node == NULL ) return NULL;

	if ( obj->_shape < 0 ) return NULL;

	if ( obj->_phy ) {
		delete obj->_phy;
		obj->_phy = NULL;
	}

	phy = new gpphy;
	if ( phy == NULL ) return NULL;
	phy->reset( obj );
	phy->_option = option;
	
	PhysicsRigidBody::Parameters rigParams;

	rigParams.mass = mass;	// 重さ
	rigParams.friction = friction;
	rigParams.restitution = 0.5f;
	rigParams.linearDamping = 0.1f;
	rigParams.angularDamping = 0.5f;

	Vector3 vec3;

	switch( obj->_shape ) {
	case GPOBJ_SHAPE_BOX:
		vec3 = obj->_sizevec;
		if ((option & BIND_PHYSICS_NOSCALE)==0) {
			Vector3 exvec = node->getScale();
			vec3.mul(exvec);
		}
		phy->bindNodeAsBox( node, vec3, &rigParams );
		break;
	case GPOBJ_SHAPE_FLOOR:
	case GPOBJ_SHAPE_PLATE:
		vec3 = obj->_sizevec;
		if ((option & BIND_PHYSICS_NOSCALE)==0) {
			Vector3 exvec = node->getScale();
			vec3.mul(exvec);
		}
		rigParams.restitution = 0.75f;
	    rigParams.linearDamping = 0.025f;
	    rigParams.angularDamping = 0.16f;
		phy->bindNodeAsBox( node, vec3, &rigParams );
		break;
	case GPOBJ_SHAPE_MODEL:
	case GPOBJ_SHAPE_MESH:
		{
		if (option & BIND_PHYSICS_MESH) {
			Model *model = (Model *)getDrawable(node);
			if (model) {
				Mesh *mesh = model->getMesh();
				if (mesh) {
					phy->bindNodeAsMesh(model->getNode(), mesh, &rigParams);
				}
			}
			break;
		}
		BoundingSphere sphere;
		Vector3 vec3 = node->getTranslation();
		sphere = node->getBoundingSphere();
		sphere.center.x -= vec3.x;
		sphere.center.y -= vec3.y;
		sphere.center.z -= vec3.z;
		phy->bindNodeAsSphere(node, sphere.radius, sphere.center, &rigParams);
		Model *model = (Model *)getDrawable(node);
		if (model) {
			Mesh *mesh = model->getMesh();
			if (mesh) {
				sphere = mesh->getBoundingSphere();
			}
		}
		break;
		}
	default:
		return NULL;
	}

	obj->_phy = phy;
	return phy;
}


int gamehsp::setObjectBindPhysics( int objid, float mass, float friction, int option )
{
	gpobj *obj;
	gpphy *phy;
	obj = getObj( objid );
	if ( obj == NULL ) return -1;

	phy = setPhysicsObjectAuto( obj, mass, friction, option );
	if ( phy == NULL ) return -1;

	return 0;
}


int gamehsp::objectPhysicsApply( int objid, int type, Vector3 *prm )
{
	gpphy *phy;
	PhysicsRigidBody *colObj;
	phy = getPhy( objid );
	if ( phy == NULL ) return -1;

	colObj = phy->_colObj;
	if ( colObj == NULL ) return -1;

	switch( type ) {
	case GPPAPPLY_FORCE:
		colObj->applyForce( *prm );
		break;
	case GPPAPPLY_IMPULSE:
		colObj->applyImpulse( *prm );
		break;
	case GPPAPPLY_TORQUE:
		colObj->applyTorque( *prm );
		break;
	case GPPAPPLY_TORQUE_IMPULSE:
		colObj->applyTorqueImpulse( *prm );
		break;
	default:
		return -1;
	}
	return 0;
}


int gamehsp::execPhysicsRayTest(Vector3 *pos, Vector3 *direction, float distance)
{
	//		Ray Test
	//
	Ray ray;
	PhysicsController::HitResult result;
	ray.setOrigin(*pos);
	ray.setDirection(*direction);
	PhysicsController *pc = Game::getInstance()->getPhysicsController();

	bool res = pc->rayTest( ray, distance, &result );
	if (res == false) return 0;

	PhysicsCollisionObject *cobj = result.object;
	gpobj *obj = (gpobj *)cobj->getUserPtr();
	if (obj->_phy == NULL) return -1;

	*pos = result.point;
	*direction = result.normal;

	return obj->_id;
}

