#ifndef gamehsp_H_
#define gamehsp_H_

#include "gameplay.h"
#include "gpmat.h"
#include "gplgt.h"
#include "gpcam.h"
#include "gpphy.h"
#include "gpevent.h"
#include "../texmes.h"

using namespace gameplay;

//#define USE_GPBFONT

/**
 * Main game class.
 */
#define GPPRM_FPS 0
#define GPPRM_VSYNC 1
#define GPPRM_MAX 2

#define GPOBJ_FLAG_NONE (0)
#define GPOBJ_FLAG_ENTRY (1)

#define GPOBJ_MODE_HIDE (1)
#define GPOBJ_MODE_CLIP (2)
#define GPOBJ_MODE_XFRONT (4)
#define GPOBJ_MODE_WIRE (8)
#define GPOBJ_MODE_MOVE (32)
#define GPOBJ_MODE_FLIP (64)
#define GPOBJ_MODE_BORDER (0x80)
#define GPOBJ_MODE_2D (0x100)
#define GPOBJ_MODE_TIMER (0x200)
#define GPOBJ_MODE_SORT (0x400)
#define GPOBJ_MODE_LATE (0x4000)

#define GPOBJ_MODE_BHIDE (0x8000)

#define GPOBJ_ID_MATFLAG  (0x200000)
#define GPOBJ_ID_SRCFLAG  (0x400000)
#define GPOBJ_ID_FLAGBIT (0xff00000)
#define GPOBJ_ID_FLAGMASK (0x0fffff)

#define GPOBJ_ID_EXFLAG (0x100000)
#define GPOBJ_ID_SCENE  (0x100001)
#define GPOBJ_ID_CAMERA (0x100002)
#define GPOBJ_ID_LIGHT  (0x100003)
#define GPOBJ_ID_TOUCHNODE  (0x100004)

#define GPOBJ_SHAPE_NONE (-1)
#define GPOBJ_SHAPE_MODEL (0)
#define GPOBJ_SHAPE_BOX (1)
#define GPOBJ_SHAPE_FLOOR (2)
#define GPOBJ_SHAPE_PLATE (3)
#define GPOBJ_SHAPE_MESH (4)
#define GPOBJ_SHAPE_SPRITE (16)

enum {
MOC_POS = 0,
MOC_QUAT,
MOC_SCALE,
MOC_DIR,
MOC_EFX,
MOC_COLOR,
MOC_WORK,
MOC_WORK2,
MOC_AXANG,
MOC_ANGX,
MOC_ANGY,
MOC_ANGZ,
MOC_FORWARD,
MOC_MAX
};

enum {
GPOBJ_USERVEC_DIR = 0,
GPOBJ_USERVEC_COLOR,
GPOBJ_USERVEC_WORK,
GPOBJ_USERVEC_WORK2,
GPOBJ_USERVEC_MAX
};

enum {
GPPSET_ENABLE = 0,
GPPSET_FRICTION,
GPPSET_DAMPING,
GPPSET_KINEMATIC,
GPPSET_ANISOTROPIC_FRICTION,
GPPSET_GRAVITY,
GPPSET_LINEAR_FACTOR,
GPPSET_ANGULAR_FACTOR,
GPPSET_ANGULAR_VELOCITY,
GPPSET_LINEAR_VELOCITY,
GPPSET_MASS_CENTER,
GPPSET_MAX
};

#define GPOBJ_PRM_ID_NONE (0)
#define GPOBJ_PRM_ID_SPR (0x100)
#define GPOBJ_PRM_ID_CLOG (0x200)

#define GPOBJ_MARK_UPDATE_POS (0x100)
#define GPOBJ_MARK_UPDATE_ANG (0x200)
#define GPOBJ_MARK_UPDATE_SCALE (0x400)

