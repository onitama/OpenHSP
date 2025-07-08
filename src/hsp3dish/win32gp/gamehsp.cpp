#include "gamehsp.h"

#include "../../hsp3/hsp3config.h"
#include "../supio.h"
#include "../sysreq.h"
#include "../hspwnd.h"

#ifdef WIN32
#include <tchar.h>
#include <direct.h>
#endif

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
static std::string model_defines_shade;

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
	_lighthash = 0;

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

	mFont = NULL;
	_maxobj = 0;
	_gpobj = NULL;
	_gpmat = NULL;
	_gpevent = NULL;
	_colrate = 1.0f / 255.0f;
	_scene = NULL;
	_meshBatch = NULL;
	_meshBatch_line = NULL;
	_meshBatch_font = NULL;
	_spriteEffect = NULL;
	_spritecolEffect = NULL;
	_collision_callback = NULL;
	_originX = 0;
	_originY = 0;
	_scaleX = 1.0f;
	_scaleY = 1.0f;
}

gamehsp::~gamehsp()
{
}

void gamehsp::initialize()
{
	// フォント作成
	if (GetSysReq(SYSREQ_USEGPBFONT)) {
		mFont = Font::create("res/font.gpb");
	}
	resetScreen();
}

void gamehsp::finalize()
{
	// release
	//
	deleteAll();

	SAFE_RELEASE(mFont);
}


