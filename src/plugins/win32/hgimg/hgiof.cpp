//
//		Polygon draw lib (full color)
//			onion software/onitama 2001/6
//
#include <stdio.h>
#include <windows.h>
#include <math.h>
#include "hginc.h"
#include "hgio.h"
#include "hgiof.h"
#include "sysreq.h"

//
//	function prototypes
//
void setDest( void *dest, int sx, int sy );
void releaseDest( void );

void ClearDest( int mode, int color, int tex );
void DrawLineF4( POLY4 *poly );
void DrawLineF4G( POLY4G *poly );
void DrawPolygonF4( POLY4 *lpPolyData );
void DrawPolygonTex( POLY4 *lpPolyData );
void DrawSpriteTex( P_SPRITE *spr );
void DrawBGSpriteTex( P_BGSPR *bg );
void DrawFontSprite( int x, int y, int cx, int cy, int px, char *mes, int attr, int tex );

void TexInit( void );
void TexTerm( void );
int RegistTex( char *data, int sx, int sy, int width, int height, int sw );
int GetNextTexID( void );
void DeleteTex( int id );
void ChangeTex( int id );
TEXINF *GetTex( int id );
void SetSrcTex( void *src, int sx, int sy );




//		Data
//
static		TEXINF texinf[TEXINF_MAX];

//		Settings
//
static		SCANDATA *scanData;
static		char *lpDest;		// 描画画面のバッファ
static		int nDestWByte;		// 描画座標幅byte数
static		int nDestWidth;		// 描画座標幅
static		int nDestHeight;	// 描画座標高さ
static		int DestSize;		// 描画画面バッファのサイズ

static		IAXIS sprclip1;		// 2D表示用クリップ領域(左上)
static		IAXIS sprclip2;		// 2D表示用クリップ領域(右下)

static		char *lpTex;		// テクスチャのバッファ
static		int curtex;			// current texID
static		int nTexWidth;		// テクスチャ幅
static		int nTexWByte;		// テクスチャ幅byte数
static		int nTexHeight;		// テクスチャ高さ

static		char ck1;			// カラーキー1
static		char ck2;			// カラーキー2
static		char ck3;			// カラーキー3

/*------------------------------------------------------------*/
/*
		interface
*/
/*------------------------------------------------------------*/

void hgiof_init( HGProc *p )
{
	//		関数ポインタ設定
	//
	p->setDest = setDest;
	p->releaseDest = releaseDest;
	p->ClearDest = ClearDest;
	p->DrawLineF4 = DrawLineF4;
	p->DrawLineF4G = DrawLineF4G;
	p->DrawPolygonF4 = DrawPolygonF4;
	p->DrawPolygonTex = DrawPolygonTex;
	*(int *)&p->DrawPolygonF4G = (int)DrawPolygonF4;
	*(int *)&p->DrawPolygonTexG = (int)DrawPolygonTex;
	p->DrawSpriteTex = DrawSpriteTex;
	p->DrawBGSpriteTex = DrawBGSpriteTex;
	p->DrawFontSprite = DrawFontSprite;

	p->TexInit = TexInit;
	p->TexTerm = TexTerm;
	p->RegistTex = RegistTex;
	p->GetNextTexID = GetNextTexID;
	p->DeleteTex = DeleteTex;
	p->ChangeTex = ChangeTex;
	p->GetTex = GetTex;
	p->SetSrcTex = SetSrcTex;

	//		バッファ初期化
	//
	scanData = NULL;
	lpDest = NULL;
	lpTex = NULL;

	//		カラーキー初期化
	//
	ck1 = 0; ck2 = 0; ck3 = 0;

	//		テクスチャ情報初期化
	//
	TexInit();
}

void hgiof_term( void )
{
	releaseDest();
	TexTerm();
}


static void setDest( void *dest, int sx, int sy )
{
	releaseDest();
	lpDest = (char *)dest;
	nDestWidth = sx;
	nDestWByte = sx*3;
	nDestHeight = sy;
	DestSize = sx*sy*3;
	scanData = (SCANDATA *)malloc( sizeof( SCANDATA ) * nDestHeight );
	setIAXIS( sprclip1, 0, 0 );
	setIAXIS( sprclip2, nDestWidth, nDestHeight );
}


static void releaseDest( void )
{
	if ( lpDest == NULL ) return;
	free( scanData );
	lpDest = NULL;
}


static void ClearDestSub( char *dst, int sx, int sy, int color )
{
	//		color fill(R,G,B)
	//
	char *p;
	char a1,a2,a3;
	int i;
	p = dst;
	a1 = color & 0xff;
	a2 = (color>>8) & 0xff;
	a3 = (color>>16) & 0xff;
	for(i=0;i<sx;i++) {
		*p++ = a1;
		*p++ = a2;
		*p++ = a3;
	}
	i = 2;
	p = dst;
	while(1) {
		if ( i>sy ) break;
		p += nDestWByte;
		memcpy( p, dst, sx*3 );
		i++;
	}
}
	

static void BlurDestSub( char *dst, int sx, int sy, int color )
{
	//		smart blur
	//
	unsigned char *p;
	short a1,a2,a3;
	short alp;
	int i;
	p = (unsigned char *)dst;
	alp = color;
	for(i=0;i<sx*sy;i++) {
		a1 = (short)p[0] - alp;if (a1<0) a1=0;
		a2 = (short)p[1] - alp;if (a2<0) a2=0;
		a3 = (short)p[2] - alp;if (a3<0) a3=0;
		p[0] = (unsigned char)a1;
		p[1] = (unsigned char)a2;
		p[2] = (unsigned char)a3;
		p+=3;
	}
}


static void ClearDest( int mode, int color, int tex )
{
	if ( mode == 1 ) {
		ClearDestSub( lpDest, nDestWidth, nDestHeight, color );
	}
	if ( mode == 2 ) {
		TEXINF *t;
		t = GetTex( tex );
		memcpy( lpDest, t->data, DestSize );
	}
	if ( mode == 3 ) {
		BlurDestSub( lpDest, nDestWidth, nDestHeight, color );
	}
}


/*------------------------------------------------------------*/
/*
		Polygon Draw Routines
*/
/*------------------------------------------------------------*/