#define GPOBJ_MATOPT_NOLIGHT (1)
#define GPOBJ_MATOPT_NOMIPMAP (2)
#define GPOBJ_MATOPT_NOCULL (4)
#define GPOBJ_MATOPT_NOZTEST (8)
#define GPOBJ_MATOPT_NOZWRITE (16)
#define GPOBJ_MATOPT_BLENDADD (32)
#define GPOBJ_MATOPT_SPECULAR (64)
#define GPOBJ_MATOPT_USERSHADER (128)
#define GPOBJ_MATOPT_USERBUFFER (256)
#define GPOBJ_MATOPT_MIRROR (512)
#define GPOBJ_MATOPT_CUBEMAP (1024)
#define GPOBJ_MATOPT_NODISCARD (2048)
#define GPOBJ_MATOPT_UVOFFSET (4096)
#define GPOBJ_MATOPT_UVREPEAT (8192)

#define GPDRAW_OPT_OBJUPDATE (1)
#define GPDRAW_OPT_DRAWSCENE (2)
#define GPDRAW_OPT_DRAW2D (4)
#define GPDRAW_OPT_DRAWSCENE_LATE (8)
#define GPDRAW_OPT_DRAW2D_LATE (16)
#define GPDRAW_OPT_PHYDEBUG (0x10000)

#define GPANIM_OPT_START_FRAME (0)
#define GPANIM_OPT_END_FRAME (1)
#define GPANIM_OPT_DURATION (2)
#define GPANIM_OPT_ELAPSED (3)
#define GPANIM_OPT_BLEND (4)
#define GPANIM_OPT_PLAYING (5)
#define GPANIM_OPT_SPEED (6)

#define GPOBJ_MULTIEVENT_MAX 4

#define GPNODEINFO_NODE (0)
#define GPNODEINFO_MODEL (1)
#define GPNODEINFO_MATNUM (2)
#define GPNODEINFO_MATERIAL (0x80)
#define GPNODEINFO_NAME (0x100)
#define GPNODEINFO_CHILD (0x101)
#define GPNODEINFO_SIBLING (0x102)
#define GPNODEINFO_SKINROOT (0x103)
#define GPNODEINFO_MATNAME (0x10000)

#define GPOBJ_PRMMETHOD_SET (0)
#define GPOBJ_PRMMETHOD_ON (1)
#define GPOBJ_PRMMETHOD_OFF (2)

#define GPOBJ_PRMSET_FLAG (2)
#define GPOBJ_PRMSET_MODE (3)
#define GPOBJ_PRMSET_ID (4)
#define GPOBJ_PRMSET_TIMER (5)
#define GPOBJ_PRMSET_MYGROUP (6)
#define GPOBJ_PRMSET_COLGROUP (7)
#define GPOBJ_PRMSET_SHAPE (8)
#define GPOBJ_PRMSET_USEGPMAT (9)
#define GPOBJ_PRMSET_USEGPPHY (10)
#define GPOBJ_PRMSET_COLILOG (11)
#define GPOBJ_PRMSET_ALPHA (12)
#define GPOBJ_PRMSET_FADE (13)
#define GPOBJ_PRMSET_SPRID (0x100)
#define GPOBJ_PRMSET_SPRCELID (0x101)
#define GPOBJ_PRMSET_SPRGMODE (0x102)


//  HGIMG4 Sprite Object
class gpspr {
public:
	gpspr();
	~gpspr();
	void reset( int id, int celid, int gmode, void *bmscr );
	int getDistanceHit(Vector3* v, float size);
	float getDistance(Vector3* v);

	int _id;							// 親オブジェクトID
	int _celid;							// 表示セルID
	int _gmode;							// gmode値
	void *_bmscr;						// 参照元bmscrポインタ
	Vector4 _pos;						// 位置
	Vector4 _ang;						// 回転角度
	Vector4 _scale;						// スケール
};


