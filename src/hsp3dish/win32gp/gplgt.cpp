#include "gamehsp.h"

#include "../../hsp3/hsp3config.h"
#include "../supio.h"
#include "../sysreq.h"

/*------------------------------------------------------------*/
/*
		Light process
*/
/*------------------------------------------------------------*/

Light *gamehsp::getLight( int objid )
{
	gpobj *obj;
	obj = getObj( objid );
	if ( obj == NULL ) return NULL;
	return obj->_light;
}


int gamehsp::makeNewLgt( int id, int lgtopt, float range, float inner, float outer )
{
	gpobj *obj;
	Node *node;
	Light *light;

	if ( id < 0 ) {
		int newid = makeNullNode();
		obj = getObj( newid );
	} else {
		obj = getObj( id );
	}
	if ( obj == NULL ) return -1;

	node = obj->_node;
	if ( node == NULL ) return -1;

	if ( obj->_light ) {
		node->setLight( NULL );
		SAFE_RELEASE( obj->_light );
	}

	light = NULL;

	switch( lgtopt ) {
	case GPLGT_OPT_POINT:
		light = Light::createPoint( 1.0f, 1.0f, 1.0f, range );
		break;
	case GPLGT_OPT_SPOT:
		light = Light::createSpot( 1.0f, 1.0f, 1.0f, range, inner, outer );
		break;
	default:
		light = Light::createDirectional( 1.0f, 1.0f, 1.0f );
		break;
	}

	if ( light ) {

		//light->setColor( 1.0f, 1.0f, 1.0f );
		node->setLight( light );
		if ( id < 0 ) {
			node->rotateX(MATH_DEG_TO_RAD(-45.0f));
		}
		obj->_light = light;
	}

	obj->_vec[GPOBJ_USERVEC_COLOR].set( 1.0f, 1.0f, 1.0f, 1.0f );
	obj->_vec[GPOBJ_USERVEC_DIR].set( 0.25f, 0.25f, 0.25f, 1.0f );

	return obj->_id;
}


int gamehsp::selectLight( int lightid, int index )
{
	Light *light;
	int opt;

	if ( lightid >= 0 ) {
		light = getLight( lightid );
		if ( light == NULL ) return -1;
		if ((index < 0) || (index >= BUFSIZE_MULTILIGHT)) {
			return -1;
		}
		opt = light->getLightType();
		switch (opt) {
		case gameplay::Light::DIRECTIONAL:
			if (index < _max_dlight) {
				_dir_light[index] = lightid;
			}
			break;
		case gameplay::Light::POINT:
			if (index < _max_plight) {
				_point_light[index] = lightid;
			}
			break;
		case gameplay::Light::SPOT:
			if (index < _max_slight) {
				_spot_light[index] = lightid;
			}
			break;
		default:
			return -1;
		}
	}
	return 0;
}


int gamehsp::getSelectLight(int index, int opt)
{
	int res = _deflight;
	if ((index < 0)||(index >= BUFSIZE_MULTILIGHT)) {
		return res;
	}
	switch (opt) {
	case LIGHT_OPT_NORMAL:
		res = _dir_light[index];
		break;
	case LIGHT_OPT_POINT:
		res = _point_light[index];
		break;
	case LIGHT_OPT_SPOT:
		res = _spot_light[index];
		break;
	default:
		break;
	}
	return res;
}


void gamehsp::updateLightVector( gpobj *obj, int moc )
{
	Light *light;
	Vector4 *vec;
	light = obj->_light;
	if ( light == NULL ) return;

	switch( moc ) {
	case MOC_COLOR:
		vec = &obj->_vec[ GPOBJ_USERVEC_COLOR ];
		light->setColor( vec->x, vec->y, vec->z );
		break;
	default:
		break;
	}
}




