#include "gamehsp.h"

#include "../../hsp3/hsp3config.h"
#include "../supio.h"
#include "../sysreq.h"

#include "shader_sprite.h"

// Default sprite shaders
//#define SPRITE_VSH "res/shaders/sprite.vert"
//#define SPRITE_FSH "res/shaders/sprite.frag"
#define SPRITECOL_VSH "res/shaders/spritecol.vert"
#define SPRITECOL_FSH "res/shaders/spritecol.frag"

enum {
CLSMODE_NONE = 0,
CLSMODE_SOLID,
CLSMODE_TEXTURE,
CLSMODE_BLUR,
CLSMODE_MAX,
};

// material defines for load model
static std::string model_defines;

extern bool hasParameter( Material* material, const char* name );


static void QuaternionToEulerAngles(Quaternion q, double& roll, double& pitch, double& yaw)
{
	// roll (x-axis rotation)
	double sinr_cosp = +2.0 * (q.w * q.x + q.y * q.z);
	double cosr_cosp = +1.0 - 2.0 * (q.x * q.x + q.y * q.y);
	roll = atan2(sinr_cosp, cosr_cosp);

	// pitch (y-axis rotation)
	double sinp = +2.0 * (q.w * q.y - q.z * q.x);
	if (fabs(sinp) >= 1)
		pitch = copysign(MATH_PI / 2, sinp); // use 90 degrees if out of range
	else
		pitch = asin(sinp);

	// yaw (z-axis rotation)
	double siny_cosp = +2.0 * (q.w * q.z + q.x * q.y);
	double cosy_cosp = +1.0 - 2.0 * (q.y * q.y + q.z * q.z);
	yaw = atan2(siny_cosp, cosy_cosp);
}

/*------------------------------------------------------------*/
/*
		gameplay Node Obj
*/
/*------------------------------------------------------------*/

gpobj::gpobj()
{
	// コンストラクタ
	_flag = GPOBJ_FLAG_NONE;
}

gpobj::~gpobj()
{
}

void gpobj::reset( int id )
{
	int i;
	_mode = 0;
	_mark = 0;
	_shape = GPOBJ_SHAPE_NONE;
	_spr = NULL;
	_phy = NULL;
	_node = NULL;
	_model = NULL;
	_camera = NULL;
	_light = NULL;
	_animation = NULL;
	_flag = GPOBJ_FLAG_ENTRY;
	_id = id;
	_mygroup = 0;
	_colgroup = 0;
	_transparent = 255;
	_fade = 0;
	_rendergroup = 1;
	_lightgroup = 1;

	_usegpmat = -1;
	_usegpphy = -1;
	_colilog = -1;

	_prm_modalpha = NULL;

	for(i=0;i<GPOBJ_USERVEC_MAX;i++) {
		_vec[i].set( Vector4::zero() );
	}

	//	イベントのリセット
	for (i = 0; i<GPOBJ_MULTIEVENT_MAX; i++) {
		_time[i] = 0.0f;
		_event[i] = NULL;
	}

}


bool gpobj::isVisible( void )
{
	//	表示可能か調べる
	//
	if ( _flag == 0 ) return false;
	if ( _mode & GPOBJ_MODE_HIDE ) {		// 非表示設定
		return false;
	}
	return true;
}


bool gpobj::isVisible( bool lateflag )
{
	//	表示可能か調べる
	//  ( GPOBJ_MODE_LATE指定時か、半透明時は手前の優先度として認識される )
	//	( lateflag : true=手前の描画優先度 )
	//
	bool curflag;
	if ( _flag == 0 ) return false;

	if ( _mode & GPOBJ_MODE_HIDE ) {		// 非表示設定
		return false;
	}
	if ( _transparent <= 0 ) {				// 完全な透明
		return false;
	}
	if ( _mode & GPOBJ_MODE_LATE ) {
		curflag = true;						// 手前を強制
	} else {
		if ( _transparent >= 255 ) {
			curflag = false;				// 通常の優先順位
		} else {
			curflag = true;					// 透明を含む
		}
	}

	return ( curflag == lateflag );
}


float gpobj::getAlphaRate( void )
{
	// Alpha値を取得する
	// ( _transparent値を0.0～1.0に変換する)
	if ( _transparent >= 255 ) return 1.0f;
	if ( _transparent <= 0 ) return 0.0f;
	return ( 1.0f / 255.0f ) * (float)_transparent;
}


void gpobj::updateParameter( Material *mat )
{
	//	マテリアル設定後処理を行なう
	//	パラメーターのポインタ設定など、モデル確定後にgpobjで必要な処理を行なう
	//
	_prm_modalpha = NULL;
	if ( _flag == 0 ) return;
	if ( mat ) {
		if ( hasParameter( mat, "u_modulateAlpha" ) )
			_prm_modalpha = mat->getParameter("u_modulateAlpha");
	}
}


int gpobj::executeFade(void)
{
	//	透明度の自動フェードイン・アウトを制御する
	//
	if (_fade == 0) return 0;
	_transparent += _fade;
	if (_transparent <= 0) {
		_fade = 0;
		_transparent = 0;
		return -1;
	}
	if (_transparent > 255) {
		_fade = 0;
		_transparent = 255;
	}
	return 0;
}


int gpobj::GetEmptyEvent(void)
{
	int i;
	for (i = 0; i<GPOBJ_MULTIEVENT_MAX; i++) {
		if (_event[i] == NULL) return i;
	}
	return -1;
}


void gpobj::DeleteEvent(int entry)
{
	_event[entry] = NULL;
}


void gpobj::StartEvent(gpevent *ev, int entry)
{
	SetEvent(ev, entry);
}


void gpobj::SetEvent(gpevent *ev, int entry)
{
	int i;
	if (entry < 0) {
		i = GetEmptyEvent();
		if (i < 0) return;
		_event[i] = ev;
		_time[i] = 0.0f;
	}
	else {
		_event[entry] = ev;
		_time[entry] = 0.0f;
	}
}


gpevent *gpobj::GetEvent(int entry)
{
	return _event[entry];
}


/*------------------------------------------------------------*/
/*
		gameplay game class
*/
/*------------------------------------------------------------*/

gamehsp::gamehsp()
{
	// コンストラクタ
#ifdef USE_GPBFONT
	mFont = NULL;
#endif
	_maxobj = 0;
	_gpobj = NULL;
	_gpmat = NULL;
	_gpevent = NULL;
	_gptexmes = NULL;
	_texmesbuf = NULL;
	_colrate = 1.0f / 255.0f;
	_scene = NULL;
	_meshBatch = NULL;
	_meshBatch_line = NULL;
	_meshBatch_font = NULL;
	_spriteEffect = NULL;
}

void gamehsp::initialize()
{
#ifdef USE_GPBFONT
	// フォント作成
	mFont = Font::create("res/font.gpb");
#endif
	resetScreen();
}

void gamehsp::finalize()
{
	// release
	//
	deleteAll();

#ifdef USE_GPBFONT
	SAFE_RELEASE(mFont);
#endif
}


void gamehsp::deleteAll( void )
{
	// release
	//
	texmesTerm();

	if ( _gpobj ) {
		int i;
		for(i=0;i<_maxobj;i++) { deleteObj( i ); }
		delete[] _gpobj;
		_gpobj = NULL;
	}

	if (_gpmat) {
		int i;
		for (i = 0; i<_maxmat; i++) { deleteMat(i); }
		delete[] _gpmat;
		_gpmat = NULL;
	}

	if (_gpevent) {
		int i;
		for (i = 0; i<_maxevent; i++) { deleteEvent(i); }
		delete[] _gpevent;
		_gpevent = NULL;
	}

	if (_meshBatch) {
		delete _meshBatch;
		_meshBatch = NULL;
	}
	if (_meshBatch_line) {
		delete _meshBatch_line;
		_meshBatch_line = NULL;
	}

	SAFE_RELEASE(_spriteEffect);

	if (_scene) {
		_scene->removeAllNodes();
		_scene->setActiveCamera(NULL);
		SAFE_RELEASE(_scene);
	}

}


void gamehsp::update(float elapsedTime)
{
}

void gamehsp::render(float elapsedTime)
{
	// 描画先をリセット
	resumeFrameBuffer();

	// ビューポート初期化
	updateViewport(_viewx1, _viewy1, _viewx2, _viewy2);

	// プロジェクションの初期化
	update2DRenderProjectionSystem(&_projectionMatrix2Dpreset);

	// 画面クリア
	clearFrameBuffer();

	// 使用するマトリクスをコピー
	_projectionMatrix2D = _projectionMatrix2Dpreset;
	_projectionMatrix2D.invert(&_projectionMatrix2Dinv);

}

void gamehsp::keyEvent(Keyboard::KeyEvent evt, int key)
{
}

void gamehsp::touchEvent(Touch::TouchEvent evt, int x, int y, unsigned int contactIndex)
{
}

/*------------------------------------------------------------*/
/*
		HSP Script Service
*/
/*------------------------------------------------------------*/

void gamehsp::hookSetSysReq( int reqid, int value )
{
	//	HGIMG4用のSetSysReq
	//
	switch( reqid ) {
	case SYSREQ_VSYNC:
		setVsync( value!=0 );
		break;
	case SYSREQ_FIXEDFRAME:
		{
		double fixedrate = (double)value;
		if (fixedrate < 0.0) {
			setFixedTime(-1);
		}
		else {
			setFixedTime(fixedrate);
		}
		break;
		}
	default:
		break;
	}
}


void gamehsp::hookGetSysReq( int reqid )
{
	//	HGIMG4用のGetSysReq
	//
	switch( reqid ) {
	case SYSREQ_FPS:
		SetSysReq( reqid, (int)getFrameRate() );
		break;
	default:
		break;
	}
}