//  HGIMG4 Node Object
class gpobj : public Ref {
public:
	gpobj();
	~gpobj();
	void reset( int id );				// 初期化
	bool isAlive(void) { return (_flag != 0); }				// 有効か調べる
	bool isVisible( void );				// 表示できるか調べる
	bool isVisible( bool lateflag );	// 表示できるか調べる(lateflagあり)
	float getAlphaRate( void );			// Alpha値を取得する
	void updateParameter( Material *mat );	// 後処理
	int executeFade(void);				// フェード処理

	int GetEmptyEvent(void);
	void DeleteEvent(int entry);
	gpevent *GetEvent(int entry);
	void SetEvent(gpevent *ev, int entry);
	void StartEvent(gpevent *ev, int entry);

	int setParameter(char *name, float value, int part);
	int setParameter(char* name, float value, float value2, int part);
	int setParameter(char *name, Vector3 *value, int part);
	int setParameter(char *name, Vector4 *value, int part);
	int setParameter(char *name, char *fname, int matopt, int part);
	int setParameter(char* name, double* p_mat, int count, int part);
	int setParameter(char* name, Texture::Sampler* sampler, int part);
	int setState(char *name, char *value, int part);
	void setFilter(Texture::Filter value, int part);
	Texture::Sampler* getSamplerByName(char* name, int part);

	short _flag;						// 存在フラグ
	short _mark;						// マーク処理用
	int _mode;							// モード(GPOBJ_MODE_*)
	int _id;							// ノードオブジェクトID
	int _timer;							// タイマー値
	int	_mygroup;						// 自分のコリジョングループ
	int	_colgroup;						// 対象のコリジョングループ
	int _shape;							// 生成された形状
	int _usegpmat;						// gpmat使用時のID(-1=固有Material)
	int _usegpphy;						// gpphy使用時のID
	int _colilog;						// 衝突ログID
	int	_transparent;					// 透明度(0=透明/255=不透明)
	int _fade;							// フェード設定(0=なし/+-で増減)
	int _rendergroup;					// レンダリンググループ
	int _lightgroup;					// ライティンググループ
	int _lighthash;						// ライティングハッシュ値

	gpspr *_spr;						// 生成された2Dスプライト情報
	gpphy *_phy;						// 生成されたコリジョン情報
	Node *_node;						// 生成されたNode
	Model *_model;						// 生成されたModel
	Camera *_camera;					// 生成されたCamera
	Light *_light;						// 生成されたLight
	Animation *_animation;				// 生成されたAnimation
	Vector3 _sizevec;					// 生成されたサイズパラメーター
	Vector4 _vec[GPOBJ_USERVEC_MAX];	// ワーク用ベクター
	Vector3 _evvec[MOC_MAX];			// イベントワーク用ベクター

	gameplay::MaterialParameter *_prm_modalpha;	// Alphaモジュレート用パラメーター

	gpevent *_event[GPOBJ_MULTIEVENT_MAX];		// Event List
	float	_time[GPOBJ_MULTIEVENT_MAX];		// Event Time
	//Vector4 _evvec[GPOBJ_MULTIEVENT_MAX];		// イベントワーク用ベクター
	gameplay::Matrix* _matbuffer;				// マトリクス保持用バッファ

};

//	original HitFilter
class myPhysicsFilter : public PhysicsController::HitFilter {
public:
	myPhysicsFilter();
	~myPhysicsFilter();
	bool filter(PhysicsCollisionObject* object);
	bool hit(const PhysicsController::HitResult& result);
};

#define BUFSIZE_POLYCOLOR 32
#define BUFSIZE_POLYTEX 64
#define BUFSIZE_MULTILIGHT 10

#define DEFAULT_ANIM_CLIPNAME "_idle"

#define PARAMNAME_LIGHT_MAXSIZE 32
#define PARAMNAME_LIGHT_DIRECTION "u_directionalLightDirection[0]"
#define PARAMNAME_LIGHT_COLOR "u_directionalLightColor[0]"
#define PARAMNAME_LIGHT_AMBIENT "u_ambientColor"
#define PARAMNAME_LIGHT_POINTCOLOR "u_pointLightColor[0]"
#define PARAMNAME_LIGHT_POINTPOSITION "u_pointLightPosition[0]"
#define PARAMNAME_LIGHT_POINTRANGE "u_pointLightRangeInverse[0]"

