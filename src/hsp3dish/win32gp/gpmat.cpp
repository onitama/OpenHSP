
#include <stdio.h>
#include <string.h>
#include "gamehsp.h"

#include "../../hsp3/hsp3config.h"
#include "../supio.h"
#include "../sysreq.h"

// Default sprite shaders
#define SPRITE_VSH "res/shaders/sprite.vert"
#define SPRITE_FSH "res/shaders/sprite.frag"

#define SPRITECOL_VSH "res/shaders/spritecol.vert"
#define SPRITECOL_FSH "res/shaders/spritecol.frag"

bool hasParameter( Material* material, const char* name );

/*------------------------------------------------------------*/
/*
		gameplay Material Obj
*/
/*------------------------------------------------------------*/

gpmat::gpmat()
{
	// コンストラクタ
	_flag = GPMAT_FLAG_NONE;
	_matbuffer = NULL;
}

gpmat::~gpmat()
{
	revoke();
}

void gpmat::revoke(void)
{
	if (_flag == GPMAT_FLAG_NONE) return;

	if (_matbuffer) {
		delete[] _matbuffer;
	}
	_flag = GPMAT_FLAG_NONE;
	_matbuffer = NULL;
}


void gpmat::reset( gamehsp *owner, int id )
{
	revoke();

	_owner = owner;
	_mode = 0;
	_mark = 0;
	_material = NULL;
	_mesh = NULL;
	_flag = GPMAT_FLAG_ENTRY;
	_id = id;
	_sx = 0;
	_sy = 0;
	_texratex = 0.0f;
	_texratey = 0.0f;
	_target_material_id = -1;
	_matopt = 0;
	_matcolor = -1;
	_filtermode = 1;
}


int gpmat::setParameter( char *name, Vector4 *value )
{
	if ( _material == NULL ) return -1;
	_material->getParameter(name)->setValue(*value);

	return 0;
}


int gpmat::setParameter(char* name, float value, float value2)
{
	if (_material == NULL) return -1;

	gameplay::Vector2 vec2;
	vec2.set(value, value2);
	_material->getParameter(name)->setValue(vec2);

	return 0;
}


int gpmat::setParameter(char* name, Vector3* value)
{
	if (_material == NULL) return -1;
	_material->getParameter(name)->setValue(*value);

	return 0;
}


int gpmat::setParameter( char *name, float value )
{
	if ( _material == NULL ) return -1;
    _material->getParameter( name )->setValue( value );

	return 0;
}


int gpmat::setParameter(char *name, double* p_mat, int count)
{
	if (_material == NULL) return -1;
	if (count < 1) return -2;

	if (_matbuffer) {
		delete [] _matbuffer;
	}
	_matbuffer = new gameplay::Matrix[count];
	gameplay::Matrix *matdat = _matbuffer;

	for (int i = 0; i < count; i++) {
		matdat[i] = {
			(float)p_mat[0], (float)p_mat[1], (float)p_mat[2], (float)p_mat[3],
			(float)p_mat[4], (float)p_mat[5], (float)p_mat[6], (float)p_mat[7],
			(float)p_mat[8], (float)p_mat[9], (float)p_mat[10], (float)p_mat[11],
			(float)p_mat[12], (float)p_mat[13], (float)p_mat[14], (float)p_mat[15]
		};
	}
	if (count == 1) {
		_material->getParameter(name)->setValue(_matbuffer[0]);
	}
	else {
		_material->getParameter(name)->setValue((const gameplay::Matrix*)_matbuffer, count);
	}

	return 0;
}


int gpmat::setParameter(char *name, char *fname, int matopt)
{
	bool mipmap,cubemap;
	if (_material == NULL) return -1;
	mipmap = (matopt & GPOBJ_MATOPT_NOMIPMAP) == 0;
	cubemap = (matopt & GPOBJ_MATOPT_CUBEMAP) != 0;
	_material->getParameter(name)->setValue( fname, mipmap,cubemap);

	return 0;
}


int gpmat::setParameter(char* name, Texture::Sampler* samp)
{
	if (_material == NULL) return -1;
	_material->getParameter(name)->setValue(samp);
	return 0;
}


Texture::Sampler* gpmat::getSampler(char *name)
{
	MaterialParameter* mprm;
	if (*name == 0) {
		mprm = _material->getParameter("u_diffuseTexture");
	}
	else {
		mprm = _material->getParameter(name);
	}
	if (mprm == NULL) return NULL;
	return mprm->getSampler();
}