void gamehsp::resetScreen( int opt )
{
	if ( opt == 1 ) {
		_scene->removeAllNodes();
		return;
	}

	// 画面の初期化
	deleteAll();

	// VSYNCの設定
	setVsync( GetSysReq( SYSREQ_VSYNC )!=0 );

	// gpobj作成
	_maxobj = GetSysReq( SYSREQ_MAXOBJ );
	_gpobj = new gpobj[ _maxobj ];
	for(int i=0;i<_maxobj;i++) { _gpobj[i].addRef(); }
	setObjectPool( 0, -1 );

	// gpmat作成
	_maxmat = GetSysReq( SYSREQ_MAXMATERIAL );
	_gpmat = new gpmat[ _maxmat ];

	// gpevent作成
	_maxevent = GetSysReq(SYSREQ_MAXEVENT);
	_gpevent = new gpevent[_maxevent];

	// シーン作成
	_scene = Scene::create();
	_curscene = 0;
	_previousFrameBuffer = NULL;

	// カメラ作成
	//Camera*	camera = Camera::createPerspective(45.0f, getAspectRatio(), 0.01f, 20.0f );
	//Camera*	camera = Camera::createPerspective(0.25f*3.141592654f, getAspectRatio(), 0.5f, 768.0f );
	//_cameraDefault = Camera::createPerspective( 45.0f, getAspectRatio(), 0.5f, 768.0f );

	_defcamera = makeNewCam(-1, 45.0f, getAspectRatio(), 0.5f, 768.0f);		// カメラを生成
	selectCamera( _defcamera );

//	_camera = _scene->addNode("camera");
//	_camera->setCamera( _cameraDefault );
//	_scene->setActiveCamera( _cameraDefault );	// カメラ設定
//	_camera->translate(0, 0, 100);
//	SAFE_RELEASE(camera);

#if 0
	_scene->setLightColor( 1.0f, 1.0f, 1.0f );

	Vector3 ldir;
	ldir.set( -0.5f, 0.0f, -0.3f );
	_scene->setLightDirection( ldir );
#endif

	// シーンライト作成
	_scene->setAmbientColor(0.25f, 0.25f, 0.25f);

	// ライト作成
	_deflight = makeNewLgt(-1, GPLGT_OPT_NORMAL);
	selectLight(_deflight);

	_max_dlight = GetSysReq(SYSREQ_DLIGHT_MAX);
	if (_max_dlight > BUFSIZE_MULTILIGHT) _max_dlight = BUFSIZE_MULTILIGHT;
	for (int i = 0; i<_max_dlight; i++) { _dir_light[i] = _deflight; }

	_max_plight = GetSysReq(SYSREQ_PLIGHT_MAX);
	if (_max_plight > BUFSIZE_MULTILIGHT) _max_plight = BUFSIZE_MULTILIGHT;
	for (int i = 0; i<_max_plight; i++) { _point_light[i] = _deflight; }

	_max_slight = GetSysReq(SYSREQ_SLIGHT_MAX);
	if (_max_slight > BUFSIZE_MULTILIGHT) _max_slight = BUFSIZE_MULTILIGHT;
	for (int i = 0; i<_max_slight; i++) { _spot_light[i] = _deflight; }

	// シェーダー定義文字列を生成
	setupDefines();

	// ボーダー初期化
	border1.set( -50.0f, 0.0f, -50.0f );
	border2.set( 50.0f, 100.0f,  50.0f );

	// 2D初期化
	init2DRender();

	//makeFloorNode( 20.0f, 20.0f, 0x404040 );
	//makeModelNode( "res/mikuA","mikuA_root" );

	// ビューポート初期化
	updateViewport( 0, 0, getWidth(), getHeight() );

	//	固定フレームレート設定
	double fixedrate = (double)GetSysReq(SYSREQ_FIXEDFRAME);
	if (fixedrate < 0.0) {
		setFixedTime(-1);
	}
	else {
		setFixedTime(fixedrate);
	}

	// gptexmat作成
	texmesInit();

}


void gamehsp::updateViewport( int x, int y, int w, int h )
{
	Rectangle viewport;
	_viewx1 = x; _viewy1 = y;
	_viewx2 = w; _viewy2 = h;
	viewport.set((float)x, (float)y, (float)w, (float)h);
	setViewport( viewport );
}


void gamehsp::selectFrameBuffer(gameplay::FrameBuffer *fb, int sx, int sy)
{
	Rectangle viewport;
	_previousFrameBuffer = fb->bind();
	viewport.set(0, 0, (float)sx, (float)sy);
	setViewport(viewport);
	clearFrameBuffer();
}


void gamehsp::resumeFrameBuffer(void)
{
	if (_previousFrameBuffer) {
		_previousFrameBuffer->bind();
		_previousFrameBuffer = NULL;
	}
}


void gamehsp::clearFrameBuffer(void)
{
	Vector4 clscolor;
	int icolor;
	int clsmode;
	icolor = GetSysReq(SYSREQ_CLSCOLOR);
	clsmode = GetSysReq(SYSREQ_CLSMODE);

	if (clsmode == CLSMODE_NONE) {
		clear(CLEAR_DEPTH, Vector4::zero(), 1.0f, 0);
		return;
	}
	clscolor.set(((icolor >> 16) & 0xff)*_colrate, ((icolor >> 8) & 0xff)*_colrate, (icolor & 0xff)*_colrate, 1.0f);
	clear(CLEAR_COLOR_DEPTH, clscolor, 1.0f, 0);
}


void gamehsp::setBorder(float x0, float x1, float y0, float y1, float z0, float z1)
{
	border1.set( x0, y0, z0 );
	border2.set( x1, y1, z1 );
}


void gamehsp::getBorder( Vector3 *v1, Vector3 *v2 )
{
	*v1 = border1;
	*v2 = border2;
}


void gamehsp::deleteObjectID( int id )
{
	//	指定されたIDのオブジェクトを削除する
	//	(gpobj,gpmat,gplgt,gpcam,gpphyで使用可能)
	//
	int flag_id;
	int base_id;
	flag_id = id & GPOBJ_ID_FLAGBIT;
	base_id = id & GPOBJ_ID_FLAGMASK;
	switch( flag_id ) {
	case GPOBJ_ID_MATFLAG:
		deleteMat( id );
		return;
	default:
		break;
	}
	deleteObj( base_id );
}


bool gamehsp::init2DRender( void )
{
	// 2D用の初期化
	//
	proj2Dcode = -2;

	// 2D用のプロジェクション
	make2DRenderProjection(&_projectionMatrix2Dpreset,getWidth(),getHeight());

	// スプライト用のshader
	_spriteEffect = Effect::createFromSource(intshd_sprite_vert, intshd_sprite_frag);
	//_spriteEffect = Effect::createFromFile(SPRITE_VSH, SPRITE_FSH);
	if ( _spriteEffect == NULL ) {
        GP_ERROR("2D shader initalize failed.");
        return false;
	}

	// MeshBatch for FlatPolygon Draw
	Material* mesh_material = make2DMaterialForMesh();
    VertexFormat::Element elements[] =
    {
        VertexFormat::Element(VertexFormat::POSITION, 3),
        VertexFormat::Element(VertexFormat::COLOR, 4)
    };
    unsigned int elementCount = sizeof(elements) / sizeof(VertexFormat::Element);
    _meshBatch = MeshBatch::create(VertexFormat(elements, elementCount), Mesh::TRIANGLE_STRIP, mesh_material, true, 16, 16 );

	SAFE_RELEASE(mesh_material);

	mesh_material = make2DMaterialForMesh();
	_meshBatch_line = MeshBatch::create(VertexFormat(elements, elementCount), Mesh::LINES, mesh_material, true, 4, 4 );

	int i;
	for(i=0;i<BUFSIZE_POLYCOLOR;i++) {
		_bufPolyColor[i] = 0.0f;
	}
	for(i=0;i<BUFSIZE_POLYTEX;i++) {
		_bufPolyTex[i] = 0.0f;
	}

	SAFE_RELEASE(mesh_material);
	return true;
}


void gamehsp::make2DRenderProjection(Matrix *mat,int sx, int sy)
{
	//	2Dシステム用のプロジェクションを作成する
	Matrix::createOrthographicOffCenter(0.0f, (float)sx, (float)sy, 0.0f, -1.0f, 1.0f, mat);
	//mat->translate(0.5f, 0.0f, 0.0f);						// 座標誤差修正のため0.5ドットずらす
}


void gamehsp::update2DRenderProjection(Material* material, Matrix *mat)
{
	//	マテリアルのプロジェクションを再設定する
	MaterialParameter *prm = material->getParameter("u_projectionMatrix");
	if (prm) {
		prm->setValue(*mat);
	}
}


void gamehsp::update2DRenderProjectionSystem(Matrix *mat)
{
	//	2Dシステム用のプロジェクションを再設定する
	if (_meshBatch) update2DRenderProjection(_meshBatch->getMaterial(), mat);
	if (_meshBatch_line) update2DRenderProjection(_meshBatch_line->getMaterial(), mat);
	if (_meshBatch_font) update2DRenderProjection(_fontMaterial, &_projectionMatrix2D);

}


void gamehsp::setUser2DRenderProjectionSystem(Matrix* mat, bool updateinv)
{
	//	ユーザー用のプロジェクションを設定する
	_projectionMatrix2D = *mat;
	update2DRenderProjectionSystem(&_projectionMatrix2D);
	if (updateinv) {
		proj2Dcode--;
		_projectionMatrix2D.invert(&_projectionMatrix2Dinv);
	}
}


void gamehsp::convert2DRenderProjection(Vector4& pos)
{
	//	ユーザー用のプロジェクションを逆変換する
	Vector4 result;
	_projectionMatrix2Dinv.transformVector(pos,&result);
	pos.x = result.x;
	pos.y = result.y;
	pos.z = result.z;
}


int gamehsp::selectScene( int sceneid )
{
	_curscene = sceneid;
	return 0;
}


int gamehsp::setObjName( int objid, char *name )
{
	Node *node;
	node = getNode( objid );
	if ( node == NULL ) return -1;
	node->setId( name );
	return 0;
}


char *gamehsp::getObjName( int objid )
{
	Node *node;
	node = getNode( objid );
	if ( node == NULL ) return NULL;
	return (char *)node->getId();
}

	
void gamehsp::addSceneVector( int ch, Vector4 *prm )
{
}


void gamehsp::addNodeVector( gpobj *obj, Node *node, int moc, Vector4 *prm )
{
	switch(moc) {
	case MOC_POS:
		if ( node ) {
			node->translate( prm->x, prm->y, prm->z );
		}
		break;
	case MOC_QUAT:
		if ( node ) {
			node->rotate( prm->x, prm->y, prm->z, prm->w );
		}
		break;
	case MOC_SCALE:
		if ( node ) {
			Vector3 vec3 = node->getScale();
			vec3.x += prm->x;
			vec3.y += prm->y;
			vec3.z += prm->z;
			node->setScale(vec3);
			//node->scale(prm->x, prm->y, prm->z);		// 掛け算だったので修正
		}
		break;
	case MOC_DIR:
		if ( obj ) {
			obj->_vec[GPOBJ_USERVEC_DIR] += *prm;
		}
		break;
	case MOC_EFX:
		break;
	case MOC_COLOR:
		if ( obj ) {
			obj->_vec[GPOBJ_USERVEC_COLOR] += *prm;
			if ( obj->_light ) updateLightVector( obj, moc );
		}
		break;
	case MOC_WORK:
		if ( obj ) {
			obj->_vec[GPOBJ_USERVEC_WORK] += *prm;
		}
		break;
	case MOC_WORK2:
		if ( obj ) {
			obj->_vec[GPOBJ_USERVEC_WORK2] += *prm;
		}
		break;

	case MOC_AXANG:
		break;
	case MOC_ANGX:
		if ( node ) {
			node->rotateX( prm->x );
			node->rotateY( prm->y );
			node->rotateZ( prm->z );
		}
		break;
	case MOC_ANGY:
		if ( node ) {
			node->rotateY( prm->y );
			node->rotateZ( prm->z );
			node->rotateX( prm->x );
		}
		break;
	case MOC_ANGZ:
		if ( node ) {
			node->rotateZ( prm->z );
			node->rotateY( prm->y );
			node->rotateX( prm->x );
		}
		break;


	}
}