#define PARAMNAME_LIGHT_SPOTCOLOR "u_spotLightColor[0]"
#define PARAMNAME_LIGHT_SPOTPOSITION "u_spotLightPosition[0]"
#define PARAMNAME_LIGHT_SPOTDIRECTION "u_spotLightDirection[0]"
#define PARAMNAME_LIGHT_SPOTRANGE "u_spotLightRangeInverse[0]"
#define PARAMNAME_LIGHT_SPOTINNER "u_spotLightInnerAngleCos[0]"
#define PARAMNAME_LIGHT_SPOTOUTER "u_spotLightOuterAngleCos[0]"

#define LIGHT_OPT_NORMAL (0)
#define LIGHT_OPT_POINT (1)
#define LIGHT_OPT_SPOT (2)

#define DEFAULT_GPB_FILEEXT ".gpb"
#define DEFAULT_MATERIAL_FILEEXT ".material"
#define DEFAULT_PHYSISCS_FILEEXT ".physics"

//	gamehsp Object
class gamehsp: public Game
{
public:

    /**
     * Constructor.
     */
	gamehsp();
	~gamehsp();

    /**
     * @see Game::keyEvent
     */
	void keyEvent(Keyboard::KeyEvent evt, int key);
	
    /**
     * @see Game::touchEvent
     */
    void touchEvent(Touch::TouchEvent evt, int x, int y, unsigned int contactIndex);

	/**
	* for PassCallback
	*/
	static std::string passCallback(Pass* pass, void* cookie, const char *defs);
	class CollisionCallback : public btCollisionWorld::ContactResultCallback
	{
	public:
		/**
		 * Constructor.
		 *
		 * @param pc The physics controller that owns the callback.
		 */
		CollisionCallback(PhysicsController* pc) : _pc(pc) {
		}

	protected:
		/**
			* Internal function used for Bullet integration (do not use or override).
			*/
		btScalar addSingleResult(btManifoldPoint& cp, const btCollisionObjectWrapper* a, int partIdA, int indexA, const btCollisionObjectWrapper* b, int partIdB, int indexB);

	private:
		PhysicsController* _pc;
	};

	/**
	* for free mesh vertex
	*/
	class FreeMeshVertex
	{
	public:
		FreeMeshVertex();
		~FreeMeshVertex();
		float x, y, z;		// vertex
		float nx, ny, nz;	// normal
		float u, v;			// UV
	};

	/*
		HSP Support Functions
	*/
	void resetScreen( int opt=0 );
	void deleteAll( void );
	void deleteObjectID( int id );
	int setObjectPool( int startid, int num );

	void hookSetSysReq( int reqid, int value );
	void hookGetSysReq( int reqid );

	void updateViewport( int x, int y, int w, int h );
	void updateScaledViewport( int x, int y, int w, int h );
	void setViewInfo( int orgx, int orgy, float scalex, float scaley );

	gpobj *getObj( int id );
	gpobj *getSceneObj(int id);
	int deleteObj( int id );
	gpobj *addObj( void );
	Node *getNode( int objid );
	Light *getLight( int lgtid );
	Camera *getCamera( int camid );
	Drawable *getDrawable(Node *node);
	int setObjName( int objid, char *name );
	char *getObjName( int objid );
	int *getObjectPrmPtr( int objid, int prmid );
	int getObjectPrm( int objid, int prmid, int *outptr );
	int setObjectPrm( int objid, int prmid, int value, int method = 0);
	int setObjLight( int objid );

	char *getAnimId(int objid, int index, int option);
	int getAnimPrm(int objid, int index, int option, int* res);
	int getAnimPrmFloat(int objid, int index, int option, float* res);
	int setAnimPrm(int objid, int index, int option, int value);
	int setAnimPrmFloat(int objid, int index, int option, float value);
	int addAnimId(int objid, char *name, int start, int end, int option);
	int playAnimId(int objid, char *name, int option);