int gpmat::setState(char *name, char *value)
{
	RenderState::StateBlock *state;

	if ( _material == NULL ) return -1;

	state = _material->getStateBlock();
	state->setState( name, value );

	return 0;
}

void gpmat::setFilter(Texture::Filter value)
{
	MaterialParameter *mprm = _material->getParameter("u_diffuseTexture");
	if (mprm == NULL) return;
	Texture::Sampler *sampler = mprm->getSampler();
	if (sampler == NULL) return;
	sampler->setFilterMode(value, value);
}

void gpmat::applyFilterMode(int mode)
{
	if (mode == _filtermode) return;
	_filtermode = mode;

	if (mode == 0) {
		setFilter(Texture::Filter::NEAREST);
	}
	else {
		setFilter(Texture::Filter::LINEAR);
	}
}

int gpmat::updateTex32(char* ptr, int mode)
{
	MaterialParameter* mprm = _material->getParameter("u_diffuseTexture");
    if (mprm == NULL) return -1;
	Texture::Sampler* sampler = mprm->getSampler();
	if (sampler == NULL) return -2;
	Texture* tex = sampler->getTexture();
	if (tex == NULL) return -3;

	tex->setData( (unsigned char *)ptr );

	return 0;
}

/*------------------------------------------------------------*/
/*
Material process for gpobj
*/
/*------------------------------------------------------------*/

int gpobj::setParameter(char *name, Vector4 *value,int part)
{
	if (_model == NULL) return -1;
	Material *material = _model->getMaterial(part);
	if (material == NULL) return -1;
	MaterialParameter *mp = material->getParameter(name);
	if (mp == NULL) return -1;
	mp->setValue(*value);

	return 0;
}


int gpobj::setParameter(char *name, Vector3 *value, int part)
{
	if (_model == NULL) return -1;
	Material *material = _model->getMaterial(part);
	if (material == NULL) return -1;
	material->getParameter(name)->setValue(*value);

	return 0;
}


int gpobj::setParameter(char* name, float value, int part)
{
	if (_model == NULL) return -1;
	Material* material = _model->getMaterial(part);
	if (material == NULL) return -1;
	material->getParameter(name)->setValue(value);

	return 0;
}


int gpobj::setParameter(char* name, float value, float value2, int part)
{
	if (_model == NULL) return -1;
	Material* material = _model->getMaterial(part);
	if (material == NULL) return -1;
	gameplay::Vector2 vec2;
	vec2.set(value, value2);
	material->getParameter(name)->setValue(vec2);

	return 0;
}


int gpobj::setParameter(char *name, double * p_mat, int count, int part)
{
	if (_model == NULL) return -1;
	Material *material = _model->getMaterial(part);
	if (material == NULL) return -1;
	if (count > 1) return -2;

	gameplay::Matrix matdat	((float)p_mat[0], (float)p_mat[1], (float)p_mat[2], (float)p_mat[3],
			(float)p_mat[4], (float)p_mat[5], (float)p_mat[6], (float)p_mat[7],
			(float)p_mat[8], (float)p_mat[9], (float)p_mat[10], (float)p_mat[11],
			(float)p_mat[12], (float)p_mat[13], (float)p_mat[14], (float)p_mat[15]);
	material->getParameter(name)->setValue(matdat);
	return 0;
}


int gpobj::setParameter(char *name, Texture::Sampler *sampler, int part)
{
	if (_model == NULL) return -1;
	Material* material = _model->getMaterial(part);
	if (material == NULL) return -1;
	material->getParameter(name)->setValue(sampler);
	return 0;
}


int gpobj::setParameter(char *name, char *fname, int matopt, int part)
{
	bool mipmap;
	if (_model == NULL) return -1;
	Material *material = _model->getMaterial(part);
	if (material == NULL) return -1;
	mipmap = (matopt & GPOBJ_MATOPT_NOMIPMAP) == 0;
	material->getParameter(name)->setValue(fname, mipmap);

	return 0;
}

int gpobj::setState(char *name, char *value, int part)
{
	RenderState::StateBlock *state;

	if (_model == NULL) return -1;
	Material *material = _model->getMaterial(part);
	if (material == NULL) return -1;

	state = material->getStateBlock();
	state->setState(name, value);

	return 0;
}