//-----------------------------------------------------
//  Calc Polygon ScanLine Data
//
static void ScanLine(POLY4 *lpPolyData,  // ポリゴンデータ
			  SCANDATA *lpScanData, // エッジ座標のスキャンデータ
			  int nScrWidth, // 描画画面幅
			  int nScrHeight, // 描画画面高さ
			  int *lpnStartY, // 描画を開始する Y 座標(out)
			  int *lpnEndY // 描画を終了する Y 座標(out)
			  )
{
	int i,y,v1y,v2y,endY;
	IAXIS2 *fv1;
	IAXIS2 *fv2;
	IAXIS2 *ftmp;

	// 描画開始位置、終了位置初期化
	*lpnStartY = nScrHeight;
	*lpnEndY = -1;
	
	// スキャンデータ初期化
	for(i=0;i< nScrHeight;i++){
		lpScanData[i].minX = nScrWidth;
		lpScanData[i].maxX = -1;
	}
	
	// ポリゴンの各辺毎にスキャンする
	for(i=0;i< POLY4N;i++){

		// 頂点番号セット
		fv1 = &lpPolyData->v[i];
		if ( (i+1)<POLY4N ) { fv2=fv1+1; } else { fv2=&lpPolyData->v[0]; }

		// 頂点 V1 と V2 の y 軸が同じ時(水平線の時)
		if( fv1->y == fv2->y ){

			y = fv1->y;
			
			// 範囲外の時はチェックしない
			if((y <  nScrHeight )&&( y >= 0)){
				
				// スタート位置計算
				*lpnStartY = min(y,*lpnStartY);
				*lpnEndY = max(y,*lpnEndY);
				
				// 頂点 V1 の方が左になるようにする
				if( fv1->x > fv2->x ){
					ftmp = fv1;
					fv1 = fv2;
					fv2 = ftmp;
				}
				
				// Max と Min の計算
				if((y >=0 )&&( y <  nScrHeight)){
					
					if( fv1->x <  lpScanData[y].minX){
						lpScanData[y].minX = fv1->x;
						lpScanData[y].minTx = fv1->tx;
						lpScanData[y].minTy = fv1->ty;
					}
					
					if( fv2->x > lpScanData[y].maxX){
						lpScanData[y].maxX = fv2->x;
						lpScanData[y].maxTx = fv2->tx;
						lpScanData[y].maxTy = fv2->ty;
					}
				}
			}
		}
		else{ // 頂点 v1 と v2 の y 座標が異なる場合
			
			// 頂点 V1 の方が上になるようにする
			if( fv1->y > fv2->y ) {
				ftmp = fv1;
				fv1 = fv2;
				fv2 = ftmp;
			}
			
			v1y = fv1->y;
			v2y = fv2->y;
			
			// 範囲外の時はチェックしない
			if((v1y <  nScrHeight )||( v2y >= 0)){
				
				// スタート位置計算
				*lpnStartY = min(v1y,*lpnStartY);
				*lpnEndY = max(v2y,*lpnEndY);
				
				// y が 1 増加した時の描画座標での x の変位と
				// テクスチャ座標での x と y の変位を計算
				
				double x,dx;  // 描画座標計算用
				double dd;
				double tx,ty,tdx,tdy; // テクスチャ座標計算用
				
				//x 方向の変位計算
				dd = 1.0f / (double)(v2y-v1y);
				dx = (double)( fv2->x - fv1->x )*dd;
				// テクスチャ座標の変位計算
				tdx = (double)(fv2->tx - fv1->tx)*dd;
				tdy = (double)(fv2->ty - fv1->ty)*dd;
				
				// 初期描画座標セット
				x = (double)fv1->x;
				
				// 初期テクスチャ座標セット
				tx = (double)fv1->tx; 
				ty = (double)fv1->ty;
				
				// サーチ範囲計算
				endY = min(nScrHeight-1,v2y);
				
				// 上から順にサーチ開始
				for(y = v1y; y <= endY; y++){
					
					// Max と Min の計算
					if(y>=0){
						
						if( x < lpScanData[y].minX){
							lpScanData[y].minX = (int)x;
							lpScanData[y].minTx = (int)tx;
							lpScanData[y].minTy = (int)ty;
						}
						
						if( x > lpScanData[y].maxX){
							lpScanData[y].maxX = (int)x;
							lpScanData[y].maxTx = (int)tx;
							lpScanData[y].maxTy = (int)ty;
						}
						
					}
					
					// 座標移動
					x += dx;
					tx += tdx;
					ty += tdy;
				}
			}
		}
	}
	
	// スタート位置計算
	*lpnStartY = max(0,*lpnStartY);
	*lpnEndY = min(nScrHeight-1,*lpnEndY);
}