	gpmat *getMat( int id );
	int deleteMat( int id );
	gpmat *addMat( void );
	Material *getMaterial( int matid );

	//		Event process
	gpevent *getEvent(int id);
	gpevent *addEvent(int id);
	gpevent *addNewEvent(void);
	int deleteEvent(int id);
	int GetEmptyEventId(void);
	int AddWaitEvent(int eventid, int frame);
	int AddMoveEvent(int eventid, int target, float x, float y, float z, int frame, int sw);
	int AddSplineMoveEvent(int eventid, int target, float x, float y, float z, int frame, int sw);
	int AddPlusEvent(int eventid, int target, float x, float y, float z);
	int AddChangeEvent(int eventid, int target, float x1, float y1, float z1, float x2, float y2, float z2);
	int AddJumpEvent(int eventid, int gonum, int rate);
	int AddParamEvent(int eventid, int mode, int target, int param);
	int AddParamEvent2(int eventid, int target, int param, int low, int high);
	int AddUVEvent(int eventid, int ysize, int count);
	int AddRegobjEvent(int eventid, int model, int event);
	int AddAimEvent(int eventid, int target, int mode, float x, float y, float z);
	int AddAnimEvent(int eventid, int anim, int opt, float speed);
	int AddSuicideEvent(int eventid, int mode);
	void ExecuteObjEvent(gpobj *obj, float timepass, int entry);
	int AttachEvent(int objid, int eventid, int entry);
	void putEventError(gpobj *obj, gpevent *ev, char *msg);

	void drawAll( int option );
	void updateAll( void );
	void drawObj( gpobj *obj );
	int updateObjColi( int objid, float size, int addcol, int startid=0, int searchnum=1 );
	int getNearestObj(int id, float range, int colgroup);
	void findeObj( int exmode, int group );
	gpobj *getNextObj( void );
	void pickupAll(int option);
	bool pickupNode(Node* node, int deep);

	void drawNode(Node* node, bool wirex, float alpha);
	bool drawNodeRecursive(Node *node,bool wire=false, float alpha = 1.0);
	int drawSceneObject(gpobj *camobj);

	int selectScene( int sceneid );
	int selectCamera( int camid );

	void makeNewModel( gpobj *obj, Mesh *mesh, Material *material );
	int makeNewModelWithMat( gpobj *obj, Mesh *mesh, int matid );

	int makeNullNode( void );
	int makeFloorNode(float xsize, float ysize, int color, int matid = -1);
	int makePlateNode( float xsize, float ysize, int color, int matid=-1 );
	int makeBoxNode( float size, int color, int matid=-1 );
	int makeModelNode( char *fname, char *idname, char *defs );

	bool makeModelNodeSub(Node *node, int nest);
	bool makeModelNodeMaterialSub(Node *node, int nest);

	int updateNodeMaterialID(int objid);
	int overwriteNodeMaterialByMatID(Node* node, int matid, int objid=-1);
	int overwriteNodeMaterialByColor(Node* node, int color, int matopt, int objid=-1);

	int makeCloneNode( int objid, int mode, int eventid );
	int makeSpriteObj( int celid, int gmode, void *bmscr );

	int makeNewMat(Material* material, int mode, int color, int matopt );
	int makeNewMat2D( char *fname, int matopt );
	int makeNewMatFromFB(gameplay::FrameBuffer *fb, int matopt);
	int makeNewMatFromObj(int objid, int part, char *nodename);
	char* getPixelMaskBuffer(char* fname, int* xsize, int* ysize);

	int makeNewLgt( int id, int lgtopt, float range=1.0f, float inner=0.5f, float outer=1.0f );
	int makeNewCam( int id, float fov, float aspect, float near, float far, int mode=0 );

