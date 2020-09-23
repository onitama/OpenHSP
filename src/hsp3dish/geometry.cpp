//---------------------------------------------------------------------------
#include <stdio.h>
#include <math.h>
#include "geometry.h"

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

static  MATRIX matrixBuf[128];
static  MATRIX *currentMatrix;
static  int rightHand;

//---------------------------------------------------------------------------
//	コンストラクタ
//---------------------------------------------------------------------------
void GeometryInit(void)
{
  InitMatrix();
  //RightHand();
  LeftHand();
}
//---------------------------------------------------------------------------
//	デストラクタ
//---------------------------------------------------------------------------
void GeometryTerm(void)
{
}



/*-----------------------------------------------
  ベクトルのコピー
-----------------------------------------------*/
void ICopyVector( VECTOR *dst, VECTOR *src )
{
	*dst = *src;
}

/*-----------------------------------------------
  ベクトルの代入
-----------------------------------------------*/
void ISetVector( VECTOR *dst, float x, float y, float z, float w )
{
	dst->x = x;
	dst->y = y;
	dst->z = z;
	dst->w = w;
}

/*-----------------------------------------------
  ベクトルの加算
-----------------------------------------------*/
void AddVector( VECTOR *dst, VECTOR *v0, VECTOR *v1 )
{
	dst->x = v0->x + v1->x;
	dst->y = v0->y + v1->y;
	dst->z = v0->z + v1->z;
	dst->w = v0->w + v1->w;
}

/*-----------------------------------------------
  ベクトルの減算
-----------------------------------------------*/
void SubVector( VECTOR *dst, VECTOR *v0, VECTOR *v1 )
{
	dst->x = v0->x - v1->x;
	dst->y = v0->y - v1->y;
	dst->z = v0->z - v1->z;
	dst->w = v0->w - v1->w;
}

/*-----------------------------------------------
  ベクトルの乗算
-----------------------------------------------*/
void MulVector( VECTOR *dst, VECTOR *v0, VECTOR *v1 )
{
	dst->x = v0->x * v1->x;
	dst->y = v0->y * v1->y;
	dst->z = v0->z * v1->z;
	dst->w = v0->w * v1->w;
}

/*-----------------------------------------------
  ベクトルの乗算
-----------------------------------------------*/
void ScaleVector( VECTOR *dst, VECTOR *v0, float r )
{
	dst->x = v0->x * r;
	dst->y = v0->y * r;
	dst->z = v0->z * r;
	dst->w = v0->w * r;
}

/*-----------------------------------------------
  ベクトルの除算
-----------------------------------------------*/
void DivVector( VECTOR *dst, VECTOR *v0, float r )
{
	float a;
	a = 1.0f / r;
	dst->x = v0->x * a;
	dst->y = v0->y * a;
	dst->z = v0->z * a;
	dst->w = v0->w * a;
}


/*-----------------------------------------------
  マトリックス初期化
-----------------------------------------------*/
void InitMatrix(void)
{
  currentMatrix = &matrixBuf[0];
  UnitMatrix();
}

/*-----------------------------------------------
  左手系にする
-----------------------------------------------*/
void LeftHand(void)
{
  rightHand = 0;
}


/*-----------------------------------------------
  右手系にする
-----------------------------------------------*/
void RightHand(void)
{
  rightHand = 1;
}


/*-----------------------------------------------
  マトリックスのコピー
-----------------------------------------------*/
void CopyMatrix( MATRIX *dst, MATRIX *src )
{
  dst->m00 = src->m00; dst->m01 = src->m01; dst->m02 = src->m02; dst->m03 = src->m03;
  dst->m10 = src->m10; dst->m11 = src->m11; dst->m12 = src->m12; dst->m13 = src->m13;
  dst->m20 = src->m20; dst->m21 = src->m21; dst->m22 = src->m22; dst->m23 = src->m23;
  dst->m30 = src->m30; dst->m31 = src->m31; dst->m32 = src->m32; dst->m33 = src->m33;
}

static inline void CopyMatrixInt( MATRIX *src, MATRIX *dst )
{
  //	Matrix copy ( for internal )
  //
  dst->m00 = src->m00; dst->m01 = src->m01; dst->m02 = src->m02; dst->m03 = src->m03;
  dst->m10 = src->m10; dst->m11 = src->m11; dst->m12 = src->m12; dst->m13 = src->m13;
  dst->m20 = src->m20; dst->m21 = src->m21; dst->m22 = src->m22; dst->m23 = src->m23;
  dst->m30 = src->m30; dst->m31 = src->m31; dst->m32 = src->m32; dst->m33 = src->m33;
}

/*-----------------------------------------------
  マトリックス退避
-----------------------------------------------*/
void PushMatrix(void)
{
  /* MATRIX STACK */
  CopyMatrixInt(currentMatrix, currentMatrix + 1);
  currentMatrix++;
}

/*-----------------------------------------------
  マトリックス復帰
-----------------------------------------------*/
void PopMatrix(void)
{
  /* MATRIX STACK */
  currentMatrix--;
}


/*-----------------------------------------------
  カレントマトリックス取得
-----------------------------------------------*/
void GetCurrentMatrix(MATRIX *dst)
{
  CopyMatrixInt(currentMatrix, dst);
}


/*-----------------------------------------------
  カレントマトリックス取得
-----------------------------------------------*/
MATRIX *GetCurrentMatrixPtr( void )
{
	return currentMatrix;
}


/*-----------------------------------------------
  カレントマトリックス設定
-----------------------------------------------*/
void SetCurrentMatrix(MATRIX *src)
{
  CopyMatrixInt(src, currentMatrix);
}


/*-----------------------------------------------
  単位マトリックス
-----------------------------------------------*/
void UnitMatrix(void)
{
  MATRIX *mat = currentMatrix;

  mat->m00 = 1.0f; mat->m01 = 0.0f; mat->m02 = 0.0f; mat->m03 = 0.0f;
  mat->m10 = 0.0f; mat->m11 = 1.0f; mat->m12 = 0.0f; mat->m13 = 0.0f;
  mat->m20 = 0.0f; mat->m21 = 0.0f; mat->m22 = 1.0f; mat->m23 = 0.0f;
  mat->m30 = 0.0f; mat->m31 = 0.0f; mat->m32 = 0.0f; mat->m33 = 1.0f;
}




/*-----------------------------------------------
  Ｘ軸回転（左手系）
-----------------------------------------------*/
//
//X axis rotation
//
//|M00 M01 M02 M03|   |1     0     0     0||m00 m01 m02 m03|
//|M10 M11 M12 M13| = |0  cosx -sinx     0||m10 m11 m12 m13|
//|M20 M21 M22 M23|   |0  sinx  cosx     0||m20 m21 m22 m23|
//|M30 M31 M32 M33|   |0     0     0     1||m30 m31 m32 m33|

