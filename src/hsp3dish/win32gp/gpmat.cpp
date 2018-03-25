
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
}

gpmat::~gpmat()
{
}

void gpmat::reset( gamehsp *owner, int id )
{
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
}


int gpmat::setParameter( char *name, Vector4 *value )
{
	if ( _material == NULL ) return -1;
	_material->getParameter(name)->setValue(*value);

	return 0;
}


int gpmat::setParameter( char *name, Vector3 *value )
{
	if ( _material == NULL ) return -1;
    _material->getParameter( name )->setValue( *value );

	return 0;
}


int gpmat::setParameter( char *name, float value )
{
	if ( _material == NULL ) return -1;
    _material->getParameter( name )->setValue( value );

	return 0;
}


int gpmat::setParameter(char *name, const Matrix *value, int count)
{
	if (_material == NULL) return -1;
	_material->getParameter(name)->setValue(value,count);

	return 0;
}


int gpmat::setParameter(char *name, char *fname, int matopt)
{
	bool mipmap;
	if (_material == NULL) return -1;
	mipmap = (matopt & GPOBJ_MATOPT_NOMIPMAP) == 0;
	_material->getParameter(name)->setValue( fname, mipmap);

	return 0;
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
	MaterialParameter *mprm = _material->getParameter("u_texture");
	if (mprm == NULL) return;
	Texture::Sampler *sampler = mprm->getSampler();
	if (sampler == NULL) return;
	sampler->setFilterMode(value, value);
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


int gpobj::setParameter(char *name, float value, int part)
{
	if (_model == NULL) return -1;
	Material *material = _model->getMaterial(part);
	if (material == NULL) return -1;
	material->getParameter(name)->setValue(value);

	return 0;
}


int gpobj::setParameter(char *name, const Matrix *value, int count, int part)
{
	if (_model == NULL) return -1;
	Material *material = _model->getMaterial(part);
	if (material == NULL) return -1;
	material->getParameter(name)->setValue(value, count);

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

	MaterialParameter *mprm = material->getParameter("u_texture");
	if (mprm == NULL) return;
	Texture::Sampler *sampler = mprm->getSampler();
	if (sampler == NULL) return;
	sampler->setFilterMode(value, value);
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
    SAFE_RELEASE( mat->_material );
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

void gamehsp::setMaterialDefaultBinding( Material* material, int icolor, int matopt )
{
	// These parameters are normally set in a .material file but this example sets them programmatically.
    // Bind the uniform "u_worldViewProjectionMatrix" to use the WORLD_VIEW_PROJECTION_MATRIX from the scene's active camera and the node that the model belongs to.

//	material->getParameter("u_worldViewProjectionMatrix")->setValue( _camera->getWorldViewProjectionMatrix() );
//	material->getParameter("u_inverseTransposeWorldViewMatrix")->setValue( _camera->getInverseTransposeWorldViewMatrix() );
//	material->getParameter("u_cameraPosition")->setValue( _camera->getTranslation() );

//	material->getParameter("u_worldViewProjectionMatrix")->bindValue( _camera, &Node::getWorldViewProjectionMatrix );
//	material->getParameter("u_inverseTransposeWorldViewMatrix")->bindValue( _camera, &Node::getInverseTransposeWorldViewMatrix );
//	material->getParameter("u_cameraPosition")->bindValue( _camera, &Node::getTranslation );

	if ( hasParameter( material, "u_cameraPosition" ) )
		material->setParameterAutoBinding("u_cameraPosition", "CAMERA_WORLD_POSITION");
	if ( hasParameter( material, "u_worldViewProjectionMatrix" ) )
		material->setParameterAutoBinding("u_worldViewProjectionMatrix", "WORLD_VIEW_PROJECTION_MATRIX");
	if ( hasParameter( material, "u_inverseTransposeWorldViewMatrix" ) )
		material->setParameterAutoBinding("u_inverseTransposeWorldViewMatrix", "INVERSE_TRANSPOSE_WORLD_VIEW_MATRIX");

	Vector4 color;
	Node *light_node;

	//	カレントライトを反映させる
	gpobj *lgt;
	lgt = getObj(_curlight);
	light_node = lgt->_node;
	// ライトの方向設定
	if (hasParameter(material, "u_directionalLightDirection[0]"))
		material->getParameter("u_directionalLightDirection[0]")->bindValue(light_node, &Node::getForwardVectorView);
	// ライトの色設定
	// (リアルタイムに変更を反映させる場合は再設定が必要。現在は未対応)
	Vector3 *vambient;
	vambient = (Vector3 *)&lgt->_vec[GPOBJ_USERVEC_DIR];
	if (hasParameter(material, "u_directionalLightColor[0]"))
		material->getParameter("u_directionalLightColor[0]")->setValue(light_node->getLight()->getColor());
	if (hasParameter(material, "u_ambientColor"))
		material->getParameter("u_ambientColor")->setValue(vambient);

	//material->setParameterAutoBinding("u_ambientColor", "SCENE_AMBIENT_COLOR");
	//material->setParameterAutoBinding("u_lightDirection", "SCENE_LIGHT_DIRECTION");
	//material->setParameterAutoBinding("u_lightColor", "SCENE_LIGHT_COLOR");

	colorVector3( icolor, color );
	if ( hasParameter( material, "u_diffuseColor" ) )
		material->getParameter("u_diffuseColor")->setValue(color);

	gameplay::MaterialParameter *prm_modalpha;
	if ( hasParameter( material, "u_modulateAlpha" ) )
		prm_modalpha = material->getParameter("u_modulateAlpha");
	if ( prm_modalpha ) { prm_modalpha->setValue( 1.0f ); }

	RenderState::StateBlock *state;
	state = material->getStateBlock();

	state->setCullFace( (( matopt & GPOBJ_MATOPT_NOCULL )==0) );
	state->setDepthTest( (( matopt & GPOBJ_MATOPT_NOZTEST )==0) );
	state->setDepthWrite( (( matopt & GPOBJ_MATOPT_NOZWRITE )==0) );

	state->setBlend(true);
	if (matopt & GPOBJ_MATOPT_BLENDADD) {
		state->setBlendSrc(RenderState::BLEND_SRC_ALPHA);
		state->setBlendDst(RenderState::BLEND_ONE);
	} else {
		state->setBlendSrc(RenderState::BLEND_SRC_ALPHA);
		state->setBlendDst(RenderState::BLEND_ONE_MINUS_SRC_ALPHA);
	}

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
	return mat->_id;
}


int gamehsp::makeNewMatFromObj(int objid, int part)
{
	//	オブジェクト固有のマテリアルを参照する
	gpobj *obj = getObj(objid);
	if (obj == NULL) return -1;
	if (obj->_model == NULL) return -1;

	gpmat *mat = addMat();
	if (mat == NULL) return -1;

	mat->_material = obj->_model->getMaterial(part);
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

	return mat->_id;
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
	//	シェーダー用のdefine定義を作成
	//
	char tmp[8];
	light_defines = "MODULATE_ALPHA";
	nolight_defines = "MODULATE_ALPHA";
	if (_max_dlight) {
		light_defines += ";DIRECTIONAL_LIGHT_COUNT ";
		sprintf(tmp,"%d",_max_dlight);
		//itoa(_max_dlight,tmp,10);
		light_defines += tmp;// std::to_string(_max_dlight);
	}
	if (_max_plight) {
		light_defines += ";POINT_LIGHT_COUNT ";
		sprintf(tmp,"%d",_max_plight);
		//itoa(_max_plight, tmp, 10);
		light_defines += tmp;// std::to_string(_max_plight);
	}
	if (_max_slight) {
		light_defines += ";SPOT_LIGHT_COUNT ";
		sprintf(tmp,"%d",_max_slight);
		//itoa(_max_slight, tmp, 10);
		light_defines += tmp;// std::to_string(_max_slight);
	}
	splight_defines = light_defines + ";SPECULAR";

	// カスタムシェーダーの初期化
	user_vsh = SPRITE_VSH;
	user_fsh = SPRITE_FSH;
	user_defines = "";
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


Material *gamehsp::makeMaterialTexture( char *fname, int matopt )
{
	Material *material;
	bool mipmap;
	char *defs;
	mipmap = (matopt & GPOBJ_MATOPT_NOMIPMAP) == 0;

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

	material = makeMaterialFromShader( "res/shaders/textured.vert", "res/shaders/textured.frag", defs );
	if ( material == NULL ) return NULL;

	setMaterialDefaultBinding( material, -1, matopt );
	material->getParameter("u_diffuseTexture")->setValue( fname, mipmap );
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
    mesh_material->getParameter(samplerUniform->getName())->setValue(sampler);

	/*
	Material* mesh_material = Material::create( SPRITE_VSH, SPRITE_FSH, NULL );
	if ( mesh_material == NULL ) {
        GP_ERROR("2D initalize failed.");
		return NULL;
	}
	mesh_material->getParameter("u_diffuseTexture")->setValue( fname, mipmap );
	*/

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