	void setUserShader2D( char *vsh, char *fsh, char *defines );
	char *getUserVSH(void) { return (char *)user_vsh.c_str(); };
	char *getUserFSH(void) { return (char *)user_fsh.c_str(); };
	char *getUserDefines(void) { return (char *)user_defines.c_str(); };

	Material *makeMaterialColor( int color, int lighting );
	Material *makeMaterialTexture( char *fname, int matopt, Texture *opttex = NULL);
	Material *makeMaterialFromShader( char *vshd, char *fshd, char *defs );
	void setMaterialDefaultBinding(Material* material);
	void setMaterialDefaultBinding(Material* material, int matopt);
	void setMaterialDefaultBinding(Material* material, int icolor, int matopt);
	float setMaterialBlend( Material* material, int gmode, int gfrate );
	Material *makeMaterialTex2D(Texture *texture, int matopt);
	int getTextureWidth( void );
	int getTextureHeight( void );
	int applySamplerModeByString(Texture::Sampler* sampler, char* name, char* value);

	gameplay::FrameBuffer *makeFremeBuffer(char *name, int sx, int sy);
	void deleteFrameBuffer(gameplay::FrameBuffer *fb);
	void selectFrameBuffer(gameplay::FrameBuffer *fb, int sx, int sy);
	void resumeFrameBuffer(void);
	void clearFrameBuffer(void);

	void drawTest( int matid );
	void setFont(char *fontname, int size, int style);
	int drawFont(void *bmscr, int x, int y, char* text, int* out_ysize);

	int getObjectVector( int objid, int moc, Vector4 *prm );
	void getNodeVector(gpobj* obj, Node* node, int moc, Vector4* prm);
	void getNodeVectorExternal(gpobj* obj, Node* node, int moc, Vector4* prm);
	void getSpriteVector( gpobj *obj, int moc, Vector4 *prm );

	int setObjectVector( int objid, int moc, Vector4 *prm );
	void setSceneVector( int moc, Vector4 *prm );
	void setNodeVector( gpobj *obj, Node *node, int moc, Vector4 *prm );
	void setSpriteVector( gpobj *obj, int moc, Vector4 *prm );

	int addObjectVector( int objid, int moc, Vector4 *prm );
	void addSceneVector( int moc, Vector4 *prm );
	void addNodeVector( gpobj *obj, Node *node, int moc, Vector4 *prm );
	void addSpriteVector( gpobj *obj, int moc, Vector4 *prm );

	int lookAtObject( int objid, Vector4 *prm );
	void lookAtNode(Node* node, const Vector3& target );

	// light
	void resetCurrentLight( int lightmax=-1, int plightmax=-1, int slightmax=-1 );
	void setLightMaterialParameter(Material* material);
	int selectLight(int lightid, int index = 0);
	int getSelectLight(int index = 0, int opt = 0);
	void updateLightVector( gpobj *obj, int moc );

	// physics
	gpphy* getPhy(int id);
	void deletePhy( gpobj *obj );
	int setObjectBindPhysics( int objid, float mass, float friction, int option=0 );
	gpphy *setPhysicsObjectAuto( gpobj *obj, float mass, float friction, int option=0 );
	int objectPhysicsApply( int objid, int type, Vector3 *prm );
	int getPhysicsContact(int objid);
	gppinfo *getPhysicsContactInfo(int objid,int index);
	int execPhysicsRayTest(Vector3* pos, Vector3* direction, float distance, int usegroup = 0);
	int execPhysicsSweepTest(int objid, Vector3* pos, int usegroup = 0, Vector3* angle = NULL);
	void setFilterGroup(int group) { _hitfilter_group = group; };
	int getFilterGroup(void) { return _hitfilter_group; };

	// sprite
	gpspr *getSpriteObj( int objid );
	void findSpriteObj( bool lateflag );
	gpobj *getNextSpriteObj( void );