static void RotXLeft(float x)
{
  MATRIX *mat, mt;
  float s, c;

  mat = currentMatrix;
  CopyMatrixInt(mat, &mt);

  s = (float)sin(x); c = (float)cos(x);

#if 0
  mat->m00 =    mt.m00;
  mat->m01 =    mt.m01;
  mat->m02 =    mt.m02;
  mat->m03 =    mt.m03;
#endif
  mat->m10 =  c*mt.m10 + -s*mt.m20;
  mat->m11 =  c*mt.m11 + -s*mt.m21;
  mat->m12 =  c*mt.m12 + -s*mt.m22;
  mat->m13 =  c*mt.m13 + -s*mt.m23;

  mat->m20 =  s*mt.m10 +  c*mt.m20;
  mat->m21 =  s*mt.m11 +  c*mt.m21;
  mat->m22 =  s*mt.m12 +  c*mt.m22;
  mat->m23 =  s*mt.m13 +  c*mt.m23;
}

/*-----------------------------------------------
  Ｙ軸回転（左手系）
-----------------------------------------------*/
//
//Y axis rotation
//
//|M00 M01 M02 M03|   | cosy     0  siny     0||m00 m01 m02 m03|
//|M10 M11 M12 M13| = |    0     1     0     0||m10 m11 m12 m13|
//|M20 M21 M22 M23|   |-siny     0  cosy     0||m20 m21 m22 m23|
//|M30 M31 M32 M33|   |    0     0     0     1||m30 m31 m32 m33|

static void RotYLeft(float y)
{
  MATRIX *mat, mt;
  float s, c;

  mat = currentMatrix;
  CopyMatrixInt(mat, &mt);

  s = (float)sin(y); c = (float)cos(y);

  mat->m00 =  c*mt.m00 +  s*mt.m20;
  mat->m01 =  c*mt.m01 +  s*mt.m21;
  mat->m02 =  c*mt.m02 +  s*mt.m22;
  mat->m03 =  c*mt.m03 +  s*mt.m23;
#if 0
  mat->m10 =    mt.m10;
  mat->m11 =    mt.m11;
  mat->m12 =    mt.m12;
  mat->m13 =    mt.m13;
#endif
  mat->m20 = -s*mt.m00 +  c*mt.m20;
  mat->m21 = -s*mt.m01 +  c*mt.m21;
  mat->m22 = -s*mt.m02 +  c*mt.m22;
  mat->m23 = -s*mt.m03 +  c*mt.m23;
}


/*-----------------------------------------------
  Ｚ軸回転（左手系）
-----------------------------------------------*/
//
//Z axis rotation
//
//|M00 M01 M02 M03|   | cosz -sinz     0     0||m00 m01 m02 m03|
//|M10 M11 M12 M13| = | sinz  cosz     0     0||m10 m11 m12 m13|
//|M20 M21 M22 M23|   |    0     0     1     0||m20 m21 m22 m23|
//|M30 M31 M32 M33|   |    0     0     0     1||m30 m31 m32 m33|

static void RotZLeft(float z)
{
  MATRIX *mat, mt;
  float s, c;

  mat = currentMatrix;
  CopyMatrixInt(mat, &mt);
  
  s = (float)sin(z); c = (float)cos(z);

  mat->m00 =  c*mt.m00 + -s*mt.m10;
  mat->m01 =  c*mt.m01 + -s*mt.m11;
  mat->m02 =  c*mt.m02 + -s*mt.m12;
  mat->m03 =  c*mt.m03 + -s*mt.m13;

  mat->m10 =  s*mt.m00 +  c*mt.m10;
  mat->m11 =  s*mt.m01 +  c*mt.m11;
  mat->m12 =  s*mt.m02 +  c*mt.m12;
  mat->m13 =  s*mt.m03 +  c*mt.m13;
#if 0
  mat->m20 =    mt.m20;
  mat->m21 =    mt.m21;
  mat->m22 =    mt.m22;
  mat->m23 =    mt.m23;
#endif
}


/*-----------------------------------------------
  Ｘ軸回転（右手系）
-----------------------------------------------*/
//
//X axis rotation
//
//|M00 M01 M02 M03|   |1     0     0     0||m00 m01 m02 m03|
//|M10 M11 M12 M13| = |0  cosx  sinx     0||m10 m11 m12 m13|
//|M20 M21 M22 M23|   |0 -sinx  cosx     0||m20 m21 m22 m23|
//|M30 M31 M32 M33|   |0     0     0     1||m30 m31 m32 m33|

static void RotXRight(float x)
{
  MATRIX *mat, mt;
  float s, c;

  mat = currentMatrix;
  CopyMatrixInt(mat, &mt);

  s = (float)sin(x); c = (float)cos(x);

#if 0
  mat->m00 =    mt.m00;
  mat->m01 =    mt.m01;
  mat->m02 =    mt.m02;
  mat->m03 =    mt.m03;
#endif
  mat->m10 =  c*mt.m10 +  s*mt.m20;
  mat->m11 =  c*mt.m11 +  s*mt.m21;
  mat->m12 =  c*mt.m12 +  s*mt.m22;
  mat->m13 =  c*mt.m13 +  s*mt.m23;

  mat->m20 = -s*mt.m10 +  c*mt.m20;
  mat->m21 = -s*mt.m11 +  c*mt.m21;
  mat->m22 = -s*mt.m12 +  c*mt.m22;
  mat->m23 = -s*mt.m13 +  c*mt.m23;

}

/*-----------------------------------------------
  Ｙ軸回転（右手系）
-----------------------------------------------*/
//
//Y axis rotation
//
//|M00 M01 M02 M03|   | cosy     0 -siny     0||m00 m01 m02 m03|
//|M10 M11 M12 M13| = |    0     1     0     0||m10 m11 m12 m13|
//|M20 M21 M22 M23|   | siny     0  cosy     0||m20 m21 m22 m23|
//|M30 M31 M32 M33|   |    0     0     0     1||m30 m31 m32 m33|

static void RotYRight(float y)
{
  MATRIX *mat, mt;
  float s, c;

  mat = currentMatrix;
  CopyMatrixInt(mat, &mt);

  s = (float)sin(y); c = (float)cos(y);

  mat->m00 =  c*mt.m00 + -s*mt.m20;
  mat->m01 =  c*mt.m01 + -s*mt.m21;
  mat->m02 =  c*mt.m02 + -s*mt.m22;
  mat->m03 =  c*mt.m03 + -s*mt.m23;
#if 0
  mat->m10 =    mt.m10;
  mat->m11 =    mt.m11;
  mat->m12 =    mt.m12;
  mat->m13 =    mt.m13;
#endif
  mat->m20 =  s*mt.m00 +  c*mt.m20;
  mat->m21 =  s*mt.m01 +  c*mt.m21;
  mat->m22 =  s*mt.m02 +  c*mt.m22;
  mat->m23 =  s*mt.m03 +  c*mt.m23;
}


/*-----------------------------------------------
  Ｚ軸回転（右手系）
-----------------------------------------------*/
//
//Z axis rotation
//
//|M00 M01 M02 M03|   |  cosz  sinz     0     0||m00 m01 m02 m03|
//|M10 M11 M12 M13| = | -sinz  cosz     0     0||m10 m11 m12 m13|
//|M20 M21 M22 M23|   |     0     0     1     0||m20 m21 m22 m23|
//|M30 M31 M32 M33|   |     0     0     0     1||m30 m31 m32 m33|

