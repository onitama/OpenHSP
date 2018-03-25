#include "game.h"
#include "../supio.h"

const float RADIUS_ROCK = 14.0f;

void GAME::init(VESSEL* pVessel_)
{
	GAMEROCK* pG = startGameRock;
	while(pG) {
		pG = delGameRock(pG);
	}
	pVessel = pVessel_;

	// 剛体用ログ領域確保
	pVessel->rockbase.init( GetSysReq( SYSREQ_MAXLOG ) );

	// ゲーム情報初期化
	gameTick = 0;
	rocknum_wall = -1;
}

// proc

void GAME::proc(void)
{
	curlog = NULL;
	gameTick ++;

	//	Vesselの更新
	pVessel->rockbase.resetCollisionLog();
	for(int i = 0; i < GetSysReq(SYSREQ_PHYSICS_RATE); i ++) {
		pVessel->proc(i);
	}
}

int GAME::addRock(int stat, int type, float x, float y, float angle, float xsize, float ysize, int nvertex, const FVECTOR2* pVec, float margin, int collisionCheck, unsigned int group, unsigned int exceptGroup)
{
	float weight;
	float moment;
	float buoyancy;
	weight = GetSysReqF(SYSREQ_DEFAULT_WEIGHT);
	moment = GetSysReqF(SYSREQ_DEFAULT_MOMENT);
	buoyancy = 0.0f;

	int r = pVessel->rockbase.addRock(stat, type, weight, moment, buoyancy, x, y, angle, xsize, ysize, nvertex, pVec, margin, collisionCheck, group, exceptGroup);
	if(r >= 0) {
		ROCK* pR = &pVessel->rockbase.rock[r];
		GAMEROCK* pG = addGameRock();

		pG->pRock = pR;
		pR->pGameRock = pG;
		pG->UpdateMinMaxInfo();
	}

	return r;
}

class GAMEROCK* GAME::delRock(GAMEROCK* pG)
{
	//		ROCKとGAMEROCKを削除
	pVessel->rockbase.delRock(pG->pRock);

	return delGameRock(pG);
}

class GAMEROCK* GAME::addGameRock(void)
{
	GAMEROCK* pG = new GAMEROCK;

	pG->next = NULL;
	pG->prev = NULL;

	if(startGameRock == NULL) {
		startGameRock = pG;
	} else {
		endGameRock->next = pG;
		pG->prev = endGameRock;
	}

	endGameRock = pG;

	return pG;
}

class GAMEROCK* GAME::delGameRock(GAMEROCK* pG)
{
	//		GAMEROCKのみ削除
	//
	GAMEROCK* pGn = pG->next;

	if(pGn) {
		pGn->prev = pG->prev;
	} else {
		endGameRock = pG->prev;
	}

	if(pG->prev) {
		pG->prev->next = pGn;
	} else {
		startGameRock = pGn;
	}

	delete pG;

	return pGn;
}


class ROCK* GAME::getRock( int num )
{
	ROCK *rock;
	if (( num < 0 )||( num >= MAX_ROCK )) return NULL;
	rock = &(pVessel->rockbase.rock[num]);
	if( rock->stat == ROCK::stat_reserve) return NULL;
	return rock;
}


class GAMEROCK* GAME::getGameRock( int num )
{
	ROCK *rock;
	if (( num < 0 )||( num >= MAX_ROCK )) return NULL;
	rock = &(pVessel->rockbase.rock[num]);
	if( rock->stat == ROCK::stat_reserve) return NULL;
	return rock->pGameRock;
}


int GAME::addShape(int shape, int stat, int type, float x, float y, float r, float xsize, float ysize, int coll, int mygroup, int exgroup, int loggroup )
{
	FVECTOR2 tt[32];
	int k = 0;
	int nR;

	k = shape;
	if ( k < 3 ) k = 3;
	if ( k > 32 ) k = 32;
	for(int j = 0; j < k; j ++) {
		tt[j].x = cosf((float)(j + 0.5f) * F_PI * 2.0f / (float)k);
		tt[j].y = -sinf((float)(j + 0.5f) * F_PI * 2.0f / (float)k);
	}
	nR = addRock( stat, type, x, y, r, xsize, ysize, k, tt, margin_default, coll, mygroup, exgroup );

	if(nR >= 0) {
		setRockWeight( nR, GetSysReqF(SYSREQ_DEFAULT_WEIGHT), GetSysReqF(SYSREQ_DEFAULT_MOMENT) );
		setRockDamper( nR, GetSysReqF(SYSREQ_DEFAULT_DAMPER), GetSysReqF(SYSREQ_DEFAULT_FRICTION) );
		setRockGroup( nR, mygroup, exgroup, loggroup );
	}

	return nR;
}