void gamehsp::addSpriteVector( gpobj *obj, int moc, Vector4 *prm )
{
	gpspr *spr;
	spr = obj->_spr;
	if ( spr == NULL ) return;

	switch(moc) {
	case MOC_POS:
		spr->_pos.add( *prm );
		break;
	case MOC_SCALE:
		spr->_scale.add( *prm );
		break;
	case MOC_DIR:
		obj->_vec[GPOBJ_USERVEC_DIR] += *prm;
		break;

	case MOC_ANGX:
	case MOC_ANGY:
	case MOC_ANGZ:
		spr->_ang.add( *prm );
		break;

	case MOC_COLOR:
		obj->_vec[GPOBJ_USERVEC_COLOR] += *prm;
		break;
	case MOC_WORK:
		obj->_vec[GPOBJ_USERVEC_WORK] += *prm;
		break;
	case MOC_WORK2:
		obj->_vec[GPOBJ_USERVEC_WORK2] += *prm;
		break;
	}
}


int gamehsp::addObjectVector( int objid, int moc, Vector4 *prm )
{
	gpobj *obj;
	int flag_id;
	flag_id = objid & GPOBJ_ID_FLAGBIT;
	if ( flag_id == 0 ) {
		obj = getObj( objid );
		if ( obj == NULL ) return -1;
		if ( obj->_spr ) {
			addSpriteVector( obj, moc, prm );
			return 0;
		}
		addNodeVector( obj, obj->_node, moc, prm );
		return 0;
	}

	switch( flag_id ) {
	case GPOBJ_ID_EXFLAG:
		break;
	default:
		return -1;
	}

	//	GPOBJ_ID_EXFLAGの場合
	switch( objid ) {
	case GPOBJ_ID_SCENE:
		addSceneVector( moc, prm );
		return 0;
	case GPOBJ_ID_CAMERA:
		obj = getObj( _defcamera );
		break;
	case GPOBJ_ID_LIGHT:
		obj = getObj( _deflight );
		break;
	default:
		return -1;
	}
	if ( obj == NULL ) return -1;
	addNodeVector( obj, obj->_node, moc, prm );
	return 0;
}


void gamehsp::lookAtNode(Node* node, const Vector3& target )
{
	Vector3 up;
	up.set( 0.0f, 1.0f, 0.0f );

	// Create lookAt matrix
    Matrix matrix;
	Matrix::createLookAt( node->getTranslation(), target, up, &matrix);
    matrix.transpose();

    // Pull rotation out of matrix
    Quaternion rotation;
    matrix.getRotation(&rotation);

    // Set rotation on node
    node->setRotation(rotation);
}



int gamehsp::lookAtObject( int objid, Vector4 *prm )
{
	Vector3 target;
	target.set( prm->x, prm->y, prm->z );
	Node *node;
	node = getNode( objid );
	if ( node == NULL ) return -1;
	lookAtNode( node, target );
	return 0;
}


void gamehsp::setSceneVector( int ch, Vector4 *prm )
{
}


void gamehsp::setSpriteVector( gpobj *obj, int moc, Vector4 *prm )
{
	gpspr *spr;
	spr = obj->_spr;
	if ( spr == NULL ) return;

	switch(moc) {
	case MOC_POS:
		spr->_pos.set( *prm );
		break;
	case MOC_SCALE:
		spr->_scale.set( *prm );
		break;
	case MOC_DIR:
		obj->_vec[GPOBJ_USERVEC_DIR] = *prm;
		break;

	case MOC_ANGX:
	case MOC_ANGY:
	case MOC_ANGZ:
		spr->_ang.set( *prm );
		break;

	case MOC_COLOR:
		obj->_vec[GPOBJ_USERVEC_COLOR] = *prm;
		break;
	case MOC_WORK:
		obj->_vec[GPOBJ_USERVEC_WORK] = *prm;
		break;
	case MOC_WORK2:
		obj->_vec[GPOBJ_USERVEC_WORK2] = *prm;
		break;
	}
}


void gamehsp::setNodeVector( gpobj *obj, Node *node, int moc, Vector4 *prm )
{
	switch(moc) {
	case MOC_POS:
		if ( node ) {
			node->setTranslation( prm->x, prm->y, prm->z );
		}
		break;
	case MOC_QUAT:
		if ( node ) {
			node->setRotation( prm->x, prm->y, prm->z, prm->w );
		}
		break;
	case MOC_SCALE:
		if ( node ) {
			node->setScale( prm->x, prm->y, prm->z );
		}
		break;
	case MOC_DIR:
		if ( obj ) {
			obj->_vec[GPOBJ_USERVEC_DIR] = *prm;
		}
		break;
	case MOC_EFX:
		break;

	case MOC_ANGX:
		if ( node ) {
			Matrix matrix;
			matrix.identity();
			matrix.rotateX( prm->x );
			matrix.rotateY( prm->y );
			matrix.rotateZ( prm->z );
			node->setRotation( matrix );
		}
		break;
	case MOC_ANGY:
		if ( node ) {
			Matrix matrix;
			matrix.identity();
			matrix.rotateY( prm->y );
			matrix.rotateX( prm->x );
			matrix.rotateZ( prm->z );
			node->setRotation( matrix );
		}
		break;
	case MOC_ANGZ:
		if ( node ) {
			Matrix matrix;
			matrix.identity();
			matrix.rotateZ( prm->z );
			matrix.rotateY( prm->y );
			matrix.rotateX( prm->x );
			node->setRotation( matrix );
		}
		break;

	case MOC_COLOR:
		if ( obj ) {
			obj->_vec[GPOBJ_USERVEC_COLOR] = *prm;
			if ( obj->_light ) updateLightVector( obj, moc );
		}
		break;
	case MOC_WORK:
		if ( obj ) {
			obj->_vec[GPOBJ_USERVEC_WORK] = *prm;
		}
		break;
	case MOC_WORK2:
		if ( obj ) {
			obj->_vec[GPOBJ_USERVEC_WORK2] = *prm;
		}
		break;
	}
}


int gamehsp::setObjectVector( int objid, int moc, Vector4 *prm )
{
	gpobj *obj;
	int flag_id;
	flag_id = objid & GPOBJ_ID_FLAGBIT;
	if ( flag_id == 0 ) {
		obj = getObj( objid );
		if ( obj == NULL ) return -1;
		if ( obj->_spr ) {
			setSpriteVector( obj, moc, prm );
			return 0;
		}
		setNodeVector( obj, obj->_node, moc, prm );
		return 0;
	}

	switch( flag_id ) {
	case GPOBJ_ID_EXFLAG:
		break;
	default:
		return -1;
	}

	//	GPOBJ_ID_EXFLAGの場合
	switch( objid ) {
	case GPOBJ_ID_SCENE:
		setSceneVector( moc, prm );
		return 0;
	case GPOBJ_ID_CAMERA:
		obj = getObj( _defcamera );
		break;
	case GPOBJ_ID_LIGHT:
		obj = getObj( _deflight );
		break;
	default:
		return -1;
	}
	if ( obj == NULL ) return -1;
	setNodeVector( obj, obj->_node, moc, prm );
	return 0;
}


void gamehsp::getNodeVector( gpobj *obj, Node *node, int moc, Vector4 *prm )
{
	switch(moc) {
	case MOC_POS:
		if ( node ) {
			*(Vector3 *)prm = node->getTranslation();
			prm->w = 0.0f;
		}
		break;
	case MOC_QUAT:
		if ( node ) {
			Quaternion quat;
			quat = node->getRotation();
			prm->x = quat.x;
			prm->y = quat.y;
			prm->z = quat.z;
			prm->w = quat.w;
		}
		break;
	case MOC_SCALE:
		if ( node ) {
			*(Vector3 *)prm = node->getScale();
			prm->w = 1.0f;
		}
		break;
	case MOC_DIR:
		if ( obj ) {
			*prm = obj->_vec[GPOBJ_USERVEC_DIR];
		}
		break;
	case MOC_EFX:
		break;


	case MOC_ANGX:
		if (node) {
			Quaternion quat;
			double roll, pitch, yaw;
			quat = node->getRotation();
			QuaternionToEulerAngles(quat, roll, pitch, yaw );
			prm->x = roll;
			prm->y = pitch;
			prm->z = yaw;
			prm->w = 0.0f;
		}
		break;


	case MOC_COLOR:
		if ( obj ) {
			*prm = obj->_vec[GPOBJ_USERVEC_COLOR];
		}
		break;
	case MOC_WORK:
		if ( obj ) {
			*prm = obj->_vec[GPOBJ_USERVEC_WORK];
		}
		break;
	case MOC_WORK2:
		if ( obj ) {
			*prm = obj->_vec[GPOBJ_USERVEC_WORK2];
		}
		break;
	}
}


void gamehsp::getSpriteVector( gpobj *obj, int moc, Vector4 *prm )
{
	gpspr *spr;
	spr = obj->_spr;
	if ( spr == NULL ) return;

	switch(moc) {
	case MOC_POS:
		prm->set( spr->_pos );
		break;
	case MOC_SCALE:
		prm->set( spr->_scale );
		break;
	case MOC_ANGX:
	case MOC_ANGY:
	case MOC_ANGZ:
		prm->set( spr->_ang );
		break;

	case MOC_DIR:
		*prm = obj->_vec[GPOBJ_USERVEC_DIR];
		break;
	case MOC_COLOR:
		*prm = obj->_vec[GPOBJ_USERVEC_COLOR];
		break;
	case MOC_WORK:
		*prm = obj->_vec[GPOBJ_USERVEC_WORK];
		break;
	case MOC_WORK2:
		*prm = obj->_vec[GPOBJ_USERVEC_WORK2];
		break;
	}
}


int gamehsp::getObjectVector( int objid, int moc, Vector4 *prm )
{
	int flag_id;
	flag_id = objid & GPOBJ_ID_FLAGBIT;
	if ( flag_id == 0 ) {
		gpobj *obj;
		obj = getObj( objid );
		if ( obj == NULL ) return -1;
		if ( obj->_spr ) {
			getSpriteVector( obj, moc, prm );
			return 0;
		}
		getNodeVector( obj, obj->_node, moc, prm );
		return 0;
	}
	getNodeVector( NULL, getNode(objid), moc, prm );
	return 0;
}