void gpobj::setFilter(Texture::Filter value, int part)
{
	if (_model == NULL) return;
	Material *material = _model->getMaterial(part);

	MaterialParameter *mprm = material->getParameter("u_diffuseTexture");
    if (mprm == NULL) return;
	Texture::Sampler *sampler = mprm->getSampler();
	if (sampler == NULL) return;
	sampler->setFilterMode(value, value);
}

Texture::Sampler *gpobj::getSamplerByName(char *name, int part)
{
	if (_model == NULL) return NULL;
	Material* material = _model->getMaterial(part);

	MaterialParameter* mprm;
	if (*name == 0) {
		mprm = material->getParameter("u_diffuseTexture");
	}
	else {
		mprm = material->getParameter(name);
	}
	if (mprm == NULL) return NULL;
	Texture::Sampler* sampler = mprm->getSampler();
	if (sampler == NULL) return NULL;
	return sampler;
}


/*------------------------------------------------------------*/
/*
		Material process
*/
/*------------------------------------------------------------*/

gpmat *gamehsp::getMat( int id )
{
	int flag_id;
	int base_id;
	flag_id = id & GPOBJ_ID_FLAGBIT;
	if ( flag_id != GPOBJ_ID_MATFLAG ) return NULL;
	base_id = id & GPOBJ_ID_FLAGMASK;
	if (( base_id < 0 )||( base_id >= _maxmat )) return NULL;
	if ( _gpmat[base_id]._flag == GPMAT_FLAG_NONE ) return NULL;
	return &_gpmat[base_id];
}


int gamehsp::deleteMat( int id )
{
	gpmat *mat = getMat( id );
	if ( mat == NULL ) return -1;
	mat->_flag = GPMAT_FLAG_NONE;
	if ( mat->_mesh ) {
		delete mat->_mesh;
		mat->_mesh = NULL;
	}

	Material* material = mat->_material;
	if (material) {
		//material->removeParameter("u_texture");
		material->removeParameter("u_diffuseTexture");

		SAFE_RELEASE(material);
	}
	return 0;
}


gpmat *gamehsp::addMat( void )
{
	int i;
	gpmat *mat = _gpmat;
	for(i=0;i<_maxmat;i++) {
		if ( mat->_flag == GPMAT_FLAG_NONE ) {
			mat->reset( this, i|GPOBJ_ID_MATFLAG );
			return mat;
		}
		mat++;
	}
	return NULL;
}


Material *gamehsp::getMaterial( int matid )
{
	gpmat *mat = getMat( matid );
	if ( mat == NULL ) return NULL;
	return mat->_material;
}