int GAME::setBorder( float x1, float y1, float x2, float y2, float cen_x, float cen_y )
{
	//		外壁を設定
	//
	FVECTOR2 tt[4];

	if ( rocknum_wall != -1 ) {
		delRock( getGameRock(rocknum_wall) );
	}

	border_x1 = cen_x + x1;
	border_y1 = cen_y + y1;
	border_x2 = cen_x + x2;
	border_y2 = cen_y + y2;

#ifdef _TACSCAN
	setTargetBorder( cen_x, cen_y, border_y1, border_y2 );
#endif

	tt[0].x = x1; tt[0].y = y1;
	tt[1].x = x1; tt[1].y = y2;
	tt[2].x = x2; tt[2].y = y2;
	tt[3].x = x2; tt[3].y = y1;

	rocknum_wall = addRock(ROCK::stat_active, ROCK::type_bind | ROCK::type_inner, cen_x, cen_y, F_PI * 0.0f, 1.0f, 1.0f, 4, tt, margin_default, false);
	if(rocknum_wall >= 0) {
		pVessel->rockbase.rock[rocknum_wall].type |= ROCK::type_quake;
		pVessel->rockbase.rock[rocknum_wall].friction = 1.0f;
		pVessel->rockbase.rock[rocknum_wall].group = 0x80000000;
	}
	return 0;
}


void GAMEROCK::UpdateMinMaxInfo( void )
{
	//		GAMEROCKマテリアルのmin,max情報を更新
	//
	float x,y;

	ROCKBASE* rockbase;

	rockbase = pRock->base;
	minx = maxx = 0.0f;
	miny = maxy = 0.0f;

	int k = 0, nv = 0, nvo;
	while(1) {
		nvo = nv;
		nv = rockbase->areaVertex[pRock->startAreaVertex + k];
		if(nv == 0) {
			break;
		}
		FVECTOR2 p;
		for(int j = nvo; j < nv; j ++) {
			p = rockbase->vertex[pRock->startVertex + j];
			x = p.x; y = p.y;
			if ( x < minx ) minx = x;
			if ( y < miny ) miny = y;
			if ( x > maxx ) maxx = x;
			if ( y > maxy ) maxy = y;
		}
		k ++;
	}
}


//	Service Functions

void GAME::setRockWeight( int num, float weight, float moment )
{
	//		weight等を設定
	//
	ROCK* pR = getRock( num );
	if ( pR == NULL ) return;
	if((pR->type & ROCK::type_bindXY) != ROCK::type_bindXY) {
		pR->weight = weight;
	}
	pR->moment = moment;
}


void GAME::setRockDamper( int num, float damper, float friction )
{
	//		damper等を設定
	//
	ROCK* pR = getRock( num );
	if ( pR == NULL ) return;
	pR->damper = damper;
	pR->friction = friction;
}


void GAME::setRockInertia( int num, float inertia, float gravity )
{
	//		inertia等を設定
	//
	ROCK* pR = getRock( num );
	if ( pR == NULL ) return;
	pR->inertia = inertia;
	pR->gravity = gravity;
}


void GAME::setRockAngle( int num, float angle )
{
	//		angleを設定
	//
	ROCK* pR = getRock( num );
	if ( pR == NULL ) return;
	pR->angle = angle;
}


void GAME::setRockCenter( int num, float x, float y )
{
	//		center位置を設定
	//
	ROCK* pR = getRock( num );
	if ( pR == NULL ) return;
	pR->center.x = x;
	pR->center.y = y;
}


void GAME::setRockSpeed( int num, float px, float py, float pr, int option )
{
	//		speedを設定
	//
	ROCK* pR = getRock( num );
	if ( pR == NULL ) return;


	switch( option ) {
	case 2:
		pR->speed.x = px;
		pR->speed.y = py;
		pR->rSpeed = pr;
		break;
	case 1:
		pR->speed.x = px;
		pR->speed.y = py;
		pR->rSpeed = pr;
	default:
		pR->speed.x += px;
		pR->speed.y += py;
		pR->rSpeed += pr;
	}
}