static void DrawPolygonF4( POLY4 *lpPolyData )
{
	int x,y; // ループ用
	int nStartY,nEndY; // 描画開始を開始する Y 座標、終了座標
	int maxX,minX;  // エッジ座標の最大最小値
//	char *p;
	short color1;
	unsigned char color2;

	DWORD dwScanData,dwDest; // エッジの座標データと転送先のベースアドレス
	int x0,x1,x2,y0,y1,y2;

	short alpha,alphaop,ialpha;
	short cc1,cc2,cc3,a1,a2,a3;
	unsigned char *up;

	// 裏表判定
	x0 = lpPolyData->v[0].x;
	x1 = lpPolyData->v[1].x;
	x2 = lpPolyData->v[2].x;
	y0 = lpPolyData->v[0].y;
	y1 = lpPolyData->v[1].y;
	y2 = lpPolyData->v[2].y;
	if ( (x0 - x1) * (y1 - y2) - (x1 - x2) * (y0 - y1) >= 0 ) return;

	// エッジの座標のスキャン
	ScanLine(lpPolyData, scanData,nDestWidth,nDestHeight,&nStartY,&nEndY);
	
	// 範囲外なら描画しない
	if(nStartY >= nDestHeight || nEndY < 0) return;
	
	// Fill Color
	color1 = (short)(lpPolyData->color & 0xffff);
	color2 = (unsigned char)((lpPolyData->color & 0xff0000 ) >>16 );

	// 転送先バッファとスキャンデータのベースアドレスセット
	// 転送先バッファ(DIB)は上下が反転しているのに注意((nDestHeight-1-nStartY)の所)

	dwDest = (DWORD)lpDest + (DWORD)((nDestHeight-1-nStartY)*nDestWByte);
	dwScanData = (DWORD)scanData + (DWORD)(nStartY*sizeof(SCANDATA));
	
	//	透明設定を判定
	//
	if ( lpPolyData->alpha != 0x100 ) {
		alphaop = lpPolyData->alpha>>8;
		alpha = lpPolyData->alpha & 255;
		cc1 = ((color1 & 0xff) * alpha )>>8;
		cc2 = (((color1>>8)&0xff) * alpha )>>8;
		cc3 = ((short)(color2) * alpha )>>8;

		if ( alphaop == 0 ) goto f4_blend;
		if ( alphaop == 2 ) goto f4_modulate;
		if ( alphaop == 3 ) goto f4_substract;
	}

	// nStartY から nEndY まで上から順に描画
	for(y = nStartY; y < nEndY; y++){
	
		// エッジの(転送先画像(dwDest)上での)座標セット
		minX = *(int *)(dwScanData);
		maxX = *(int *)(dwScanData+4);

		if(minX < 0) minX = 0;
		if(maxX >= 0){

			// 左から右に横に描画していく
			maxX = min(nDestWidth-1,maxX);
			up = (unsigned char *)dwDest + minX*3;
			for(x = minX; x < maxX; x++){
	
			// コピー
			*(short *)up = color1;
			up[2] = color2; up+=3;
	
			}
		}
		// バッファとスキャンデータのベースアドレス更新
		dwDest -= nDestWByte;
		dwScanData += sizeof(SCANDATA);
	}
	return;

f4_blend:
	ialpha = 0xff - alpha;
	for(y = nStartY; y < nEndY; y++){
		minX = *(int *)(dwScanData);
		maxX = *(int *)(dwScanData+4);
		if(minX < 0) minX = 0;
		if(maxX >= 0){
			maxX = min(nDestWidth-1,maxX);
			up = (unsigned char *)dwDest + minX*3;
			for(x = minX; x < maxX; x++){
				a1=((((short)up[0])*ialpha)>>8)+cc1;
				a2=((((short)up[1])*ialpha)>>8)+cc2;
				a3=((((short)up[2])*ialpha)>>8)+cc3;
				up[0]=(unsigned char)a1;
				up[1]=(unsigned char)a2;
				up[2]=(unsigned char)a3;
				up+=3;
			}
		}
		dwDest -= nDestWByte;
		dwScanData += sizeof(SCANDATA);
	}
	return;

f4_modulate:
	for(y = nStartY; y < nEndY; y++){
		minX = *(int *)(dwScanData);
		maxX = *(int *)(dwScanData+4);
		if(minX < 0) minX = 0;
		if(maxX >= 0){
			maxX = min(nDestWidth-1,maxX);
			up = (unsigned char *)dwDest + minX*3;
			for(x = minX; x < maxX; x++){
				a1=cc1+(short)up[0];if (a1>255) a1=255;
				a2=cc2+(short)up[1];if (a2>255) a2=255;
				a3=cc3+(short)up[2];if (a3>255) a3=255;
				up[0]=(unsigned char)a1;
				up[1]=(unsigned char)a2;
				up[2]=(unsigned char)a3;
				up+=3;
			}
		}
		dwDest -= nDestWByte;
		dwScanData += sizeof(SCANDATA);
	}
	return;
f4_substract:
	for(y = nStartY; y < nEndY; y++){
		minX = *(int *)(dwScanData);
		maxX = *(int *)(dwScanData+4);
		if(minX < 0) minX = 0;
		if(maxX >= 0){
			maxX = min(nDestWidth-1,maxX);
			up = (unsigned char *)dwDest + minX*3;
			for(x = minX; x < maxX; x++){
				a1=(short)up[0]-cc1;if (a1<0) a1=0;
				a2=(short)up[1]-cc2;if (a2<0) a2=0;
				a3=(short)up[2]-cc3;if (a3<0) a3=0;
				up[0]=(unsigned char)a1;
				up[1]=(unsigned char)a2;
				up[2]=(unsigned char)a3;
				up+=3;
			}
		}
		dwDest -= nDestWByte;
		dwScanData += sizeof(SCANDATA);
	}
	return;


}