void gamehsp::setLightMaterialParameter(Material* material)
{
	gpobj *lgt;
	Node *light_node;

	//	ディレクショナルライト
	for (int i = 0; i < _max_dlight; i++) {
		lgt = getObj(_dir_light[i]);
		light_node = lgt->_node;
		if (light_node) {
			// ライトの方向設定
			lightname_direction[28] = '0' + i;	// "u_directionalLightDirection[0]"
			if (hasParameter(material, lightname_direction)) {
				material->getParameter(lightname_direction)->bindValue(light_node, &Node::getForwardVectorView);
			}
			// ライトの色設定
			lightname_color[24] = '0' + i;	// "u_directionalLightColor[0]"
			if (hasParameter(material, lightname_color)) {
				material->getParameter(lightname_color)->bindValue(light_node, &Node::getLightColor);
				//material->getParameter(lightname_color)->setValue(light_node->getLight()->getColor());
			}
		}
	}
	//	ポイントライト
	for (int i = 0; i < _max_plight; i++) {
		lgt = getObj(_point_light[i]);
		light_node = lgt->_node;
		if (light_node) {
			Light *lg = light_node->getLight();
			if (lg->getLightType() == gameplay::Light::POINT) {
				// ライトの方向設定
				lightname_pointposition[21] = '0' + i;	// "u_pointLightPosition[0]"
				if (hasParameter(material, lightname_pointposition)) {
					material->getParameter(lightname_pointposition)->bindValue(light_node, &Node::getTranslationView);
				}
				// ライトの色設定
				lightname_pointcolor[18] = '0' + i;	// "u_pointLightColor[0]"
				if (hasParameter(material, lightname_pointcolor)) {
					material->getParameter(lightname_pointcolor)->bindValue(light_node, &Node::getLightColor);
					//material->getParameter(lightname_pointcolor)->setValue(lg->getColor());
				}
				// ライトの範囲
				lightname_pointrange[25] = '0' + i;	// "u_pointLightRangeInverse[0]"
				if (hasParameter(material, lightname_pointrange)) {
					material->getParameter(lightname_pointrange)->setValue(lg->getRangeInverse());
				}
			}
		}
	}
	//	スポットライト
	for (int i = 0; i < _max_slight; i++) {
		lgt = getObj(_spot_light[i]);
		light_node = lgt->_node;
		if (light_node) {
			Light *lg = light_node->getLight();
			if (lg->getLightType() == gameplay::Light::SPOT) {
				// ライトの方向設定
				lightname_spotposition[20] = '0' + i;	// "u_spotLightPosition[0]"
				if (hasParameter(material, lightname_spotposition)) {
					material->getParameter(lightname_spotposition)->bindValue(light_node, &Node::getTranslationView);
				}
				lightname_spotdirection[21] = '0' + i;	// "u_spotLightDirection[0]"
				if (hasParameter(material, lightname_spotdirection)) {
					material->getParameter(lightname_spotdirection)->bindValue(light_node, &Node::getForwardVectorView);
				}
				// ライトの色設定
				lightname_spotcolor[17] = '0' + i;	// "u_spotLightColor[0]"
				if (hasParameter(material, lightname_spotcolor)) {
					material->getParameter(lightname_spotcolor)->bindValue(light_node, &Node::getLightColor);
					//material->getParameter(lightname_spotcolor)->setValue(lg->getColor());
				}
				// ライトの範囲
				lightname_spotrange[24] = '0' + i;	// "u_spotLightRangeInverse[0]"
				if (hasParameter(material, lightname_spotrange)) {
					material->getParameter(lightname_spotrange)->setValue(lg->getRangeInverse());
				}
				lightname_spotinner[25] = '0' + i;	// "u_spotLightInnerAngleCos[0]"
				if (hasParameter(material, lightname_spotinner)) {
					material->getParameter(lightname_spotinner)->setValue(lg->getInnerAngleCos());
				}
				lightname_spotouter[25] = '0' + i;	// "u_spotLightOuterAngleCos[0]"
				if (hasParameter(material, lightname_spotouter)) {
					material->getParameter(lightname_spotouter)->setValue(lg->getOuterAngleCos());
				}
			}
		}
	}

	Vector3 *vambient;
	lgt = getObj(_dir_light[0]);
	vambient = (Vector3 *)&lgt->_vec[GPOBJ_USERVEC_DIR];
	material->getParameter(lightname_ambient)->setValue(vambient);
}


void gamehsp::setMaterialDefaultBinding(Material* material)
{
	// These parameters are normally set in a .material file but this example sets them programmatically.
	// Bind the uniform "u_worldViewProjectionMatrix" to use the WORLD_VIEW_PROJECTION_MATRIX from the scene's active camera and the node that the model belongs to.

	if (hasParameter(material, "u_cameraPosition"))
		material->setParameterAutoBinding("u_cameraPosition", "CAMERA_WORLD_POSITION");
	if (hasParameter(material, "u_worldViewProjectionMatrix"))
		material->setParameterAutoBinding("u_worldViewProjectionMatrix", "WORLD_VIEW_PROJECTION_MATRIX");
	if (hasParameter(material, "u_inverseTransposeWorldViewMatrix"))
		material->setParameterAutoBinding("u_inverseTransposeWorldViewMatrix", "INVERSE_TRANSPOSE_WORLD_VIEW_MATRIX");
	if (hasParameter(material, "u_worldViewMatrix"))
		material->setParameterAutoBinding("u_worldViewMatrix", "WORLD_VIEW_MATRIX");
	if (hasParameter(material, "u_viewProjectionMatrix"))
		material->setParameterAutoBinding("u_viewProjectionMatrix", "VIEW_PROJECTION_MATRIX");
	if (hasParameter(material, "u_projectionMatrix"))
		material->setParameterAutoBinding("u_projectionMatrix", "PROJECTION_MATRIX");
	if (hasParameter(material, "u_viewMatrix"))
		material->setParameterAutoBinding("u_viewMatrix", "VIEW_MATRIX");
	if (hasParameter(material, "u_worldMatrix"))
		material->setParameterAutoBinding("u_worldMatrix", "WORLD_MATRIX");

	gameplay::MaterialParameter* prm_modalpha = material->getParameter("u_modulateAlpha");
	if (prm_modalpha) {
		prm_modalpha->setValue(1.0f);
	}
}


