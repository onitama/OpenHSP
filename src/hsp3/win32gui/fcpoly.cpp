//
//		Polygon draw lib (full color)
//			onion software/onitama 2001/6
//

// hsp3cでは全体を無視
#ifndef HSP_COMPACT

#include <stdio.h>
#include <windows.h>
#include <math.h>
#include "fcpoly.h"
#include "../supio.h"

//		Settings
//
int resY0, resY1;	// 書き換え範囲(高さ)min,max

static		SCANDATA *scanData;
static		char *lpDest;		// 描画画面のバッファ
static		int nDestWByte;		// 描画座標幅byte数
static		int nDestWidth;		// 描画座標幅
static		int nDestWidth2;	// 描画座標幅(VRAM用)
static		int nDestHeight;	// 描画座標高さ
static		int DestSize;		// 描画画面バッファのサイズ
static		int maxysize;		// SCANDATAの最大高さ

static		char *lpTex;		// テクスチャのバッファ
static		int curtex;			// current texID
static		int nTexWidth;		// テクスチャ幅
static		int nTexWByte;		// テクスチャ幅byte数
static		int nTexHeight;		// テクスチャ高さ

static		unsigned char ck1;	// カラーキー1
static		unsigned char ck2;	// カラーキー2
static		unsigned char ck3;	// カラーキー3

/*------------------------------------------------------------*/
/*
		interface
*/
/*------------------------------------------------------------*/

void SetupScanData( int ysize )
{
	if ( maxysize < ysize ) {
		if ( scanData != NULL ) { free( scanData ); }
		scanData = (SCANDATA *)malloc( sizeof( SCANDATA ) * ysize );
		maxysize = ysize;
	}
}

void hgiof_init( void )
{
	//		Setup system valiables
	//
	HDC hdc;
	int dispy;
	hdc=GetDC(NULL);
	dispy = GetSystemMetrics( SM_CYSCREEN );
	ReleaseDC(NULL,hdc);

	//		バッファ初期化
	//
	lpDest = NULL;
	lpTex = NULL;
	maxysize = 0;
	scanData = NULL;
	SetupScanData( dispy );

	//		カラーキー初期化
	//
	ck1 = 0; ck2 = 0; ck3 = 0;

	//		テクスチャ情報初期化
	//
	TexInit();
}

void hgiof_term( void )
{
	if ( scanData != NULL ) { free( scanData ); scanData = NULL; }
	lpDest = NULL;
	TexTerm();
}


void SetPolyDest( void *dest, int sx, int sy )
{
	lpDest = (char *)dest;
	nDestWidth = sx;
	nDestWByte = ((sx*3)+3)&~3;
	nDestWidth2 = ((sx)+3)&~3;
	nDestHeight = sy;
	DestSize = nDestWByte * sy * 3;
}


void SetPolyColorKey( int color )
{
	ck1 = (unsigned char)(color>>16);
	ck2 = (unsigned char)(color>>8);
	ck3 = (unsigned char)(color);
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
	SCANDATA *lpScanData;

	double x,dx;  // 描画座標計算用
	double dd;
	double tx,ty,tdx,tdy; // テクスチャ座標計算用
				
	// 描画開始位置、終了位置初期化
	*lpnStartY = nScrHeight;
	*lpnEndY = -1;

	// スキャンデータ初期化
	SetupScanData( nScrHeight );
	lpScanData = scanData;

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
					//if((y >=0 )&&( y <  nScrHeight)){
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
	*lpnEndY = min(nScrHeight,*lpnEndY);
}