void GAME::setRockGroup( int num, int group, int exgroup, int loggroup )
{
	//		groupを設定
	//
	ROCK* pR = getRock( num );
	if ( pR == NULL ) return;
	pR->group = (unsigned int)group;
	pR->exceptGroup = (unsigned int)exgroup;
	pR->recordGroup = (unsigned int)loggroup;
}


void GAME::setRockType( int num, unsigned int type, int calcop )
{
	//		typeを設定
	//		(calcop:0=set,1=on,2=off)
	//
	int oldtype;
	ROCK* pR = getRock( num );
	if ( pR == NULL ) return;
	oldtype = pR->type;
	switch( calcop ) {
	case 1:
		pR->type |= type;
		break;
	case 2:
		pR->type &= ~type;
		break;
	default:
		pR->type = type;
		break;
	}
	if (( oldtype & ROCK::type_inner )||( type & ROCK::type_inner )) {
		pR->reset();
	}
}


void GAME::setRockStat( int num, unsigned int stat, int sleepcount )
{
	//		statを設定
	//
	ROCK* pR = getRock( num );
	if ( pR == NULL ) return;
	pR->stat = stat;
	pR->sleepCount = sleepcount;
}


void GAME::getCollision( int num, int target )
{
	//		コリジョン取得開始
	//
	curlog = NULL;
	collision_id = num;
	collision_target = target;
}


COLLISIONLOG* GAME::getCollisionLog( void )
{
	//		コリジョン取得
	//
	curlog = pVessel->rockbase.getCollisionLog( collision_id, collision_target, curlog==NULL );
	return curlog;
}


void GAME::setRockMaterial( int num, int type, int matid, int subid )
{
	//		マテリアルを設定
	//
	GAMEROCK *gr = getGameRock( num );
	if ( gr == NULL ) return;
	gr->material_type = type;
	gr->material_id = matid;
	gr->material_subid = subid;
}


void GAME::setRockMaterial2( int num, int offsetx, int offsety, float zoomx, float zoomy )
{
	//		マテリアルを設定
	//
	GAMEROCK *gr = getGameRock( num );
	if ( gr == NULL ) return;
	gr->material_offsetx = offsetx;
	gr->material_offsety = offsety;
	gr->material_zoomx = zoomx;
	gr->material_zoomy = zoomy;
}


void GAME::setRockMaterial3( int num, int gmode, int rate )
{
	//		マテリアルを設定
	//
	GAMEROCK *gr = getGameRock( num );
	if ( gr == NULL ) return;
	gr->material_gmode = gmode;
	gr->material_grate = rate;
}


void GAME::setRockUserData( int num, int user1, int user2, int user3 )
{
	//		ユーザー定義データを設定
	//
	GAMEROCK *gr = getGameRock( num );
	if ( gr == NULL ) return;
	gr->user1 = user1;
	gr->user2 = user2;
	gr->user3 = user3;
}


void GAME::setRockUserData2( int num, float user4, float user5, float user6 )
{
	//		ユーザー定義データを設定
	//
	GAMEROCK *gr = getGameRock( num );
	if ( gr == NULL ) return;
	gr->user4 = user4;
	gr->user5 = user5;
	gr->user6 = user6;
}


void GAME::BlastAll( float ax, float ay, float power, float p_near, float p_far, int sw )
{
	//		放射状に力を与える
	//
	int i;
	for(i = 0; i < MAX_ROCK; i ++) {
		ROCK* pR = getRock( i );
		if ( pR != NULL ) {
			if(pR->stat == ROCK::stat_active) {
				if ( sw ) {
					pR->blastMove( ax, ay,  power, p_near, p_far );
				} else {
					pR->blast( ax, ay,  power, p_near, p_far );
				}
			}
		}
	}
}


