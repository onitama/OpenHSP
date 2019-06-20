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


int gamehsp::makeNewCam( int id, float fov, float aspect, float near, float far, int mode )
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

	if (mode == 0) {
		camera = Camera::createPerspective(fov, aspect, near, far);
	}
	else {
		camera = Camera::createOrthographic(fov, fov, aspect, near, far);

	}
	node->setCamera( camera );
	node->setTranslation(0, 0, 100);


	obj->_camera = camera;

	return obj->_id;
}


int gamehsp::selectCamera( int camid )
{
	gpobj *obj;
	int flag_id;
	flag_id = camid & GPOBJ_ID_FLAGBIT;
	if (flag_id == 0) {
		obj = getObj(camid);
	}
	else {
		//	GPOBJ_ID_EXFLAGの場合
		switch (camid) {
		case GPOBJ_ID_CAMERA:
			obj = getObj(_defcamera);
			break;
		default:
			return -1;
		}
	}

	if ( obj == NULL ) return -1;
	if ( obj->_camera == NULL ) return -1;

	_curcamera = camid;
	_cameraDefault = obj->_camera;
	_scene->setActiveCamera( _cameraDefault );	// カメラ設定
	return 0;
}