static void RotZRight(float z)
{
  MATRIX *mat, mt;
  float s, c;

  mat = currentMatrix;
  CopyMatrixInt(mat, &mt);
  
  s = (float)sin(z); c = (float)cos(z);

  mat->m00 =  c*mt.m00 +  s*mt.m10;
  mat->m01 =  c*mt.m01 +  s*mt.m11;
  mat->m02 =  c*mt.m02 +  s*mt.m12;
  mat->m03 =  c*mt.m03 +  s*mt.m13;

  mat->m10 = -s*mt.m00 +  c*mt.m10;
  mat->m11 = -s*mt.m01 +  c*mt.m11;
  mat->m12 = -s*mt.m02 +  c*mt.m12;
  mat->m13 = -s*mt.m03 +  c*mt.m13;
#if 0
  mat->m20 =    mt.m20;
  mat->m21 =    mt.m21;
  mat->m22 =    mt.m22;
  mat->m23 =    mt.m23;
#endif
}

/*-----------------------------------------------
  Ｘ軸回転
-----------------------------------------------*/
void RotX(float x)
{
  if (rightHand) RotXRight(x);
  else           RotXLeft(x);
}
/*-----------------------------------------------
  Ｙ軸回転
-----------------------------------------------*/
void RotY(float y)
{
  if (rightHand) RotYRight(y);
  else           RotYLeft(y);
}
/*-----------------------------------------------
  Ｚ軸回転
-----------------------------------------------*/
void RotZ(float z)
{
  if (rightHand) RotZRight(z);
  else           RotZLeft(z);
}


/*-----------------------------------------------
  平行移動
-----------------------------------------------*/
//
//translation
//
//|M00 M01 M02 M03|   | 1     0     0     0||m00 m01 m02 m03|
//|M10 M11 M12 M13| = | 0     1     0     0||m10 m11 m12 m13|
//|M20 M21 M22 M23|   | 0     0     1     0||m20 m21 m22 m23|
//|M30 M31 M32 M33|   | x     y     z     1||m30 m31 m32 m33|

void Trans(float x, float y, float z)
{
  MATRIX *mat;
  mat = currentMatrix;

  mat->m30 += x * mat->m00  +  y * mat->m10  +  z * mat->m20;
  mat->m31 += x * mat->m01  +  y * mat->m11  +  z * mat->m21;
  mat->m32 += x * mat->m02  +  y * mat->m12  +  z * mat->m22;
  mat->m33 += x * mat->m03  +  y * mat->m13  +  z * mat->m23;
}


/*-----------------------------------------------
  スケール
-----------------------------------------------*/
//
//scaling
//
//|M00 M01 M02 M03|   | x     0     0     0||m00 m01 m02 m03|
//|M10 M11 M12 M13| = | 0     y     0     0||m10 m11 m12 m13|
//|M20 M21 M22 M23|   | 0     0     z     0||m20 m21 m22 m23|
//|M30 M31 M32 M33|   | 0     0     0     1||m30 m31 m32 m33|

void  Scale(float x, float y, float z)	/* scale vector */
{
  MATRIX *mat;
  mat = currentMatrix;

  mat->m00 *= x;
  mat->m01 *= x;
  mat->m02 *= x;
  mat->m03 *= x;
  mat->m10 *= y;
  mat->m11 *= y;
  mat->m12 *= y;
  mat->m13 *= y;
  mat->m20 *= z;
  mat->m21 *= z;
  mat->m22 *= z;
  mat->m23 *= z;
}


/*-----------------------------------------------
  透視投影
-----------------------------------------------*/
//
//perspective
//
//|M00 M01 M02 M03|   | 1     0     0     0||m00 m01 m02 m03|
//|M10 M11 M12 M13| = | 0     1     0     0||m10 m11 m12 m13|
//|M20 M21 M22 M23|   | 0     0     1     r||m20 m21 m22 m23|
//|M30 M31 M32 M33|   | 0     0     0     1||m30 m31 m32 m33|

void Perspective(float r)
{
  MATRIX *mat;
  mat = currentMatrix;

  mat->m20 += r * mat->m30;
  mat->m21 += r * mat->m31;
  mat->m22 += r * mat->m32;
  mat->m23 += r * mat->m33;
}


/*-----------------------------------------------
  ＺをＺバッファ値にする
-----------------------------------------------*/
//（注意）
//透視変換マトリックスを掛けた後にこのマトリックスを掛ける。
//
//----------解説-------------
//
//前方クリップ面のＺ座標 N,その時のＺバッファ値Zn
//後方クリップ面のＺ座標 F,その時のＺバッファ値Zf とする。
//
//透視変換したベクトルは(x,y,z,rz+1)
//スクリーン上での座標は( x/(rz+1), y/(rz+1), z/(rz+1), 1 )
//
//z/(rz+1) = N の時 Zbuffer=Zn
//z/(rz+1) = F の時 Zbuffer=Zf
//となるようにしたい。
//
//z/(rz+1) = N を zについて解くと
//z = N/(1-rN) となる。
//つまり
//
//z = N/(1-rN) の時にZbuffer=Zn ... (1)
//z = F/(1-rF) の時にZbuffer=Zf ... (2)
//
//となるような変換マトリックスを用意する。
//
//                            |1   0   0   0|
//|X  Y  Z  W|  = |x y z rz+1||0   1   0   0|
//                            |0   0  m22  0|
//                            |0   0  m32  1|
//
//Z がZBufferの値。
//Z について計算すると
//
//Z = m22z + m32(rz+1)
//
//(1)(2)より
//m22{N/(1-rN)} + m32{(rN)/(1-rN) + 1} = Zn
//m32{F/(1-rF)} + m32{(rF)/(1-rF) + 1} = Zf
//
//A = N/(1-rN)
//B = F/(1-rF)
//C = (rN)/(1-rN) + 1
//D = (rF)/(1-rF) + 1
//
//とすると、
//
//m22A + m32C = Zn
//m22B + m32D = Zf
//
//これを解くと、
//
//m22 = (DZn - CZf) / (AD - BC)
//m32 = (AZf - BZn) / (AD - BC)
//
//となる。
//
//----------解説終了------------
//
//|M00 M01 M02 M03|   | 1     0     0     0||m00 m01 m02 m03|
//|M10 M11 M12 M13| = | 0     1     0     0||m10 m11 m12 m13|
//|M20 M21 M22 M23|   | 0     0    m22    0||m20 m21 m22 m23|
//|M30 M31 M32 M33|   | 0     0    m32    1||m30 m31 m32 m33|
//
//※※※※※※※※※※※※※※※※※※※※※※※※※
//Zn>Zfでないとうまく動かない（バグか？）
//※※※※※※※※※※※※※※※※※※※※※※※※※

void ZBufferMatrix(float N, float F, float Zn, float Zf, float r)
{
  MATRIX m;
  float A,B,C,D;
  float coef;

  A = N / (1.0f - r * N);
  B = F / (1.0f - r * F);
  C = r * N / (1.0f - r * N) + 1.0f;
  D = r * F / (1.0f - r * F) + 1.0f;
  coef = 1.0f / (A * D - B * C);

  m.m00 = 1.0f;
  m.m01 = 0.0f;
  m.m02 = 0.0f;
  m.m03 = 0.0f;

  m.m10 = 0.0f;
  m.m11 = 1.0f;
  m.m12 = 0.0f;
  m.m13 = 0.0f;

  m.m20 = 0.0f;
  m.m21 = 0.0f;
  m.m22 = (D * Zn - C * Zf) * coef;
  m.m23 = 0.0f;

  m.m30 = 0.0f;
  m.m31 = 0.0f;
  m.m32 = (A * Zf - B * Zn) * coef;
  m.m33 = 1.0f;

  MulMatrix(&m);
}