void gamehsp::setMaterialDefaultBinding(Material* material, int matopt)
{
	//	Render state set
	//
	RenderState::StateBlock* state;
	state = material->getStateBlock();
	if (state) {
		state->setCullFace(((matopt & GPOBJ_MATOPT_NOCULL) == 0));
		state->setDepthTest(((matopt & GPOBJ_MATOPT_NOZTEST) == 0));
		state->setDepthWrite(((matopt & GPOBJ_MATOPT_NOZWRITE) == 0));

		state->setBlend(true);
		if (matopt & GPOBJ_MATOPT_BLENDADD) {
			state->setBlendSrc(RenderState::BLEND_SRC_ALPHA);
			state->setBlendDst(RenderState::BLEND_ONE);
		}
		else {
			state->setBlendSrc(RenderState::BLEND_SRC_ALPHA);
			state->setBlendDst(RenderState::BLEND_ONE_MINUS_SRC_ALPHA);
		}
	}
}


void gamehsp::setMaterialDefaultBinding( Material* material, int icolor, int matopt )
{
	Vector4 color;

	//	シェーダーに必要なパラメーターを反映させる
	setMaterialDefaultBinding(material);

	//	カレントライトを反映させる
	setLightMaterialParameter(material);

	colorVector3( icolor, color );
	if ( hasParameter( material, "u_diffuseColor" ) )
		material->getParameter("u_diffuseColor")->setValue(color);

	setMaterialDefaultBinding(material, matopt);
}


float gamehsp::setMaterialBlend( Material* material, int gmode, int gfrate )
{
	//	プレンド描画設定
	//	gmdoe : HSPのgmode値
	//	gfrate : HSPのgfrate値
	//	(戻り値=alpha値(0.0～1.0))
	//
	RenderState::StateBlock *state;
	float alpha;

	state = material->getStateBlock();

    //ブレンドモード設定
    switch( gmode ) {
        case 0:                     //no blend
			state->setBlendSrc(RenderState::BLEND_ONE);
			state->setBlendDst(RenderState::BLEND_ZERO);
			alpha = 1.0f;
            break;
		case 1:                     //blend+alpha one
		case 2:                     //blend+alpha one
			state->setBlendSrc(RenderState::BLEND_SRC_ALPHA);
			state->setBlendDst(RenderState::BLEND_ONE_MINUS_SRC_ALPHA);
			alpha = 1.0f;
            break;
        case 5:                     //add
			state->setBlendSrc(RenderState::BLEND_SRC_ALPHA);
			state->setBlendDst(RenderState::BLEND_ONE);
			alpha = ((float)gfrate) * _colrate;
            break;
        case 6:                     //sub
			state->setBlendSrc(RenderState::BLEND_ONE_MINUS_DST_COLOR);
			state->setBlendDst(RenderState::BLEND_ZERO);
			alpha = ((float)gfrate) * _colrate;
            break;
        default:                    //normal blend
			state->setBlendSrc(RenderState::BLEND_SRC_ALPHA);
			state->setBlendDst(RenderState::BLEND_ONE_MINUS_SRC_ALPHA);
			alpha = ((float)gfrate) * _colrate;
            break;
    }
	return alpha;
}


int gamehsp::makeNewMat( Material* material, int mode, int color, int matopt )
{
	//	マテリアルを生成する
	gpmat *mat = addMat();
	if ( mat == NULL ) return -1;
	mat->_material = material;
	mat->_mode = mode;
	mat->_matcolor = color;
	mat->_matopt = matopt;
	mat->applyFilterMode(1);
	return mat->_id;
}


int gamehsp::makeNewMatFromObj(int objid, int part, char *nodename)
{
	//	オブジェクト固有のマテリアルを参照する
	gpobj *obj = getObj(objid);
	if (obj == NULL) return -1;

	Model* model;
	Material* material;

	if (*nodename == 0) {
		if (obj->_model == NULL) return -1;
		model = obj->_model;
		if (model == NULL) return -1;
		if (model->getMeshPartCount() == 0) {
			material = model->getMaterial();
		}
		else {
			material = model->getMaterial(part);
		}
	}
	else {
		Node* node = getNodeFromName(objid, nodename);
		Drawable* drawable = node->getDrawable();
		if (drawable == NULL) return -1;
		Model* model = dynamic_cast<Model*>(drawable);
		if (model->getMeshPartCount() == 0) {
			material = model->getMaterial();
		}
		else {
			material = model->getMaterial(part);
		}
	}
	if (material == NULL) return -1;

	gpmat *mat = addMat();
	if (mat == NULL) return -1;

	mat->_material = material;
	mat->_mode = GPMAT_MODE_PROXY;
	return mat->_id;
}


