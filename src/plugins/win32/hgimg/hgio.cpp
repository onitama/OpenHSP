//
//		Polygon draw lib (palette)
//			onion software/onitama 2000/11
//
#include <stdio.h>
#include <windows.h>
#include <math.h>
#include "hginc.h"
#include "hgio.h"


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
static		int nDestWidth;		// 描画座標幅
static		int nDestHeight;	// 描画座標高さ
static		int DestSize;		// 描画画面バッファのサイズ

static		IAXIS sprclip1;		// 2D表示用クリップ領域(左上)
static		IAXIS sprclip2;		// 2D表示用クリップ領域(右下)

static		char *lpTex;		// テクスチャのバッファ
static		int curtex;			// current texID
static		int nTexWidth;		// テクスチャ幅
static		int nTexHeight;		// テクスチャ高さ

/*------------------------------------------------------------*/
/*
		interface
*/
/*------------------------------------------------------------*/

void hgio_init( HGProc *p )
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

	//		テクスチャ情報初期化
	//
	TexInit();
}

void hgio_term( void )
{
	releaseDest();
	TexTerm();
}


static void setDest( void *dest, int sx, int sy )
{
	releaseDest();
	lpDest = (char *)dest;
	nDestWidth = sx;
	nDestHeight = sy;
	DestSize = sx*sy;
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


static void ClearDest( int mode, int color, int tex )
{
	if ( mode == 1 ) {
		memset( lpDest, color, DestSize );
	}
	if ( mode == 2 ) {
		TEXINF *t;
		t = GetTex( tex );
		memcpy( lpDest, t->data, DestSize );
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

	DWORD dwScanData,dwDest; // エッジの座標データと転送先のベースアドレス
	int x0,x1,x2,y0,y1,y2;
	
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
	
	// 転送先バッファとスキャンデータのベースアドレスセット
	// 転送先バッファ(DIB)は上下が反転しているのに注意((nDestHeight-1-nStartY)の所)

	dwDest = (DWORD)lpDest + (DWORD)((nDestHeight-1-nStartY)*nDestWidth);
	dwScanData = (DWORD)scanData + (DWORD)(nStartY*sizeof(SCANDATA));
	
	// nStartY から nEndY まで上から順に描画
	for(y = nStartY; y <= nEndY; y++){
	
		// エッジの(転送先画像(dwDest)上での)座標セット
		minX = *(int *)(dwScanData);
		maxX = *(int *)(dwScanData+4);

		if(minX < 0) minX = 0;
		if(maxX >= 0){

			// 左から右に横に描画していく
			maxX = min(nDestWidth-1,maxX);
			for(x = minX; x <= maxX; x++){
	
			// コピー
			*(LPBYTE)(dwDest+x) = 0x80;
	
			}
		}
	
		// バッファとスキャンデータのベースアドレス更新
		dwDest -= nDestWidth;
		dwScanData += sizeof(SCANDATA);
	}
}


static void DrawPolygonTex( POLY4 *lpPolyData )
{
	int i2,x,y; // ループ用
	int nStartY,nEndY; // 描画開始を開始する Y 座標、終了座標
	int maxX,minX;  // エッジ座標の最大最小値
	int x0,x1,x2,y0,y1,y2;
	char a1;
	DWORD tx,ty,tdx,tdy; 
	DWORD dwTexSize; // テクスチャのサイズ
	DWORD dwReadPoint; // テクスチャデータを読み込むアドレス
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
	dwTexSize = (DWORD)(nTexHeight*nTexWidth);
	ScanLine( lpPolyData, scanData, nDestWidth, nDestHeight, &nStartY, &nEndY );
	
	// 範囲外なら描画しない
	if(nStartY >= nDestHeight || nEndY < 0) return;
	
	// 転送先バッファとスキャンデータのベースアドレスセット
	// 転送先バッファ(DIB)は上下が反転しているのに注意((nDestHeight-1-nStartY)の所)
	dwDest = (DWORD)lpDest + (DWORD)((nDestHeight-1-nStartY)*nDestWidth);
	dwScanData = (DWORD)scanData + (DWORD)(nStartY*sizeof(SCANDATA));

	if ( lpPolyData->attr & NODE_ATTR_COLKEY ) goto p4trans;

	//
	//	透明色なし描画
	//
	//
	// nStartY から nEndY まで上から順に描画
	for(y = nStartY; y <= nEndY; y++){
	
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
	for(x = minX; x <= maxX; x++){
	
	// テクスチャデータを読み込む点を計算
	//dwReadPoint = (DWORD)(nTexHeight-1-(ty>>5))*nTexWidth+(tx>>5);
	//dwReadPoint = (DWORD)(nTexHeight-1-(ty>>16))*nTexWidth+(tx>>16);
	dwReadPoint = (DWORD)((ty>>16))*nTexWidth+(tx>>16);
	
	// コピー
	if(dwReadPoint < dwTexSize) *(LPBYTE)(dwDest+x) = lpTex[dwReadPoint];
	
	// テクスチャ座標移動
	tx += tdx;
	ty += tdy;
	}
	}
	
	// バッファとスキャンデータのベースアドレス更新
	dwDest -= nDestWidth;
	dwScanData += sizeof(SCANDATA);
	}

	return;

p4trans:
	//
	//	透明色あり描画
	//
	//
	// nStartY から nEndY まで上から順に描画
	for(y = nStartY; y <= nEndY; y++){
	
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
		for(x = minX; x <= maxX; x++)
			{
	
			// テクスチャデータを読み込む点を計算
			//dwReadPoint = (DWORD)(nTexHeight-1-(ty>>16))*nTexWidth+(tx>>16);
			dwReadPoint = (DWORD)((ty>>16))*nTexWidth+(tx>>16);
	
			// コピー
			if(dwReadPoint < dwTexSize) {
				a1 = lpTex[dwReadPoint];
				if ( a1 != 0 ) *(LPBYTE)(dwDest+x) = a1;
			}
	
			// テクスチャ座標移動
			tx += tdx;
			ty += tdy;
		}
	}
	
	// バッファとスキャンデータのベースアドレス更新
	dwDest -= nDestWidth;
	dwScanData += sizeof(SCANDATA);
	}
}