	// utility function
	void colorVector3( int color, Vector4 &vec );
	void colorVector4( int color, Vector4 &vec );
	void attachColorMaterial( Model *model, int icolor );
	Mesh *createCubeMesh( float size );
	Material*make2DMaterialForMesh( void );
	void setBorder( float x0, float x1, float y0, float y1, float z0, float z1 );
	void getBorder( Vector3 *v1, Vector3 *v2 );
	float GetTimerFromFrame(int frame);
	int convertAxis(Vector3 *res, Vector3 *pos, int mode);
	void storeNextVector(gpevent *myevent);
	int getCurrentFilterMode(void);
	void setCurrentFilterMode(int mode);
	Node* getNodeFromName( int objid, char *name );
	bool getNodeFromNameSub(Node* node, char *name, int deep);
	int getNodeInfo(int objid, int option, char* name, int* result);
	int getNodeInfoString(int objid, int option, char* name, std::string *res);
	int setNodeInfoMaterial(int objid, int option, char* name, int matid);

	// 2D draw function
	float *startPolyTex2D( gpmat *mat, int material_id );
	void drawPolyTex2D( gpmat *mat );
	void addPolyTex2D( gpmat *mat );
	void finishPolyTex2D( gpmat *mat );
	void setPolyDiffuseTex2D( float r, float g, float b, float a );

	float *startPolyColor2D( void );
	void drawPolyColor2D( void );
	void addPolyColor2D( int num );
	void finishPolyColor2D( void );
	float setPolyColorBlend( int gmode, int gfrate );
	void setPolyDiffuse2D( float r, float g, float b, float a );
	float *startLineColor2D( void );
	void drawLineColor2D( void );
	void addLineColor2D( int num );
	void finishLineColor2D( void );

	// global light function
	void setupDefines(void);
	char *getLightDefines(void) { return (char *)light_defines.c_str(); }
	char *getNoLightDefines(void) { return (char *)nolight_defines.c_str(); }
	char *getSpecularLightDefines(void) { return (char *)splight_defines.c_str(); }

	// folder manage
	char* getShaderFolder(void) { return (char*)shader_folder.c_str(); }
	void setShaderFolder(char *folder) { shader_folder = folder; }

	/**
	* 2D projection parameter
	*/
	void make2DRenderProjection(Matrix *mat, int sx, int sy);
	void update2DRenderProjection(Material* material, Matrix *mat);
	void update2DRenderProjectionSystem(Matrix* mat);
	void setUser2DRenderProjectionSystem(Matrix* mat, bool updateinv);
	void convert2DRenderProjection(Vector4 &pos);

	/**
	* Message texture service
	*/
	void texmesProc(void);
	void texmesDrawClip(void *bmscr, int x, int y, int psx, int psy, texmes *tex, int basex, int basey);
	texmesManager *getTexmesManager(void) { return &tmes; };

	//	free vertex function
	void clearFreeVertex(void);
	int addFreeVertex(float x, float y, float z, float nx, float ny, float nz, float u, float v);
	int addFreeVertexPolygon(int id1, int id2, int id3, int id4=-1);
	int makeFreeVertexNode(int color, int matid);


protected:
    /**
     * Internal use
     */

    /**
     * @see Game::initialize
     */
    void initialize();

    /**
     * @see Game::finalize
     */
    void finalize();

    /**
     * @see Game::update
     */
    void update(float elapsedTime);

    /**
     * @see Game::render
     */
    void render(float elapsedTime);

private:

    /**
     * update the scene each frame.
     */
	int updateObjBorder( int mode, Vector3 *pos, Vector4 *dir );
	void updateObj( gpobj *obj );

    /**
     * Draws the scene each frame.
     */
    bool updateNodeMaterial( Node* node, Material *material );
	bool drawScene(Node* node);
	bool init2DRender( void );

	Font*	mFont;

	float _colrate;
	int _tex_width;
	int _tex_height;

	// gpobj
	int _maxobj;
	int _objpool_startid;
	int _objpool_max;
	gpobj *_gpobj;

