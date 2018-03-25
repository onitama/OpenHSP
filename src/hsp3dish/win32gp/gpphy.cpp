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
}

gpphy::~gpphy()
{
}

void gpphy::reset( int id )
{
	_mode = 0;
	_mark = 0;
	_colObj = NULL;
	_flag = GPPHY_FLAG_ENTRY;
	_id = id;
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
	default:
		return -1;

	}
	return 0;
}


void gpphy::bindNodeAsBox( Node *node, Vector3 &size, PhysicsRigidBody::Parameters *rigParams )
{
	PhysicsCollisionObject *pco;
	pco =node->setCollisionObject(PhysicsCollisionObject::RIGID_BODY, 
			PhysicsCollisionShape::box( size ), rigParams );
	_colObj = dynamic_cast<PhysicsRigidBody*>(pco);
}


void gpphy::bindNodeAsSphere( Node *node, float radius, Vector3 &center, PhysicsRigidBody::Parameters *rigParams )
{
	PhysicsCollisionObject *pco;
	pco = node->setCollisionObject(PhysicsCollisionObject::RIGID_BODY, 
			PhysicsCollisionShape::sphere( radius, center ), rigParams );
	_colObj = dynamic_cast<PhysicsRigidBody*>(pco);
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


gpphy *gamehsp::setPhysicsObjectAuto( gpobj *obj, float mass, float friction )
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
	
	PhysicsRigidBody::Parameters rigParams;

	rigParams.mass = mass;	// 重さ
	rigParams.friction = friction;
	rigParams.restitution = 0.5f;
	rigParams.linearDamping = 0.1f;
	rigParams.angularDamping = 0.5f;

	switch( obj->_shape ) {
	case GPOBJ_SHAPE_BOX:
		phy->bindNodeAsBox( node, obj->_sizevec, &rigParams );
		break;
	case GPOBJ_SHAPE_FLOOR:
	case GPOBJ_SHAPE_PLATE:
	    rigParams.restitution = 0.75f;
	    rigParams.linearDamping = 0.025f;
	    rigParams.angularDamping = 0.16f;
		phy->bindNodeAsBox( node, obj->_sizevec, &rigParams );
		break;
	case GPOBJ_SHAPE_MODEL:
		{
		BoundingSphere sphere;
		sphere = node->getBoundingSphere();
		phy->bindNodeAsSphere( node, sphere.radius, sphere.center, &rigParams );
		break;
		}
	default:
		return NULL;
	}

	obj->_phy = phy;
	return phy;
}


int gamehsp::setObjectBindPhysics( int objid, float mass, float friction )
{
	gpobj *obj;
	gpphy *phy;
	obj = getObj( objid );
	if ( obj == NULL ) return -1;

	phy = setPhysicsObjectAuto( obj, mass, friction );
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