void gamehsp::drawNode( Node *node )
{
	Drawable* drawable = node->getDrawable(); 
	if (drawable) {
		drawable->draw();
	}
}


bool gamehsp::drawNodeRecursive(Node *node, bool wireflag)
{
	Drawable* drawable = node->getDrawable();
	if (drawable) {
		drawable->draw(wireflag);
		_render_numpoly += drawable->_drawtotal;
	}

	Node *pnode = node->getFirstChild();
	while (1) {
		if (pnode == NULL) break;
		drawNodeRecursive(pnode, wireflag);
		pnode = pnode->getNextSibling();
	}

	return true;
}

int gamehsp::drawSceneObject(gpobj *camobj)
{
	//	すべてのオブジェクトを描画する
	//	(camobjにコリジョングループ設定がある場合は対象コリジョンのみ描画)
	//	(camobjがNULLか、対象コリジョンなしの場合はすべてのオブジェクトを描画)
	//
	int i,num;
	gpobj *obj = _gpobj;

	int target_group = 0;
	if (camobj) target_group = camobj->_rendergroup;

	num = 0;
	for (i = 0; i < _maxobj; i++) {
		if (obj->isVisible(_scenedraw_lateflag)) {
			if (target_group) {
				if ((target_group & obj->_rendergroup) == 0) continue;
			}
			Node *node = obj->_node;
			if (node) {
				bool clip = true;
				bool wireflag = false;
				int mode = obj->_mode;
				if (mode & GPOBJ_MODE_XFRONT) {
					node->setRotation(_qcam_billboard);
				}
				if (mode & GPOBJ_MODE_CLIP) {
					clip = node->getBoundingSphere().intersects(_cameraDefault->getFrustum());
				}
				if (mode & GPOBJ_MODE_WIRE) {			// ワイヤーフレーム描画時
					wireflag = true;
				}

				if (clip) {
					//	Alphaのモジュレート設定
					gameplay::MaterialParameter *prm_modalpha = obj->_prm_modalpha;
					if (prm_modalpha) { prm_modalpha->setValue(obj->getAlphaRate()); }
					drawNodeRecursive(obj->_node, wireflag);
					num++;
				}
			}
		}
		obj++;
	}

	return num;
}


void gamehsp::drawAll( int option )
{
	// すべてのノードを描画
	//
	_render_numobj = 0;
	_render_numpoly = 0;

	// ビルボード用の向きを作成
	Matrix m;
	Camera* camera = _scene->getActiveCamera();
	m = camera->getNode()->getMatrix();
	m.getRotation(&_qcam_billboard);
	gpobj *camobj = (gpobj *)camera->getNode()->getUserObject();

	//	gpobjの3Dシーン描画
	//
	if (option & GPDRAW_OPT_DRAWSCENE) {
		_scenedraw_lateflag = false;
		_render_numobj += drawSceneObject(camobj);
	}
	if (option & GPDRAW_OPT_DRAWSCENE_LATE) {
		_scenedraw_lateflag = true;
		_render_numobj += drawSceneObject(camobj);
	}

#if 0
	if ( option & GPDRAW_OPT_DRAWSCENE ) {
		_scenedraw_lateflag = false;
		_scene->visit(this, &gamehsp::drawScene);
	}
	if ( option & GPDRAW_OPT_DRAWSCENE_LATE ) {
		_scenedraw_lateflag = true;
		_scene->visit(this, &gamehsp::drawScene);
	}
#endif

	SetSysReq(SYSREQ_DRAWNUMOBJ, _render_numobj);
	SetSysReq(SYSREQ_DRAWNUMPOLY, _render_numpoly);
}


bool gamehsp::pickupNode(Node *node, int deep)
{
	Drawable* drawable = node->getDrawable();
	Model* model = dynamic_cast<Model*>(drawable);
	if (model)
	{
		unsigned int partCount = model->getMeshPartCount();
		GP_WARN( "#model %s (deep:%d)(part:%d)",node->getId(), deep, partCount);

		Material *mat = model->getMaterial(0);
		if (mat)
		{
			Technique* technique = mat->getTechnique();
			unsigned int passCount = technique->getPassCount();
			for (unsigned int i = 0; i < passCount; ++i)
			{
				Pass* pass = technique->getPassByIndex(i);
				GP_WARN("#Pass%d ", i);
			}
		}

		MeshSkin* skin = model->getSkin();
		if (skin) {
			Joint *joint = skin->getRootJoint();
			Node* jointParent = joint->getParent();
			GP_WARN("#Skin Root:%s", joint->getId());
		}
	}

	if (model) {
		MeshSkin *skin = model->getSkin();
		if (skin) {
			Node *root = skin->getRootNode();
			if (root) {
				pickupNode(root, deep + 1);
			}
		}
	}

	Node *pnode = node->getFirstChild();
	while (1) {
		if (pnode == NULL) break;
		pickupNode(pnode, deep+1);
		pnode = pnode->getNextSibling();
	}

	return true;
}

void gamehsp::pickupAll(int option)
{
	// すべてのノードを検証
	//
	Node *node = _scene->getFirstNode();
	while (1) {
		if (node == NULL) break;
		pickupNode(node, 0);
		node = node->getNextSibling();
	}
}

#if 0
bool gamehsp::updateNodeMaterial( Node* node, Material *material )
{
	//	再帰的にノードのマテリアルを設定
	//
	Node *sub_node;
	sub_node = node->getFirstChild();
	if ( sub_node ) {
		updateNodeMaterial( sub_node, material );
	}
	sub_node = node->getNextSibling();
	if ( sub_node ) {
		updateNodeMaterial( sub_node, material );
	}

	Drawable* drawable = node->getDrawable();
	Model* model = dynamic_cast<Model*>(drawable);
    if (model)
    {
		model->setMaterial( material );
    }
	return true;
}
#endif

#if 0
bool gamehsp::drawScene(Node* node)
{
    // If the node visited contains a model, draw it
	gpobj *obj = (gpobj *)node->getUserObject();
	Drawable* drawable = node->getDrawable();
	Model* model = dynamic_cast<Model*>(drawable);
	if ( obj ) {
		if ( obj->isVisible( _scenedraw_lateflag ) == false ) return false;

		int mode = obj->_mode;
		if ( mode & GPOBJ_MODE_XFRONT ) {
			node->setRotation(_qcam_billboard);
		}
		if ( mode & GPOBJ_MODE_CLIP ) {
			if (node->getBoundingSphere().intersects(_cameraDefault->getFrustum()) == false ) return false;
		}

		//	Alphaのモジュレート設定
		gameplay::MaterialParameter *prm_modalpha = obj->_prm_modalpha;
		if ( prm_modalpha ) { prm_modalpha->setValue( obj->getAlphaRate() ); }

		if ( model ) {

			if ( mode & GPOBJ_MODE_WIRE ) {			// ワイヤーフレーム描画時
				model->draw(true);
				return true;
			}

		}
	}

    if (model)
    {
        model->draw();
    }
    return true;
}
#endif

int *gamehsp::getObjectPrmPtr( int objid, int prmid )
{
	int id;
	int *base_i;
	gpobj *obj;
	obj = getObj( objid );
	if ( obj == NULL ) return NULL;
	if ( prmid < 0 ) return NULL;
	if ( prmid & 0x100 ) {
		gpspr *spr;
		spr = obj->_spr;
		if ( spr == NULL ) return NULL;
		id = prmid & 0xff;
		if ( id >= (sizeof(gpspr)/sizeof(int)) ) return NULL;
		base_i = (int *)spr;
		base_i += id;
	} else {
		base_i = (int *)obj;
		if ( prmid >= (sizeof(gpobj)/sizeof(int)) ) return NULL;
		base_i += prmid;
	}
	return base_i;
}


int gamehsp::getObjectPrm( int objid, int prmid, int *outptr )
{
	int *base_i;
	base_i = getObjectPrmPtr( objid, prmid );
	if ( base_i == NULL ) return -1;
	*outptr = *base_i;
	return 0;
}


int gamehsp::setObjectPrm( int objid, int prmid, int value )
{
	int *base_i;
	base_i = getObjectPrmPtr( objid, prmid );
	if ( base_i == NULL ) return -1;
	*base_i = value;
	return 0;
}

char *gamehsp::getAnimId(int objid, int index, int option)
{
	gpobj *obj;
	Animation *anim;
	AnimationClip *clip;
	obj = getObj(objid);
	if (obj == NULL) return NULL;
	anim = obj->_animation;
	if (anim == NULL) return NULL;
	int max = anim->getClipCount();
	if ((index<0)||(index>=max)) return NULL;
	clip = anim->getClip(index);
	return (char *)clip->getId();
}

int gamehsp::getAnimPrm(int objid, int index, int option, int *res)
{
	gpobj *obj;
	Animation *anim;
	AnimationClip *clip;
	int p_res = 0;
	obj = getObj(objid);
	if (obj == NULL) return -1;
	anim = obj->_animation;
	if (anim == NULL) return -1;
	int max = anim->getClipCount();
	if ((index<0) || (index >= max)) return -1;
	clip = anim->getClip(index);
	switch (option) {
	case GPANIM_OPT_START_FRAME:
		p_res = (int)clip->getStartTime();
		break;
	case GPANIM_OPT_END_FRAME:
		p_res = (int)clip->getEndTime();
		break;
	case GPANIM_OPT_DURATION:
		p_res = (int)clip->getDuration();
		break;
	case GPANIM_OPT_ELAPSED:
		p_res = (int)clip->getElapsedTime();
		break;
	case GPANIM_OPT_PLAYING:
		if (clip->isPlaying()) { p_res = 1; }
		else { p_res = 0; }
		break;
	case GPANIM_OPT_BLEND:
		p_res = (int)( clip->getBlendWeight() * 100.0f );
		break;
	case GPANIM_OPT_SPEED:
		p_res = (int)( clip->getSpeed() * 100.0f );
		break;
	default:
		return -1;
	}
	*res = p_res;
	return 0;
}

int gamehsp::setAnimPrm(int objid, int index, int option, int value)
{
	gpobj *obj;
	Animation *anim;
	AnimationClip *clip;
	obj = getObj(objid);
	if (obj == NULL) return -1;
	anim = obj->_animation;
	if (anim == NULL) return -1;
	int max = anim->getClipCount();
	if ((index<0) || (index >= max)) return -1;
	clip = anim->getClip(index);
	switch (option) {
	case GPANIM_OPT_PLAYING:
		if (value == 0) clip->stop();
		if (value == 1) clip->play();
		if (value == 2) clip->pause();
		break;
	case GPANIM_OPT_DURATION:
		clip->setActiveDuration( (unsigned long)value );
		break;
	case GPANIM_OPT_BLEND:
	{
		float weight = (float)value;
		weight = weight * 0.01f;
		clip->setBlendWeight(weight);
		break;
	}
	case GPANIM_OPT_SPEED:
	{
		float weight = (float)value;
		weight = weight * 0.01f;
		clip->setSpeed(weight);
		break;
	}
	default:
		return -1;
	}
	return 0;
}