int gamehsp::makeNewMat2D( char *fname, int matopt )
{
	//	マテリアルを生成する(2D)
	gpmat *mat = addMat();
	if ( mat == NULL ) return -1;

	Texture* texture = Texture::create(fname);
	if (texture == NULL) {
		Alertf("Texture not found.(%s)", fname);
		return -1;
	}

	Material *mesh_material = makeMaterialTex2D(texture, matopt);
	if ( mesh_material == NULL ) return -1;
	SAFE_RELEASE(texture);

    VertexFormat::Element elements[] =
    {
        VertexFormat::Element(VertexFormat::POSITION, 3),
        VertexFormat::Element(VertexFormat::TEXCOORD0, 2),
        VertexFormat::Element(VertexFormat::COLOR, 4)
    };

	unsigned int elementCount = sizeof(elements) / sizeof(VertexFormat::Element);
	MeshBatch *meshBatch = MeshBatch::create(VertexFormat(elements, elementCount), Mesh::TRIANGLE_STRIP, mesh_material, true, 16, 256 );

	mat->_mesh = meshBatch;
	mat->_material = mesh_material;
	mat->_mode = GPMAT_MODE_2D;
	mat->_sx = _tex_width;
	mat->_sy = _tex_height;
	mat->_texratex = 1.0f / (float)_tex_width;
	mat->_texratey = 1.0f / (float)_tex_height;

	// 2D用のプロジェクション
	make2DRenderProjection(&mat->_projectionMatrix2D, _tex_width, _tex_height);
	mat->_target_material_id = -1;
	mat->_matcolor = -1;
	mat->_matopt = matopt;
	mat->applyFilterMode(0);

	return mat->_id;
}


char* gamehsp::getPixelMaskBuffer(char* fname, int* xsize, int* ysize)
{
	//		画像のピクセルバッファを取得する
	//		(画像ファイルのポインタを渡すと、αチャンルを2値化したバッファを返す)
	//
	Image* image = Image::create(fname);
	if (image == NULL) {
		Alertf("Texture not found.(%s)", fname);
		return NULL;
	}
	int sx = (int)image->getWidth();
	int sy = (int)image->getHeight();
	int bwsize = sx * sy;
	char* mem = (char*)malloc(bwsize);

	char* p = (char *)image->getData();		// 転送先のサーフェイスの始点(32bit)
	int i;
	char* src = mem;
	for (i = 0; i < bwsize; i++) {
		*src++ = p[3];
		p += 4;
	}
	*xsize = sx;
	*ysize = sy;

	SAFE_RELEASE(image);
	return NULL;
}


int gamehsp::makeNewMatFromFB(gameplay::FrameBuffer *fb, int matopt)
{
	int tex_width, tex_height;
	Texture* texture;

	if (fb == NULL) return -1;

	gpmat *mat = addMat();
	if (mat == NULL) return -1;

	RenderTarget *target = fb->getRenderTarget();
	if (target == NULL) return -1;
	texture = target->getTexture();

	Material *mesh_material = makeMaterialTex2D(texture, matopt);
	if (mesh_material == NULL) return -1;

	tex_width = (int)fb->getWidth();
	tex_height = (int)fb->getHeight();

	VertexFormat::Element elements[] =
	{
		VertexFormat::Element(VertexFormat::POSITION, 3),
		VertexFormat::Element(VertexFormat::TEXCOORD0, 2),
		VertexFormat::Element(VertexFormat::COLOR, 4)
	};

	unsigned int elementCount = sizeof(elements) / sizeof(VertexFormat::Element);
	MeshBatch *meshBatch = MeshBatch::create(VertexFormat(elements, elementCount), Mesh::TRIANGLE_STRIP, mesh_material, true, 16, 256);

	mat->_mesh = meshBatch;
	mat->_material = mesh_material;
	mat->_mode = GPMAT_MODE_2D;
	mat->_sx = tex_width;
	mat->_sy = tex_height;
	mat->_texratex = 1.0f / (float)tex_width;
	mat->_texratey = 1.0f / (float)tex_height;

	// 2D用のプロジェクション
	make2DRenderProjection(&mat->_projectionMatrix2D, tex_width, tex_height);
	mat->_target_material_id = -1;
	mat->_matcolor = -1;
	mat->_matopt = matopt;

	return mat->_id;
}