static void DrawPolygonTex( POLY4 *lpPolyData )
{
	int i2,x,y; // ループ用
	int nStartY,nEndY; // 描画開始を開始する Y 座標、終了座標
	int maxX,minX;  // エッジ座標の最大最小値
	int x0,x1,x2,y0,y1,y2;
	char *p;
	char *srcp;
	DWORD tx,ty,tdx,tdy; 

	short alpha,alphaop,ialpha,a1,a2,a3;
	unsigned char *up;
	unsigned char *usrcp;

	DWORD dwTexSize; // テクスチャのサイズ
	DWORD dwScanData,dwDest; // エッジの座標データと転送先のベースアドレス

	//		Texture select
	if ( lpPolyData->tex != curtex ) ChangeTex( lpPolyData->tex );

	// 裏表判定
	x0 = lpPolyData->v[0].x;
	x1 = lpPolyData->v[1].x;
	x2 = lpPolyData->v[3].x;
	y0 = lpPolyData->v[0].y;
	y1 = lpPolyData->v[1].y;
	y2 = lpPolyData->v[3].y;
	if ( (x0 - x1) * (y1 - y2) - (x1 - x2) * (y0 - y1) >= 0 ) return;

	// エッジの座標のスキャン
	dwTexSize = (DWORD)(nTexHeight*nTexWByte);
	ScanLine( lpPolyData, scanData, nDestWidth, nDestHeight, &nStartY, &nEndY );
	
	// 範囲外なら描画しない
	if(nStartY >= nDestHeight || nEndY < 0) return;
	
	// 転送先バッファとスキャンデータのベースアドレスセット
	// 転送先バッファ(DIB)は上下が反転しているのに注意((nDestHeight-1-nStartY)の所)
	p = (char *)lpDest + ((nDestHeight-1-nStartY)*nDestWByte);
	dwDest = (DWORD)p;
	dwScanData = (DWORD)scanData + (DWORD)(nStartY*sizeof(SCANDATA));

	//	透明設定を判定
	//
	if ( lpPolyData->alpha != 0x100 ) {
		alphaop = lpPolyData->alpha>>8;
		alpha = lpPolyData->alpha & 255;
		if ( alphaop == 0 ) goto p4_blend;
		if ( alphaop == 2 ) goto p4_modulate;
		if ( alphaop == 3 ) goto p4_substract;
	}
	if ( lpPolyData->attr & NODE_ATTR_COLKEY ) goto p4trans;

	//
	//	透明色なし描画
	//
	//
	// nStartY から nEndY まで上から順に描画
	for(y = nStartY; y < nEndY; y++){
	
	// エッジの(転送先画像(dwDest)上での)座標セット
	minX = *(LPLONG)(dwScanData);
	maxX = *(LPLONG)(dwScanData+4);

	if(maxX >= 0){
	
	// テクスチャ座標計算用変数(16 bit 固定少数)
	
	tx = *(LPLONG)(dwScanData+8);
	ty = *(LPLONG)(dwScanData+12);
	tx <<= 16; 
	ty <<= 16;
	
	// 描画座標で x  が 1 増加した時のテクスチャ座標での
	// x と y の変位を計算
	if(maxX != minX){ 
	
	i2 = 0x10000 / (maxX-minX); // DWORD -> WORD 変換
	tdx = ((*(LPLONG)(dwScanData+16)-*(LPLONG)(dwScanData+8)))*i2;
	tdy = ((*(LPLONG)(dwScanData+20)-*(LPLONG)(dwScanData+12)))*i2;

	}
	else { // 一点の時
	tdx = 0;
	tdy = 0;
	}
	
	// 左側エッジ(minX)が 0 より小さいなら 0 になるまで回す

	while(minX < 0){
	minX++;
	tx += tdx;
	ty += tdy;
	}
	
	// 左から右に横に描画していく
	maxX = min(nDestWidth-1,maxX);
	p = (char *)(dwDest) + minX*3;
	for(x = minX; x < maxX; x++){
	
	// テクスチャデータを読み込む点を計算
	//dwReadPoint = (DWORD)(nTexHeight-1-(ty>>5))*nTexWidth+(tx>>5);
	//dwReadPoint = (DWORD)(nTexHeight-1-(ty>>16))*nTexWidth+(tx>>16);
	//dwReadPoint = (DWORD)((ty>>16))*nTexWByte+((tx>>16)*3);
//	i2 = ((tx>>16)*3);
	i2 = ((ty>>16)*nTexWByte)+((tx>>16)<<2);
	
	// コピー
	if( i2 < (int)dwTexSize) {
		srcp = (char *)lpTex + i2 + 1;
		*(short *)p = *(short *)srcp; p+=2; srcp+=2;
		*p++ = *srcp++;
	}
	
	// テクスチャ座標移動
	tx += tdx;
	ty += tdy;
	}
	}
	
	// バッファとスキャンデータのベースアドレス更新
	dwDest -= nDestWByte;
	dwScanData += sizeof(SCANDATA);
	}

	return;

p4trans:
	//
	//	透明色あり描画
	//
	//
	// nStartY から nEndY まで上から順に描画
	for(y = nStartY; y < nEndY; y++){
	
	// エッジの(転送先画像(dwDest)上での)座標セット
	minX = *(LPLONG)(dwScanData);
	maxX = *(LPLONG)(dwScanData+4);

	if(maxX >= 0){

		// テクスチャ座標計算用変数(16 bit 固定少数)
	
		tx = *(LPLONG)(dwScanData+8);
		ty = *(LPLONG)(dwScanData+12);
		tx <<= 16; 
		ty <<= 16;
	
		// 描画座標で x  が 1 増加した時のテクスチャ座標での
		// x と y の変位を計算
		if(maxX != minX){ 
	
			i2 = 0x10000 / (maxX-minX); // DWORD -> WORD 変換
			tdx = ((*(LPLONG)(dwScanData+16)-*(LPLONG)(dwScanData+8)))*i2;
			tdy = ((*(LPLONG)(dwScanData+20)-*(LPLONG)(dwScanData+12)))*i2;

		}
		else { // 一点の時
			tdx = 0;
			tdy = 0;
		}
	
		// 左側エッジ(minX)が 0 より小さいなら 0 になるまで回す

		while(minX < 0){
			minX++;
			tx += tdx;
			ty += tdy;
		}
	
		// 左から右に横に描画していく
		maxX = min(nDestWidth-1,maxX);
		p = (char *)(dwDest + minX*3);
		for(x = minX; x < maxX; x++) {

			// テクスチャデータを読み込む点を計算
			i2 = ((ty>>16)*nTexWByte)+((tx>>16)<<2);
	
			// コピー
			if( i2 < (int)dwTexSize) {
				srcp = (char *)lpTex + i2;
				if ( *srcp++ ) {
					*(short *)p = *(short *)srcp; p+=2; srcp+=2;
					*p++ = *srcp++;
				}
				else p+=3;
			}
		
			// テクスチャ座標移動
			tx += tdx;
			ty += tdy;
		}
	}
	
	// バッファとスキャンデータのベースアドレス更新
	dwDest -= nDestWByte;
	dwScanData += sizeof(SCANDATA);
	}
	return;

p4_blend:
	//
	//	透明色あり描画(BLEND)
	//
	//
	ialpha = 0xff - alpha;
	for(y = nStartY; y < nEndY; y++){
		minX = *(LPLONG)(dwScanData);
		maxX = *(LPLONG)(dwScanData+4);
		if(maxX >= 0){
		tx = *(LPLONG)(dwScanData+8);
		ty = *(LPLONG)(dwScanData+12);
		tx <<= 16; 
		ty <<= 16;
		if(maxX != minX){ 
			i2 = 0x10000 / (maxX-minX); // DWORD -> WORD 変換
			tdx = ((*(LPLONG)(dwScanData+16)-*(LPLONG)(dwScanData+8)))*i2;
			tdy = ((*(LPLONG)(dwScanData+20)-*(LPLONG)(dwScanData+12)))*i2;
		}
		else {
			tdx = 0;
			tdy = 0;
		}
		while(minX < 0){
			minX++;
			tx += tdx;
			ty += tdy;
		}
		maxX = min(nDestWidth-1,maxX);
		up = (unsigned char *)(dwDest + minX*3);
		for(x = minX; x < maxX; x++) {
			i2 = ((ty>>16)*nTexWByte)+((tx>>16)<<2);
			if( i2 < (int)dwTexSize) {
				usrcp = (unsigned char *)lpTex + i2;
				if ( *usrcp ) {
					a1=((((short)usrcp[1])*alpha)+(((short)up[0])*ialpha))>>8;
					a2=((((short)usrcp[2])*alpha)+(((short)up[1])*ialpha))>>8;
					a3=((((short)usrcp[3])*alpha)+(((short)up[2])*ialpha))>>8;
					*up++=(unsigned char)a1;*up++=(unsigned char)a2;*up++=(unsigned char)a3;
				}
				else up+=3;
			}
			tx += tdx;
			ty += tdy;
		}
	}
	dwDest -= nDestWByte;
	dwScanData += sizeof(SCANDATA);
	}
	return;

p4_modulate:
	//
	//	透明色あり描画(MODULATE)
	//
	//
	for(y = nStartY; y < nEndY; y++){
		minX = *(LPLONG)(dwScanData);
		maxX = *(LPLONG)(dwScanData+4);
		if(maxX >= 0){
		tx = *(LPLONG)(dwScanData+8);
		ty = *(LPLONG)(dwScanData+12);
		tx <<= 16; 
		ty <<= 16;
		if(maxX != minX){ 
			i2 = 0x10000 / (maxX-minX); // DWORD -> WORD 変換
			tdx = ((*(LPLONG)(dwScanData+16)-*(LPLONG)(dwScanData+8)))*i2;
			tdy = ((*(LPLONG)(dwScanData+20)-*(LPLONG)(dwScanData+12)))*i2;
		}
		else {
			tdx = 0;
			tdy = 0;
		}
		while(minX < 0){
			minX++;
			tx += tdx;
			ty += tdy;
		}
		maxX = min(nDestWidth-1,maxX);
		up = (unsigned char *)(dwDest + minX*3);
		for(x = minX; x < maxX; x++) {
			i2 = ((ty>>16)*nTexWByte)+((tx>>16)<<2);
			if( i2 < (int)dwTexSize) {
				usrcp = (unsigned char *)lpTex + i2;
				if ( *usrcp ) {
					a1=((((short)usrcp[1])*alpha)>>8)+(short)up[0];
					if (a1>255) a1=255;
					a2=((((short)usrcp[2])*alpha)>>8)+(short)up[1];
					if (a2>255) a2=255;
					a3=((((short)usrcp[3])*alpha)>>8)+(short)up[2];
					if (a3>255) a3=255;
					*up++=(unsigned char)a1;*up++=(unsigned char)a2;*up++=(unsigned char)a3;
				}
				else up+=3;
			}
			tx += tdx;
			ty += tdy;
		}
	}
	dwDest -= nDestWByte;
	dwScanData += sizeof(SCANDATA);
	}
	return;

p4_substract:
	//
	//	透明色あり描画(MODULATE)
	//
	//
	for(y = nStartY; y < nEndY; y++){
		minX = *(LPLONG)(dwScanData);
		maxX = *(LPLONG)(dwScanData+4);
		if(maxX >= 0){
		tx = *(LPLONG)(dwScanData+8);
		ty = *(LPLONG)(dwScanData+12);
		tx <<= 16; 
		ty <<= 16;
		if(maxX != minX){ 
			i2 = 0x10000 / (maxX-minX); // DWORD -> WORD 変換
			tdx = ((*(LPLONG)(dwScanData+16)-*(LPLONG)(dwScanData+8)))*i2;
			tdy = ((*(LPLONG)(dwScanData+20)-*(LPLONG)(dwScanData+12)))*i2;
		}
		else {
			tdx = 0;
			tdy = 0;
		}
		while(minX < 0){
			minX++;
			tx += tdx;
			ty += tdy;
		}
		maxX = min(nDestWidth-1,maxX);
		up = (unsigned char *)(dwDest + minX*3);
		for(x = minX; x < maxX; x++) {
			i2 = ((ty>>16)*nTexWByte)+((tx>>16)<<2);
			if( i2 < (int)dwTexSize) {
				usrcp = (unsigned char *)lpTex + i2;
				if ( *usrcp ) {
				a1=(short)up[0]-((((short)usrcp[1])*alpha)>>8);
				if (a1<0) a1=0;
				a2=(short)up[1]-((((short)usrcp[2])*alpha)>>8);
				if (a2<0) a2=0;
				a3=(short)up[2]-((((short)usrcp[3])*alpha)>>8);
				if (a3<0) a3=0;
				*up++=(unsigned char)a1;*up++=(unsigned char)a2;*up++=(unsigned char)a3;
				}
				else up+=3;
			}
			tx += tdx;
			ty += tdy;
		}
	}
	dwDest -= nDestWByte;
	dwScanData += sizeof(SCANDATA);
	}
	return;
}