int gamehsp::addAnimId(int objid, char *name, int start, int end, int option)
{
	gpobj *obj;
	Animation *anim;
	AnimationClip *clip;
	unsigned long p_end;
	unsigned long p_start;
	obj = getObj(objid);
	if (obj == NULL) return -1;
	anim = obj->_animation;
	if (anim == NULL) {
		GP_WARN( "Not Found Animation in OBJ#%d",objid );
		return -1;
	}
	if (*name == 0) {
		GP_WARN("No Animation name");
		return -1;
	}
	clip = anim->getClip(name);
	if (clip != NULL) {
		GP_WARN("Already exist Animation [%s] in OBJ#%d", name, objid);
		return -1;
	}

	p_start = (unsigned long)start;
	if (end < 0) {
		p_end = anim->getDuration();
	}
	else {
		p_end = (unsigned long)end;
	}
	clip = anim->createClip(name, p_start, p_end);
	clip->setRepeatCount(AnimationClip::REPEAT_INDEFINITE);
	return 0;
}

int gamehsp::playAnimId(int objid, char *name, int option)
{
	gpobj *obj;
	Animation *anim;
	AnimationClip *clip;
	obj = getObj(objid);
	if (obj == NULL) return -1;
	anim = obj->_animation;
	if (anim == NULL) return -1;
	if (*name == 0) {
		clip = anim->getClip(DEFAULT_ANIM_CLIPNAME);
	}
	else {
		clip = anim->getClip(name);
	}
	if (clip == NULL) return -1;
	if (option == 0) clip->stop();
	if (option == 1) clip->play();
	if (option == 2) clip->pause();

	return 0;
}


/*------------------------------------------------------------*/
/*
		Primitive process
*/
/*------------------------------------------------------------*/

int gamehsp::makeNullNode( void )
{
	Node *node;
	gpobj *obj = addObj();
	if ( obj == NULL ) return -1;

	node = Node::create();
	obj->_node = node;
	node->setUserObject( obj );
	obj->addRef();

	if ( _curscene >= 0 ) {
		_scene->addNode( obj->_node );
	}

	return obj->_id;
}


int gamehsp::makeSpriteObj(int celid, int gmode, void *bmscr)
{
	gpobj *obj = addObj();
	if ( obj == NULL ) return -1;

	obj->_spr = new gpspr;
	obj->_spr->reset( obj->_id, celid, gmode, bmscr );
	obj->_mode |= GPOBJ_MODE_2D;

	return obj->_id;
}


int gamehsp::makeFloorNode( float xsize, float ysize, int color, int matid )
{
	gpobj *obj = addObj();
	if ( obj == NULL ) return -1;

    // 平面作成
	Mesh* floorMesh = Mesh::createQuad(
		Vector3( -xsize * 0.5f , 0, -ysize * 0.5f ), Vector3( -xsize * 0.5f , 0, ysize * 0.5f ), 
		Vector3( xsize * 0.5f ,  0, -ysize * 0.5f ), Vector3( xsize * 0.5f , 0, ysize * 0.5f ));

	//Mesh* floorMesh = createFloorMesh( xsize, ysize );

	Material *material;
	if ( matid < 0 ) {
		int matopt = 0;
		if ( _curlight < 0 ) matopt |= GPOBJ_MATOPT_NOLIGHT;
		material = makeMaterialColor( color, matopt );
		makeNewModel(obj, floorMesh, material);
	}
	else {
		material = getMaterial( matid );
		if (material == NULL) {
			material = makeMaterialColor(-1, GPOBJ_MATOPT_NOLIGHT);
			makeNewModel(obj, floorMesh, material);
		}
		else {
			makeNewModelWithMat(obj, floorMesh, matid);
		}
	}

    // メッシュ削除
    SAFE_RELEASE(floorMesh);

	// 初期化パラメーターを保存
	obj->_shape = GPOBJ_SHAPE_FLOOR;
	obj->_sizevec.set( xsize, 0, ysize );

/*
	// 物理設定
	PhysicsRigidBody::Parameters rigParams;
	rigParams.mass = 0.0f;	// 重さ
    rigParams.friction = 0.5;
    rigParams.restitution = 0.75;
    rigParams.linearDamping = 0.025;
    rigParams.angularDamping = 0.16;
	obj->_node->setCollisionObject(PhysicsCollisionObject::RIGID_BODY, 
		PhysicsCollisionShape::box(Vector3(xsize * 2, 0, ysize * 2)), &rigParams);
*/

	if ( _curscene >= 0 ) {
		_scene->addNode( obj->_node );
	}

	return obj->_id;
}


int gamehsp::makePlateNode( float xsize, float ysize, int color, int matid )
{
	gpobj *obj = addObj();
	if (obj == NULL) {
		return -1;
	}

    // 平面作成
	Mesh* floorMesh = Mesh::createQuad(
		Vector3( -xsize * 0.5f , ysize * 0.5f, 0 ), Vector3( -xsize * 0.5f , -ysize * 0.5f, 0 ), 
		Vector3( xsize * 0.5f ,  ysize * 0.5f, 0 ), Vector3( xsize * 0.5f , -ysize * 0.5f, 0 ));

	//Mesh* floorMesh = createFloorMesh( xsize, ysize );

	Material *material;
	if ( matid < 0 ) {
		int matopt = 0;
		if ( _curlight < 0 ) matopt |= GPOBJ_MATOPT_NOLIGHT;
		material = makeMaterialColor( color, matopt );
		makeNewModel(obj, floorMesh, material);
	}
	else {
		material = getMaterial( matid );
		if (material) {
			makeNewModelWithMat(obj, floorMesh, matid);
		}
		else {
			material = makeMaterialColor(-1, GPOBJ_MATOPT_NOLIGHT);
			makeNewModel(obj, floorMesh, material);
		}
	}

    // メッシュ削除
    SAFE_RELEASE(floorMesh);

	// 初期化パラメーターを保存
	obj->_shape = GPOBJ_SHAPE_PLATE;
	obj->_sizevec.set( xsize, ysize, 0 );

	if ( _curscene >= 0 ) {
		_scene->addNode( obj->_node );
	}

	return obj->_id;
}


int gamehsp::makeBoxNode( float size, int color, int matid )
{
	Mesh *mesh;
	gpobj *obj = addObj();
	if ( obj == NULL ) return -1;

	mesh = createCubeMesh( size );

	Material *material;
	if ( matid < 0 ) {
		int matopt = 0;
		if ( _curlight < 0 ) matopt |= GPOBJ_MATOPT_NOLIGHT;
		material = makeMaterialColor( color, matopt );
		makeNewModel(obj, mesh, material);
	}
	else {
		material = getMaterial(matid);
		if (material) {
			makeNewModelWithMat(obj, mesh, matid);
		}
		else {
			material = makeMaterialColor(-1, GPOBJ_MATOPT_NOLIGHT);
			makeNewModel(obj, mesh, material);
		}
	}

	// 初期化パラメーターを保存
	obj->_shape = GPOBJ_SHAPE_BOX;
	obj->_sizevec.set( size, size, size );

    // メッシュ削除
    SAFE_RELEASE(mesh);

/*
	// 物理設定
	PhysicsRigidBody::Parameters rigParams;
	rigParams.mass = 1.0f;	// 重さ
	rigParams.friction = 0.5;
	rigParams.restitution = 0.5;
	rigParams.linearDamping = 0.1;
	rigParams.angularDamping = 0.5;
	obj->_node->setCollisionObject(PhysicsCollisionObject::RIGID_BODY, 
		PhysicsCollisionShape::box(Vector3(1,1,1)), &rigParams);
*/

	if ( _curscene >= 0 ) {
		_scene->addNode( obj->_node );
	}

	BoundingSphere bound = obj->_node->getBoundingSphere();

	return obj->_id;
}


bool gamehsp::makeModelNodeSub(Node *rootnode, int nest)
{
	Node *node = rootnode;
	Material *mat;
	Vector3 *vambient;
	int part, tecs, prms;

	gpobj *lgt;
	Node *light_node;
	lgt = getObj(_curlight);
	if (lgt == NULL) node = NULL;
	light_node = lgt->_node;
	if (light_node == NULL) node = NULL;
	vambient = (Vector3 *)&lgt->_vec[GPOBJ_USERVEC_DIR];

	while (node != NULL) {
		part = 0;
		mat = NULL;
		Drawable* drawable = node->getDrawable();
		Model* model = dynamic_cast<Model*>(drawable);
		if (model){
			Technique *tec = NULL;
			mat = model->getMaterial(0);
			part = model->getMeshPartCount();
			tecs = 0; prms = 0;
			if (part) {
				for (int i = 0; i < part; i++){
					mat = model->getMaterial(i);
					if (mat) {
						//tec = mat->getTechniqueByIndex(0);
						//mat->getParameter("u_directionalLightColor[0]")->setValue(Vector3(1, 1, 0));
						if (hasParameter(mat, "u_directionalLightColor[0]")) {
							mat->getParameter("u_directionalLightColor[0]")->bindValue(light_node->getLight(), &Light::getColor);
						}
						//mat->getParameter("u_directionalLightDirection[0]")->setValue(&directionalLightVector);
						if (hasParameter(mat, "u_directionalLightDirection[0]")) {
							mat->getParameter("u_directionalLightDirection[0]")->bindValue(light_node, &Node::getForwardVectorView);
						}
						if (hasParameter(mat, "u_ambientColor")) {
							mat->getParameter("u_ambientColor")->setValue(vambient);
						}
					}
				}
			}
		}
		node = node->getNextSibling();
	}

	node = rootnode->getFirstChild();
	if (node != NULL) {
		makeModelNodeSub(node, nest + 1);
	}
	return true;
}


std::string gamehsp::passCallback(Pass* pass, void* cookie)
{
	return model_defines;
}