void DrawPolygonF4( POLY4 *lpPolyData )
{
	int x,y; // ループ用
	int nStartY,nEndY; // 描画開始を開始する Y 座標、終了座標
	int maxX,minX;  // エッジ座標の最大最小値
//	char *p;
	short color1;
	unsigned char color2;

	DWORD dwScanData,dwDest; // エッジの座標データと転送先のベースアドレス
//	int x0,x1,x2,y0,y1,y2;

	short alpha,alphaop,ialpha;
	short cc1,cc2,cc3,a1,a2,a3;
	unsigned char *up;

	// 裏表判定
/*
	x0 = lpPolyData->v[0].x;
	x1 = lpPolyData->v[1].x;
	x2 = lpPolyData->v[2].x;
	y0 = lpPolyData->v[0].y;
	y1 = lpPolyData->v[1].y;
	y2 = lpPolyData->v[2].y;
	if ( (x0 - x1) * (y1 - y2) - (x1 - x2) * (y0 - y1) >= 0 ) return;
*/
	// エッジの座標のスキャン
	ScanLine(lpPolyData, nDestWidth,nDestHeight,&nStartY,&nEndY);

	// 範囲外なら描画しない
	if(nStartY >= nDestHeight || nEndY < 0) { resY0 = resY1 = -1; return; }

	resY0 = nStartY;
	resY1 = nEndY;

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
		alpha = ( lpPolyData->alpha & 255 ) + 1;
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
			maxX = min(nDestWidth,maxX);
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
	ialpha = 0x100 - alpha;
	for(y = nStartY; y < nEndY; y++){
		minX = *(int *)(dwScanData);
		maxX = *(int *)(dwScanData+4);
		if(minX < 0) minX = 0;
		if(maxX >= 0){
			maxX = min(nDestWidth,maxX);
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
			maxX = min(nDestWidth,maxX);
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
			maxX = min(nDestWidth,maxX);
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


void DrawPolygonTex( POLY4 *lpPolyData )
{
	int i2,x,y; // ループ用
	int nStartY,nEndY; // 描画開始を開始する Y 座標、終了座標
	int maxX,minX;  // エッジ座標の最大最小値
//	int x0,x1,x2,y0,y1,y2;
	char *p;
	char *srcp;
	char d1,d2,d3;
	DWORD tx,ty,tdx,tdy; 

	short alpha,alphaop,ialpha,a1,a2,a3;
	unsigned char *up;
	unsigned char *usrcp;

	DWORD dwTexSize; // テクスチャのサイズ
	DWORD dwScanData,dwDest; // エッジの座標データと転送先のベースアドレス

	//		Texture select
	//if ( lpPolyData->tex != curtex ) ChangeTex( lpPolyData->tex );

	// 裏表判定
/*
	x0 = lpPolyData->v[0].x;
	x1 = lpPolyData->v[1].x;
	x2 = lpPolyData->v[3].x;
	y0 = lpPolyData->v[0].y;
	y1 = lpPolyData->v[1].y;
	y2 = lpPolyData->v[3].y;
	if ( (x0 - x1) * (y1 - y2) - (x1 - x2) * (y0 - y1) >= 0 ) return;
*/
	// エッジの座標のスキャン
	dwTexSize = (DWORD)(nTexHeight*nTexWByte);
	ScanLine( lpPolyData, nDestWidth, nDestHeight, &nStartY, &nEndY );
	
	// 範囲外なら描画しない
	if(nStartY >= nDestHeight || nEndY < 0) { resY0 = resY1 = -1; return; }

	resY0 = nStartY;
	resY1 = nEndY;

	// 転送先バッファとスキャンデータのベースアドレスセット
	// 転送先バッファ(DIB)は上下が反転しているのに注意((nDestHeight-1-nStartY)の所)
	p = (char *)lpDest + ((nDestHeight-1-nStartY)*nDestWByte);
	dwDest = (DWORD)p;
	dwScanData = (DWORD)scanData + (DWORD)(nStartY*sizeof(SCANDATA));

	//	透明設定を判定
	//
	if ( lpPolyData->alpha != 0x100 ) {
		alphaop = lpPolyData->alpha>>8;
		alpha = ( lpPolyData->alpha & 255 ) + 1;
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
	maxX = min(nDestWidth,maxX);
	p = (char *)(dwDest) + minX*3;
	for(x = minX; x < maxX; x++){
	
	// テクスチャデータを読み込む点を計算
	//dwReadPoint = (DWORD)(nTexHeight-1-(ty>>5))*nTexWidth+(tx>>5);
	//dwReadPoint = (DWORD)(nTexHeight-1-(ty>>16))*nTexWidth+(tx>>16);
	//dwReadPoint = (DWORD)((ty>>16))*nTexWByte+((tx>>16)*3);
//	i2 = ((tx>>16)*3);
	i2 = ((ty>>16)*nTexWByte)+((tx>>16)*3);
	
	// コピー
	if( i2 < (int)dwTexSize) {
		srcp = (char *)lpTex + i2;
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
		maxX = min(nDestWidth,maxX);
		p = (char *)(dwDest + minX*3);
		for(x = minX; x < maxX; x++) {

			// テクスチャデータを読み込む点を計算
			i2 = ((ty>>16)*nTexWByte)+((tx>>16)*3);
	
			// コピー
			if( i2 < (int)dwTexSize) {
				srcp = (char *)lpTex + i2;
				d1=*srcp++;
				d2=*srcp++;
				d3=*srcp++;
				if ( d1|d2|d3 ) {
					p[0] = d1;
					p[1] = d2;
					p[2] = d3;
				}
				p+=3;
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
	//	描画(BLEND)
	//
	//
	if ( lpPolyData->attr & NODE_ATTR_COLKEY ) goto p4_tblend;

	ialpha = 0x100 - alpha;
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
		maxX = min(nDestWidth,maxX);
		up = (unsigned char *)(dwDest + minX*3);
		for(x = minX; x < maxX; x++) {
			i2 = ((ty>>16)*nTexWByte)+((tx>>16)*3);
			if( i2 < (int)dwTexSize) {
				usrcp = (unsigned char *)lpTex + i2;
				a1=((((short)usrcp[0])*alpha)+(((short)up[0])*ialpha))>>8;
				a2=((((short)usrcp[1])*alpha)+(((short)up[1])*ialpha))>>8;
				a3=((((short)usrcp[2])*alpha)+(((short)up[2])*ialpha))>>8;
				*up++=(unsigned char)a1;*up++=(unsigned char)a2;*up++=(unsigned char)a3;
			}
			tx += tdx;
			ty += tdy;
		}
	}
	dwDest -= nDestWByte;
	dwScanData += sizeof(SCANDATA);
	}
	return;

p4_tblend:
	//
	//	透明色あり描画(BLEND)
	//
	//
	ialpha = 0x100 - alpha;
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
		maxX = min(nDestWidth,maxX);
		up = (unsigned char *)(dwDest + minX*3);
		for(x = minX; x < maxX; x++) {
			i2 = ((ty>>16)*nTexWByte)+((tx>>16)*3);
			if( i2 < (int)dwTexSize) {
				usrcp = (unsigned char *)lpTex + i2;
				if (( usrcp[0]==ck1 )&&( usrcp[1]==ck2 )&&( usrcp[2]==ck3 )) {
					up+=3;
				} else {
					a1=((((short)usrcp[0])*alpha)+(((short)up[0])*ialpha))>>8;
					a2=((((short)usrcp[1])*alpha)+(((short)up[1])*ialpha))>>8;
					a3=((((short)usrcp[2])*alpha)+(((short)up[2])*ialpha))>>8;
					*up++=(unsigned char)a1;*up++=(unsigned char)a2;*up++=(unsigned char)a3;
				}
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
	//	描画(MODULATE)
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
		maxX = min(nDestWidth,maxX);
		up = (unsigned char *)(dwDest + minX*3);
		for(x = minX; x < maxX; x++) {
			i2 = ((ty>>16)*nTexWByte)+((tx>>16)*3);
			if( i2 < (int)dwTexSize) {
				usrcp = (unsigned char *)lpTex + i2;
				a1=((((short)usrcp[0])*alpha)>>8)+(short)up[0];
				if (a1>255) a1=255;
				a2=((((short)usrcp[1])*alpha)>>8)+(short)up[1];
				if (a2>255) a2=255;
				a3=((((short)usrcp[2])*alpha)>>8)+(short)up[2];
				if (a3>255) a3=255;
				*up++=(unsigned char)a1;*up++=(unsigned char)a2;*up++=(unsigned char)a3;
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
	//	描画(MODULATE)
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
		maxX = min(nDestWidth,maxX);
		up = (unsigned char *)(dwDest + minX*3);
		for(x = minX; x < maxX; x++) {
			i2 = ((ty>>16)*nTexWByte)+((tx>>16)*3);
			if( i2 < (int)dwTexSize) {
				usrcp = (unsigned char *)lpTex + i2;
				a1=(short)up[0]-((((short)usrcp[0])*alpha)>>8);
				if (a1<0) a1=0;
				a2=(short)up[1]-((((short)usrcp[1])*alpha)>>8);
				if (a2<0) a2=0;
				a3=(short)up[2]-((((short)usrcp[2])*alpha)>>8);
				if (a3<0) a3=0;
				*up++=(unsigned char)a1;*up++=(unsigned char)a2;*up++=(unsigned char)a3;
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
		palette mode process
*/
/*------------------------------------------------------------*/

void DrawPolygonF4P( POLY4 *lpPolyData )
{
	int x,y; // ループ用
	int nStartY,nEndY; // 描画開始を開始する Y 座標、終了座標
	int maxX,minX;  // エッジ座標の最大最小値
//	char *p;
	unsigned char color1;

	DWORD dwScanData,dwDest; // エッジの座標データと転送先のベースアドレス
//	int x0,x1,x2,y0,y1,y2;

//	short alpha,alphaop,ialpha;
//	short cc1,cc2,cc3,a1,a2,a3;
	unsigned char *up;

	// エッジの座標のスキャン
	ScanLine(lpPolyData, nDestWidth,nDestHeight,&nStartY,&nEndY);
	
	// 範囲外なら描画しない
	if(nStartY >= nDestHeight || nEndY < 0) { resY0 = resY1 = -1; return; }

	resY0 = nStartY;
	resY1 = nEndY;

	// Fill Color
	color1 = (unsigned char)( lpPolyData->color & 0xff );

	// 転送先バッファとスキャンデータのベースアドレスセット
	// 転送先バッファ(DIB)は上下が反転しているのに注意((nDestHeight-1-nStartY)の所)

	dwDest = (DWORD)lpDest + (DWORD)((nDestHeight-1-nStartY)*nDestWidth2);
	dwScanData = (DWORD)scanData + (DWORD)(nStartY*sizeof(SCANDATA));

	// nStartY から nEndY まで上から順に描画
	for(y = nStartY; y < nEndY; y++){
	
		// エッジの(転送先画像(dwDest)上での)座標セット
		minX = *(int *)(dwScanData);
		maxX = *(int *)(dwScanData+4);

		if(minX < 0) minX = 0;
		if(maxX >= 0){

			// 左から右に横に描画していく
			maxX = min(nDestWidth,maxX);
			up = (unsigned char *)dwDest + minX;
			for(x = minX; x < maxX; x++){
	
			// コピー
			*up++ = color1;
	
			}
		}
		// バッファとスキャンデータのベースアドレス更新
		dwDest -= nDestWidth2;
		dwScanData += sizeof(SCANDATA);
	}
}


void DrawPolygonTexP( POLY4 *lpPolyData )
{
	int i2,x,y; // ループ用
	int nStartY,nEndY; // 描画開始を開始する Y 座標、終了座標
	int maxX,minX;  // エッジ座標の最大最小値
	char *p;
	char d1;
	DWORD tx,ty,tdx,tdy; 

	DWORD dwTexSize; // テクスチャのサイズ
	DWORD dwScanData,dwDest; // エッジの座標データと転送先のベースアドレス

	// エッジの座標のスキャン
	dwTexSize = (DWORD)(nTexHeight*nTexWidth);
	ScanLine( lpPolyData, nDestWidth, nDestHeight, &nStartY, &nEndY );
	
	// 範囲外なら描画しない
	if(nStartY >= nDestHeight || nEndY < 0) { resY0 = resY1 = -1; return; }

	resY0 = nStartY;
	resY1 = nEndY;

	// 転送先バッファとスキャンデータのベースアドレスセット
	// 転送先バッファ(DIB)は上下が反転しているのに注意((nDestHeight-1-nStartY)の所)
	p = (char *)lpDest + ((nDestHeight-1-nStartY)*nDestWidth2);
	dwDest = (DWORD)p;
	dwScanData = (DWORD)scanData + (DWORD)(nStartY*sizeof(SCANDATA));

	//	透明設定を判定
	//
	if ( lpPolyData->attr & NODE_ATTR_COLKEY ) goto p4transp;

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
	maxX = min(nDestWidth,maxX);
	p = (char *)(dwDest) + minX;
	for(x = minX; x < maxX; x++){
	
	// テクスチャデータを読み込む点を計算
	i2 = ((ty>>16)*nTexWidth)+(tx>>16);
	
	// コピー
	if( i2 < (int)dwTexSize) {
		*p++ = lpTex[i2];
	}
	
	// テクスチャ座標移動
	tx += tdx;
	ty += tdy;
	}
	}
	
	// バッファとスキャンデータのベースアドレス更新
	dwDest -= nDestWidth2;
	dwScanData += sizeof(SCANDATA);
	}

	return;

p4transp:
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
		maxX = min(nDestWidth,maxX);
		p = (char *)(dwDest + minX);
		for(x = minX; x < maxX; x++) {

			// テクスチャデータを読み込む点を計算
			i2 = ((ty>>16)*nTexWidth)+(tx>>16);
	
			// コピー
			if( i2 < (int)dwTexSize) {
				d1 = lpTex[i2];
				if ( d1 ) *p=d1;
				p++;
			}
		
			// テクスチャ座標移動
			tx += tdx;
			ty += tdy;
		}
	}
	
	// バッファとスキャンデータのベースアドレス更新
	dwDest -= nDestWidth2;
	dwScanData += sizeof(SCANDATA);
	}
	return;
}


/*------------------------------------------------------------*/
/*
		texture process
*/
/*------------------------------------------------------------*/

/*
	rev 43
	mingw : error : 宣言と修飾が異なる
	に対処
*/

void TexInit( void )
{
	lpTex = NULL;
}


void TexTerm( void )
{
}


void SetPolySource( void *src, int sx, int sy )
{
	lpTex = (char *)src;
	nTexWidth = sx;
	nTexWByte = ((nTexWidth*3)+3)&~3;
	nTexHeight = sy;
}



#endif