/*------------------------------------------------------------*/
/*
		Line Draw Routines
*/
/*------------------------------------------------------------*/

static void DrawLine( int x1, int y1, int x2, int y2, int color, int alphasw )
{
    int i, x, y, dx, dy, addx, addy;
	short color1;
	unsigned char color2;
	unsigned char *up;
	char *dwDest;
	short alpha,alphaop,ialpha;
	short cc1,cc2,cc3,a1,a2,a3;
    int cnt = 0;

	dwDest = lpDest + ((nDestHeight-1)*nDestWByte);
	color1 = (short)(color & 0xffff);
	color2 = (unsigned char)( (color & 0xff0000 ) >>16 );

    dx = x2 - x1;
    if (dx < 0){ addx = -1; dx = -dx; } else addx = 1;
    dy = y2 - y1;
    if (dy < 0){ addy = -1; dy = -dy; } else addy = 1;
    x = x1; y = y1;

	//	透明設定を判定
	//
	if ( alphasw != 0x100 ) {
		alphaop = alphasw>>8;
		alpha = alphasw & 255;
		cc1 = ((color1 & 0xff) * alpha )>>8;
		cc2 = (((color1>>8)&0xff) * alpha )>>8;
		cc3 = ((short)(color2) * alpha )>>8;
		if ( alphaop == 0 ) goto l4_blend;
		if ( alphaop == 2 ) goto l4_modulate;
		if ( alphaop == 3 ) goto l4_substract;
	}

	//	通常ライン
	//
    if (dx > dy){
        for (i = 0; i < dx; ++i){
			if ((y>=0)&&(y<nDestHeight)&&(x>=0)&&(x<nDestWidth)) {
				up = (unsigned char *)(dwDest - nDestWByte*y + x*3 );
				*(short *)up = color1; up[2] = color2;
			}
            cnt += dy; x += addx;
            if (cnt >= dx){ cnt-=dx; y+=addy; }
        }
    }
    else{
        for (i = 0; i < dy; ++i){
			if ((y>=0)&&(y<nDestHeight)&&(x>=0)&&(x<nDestWidth)) {
				up = (unsigned char *)(dwDest - nDestWByte*y + x*3 );
				*(short *)up = color1; up[2] = color2;
			}
            cnt += dx; y += addy;
            if (cnt >= dy){ cnt-=dy; x+=addx; }
        }
    }
	return;

l4_blend:
	//	Blendライン
	//
	ialpha = 0xff - alpha;
    if (dx > dy){
        for (i = 0; i < dx; ++i){
			if ((y>=0)&&(y<nDestHeight)&&(x>=0)&&(x<nDestWidth)) {
				up = (unsigned char *)(dwDest - nDestWByte*y + x*3 );
				a1=((((short)up[0])*ialpha)>>8)+cc1;
				a2=((((short)up[1])*ialpha)>>8)+cc2;
				a3=((((short)up[2])*ialpha)>>8)+cc3;
				up[0]=(unsigned char)a1;
				up[1]=(unsigned char)a2;
				up[2]=(unsigned char)a3;
			}
            cnt += dy; x += addx;
            if (cnt >= dx){ cnt-=dx; y+=addy; }
        }
    }
    else{
        for (i = 0; i < dy; ++i){
			if ((y>=0)&&(y<nDestHeight)&&(x>=0)&&(x<nDestWidth)) {
				up = (unsigned char *)(dwDest - nDestWByte*y + x*3 );
				a1=((((short)up[0])*ialpha)>>8)+cc1;
				a2=((((short)up[1])*ialpha)>>8)+cc2;
				a3=((((short)up[2])*ialpha)>>8)+cc3;
				up[0]=(unsigned char)a1;
				up[1]=(unsigned char)a2;
				up[2]=(unsigned char)a3;
			}
            cnt += dx; y += addy;
            if (cnt >= dy){ cnt-=dy; x+=addx; }
        }
    }
	return;

l4_modulate:
	//	Modulateライン
	//
    if (dx > dy){
        for (i = 0; i < dx; ++i){
			if ((y>=0)&&(y<nDestHeight)&&(x>=0)&&(x<nDestWidth)) {
				up = (unsigned char *)(dwDest - nDestWByte*y + x*3 );
				a1=cc1+(short)up[0];if (a1>255) a1=255;
				a2=cc2+(short)up[1];if (a2>255) a2=255;
				a3=cc3+(short)up[2];if (a3>255) a3=255;
				up[0]=(unsigned char)a1;
				up[1]=(unsigned char)a2;
				up[2]=(unsigned char)a3;
			}
            cnt += dy; x += addx;
            if (cnt >= dx){ cnt-=dx; y+=addy; }
        }
    }
    else{
        for (i = 0; i < dy; ++i){
			if ((y>=0)&&(y<nDestHeight)&&(x>=0)&&(x<nDestWidth)) {
				up = (unsigned char *)(dwDest - nDestWByte*y + x*3 );
				a1=cc1+(short)up[0];if (a1>255) a1=255;
				a2=cc2+(short)up[1];if (a2>255) a2=255;
				a3=cc3+(short)up[2];if (a3>255) a3=255;
				up[0]=(unsigned char)a1;
				up[1]=(unsigned char)a2;
				up[2]=(unsigned char)a3;
			}
            cnt += dx; y += addy;
            if (cnt >= dy){ cnt-=dy; x+=addx; }
        }
    }
	return;

l4_substract:
	//	Substractライン
	//
    if (dx > dy){
        for (i = 0; i < dx; ++i){
			if ((y>=0)&&(y<nDestHeight)&&(x>=0)&&(x<nDestWidth)) {
				up = (unsigned char *)(dwDest - nDestWByte*y + x*3 );
				a1=(short)up[0]-cc1;if (a1<0) a1=0;
				a2=(short)up[1]-cc2;if (a2<0) a2=0;
				a3=(short)up[2]-cc3;if (a3<0) a3=0;
				up[0]=(unsigned char)a1;
				up[1]=(unsigned char)a2;
				up[2]=(unsigned char)a3;
			}
            cnt += dy; x += addx;
            if (cnt >= dx){ cnt-=dx; y+=addy; }
        }
    }
    else{
        for (i = 0; i < dy; ++i){
			if ((y>=0)&&(y<nDestHeight)&&(x>=0)&&(x<nDestWidth)) {
				up = (unsigned char *)(dwDest - nDestWByte*y + x*3 );
				a1=(short)up[0]-cc1;if (a1<0) a1=0;
				a2=(short)up[1]-cc2;if (a2<0) a2=0;
				a3=(short)up[2]-cc3;if (a3<0) a3=0;
				up[0]=(unsigned char)a1;
				up[1]=(unsigned char)a2;
				up[2]=(unsigned char)a3;
			}
            cnt += dx; y += addy;
            if (cnt >= dy){ cnt-=dy; x+=addx; }
        }
    }
	return;
}