int gamehsp::makeModelNode(char *fname, char *idname, char *defs)
{
	char fn[512];
	char fn2[512];
	gpobj *obj = addObj();
	if (obj == NULL) return -1;

	getpath(fname, fn, 1);
	strcpy(fn2, fn);
	strcat(fn, ".gpb");
	strcat(fn2, ".material");

	Bundle *bundle = Bundle::create(fn);
	Node *rootNode;
	Animation *animation;
	Node *node;

	model_defines = defs;
	Material* boxMaterial = Material::create(fn2,gamehsp::passCallback,NULL);
	if (boxMaterial == NULL) return -1;

	animation = NULL;
	if (idname) {
		rootNode = bundle->loadNode(idname);
		if (rootNode == NULL) {
			Alertf("Node not found.(%s#%s)", fname, idname);
			return -1;
		}
	}
	else {
		Scene *scene;
		char *rootid;
		rootNode = Node::create("objRoot");
		rootid = NULL;
		scene = bundle->loadScene();
		if (scene == NULL) {
			Alertf("Scene not found.(%s)", fname);
			return -1;
		}

		size_t i, max;
		std::vector<Node*> nodes;
		node = scene->getFirstNode();
		while (1) {
			if (node == NULL) break;
			nodes.push_back(node);
			node = node->getNextSibling();
		}
		max = nodes.size();
		for (i = 0; i < max; i++) {
			node = nodes[i];
			rootNode->addChild(node);
			//GP_WARN("#ADD[%s]", node->getId());
		}
		max = bundle->_savedNode.size();
		for (i = 0; i < max; i++) {
			node = bundle->_savedNode[i];
			//GP_WARN("#SAVE[%s]", node->getId());
			Node *subnode = Node::create("Subobj");
			subnode->setDrawable(node->getDrawable());
			subnode->setId(node->getId());
			subnode->setRefNode(node);
			rootNode->addChild(subnode);
		}

		animation = rootNode->getAnimation("animations");
		if (animation) {
			AnimationClip *aclip;
			aclip = animation->createClip(DEFAULT_ANIM_CLIPNAME, 0, animation->getDuration());
			aclip->setRepeatCount(AnimationClip::REPEAT_INDEFINITE);
			//animation->play(aclip->getId());
		}

		SAFE_RELEASE(scene);
	}

	//obj->updateParameter(boxMaterial);
	//updateNodeMaterial(rootNode, boxMaterial);
	makeModelNodeSub(rootNode, 0);

	if (_curscene >= 0) {
		_scene->addNode(rootNode);
	}

	SAFE_RELEASE(bundle);
	SAFE_RELEASE(boxMaterial);

	rootNode->setUserObject(obj);
	obj->addRef();
	obj->_node = rootNode;
	obj->_animation = animation;

	// 初期化パラメーターを保存
	obj->_shape = GPOBJ_SHAPE_MODEL;

	return obj->_id;
}


void gamehsp::makeNewModel( gpobj *obj, Mesh *mesh, Material *material )
{
	//	gpobjに新規モデルを追加してメッシュとマテリアルを設定する
	//
	Model *model;
	model = Model::create(mesh);	// モデル作成
	model->setMaterial( material );

	Node *node = obj->_node;
	if ( node == NULL ) {
		node = Node::create();
		obj->_node = node;
	}

	node->setDrawable(model);
	node->setUserObject( obj );
	obj->addRef();
	obj->_model = model;
	obj->updateParameter( material );
	SAFE_RELEASE(model);
}


int gamehsp::makeNewModelWithMat( gpobj *obj, Mesh *mesh, int matid )
{
	//	gpobjに新規モデルを追加してメッシュとマテリアルを設定する
	//  (マテリアルIDを使用)
	//
	gpmat *mat = getMat( matid );
	if ( mat == NULL ) return -1;

	Material *new_material;
	NodeCloneContext context;
	//new_material = mat->_material;
	new_material = mat->_material->clone(context);		// 元のマテリアルをクローンして適用する
	setMaterialDefaultBinding(new_material,mat->_matcolor,mat->_matopt);		// 正しくクローンされないBinding情報を上書きする

	//Alertf("[%x]===",new_material);
	makeNewModel(obj, mesh, new_material);

#if 0
	Technique *tec = new_material->getTechnique();
	Pass *pass = tec->getPassByIndex(0);
	Effect *effect = pass->getEffect();

	//new_material = Material::create( effect );
	for (unsigned int i = 0, count = tec->getParameterCount(); i < count; ++i)
	{
		MaterialParameter *mp = tec->getParameterByIndex(i);
		Alertf( "%d: [%s]",i, mp->getName() );
	}

	// Search for the first sampler uniform in the effect.
	Uniform* samplerUniform = NULL;
	for (unsigned int i = 0, count = effect->getUniformCount(); i < count; ++i)
	{
		Uniform* uniform = effect->getUniform(i);
		if (uniform && uniform->getType() == GL_SAMPLER_2D)
		{
			samplerUniform = uniform;
			Alertf("[%s] passcount%d [%s][%x]", samplerUniform->getName(), tec->getPassCount(), pass->getId(), new_material);
			break;
		}
	}
#endif

	obj->_usegpmat = matid;
	return 0;
}


/*------------------------------------------------------------*/
/*
		Node process
*/
/*------------------------------------------------------------*/

gpobj *gamehsp::getObj( int id )
{
	if (( id < 0 )||( id >= _maxobj )) return NULL;
	if ( _gpobj[id]._flag == GPOBJ_FLAG_NONE ) return NULL;
	return &_gpobj[id];
}


gpobj *gamehsp::getSceneObj(int id)
{
	//	システム系も含めたシーンのオブジェクトを取得する
	//
	int flag_id;
	flag_id = id & GPOBJ_ID_FLAGBIT;
	if (flag_id == 0) {
		if ((id < 0) || (id >= _maxobj)) return NULL;
		if (_gpobj[id]._flag == GPOBJ_FLAG_NONE) return NULL;
		return &_gpobj[id];
	}

	switch (flag_id) {
	case GPOBJ_ID_EXFLAG:
		break;
	default:
		return NULL;
	}

	//	GPOBJ_ID_EXFLAGの場合
	gpobj *obj = NULL;
	switch (id) {
	case GPOBJ_ID_CAMERA:
		obj = getObj(_defcamera);
		break;
	case GPOBJ_ID_LIGHT:
		obj = getObj(_deflight);
		break;
	default:
		break;
	}
	return obj;
}


int gamehsp::deleteObj( int id )
{
	Model *model;
	Material *material;
	gpobj *obj = getObj( id );
	if ( obj == NULL ) return -1;

	obj->_flag = GPOBJ_FLAG_NONE;
	if ( obj->_spr ) {
		delete obj->_spr;
		obj->_spr = NULL;
	}
	if ( obj->_phy ) {
		delete obj->_phy;
		obj->_phy = NULL;
	}
	model = obj->_model;

	if ( model ) {
			material = model->getMaterial();
			SAFE_RELEASE(material);		// マテリアルはモデルに個別で用意されるので削除
	}

	if (obj->_node) {
		obj->_node->setUserObject(NULL);
		if (_curscene >= 0) {
			_scene->removeNode( obj->_node);
		}
		obj->_node->removeAllChildren();
		obj->_node->setLight(NULL);
		obj->_node->setCamera(NULL);

		SAFE_RELEASE(obj->_node);
		//unsigned int cnt = obj->_node->getRefCount();
		//Alertf( "count[%d]",cnt );
	}

	SAFE_RELEASE( obj->_camera );
    SAFE_RELEASE( obj->_light );

	return 0;
}


int gamehsp::setObjectPool( int startid, int num )
{
	int max;
	max = num;
	if ( max < 0 ) {
		max = _maxobj - startid;
	}
	if ( startid < 0 ) return -1;
	if ( startid >= _maxobj ) return -1;
	if ( ( startid + max ) > _maxobj ) return -1;
	_objpool_startid = startid;
	_objpool_max = startid + max;
	return 0;
}


gpobj *gamehsp::addObj( void )
{
	//	空のgpobjを生成する
	//
	int i;
	gpobj *obj = _gpobj;
	obj += _objpool_startid;
	for( i=_objpool_startid; i<_objpool_max; i++ ) {
		if ( obj->_flag == GPOBJ_FLAG_NONE ) {
			obj->reset(i);
			return obj;
		}
		obj++;
	}
	return NULL;
}


Node *gamehsp::getNode( int objid )
{
	//	指定されたIDの持つノードを返す
	//	(gpobj,gplgt,cameraで使用可能)
	//
	int flag_id;
	gpobj *obj;
	flag_id = objid & GPOBJ_ID_FLAGBIT;
	switch( flag_id ) {
	case 0:
		obj = getObj( objid );
		if ( obj == NULL ) return NULL;
		return obj->_node;
	case GPOBJ_ID_EXFLAG:
		break;
	default:
		return NULL;
	}

	//	GPOBJ_ID_EXFLAGの場合
	switch( objid ) {
//	case GPOBJ_ID_SCENE:
	case GPOBJ_ID_CAMERA:
		return _cameraDefault->getNode();
	case GPOBJ_ID_LIGHT:
		obj = getObj( _deflight );
		if ( obj == NULL ) return NULL;
		return obj->_node;
	default:
		break;
	}
	return NULL;
}


int gamehsp::makeCloneNode( int objid, int mode, int eventID )
{
	gpobj *obj;
	Node *node;
	int id;
	obj = getObj( objid );
	if ( obj == NULL ) return -1;

	if ( obj->_spr ) {
		// 2Dスプライトの場合
		id = makeSpriteObj( obj->_spr->_celid, obj->_spr->_gmode, obj->_spr->_bmscr );
	}
	else {
		gpobj *newobj = addObj();
		if (newobj == NULL) return -1;

		node = obj->_node;
		if (node == NULL) {
			return newobj->_id;
		}

		newobj->_mode = obj->_mode;
		newobj->_usegpmat = obj->_usegpmat;
		newobj->_shape = obj->_shape;
		newobj->_sizevec = obj->_sizevec;

		node->setUserObject(NULL);

		newobj->_node = node->clone();
		newobj->_animation = newobj->_node->getAnimation("animations");

		newobj->_node->setUserObject(newobj);
		newobj->addRef();

		makeModelNodeSub(newobj->_node, 0);

		if (_curscene >= 0) {
			_scene->addNode(newobj->_node);
		}

		node->setUserObject(obj);
		id = newobj->_id;
	}

	obj = getObj(id);
	if (obj == NULL) return -1;
	obj->_mode |= mode;
	if (eventID >= 0) {
		AttachEvent(id,eventID,-1);
	}
	return id;
}


/*------------------------------------------------------------*/
/*
		gpobj update
*/
/*------------------------------------------------------------*/