Material *gamehsp::makeMaterialFromShader(char *vshd, char *fshd, char *defs)
{
	Material *material;
	material = Material::create( vshd, fshd, defs );

	if ( material == NULL ) {
		return NULL;
	}
	return material;
}


void gamehsp::setupDefines(void)
{
	// カスタムシェーダーの初期化
	user_vsh = SPRITE_VSH;
	user_fsh = SPRITE_FSH;
	user_defines = "";

	// ライトのシェーダーパラメーター
	strcpy(lightname_ambient, PARAMNAME_LIGHT_AMBIENT);
	strcpy(lightname_color, PARAMNAME_LIGHT_COLOR);
	strcpy(lightname_direction, PARAMNAME_LIGHT_DIRECTION);

	strcpy(lightname_pointcolor, PARAMNAME_LIGHT_POINTCOLOR);
	strcpy(lightname_pointposition, PARAMNAME_LIGHT_POINTPOSITION);
	strcpy(lightname_pointrange, PARAMNAME_LIGHT_POINTRANGE);

	strcpy(lightname_spotcolor, PARAMNAME_LIGHT_SPOTCOLOR);
	strcpy(lightname_spotposition, PARAMNAME_LIGHT_SPOTPOSITION);
	strcpy(lightname_spotdirection, PARAMNAME_LIGHT_SPOTDIRECTION);
	strcpy(lightname_spotrange, PARAMNAME_LIGHT_SPOTRANGE);
	strcpy(lightname_spotinner, PARAMNAME_LIGHT_SPOTINNER);
	strcpy(lightname_spotouter, PARAMNAME_LIGHT_SPOTOUTER);

}


void gamehsp::setUserShader2D(char *vsh, char *fsh, char *defines)
{
	// カスタムシェーダーの設定
	user_vsh = vsh;
	user_fsh = fsh;
	user_defines = defines;
}


Material *gamehsp::makeMaterialColor( int color, int matopt )
{
	Material *material;
	char *defs;
	if (matopt & GPOBJ_MATOPT_NOLIGHT){
		defs = this->getNoLightDefines();
	}
	else {
		if (matopt & GPOBJ_MATOPT_SPECULAR) {
			defs = this->getSpecularLightDefines();
		}
		else {
			defs = this->getLightDefines();
		}
	}
	material = makeMaterialFromShader("res/shaders/colored.vert", "res/shaders/colored.frag",defs);
	if ( material == NULL ) return NULL;

	setMaterialDefaultBinding( material, color, matopt );
	return material;
}


Material *gamehsp::makeMaterialTexture( char *fname, int matopt, Texture *opttex )
{
	Material *material;
	bool mipmap, cubemap;
	char *defs;
	char extradefs[4096];
	mipmap = (matopt & GPOBJ_MATOPT_NOMIPMAP) == 0;
	cubemap = (matopt & GPOBJ_MATOPT_CUBEMAP) != 0;

	if (matopt & GPOBJ_MATOPT_NOLIGHT){
		defs = this->getNoLightDefines();
	}
	else {
		if (matopt & GPOBJ_MATOPT_SPECULAR) {
			defs = this->getSpecularLightDefines();
		}
		else {
			defs = this->getLightDefines();
		}
	}
	if (matopt & GPOBJ_MATOPT_MIRROR) {
		strcpy(extradefs,defs);
		strcat(extradefs,";MIRRORTEX");
		defs = extradefs;
	}
	if (matopt & GPOBJ_MATOPT_NODISCARD) {
		strcpy(extradefs, defs);
		strcat(extradefs, ";TEXTURE_NODISCARD_ALPHA");
		defs = extradefs;
	}
	if (matopt & GPOBJ_MATOPT_UVOFFSET) {
		strcpy(extradefs, defs);
		strcat(extradefs, ";TEXTURE_OFFSET");
		defs = extradefs;
	}
	if (matopt & GPOBJ_MATOPT_UVREPEAT) {
		strcpy(extradefs, defs);
		strcat(extradefs, ";TEXTURE_REPEAT");
		defs = extradefs;
	}

	material = makeMaterialFromShader("res/shaders/textured.vert", "res/shaders/textured.frag", defs);
	if ( material == NULL ) return NULL;

	setMaterialDefaultBinding( material, -1, matopt );

	if (matopt & GPOBJ_MATOPT_USERBUFFER) {
		MaterialParameter *mp = material->getParameter("u_diffuseTexture");
        if (mp) {
            if (opttex) {
                Texture::Sampler* sampler = Texture::Sampler::create(opttex);
                if (sampler) {
                    mp->setValue(sampler);
					SAFE_RELEASE(sampler);
                    return material;
                }
            }
        }
		return NULL;
	}

    MaterialParameter *mp2 = material->getParameter("u_diffuseTexture");
    if (mp2) {
        mp2->setValue( fname, mipmap, cubemap );
    }
	return material;
}