static void DrawLineF4( POLY4 *lpPolyData )
{
	int x1,y1,x2,y2,x3,y3,x4,y4;
	x1=lpPolyData->v[0].x; y1=lpPolyData->v[0].y;
	x2=lpPolyData->v[1].x; y2=lpPolyData->v[1].y;
	DrawLine( x1,y1,x2,y2,lpPolyData->color, lpPolyData->alpha );
	x3=lpPolyData->v[2].x; y3=lpPolyData->v[2].y;
	DrawLine( x2,y2,x3,y3,lpPolyData->color, lpPolyData->alpha );
	x4=lpPolyData->v[3].x; y4=lpPolyData->v[3].y;
	if ((x3==x4)&&(y3==y4)) {
		DrawLine( x1,y1,x3,y3,lpPolyData->color, lpPolyData->alpha );
		return;
	}
	DrawLine( x3,y3,x4,y4,lpPolyData->color, lpPolyData->alpha );
	DrawLine( x1,y1,x4,y4,lpPolyData->color, lpPolyData->alpha );
	return;
}


static void DrawLineF4G( POLY4G *lpPolyData )
{
	POLY4 *poly;
	short alpha;
	poly = (POLY4 *)lpPolyData;
	alpha = lpPolyData->alpha;
	poly->alpha = alpha;
	DrawLineF4( poly );
}