int gamehsp::updateObjBorder( int mode, Vector3 *pos, Vector4 *dir )
{
	//		自動範囲クリップ
	//
	int cflag,thru;
	cflag = 0;
	thru = GetSysReq( SYSREQ_THROUGHFLAG );
		if ( thru & 1 ) {
			if ( pos->x < border1.x ) {
				pos->x = border2.x - ( border1.x - pos->x );
			}
			if ( pos->x > border2.x ) {
				pos->x = border1.x + ( pos->x - border2.x );
			}
		}
		if ( thru & 2 ) {
			if ( pos->y < border1.y ) {
				pos->y = border2.y - ( border1.y - pos->y );
			}
			if ( pos->y > border2.y ) {
				pos->y = border1.y + ( pos->y - border2.y );
			}
		}
		if ( thru & 4 ) {
			if ( pos->z < border1.z ) {
				pos->z = border2.z - ( border1.z - pos->z );
			}
			if ( pos->z > border2.z ) {
				pos->z = border1.z + ( pos->z - border2.z );
			}
		}
		if ( mode & GPOBJ_MODE_FLIP ) {
			if ( pos->x < border1.x ) {
				pos->x = border1.x;
				dir->x *= -1.0f;
			}
			if ( pos->x > border2.x ) {
				pos->x = border2.x;
				dir->x *= -1.0f;
			}
			if ( pos->y < border1.y ) {
				pos->y = border1.y;
				dir->y *= -1.0f;
			}
			if ( pos->y > border2.y ) {
				pos->y = border2.y;
				dir->y *= -1.0f;
			}
			if ( pos->z < border1.z ) {
				pos->z = border1.z;
				dir->z *= -1.0f;
			}
			if ( pos->z > border2.z ) {
				pos->z = border2.z;
				dir->z *= -1.0f;
			}
		}
		else {
			if ( pos->x < border1.x ) {
				cflag++;
			}
			if ( pos->x > border2.x ) {
				cflag++;
			}
			if ( pos->y < border1.y ) {
				cflag++;
			}
			if ( pos->y > border2.y ) {
				cflag++;
			}
			if ( pos->z < border1.z ) {
				cflag++;
			}
			if ( pos->z > border2.z ) {
				cflag++;
			}
			if ( cflag ) {
				return -1;
			}
		}

	return 0;
}


void gamehsp::updateObj( gpobj *obj )
{
	//		gpobjの更新
	//
	int mode = obj->_mode;

	if (obj->isVisible() == false) return;

	if (obj->executeFade()) {
		deleteObj(obj->_id);
		return;
	}

	if ( mode & ( GPOBJ_MODE_MOVE|GPOBJ_MODE_BORDER) ) {
		int cflag;
		Vector3 pos;
		Vector4 *dir;
		if ( obj->_spr ) {
			pos = *(Vector3 *)&obj->_spr->_pos;
		} else {
			pos = obj->_node->getTranslation();
		}
		dir = &obj->_vec[GPOBJ_USERVEC_DIR];
		if ( mode & GPOBJ_MODE_MOVE ) {
			pos.add( *(Vector3 *)dir );
		}
		if ( mode & GPOBJ_MODE_BORDER ) {
			cflag = updateObjBorder( mode, &pos, dir );
			if ( cflag ) {												// 消去フラグ
				if (mode & GPOBJ_MODE_BHIDE) {
					obj->_mode |= GPOBJ_MODE_HIDE;
				} else {
					deleteObj(obj->_id);
				}
				return;
			}
		}
		if ( obj->_spr ) {
			obj->_spr->_pos.set( pos.x, pos.y, pos.z, 1.0f );
		} else {
			obj->_node->setTranslation(pos);
		}
	}

}


void gamehsp::updateAll( void )
{
	/*
		All update of gpobj
	*/
	int i,j;
	float timepass = 1.0f;

    if ( getState() == PAUSED ) return;

	gpobj *obj = _gpobj;
	for(i=0;i<_maxobj;i++) {
		if (obj->isVisible()) {
			// Execute Events
			for (j = 0; j<GPOBJ_MULTIEVENT_MAX; j++) {
				if (obj->GetEvent(j) != NULL) ExecuteObjEvent(obj, timepass, j);
			}
		}
		if (obj->isVisible()) {
			updateObj(obj);
		}
		obj++;
	}
}


int gamehsp::updateObjColi( int objid, float size, int addcol )
{
	int i;
	int chkgroup;
	gpobj *obj;
	gpobj *atobj;
	gpspr *spr;
	Vector3 *pos;

	obj = getSceneObj( objid );
	if ( obj == NULL ) return -1;

	if ( addcol == 0 ) {
		chkgroup = obj->_colgroup;
	} else {
		chkgroup = addcol;
	}

	spr = obj->_spr;
	if ( spr ) {									// 2Dスプライト時の処理
		gpspr *atspr;
		pos = (Vector3 *)&spr->_pos;
		atobj = _gpobj;
		for(i=0;i<_maxobj;i++) {
			if ( atobj->isVisible() ) {
				if (( atobj->_mygroup & chkgroup )&&( i != objid )) {
					atspr = atobj->_spr;
					if ( atspr ) {
						if ( atspr->getDistanceHit( pos, size ) ) {
							return i;
						}
					}
				}
			}
			atobj++;
		}
		return -1;
	}

	Vector3 vpos;
	Vector3 enepos;
	Node *node;
	BoundingSphere bound;
	if ( obj->_node == NULL ) return -1;

	vpos = obj->_node->getTranslation();
	atobj = _gpobj;

	if (size < 0.0f) {
		bound = obj->_node->getBoundingSphere();
		bound.radius *= size;							// 自分のサイズを調整する

		for (i = 0; i<_maxobj; i++) {
			if (atobj->isVisible()) {
				if ((atobj->_mygroup & chkgroup) && (i != objid)) {
					node = atobj->_node;
					if (node) {
						if (bound.intersects(node->getBoundingSphere())) {
							return i;
						}
					}
				}
			}
			atobj++;
		}
		return -1;
	}

	for (i = 0; i<_maxobj; i++) {
		if (atobj->isVisible()) {
			if ((atobj->_mygroup & chkgroup) && (i != objid)) {
				node = atobj->_node;
				if (node) {
					enepos = node->getTranslation();
					enepos -= vpos;
					if ( enepos.length() <= size ) {
						return i;
					}
				}
			}
		}
		atobj++;
	}

	return -1;
}


/*------------------------------------------------------------*/
/*
		gpspr Obj
*/
/*------------------------------------------------------------*/

gpspr::gpspr()
{
	_id = -1;
}

gpspr::~gpspr()
{
}

void gpspr::reset( int id, int celid, int gmode, void *bmscr )
{
	_id = id;
	_celid = celid;
	_gmode = gmode;
	_bmscr = bmscr;
	_pos.set( Vector4::zero() );
	_ang.set( Vector4::zero() );
	_scale.set( 1.0f, 1.0f, 1.0f, 1.0f );
}


int gpspr::getDistanceHit( Vector3 *v, float size )
{
	float sz;
	sz = size * 1.0f;//colscale[0];
	if ( fabs( _pos.x - v->x ) < sz ) {
		sz = size * 1.0f;//colscale[1]
		if ( fabs( _pos.y - v->y ) < sz ) {
			return 1;
		}
	}
	return 0;
}




gpspr *gamehsp::getSpriteObj( int objid )
{
	//	スプライト情報を返す
	//
	gpobj *obj = getObj( objid );
	if ( obj == NULL ) return NULL;
	return obj->_spr;
}


void gamehsp::findSpriteObj( bool lateflag )
{
	_find_count = 0;
	_find_gpobj = _gpobj;
	_find_lateflag = lateflag;
}


gpobj *gamehsp::getNextSpriteObj( void )
{
	gpobj *res;
	while(1) {
		if ( _find_count >= _maxobj ) { return NULL; }
		if ( _find_gpobj->_flag ) {
			if ( _find_gpobj->_spr ) {
				if ( _find_gpobj->isVisible( _find_lateflag ) ) {
					res = _find_gpobj;
					break;
				}
			}
		}
		_find_count++;
		_find_gpobj++;
	}
	_find_count++;
	_find_gpobj++;
	return res;
}


void gamehsp::findeObj( int exmode, int group )
{
	_find_count = 0;
	_find_gpobj = _gpobj;
	_find_exmode = exmode;
	_find_group = group;
}


gpobj *gamehsp::getNextObj( void )
{
	gpobj *res;
	while(1) {
		if ( _find_count >= _maxobj ) { return NULL; }
		if ( _find_gpobj->_flag ) {
			if (_find_group == 0) {
				res = _find_gpobj;
				break;
			}
			if ( _find_gpobj->_mygroup & _find_group ) {
				if (( _find_gpobj->_mode & _find_exmode ) == 0 ) {
					res = _find_gpobj;
					break;
				}
			}
		}
		_find_count++;
		_find_gpobj++;
	}
	_find_count++;
	_find_gpobj++;
	return res;
}


/*------------------------------------------------------------*/
/*
		Utils
*/
/*------------------------------------------------------------*/

void gamehsp::colorVector3( int icolor, Vector4 &vec )
{
	vec.set( ( (icolor>>16)&0xff )*_colrate, ( (icolor>>8)&0xff )*_colrate, ( icolor&0xff )*_colrate, 1.0f );
}


void gamehsp::colorVector4( int icolor, Vector4 &vec )
{
	vec.set( ( (icolor>>16)&0xff )*_colrate, ( (icolor>>8)&0xff )*_colrate, ( icolor&0xff )*_colrate, ( (icolor>>24)&0xff )*_colrate );
}


Mesh *gamehsp::createCubeMesh( float size )
{
    float a = size * 0.5f;
    float vertices[] =
    {
        -a, -a,  a,    0.0,  0.0,  1.0,   0.0, 0.0,
         a, -a,  a,    0.0,  0.0,  1.0,   1.0, 0.0,
        -a,  a,  a,    0.0,  0.0,  1.0,   0.0, 1.0,
         a,  a,  a,    0.0,  0.0,  1.0,   1.0, 1.0,
        -a,  a,  a,    0.0,  1.0,  0.0,   0.0, 0.0,
         a,  a,  a,    0.0,  1.0,  0.0,   1.0, 0.0,
        -a,  a, -a,    0.0,  1.0,  0.0,   0.0, 1.0,
         a,  a, -a,    0.0,  1.0,  0.0,   1.0, 1.0,
        -a,  a, -a,    0.0,  0.0, -1.0,   0.0, 0.0,
         a,  a, -a,    0.0,  0.0, -1.0,   1.0, 0.0,
        -a, -a, -a,    0.0,  0.0, -1.0,   0.0, 1.0,
         a, -a, -a,    0.0,  0.0, -1.0,   1.0, 1.0,
        -a, -a, -a,    0.0, -1.0,  0.0,   0.0, 0.0,
         a, -a, -a,    0.0, -1.0,  0.0,   1.0, 0.0,
        -a, -a,  a,    0.0, -1.0,  0.0,   0.0, 1.0,
         a, -a,  a,    0.0, -1.0,  0.0,   1.0, 1.0,
         a, -a,  a,    1.0,  0.0,  0.0,   0.0, 0.0,
         a, -a, -a,    1.0,  0.0,  0.0,   1.0, 0.0,
         a,  a,  a,    1.0,  0.0,  0.0,   0.0, 1.0,
         a,  a, -a,    1.0,  0.0,  0.0,   1.0, 1.0,
        -a, -a, -a,   -1.0,  0.0,  0.0,   0.0, 0.0,
        -a, -a,  a,   -1.0,  0.0,  0.0,   1.0, 0.0,
        -a,  a, -a,   -1.0,  0.0,  0.0,   0.0, 1.0,
        -a,  a,  a,   -1.0,  0.0,  0.0,   1.0, 1.0
    };
    short indices[] = 
    {
        0, 1, 2, 2, 1, 3, 4, 5, 6, 6, 5, 7, 8, 9, 10, 10, 9, 11, 12, 13, 14, 14, 13, 15, 16, 17, 18, 18, 17, 19, 20, 21, 22, 22, 21, 23
    };
    unsigned int vertexCount = 24;
    unsigned int indexCount = 36;
    VertexFormat::Element elements[] =
    {
        VertexFormat::Element(VertexFormat::POSITION, 3),
        VertexFormat::Element(VertexFormat::NORMAL, 3),
        VertexFormat::Element(VertexFormat::TEXCOORD0, 2)
    };
    Mesh* mesh = Mesh::createMesh(VertexFormat(elements, 3), vertexCount, false);
    if (mesh == NULL)
    {
        GP_ERROR("Failed to create mesh.");
        return NULL;
    }
    mesh->setVertexData(vertices, 0, vertexCount);
    MeshPart* meshPart = mesh->addPart(Mesh::TRIANGLES, Mesh::INDEX16, indexCount, false);
    meshPart->setIndexData(indices, 0, indexCount);
    return mesh;
}