Material *gamehsp::makeMaterialTex2D(Texture *texture, int matopt)
{
	bool mipmap;
	mipmap = (matopt & GPOBJ_MATOPT_NOMIPMAP ) == 0;

	if (texture == NULL) return NULL;
	_tex_width = texture->getWidth();
	_tex_height = texture->getHeight();

    // Search for the first sampler uniform in the effect.
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
    if (!samplerUniform)
    {
        GP_ERROR("No uniform of type GL_SAMPLER_2D found in sprite effect.");
        return NULL;
    }

	RenderState::StateBlock *state;
	Material* mesh_material = NULL;

	if (matopt & GPOBJ_MATOPT_USERSHADER) {
		// User custom shader
		//mesh_material = Material::create(SPRITE_VSH, SPRITE_FSH, NULL);
		mesh_material = Material::create(user_vsh.c_str(), user_fsh.c_str(), user_defines.c_str());
	}
	else {
		// Wrap the effect in a material
		mesh_material = Material::create(_spriteEffect); // +ref effect
	}
	if (mesh_material == NULL) {
		GP_ERROR("Can't create material for sprite.");
		return NULL;
	}

    // Bind the texture to the material as a sampler
    Texture::Sampler* sampler = Texture::Sampler::create(texture); // +ref texture

	sampler->setFilterMode(Texture::Filter::NEAREST, Texture::Filter::NEAREST);		// 2Dはデフォルトでフィルタなし
	mesh_material->getParameter(samplerUniform->getName())->setValue(sampler);
	SAFE_RELEASE(sampler);

	update2DRenderProjection(mesh_material, &_projectionMatrix2D);

	state = mesh_material->getStateBlock();

	state->setCullFace(false);
	state->setDepthTest(false);
	state->setDepthWrite(false);
	state->setBlend(true);
	state->setBlendSrc(RenderState::BLEND_SRC_ALPHA);
	state->setBlendDst(RenderState::BLEND_ONE_MINUS_SRC_ALPHA);

	return mesh_material;
}


int gamehsp::getTextureWidth( void )
{
	return _tex_width;
}


int gamehsp::getTextureHeight( void )
{
	return _tex_height;
}

bool hasParameter( Material* material, const char* name )
{
	unsigned int mc = material->getTechniqueCount();
	for (unsigned int i = 0; i < mc; ++i)
	{
		Technique *tech = material->getTechniqueByIndex( i );
		unsigned int pc = tech->getPassCount();
		for (unsigned int j = 0; j < pc; ++j)
		{
			Pass *pass = tech->getPassByIndex(j);
			Effect *effect = pass->getEffect();
			if (effect->getUniform( name ) != NULL)
				return true;
		}
	}
	return false;
}


int gamehsp::applySamplerModeByString(Texture::Sampler* sampler, char* name, char* value)
{
	if (sampler == NULL) return -1;
	strcase(name); strcase(value);
	if (strcmp(name, "filter") == 0) {
		if (strcmp(value, "nearest") == 0) {
			sampler->setFilterMode(Texture::Filter::NEAREST, Texture::Filter::NEAREST);
			return 0;
		}
		if (strcmp(value, "linear") == 0) {
			sampler->setFilterMode(Texture::Filter::LINEAR, Texture::Filter::LINEAR);
			return 0;
		}
		return -2;
	}
	if (strcmp(name, "wrap") == 0) {
		if (strcmp(value, "repeat") == 0) {
			sampler->setWrapMode(Texture::Wrap::REPEAT, Texture::Wrap::REPEAT);
			return 0;
		}
		if (strcmp(value, "clamp") == 0) {
			sampler->setWrapMode(Texture::Wrap::CLAMP, Texture::Wrap::CLAMP);
			return 0;
		}
		return -2;
	}
	return -1;
}