int GAME::CheckInsidePosition( int num, float chkx, float chky )
{
	//		指定座標がオブジェクト範囲内かどうか調べる
	//
	int i;
	int dummy;
	float fmx,fmy;

	fmx = chkx; fmy = chky;
	if ( num < 0 ) {
		for( i = 0; i < MAX_ROCK; i++ ) {
			ROCK* pR = getRock( i );
			if ( pR != NULL ) {
				if(pR->stat == ROCK::stat_active && (pR->type & ROCK::type_inner)==0 ) {
					if(fmx >= pR->minPos.x && fmx <= pR->maxPos.x && fmy >= pR->minPos.y && fmy <= pR->maxPos.y) {
						float d = pR->collision_point_n(fmx, fmy, NULL, dummy);
						if(d > 0.0f) {
							return i;
						}
					}
				}
			}
		}
	} else {
		ROCK* pR = getRock( num );
		if ( pR != NULL ) {
			if(pR->stat == ROCK::stat_active && (pR->type & ROCK::type_inner)==0 ) {
				if(fmx >= pR->minPos.x && fmx <= pR->maxPos.x && fmy >= pR->minPos.y && fmy <= pR->maxPos.y) {
					float d = pR->collision_point_n(fmx, fmy, NULL, dummy);
					if(d > 0.0f) {
						return num;
					}
				}
			}
		}
	}
	return -1;
}


float GAME::moveRockToTarget( ROCK *pR, float x, float y, float angle, float maxspeed, float maxrotsp, int opt )
{
	//		指定座標(角度)にオブジェクトを近づける
	//
	float px,py,prot,gorot,dist,org;
	int opti;
	if ( pR == NULL ) return -1.0f;

	opti = opt & 15;
	px = x - pR->center.x;
	py = y - pR->center.y;
	gorot = atan2( py, px );
	px = abs( px );
	py = abs( py );
	org = sqrt( px*px + py*py );
	dist = px + py;

	if (( dist < 2.0f )&&( (opt&64)==0 )) {
		px = py = 0.0f;
		pR->speed.x = 0.0f;
		pR->speed.y = 0.0f;
	} else {
		if ( opt&32 ) { dist *= 0.01f; }
		if ( dist > maxspeed ) dist = maxspeed;
		py = sin( gorot ) * dist;
		px = cos( gorot ) * dist;
	}

	if ( opt & 16 ) {
		prot = gorot + (F_PI*0.5f) - pR->angle;
	} else {
		prot = angle - pR->angle;
	}
	if ( prot < 0.0f ) {
		if ( prot < -maxrotsp ) prot = -maxrotsp;
	} else {
		if ( prot > maxrotsp ) prot = maxrotsp;
	}

	switch( opti ) {
	case 1:
		pR->pushMove( 0.0f, 0.0f, px, py );
		pR->rSpeed = prot;
		break;
	case 2:
		pR->pushMove( px*0.1f, py*0.1f, px, py );
		break;
	default:
		//pR->center.x += px;
		//pR->center.y += py;
		pR->speed.x = (pR->force.x + px ); // pR->weight;
		pR->speed.y = (pR->force.y + py ); // pR->weight;
		//pR->rSpeed += prot;
		break;
	}
	return org;
}


#define FRND_4096 (1.0f / 4096.0f)

void GAME::ExecuteAutoWipe( void )
{
	int id;
	ROCK *tr;
	COLLISIONLOG* clog;

	for(int i = 0; i < MAX_ROCK; i ++) {
		ROCK* pR = getRock(i);
		if ( pR != NULL ) {
			if(pR->stat == ROCK::stat_active) {
				if ( pR->type & ROCK::type_autowipe ) {
					getCollision( i, -1 );
					while(1) {
						clog = getCollisionLog();
						if ( clog == NULL ) break;
						id = (int)clog->rockNum & 0xffff;
						if ( id == i ) {
							id = (int)((clog->rockNum>>16) & 0xffff);
						}
						tr = getRock( id );
						if ( tr != NULL ) {
							if ( tr->group & pR->recordGroup ) {
							//if ( tr->group & ROCK::type_debug ) {
								delRock( getGameRock( i ) );
								break;
							}
						}
					}
				}
				if ( pR->type & ROCK::type_autoguide ) {
					if ( moveRockAutoTarget( pR ) ) {
						delRock( getGameRock( i ) );
					}
				}
			}
		}
	}
}


void GAME::setTargetBorder( float centerx, float centery, float y1, float y2 )
{
	target_cenx = centerx;
	target_ceny = centery;
	target_y1 = y1;
	target_y2 = y2;
	target_px = 0.0f;
	target_py = 0.0f;
	target_defx1 = border_x1;
	target_defy1 = border_y1;
	target_defx2 = border_x2;
	target_defy2 = border_y2;
}