float *gamehsp::startPolyColor2D( void )
{
    _meshBatch->start();
	return _bufPolyColor;
}


void gamehsp::drawPolyColor2D( void )
{
    static unsigned short indices[] = { 0, 1, 2, 3 };
	_meshBatch->add( _bufPolyColor, 4, indices, 4 );
    _meshBatch->finish();
    _meshBatch->draw();
}


void gamehsp::addPolyColor2D( int num )
{
    static unsigned short indices[] = { 0, 1, 2, 3 };
	_meshBatch->add( _bufPolyColor, num, indices, num );
}


void gamehsp::finishPolyColor2D( void )
{
    _meshBatch->finish();
    _meshBatch->draw();
}


void gamehsp::setPolyDiffuse2D( float r, float g, float b, float a )
{
	//	Vertexのカラーコードのみを設定する
	//
	int i;
	float *v = _bufPolyColor;
	for(i=0;i<4;i++) {
		v += 3;						// Posを飛ばす
		*v++ = r;
		*v++ = g;
		*v++ = b;
		*v++ = a;
	}
}


float *gamehsp::startLineColor2D( void )
{
    _meshBatch_line->start();
	return _bufPolyColor;
}


void gamehsp::drawLineColor2D( void )
{
    static unsigned short indices[] = { 0, 1, 2, 3 };
	_meshBatch_line->add( _bufPolyColor, 2, indices, 2 );
    _meshBatch_line->finish();
    _meshBatch_line->draw();
}


void gamehsp::addLineColor2D( int num )
{
    static unsigned short indices[] = { 0, 1, 2, 3 };
	_meshBatch_line->add( _bufPolyColor, num, indices, num );
}


void gamehsp::finishLineColor2D( void )
{
    _meshBatch_line->finish();
    _meshBatch_line->draw();
}


float *gamehsp::startPolyTex2D(gpmat *mat, int material_id )
{
	//	テクスチャポリゴン描画開始
	//		mat : コピー元のマテリアル
	//		material_id : 描画先のマテリアルID
	//
	MeshBatch *mesh = mat->_mesh;
	if ( mesh == NULL ) {
        GP_ERROR("Bad Material.");
        return NULL;
	}

	gpmat *targetmat = getMat(material_id);
	if (targetmat) {
		if (mat->_target_material_id != material_id) {
			//	同一マテリアルIDの場合はプロジェクションを設定しない(高速化のため)
			update2DRenderProjection(mat->_material, &targetmat->_projectionMatrix2D);
			mat->_target_material_id = material_id;
		}
	}
	else {
		//	メイン画面用の2Dプロジェクション
		if (mat->_target_material_id != proj2Dcode ) {
			update2DRenderProjection(mat->_material, &_projectionMatrix2D);
			mat->_target_material_id = proj2Dcode;
		}
	}

	mesh->start();
	return _bufPolyTex;
}


void gamehsp::drawPolyTex2D( gpmat *mat )
{
    static unsigned short indices[] = { 0, 1, 2, 3 };

	MeshBatch *mesh = mat->_mesh;
	if ( mesh == NULL ) {
        GP_ERROR("Bad Material.");
        return;
	}

	mesh->add( _bufPolyTex, 4, indices, 4 );
    mesh->finish();
    mesh->draw();
}


void gamehsp::addPolyTex2D( gpmat *mat )
{
    static unsigned short indices[] = { 0, 1, 2, 3 };

	MeshBatch *mesh = mat->_mesh;
	if ( mesh == NULL ) {
        GP_ERROR("Bad Material.");
        return;
	}
	mesh->add( _bufPolyTex, 4, indices, 4 );
}


void gamehsp::finishPolyTex2D( gpmat *mat )
{
	MeshBatch *mesh = mat->_mesh;
	if ( mesh == NULL ) {
        GP_ERROR("Bad Material.");
        return;
	}
    mesh->finish();
    mesh->draw();
}


void gamehsp::setPolyDiffuseTex2D( float r, float g, float b, float a )
{
	//	Vertexのカラーコードのみを設定する
	//
	int i;
	float *v = _bufPolyTex;
	for(i=0;i<4;i++) {
		v += 3 + 2;					// Pos,UVを飛ばす
		*v++ = r;
		*v++ = g;
		*v++ = b;
		*v++ = a;
	}
}


float gamehsp::setPolyColorBlend( int gmode, int gfrate )
{
	//	2Dカラー描画設定
	//	(戻り値=alpha値(0.0～1.0))
	//
	Material *material;
	material = _meshBatch->getMaterial();
	return setMaterialBlend( material, gmode, gfrate );
}


void gamehsp::drawTest( int matid )
{

	float points[] ={
	        0,100,0, 1,1,1,1,
	        0,0,0, 1,1,1,1,
	        100,100,0, 1,1,0,1,
	        100,0,0, 1,1,0,1,

	        0,300,0, 1,1,1,0,
	        0,200,0, 1,1,1,0,
	        100,300,0, 1,0,1,1,
	        100,200,0, 1,0,1,1,
	};

    //SPRITE_ADD_VERTEX(v[0], downLeft.x, downLeft.y, z, u1, v1, color.x, color.y, color.z, color.w);
    //SPRITE_ADD_VERTEX(v[1], upLeft.x, upLeft.y, z, u1, v2, color.x, color.y, color.z, color.w);
    //SPRITE_ADD_VERTEX(v[2], downRight.x, downRight.y, z, u2, v1, color.x, color.y, color.z, color.w);
    //SPRITE_ADD_VERTEX(v[3], upRight.x, upRight.y, z, u2, v2, color.x, color.y, color.z, color.w);

//	material->getParameter("u_projectionMatrix")->bindValue(this, &gamehsp::getProjectionMatrix);
//	material->getParameter("u_projectionMatrix")->setValue( _projectionMatrix2D );
//	material->getParameter("u_worldViewProjectionMatrix")->setValue( _camera->getWorldViewProjectionMatrix() );
//	material->getParameter("u_inverseTransposeWorldViewMatrix")->setValue( _camera->getInverseTransposeWorldViewMatrix() );
//	material->getParameter("u_cameraPosition")->setValue( _camera->getTranslation() );



    static unsigned short indices[] = { 0, 1, 2, 3, 3,4,   4,5,6,7, };



    _meshBatch->start();
    _meshBatch->add( points, 8, indices, 10 );
    _meshBatch->finish();
    _meshBatch->draw();


	return;
}


Material* gamehsp::make2DMaterialForMesh( void )
{
	RenderState::StateBlock *state;
	Material* mesh_material = NULL;
	Effect* _spritecol = Effect::createFromSource(intshd_spritecol_vert, intshd_spritecol_frag);
//	Effect* _spritecol = Effect::createFromFile(SPRITECOL_VSH, SPRITECOL_FSH);
	if (_spritecol) {
		mesh_material = Material::create(_spritecol);
	}
	//Material* mesh_material = Material::create(SPRITECOL_VSH, SPRITECOL_FSH, NULL);
	if ( mesh_material == NULL ) {
        GP_ERROR("2D initalize failed.");
        return NULL;
	}
	update2DRenderProjection(mesh_material, &_projectionMatrix2Dpreset);

	state = mesh_material->getStateBlock();
	state->setCullFace(false);
	state->setDepthTest(false);
	state->setDepthWrite(false);
	state->setBlend(true);
	state->setBlendSrc(RenderState::BLEND_SRC_ALPHA);
	state->setBlendDst(RenderState::BLEND_ONE_MINUS_SRC_ALPHA);
	return mesh_material;
}


gameplay::FrameBuffer *gamehsp::makeFremeBuffer(char *name, int sx, int sy)
{
	gameplay::FrameBuffer *frameBuffer;
	frameBuffer = FrameBuffer::create(name, sx, sy);
	DepthStencilTarget* dst = DepthStencilTarget::create(name, DepthStencilTarget::DEPTH_STENCIL, sx, sy);
	frameBuffer->setDepthStencilTarget(dst);
	SAFE_RELEASE(dst);
	return frameBuffer;
}

void gamehsp::deleteFrameBuffer(gameplay::FrameBuffer *fb)
{
	SAFE_RELEASE(fb);
}

int gamehsp::convertAxis(Vector3 *res, Vector3 *pos, int mode)
{
	//	座標変換
	//	mode:0=screen/1=projection/2=view
	//
	Rectangle viewport;

	switch (mode) {
	case 0:
		viewport = getViewport();
		_cameraDefault->project(viewport, *pos, res);
		break;
	case 1:
		viewport.set(0.0f,0.0f,1.0f,1.0f);
		_cameraDefault->project(viewport, *pos, res);
		break;
	case 2:
	{
		Vector4 v1,v2;
		Matrix mat = _cameraDefault->getInverseViewMatrix();
		mat.transformVector(pos->x, pos->y, pos->z, 1.0f, res);
		break;
	}
	case 3:
	{
		Vector4 v1, v2;
		Matrix mat = _cameraDefault->getViewMatrix();
		mat.transformVector(pos->x, pos->y, pos->z, 1.0f, res);
		break;
	}
	default:
		return -1;
	}

	return 0;
}