/*-----------------------------------------------
  透視投影かつＺをＺバッファ値に
-----------------------------------------------*/
void  PerspectiveWithZBuffer(float r, float N, float F, float Zn, float Zf)
{
   ZBufferMatrix(N, F, Zn, Zf, r);
   Perspective(r);
//  currentMatrix->m00 *= 0.001f;
//  currentMatrix->m11 *= 0.001f;

  float right = 480.0f;
  float left = 0.0f;
  float top = 0.0f;
  float bottom = 800.0f;

  MATRIX *mat = currentMatrix;

  mat->m00 *= 2.0f / (right - left);
  mat->m11 *= 2.0f / (top - bottom);

  mat->m30 = -(right + left) / (right - left);
  mat->m31 = -(top + bottom) / (top - bottom);
  mat->m32 = 0.5f;

}


void  PerspectiveFOV(float fov, float Zn, float Zf, float left, float top, float right, float bottom)
{
	float f_n = 1.0f / (Zf - Zn);
	float theta = DEG2RAD(fov) * 0.5f;
	float divisor = tan(theta);
	float factor = 1.0f / divisor;
	MATRIX* mat = currentMatrix;

	mat->m00 = factor * (2.0f / (right - left));
	mat->m01 = 0.0f;
	mat->m02 = 0.0f;
	mat->m03 = 0.0f;

	mat->m10 = 0.0f;
	mat->m11 = factor * (2.0f / (top - bottom));
	mat->m12 = 0.0f;
	mat->m13 = 0.0f;

	mat->m20 = 0.0f;
	mat->m21 = 0.0f;
	mat->m22 = (-(Zf + Zn)) * f_n;
	mat->m23 = -2.0f * Zf * Zn * f_n;

	mat->m30 = 0.0f;
	mat->m31 = 0.0f;
	mat->m32 = -1.0f;
	mat->m33 = 1.0f;

	mat->m30 = -(right + left) / (right - left);
	mat->m31 = -(top + bottom) / (top - bottom);
}


/*-----------------------------------------------
  マトリックス同士の積（カレントマトリックスに左から掛ける）
-----------------------------------------------*/
void MulMatrix(MATRIX *l)
{
  MATRIX *dst, *r, mt;

  dst = currentMatrix;
  CopyMatrixInt(dst, &mt);
  r = &mt;

  dst->m00 = l->m00 * r->m00 + l->m01 * r->m10 + l->m02 * r->m20 + l->m03 * r->m30;
  dst->m01 = l->m00 * r->m01 + l->m01 * r->m11 + l->m02 * r->m21 + l->m03 * r->m31;
  dst->m02 = l->m00 * r->m02 + l->m01 * r->m12 + l->m02 * r->m22 + l->m03 * r->m32;
  dst->m03 = l->m00 * r->m03 + l->m01 * r->m13 + l->m02 * r->m23 + l->m03 * r->m33;

  dst->m10 = l->m10 * r->m00 + l->m11 * r->m10 + l->m12 * r->m20 + l->m13 * r->m30;
  dst->m11 = l->m10 * r->m01 + l->m11 * r->m11 + l->m12 * r->m21 + l->m13 * r->m31;
  dst->m12 = l->m10 * r->m02 + l->m11 * r->m12 + l->m12 * r->m22 + l->m13 * r->m32;
  dst->m13 = l->m10 * r->m03 + l->m11 * r->m13 + l->m12 * r->m23 + l->m13 * r->m33;

  dst->m20 = l->m20 * r->m00 + l->m21 * r->m10 + l->m22 * r->m20 + l->m23 * r->m30;
  dst->m21 = l->m20 * r->m01 + l->m21 * r->m11 + l->m22 * r->m21 + l->m23 * r->m31;
  dst->m22 = l->m20 * r->m02 + l->m21 * r->m12 + l->m22 * r->m22 + l->m23 * r->m32;
  dst->m23 = l->m20 * r->m03 + l->m21 * r->m13 + l->m22 * r->m23 + l->m23 * r->m33;

  dst->m30 = l->m30 * r->m00 + l->m31 * r->m10 + l->m32 * r->m20 + l->m33 * r->m30;
  dst->m31 = l->m30 * r->m01 + l->m31 * r->m11 + l->m32 * r->m21 + l->m33 * r->m31;
  dst->m32 = l->m30 * r->m02 + l->m31 * r->m12 + l->m32 * r->m22 + l->m33 * r->m32;
  dst->m33 = l->m30 * r->m03 + l->m31 * r->m13 + l->m32 * r->m23 + l->m33 * r->m33;
}


/*-----------------------------------------------
  マトリックス同士の積（カレントマトリックスに右から掛ける）
-----------------------------------------------*/
void MulMatrixR(MATRIX *r)
{
  MATRIX *dst, *l, mt;

  dst = currentMatrix;
  CopyMatrixInt(dst, &mt);
  l = &mt;

  dst->m00 = l->m00 * r->m00 + l->m01 * r->m10 + l->m02 * r->m20 + l->m03 * r->m30;
  dst->m01 = l->m00 * r->m01 + l->m01 * r->m11 + l->m02 * r->m21 + l->m03 * r->m31;
  dst->m02 = l->m00 * r->m02 + l->m01 * r->m12 + l->m02 * r->m22 + l->m03 * r->m32;
  dst->m03 = l->m00 * r->m03 + l->m01 * r->m13 + l->m02 * r->m23 + l->m03 * r->m33;

  dst->m10 = l->m10 * r->m00 + l->m11 * r->m10 + l->m12 * r->m20 + l->m13 * r->m30;
  dst->m11 = l->m10 * r->m01 + l->m11 * r->m11 + l->m12 * r->m21 + l->m13 * r->m31;
  dst->m12 = l->m10 * r->m02 + l->m11 * r->m12 + l->m12 * r->m22 + l->m13 * r->m32;
  dst->m13 = l->m10 * r->m03 + l->m11 * r->m13 + l->m12 * r->m23 + l->m13 * r->m33;

  dst->m20 = l->m20 * r->m00 + l->m21 * r->m10 + l->m22 * r->m20 + l->m23 * r->m30;
  dst->m21 = l->m20 * r->m01 + l->m21 * r->m11 + l->m22 * r->m21 + l->m23 * r->m31;
  dst->m22 = l->m20 * r->m02 + l->m21 * r->m12 + l->m22 * r->m22 + l->m23 * r->m32;
  dst->m23 = l->m20 * r->m03 + l->m21 * r->m13 + l->m22 * r->m23 + l->m23 * r->m33;

  dst->m30 = l->m30 * r->m00 + l->m31 * r->m10 + l->m32 * r->m20 + l->m33 * r->m30;
  dst->m31 = l->m30 * r->m01 + l->m31 * r->m11 + l->m32 * r->m21 + l->m33 * r->m31;
  dst->m32 = l->m30 * r->m02 + l->m31 * r->m12 + l->m32 * r->m22 + l->m33 * r->m32;
  dst->m33 = l->m30 * r->m03 + l->m31 * r->m13 + l->m32 * r->m23 + l->m33 * r->m33;
}