/*------------------------------------------------------------*/
/*
		Line Draw Routines
*/
/*------------------------------------------------------------*/

static void DrawLine( int x1, int y1, int x2, int y2, int color )
{
    int i, x, y, dx, dy, addx, addy;
	unsigned char uc;
	char *dwDest;
    int cnt = 0;

	dwDest = lpDest + ((nDestHeight-1)*nDestWidth);
	uc = (unsigned char)( color & 0xff );

    dx = x2 - x1;
    if (dx < 0){ addx = -1; dx = -dx; } else addx = 1;
    dy = y2 - y1;
    if (dy < 0){ addy = -1; dy = -dy; } else addy = 1;
    x = x1; y = y1;

    if (dx > dy){
        for (i = 0; i < dx; ++i){
			if ((y>=0)&&(y<nDestHeight)&&(x>=0)&&(x<nDestWidth)) {
				*(unsigned char *)(dwDest - nDestWidth*y + x ) = uc;
			}
            cnt += dy; x += addx;
            if (cnt >= dx){ cnt-=dx; y+=addy; }
        }
    }
    else{
        for (i = 0; i < dy; ++i){
			if ((y>=0)&&(y<nDestHeight)&&(x>=0)&&(x<nDestWidth)) {
				*(unsigned char *)(dwDest - nDestWidth*y + x ) = uc;
			}
            cnt += dx; y += addy;
            if (cnt >= dy){ cnt-=dy; x+=addx; }
        }
    }
}


static void DrawLineF4( POLY4 *lpPolyData )
{
	int x1,y1,x2,y2,x3,y3,x4,y4;
	x1=lpPolyData->v[0].x; y1=lpPolyData->v[0].y;
	x2=lpPolyData->v[1].x; y2=lpPolyData->v[1].y;
	DrawLine( x1,y1,x2,y2,lpPolyData->color );
	x3=lpPolyData->v[2].x; y3=lpPolyData->v[2].y;
	DrawLine( x2,y2,x3,y3,lpPolyData->color );
	x4=lpPolyData->v[3].x; y4=lpPolyData->v[3].y;
	if ((x3==x4)&&(y3==y4)) {
		DrawLine( x1,y1,x3,y3,lpPolyData->color );
		return;
	}
	DrawLine( x3,y3,x4,y4,lpPolyData->color );
	DrawLine( x1,y1,x4,y4,lpPolyData->color );
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
		tx0 -= x0-clipx1+2;// x0 = clipx1-1;
		tsx = tx0-tx+1;
	}
	if ( y0 >= clipy1 ) {
		ty0 -= y0-clipy1+2;// y0 = clipy1-1;
	}
//	sx = x0-x+1;
//	sy = y0-y+1;
	tsx = tx0-tx+1;
	tsy = ty0-ty+1;
	if ((tsx<1)||(tsy<1)) return;

	//		Texture select
	if ( spr->tex != curtex ) ChangeTex( spr->tex );

	pdst = lpDest + ((nDestHeight-1-y)*nDestWidth) + x;
	psrc = lpTex + (ty * nTexWidth) + tx;

	if ( spr->attr & NODE_ATTR_COLKEY ) goto sptrans;
	
	for(y=0;y<tsy;y++) {
		p = pdst;
		tp = psrc;
		for(x=0;x<tsx;x++) {
			*p++ = *tp++;
		}
		psrc+=nTexWidth;
		pdst-=nDestWidth;
	}
	return;

sptrans:
//	if (( tsx&3 )==0) goto sptransf;
	for(y=0;y<tsy;y++) {
		p = pdst;
		tp = psrc;
		for (x=0;x<tsx;x++) {
			if ( *tp ) *p = *tp;
			p++;tp++;
		}
		psrc+=nTexWidth;
		pdst-=nDestWidth;
	}
	return;
/*
sptransf:
	for(y=0;y<tsy;y++) {
		p = pdst;
		tp = psrc;
		x = 0;
		while(1) {
			if ( x>=tsx ) break;
			if ( (*(int *)tp) != 0 ) {
				if ( *tp ) *p = *tp;
				p++;tp++;
				if ( *tp ) *p = *tp;
				p++;tp++;
				if ( *tp ) *p = *tp;
				p++;tp++;
				if ( *tp ) *p = *tp;
				p++;tp++;
			}
			else {
				p+=4;tp+=4;
			}
			x+=4;
		}
		psrc+=nTexWidth;
		pdst-=nDestWidth;
	}
*/
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
	spr.attr = 0;
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


static int RegistTex( char *data, int sx, int sy, int width, int height, int sw )
{
	int sel;
	int y;
	char *p;
	char *src;
	TEXINF *t;
	sel = GetNextTexID();
	p = (char *)malloc( sx*sy );
	t = GetTex( sel );
	t->flag = 1;
	t->mode = 0;
	t->sx = width;
	t->sy = height;
	t->data = p;

	//		上下逆にしてコピーする
	//
	if ( sw == 0 ) {
		for(y=0;y<sy;y++) {
			src = data + (sx*(sy-1-y));
			memcpy( p, src, sx );
			p+=sx;
		}
	}
	else {
		memcpy( p, data, sx*sy );
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
	nTexHeight = sy;
}