void gamehsp::deleteAll( void )
{
	// release
	//
	tmes.texmesTerm();

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
	if (_meshBatch_font) {
		delete _meshBatch_font;
		_meshBatch_font = NULL;
	}

	SAFE_RELEASE(_spriteEffect);
	SAFE_RELEASE(_spritecolEffect);

	if (_scene) {
		_scene->removeAllNodes();
		_scene->setActiveCamera(NULL);
		SAFE_RELEASE(_scene);
	}
	if ( _collision_callback ) {
		delete 	_collision_callback;
		_collision_callback = NULL;
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


void gamehsp::resetCurrentLight(int lightmax, int plightmax, int slightmax)
{
	//	カレントライトの初期化
	//
	_max_dlight = lightmax;
	if (_max_dlight < 0) _max_dlight = GetSysReq(SYSREQ_DLIGHT_MAX);
	if (_max_dlight >= BUFSIZE_MULTILIGHT) _max_dlight = BUFSIZE_MULTILIGHT-1;
	if (_max_dlight > BUFSIZE_MULTILIGHT) _max_dlight = BUFSIZE_MULTILIGHT;
	if (_max_dlight < 1) _max_dlight = 1;
	for (int i = 0; i < BUFSIZE_MULTILIGHT; i++) { _dir_light[i] = _deflight; }

	_max_plight = plightmax;
	if (_max_plight <0) _max_plight = GetSysReq(SYSREQ_PLIGHT_MAX);
	if (_max_plight >= BUFSIZE_MULTILIGHT) _max_plight = BUFSIZE_MULTILIGHT - 1;
	if (_max_plight > BUFSIZE_MULTILIGHT) _max_plight = BUFSIZE_MULTILIGHT;
	for (int i = 0; i < BUFSIZE_MULTILIGHT; i++) { _point_light[i] = _deflight; }

	_max_slight = slightmax;
	if (_max_slight <0) _max_slight = GetSysReq(SYSREQ_SLIGHT_MAX);
	if (_max_slight >= BUFSIZE_MULTILIGHT) _max_slight = BUFSIZE_MULTILIGHT - 1;
	if (_max_slight > BUFSIZE_MULTILIGHT) _max_slight = BUFSIZE_MULTILIGHT;
	for (int i = 0; i < BUFSIZE_MULTILIGHT; i++) { _spot_light[i] = _deflight; }

	_curlight_hash = (_max_slight<<16)|(_max_plight<<8)|(_max_dlight);

	//	シェーダー用のdefine定義を作成
	//
	char tmp[8];
	light_defines = "MODULATE_ALPHA";
	nolight_defines = "MODULATE_ALPHA";
	if (_max_dlight) {
		light_defines += ";DIRECTIONAL_LIGHT_COUNT ";
		sprintf(tmp, "%d", _max_dlight);
		//itoa(_max_dlight,tmp,10);
		light_defines += tmp;// std::to_string(_max_dlight);
	}
	if (_max_plight) {
		light_defines += ";POINT_LIGHT_COUNT ";
		sprintf(tmp, "%d", _max_plight);
		//itoa(_max_plight, tmp, 10);
		light_defines += tmp;// std::to_string(_max_plight);
	}
	if (_max_slight) {
		light_defines += ";SPOT_LIGHT_COUNT ";
		sprintf(tmp, "%d", _max_slight);
		//itoa(_max_slight, tmp, 10);
		light_defines += tmp;// std::to_string(_max_slight);
	}
	splight_defines = light_defines + ";SPECULAR";
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

	//	setup folder
	shader_folder.clear();

#ifdef WIN32
	TCHAR pw[1024];
	_tgetcwd(pw, 1024);
	shader_folder = pw;
	Effect::SetDefaultFolder(pw);
#endif

	// シーン作成
	_scene = Scene::create();
	_curscene = 0;
	_previousFrameBuffer = NULL;
	_filtermode = 0;

	// カメラ作成
	_defcamera = makeNewCam(-1, 45.0f, getAspectRatio(), 0.5f, 768.0f);		// カメラを生成
	selectCamera( _defcamera );

	// free vertex
	clearFreeVertex();

	// シーンライト作成
	_scene->setAmbientColor(0.25f, 0.25f, 0.25f);

	// ライト作成
	_deflight = makeNewLgt(-1, GPLGT_OPT_NORMAL);
	selectLight(_deflight);

	resetCurrentLight();

	// シェーダー定義文字列を生成
	setupDefines();

	// ボーダー初期化
	border1.set( -50.0f, 0.0f, -50.0f );
	border2.set( 50.0f, 100.0f,  50.0f );

	// 2D初期化
	init2DRender();

	// ビューポート初期化
	updateScaledViewport( 0, 0, getWidth(), getHeight() );
	//updateViewport( 0, 0, getWidth(), getHeight() );

	//	固定フレームレート設定
	double fixedrate = (double)GetSysReq(SYSREQ_FIXEDFRAME);
	if (fixedrate < 0.0) {
		setFixedTime(-1);
	}
	else {
		setFixedTime(fixedrate);
	}

	// texmat作成
	tmes.texmesInit(GetSysReq(SYSREQ_MESCACHE_MAX));

	int sx = 32;
	int sy = 32;

	Texture* texture;
	texture = Texture::create(Texture::Format::RGBA, sx, sy, NULL, false, Texture::TEXTURE_2D);
	//texture = Texture::create(id, sx, sy, Texture::Format::RGBA);
	//texture = Texture::create("chr.png");
	if (texture == NULL) {
		Alertf("ERR");
		return;
	}
	//texture->setData( (const unsigned char *)data );
	Material* _fontMaterial;
	_fontMaterial = makeMaterialTex2D(texture, GPOBJ_MATOPT_NOMIPMAP);
	if (_fontMaterial == NULL) {
		Alertf("ERR");
		return;
	}

	VertexFormat::Element elements[] =
	{
		VertexFormat::Element(VertexFormat::POSITION, 3),
		VertexFormat::Element(VertexFormat::TEXCOORD0, 2),
		VertexFormat::Element(VertexFormat::COLOR, 4)
	};

	unsigned int elementCount = sizeof(elements) / sizeof(VertexFormat::Element);
	_meshBatch_font = MeshBatch::create(VertexFormat(elements, elementCount), Mesh::TRIANGLE_STRIP, _fontMaterial, true, 16, 256);

	PhysicsController *pc = Game::getInstance()->getPhysicsController();
	_collision_callback = new CollisionCallback(pc);

	SAFE_RELEASE(texture);
	SAFE_RELEASE(_fontMaterial);

	touchNode = NULL;

}


void gamehsp::updateViewport( int x, int y, int w, int h )
{
	Rectangle viewport;
	_viewx1 = x; _viewy1 = y;
	_viewx2 = w; _viewy2 = h;
	viewport.set((float)x, (float)y, (float)w, (float)h);
	setViewport( viewport );
}


void gamehsp::updateScaledViewport( int x, int y, int w, int h )
{
	float px = (float)w * _scaleX;
	float py = (float)h * _scaleY;
	float xx = (float)x * _scaleX;
	float yy = (float)y * _scaleY;
	updateViewport( _originX + xx, _originY + yy, (int)px, (int)py );
}


void gamehsp::setViewInfo( int orgx, int orgy, float scalex, float scaley )
{
	_originX = orgx;
	_originY = orgy;
	_scaleX = scalex;
	_scaleY = scaley;
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
	if ( _spriteEffect == NULL ) {
        GP_ERROR("2D shader initalize failed.");
        return false;
	}
	_spritecolEffect = Effect::createFromSource(intshd_spritecol_vert, intshd_spritecol_frag);
	if (_spritecolEffect == NULL) {
		GP_ERROR("2D shader initalize failed.");
		return false;
	}

	// MeshBatch for FlatPolygon Draw
    VertexFormat::Element elements[] =
    {
        VertexFormat::Element(VertexFormat::POSITION, 3),
        VertexFormat::Element(VertexFormat::COLOR, 4)
    };
	unsigned int elementCount = sizeof(elements) / sizeof(VertexFormat::Element);
	Material* mesh_material = make2DMaterialForMesh();
	_meshBatch = MeshBatch::create(VertexFormat(elements, elementCount), Mesh::TRIANGLE_STRIP, mesh_material, true, 16, 16 );
	SAFE_RELEASE(mesh_material);

	mesh_material = make2DMaterialForMesh();
	_meshBatch_line = MeshBatch::create(VertexFormat(elements, elementCount), Mesh::LINES, mesh_material, true, 4, 4 );
	SAFE_RELEASE(mesh_material);

	int i;
	for(i=0;i<BUFSIZE_POLYCOLOR;i++) {
		_bufPolyColor[i] = 0.0f;
	}
	for(i=0;i<BUFSIZE_POLYTEX;i++) {
		_bufPolyTex[i] = 0.0f;
	}

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
	if (_meshBatch_font) update2DRenderProjection(_meshBatch_font->getMaterial(), mat);

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


int gamehsp::getCurrentFilterMode(void)
{
	//	現在のフィルターモードを返す
	return _filtermode;
}


void gamehsp::setCurrentFilterMode(int mode)
{
	//	フィルターモードを設定する
	_filtermode = mode;
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
	case GPOBJ_ID_TOUCHNODE:
		if (touchNode == NULL) return -1;
		setNodeVector(NULL, touchNode, moc, prm);
		return 0;
	default:
		return -1;
	}
	if ( obj == NULL ) return -1;
	setNodeVector( obj, obj->_node, moc, prm );
	return 0;
}



void gamehsp::getNodeVectorExternal(gpobj* obj, Node* node, int moc, Vector4* prm)
{
	switch (moc) {
	case MOC_POS:
		if (node) {
			*(Vector3*)prm = node->getTranslationView();
			prm->w = 0.0f;
		}
		break;
	case MOC_WORK:
		if (node) {
			*(Vector3*)prm = node->getTranslation();
			//*(Vector3*)prm = node->getTranslationWorld();
			prm->w = 0.0f;
		}
		break;
	case MOC_QUAT:
		if (node) {
			Quaternion quat;
			quat = node->getRotation();
			prm->x = quat.x;
			prm->y = quat.y;
			prm->z = quat.z;
			prm->w = quat.w;
		}
		break;
	case MOC_SCALE:
		if (node) {
			*(Vector3*)prm = node->getScale();
			prm->w = 1.0f;
		}
		break;
	case MOC_ANGX:
		if (node) {
			Quaternion quat;
			double roll, pitch, yaw;
			quat = node->getRotation();
			QuaternionToEulerAngles(quat, roll, pitch, yaw);
			prm->x = roll;
			prm->y = pitch;
			prm->z = yaw;
			prm->w = 0.0f;
		}
		break;

	case MOC_FORWARD:
		if (node) {
			*(Vector3*)prm = node->getForwardVector();
			prm->w = 1.0f;
		}
		break;
	}
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
	case MOC_FORWARD:
		if (node) {
			*(Vector3 *)prm = node->getForwardVector();
			prm->w = 1.0f;
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
	gpobj *obj;
	int flag_id;
	flag_id = objid & GPOBJ_ID_FLAGBIT;
	if ( flag_id == 0 ) {
		obj = getObj( objid );
		if ( obj == NULL ) return -1;
		if ( obj->_spr ) {
			getSpriteVector( obj, moc, prm );
			return 0;
		}
		if (obj->_phy) {
			gpphy* phy = obj->_phy;
			if (phy->_option & BIND_PHYSICS_MESH) {		// 物理Meshノードの場合は実際のNodeを参照する
				Node* realnode = obj->_node;
				if (realnode) realnode = realnode->getFirstChild();
				if (realnode) {
					getNodeVector(obj, realnode, moc, prm);
					return 0;
				}
			}
		}
		getNodeVector( obj, obj->_node, moc, prm );
		return 0;
	}

	switch (flag_id) {
	case GPOBJ_ID_EXFLAG:
		break;
	default:
		return -1;
	}

	//	GPOBJ_ID_EXFLAGの場合
	switch (objid) {
	case GPOBJ_ID_SCENE:
		setSceneVector(moc, prm);
		return 0;
	case GPOBJ_ID_CAMERA:
		obj = getObj(_defcamera);
		break;
	case GPOBJ_ID_LIGHT:
		obj = getObj(_deflight);
		break;
	case GPOBJ_ID_TOUCHNODE:
		if (touchNode == NULL) return -1;
		getNodeVectorExternal(NULL, touchNode, moc, prm);
		return 0;
	default:
		return -1;
	}
	if (obj == NULL) return -1;

	getNodeVector( obj, obj->_node, moc, prm );
	return 0;
}


void gamehsp::drawNode( Node *node, bool wireflag, float alpha)
{
	//		単一のノードを描画する
	//
	Drawable* drawable = node->getDrawable(); 
	if (drawable == NULL)  return;

	if (wireflag) {
		//	ワイヤーフレーム
		drawable->draw(true);
		_render_numpoly += drawable->_drawtotal;
		return;
	}

	if ( alpha >= 0.0f ) {
		//	3Dモデル用のalpha再設定
		Model* model = dynamic_cast<Model*>(drawable);
		if (model) {
			int part = model->getMeshPartCount();
			Material* mat;
			if (part > 0) {
				for (int i = 0; i < part; i++) {
					mat = model->getMaterial(i);
					if (mat) {
						gameplay::MaterialParameter* prm_modalpha = mat->getParameter("u_modulateAlpha");
						if (prm_modalpha) {
							prm_modalpha->setValue(alpha);
						}
					}
				}
			}
			else {
				mat = model->getMaterial();
				if (mat) {
					gameplay::MaterialParameter* prm_modalpha = mat->getParameter("u_modulateAlpha");
					if (prm_modalpha) {
						prm_modalpha->setValue(alpha);
					}
				}
			}
		}
	}

	drawable->draw(false);
	_render_numpoly += drawable->_drawtotal;
}


bool gamehsp::drawNodeRecursive(Node *node, bool wireflag, float alpha)
{
	//		再帰的にノードを描画する
	//
	drawNode(node, wireflag, alpha);

	Node *pnode = node->getFirstChild();
	while (1) {
		if (pnode == NULL) break;
		drawNodeRecursive(pnode, wireflag, alpha);
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

	int target_group = 1;
	if (camobj) {
		target_group = camobj->_rendergroup;
	}

	num = 0;
	for (i = 0; i < _maxobj; i++) {
		if (obj->isVisible(_scenedraw_lateflag)) {
			if (target_group) {
				if ((target_group & obj->_rendergroup) == 0) {
					obj++;
					continue;
				}
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
					float alpha = obj->getAlphaRate();
					gameplay::MaterialParameter* prm_modalpha = obj->_prm_modalpha;
					if (prm_modalpha) {
						prm_modalpha->setValue(alpha);
						alpha = -1.0f;
					}
					drawNodeRecursive(node, wireflag, alpha);
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

	if (option & GPDRAW_OPT_PHYDEBUG) {
		Game::getInstance()->getPhysicsController()->drawDebug(camera->getViewProjectionMatrix());
	}
}


Drawable *gamehsp::getDrawable(Node *node)
{
	Drawable* drawable = node->getDrawable();
	if (drawable) return drawable;

	Node *pnode = node->getFirstChild();
	while (1) {
		if (pnode == NULL) break;
		drawable = getDrawable(pnode);
		if (drawable) return drawable;
		pnode = pnode->getNextSibling();
	}
	return NULL;
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
				//GP_WARN("#Pass%d ", i);
			}
		}

		MeshSkin* skin = model->getSkin();
		if (skin) {
			Joint *joint = skin->getRootJoint();
			Node* jointParent = joint->getParent();
			GP_WARN("#Skin Root:%s", joint->getId());
		}
	}
	else {
		GP_WARN("#node %s (deep:%d)", node->getId(), deep);
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


int gamehsp::setObjectPrm( int objid, int prmid, int value, int method )
{
	int *base_i;
	int newvalue;
	base_i = getObjectPrmPtr( objid, prmid );
	if ( base_i == NULL ) return -1;

	switch (method)
	{
	case GPOBJ_PRMMETHOD_ON:
		newvalue = value | (*base_i);
		break;
	case GPOBJ_PRMMETHOD_OFF:
		newvalue = (*base_i) & ( value ^ 0xffffffff );
		break;
	default:
		newvalue = value;
		break;
	}

	*base_i = newvalue;

	switch (prmid)
	{
	case GPOBJ_PRMSET_USEGPMAT:
		updateNodeMaterialID(objid);
		break;
	default:
		break;
	}

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
	if ( p_start > p_end ) {
		GP_WARN("Animation Range Invalid");
		return -2;
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

	Material *material;
	if (matid < 0) {
		int matopt = 0;
		//if (_curlight < 0) matopt |= GPOBJ_MATOPT_NOLIGHT;
		material = makeMaterialColor(color, matopt);
		makeNewModel(obj, floorMesh, material);
	}
	else {
		material = getMaterial(matid);
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
	obj->_shape = GPOBJ_SHAPE_FLOOR;
	obj->_sizevec.set( xsize, 0, ysize );

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
	Mesh* mesh = Mesh::createQuad(
		Vector3( -xsize * 0.5f , ysize * 0.5f, 0 ), Vector3( -xsize * 0.5f , -ysize * 0.5f, 0 ), 
		Vector3( xsize * 0.5f ,  ysize * 0.5f, 0 ), Vector3( xsize * 0.5f , -ysize * 0.5f, 0 ));

	//Mesh* floorMesh = createFloorMesh( xsize, ysize );

	Material* material;
	if (matid < 0) {
		int matopt = 0;
		//if ( _curlight < 0 ) matopt |= GPOBJ_MATOPT_NOLIGHT;
		material = makeMaterialColor(color, matopt);
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
	obj->_sizevec.set(xsize, ysize, 0);

	// メッシュ削除
	SAFE_RELEASE(mesh);

	if (_curscene >= 0) {
		_scene->addNode(obj->_node);
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
		//if ( _curlight < 0 ) matopt |= GPOBJ_MATOPT_NOLIGHT;
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

	if ( _curscene >= 0 ) {
		_scene->addNode( obj->_node );
	}

	return obj->_id;
}


bool gamehsp::makeModelNodeMaterialSub(Node *rootnode, int nest)
{
	Node *node = rootnode;
	Material *mat;
	int part, tecs, prms;

	part = 0;
	mat = NULL;
	Drawable* drawable = node->getDrawable();
	Model* model = dynamic_cast<Model*>(drawable);
	if (model) {
		Technique *tec = NULL;
		part = model->getMeshPartCount();
		tecs = 0; prms = 0;
		if (part) {
			for (int i = 0; i < part; i++) {
				mat = model->getMaterial(i);
				if (mat) {
					setMaterialDefaultBinding(mat);
				}
			}
		}
		else {
			mat = model->getMaterial(-1);
			if (mat) {
				setMaterialDefaultBinding(mat);
			}
		}
	}

	Node *pnode = node->getFirstChild();
	while (1) {
		if (pnode == NULL) break;
		makeModelNodeMaterialSub(pnode, nest + 1);
		pnode = pnode->getNextSibling();
	}
	return true;
}


bool gamehsp::makeModelNodeSub(Node *rootnode, int nest)
{
	Node *node = rootnode;
	Material *mat;
	int part, tecs, prms;

	part = 0;
	mat = NULL;
	Drawable* drawable = node->getDrawable();
	Model* model = dynamic_cast<Model*>(drawable);
	if (model){
		Technique *tec = NULL;
		mat = model->getMaterial(-1);
		if (mat) setLightMaterialParameter(mat);

		part = model->getMeshPartCount();
		tecs = 0; prms = 0;
		if (part) {
			for (int i = 0; i < part; i++){
				mat = model->getMaterial(i);
				if (mat) {
					setLightMaterialParameter(mat);
				}
			}
		}
	}

	Node *pnode = node->getFirstChild();
	while (1) {
		if (pnode == NULL) break;
		makeModelNodeSub(pnode, nest + 1);
		pnode = pnode->getNextSibling();
	}
	return true;
}


std::string gamehsp::passCallback(Pass* pass, void* cookie, const char *defs)
{
	char *shade = strstr((char *)defs,"DIRECTIONAL_LIGHT_COUNT");
//	GP_WARN("DEFS:%s",defs);
//	Alertf("%s\r\n%s", defs, model_defines_shade.c_str());
	if (shade == NULL) {
		return model_defines;			//光源計算がない場合
	}
	return model_defines_shade;			//光源計算を行う場合
}


int gamehsp::makeModelNode(char *fname, char *idname, char *defs)
{
	char fn[1024];
	char fn2[1024];
	char fn3[1024];
	gpobj *obj = addObj();
	if (obj == NULL) return -1;

	getpath(fname, fn, 1);
	strcpy(fn2, fn);
	strcpy(fn3, fn);

	if (strstr2(fn, DEFAULT_GPB_FILEEXT) == NULL) {
		strcat(fn, DEFAULT_GPB_FILEEXT);
	}
	if (strstr2(fn2, DEFAULT_MATERIAL_FILEEXT) == NULL) {
		strcat(fn2, DEFAULT_MATERIAL_FILEEXT);
	}
	if (strstr2(fn3, DEFAULT_PHYSISCS_FILEEXT) == NULL) {
		strcat(fn3, DEFAULT_PHYSISCS_FILEEXT);
	}

	Bundle *bundle = Bundle::create(fn);
	Node *rootNode;
	Animation *animation;
	Node *node;

	model_defines = defs;
	model_defines_shade = defs;

	if (*defs != 0) {
		model_defines += ";";
		model_defines_shade += ";";
	}
	model_defines += nolight_defines;
	model_defines_shade += light_defines;

	Material* boxMaterial = Material::create(fn2,gamehsp::passCallback,NULL);
	if (boxMaterial == NULL) {
		deleteObj(obj->_id);
		return -1;
	}

	animation = NULL;
	if (idname) {
		rootNode = bundle->loadNode(idname);
		if (rootNode == NULL) {
			Alertf("Node not found.(%s#%s)", fname, idname);
			deleteObj(obj->_id);
			return -1;
		}
	}
	else {
		Scene *scene;
		char *rootid;
		rootNode = Node::create("objRoot");
		rootid = NULL;
		scene = bundle->loadScene(NULL, gamehsp::passCallback);
		if (scene == NULL) {
			Alertf("Scene not found.(%s)", fname);
			deleteObj(obj->_id);
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
			//subnode->setRefNode(node);
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

	makeModelNodeMaterialSub(rootNode, 0);
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

#if 0
	Properties* properties = Properties::create(fn3);
	if (properties != NULL)
	{
		rootNode->setCollisionObject(properties);
		SAFE_DELETE(properties);
	}
#endif
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

	obj->_lighthash = _curlight_hash;		// ライティングのハッシュ値を設定する
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


int gamehsp::overwriteNodeMaterialByMatID(Node *node, int matid, int objid)
{
	Drawable* drawable = node->getDrawable();
	Model* model = dynamic_cast<Model*>(drawable);
	if (model == NULL) return -1;
	gpmat* mat = getMat(matid);
	if (mat == NULL) return -1;
	NodeCloneContext context;
	Material* material = mat->_material->clone(context);		// 元のマテリアルをクローンして適用する
	setMaterialDefaultBinding(material, mat->_matcolor, mat->_matopt);		// 正しくクローンされないBinding情報を上書きする

	//model->setMaterial(material);

	if (model->getMeshPartCount() == 0) {
		model->setMaterial(material);
	}
	else {
		int part = model->getMeshPartCount();
		for (int i = 0; i < part; i++) {
			model->setMaterial(material, i);
		}
	}
	if (objid >= 0) {
		gpobj *obj = getObj(objid);
		if (obj) {
			obj->updateParameter(material);
		}
	}
	return 0;
}


int gamehsp::overwriteNodeMaterialByColor(Node* node, int color, int matopt, int objid)
{
	Drawable* drawable = node->getDrawable();
	Model* model = dynamic_cast<Model*>(drawable);
	if (model == NULL) return -1;
	Material* material = model->getMaterial();
	setMaterialDefaultBinding(material, color, matopt);		// 正しくクローンされないBinding情報を上書きする
	if (objid >= 0) {
		gpobj* obj = getObj(objid);
		if (obj) {
			obj->updateParameter(material);
		}
	}
	return 0;
}


int gamehsp::updateNodeMaterialID(int objid)
{
	//	_usegpmatの変更を反映させる()
	gpobj* obj;
	obj = getObj(objid);
	if (obj == NULL) return -1;
	if (obj->_spr) return -1;
	if (obj->_usegpmat < 0) return -1;

	Node* mynode = obj->_node;
	if (mynode == NULL) return -1;
	overwriteNodeMaterialByMatID(mynode, obj->_usegpmat, objid);
	return 0;
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
		gpobj* newobj = addObj();
		if (newobj == NULL) return -1;

		node = obj->_node;
		if (node == NULL) {
			return newobj->_id;
		}

		newobj->_mode = obj->_mode;
		newobj->_timer = obj->_timer;
		newobj->_mygroup = obj->_mygroup;
		newobj->_colgroup = obj->_colgroup;
		newobj->_shape = obj->_shape;
		newobj->_usegpmat = obj->_usegpmat;
		newobj->_transparent = obj->_transparent;
		newobj->_fade = obj->_fade;
		newobj->_rendergroup = obj->_rendergroup;
		newobj->_lightgroup = obj->_lightgroup;
		newobj->_lighthash = obj->_lighthash;

		newobj->_sizevec = obj->_sizevec;

		//Alertf("mat:%d", newobj->_usegpmat);

		node->setUserObject(NULL);

		newobj->_node = node->clone();

		bool bNeedUpdateMaterial = false;
		bool bNeedUpdatePosition = false;

		if (obj->_phy) {
			gpphy* phy = obj->_phy;
			if (phy->_option & BIND_PHYSICS_MESH) {		// 物理Meshノードの場合は実際のNode座標をクリアする
				bNeedUpdatePosition = true;
			}
		}

		switch (newobj->_shape) {
		case GPOBJ_SHAPE_BOX:
		case GPOBJ_SHAPE_FLOOR:
		case GPOBJ_SHAPE_PLATE:
		case GPOBJ_SHAPE_MESH:
			bNeedUpdateMaterial = true;
		}
		if (bNeedUpdateMaterial) {
			//	マテリアルの更新
			Node* mynode = newobj->_node;
			if (newobj->_usegpmat >= 0) {
				overwriteNodeMaterialByMatID(mynode, newobj->_usegpmat);
			}
			else {
				overwriteNodeMaterialByColor(mynode, -1, 0);
			}
		}

		newobj->_animation = newobj->_node->getAnimation("animations");

		newobj->_node->setUserObject(newobj);
		newobj->addRef();

		makeModelNodeSub(newobj->_node, 0);
		{
			gameplay::Vector3 ppp;
			Node* realnode = newobj->_node;
			if (realnode) {
				realnode = realnode->getFirstChild();
				if (realnode) {
					if (bNeedUpdatePosition) {
						//realnode->getTranslation(&ppp);
						//Alertf("(%f,%f,%f)",ppp.x,ppp.y,ppp.z );
						realnode->setRotation(0.0f, 0.0f, 0.0f, 0.0f);
						realnode->setTranslation(0.0f, 0.0f, 0.0f);
					}
				}
			}
		}

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

	if (obj->isAlive() == false) return;

	if (obj->executeFade()) {
		deleteObj(obj->_id);
		return;
	}
	if (mode & GPOBJ_MODE_TIMER) {
		if (obj->_timer<=0) {
			deleteObj(obj->_id);
			return;
		}
		obj->_timer--;
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

	//	Update physics and animation
	updateAnimation();

	gpobj *obj = _gpobj;
	for(i=0;i<_maxobj;i++) {
		if (obj->isAlive()) {
			// Execute Events
			for (j = 0; j<GPOBJ_MULTIEVENT_MAX; j++) {
				if (obj->GetEvent(j) != NULL) ExecuteObjEvent(obj, timepass, j);
			}
		}
		if (obj->isAlive()) {
			updateObj(obj);
		}
		obj++;
	}

}


int gamehsp::updateObjColi( int objid, float size, int addcol, int startid , int searchnum )
{
	//		範囲内に衝突するオブジェクトを検索する
	//
	int i;
	int chkgroup;
	gpobj *obj;
	gpobj *atobj;
	gpspr *spr;
	Vector3 *pos;
	int num = searchnum;
	if ((startid + num) > _maxobj) {
		num = -1;
	}
	if (num < 0) {
		num = _maxobj - startid;
	}

	obj = getSceneObj( objid );
	if ( obj == NULL ) return -1;

	if ( addcol == 0 ) {
		chkgroup = obj->_colgroup;
	} else {
		chkgroup = addcol;
	}
	if (chkgroup == 0) return -1;

	atobj = &_gpobj[startid];

	spr = obj->_spr;
	if ( spr ) {									// 2Dスプライト時の処理
		gpspr *atspr;
		pos = (Vector3 *)&spr->_pos;
		for(i= 0;i<num;i++) {
			if ( atobj->isAlive() ) {
				if (( atobj->_mygroup & chkgroup )&&( (startid+i) != objid )) {
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

	if (size < 0.0f) {
		bound = obj->_node->getBoundingSphere();
		bound.radius *= fabs(size);							// 自分のサイズを調整する
		//Alertf("s%f", bound.radius);

		for (i = 0; i < num; i++) {
			if (atobj->isAlive()) {
				if ((atobj->_mygroup & chkgroup) && ((startid+i) != objid)) {
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

	for (i = 0; i < num; i++) {
		if (atobj->isAlive()) {
			if ((atobj->_mygroup & chkgroup) && ((startid+i) != objid)) {
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


int gamehsp::getNearestObj(int id, float range, int colgroup)
{
	//		最も近いオブジェクトを検索する
	//
	int i;
	int chkgroup;
	gpobj* obj;
	gpobj* atobj;
	gpspr* spr;
	Vector3* pos;
	int result = -1;
	float size = range;
	float distance;

	obj = getSceneObj(id);
	if (obj == NULL) return -1;

	if (colgroup == 0) {
		chkgroup = obj->_colgroup;
	}
	else {
		chkgroup = colgroup;
	}
	if (size <= 0.0f) return -1;

	spr = obj->_spr;
	if (spr) {									// 2Dスプライト時の処理
		gpspr* atspr;
		pos = (Vector3*)&spr->_pos;
		atobj = _gpobj;
		for (i = 0; i < _maxobj; i++) {
			if (atobj->isAlive()) {
				if ((atobj->_mygroup & chkgroup) && (i != id)) {
					atspr = atobj->_spr;
					if (atspr) {
						distance = atspr->getDistance(pos);
						if (distance < size) {
							result = atobj->_id;
							size = distance;
						}
					}
				}
			}
			atobj++;
		}
		return result;
	}

	Vector3 vpos;
	Vector3 enepos;
	Node* node;
	BoundingSphere bound;
	if (obj->_node == NULL) return -1;

	vpos = obj->_node->getTranslation();
	atobj = _gpobj;

	if (size < 0.0f) {
		bound = obj->_node->getBoundingSphere();
		bound.radius *= fabs(size);							// 自分のサイズを調整する
		size = bound.radius;
		//Alertf("s%f", bound.radius);

		for (i = 0; i < _maxobj; i++) {
			if (atobj->isAlive()) {
				if ((atobj->_mygroup & chkgroup) && (i != id)) {
					node = atobj->_node;
					if (node) {
						BoundingSphere tbound = node->getBoundingSphere();
						Vector3& point = tbound.center;
						distance = sqrt((point.x - bound.center.x) * (point.x - bound.center.x) +
							(point.y - bound.center.y) * (point.y - bound.center.x) +
							(point.z - bound.center.z) * (point.z - bound.center.x));
						if (distance < size) {
							result = atobj->_id;
							size = distance;
						}
					}
				}
			}
			atobj++;
		}
		return result;
	}

	for (i = 0; i < _maxobj; i++) {
		if (atobj->isAlive()) {
			if ((atobj->_mygroup & chkgroup) && (i != id)) {
				node = atobj->_node;
				if (node) {
					enepos = node->getTranslation();
					enepos -= vpos;
					distance = enepos.length();
					if (distance < size) {
						result = atobj->_id;
						size = distance;
					}
				}
			}
		}
		atobj++;
	}
	return result;
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
	sz = size;
	if ( fabs( _pos.x - v->x ) < sz ) {
		if ( fabs( _pos.y - v->y ) < sz ) {
			return 1;
		}
	}
	return 0;
}

float gpspr::getDistance(Vector3* v)
{
	float sx = fabs(_pos.x - v->x);
	float sy = fabs(_pos.y - v->y);
	if (sx < sy) return sx;
	return sy;
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
		Text message support
*/
/*------------------------------------------------------------*/

void gamehsp::texmesProc(void)
{
	tmes.texmesProc();
}

void gamehsp::texmesDrawClip(void *bmscr, int x, int y, int psx, int psy, texmes *tex, int basex, int basey)
{
	//		画像コピー
	//		texid内の(xx,yy)-(xx+srcsx,yy+srcsy)を現在の画面に(psx,psy)サイズでコピー
	//		カレントポジション、描画モードはBMSCRから取得
	//
	//	float psx, psy;
	float x1, y1, x2, y2, sx, sy;
	float tx0, ty0, tx1, ty1;

	//		meshのTextureを差し替える
	Uniform* samplerUniform = NULL;
	for (unsigned int i = 0, count = _spriteEffect->getUniformCount(); i < count; ++i)
	{
		Uniform* uniform = _spriteEffect->getUniform(i);
		if (uniform && uniform->getType() == GL_SAMPLER_2D)
		{
			samplerUniform = uniform;
			break;
		}
	}

	MaterialParameter* mp = _meshBatch_font->getMaterial()->getParameter(samplerUniform->getName());
	mp->setValue(tex->_texture);

	tx0 = ((float)(basex));
	tx1 = ((float)(basex+psx));
	ty0 = ((float)(basey));
	ty1 = ((float)(basey+psy));

	x1 = ((float)x);
	y1 = ((float)y);
	x2 = x1 + psx;
	y2 = y1 + psy;

	sx = tex->ratex;
	sy = tex->ratey;

	tx0 *= sx;
	tx1 *= sx;
	ty0 *= sy;
	ty1 *= sy;

	float* v = _bufPolyTex;

	BMSCR *bm = (BMSCR *)bmscr;
	float r_val = bm->colorvalue[0];
	float g_val = bm->colorvalue[1];
	float b_val = bm->colorvalue[2];
	float a_val = setPolyColorBlend(bm->gmode, bm->gfrate);

	*v++ = x1; *v++ = y2; v++;
	*v++ = tx0; *v++ = ty1;
	*v++ = r_val; *v++ = g_val; *v++ = b_val; *v++ = a_val;
	*v++ = x1; *v++ = y1; v++;
	*v++ = tx0; *v++ = ty0;
	*v++ = r_val; *v++ = g_val; *v++ = b_val; *v++ = a_val;
	*v++ = x2; *v++ = y2; v++;
	*v++ = tx1; *v++ = ty1;
	*v++ = r_val; *v++ = g_val; *v++ = b_val; *v++ = a_val;
	*v++ = x2; *v++ = y1; v++;
	*v++ = tx1; *v++ = ty0;
	*v++ = r_val; *v++ = g_val; *v++ = b_val; *v++ = a_val;

	static unsigned short indices[] = { 0, 1, 2, 3 };

	_meshBatch_font->start();
	_meshBatch_font->add(_bufPolyTex, 4, indices, 4);
	_meshBatch_font->finish();
	_meshBatch_font->draw();

	mp->setValue(0);
}


int gamehsp::drawFont(void *bmscr, int x, int y, char* text, int* out_ysize)
{
	// フォントで描画
	int xsize, ysize;
	Vector4 p_color;
	BMSCR *bm = (BMSCR *)bmscr;

	if (mFont == NULL) {
		mFont = Font::create("res/font.gpb");
		if (mFont == NULL) return 0;
	}
	p_color.x = bm->colorvalue[0];
	p_color.y = bm->colorvalue[1];
	p_color.z = bm->colorvalue[2];
	p_color.w = bm->colorvalue[3];
	mFont->start();
	xsize = mFont->drawText(text, x, y, p_color, tmes._fontsize);
	mFont->finish();
	ysize = tmes._fontsize;
	*out_ysize = ysize;
	return xsize;
}


void gamehsp::setFont(char* fontname, int size, int style)
{
	// フォント設定
	tmes.setFont(fontname,size,style);
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

	BoundingSphere sphere;
	sphere.radius = (float)sqrt((a*a)+(a*a));
	mesh->setBoundingSphere(sphere);
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
	mat->applyFilterMode(_filtermode);
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
	mat->applyFilterMode(_filtermode);
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
	if (_spritecolEffect) {
		mesh_material = Material::create(_spritecolEffect);
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

int gamehsp::setObjLight(int objid)
{
	//		ライト設定
	//
	gpobj *obj = getObj(objid);
	if (obj == NULL) return 0;

	Node *rootNode = obj->_node;
	if (rootNode) {
		makeModelNodeSub(rootNode, 0);
	}
	return 0;
}


gamehsp::FreeMeshVertex::FreeMeshVertex()
{
	x = y = z = 0.0f;		// vertex
	nx = ny = nz = 0.0f;	// normal
	u = v = 0.0f;			// UV
}


gamehsp::FreeMeshVertex::~FreeMeshVertex()
{
}


void gamehsp::clearFreeVertex(void)
{
	_freevertex.clear();
	_freeindex.clear();
	_fv_minradius = 0.0f;
	_fv_maxradius = 0.0f;
}


int gamehsp::addFreeVertex(float x, float y, float z, float nx, float ny, float nz, float u, float v)
{
	int res;

	res = _freevertex.size();

	// データの重複チェック
	for (int i = 0; i < res; i++) {
		FreeMeshVertex *vv = &_freevertex[i];
		if ((vv->x == x) && (vv->y == y) && (vv->z == z)) {
			if ((vv->nx == nx) && (vv->ny == ny) && (vv->nz == nz)) {
				if ((vv->u == u) && (vv->v == v)) {
					return i;
				}
			}
		}
	}

	FreeMeshVertex vert;
	vert.x = x; vert.y = y; vert.z = z;
	vert.nx = nx; vert.ny = ny; vert.nz = nz;
	vert.u = u; vert.v = v;
	_freevertex.push_back(vert);

	if (x > _fv_maxradius) _fv_maxradius = x;
	if (y > _fv_maxradius) _fv_maxradius = y;
	if (z > _fv_maxradius) _fv_maxradius = z;
	if (x < _fv_minradius) _fv_minradius = x;
	if (y < _fv_minradius) _fv_minradius = y;
	if (z < _fv_minradius) _fv_minradius = z;

	return res;
}


int gamehsp::addFreeVertexPolygon(int id1, int id2, int id3, int id4)
{
	int res;
	res = _freeindex.size();

	_freeindex.push_back((short)id1);
	_freeindex.push_back((short)id2);
	_freeindex.push_back((short)id3);

	if (id4 >= 0) {
		_freeindex.push_back((short)id3);
		_freeindex.push_back((short)id2);
		_freeindex.push_back((short)id4);
	}

	return res;
}


int gamehsp::makeFreeVertexNode(int color, int matid)
{
	float *vertices = (float *)_freevertex.data();
	short *indices = (short *)_freeindex.data();

	unsigned int vertexCount = _freevertex.size();
	unsigned int indexCount = _freeindex.size();

	if ((vertexCount==0) || (indexCount==0)) return -1;

	gpobj* obj = addObj();
	if (obj == NULL) return -1;

	VertexFormat::Element elements[] =
	{
		VertexFormat::Element(VertexFormat::POSITION, 3),
		VertexFormat::Element(VertexFormat::NORMAL, 3),
		VertexFormat::Element(VertexFormat::TEXCOORD0, 2)
	};
	Mesh* mesh = Mesh::createMesh(VertexFormat(elements, 3), vertexCount, false);
	if (mesh == NULL)
	{
		GP_ERROR("Failed to create user mesh.");
		deleteObj(obj->_id);
		return -1;
	}
	mesh->setVertexData(vertices, 0, vertexCount);
	MeshPart* meshPart = mesh->addPart(Mesh::TRIANGLES, Mesh::INDEX16, indexCount, false);
	meshPart->setIndexData(indices, 0, indexCount);

	float _fv_radius = _fv_maxradius - _fv_minradius;
	BoundingSphere sphere;
	sphere.radius = _fv_radius;
	mesh->setBoundingSphere(sphere);

	Material *material;
	if (matid < 0) {
		int matopt = 0;
		//if ( _curlight < 0 ) matopt |= GPOBJ_MATOPT_NOLIGHT;
		material = makeMaterialColor(color, matopt);
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
	obj->_sizevec.set(_fv_radius, _fv_radius, _fv_radius);

	// メッシュ削除
	SAFE_RELEASE(mesh);

	if (_curscene >= 0) {
		_scene->addNode(obj->_node);
	}

	return obj->_id;
}


bool gamehsp::getNodeFromNameSub(Node* node, char *name, int deep)
{
	if (tempNode != NULL) return true;
	if (strcmp(node->getId(), name) == 0) {
		tempNode = node;
		return true;
	}

	Drawable* drawable = node->getDrawable();
	Model* model = dynamic_cast<Model*>(drawable);
	if (model) {
		MeshSkin *skin = model->getSkin();
		if (skin) {
			Node *root = skin->getRootNode();
			if (root) {
				if (getNodeFromNameSub(root, name, deep + 1)) return true;
			}
		}
	}

	Node* pnode = node->getFirstChild();
	while (1) {
		if (pnode == NULL) break;
		if (getNodeFromNameSub(pnode, name, deep + 1)) return true;
		pnode = pnode->getNextSibling();
	}

	return false;
}


Node* gamehsp::getNodeFromName(int objid, char* name)
{
	gpobj* obj = getObj(objid);
	if (obj == NULL) return NULL;
	if (name == NULL) return NULL;

	tempNode = NULL;

	Node* node = obj->_node;
	if (*name == 0) {
		tempNode = node;
		return tempNode;
	}

	while (1) {
		if (node == NULL) break;
		getNodeFromNameSub(node, name, 0);
		if (tempNode != NULL) break;
		node = node->getNextSibling();
	}
	return tempNode;
}


int gamehsp::getNodeInfo(int objid, int option, char* name, int *result)
{
	int res = 0;
	Model* model;
	Drawable* drawable;
	Node* node = getNodeFromName(objid, name);
	if (node == NULL) {
		*result = -1;
		return -1;
	}
	if (option & GPNODEINFO_MATERIAL) {
		int matindex = option & (0x7f);
		*result = makeNewMatFromObj(objid, matindex, name);
		return res;
	}
	switch(option) {
	case GPNODEINFO_NODE:
		touchNode = node;
		*result = GPOBJ_ID_TOUCHNODE;
		break;
	case GPNODEINFO_MODEL:
	{
		drawable = node->getDrawable();
		model = dynamic_cast<Model*>(drawable);
		if (model) {
			touchNode = node;
			*result = GPOBJ_ID_TOUCHNODE;
		}
		else {
			*result = -1;
		}
		break;
	}
	case GPNODEINFO_MATNUM:
		drawable = node->getDrawable();
		model = dynamic_cast<Model*>(drawable);
		if (model) {
			*result = (int)model->getMeshPartCount();
		}
		else {
			*result = -1;
		}
		break;
	default:
		return -1;
	}
	return res;
}


int gamehsp::getNodeInfoString(int objid, int option, char* name, std::string* res)
{
	int result = 0;
	Node* node = getNodeFromName(objid, name);
	if (node == NULL) return -1;

	if (option & GPNODEINFO_MATNAME) {
		int matindex = option & (0xffff);
		Drawable* drawable = node->getDrawable();
		Model* model = dynamic_cast<Model*>(drawable);
		Material* material = NULL;
		res->clear();
		if (model) {
			if (model->getMeshPartCount() == 0) {
				material = model->getMaterial();
			}
			else {
				material = model->getMaterial(matindex);
			}
			if (material) {
				*res = material->getName();
			}
		}
		return result;
	}

	switch (option) {
	case GPNODEINFO_NAME:
		*res = node->getId();
		break;
	case GPNODEINFO_CHILD:
		node = node->getFirstChild();
		if (node) {
			*res = node->getId();
		}
		break;
	case GPNODEINFO_SIBLING:
		node = node->getNextSibling();
		if (node) {
			*res = node->getId();
		}
		break;
	case GPNODEINFO_SKINROOT:
	{
		Drawable* drawable = node->getDrawable();
		Model* model = dynamic_cast<Model*>(drawable);
		if (model) {
			MeshSkin *skin = model->getSkin();
			if (skin) {
				Node *root = skin->getRootNode();
				if (root) {
					*res = root->getId();
				}
			}
		}
		break;
	}
	default:
		return -1;
	}
	return result;
}


int gamehsp::setNodeInfoMaterial(int objid, int option, char* name, int matid)
{
	int result = 0;
	Node* node = getNodeFromName(objid, name);
	if (node == NULL) return -1;

	Drawable* drawable = node->getDrawable();
	Model* model = dynamic_cast<Model*>(drawable);
	if (model == NULL) return -1;

	Material* material = getMaterial(matid);
	if (material == NULL) return -1;

	if (model->getMeshPartCount() == 0) {
		model->setMaterial(material);
	}
	else {
		model->setMaterial(material, option);
	}
	return result;
}