/*-----------------------------------------------
  転置行列
-----------------------------------------------*/
void Transpose(MATRIX *dst)
{
  MATRIX *src;
  MATRIX mt;

  src = &mt;
  if (dst == NULL){
    dst = currentMatrix;
  }

  CopyMatrixInt(dst, src);

  /* dst->m00 = src->m00; */ dst->m01 = src->m10;    dst->m02 = src->m20;    dst->m03 = src->m30;
     dst->m10 = src->m01; /* dst->m11 = src->m11; */ dst->m12 = src->m21;    dst->m13 = src->m31;
     dst->m20 = src->m02;    dst->m21 = src->m12; /* dst->m22 = src->m22; */ dst->m23 = src->m32;
     dst->m30 = src->m03;    dst->m31 = src->m13;    dst->m32 = src->m23; /* dst->m33 = src->m33; */

}


/*-----------------------------------------------
  マトリックスの行列式
-----------------------------------------------*/
float Determinant(MATRIX *src)
{
  float val0, val1, val2, val3, val4, val5;
  float val00, val01, val02, val03;
  float value;

  if (src == NULL){
    src = currentMatrix;
  }

  val0 = (src->m22 * src->m33 - src->m23 * src->m32);
  val1 = (src->m21 * src->m33 - src->m23 * src->m31);
  val2 = (src->m21 * src->m32 - src->m22 * src->m31);
  val3 = (src->m20 * src->m33 - src->m23 * src->m30);
  val4 = (src->m20 * src->m32 - src->m22 * src->m30);
  val5 = (src->m20 * src->m31 - src->m21 * src->m30);

  val00 = (src->m11 * val0 - src->m12 * val1 + src->m13 * val2);
  val01 = (src->m10 * val0 - src->m12 * val3 + src->m13 * val4);
  val02 = (src->m10 * val1 - src->m11 * val3 + src->m13 * val5);
  val03 = (src->m10 * val2 - src->m11 * val4 + src->m12 * val5);

  value = src->m00 * val00 - src->m01 * val01 + src->m02 * val02 - src->m03 * val03;

#if 0
  value  =  src->m00 * (src->m11 * val0 - src->m12 * val1 + src->m13 * val2);
  value += -src->m01 * (src->m10 * val0 - src->m12 * val3 + src->m13 * val4);
  value +=  src->m02 * (src->m10 * val1 - src->m11 * val3 + src->m13 * val5);
  value += -src->m03 * (src->m10 * val2 - src->m11 * val4 + src->m12 * val5);


  value  =  src->m00 * (
			 src->m11 * (src->m22 * src->m33 - src->m23 * src->m32) +
			-src->m12 * (src->m21 * src->m33 - src->m23 * src->m31) +
			 src->m13 * (src->m21 * src->m32 - src->m22 * src->m31)
			);

  value += -src->m01 * (
			 src->m10 * (src->m22 * src->m33 - src0>m23 * src->m32) +
			-src->m12 * (src->m20 * src->m33 - src->m23 * src->m30) +
			 src->m13 * (src->m20 * src->m32 - src->m22 * src->m30)
			);

  value +=  src->m02 * (
			 src->m10 * (src->m21 * src->m33 - src->m23 * src->m31) +
			-src->m11 * (src->m20 * src->m33 - src->m23 * src->m30) +
			 src->m13 * (src->m20 * src->m31 - src->m21 * src->m30)
			);

  value += -src->m03 * (
			 src->m10 * (src->m21 * src->m32 - src->m22 * src->m31) +
			-src->m11 * (src->m20 * src->m32 - src->m22 * src->m30) +
			 src->m12 * (src->m20 * src->m31 - src->m21 * src->m30)
			);
#endif

  return value;
}


/*
 *
 * 3x3行列式の値（逆行列用）
 *
 */
float Determinant3(MATRIX *src)
{
  float val0, val1, val2;
  float value;
  
  val0 = (src->m11 * src->m22 - src->m12 * src->m21);
  val1 = (src->m10 * src->m22 - src->m12 * src->m20);
  val2 = (src->m10 * src->m21 - src->m11 * src->m20);

  value = src->m00 * val0 - src->m01 * val1 + src->m02 * val2;

  return value;
}