void GAME::updateRockAutoTarget( ROCK *pR )
{
	int tty;
	float x,y;
	tty = pR->target_opt;
	if ( tty & target_optposx ) {
		x = ((float)(rand()&4095) * FRND_4096 ) * ( pR->target_x2 - pR->target_x1 );
		pR->target_x = pR->target_x1 + x;
		//Alertf("x=%f",pR->target_x);
	}
	if ( tty & target_optposy ) {
		y = ((float)(rand()&4095) * FRND_4096 ) * ( pR->target_y2 - pR->target_y1 );
		pR->target_y = pR->target_y1 + y;
		//Alertf("y=%f",pR->target_y);
	}
}


int GAME::moveRockAutoTarget( ROCK *pR )
{
	int opt,tty;
	float tx,ty,dist;

	if ( pR == NULL ) return 0;

	opt = 16+32+64;
	tty = pR->target_opt;

	if (( tty & target_optdirect )==0) opt|=2;

	if ( tty & ( target_addpos|target_addpos2) ) {
		if ( tty & target_addpos ) {
			pR->speed.x = pR->target_px;
			pR->speed.y = pR->target_py;
		}
		if ( tty & target_addpos2 ) {
			pR->speed.x += pR->target_px;
			pR->speed.y += pR->target_py;
		}
	} else {
		tx = pR->target_x;
		ty = pR->target_y;
		if ( pR->target_id >= 0 ) {
			ROCK *tr = getRock( pR->target_id );
			if ( tr != NULL ) {
				tx = tr->center.x;
				ty = tr->center.y;
			}
		}
		dist = moveRockToTarget( pR, tx, ty, 0.0f, pR->target_maxsp, 0.1f, opt );
		if ( pR->target_int > 0 ) {
			pR->target_timer++;
			if ( pR->target_timer >= pR->target_int ) {
				pR->target_timer = 0;
				dist = 0.0f;
			}
		}

		if ( dist < 10.0f ) {
			if ( tty & target_optincy ) {
				pR->target_y1 += pR->target_py;
				pR->target_y2 += pR->target_py;
				//Alertf("incy=%f",pR->target_y1);
			}
			if ( tty & target_optflipx ) {
				pR->target_x1 = target_cenx - ( pR->target_x1 - target_cenx );
				pR->target_x2 = target_cenx - ( pR->target_x2 - target_cenx );
			}
			if ( tty & target_optflipy ) {
				pR->target_y1 = target_ceny - ( pR->target_y1 - target_ceny );
				pR->target_y2 = target_ceny - ( pR->target_y2 - target_ceny );
			}
			updateRockAutoTarget( pR );
		}
	}

	if (( tty & target_nowipe )==0 ) {
		float x,y;
		x = pR->center.x;
		y = pR->center.y;
		if ( ( y < target_y1 )||( y >= target_y2 )) return -1;
	}
	return 0;
}


void GAME::setTargetDefault( float x1, float y1, float x2, float y2 )
{
	target_defx1 = x1;
	target_defy1 = y1;
	target_defx2 = x2;
	target_defy2 = y2;
}


void GAME::setTargetDefaultAdd( float px, float py )
{
	target_px = px;
	target_py = py;
}


void GAME::setRockTargetTimer( int id, int timer )
{
	ROCK* pR = getRock(id);
	if ( pR == NULL ) return;
	pR->target_int = (short)timer;
	pR->target_timer = 0;
}


void GAME::resetRockTarget( int id, int opt, int target_id, float targetx, float targety, float speed )
{
	ROCK* pR = getRock(id);
	if ( pR == NULL ) return;

	pR->target_id = target_id;
	pR->target_x = targetx;
	pR->target_y = targety;
	pR->target_maxsp = speed;
	pR->target_opt = opt;
	pR->type |= ROCK::type_autoguide;
	setRockTargetAdd( id, target_px, target_py );
	setRockTargetArea( id, target_defx1, target_defy1, target_defx2, target_defy2 );
	setRockTargetTimer( id, 0 );
	updateRockAutoTarget( pR );
}


void GAME::setRockTargetAdd( int id, float px, float py )
{
	ROCK* pR = getRock(id);
	if ( pR == NULL ) return;
	pR->target_px = px;
	pR->target_py = py;
}


void GAME::setRockTargetArea( int id, float x1, float y1, float x2, float y2 )
{
	ROCK* pR = getRock(id);
	if ( pR == NULL ) return;
	pR->target_x1 = x1;
	pR->target_y1 = y1;
	pR->target_x2 = x2;
	pR->target_y2 = y2;
}



