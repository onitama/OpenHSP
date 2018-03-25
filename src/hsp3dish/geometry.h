//---------------------------------------------------------------------------
#ifndef __geometry_h
#define __geometry_h

#define PI 3.14159265358979f
#define PI2 6.28318530717958f
#define HPI 1.570796326794895f

#define DEG2ANG(x) ((int)((x) * 32768.0f / 180.0f) & 0xffff)
#define RAD2ANG(x) ((int)((x) * 32768.0f / PI) & 0xffff)
#define DEG2RAD(x) ((x) * PI / 180.0f)
#define RAD2DEG(x) ((x) * 180.0f / PI)

typedef struct {
	float x, y, z, w;
} VECTOR;

typedef struct {
  float m00, m01, m02, m03;
  float m10, m11, m12, m13;
  float m20, m21, m22, m23;
  float m30, m31, m32, m33;
} MATRIX;

typedef float FVECTOR[4];
typedef float * FVRef;
typedef int * IVRef;
typedef float (*FMatRef)[4];

#define VECREF (VECTOR *)(FVRef)

// 整数VECTOR型
typedef struct {
int x,y,z,w;
} IVECTOR;

/*
// 小数VECTOR型
typedef struct {
float x,y,z,w;
} FVECTOR;
*/

// 整数AXIS型
typedef struct {
int x,y;
} IAXIS;

// 小数AXIS型
typedef struct {
float x,y;
} FAXIS;

// 整数AXIS2型(UV付き)
typedef struct {
short x,y,tx,ty;
} IAXIS2;

#define setIAXIS( a,b,c ) a.x=(int)b;a.y=(int)c;
#define setIAXIS2( a,b,c,d,e ) a.x=(int)b;a.y=(int)c;a.tx=(int)d;a.ty=(int)e;

typedef struct {
	VECTOR a;			// 透視変換行列の係数(x, y, z0, z1)
	VECTOR b;			// 透視変換行列の加算値 (0, 0, z2, z3)
	VECTOR ra;			// 係数の逆数(1/K)
	VECTOR rb;			// 加算値の補数 (-K)
	VECTOR v;			// Ｚバッファとフォグのパラメータ
	VECTOR t;			// オフセットＸＹ座標とＺバッファとフォグのパラメータ
	VECTOR cmin;		// クリップ最小範囲＋テクスチャ補正係数
	VECTOR cmax;		// クリップ最大範囲
} ViewScreen;


//---------------------------------------------------------------------------

void GeometryInit(void);
void GeometryTerm(void);

//void CopyVector( VECTOR *dst, VECTOR *src );
#define CopyVector( dst,src ) *(dst) = *(src)

//void SetVector( VECTOR *dst, float x, float y, float z, float w );
#define SetVector(dst,xarg,yarg,zarg,warg)	(dst)->x=xarg;(dst)->y=yarg;(dst)->z=zarg;(dst)->w=warg
#define SetVector3(dst,xarg,yarg,zarg)	(dst)->x=xarg;(dst)->y=yarg;(dst)->z=zarg


void AddVector( VECTOR *dst, VECTOR *v0, VECTOR *v1 );
void SubVector( VECTOR *dst, VECTOR *v0, VECTOR *v1 );
void MulVector( VECTOR *dst, VECTOR *v0, VECTOR *v1 );
void DivVector( VECTOR *dst, VECTOR *v0, float r );
void ScaleVector( VECTOR *dst, VECTOR *v0, float r );

//  マトリックス初期化
void  InitMatrix(void);
//  左手系にする
void  LeftHand(void);
//  右手系にする(default)
void  RightHand(void);
//  マトリックスのコピー
void  CopyMatrix( MATRIX *src, MATRIX *dst );
//  マトリックス退避
void  PushMatrix(void);
//  マトリックス復帰
void  PopMatrix(void);
//  カレントマトリックス取得
void  GetCurrentMatrix(MATRIX *dst);
MATRIX *GetCurrentMatrixPtr( void );
//  カレントマトリックス設定
void  SetCurrentMatrix(MATRIX *dst);
//  カレントマトリックスのトランスを得る
void  GetCurrentTrans(VECTOR *v);
//  単位マトリックス
void  UnitMatrix(void);
//  Ｘ軸回転
void  RotX(float x);
//  Ｙ軸回転
void  RotY(float y);
//  Ｚ軸回転
void  RotZ(float z);
//  平行移動
void  Trans(float x, float y, float z);
//  スケール
void  Scale(float x, float y, float z);
//  透視投影
void  Perspective(float r);
//  透視投影かつＺをＺバッファ値に
void  PerspectiveWithZBuffer(float r, float N, float F, float Zn, float Zf);
//  マトリックス同士の積（カレントマトリックスに左から掛ける）
void  MulMatrix(MATRIX *left);
//  マトリックス同士の積（カレントマトリックスに右から掛ける）
void  MulMatrixR(MATRIX *right);
//  転置行列
void  Transpose(MATRIX *mat);
//  マトリックスの行列式
float  Determinant(MATRIX *mat);
//  3x3行列式の値（逆行列用）
float  Determinant3(MATRIX *src);
//  逆行列
int  InverseMatrix(MATRIX *mat);
//  ベクトル＊マトリックス -> ベクトル
void  ApplyMatrix(VECTOR *v0, VECTOR *v1);
void  ApplyMatrixN(VECTOR *v0, VECTOR *v1, int nb);
//  ベクトル＊マトリックス -> ベクトル（回転のみ）
void  ApplyMatrixRot(VECTOR *v0, VECTOR *v1);
//  ベクトル＊マトリックス -> ベクトル（平行移動のみ）
void  ApplyMatrixTrans(VECTOR *v0, VECTOR *v1);
//  ２ベクトルの外積
void  OuterProduct(VECTOR *v1, VECTOR *v2, VECTOR *dst);
//  ２ベクトルの内積
float  InnerProduct(VECTOR *v1, VECTOR *v2);
//  単位ベクトル
void  UnitVector(VECTOR *v);
//  カメラマトリックス生成
void  LookAt(VECTOR *cam_pos, VECTOR *cam_int);
void  LookAtWithRoll(VECTOR *cam_pos, VECTOR *cam_int, float roll);
void GetTargetAngle( VECTOR *ang, VECTOR *src, VECTOR *target );

//	View->Screenマトリクス生成
void PerspectiveViewScreen( ViewScreen * vs, float scrz, float ax, float ay, 
	       float xcenter, float ycenter, float zbufsmall, float zbufbig, float D, float F, float fognearz, float fogfarz, float W, float H);

//	ポリゴン接触判定
int HasPoint2D( float x, float y, VECTOR *v );
int HasPoint3D( VECTOR *p, VECTOR *v );
int IntersectLinePlane( VECTOR *lpoint, VECTOR *lvector, VECTOR *ppoint, VECTOR *pnormal, VECTOR *result );
float GetVectorDistance( VECTOR *v1, VECTOR *v2 );


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
#endif