/*-----------------------------------------------
  逆行列
 *     0   4   8   12
 *     1   5   9   13
 *     2   6   10  14
 *     3   7   11  15

-----------------------------------------------*/
int InverseMatrix(MATRIX *p_dst)
{
	float m[16];
	MATRIX *dst = p_dst;
	if (dst == NULL) {
		dst = currentMatrix;
	}

	m[0]  = currentMatrix->m00;
	m[4]  = currentMatrix->m01;
	m[8]  = currentMatrix->m02;
	m[12] = currentMatrix->m03;

	m[1]  = currentMatrix->m10;
	m[5]  = currentMatrix->m11;
	m[9]  = currentMatrix->m12;
	m[13] = currentMatrix->m13;

	m[2]  = currentMatrix->m20;
	m[6]  = currentMatrix->m21;
	m[10] = currentMatrix->m22;
	m[14] = currentMatrix->m23;

	m[3]  = currentMatrix->m30;
	m[7]  = currentMatrix->m31;
	m[11] = currentMatrix->m32;
	m[15] = currentMatrix->m33;

	float a0 = m[0] * m[5] - m[1] * m[4];
	float a1 = m[0] * m[6] - m[2] * m[4];
	float a2 = m[0] * m[7] - m[3] * m[4];
	float a3 = m[1] * m[6] - m[2] * m[5];
	float a4 = m[1] * m[7] - m[3] * m[5];
	float a5 = m[2] * m[7] - m[3] * m[6];
	float b0 = m[8] * m[13] - m[9] * m[12];
	float b1 = m[8] * m[14] - m[10] * m[12];
	float b2 = m[8] * m[15] - m[11] * m[12];
	float b3 = m[9] * m[14] - m[10] * m[13];
	float b4 = m[9] * m[15] - m[11] * m[13];
	float b5 = m[10] * m[15] - m[11] * m[14];

	// Calculate the determinant.
	float det = a0 * b5 - a1 * b4 + a2 * b3 + a3 * b2 - a4 * b1 + a5 * b0;

	// Close to zero, can't invert.
	if (fabs(det) == 0.0f) return FALSE;

	// Support the case where m == dst.
	dst->m00 = m[5] * b5 - m[6] * b4 + m[7] * b3;
	dst->m10 = -m[1] * b5 + m[2] * b4 - m[3] * b3;
	dst->m20 = m[13] * a5 - m[14] * a4 + m[15] * a3;
	dst->m30 = -m[9] * a5 + m[10] * a4 - m[11] * a3;

	dst->m01 = -m[4] * b5 + m[6] * b2 - m[7] * b1;
	dst->m11 = m[0] * b5 - m[2] * b2 + m[3] * b1;
	dst->m21 = -m[12] * a5 + m[14] * a2 - m[15] * a1;
	dst->m31 = m[8] * a5 - m[10] * a2 + m[11] * a1;

	dst->m02 = m[4] * b4 - m[5] * b2 + m[7] * b0;
	dst->m12 = -m[0] * b4 + m[1] * b2 - m[3] * b0;
	dst->m22 = m[12] * a4 - m[13] * a2 + m[15] * a0;
	dst->m32 = -m[8] * a4 + m[9] * a2 - m[11] * a0;

	dst->m03 = -m[4] * b3 + m[5] * b1 - m[6] * b0;
	dst->m13 = m[0] * b3 - m[1] * b1 + m[2] * b0;
	dst->m23 = -m[12] * a3 + m[13] * a1 - m[14] * a0;
	dst->m33 = m[8] * a3 - m[9] * a1 + m[10] * a0;

	float w = 1.0f / det;

	dst->m00 *= w;
	dst->m01 *= w;
	dst->m02 *= w;
	dst->m03 *= w;

	dst->m10 *= w;
	dst->m11 *= w;
	dst->m12 *= w;
	dst->m13 *= w;

	dst->m20 *= w;
	dst->m21 *= w;
	dst->m22 *= w;
	dst->m23 *= w;

	dst->m30 *= w;
	dst->m31 *= w;
	dst->m32 *= w;
	dst->m33 *= w;

#if 0

  MATRIX *src;
  MATRIX mt, mat3;
  float invA;

  src = &mt;
  if (dst == NULL){
    dst = currentMatrix;
  }

  invA = Determinant(NULL);
  if (invA == 0.0f){
    return FALSE;
  }
  invA = 1.0f / invA;

  CopyMatrixInt(dst, src);

  mat3.m00 = src->m11; mat3.m01 = src->m12; mat3.m02 = src->m13;
  mat3.m10 = src->m21; mat3.m11 = src->m22; mat3.m12 = src->m23;
  mat3.m20 = src->m31; mat3.m21 = src->m32; mat3.m22 = src->m33;
  dst->m00 = Determinant3(&mat3) * invA;

  mat3.m00 = src->m10; /* mat3.m01 = src->m12; mat3.m02 = src->m13; */
  mat3.m10 = src->m20; /* mat3.m11 = src->m22; mat3.m12 = src->m23; */
  mat3.m20 = src->m30; /* mat3.m21 = src->m32; mat3.m22 = src->m33; */
  dst->m01 = -Determinant3(&mat3) * invA;

  /* mat3.m00 = src->m10; */ mat3.m01 = src->m11; /* mat3.m02 = src->m13; */
  /* mat3.m10 = src->m20; */ mat3.m11 = src->m21; /* mat3.m12 = src->m23; */
  /* mat3.m20 = src->m30; */ mat3.m21 = src->m31; /* mat3.m22 = src->m33; */
  dst->m02 = Determinant3(&mat3) * invA;

  /* mat3.m00 = src->m10; mat3.m01 = src->m11; */ mat3.m02 = src->m12;
  /* mat3.m10 = src->m20; mat3.m11 = src->m21; */ mat3.m12 = src->m22;
  /* mat3.m20 = src->m30; mat3.m21 = src->m31; */ mat3.m22 = src->m32;
  dst->m03 = -Determinant3(&mat3) * invA;

  mat3.m00 = src->m01; mat3.m01 = src->m02; mat3.m02 = src->m03;
  mat3.m10 = src->m21; mat3.m11 = src->m22; mat3.m12 = src->m23;
  mat3.m20 = src->m31; mat3.m21 = src->m32; mat3.m22 = src->m33;
  dst->m10 = -Determinant3(&mat3) * invA;

  mat3.m00 = src->m00; /* mat3.m01 = src->m02; mat3.m02 = src->m03; */
  mat3.m10 = src->m20; /* mat3.m11 = src->m22; mat3.m12 = src->m23; */
  mat3.m20 = src->m30; /* mat3.m21 = src->m32; mat3.m22 = src->m33; */
  dst->m11 = Determinant3(&mat3) * invA;

  /* mat3.m00 = src->m00; */ mat3.m01 = src->m01; /* mat3.m02 = src->m03; */
  /* mat3.m10 = src->m20; */ mat3.m11 = src->m21; /* mat3.m12 = src->m23; */
  /* mat3.m20 = src->m30; */ mat3.m21 = src->m31; /* mat3.m22 = src->m33; */
  dst->m12 = -Determinant3(&mat3) * invA;

  /* mat3.m00 = src->m00; mat3.m01 = src->m01; */ mat3.m02 = src->m02;
  /* mat3.m10 = src->m20; mat3.m11 = src->m21; */ mat3.m12 = src->m22;
  /* mat3.m20 = src->m30; mat3.m21 = src->m31; */ mat3.m22 = src->m32;
  dst->m13 = Determinant3(&mat3) * invA;

  mat3.m00 = src->m01; mat3.m01 = src->m02; mat3.m02 = src->m03;
  mat3.m10 = src->m11; mat3.m11 = src->m12; mat3.m12 = src->m13;
  mat3.m20 = src->m31; mat3.m21 = src->m32; mat3.m22 = src->m33;
  dst->m20 = Determinant3(&mat3) * invA;

  mat3.m00 = src->m00; /* mat3.m01 = src->m02; mat3.m02 = src->m03; */
  mat3.m10 = src->m10; /* mat3.m11 = src->m12; mat3.m12 = src->m13; */
  mat3.m20 = src->m30; /* mat3.m21 = src->m32; mat3.m22 = src->m33; */
  dst->m21 = -Determinant3(&mat3) * invA;

  /* mat3.m00 = src->m00; */ mat3.m01 = src->m01; /* mat3.m02 = src->m03; */
  /* mat3.m10 = src->m10; */ mat3.m11 = src->m11; /* mat3.m12 = src->m13; */
  /* mat3.m20 = src->m30; */ mat3.m21 = src->m31; /* mat3.m22 = src->m33; */
  dst->m22 = Determinant3(&mat3) * invA;

  /* mat3.m00 = src->m00; mat3.m01 = src->m01; */ mat3.m02 = src->m02;
  /* mat3.m10 = src->m10; mat3.m11 = src->m11; */ mat3.m12 = src->m12;
  /* mat3.m20 = src->m30; mat3.m21 = src->m31; */ mat3.m22 = src->m32;
  dst->m23 = -Determinant3(&mat3) * invA;

  mat3.m00 = src->m01; mat3.m01 = src->m02; mat3.m02 = src->m03;
  mat3.m10 = src->m11; mat3.m11 = src->m12; mat3.m12 = src->m13;
  mat3.m20 = src->m21; mat3.m21 = src->m22; mat3.m22 = src->m23;
  dst->m30 = -Determinant3(&mat3) * invA;

  mat3.m00 = src->m00; /* mat3.m01 = src->m02; mat3.m02 = src->m03; */
  mat3.m10 = src->m10; /* mat3.m11 = src->m12; mat3.m12 = src->m13; */
  mat3.m20 = src->m20; /* mat3.m21 = src->m22; mat3.m22 = src->m23; */
  dst->m31 = Determinant3(&mat3) * invA;

  /* mat3.m00 = src->m00; */ mat3.m01 = src->m01; /* mat3.m02 = src->m03; */
  /* mat3.m10 = src->m10; */ mat3.m11 = src->m11; /* mat3.m12 = src->m13; */
  /* mat3.m20 = src->m20; */ mat3.m21 = src->m21; /* mat3.m22 = src->m23; */
  dst->m32 = -Determinant3(&mat3) * invA;

  /* mat3.m00 = src->m00; mat3.m01 = src->m01; */ mat3.m02 = src->m02;
  /* mat3.m10 = src->m10; mat3.m11 = src->m11; */ mat3.m12 = src->m12;
  /* mat3.m20 = src->m20; mat3.m21 = src->m21; */ mat3.m22 = src->m22;
  dst->m33 = Determinant3(&mat3) * invA;

  Transpose(NULL);
#endif

  return TRUE;
}