	int _find_count;
	int _find_exmode;
	int _find_group;
	bool _find_lateflag;
	gpobj *_find_gpobj;

	// gpmat
	int _maxmat;
	gpmat *_gpmat;

	// gpevent
	int _maxevent;
	gpevent *_gpevent;

	// physics
	CollisionCallback *_collision_callback;
	std::vector<FreeMeshVertex> _freevertex;	// FreeMeshVertex配列
	std::vector<short> _freeindex;				// FreeMeshVertex配列のindex
	float _fv_maxradius;						// FreeMeshVertex radius
	float _fv_minradius;						// FreeMeshVertex radius
	myPhysicsFilter _hitfilter;
	int _hitfilter_group;

	// default scene
	int _curscene;
	int _deflight;
	int _curcamera;
	int _defcamera;
	bool _scenedraw_lateflag;
	Scene *_scene;
	Camera *_cameraDefault;
	Quaternion _qcam_billboard;
	int _viewx1, _viewy1, _viewx2, _viewy2;		// ビューポート
	int   _originX, _originY; 					// 原点Y
	float _scaleX, _scaleY;						// スケールY
	FrameBuffer* _previousFrameBuffer;
	int _render_numobj;
	int _render_numpoly;
	int _filtermode;

	Node* touchNode;
	Node* tempNode;

	// Multi Light
	int _max_dlight;
	int _max_plight;
	int _max_slight;
	int _dir_light[BUFSIZE_MULTILIGHT];
	int _point_light[BUFSIZE_MULTILIGHT];
	int _spot_light[BUFSIZE_MULTILIGHT];
	int _curlight_hash;

	// Obj support value
	Vector3 border1;		// BORDER座標1
	Vector3 border2;		// BORDER座標2

	// preset flat mesh
	int proj2Dcode;								// 2D projection Matrix ID code
	Matrix _projectionMatrix2Dpreset;			// 2D projection Matrix (画面全体用)
	Matrix _projectionMatrix2D;					// 2D projection Matrix (システム用)
	Matrix _projectionMatrix2Dinv;				// 2D projection Matrix (逆変換用)
	MeshBatch* _meshBatch;						// MeshBatch for Polygon
	MeshBatch* _meshBatch_line;					// MeshBatch for Line
	MeshBatch* _meshBatch_font;					// MeshBatch for Font

	Effect *_spriteEffect;
	Effect *_spritecolEffect;
	float _bufPolyColor[BUFSIZE_POLYCOLOR];
	float _bufPolyTex[BUFSIZE_POLYTEX];

	// texmes
	texmesManager tmes;

	// preset light defines
	std::string	light_defines;
	std::string	nolight_defines;
	std::string	splight_defines;

	char lightname_ambient[PARAMNAME_LIGHT_MAXSIZE];
	char lightname_color[PARAMNAME_LIGHT_MAXSIZE];
	char lightname_direction[PARAMNAME_LIGHT_MAXSIZE];

	char lightname_pointcolor[PARAMNAME_LIGHT_MAXSIZE];
	char lightname_pointposition[PARAMNAME_LIGHT_MAXSIZE];
	char lightname_pointrange[PARAMNAME_LIGHT_MAXSIZE];

	char lightname_spotcolor[PARAMNAME_LIGHT_MAXSIZE];
	char lightname_spotposition[PARAMNAME_LIGHT_MAXSIZE];
	char lightname_spotdirection[PARAMNAME_LIGHT_MAXSIZE];
	char lightname_spotrange[PARAMNAME_LIGHT_MAXSIZE];
	char lightname_spotinner[PARAMNAME_LIGHT_MAXSIZE];
	char lightname_spotouter[PARAMNAME_LIGHT_MAXSIZE];

	// preset user defines
	std::string	user_vsh;
	std::string	user_fsh;
	std::string	user_defines;

	// shader folder
	std::string	shader_folder;
};

#endif
