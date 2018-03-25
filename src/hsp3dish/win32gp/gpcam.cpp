#include "gamehsp.h"

#include "../../hsp3/hsp3config.h"
#include "../supio.h"
#include "../sysreq.h"

/*------------------------------------------------------------*/
/*
		Camera process
*/
/*------------------------------------------------------------*/

Camera *gamehsp::getCamera( int camid )
{
	gpobj *obj = getObj( camid );
	if ( obj == NULL ) return NULL;
	return obj->_camera;
}


int gamehsp::makeNewCam( int id, float fov, float aspect, float near, float far )
{
	gpobj *obj;
	Node *node;
	Camera *camera;

	if ( id < 0 ) {
		int newid = makeNullNode();
		obj = getObj( newid );
	} else {
		obj = getObj( id );
	}
	if ( obj == NULL ) return -1;

	node = obj->_node;
	if ( node == NULL ) return -1;

	if ( obj->_camera ) {
		node->setCamera( NULL );
		SAFE_RELEASE( obj->_camera );
	}

	camera = Camera::createPerspective( fov, aspect, near, far );
	node->setCamera( camera );
	node->setTranslation(0, 0, 100);

	obj->_camera = camera;

	return obj->_id;
}


void gamehsp::selectCamera( int camid )
{
	gpobj *obj = getObj( camid );
	if ( obj == NULL ) return;
	if ( obj->_camera == NULL ) return;

	_curcamera = camid;
	_cameraDefault = obj->_camera;
	_scene->setActiveCamera( _cameraDefault );	// カメラ設定
}