/*-----------------------------------------------
  カレントマトリックスのトランスを得る
-----------------------------------------------*/
void GetCurrentTrans(VECTOR *v)
{
  MATRIX *mat = currentMatrix;

  v->x = mat->m30;
  v->y = mat->m31;
  v->z = mat->m32;
}


/*-----------------------------------------------
  ベクトル＊マトリックス -> ベクトル
-----------------------------------------------*/
//
//                           |m00 m01 m02 m03|
//|X  Y  Z  W| = |x  y  z  1||m10 m11 m12 m13|
//                           |m20 m21 m22 m23|
//                           |m30 m31 m32 m33|

void ApplyMatrix(VECTOR *v1, VECTOR *v0)
{
  MATRIX *mat;
//  float w, oow;

  mat = currentMatrix;

  v1->x = (v0->x * mat->m00 + v0->y * mat->m10 + v0->z * mat->m20) + mat->m30;
  v1->y = (v0->x * mat->m01 + v0->y * mat->m11 + v0->z * mat->m21) + mat->m31;
  v1->z = (v0->x * mat->m02 + v0->y * mat->m12 + v0->z * mat->m22) + mat->m32;
  v1->w = (v0->x * mat->m03 + v0->y * mat->m13 + v0->z * mat->m23) + mat->m33;
/*
  if (w != 0.0f){
    oow = 1.0f / w;
  }
  else {
    oow = 1.0f;
  }
  v1->x *= oow;
  v1->y *= oow;
  v1->z *= oow;
  v1->w = w;
*/
}

void ApplyMatrix(MATRIX* mat, VECTOR *v1, VECTOR *v0)
{
  v1->x = (v0->x * mat->m00 + v0->y * mat->m10 + v0->z * mat->m20) + mat->m30;
  v1->y = (v0->x * mat->m01 + v0->y * mat->m11 + v0->z * mat->m21) + mat->m31;
  v1->z = (v0->x * mat->m02 + v0->y * mat->m12 + v0->z * mat->m22) + mat->m32;
  v1->w = (v0->x * mat->m03 + v0->y * mat->m13 + v0->z * mat->m23) + mat->m33;
}

/*-----------------------------------------------
  ベクトル＊マトリックス -> ベクトル（回転のみ）
-----------------------------------------------*/
//
//                           |m00 m01 m02  0|
//|X  Y  Z  1| = |x  y  z  1||m10 m11 m12  0|
//                           |m20 m21 m22  0|
//                           | 0   0   0   1|

void ApplyMatrixRot(VECTOR *v1, VECTOR *v0)
{
  MATRIX *mat;
  mat = currentMatrix;

  v1->x = (v0->x * mat->m00 + v0->y * mat->m10 + v0->z * mat->m20);
  v1->y = (v0->x * mat->m01 + v0->y * mat->m11 + v0->z * mat->m21);
  v1->z = (v0->x * mat->m02 + v0->y * mat->m12 + v0->z * mat->m22);
}


/*-----------------------------------------------
  ベクトル＊マトリックス -> ベクトル（平行移動のみ）
-----------------------------------------------*/
//
//                           | 1   0   0   0|
//|X  Y  Z  1| = |x  y  z  1|| 0   1   0   0|
//                           | 0   0   1   0|
//                           |m30 m31 m32  1|

void ApplyMatrixTrans(VECTOR *v1, VECTOR *v0)
{
  MATRIX *mat;
  mat = currentMatrix;

  v1->x = v0->x + mat->m30;
  v1->y = v0->y + mat->m31;
  v1->z = v0->z + mat->m32;
}


/*-----------------------------------------------
  ２ベクトルの外積
-----------------------------------------------*/
void OuterProduct( VECTOR *dst, VECTOR *v1, VECTOR *v2 )
{
  dst->x = v1->y * v2->z - v1->z * v2->y;
  dst->y = v1->z * v2->x - v1->x * v2->z;
  dst->z = v1->x * v2->y - v1->y * v2->x;
}


/*-----------------------------------------------
  ２ベクトルの内積
-----------------------------------------------*/
float InnerProduct(VECTOR *v1, VECTOR *v2)
{
  return v1->x * v2->x + v1->y * v2->y + v1->z * v2->z;
}


/*-----------------------------------------------
  単位ベクトル
-----------------------------------------------*/
void UnitVector(VECTOR *v)
{
  float d;

  d = (float)sqrt(v->x * v->x + v->y * v->y + v->z * v->z);
  if (d < 0.00001f){
    v->x = 1.0f;
    v->y = 0.0f;
    v->z = 0.0f;
    return;
  }
  d = 1.0f / d;
  v->x *= d;
  v->y *= d;
  v->z *= d;
}


/*-----------------------------------------------
  カメラマトリックス生成
-----------------------------------------------*/
void LookAt(VECTOR *cam_pos, VECTOR *cam_int)
{
  VECTOR vec;
  float x, y;

  /* 注視点->ポジションのベクトル */
  vec.x = cam_pos->x - cam_int->x;
  vec.y = cam_pos->y - cam_int->y;
  vec.z = cam_pos->z - cam_int->z;

  /* 回転角 */
  y = (float)atan2(vec.x, vec.z);
  x = (float)-atan2(vec.y, sqrt(vec.x*vec.x + vec.z*vec.z));

  RotX(-x);
  RotY(-y);
  Trans(-cam_pos->x, -cam_pos->y, -cam_pos->z);
}

void  LookAtWithRoll(VECTOR *cam_pos, VECTOR *cam_int, float roll)
{
  VECTOR vec;
  float x, y;

  /* 注視点->ポジションのベクトル */
  vec.x = cam_pos->x - cam_int->x;
  vec.y = cam_pos->y - cam_int->y;
  vec.z = cam_pos->z - cam_int->z;

  /* 回転角 */
  y = (float)atan2(vec.x, vec.z);
  x = (float)-atan2(vec.y, sqrt(vec.x*vec.x + vec.z*vec.z));

  RotX(-x);
  RotY(-y);
  Trans(-cam_pos->x, -cam_pos->y, -cam_pos->z);
  RotZ(-roll);
}


/*---------------------------------------------------------------------*
 *	透視投影
 *---------------------------------------------------------------------*/