/*------------------------------------------------------------*/
/*
		Sprite Draw Routines
*/
/*------------------------------------------------------------*/

static void DrawSpriteTex( P_SPRITE *spr )
{
	char *psrc;
	char *pdst;
	char *p;
	char *tp;
	int x,y;
//	int sx,sy;
	int tx,ty,tsx,tsy;
	int x0,y0,tx0,ty0;
	int clipx0,clipy0;
	int clipx1,clipy1;
	short alpha,alphaop,ialpha,a1,a2,a3;
	unsigned char *up;
	unsigned char *utp;

	clipx0 = sprclip1.x;
	clipy0 = sprclip1.y;
	clipx1 = sprclip2.x;
	clipy1 = sprclip2.y;

	x = spr->v[0].x;
	y = spr->v[0].y;
	tx = spr->v[0].tx;
	ty = spr->v[0].ty;
	if ( x >= clipx1 ) return;
	if ( y >= clipy1 ) return;
	if ( x < clipx0 ) {
		tx += clipx0-x; x = clipx0;
	}
	if ( y < clipy0 ) {
		ty += clipy0-y; y = clipy0;
	}

	x0 = spr->v[1].x;
	y0 = spr->v[1].y;
	tx0 = spr->v[1].tx;
	ty0 = spr->v[1].ty;
	if ( x0 < clipx0 ) return;
	if ( y0 < clipy0 ) return;

	if ( x0 >= clipx1 ) {
		tx0 -= x0-clipx1+1; x0 = clipx1-1;
	}
	if ( y0 >= clipy1 ) {
		ty0 -= y0-clipy1+1; y0 = clipy1-1;
	}
//	sx = x0-x+1;
//	sy = y0-y+1;
	tsx = tx0-tx+1;
	tsy = ty0-ty+1;

	//		Texture select
	if ( spr->tex != curtex ) ChangeTex( spr->tex );

	pdst = lpDest + ((nDestHeight-1-y)*nDestWByte) + (x*3);
	psrc = lpTex + (ty * nTexWByte) + (tx*4);

	if ( spr->alpha != 0x100 ) {
		alphaop = spr->alpha>>8;
		alpha = spr->alpha & 255;
		if ( alphaop == 0 ) goto sp_blend;
		if ( alphaop == 2 ) goto sp_modulate;
		if ( alphaop == 3 ) goto sp_substract;
	}
	if ( spr->attr & NODE_ATTR_COLKEY ) goto sptrans;

	for(y=0;y<tsy;y++) {
		p = pdst;
		tp = psrc;
		for(x=0;x<tsx;x++) {
			tp++;
			*(short *)p = *(short *)tp; p+=2; tp+=2;
			*p++ = *tp++;
		}
		psrc+=nTexWByte;
		pdst-=nDestWByte;
	}
	return;

sptrans:
	for(y=0;y<tsy;y++) {
		p = pdst;
		tp = psrc;
		for(x=0;x<tsx;x++) {
			if ( *tp++ ) {
				*(short *)p = *(short *)tp; p+=2; tp+=2;
				*p++ = *tp++;
			}
			else {
				p+=3;tp+=3;
			}
		}
		psrc+=nTexWByte;
		pdst-=nDestWByte;
	}
	return;

sp_blend:
	ialpha = 0xff - alpha;
	for(y=0;y<tsy;y++) {
		up = (unsigned char *)pdst;
		utp = (unsigned char *)psrc;
		for(x=0;x<tsx;x++) {
			if ( *utp++ ) {
				a1=((((short)*utp++)*alpha)+(((short)up[0])*ialpha))>>8;
				a2=((((short)*utp++)*alpha)+(((short)up[1])*ialpha))>>8;
				a3=((((short)*utp++)*alpha)+(((short)up[2])*ialpha))>>8;
				*up++=(unsigned char)a1;*up++=(unsigned char)a2;*up++=(unsigned char)a3;
			}
			else {
				up+=3;utp+=3;
			}
		}
		psrc+=nTexWByte;
		pdst-=nDestWByte;
	}
	return;

sp_modulate:
	for(y=0;y<tsy;y++) {
		up = (unsigned char *)pdst;
		utp = (unsigned char *)psrc;
		for(x=0;x<tsx;x++) {
			if ( *utp++ ) {
				a1=((((short)*utp++)*alpha)>>8)+(short)up[0];
				if (a1>255) a1=255;
				a2=((((short)*utp++)*alpha)>>8)+(short)up[1];
				if (a2>255) a2=255;
				a3=((((short)*utp++)*alpha)>>8)+(short)up[2];
				if (a3>255) a3=255;
				*up++=(unsigned char)a1;*up++=(unsigned char)a2;*up++=(unsigned char)a3;
			}
			else {
				up+=3;utp+=3;
			}
		}
		psrc+=nTexWByte;
		pdst-=nDestWByte;
	}
	return;

sp_substract:
	for(y=0;y<tsy;y++) {
		up = (unsigned char *)pdst;
		utp = (unsigned char *)psrc;
		for(x=0;x<tsx;x++) {
			if ( *utp++ ) {
				a1=(short)up[0]-((((short)*utp++)*alpha)>>8);
				if (a1<0) a1=0;
				a2=(short)up[1]-((((short)*utp++)*alpha)>>8);
				if (a2<0) a2=0;
				a3=(short)up[2]-((((short)*utp++)*alpha)>>8);
				if (a3<0) a3=0;
				*up++=(unsigned char)a1;*up++=(unsigned char)a2;*up++=(unsigned char)a3;
			}
			else {
				up+=3;utp+=3;
			}
		}
		psrc+=nTexWByte;
		pdst-=nDestWByte;
	}
	return;
}