void PerspectiveViewScreen(ViewScreen* vs, float scrz, float ax, float ay,
	float xcenter, float ycenter, float zbufsmall, float zbufbig, float zD, float zF, float fognearz, float fogfarz, float W, float H)
{

		// D = nearz, F = farz, W = width/2, H = Height/2
		//
		// Perspective
		//
		//     | scrz*ax/W          0            0           0 |
		// m = |    0       scrz*ay/H            0           0 | 
		//     |    0               0  (F+D)/(F-D)  -2FD/(F-D) |
		//     |    0               0            1           0 |
		// z = D, zh = -D, z = F, zh = F		Ｚクリップの範囲 zh :(-D, F)


	float p, q, fscale, foffset, zscale, zoffset, qa, qb, za, zb;

		p = (zF+zD)/(zF-zD);
		q = -2*zF*zD/(zF-zD);

		SetVector( &vs->a, scrz * ax / W, scrz * ay / H, p, 1.0f );
		SetVector( &vs->b, 0.0f, 0.0f, q, 0.0f);
		SetVector( &vs->ra, 1/(scrz * ax / W), 1/(scrz * ay / H), 1/(p), 0.0f );
		SetVector( &vs->rb, 0.0f, 0.0f, -q/(p), 0.0f);


		za = zD * zF * (zbufbig - zbufsmall) / (zF - zD);
		zb = (zbufsmall * zF - zbufbig * zD) / (zF - zD);

		zscale = za / q;
		zoffset = zb - za * p / q;


		if ( fognearz < fogfarz ) {
			qa =  fogfarz * fognearz / (fogfarz - fognearz) * 255.0f;
			qb =  -fognearz / (fogfarz - fognearz) * 255.0f;
		}
		else {
			qa = 0.0f;
			qb = 255.0f;
		};

		fscale = qa / q;
		foffset = qb - qa * p / q;

		SetVector( &vs->v, xcenter, ycenter, zoffset, fscale );
		SetVector( &vs->t, W, H, zscale, foffset);

		SetVector( &vs->cmin, -W + xcenter, -H + ycenter, -zD, 1.0f );
		SetVector( &vs->cmax,  W + xcenter,  H + ycenter,  zF, -zb );
}


/*---------------------------------------------------------------------*
 *	平行投影(垂直投影)
 *---------------------------------------------------------------------*/

void OrthoMatrix(float basex, float basey, float width, float height, float Znear, float Zfar )
{
	MATRIX* mat = currentMatrix;

	float right = basex+width;
	float left = basex;
	float top = basey;
	float bottom = basey+height;

	mat->m00 = 2.0f / (right - left);
	mat->m01 = 0.0f;
	mat->m02 = 0.0f;
	mat->m03 = 0.0f;

	mat->m10 = 0.0f;
	mat->m11 = 2.0f / (top - bottom);
	mat->m12 = 0.0f;
	mat->m13 = 0.0f;

	mat->m20 = 0.0f;
	mat->m21 = 0.0f;
	mat->m22 = 1.0f / (Znear - Zfar );
	mat->m23 = 0.0f;

	mat->m30 = -(right + left) / (right - left);
	mat->m31 = -(top + bottom) / (top - bottom);
	mat->m32 = -Znear / (Znear - Zfar );
	mat->m33 = 1.0f;

	/*
	mat->m00 = 0.1f;
	mat->m01 = 0.0f;
	mat->m02 = 0.0f;
	mat->m03 = 0.0f;

	mat->m10 = 0.0f;
	mat->m11 = 0.1f;
	mat->m12 = 0.0f;
	mat->m13 = 0.0f;

	mat->m20 = 0.0f;
	mat->m21 = 0.0f;
	mat->m22 = 1.0f;
	mat->m23 = 0.0f;

	mat->m30 = 0.0f;
	mat->m31 = 0.0f;
	mat->m32 = 0.0f;
	mat->m33 = 1.0f;
	*/
}


/*-----------------------------------------------
  2点間の距離を求める(3D)
-----------------------------------------------*/

float GetVectorDistance( VECTOR *v1, VECTOR *v2 )
{
	float x,y,z;
	x = (float)fabs( v1->x - v2->x );
	y = (float)fabs( v1->y - v2->y );
	z = (float)fabs( v1->z - v2->z );
	return (float)sqrt( x * x + y * y + z * z );
}


/*-----------------------------------------------
  任意の点がポリゴン(四角形)内にあるか?(2D)
-----------------------------------------------*/

int HasPoint2D( float x, float y, VECTOR *v )
{
	int i,j,r,npol;
	r=0;npol=4;
	for( i=0,j=npol-1; i<npol; j=i++ ) {
		if (((( v[i].y <=y )
			&& ( y<v[j].y ))
			|| (( v[j].y <= y )
			&& ( y<v[i].y )))
			&& ( x<(v[j].x-v[i].x)*(y-v[i].y)/(v[j].y-v[i].y)+v[i].x))
			r=!r;
	}
	return r;
}


/*-----------------------------------------------
  直線と平面の交点を求める(3D)
-----------------------------------------------*/

int IntersectLinePlane( VECTOR *lpoint, VECTOR *lvector, VECTOR *ppoint, VECTOR *pnormal, VECTOR *result )
{
	float top,bottom,mul;
	top = ( ppoint->x - lpoint->x ) * pnormal->x
		+ ( ppoint->y - lpoint->y ) * pnormal->y
		+ ( ppoint->z - lpoint->z ) * pnormal->z;
	bottom  = lvector->x * pnormal->x
			+ lvector->y * pnormal->y
			+ lvector->z * pnormal->z;
	if ( bottom == 0.0f ) return -1;		// 平行
	mul = top / bottom;
	result->x = lpoint->x + mul * lvector->x;
	result->y = lpoint->y + mul * lvector->y;
	result->z = lpoint->z + mul * lvector->z;
	return 0;
}


/*-----------------------------------------------
  任意の点がポリゴン(四角形)内にあるか?(3D)
-----------------------------------------------*/

static int HasPoint3DSub( float x, float y, VECTOR *v, int p1, int p2 )
{
	int i,j,r,npol;
	float *ti;
	float *tj;
	r=0;npol=4;
	for( i=0,j=npol-1; i<npol; j=i++ ) {
		ti = (float *)&v[i]; tj = (float *)&v[j];
		if (((( ti[p2] <=y )
			&& ( y<tj[p2] ))
			|| (( tj[p2] <= y )
			&& ( y<ti[p2] )))
			&& ( x<(tj[p1]-ti[p1])*(y-ti[p2])/(tj[p2]-ti[p2])+ti[p1]))
			r=!r;
	}
	return r;
}


int HasPoint3D( VECTOR *p, VECTOR *v )
{
	if ( HasPoint3DSub( p->x, p->y, v, 0, 1 ) )
//		if ( HasPoint3DSub( p->x, p->z, v, 0, 2 ) ) return -1;
			if ( HasPoint3DSub( p->y, p->z, v, 1, 2 ) ) return -1;
	return 0;
}


void GetTargetAngle( VECTOR *ang, VECTOR *src, VECTOR *target )
{
	//--------------------------------------------------
	//	カメラのポジション->注視点の回転角を求める
	//--------------------------------------------------
	VECTOR vec;
	/* 注視点->ポジションのベクトル */
	vec.x = src->x - target->x;
	vec.y = src->y - target->y;
	vec.z = src->z - target->z;
	/* 回転角 */
	ang->y = (float)-atan2( vec.x, vec.z );
	ang->x = (float)-atan2( vec.y, sqrt(vec.x*vec.x + vec.z*vec.z) );
	ang->z = 0.0f;
	ang->w = 0.0f;
}
	