static void DrawBGSpriteTex( P_BGSPR *bg )
{
	P_SPRITE spr;
	short *vram;
	short val;
	char *p;
	int x,y,wx,wy,vx,vy,cx,cy,sx,sy,adjx,adjy;
	int i,j,a;

	vx = bg->vx / bg->cx;
	vy = bg->vy / bg->cy;
	adjx = bg->vx % bg->cx;
	adjy = bg->vy % bg->cy;
	x = bg->xx;
	y = bg->yy;
	sx = bg->sx << 1;
	sy = bg->sy;
	cx = bg->cx;
	cy = bg->cy;
	wx = bg->wx;if ( adjx ) { wx+=1; x-=adjx; }
	wy = bg->wy;if ( adjy ) { wy+=1; y-=adjy; }
	p = bg->vram + ( vy * sx ) + (vx<<1);

	spr.v[0].y = y;
	spr.v[1].y = y + cy - 1;

	//		Set Clip
	//
	setIAXIS( sprclip1, bg->xx, bg->yy );
	setIAXIS( sprclip2, bg->xx + ( bg->wx * cx ), bg->yy + ( bg->wy * cy ) );

	spr.tex = bg->tex;
	spr.attr = bg->attr;
	spr.alpha = bg->alpha;
	for(j=0;j<wy;j++) {
		spr.v[0].x = x;
		spr.v[1].x = x + cx - 1;
		vram = (short *)p;
		for(i=0;i<wx;i++) {
			val = *vram++;
			a = (val & 15) * cx;
			spr.v[0].tx = a;
			spr.v[1].tx = a + cx -1;
			a = (val >> 4) * cy;
			spr.v[0].ty = a;
			spr.v[1].ty = a + cy -1;

			DrawSpriteTex( &spr );
		
			spr.v[0].x += cx;
			spr.v[1].x += cx;
		}
		p += sx;
		spr.v[0].y += cy;
		spr.v[1].y += cy;
	}

	//		Reset Clip
	//
	setIAXIS( sprclip1, 0, 0 );
	setIAXIS( sprclip2, nDestWidth, nDestHeight );
}


static void DrawFontSprite( int x, int y, int cx, int cy, int px, char *mes, int attr, int tex )
{
	unsigned char *p;
	int a,val;
	P_SPRITE spr;
	spr.attr = (short)attr;
	if ( attr & NODE_ATTR_USEALPHA ) {
		spr.alpha = attr & 0x3ff;
	} else {
		spr.alpha = 0x100;
	}
	spr.tex = tex;
	spr.v[0].y = y;
	spr.v[1].y = y + cy - 1;
	spr.v[0].x = x;
	spr.v[1].x = x + cx - 1;
	p = (unsigned char *)mes;
	while(1) {
		val = (int)*p++;
		if ( val == 0 ) break;
		if ( val != 32 ) {			// spaceは無視
			a = (val & 15) * cx;
			spr.v[0].tx = a;
			spr.v[1].tx = a + cx -1;
			a = (val >> 4) * cy;
			spr.v[0].ty = a;
			spr.v[1].ty = a + cy -1;
			DrawSpriteTex( &spr );
		}
		spr.v[0].x += px;
		spr.v[1].x += px;
	}
}


/*------------------------------------------------------------*/
/*
		texture process
*/
/*------------------------------------------------------------*/

static void TexInit( void )
{
	int i;
	for(i=0;i<TEXINF_MAX;i++) {
		texinf[i].flag = 0;
	}
}


static void TexTerm( void )
{
	int i;
	for(i=0;i<TEXINF_MAX;i++) {
		DeleteTex( i );
	}
}


static int GetNextTexID( void )
{
	int i,sel;
	sel = -1;
	for(i=0;i<TEXINF_MAX;i++) {
		if ( texinf[i].flag == 0 ) { sel=i;break; }
	}
	return sel;
}


static void TexCopySub( char *dst, char *src, int size )
{
	//		(R,G,B) を (M,R,G,B) 形式にしてコピー
	//
	char *p;
	char *sp;
	char a1,a2,a3,a4;
	int i;
	i = size;
	p = dst; sp = src;
	while( i>0 ) {
		a1=*sp++;a2=*sp++;a3=*sp++;
		a4=(char)0xff;
		if (( a1==ck1 )&&( a2==ck2 )&&( a3==ck3 )) a4=0;
		*p++ = a4;
		*p++ = a1;
		*p++ = a2;
		*p++ = a3;
		i--;
	}
}
	

static int RegistTex( char *data, int sx, int sy, int width, int height, int sw )
{
	int sel;
	int i,y;
	char *p;
	char *src;
	TEXINF *t;
	sel = GetNextTexID();
	p = (char *)malloc( sx*sy*4 );
	t = GetTex( sel );
	t->flag = 1;
	t->mode = 0;
	t->sx = width;
	t->sy = height;
	t->data = p;

	//		カラーキー更新
	//
	i = GetSysReq( SYSREQ_COLORKEY );
	ck1 = i & 0xff;
	ck2 = (i>>8) & 0xff;
	ck3 = (i>>16) & 0xff;

	//		上下逆にしてコピーする
	//
	if ( sw == 0 ) {
		for(y=0;y<sy;y++) {
			src = data + (sx*3*(sy-1-y));
			TexCopySub( p, src, sx );
			p+=sx*4;
		}
	}
	else {
		memcpy( p, data, sx*sy*3 );
		//TexCopySub( p, data, sx*sy );
	}

	return sel;
}


static void DeleteTex( int id )
{
	TEXINF *t;
	t = GetTex( id );
	if ( t->flag == 0 ) return;
	free( t->data );
	t->flag = 0;
}


static void ChangeTex( int id )
{
	TEXINF *t;
	if ( id < 0 ) { curtex=-1; return; }
	//if ( curtex == id ) return;
	t = GetTex( id );
	if ( t->flag == 0 ) return;
	curtex = id;
	lpTex = t->data;			// テクスチャのバッファ
	nTexWidth = t->sx;			// テクスチャ幅
	nTexWByte = nTexWidth * 4;
	nTexHeight = t->sy;			// テクスチャ高さ
}


static TEXINF *GetTex( int id )
{
	return &texinf[id];
}


static void SetSrcTex( void *src, int sx, int sy )
{
	lpTex = (char *)src;
	nTexWidth = sx;
	nTexWByte = nTexWidth * 4;
	nTexHeight = sy;
}


