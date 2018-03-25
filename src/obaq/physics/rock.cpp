//
// OBAQ physics library
// Copyright   : Zener Works, Inc/kuni/onitama
// License     : CC BY-NC-ND 4.0
// See OBAQlicense.txt for information
// This is a part of HSP related product.
// http://hsp.tv/
//
//

#include "rock.h"

//
// base
//

int c1, c2, c3;

void ROCKBASE::proc(int procCount, bool wake)
{
//	DebugMessage(0, "nSide %d, nVertex %d, nVertexW %d\n", nSide, nVertex, nVertexW);

	// フレーム毎初期化
	for(int i = 0; i < MAX_ROCK; i ++) {
		ROCK* pR = &rock[i];
		if(pR->stat == ROCK::stat_sleep) {
			if(pR->sleepCount != 0) {
				if(pR->sleepCount > 1 || wake) {
					pR->sleepCount --;
				}
				if(pR->sleepCount == 0) {
					pR->stat = ROCK::stat_active;
					for(int j = 0; j < MAX_ROCK; j ++) {
						if(i != j) {
							ROCK* pR2 = &rock[j];
						//	if(pR2->stat == ROCK::stat_active && !(pR2->type & (ROCK::type_inner | ROCK::type_sponge))) {
							if(pR2->stat == ROCK::stat_active && !(pR2->type & (ROCK::type_inner))) {
							//	float lx = pR2->center.x - pR->center.x;
							//	float ly = pR2->center.y - pR->center.y;
							//	DebugMessage(0, "sleep %d %d : %d %d\n", i, j, pR->parentRock, pR2->parentRock);
							//	if((!(pR->group & pR2->exceptGroup) && !(pR2->group & pR->exceptGroup)) && (lx * lx + ly * ly < 12.0f * 12.0f || pR->check_rock(pR2) >= -1.0f || pR2->check_rock(pR) >= -1.0f)) {
								if((!(pR->group & pR2->exceptGroup) && !(pR2->group & pR->exceptGroup)) && (pR->check_rock(pR2) >= -1.0f || pR2->check_rock(pR) >= -1.0f)) {
									pR->stat = ROCK::stat_sleep;
									pR->sleepCount = 1;
									break;
								}
							}
						}
					}

				}
			}
		}
		if(pR->stat == ROCK::stat_active) {
			pR->force.x = 0.0f;
			pR->force.y = 0.0f;
			pR->torque = 0.0f;
			pR->torqueV = 0.0f;

			pR->ground = 0;

			if(procCount == 0) {
				pR->bombarded = 0;
			}

		//	pR->showered = 0;
		//	pR->cost = cosf(pR->angle);
		//	pR->sint = sinf(pR->angle);
		}
		pR->shave = false;
	}

	c1 = 0;
	c2 = 0;
	c3 = 0;
	for(int i = 0; i < MAX_ROCK; i ++) {
		ROCK* pR = &rock[i];
		if(pR->stat == ROCK::stat_active && !(pR->type & (ROCK::type_bar | ROCK::type_inner))) {
	//	if(pR->stat == ROCK::stat_active) {
		//	pR->calcVertexW();

			// 岩間コリジョン
			for(int j = 0; j < MAX_ROCK; j ++) {
				ROCK* pR2 = &rock[j];
#ifdef ENABLE_COLLISIONLOG
				if(pR2->stat == ROCK::stat_active && pR != pR2 && ((!(pR->group & pR2->exceptGroup) && !(pR2->group & pR->exceptGroup)) || ((pR->group & pR2->recordGroup) || (pR2->group & pR->recordGroup))) && !(pR2->type & ROCK::type_bar)) {
#else
				if(pR2->stat == ROCK::stat_active && pR != pR2 && (!(pR->group & pR2->exceptGroup) && !(pR2->group & pR->exceptGroup)) && !(pR2->type & ROCK::type_bar)) {
#endif
				//	float xs = pR->center.x - pR2->center.x;
				//	float ys = pR->center.y - pR2->center.y;
				//	float r = pR->radiusSqrt + pR2->radiusSqrt;
				//	if(xs * xs + ys * ys <= r * r) {
					if(pR->minPos.x <= pR2->maxPos.x && pR2->minPos.x <= pR->maxPos.x && pR->minPos.y <= pR2->maxPos.y && pR2->minPos.y <= pR->maxPos.y) {
						float d = pR->collision_rock(pR2);
						mutationRockRock(pR, pR2, d)
					}
				//	if(pR2->type & ROCK::type_inner) {
				//	}
				}
			}
		}
	}
//	DebugMessage(0, "c %d %d %d\n", c1, c2, c3);
}

void ROCKBASE::proc2(void)
{
	static float maxaccel = 0.0f;
	static float maxturn = 0.0f;
	FVECTOR2 tpos;

	// 親子パーツ吸着
	for(int i = 0; i < MAX_ROCK; i ++) {
		ROCK* pR = &rock[i];
		if(pR->stat == ROCK::stat_active) {
			if(pR->parentRock >= 0) {
				ROCK* pR2 = &rock[pR->parentRock];
				pR->tmpCenter.x = pR->center.x - cosf(pR2->parentAngle + pR->parentAngle) * pR->parentDistance;
				pR->tmpCenter.y = pR->center.y - sinf(pR2->parentAngle + pR->parentAngle) * pR->parentDistance;
				pR->tmpAngle = pR->angle - pR->parentAngle;
			} else {
				pR->tmpCenter = pR->center;
				pR->tmpAngle = pR->angle;
			}
		}
	}
	for(int i = 0; i < MAX_ROCK; i ++) {
		ROCK* pR = &rock[i];
		if(pR->stat == ROCK::stat_active && pR->parentRock >= 0) {
			ROCK* pR2 = &rock[pR->parentRock];
			float x = pR->tmpCenter.x - pR2->tmpCenter.x;
			float y = pR->tmpCenter.y - pR2->tmpCenter.y;
			pR->center.x -= x / 2.0f;
			pR2->center.x += x / 16.0f;
			pR->center.y -= y / 2.0f;
			pR2->center.y += y / 16.0f;

			float r = pR->tmpAngle - pR2->parentAngle;
			pR->angle -= r / 4.0f;
		}
	}

	float ga = gravity.x * gravity.x + gravity.y * gravity.y;

	// 移動
	for(int i = 0; i < MAX_ROCK; i ++) {
		ROCK* pR = &rock[i];
		if(pR->stat == ROCK::stat_active) {
			// ワイヤ処理
			for(int j = 0; j < MAX_ROCK_WIRE; j ++) {
				if(pR->wirenum[j] >= 0) {
					WIRE* pW = &wire[pR->wirenum[j]];
					ROCK* pR2;

					int vn = pW->vertex;
					float xr, yr;
					if(vn >= 0) {
						xr = vertexW[pR->startVertexW + vn].x;
						yr = vertexW[pR->startVertexW + vn].y;
					} else {
						xr = pR->center.x;
						yr = pR->center.y;
					}

					tpos.x = pW->position.x;
					tpos.y = pW->position.y;
					if(pW->rock >= 0) {
						pR2 = &rock[pW->rock];
						float x1 = tpos.x;
						float y1 = tpos.y;
						tpos.x = (x1 * pR2->cost - y1 * pR2->sint) + pR2->center.x;
						tpos.y = (x1 * pR2->sint + y1 * pR2->cost) + pR2->center.y;
					}

					if(pW->unfix == 0) {
						float xb = tpos.x - xr;
						float yb = tpos.y - yr;
						float l = sqrtf(xb * xb + yb * yb);
						float l2 = pW->length;
						if(l > l2) {
							l = (l - l2) / l * pW->tension;
							xb *= l;
							yb *= l;
							pR->push(xr, yr, xb, yb);
							if(pW->rock >= 0) {
								pR2->push(tpos.x, tpos.y, -xb, -yb);
							}
						}
					}
					if(pW->length > 0.0f) {
						if(pW->nPoint == 0) {
							pW->nPoint = (short)ceilf(pW->length / 2.0f);
							pW->jointLength = pW->length / (float)pW->nPoint;
							if(pW->nPoint > MAX_WIRE_POINT) {
								pW->nPoint = MAX_WIRE_POINT;
							}
							int n = pW->nPoint - 1;
							pW->pointPosition[n] = tpos;
							for(int t = 0; t < n; t ++) {
								pW->pointPosition[t].x = (xr * (float)(n - t) + tpos.x * (float)(t + 1)) / (float)(n + 1);
								pW->pointPosition[t].y = (yr * (float)(n - t) + tpos.y * (float)(t + 1)) / (float)(n + 1);
								pW->pointSpeed[t].x = pW->pointSpeed[t].y = 0.0f;
							}
						} else {
							int n = pW->nPoint - 1;
							float jl = pW->jointLength * 0.94f, jl2 = jl * jl;
							if(pW->unfix == 0) {
								pW->pointPosition[n] = tpos;
							}
							if(n > 0) {
								for(int ll = 0; ll < 8; ll ++) {
									float x1, y1, x2, y2;
									if(pW->vertex >= 0) {
										x2 = vertexW[pR->startVertexW + pW->vertex].x;
										y2 = vertexW[pR->startVertexW + pW->vertex].y;
									} else {
										x2 = pR->center.x;
										y2 = pR->center.y;
									}

									for(int t = 0; t <= n; t ++) {
										x1 = x2;
										y1 = y2;
										x2 = pW->pointPosition[t].x;
										y2 = pW->pointPosition[t].y;

										float x12 = x1 - x2;
										float y12 = y1 - y2;
										float l12 = x12 * x12 + y12 * y12;
										if(l12 > jl2) {
											l12 = sqrtf(l12);
											l12 = (l12 - jl) / l12 * 0.8f;
											pW->pointSpeed[t].x += x12 * l12;
											pW->pointSpeed[t].y += y12 * l12;
											if(t > 0) {
												pW->pointSpeed[t - 1].x -= x12 * l12;
												pW->pointSpeed[t - 1].y -= y12 * l12;
											}
										}
									}
									for(int t = n + pW->unfix - 1; t >= 0; t --) {
										pW->pointPosition[t].x += pW->pointSpeed[t].x;
										pW->pointPosition[t].y += pW->pointSpeed[t].y + 0.05f / 4.0f;
										pW->pointSpeed[t].x *= 0.95f;
										pW->pointSpeed[t].y *= 0.95f;
									}
								}
							}
						}
					}
				}
			}

			{
				pR->tSign <<= 1;
				if(pR->torqueV < 0.0f) {
					pR->tSign |= 1;
				}
				pR->stop <<= 1;

				float xp = pR->force.x / pR->weight + gravity.x * pR->gravity;
				float yp = pR->force.y / pR->weight + gravity.y * pR->gravity;
				float ap = xp * xp + yp * yp;
				if(ap < pR->friction * ga * pR->gravity * pR->gravity / (4.0f * 4.0f)) {
					if(pR->torqueV == 0.0f) {
						pR->stop |= 1;
					//	DebugMessage(0, "equal\n");
					} else {
						unsigned int ts = pR->tSign;
						unsigned int cb = ts & 0x00000001;
					//	int ct = 0;
						for(int i = 0; i < 31; i ++) {
							ts >>= 1;
							if((ts & 0x00000001) != cb) {
								cb ^= 0x00000001;
							//	ct ++;
							//	if(ct > 0) {
									pR->stop |= 1;
									break;
							//	}
							}
						}
					}
				}

				if(!(pR->type & (ROCK::type_bindX | ROCK::type_freeze))) {
					if(!(pR->type & (ROCK::type_bindY | ROCK::type_freeze))) {
						if((pR->stop & 1) == 0) {
							if(ap > 0.5f * 0.5f) {
								ap = sqrtf(ap);
							//	DebugMessage(0,"accel %f\n", a);
								xp *= 0.5f / ap;
								yp *= 0.5f / ap;
							}
							pR->speed.x += xp;
							pR->speed.y += yp;

							pR->speed.x *= pR->inertia;
							pR->speed.y *= pR->inertia;

							float s = pR->speed.x * pR->speed.x + pR->speed.y * pR->speed.y;
							if(s > maxRockSpeed * maxRockSpeed) {
								s = sqrtf(s);
								pR->speed.x *= maxRockSpeed / s;
								pR->speed.y *= maxRockSpeed / s;
							//	DebugMessage(0, "%x : %f\n", pR->pGameRock->attr, s);
							}
						} else {
							pR->speed.x *= 0.95f;
							pR->speed.y *= 0.95f;
						}
						pR->center.x += pR->speed.x;
						pR->center.y += pR->speed.y;
					} else {
						pR->speed.x += pR->force.x / pR->weight + gravity.x * pR->gravity;
						pR->speed.x *= pR->inertia;
						pR->speed.y = 0.0f;
						float s = fabsf(pR->speed.x);
						if(s > maxRockSpeed) {
							pR->speed.x *= maxRockSpeed / s;
						}
						pR->center.x += pR->speed.x;
					}
				} else {
					if(!(pR->type & (ROCK::type_bindY | ROCK::type_freeze))) {
						pR->speed.y += pR->force.y / pR->weight + gravity.y * pR->gravity;
						pR->speed.y *= pR->inertia;
						pR->speed.x = 0.0f;
						float s = fabsf(pR->speed.y);
						if(s > maxRockSpeed) {
							pR->speed.y *= maxRockSpeed / s;
						}
						pR->center.y += pR->speed.y;
					} else {
						pR->speed.x = pR->speed.y = 0.0f;
					}
				}

				if(!(pR->type & (ROCK::type_bindR | ROCK::type_freeze))) {
					float a = pR->torque / pR->moment;
					if(a > (F_PI / 360.0f)) {
						a = F_PI / 360.0f;
					} else if(a < (-F_PI / 360.0f)) {
						a = -F_PI / 360.0f;
					}
					pR->rSpeed += a;
					if((pR->stop & 1) == 0) {
						pR->rSpeed *= pR->inertia;
					}
					pR->angle += pR->rSpeed;
#ifdef _TACSCAN
					if ( pR->type & ROCK::type_rewind ) {
						float pang;
						pang = pR->angle * 0.05f;
						if ( pang > 0.01f ) pang = 0.01f;
						if ( pang < -0.01f ) pang = -0.01f;
						pR->angle -= pang;
						pR->rSpeed *= 0.5f;
					}
#endif
				} else {
					pR->rSpeed = 0.0f;
				}

				pR->calcVertexW();
			}
		}
	}

//	tick ++;
}

// 岩の存在テーブルを作成
/*
void ROCKBASE::makeRockMap(unsigned char* pMap, int width, int height)
{
	memset(pMap, 0, width * height * sizeof(unsigned char));

	for(int i = 0; i < MAX_ROCK; i ++) {
		ROCK* pR = &rock[i];
		if(pR->stat == ROCK::stat_active && !(pR->type & (ROCK::type_inner))) {
			int x1 = (int)(pR->center.x - pR->radius);
			int x2 = (int)(pR->center.x + pR->radius);
			int y1 = (int)(pR->center.y - pR->radius);
			int y2 = (int)(pR->center.y + pR->radius);
			if(x1 >= width || x2 < 0 || y1 >= height || y2 < 0) {
				break;
			}
			if(x1 < 0) {
				x1 = 0;
			}
			if(x2 >= width) {
				x2 = width - 1;
			}
			if(y1 < 0) {
				y1 = 0;
			}
			if(y2 >= height) {
				y2 = height - 1;
			}
			for(int y = y1; y <= y2; y ++) {
				for(int x = x1; x <= x2; x ++) {
					if(pR->collision_point((float)x, (float)y, NULL) > 0.0f) {
						pMap[x + y * width] = 1;
					}
				}
			}
		}
	}
}
*/

// 新しい岩を登録
int ROCKBASE::addRock(int stat, int type, float weight, float moment, float buoyancy, float x, float y, float angle, float xsize, float ysize, int nvertex, const FVECTOR2* pVec, float margin, int collisionCheck, unsigned int group, unsigned int exceptGroup)
{
	for(int i = 0; i < MAX_ROCK; i ++) {
		ROCK* pR = &rock[i];
		if(pR->stat == ROCK::stat_reserve) {
			pR->startVertex = nVertex;
			pR->startVertexW = nVertexW;

			pR->speed.x = 0.0f;
			pR->speed.y = 0.0f;
			pR->rSpeed = 0.0f;
			pR->force.x = 0.0f;
			pR->force.y = 0.0f;
			pR->torque = 0.0f;

			pR->mineral = 0;

			pR->damage = 0.0f;
			pR->damageLimit = -1.0f;
			pR->damageShave = 0.0f;
			pR->bombarded = 0;

			pR->sleepCount = 0;

			pR->stop = 0;

#ifdef LITRON
			if(type & ROCK::type_puppet) {
				pR->inertia = 0.99f;
			} else {
				pR->inertia = 0.999f;
			}
			pR->friction = 0.1f;
			pR->damper = 0.35f;
#else
			pR->inertia = 0.999f;
			pR->friction = 0.7f;
			pR->damper = 0.7f;
#endif

			pR->round = 0;

			pR->group = 0x00000000;
			pR->exceptGroup = 0x00000000;
			pR->recordGroup = 0x00000000;

			for(int j = 0; j < MAX_ROCK_WIRE; j ++) {
				pR->wirenum[j] = -1;
			}

			pR->num = i;
			pR->base = this;

			pR->parentRock = -1;

			pR->group = group;
			pR->exceptGroup = exceptGroup;

			if(adjustRock(pR, stat, type, weight, moment, buoyancy, x, y, angle, xsize, ysize, nvertex, pVec, margin, collisionCheck) < 0) {
				pR->init();
				return ROCK::addFail_collision;
			}

			nVertexW += pR->nTotalVertex;

			ROCK* pR2 = &rock[0];
			while(pR2 != &rock[MAX_ROCK]) {
				if(pR2->stat != ROCK::stat_reserve && pR != pR2) {
					int n = pR2->nTotalVertex;
					if(n == pR->nTotalVertex) {
						if(memcmp(&vertex[pR->startVertex], &vertex[pR2->startVertex], sizeof(FVECTOR2) * n) == 0) {
							pR->startVertex = pR2->startVertex;
							//DebugMessage(0, "add(copy) nVertex %d, nVertexW %d", nVertex, nVertexW);
							goto areasearch;
						}
					}
				}
				pR2 ++;
			}

			nVertex += pR->nTotalVertex;
			//DebugMessage(0, "add(org) nVertex %d, nVertexW %d", nVertex, nVertexW);

areasearch:
			pR2 = &rock[0];
			while(pR2 != &rock[MAX_ROCK]) {
				if(pR2->stat != ROCK::stat_reserve && pR != pR2) {
					int n = pR2->nArea;
					if(n == pR->nArea) {
						if(memcmp(&areaVertex[pR->startAreaVertex], &areaVertex[pR2->startAreaVertex], n) == 0) {
							pR->startAreaVertex = pR2->startAreaVertex;
							//DebugMessage(0, ", nAreaVertex %d\n", nAreaVertex);
							return i;
						}
					}
				}
				pR2 ++;
			}

			nAreaVertex += pR->nArea + 1;
			//DebugMessage(0, ", nAreaVertex %d\n", nAreaVertex);
			return i;
		}
	}

	return ROCK::addFail_overflow;
}

int ROCKBASE::adjustRock(ROCK* pR, int stat, int type, float weight, float moment, float buoyancy, float x, float y, float angle, float xsize, float ysize, int nvertex, const FVECTOR2* pVec, float margin, int collisionCheck)
{
	pR->stat = stat;
	pR->type = type;

	if((type & ROCK::type_bindXY) == ROCK::type_bindXY) {
		pR->weight = FLT_MAX;
	} else {
		pR->weight = weight;
	}
	if(type & ROCK::type_bindR) {
		pR->moment = FLT_MAX;
	} else {
		pR->moment = moment;
	}

	pR->buoyancy = buoyancy;
	pR->center.x = x;
	pR->center.y = y;
	pR->angle = angle;

	pR->gravity = 1.0f;

	FVECTOR2* pV = &vertex[pR->startVertex];

	if(pR->startAreaVertex < 0) {
		pR->startAreaVertex = nAreaVertex;
	}

	if(nvertex > 0) {
		pR->nTotalVertex = nvertex;

		areaVertex[pR->startAreaVertex] = nvertex;
		areaVertex[pR->startAreaVertex + 1] = 0;
		pR->nArea = 1;

		for(int j = 0; j < nvertex; j ++) {
		//	pR->vertex[j] = *pVec ++;
			pV[j].x = pVec->x * xsize;
			pV[j].y = pVec->y * ysize;
			pVec ++;
		}
	} else {
		int tv = 0;
		int nav = pR->startAreaVertex;
		pR->nArea = 0;
		do {
			areaVertex[nav] = 0;
			while(pVec->x != FLT_MAX) {
			//	pR->vertex[j] = *pVec ++;
				pV[tv].x = pVec->x * xsize;
				pV[tv].y = pVec->y * ysize;
				pVec ++;
				areaVertex[nav] ++;
				tv ++;
			}
			pVec ++;
			pR->nArea ++;
			nav ++;
		} while(pVec->x != FLT_MAX);
		pR->nTotalVertex = tv;
		for(int j = pR->startAreaVertex + 1; j < pR->startAreaVertex + pR->nArea; j ++) {
			areaVertex[j] += areaVertex[j - 1];
		}
		areaVertex[nav] = 0;

		/*
		// debug
		DebugMessage(0, "0: %d %d\n", nArea, pR->nTotalVertex);
		for(j = 0; j < nArea; j ++) {
			DebugMessage(0, "1: %d %d\n", j, pR->nVertex[j]);
		}
		for(j = 0; j < pR->nTotalVertex; j ++) {
			DebugMessage(0, "2: %d %f %f\n", j, pR->vertex[j].x, pR->vertex[j].y);
		}
		*/
	}

	pR->setSide(margin);
	pR->calcVertexW();

	if(collisionCheck == ROCK::collisionCheck_active) {
		for(int j = 0; j < MAX_ROCK; j ++) {
			ROCK* pR2 = &rock[j];
			if(pR != pR2) {
				if(pR->minPos.x <= pR2->maxPos.x && pR2->minPos.x <= pR->maxPos.x && pR->minPos.y <= pR2->maxPos.y && pR2->minPos.y <= pR->maxPos.y) {
					if(pR2->stat == ROCK::stat_active && (!(pR->group & pR2->exceptGroup) && !(pR2->group & pR->exceptGroup)) && (pR->check_rock(pR2) >= -1.0f || pR2->check_rock(pR) >= -1.0f)) {
						return -1;
					}
				}
			}
		}
	} else if(collisionCheck == ROCK::collisionCheck_all) {
		for(int j = 0; j < MAX_ROCK; j ++) {
			ROCK* pR2 = &rock[j];
			if(pR != pR2) {
				if(pR->minPos.x <= pR2->maxPos.x && pR2->minPos.x <= pR->maxPos.x && pR->minPos.y <= pR2->maxPos.y && pR2->minPos.y <= pR->maxPos.y) {
					if(pR2->stat != ROCK::stat_reserve && (!(pR->group & pR2->exceptGroup) && !(pR2->group & pR->exceptGroup)) && (pR->check_rock(pR2) >= -1.0f || pR2->check_rock(pR) >= -1.0f)) {
						return -1;
					}
				}
			}
		}
	} else if(collisionCheck == ROCK::collisionCheck_nest) {
		for(int j = 0; j < MAX_ROCK; j ++) {
			ROCK* pR2 = &rock[j];
			if(pR != pR2) {
				if(pR->minPos.x <= pR2->maxPos.x && pR2->minPos.x <= pR->maxPos.x && pR->minPos.y <= pR2->maxPos.y && pR2->minPos.y <= pR->maxPos.y) {
					if(pR2->stat != ROCK::stat_reserve && (!(pR->group & pR2->exceptGroup) && !(pR2->group & pR->exceptGroup)) && !(pR2->type & ROCK::type_nest) && (pR->check_rock(pR2) >= -1.0f || pR2->check_rock(pR) >= -1.0f)) {
						return -1;
					}
				}
			}
		}
		for(int j = 0; j < MAX_ROCK; j ++) {
			ROCK* pR2 = &rock[j];
			if(pR != pR2) {
				if(pR2->stat == ROCK::stat_active && (pR2->type & ROCK::type_nest) && pR->check_rock_rev(pR2) >= 0.0f) {
					pR->exceptGroup |= pR2->group;
					return 0;
				}
			}
		}
		return -1;
	}

	return 0;
}


void ROCKBASE::delRock(ROCK* pR)
{
	for(int i = 0; i < MAX_ROCK_WIRE; i ++) {
		if(pR->wirenum[i] >= 0) {
			wire[pR->wirenum[i]].length = -1.0f;
		}
	}

	pR->deleteSide();
	pR->init();
}

/*
void ROCKBASE::sync(ROCKBASE* pSrc, int cont)
{
	for(int i = 0; i < MAX_ROCK; i ++) {
		ROCK* pRs = &pSrc->rock[i];
		if(pRs->stat == ROCK::stat_active) {
		//	ROCK* pR = &rock[i];
			if(pRs->sync == 0) {
				if(pRs->pGameRock->attr & (GAME::objtype_enemy | GAME::objtype_targetenemy | GAME::objtype_normal | GAME::objtype_item)) {
					for(int j = 0; j < MAX_ROCK; j ++) {
						ROCK* pR = &rock[j];
						if(pR->stat == ROCK::stat_reserve) {
							*pR = *pRs;
							pR->calcVertexW();
							pRs->sync = 1;
							pRs->syncto = j;
							break;
						}
					}
				} else {
				//	ROCK* pR = &rock[i];
				//	*pR = *pRs;
					pRs->sync = 1;
					pRs->syncto = i;
				}
			} else if(pRs->sync == 1) {
				ROCK* pR = &rock[pRs->syncto];
				if(pR->stat == ROCK::stat_active) {
				//	if(pRs->damage > 0.0f && pRs->damage >= pR->damageLimit && pRs->damageLimit >= 0.0f) {
				//		pR->sync = 1;
				//	}

					float t = 0.02f;
					if(pR->game.attr & (GAME::objtype_player1 | GAME::objtype_helper)) {
						if(cont) {
							t = 0.0f;
						} else {
							t = 0.2f;
						}
					}

					pR->center.x += (pRs->center.x - pR->center.x) * t;
					pR->center.y += (pRs->center.y - pR->center.y) * t;
					pR->angle += (pRs->angle - pR->angle) * t;
					pR->speed.x += (pRs->speed.x - pR->speed.x) * t;
					pR->speed.y += (pRs->speed.y - pR->speed.y) * t;
					pR->rSpeed += (pRs->rSpeed - pR->rSpeed) * t;

				//	pR->calcVertexW();

				//	pR->center = pRs->center;
				//	pR->angle = pRs->angle;
				//	pR->speed = pRs->speed;
				//	pR->rSpeed = pRs->rSpeed;
				} else {
					pRs->sync = 2;
				}
			}
		}
	}
}
*/

//
// rock
//

// 初期化
void ROCK::init(void)
{
	stat = stat_reserve;

	nTotalVertex = 0;

	startAreaVertex = -1;
	startVertex = -1;
	startVertexW = -1;
}

void ROCK::deleteSide(void)
{
	bool d = true;

	// area
	ROCK* pR = &base->rock[0];
	while(pR != &base->rock[MAX_ROCK]) {
		if(pR->stat != ROCK::stat_reserve && pR != this) {
			if(nArea == pR->nArea) {
				if(memcmp(&base->areaVertex[startAreaVertex], &base->areaVertex[pR->startAreaVertex], nArea) == 0) {
					d = false;
					break;
				}
			}
		}
		pR ++;
	}
	if(d) {
		int s = base->nAreaVertex - (startAreaVertex + nArea + 1);
		if(s > 0) {
			memmove(&base->areaVertex[startAreaVertex], &base->areaVertex[startAreaVertex + nArea + 1], s);

			ROCK* pR = &base->rock[0];
			while(pR != &base->rock[MAX_ROCK]) {
				if(pR->startAreaVertex > startAreaVertex) {
					pR->startAreaVertex -= (nArea + 1);
				//	DebugMessage(0, "slide %d -= %d : %d %d : %d\n", i, nTotalVertex, pR->pGameRock->attr, pGameRock->attr, base->nSide);
				}
				pR ++;
			}
		}
		base->nAreaVertex -= (nArea + 1);
	}

	d = true;
	// vertex
	pR = &base->rock[0];
	while(pR != &base->rock[MAX_ROCK]) {
		if(pR->stat != ROCK::stat_reserve && pR != this) {
			if(nTotalVertex == pR->nTotalVertex) {
				if(memcmp(&base->vertex[startVertex], &base->vertex[pR->startVertex], sizeof(FVECTOR2) * nTotalVertex) == 0) {
					d = false;
					break;
				}
			}
		}
		pR ++;
	}
	if(d) {
		int s = base->nVertex - (startVertex + nTotalVertex);
		if(s > 0) {
			memmove(&base->side[startVertex], &base->side[startVertex + nTotalVertex], sizeof(FSEQ) * s);
			memmove(&base->vertex[startVertex], &base->vertex[startVertex + nTotalVertex], sizeof(FVECTOR2) * s);

			ROCK* pR = &base->rock[0];
			while(pR != &base->rock[MAX_ROCK]) {
				if(pR->startVertex > startVertex) {
					pR->startVertex -= nTotalVertex;
				//	DebugMessage(0, "slide %d -= %d : %d %d : %d\n", i, nTotalVertex, pR->pGameRock->attr, pGameRock->attr, base->nSide);
				}
				pR ++;
			}
		}
		base->nVertex -= nTotalVertex;
	}

	// vertexW
	int s = base->nVertexW - (startVertexW + nTotalVertex);
	if(s > 0) {
		memmove(&base->vertexW[startVertexW], &base->vertexW[startVertexW + nTotalVertex], sizeof(FVECTOR2) * s);

		ROCK* pR = &base->rock[0];
		while(pR != &base->rock[MAX_ROCK]) {
			if(pR->startVertexW > startVertexW) {
				pR->startVertexW -= nTotalVertex;
			}
			pR ++;
		}
	}
	base->nVertexW -= nTotalVertex;
/*
	if(d) {
		DebugMessage(0, "del(org) nVertex %d, nVertexW %d\n", base->nVertex, base->nVertexW);
	} else {
		DebugMessage(0, "del(copy) nVertex %d, nVertexW %d\n", base->nVertex, base->nVertexW);
	}
*/
}

// 切片情報を登録
void ROCK::setSide(float margin)
{
	radius = 0.0f;

	int i = 0, nv = 0, nvo;

	while(1) {
		nvo = nv;
		nv = base->areaVertex[startAreaVertex + i];
		if(nv == 0) {
			break;
		}

		FVECTOR2*	v = &base->vertex[startVertex + nv];
		FSEQ*		s = &base->side[startVertex + nv];
		float		x2 = base->vertex[startVertex + nvo].x;
		float		y2 = base->vertex[startVertex + nvo].y;

		do {
			v --;
			float x1 = x2;
			float y1 = y2;
			x2 = v->x;
			y2 = v->y;

			float x0 = x1 - x2;
			float y0 = y1 - y2;
			float l0 = sqrtf(x0 * x0 + y0 * y0);

			float ra = x2 * x2 + y2 * y2 + 0.0f;
			if(ra > radius) {
				radius = ra;
			}

			float a = y1 - y2;
			float b = x2 - x1;
			float l = sqrtf(a * a + b * b);
			a /= l;
			b /= l;
			float c = -(a * (x1 + x2) + b * (y1 + y2)) / 2.0f;

		//	DebugMessage(0, "setside %f %f : %f %f\n", x1, y1, x2, y2);

			if(s == &base->side[startVertex + nv]) {
				(s - nv)->l2 = l;
			} else {
				s->l2 = l;
			}
			s --;
			if(type & type_inner) {
				s->a = -a;
				s->b = -b;
				s->c = -c;
			} else {
				s->a = a;
				s->b = b;
				s->c = c;
			}
			s->l = l;
		} while(v != &base->vertex[startVertex + nvo]);
		i ++;
	}

	if(type & type_inner) {
		radius = radiusSqrt = FLT_MAX;
	} else {
		radiusSqrt = sqrtf(radius) + margin;
		radius = radiusSqrt* radiusSqrt;
	}

	marginOrg = margin;
}

// 頂点座標を更新
void ROCK::calcVertexW(void)
{
	float c = cost = cosf(angle);
	float s = sint = sinf(angle);

	minPos.x = minPos.y = FLT_MAX;
	maxPos.x = maxPos.y = -FLT_MAX;

	for(int i = 0; i < nTotalVertex; i ++) {
		float x1 = base->vertex[startVertex + i].x;
		float y1 = base->vertex[startVertex + i].y;

		float x = (x1 * c - y1 * s) + center.x;
		float y = (x1 * s + y1 * c) + center.y;

		if(x < minPos.x) {
			minPos.x = x;
		}
		if(x > maxPos.x) {
			maxPos.x = x;
		}
		if(y < minPos.y) {
			minPos.y = y;
		}
		if(y > maxPos.y) {
			maxPos.y = y;
		}

		base->vertexW[startVertexW + i].x = x;
		base->vertexW[startVertexW + i].y = y;
	}

	if(type & type_inner) {
		minPos.x = minPos.y = -FLT_MAX;
		maxPos.x = maxPos.y = FLT_MAX;
	} else {
		minPos.x -= marginOrg;
		minPos.y -= marginOrg;
		maxPos.x += marginOrg;
		maxPos.y += marginOrg;
	}
}

const float margin_vertex = 3.0f;

// 岩に含まれる点と、他の岩との判定（衝突挙動付き）
float ROCK::collision_rock(ROCK* pR2)
{
	float r = -FLT_MAX;

	FVECTOR2* pVtx = &base->vertexW[startVertexW];
	FSEQ* pSeq = &base->side[startVertex];
	char* pNv = &base->areaVertex[startAreaVertex];

	FVECTOR2* mVtx;
	FSEQ* mSeq;

	do {
		mVtx = &base->vertexW[startVertexW + *pNv - 1];
		mSeq = &base->side[startVertex + *pNv - 1];
		for(; pVtx != &base->vertexW[startVertexW + *pNv]; pVtx ++, pSeq ++) {
			FVECTOR2* lVtx = pVtx;
			float xw = pVtx->x;
			float yw = pVtx->y;

		//	float l1 = sqrtf(xwp * xwp + ywp * ywp);
		//	float l2 = sqrtf(xwp2 * xwp2 + ywp2 * ywp2);
		//	float l1 = base->side[startVertex + im].l;
		//	float l2 = base->side[startVertex + i].l;

			int nvv = 0;
			if(xw >= pR2->minPos.x && xw <= pR2->maxPos.x && yw >= pR2->minPos.y && yw <= pR2->maxPos.y) {
				float d = pR2->collision_point_n(xw, yw, this, nvv, lVtx);
				if(d > r) {
					r = d;
				}
			//	if(d > -100.0f) {
					c2 ++;
			//	}
			}
			c1 ++;

			if(r > 0.0f) {
				lVtx = NULL;
			}

			if(nvv >= 2) {
				float l1 = mSeq->l;
				float l2 = pSeq->l;
				c3 ++;

				FVECTOR2* tVtx = pVtx + 1;
				if(tVtx == &base->vertexW[startVertexW + *pNv]) {
					tVtx = tVtx - *pNv;
				}

				float xw2 = xw;
				float yw2 = yw;

				float xwp = (mVtx->x - xw);
				float ywp = (mVtx->y - yw);
				xwp /= (l1 * 2.0f);
				ywp /= (l1 * 2.0f);

				float xwp2 = (tVtx->x - xw2);
				float ywp2 = (tVtx->y - yw2);
				xwp2 /= (l2 * 2.0f);
				ywp2 /= (l2 * 2.0f);

				l1 /= 4.0f;
				if(l1 > margin_vertex) {
					l1 = margin_vertex;
				}
				float ls1 = 0.5f;
				while(ls1 < l1) {
					xw += xwp;
					yw += ywp;
					float d2 = pR2->collision_point_n(xw, yw, this, nvv, lVtx);
					if(d2 > r) {
						r = d2;
					}
					xwp += xwp;
					ywp += ywp;
					ls1 += ls1;
				}

				l2 /= 4.0f;
				if(l2 > margin_vertex) {
					l2 = margin_vertex;
				}
				float ls2 = 0.5f;
				while(ls2 < l2) {
					xw2 += xwp2;
					yw2 += ywp2;
					float d2 = pR2->collision_point_n(xw2, yw2, this, nvv, lVtx);
					if(d2 > r) {
						r = d2;
					}
					xwp2 += xwp2;
					ywp2 += ywp2;
					ls2 += ls2;
				}
			}
		//	i ++;
			mVtx = pVtx;
			mSeq = pSeq;
		}
		pNv ++;
	} while(*pNv != 0);

	return r;
}

// 岩に含まれる点と、他の岩との判定（判定のみ）
float ROCK::check_rock(ROCK* pR2)
{
	float r = -FLT_MAX;

	FVECTOR2* pVtx = &base->vertexW[startVertexW];
	char* pNv = &base->areaVertex[startAreaVertex];

	FVECTOR2* mVtx;

	do {
		mVtx = &base->vertexW[startVertexW + *pNv - 1];
		for(; pVtx != &base->vertexW[startVertexW + *pNv]; pVtx ++) {
			float xw = pVtx->x;
			float yw = pVtx->y;

			{
				int nvv;
				float xwp = (mVtx->x - xw);
				float ywp = (mVtx->y - yw);
				xwp /= 4.0f;
				ywp /= 4.0f;

				int i = 4;
				do {
					float d = pR2->collision_point_n(xw, yw, NULL, nvv);
					if(d > r) {
						if(d > 0.0f) {
							return d;
						}
						r = d;
					}
					xw += xwp;
					yw += ywp;
					i --;
				} while(i > 0);
			}
			mVtx = pVtx;
		}
		pNv ++;
	} while(*pNv != 0);

	return r;
}

// 岩に含まれる点と、他の岩との判定（判定のみ）
float ROCK::check_rock_rev(ROCK* pR2)
{
	int dummy;
	float r = FLT_MAX;

	for(int i = 0; i < nTotalVertex; i ++) {
		float xw = base->vertexW[startVertexW + i].x;
		float yw = base->vertexW[startVertexW + i].y;
		float d = pR2->collision_point_n(xw, yw, NULL, dummy);
		if(d < r) {
			r = d;
		}
	}

	return r;
}

// 岩に含まれる辺と、任意の点との判定
float ROCK::collision_point_n(float xw, float yw, ROCK* pR, int &nvv, FVECTOR2* pVertex)
{
//	if(xw < minPos.x || xw > maxPos.x || yw < minPos.y || yw > maxPos.y) {
//		return -100;
//	}

	float xd = xw - center.x;
	float yd = yw - center.y;

//	if(xd * xd + yd * yd > radius) {
//		return - 100.0f;
//	}

	float c = cost;
	float s = sint;

	float x = xd * c + yd * s;
	float y = -xd * s + yd * c;

	FSEQ* tSeq;
	float md = FLT_MAX;

	if(type & type_inner) {
		md = -FLT_MAX;
	//	float mdr = -FLT_MAX;
		for(FSEQ* pSeq = &base->side[startVertex]; pSeq != &base->side[startVertex + nTotalVertex]; pSeq ++) {
			float d = pSeq->a * x + pSeq->b * y + pSeq->c;
		//	if(d > 0.0f) {
				if(d > md) {
					md = d;
					tSeq = pSeq;
				}
		//	} else {
		//		if(d > mdr) {
		//			mdr = d;
		//		}
		//	}
		}
		if(md <= 0.0f) {
			if(yw < 0.0f && pR && (pR->type & type_ceiling)) {
				pR->force.y -= yw;
				return -yw;
			}
			return md;
		}
		if(md < -yw && pR && (pR->type & type_ceiling)) {
			pR->force.y -= yw;
			return -yw;
		}
	} else {
	//	float nx2 = nx * c + ny * s;
	//	float ny2 = -nx * s + ny * c;

		FSEQ* pSeq = &base->side[startVertex];
		FSEQ* tSeq2;
		float mdr = FLT_MAX;

		char* pNv = &base->areaVertex[startAreaVertex];
		do {
			float md2 = FLT_MAX;
			for(; pSeq != &base->side[startVertex + *pNv]; pSeq ++) {
				float d = pSeq->a * x + pSeq->b * y + pSeq->c;
				if(d < md2) {
				//	if(d <= 0.0f || (pSeq->a * nx2 + pSeq->b * ny2) < 0.0f) {
						md2 = d;
						tSeq2 = pSeq;
				//	}
				}
				if(d > -3.0f) {
					nvv ++;
				}
			}
			if(nvv < 2) {
				nvv = 0;
			}
			if(md2 > 0.0f) {
				if(md2 < md) {
					md = md2;
					tSeq = tSeq2;
				}
			} else {
				if(md2 < mdr) {
					mdr = md2;
				}
			}
			pNv ++;
		} while(*pNv != 0);

		if(md == FLT_MAX) {
			return mdr;
		}
	}

	if(tSeq) {
		if(pR) {
			FVECTOR2 v, v0;
			v0.x = tSeq->a;
			v0.y = tSeq->b;

			v.x = v0.x * c - v0.y * s;
			v.y = v0.x * s + v0.y * c;

			COLLISIONLOG* pCL = NULL;

#ifdef ENABLE_COLLISIONLOG
			if(pVertex && ((group & pR->recordGroup) || (pR->group & recordGroup)) && base->nCollisionLog <= base->maxCollisionLog) {
				unsigned int rocknum;
				if(num < pR->num) {
					rocknum = (num << 16) | pR->num;
				} else {
					rocknum = (pR->num << 16) | num;
				}

				COLLISIONLOG* pCLt = &base->pCollisionLog[0];
				for(pCLt = &base->pCollisionLog[0]; pCLt != &base->pCollisionLog[base->nCollisionLog]; pCLt ++) {
					if(pCLt->rockNum == rocknum && pCLt->vertexID == pVertex) {
						if(md <= pCLt->depth) {
							// より深い接触があるため見送り
							goto tl;
						}
						// 上書き
						break;
					}
				}
				if(pCLt == &base->pCollisionLog[base->nCollisionLog]) {
					if(base->nCollisionLog == base->maxCollisionLog) {
						// オーバーフロー
						base->overflowCollisionLog ++;
						goto tl;
					} else {
						// 追加
						base->nCollisionLog ++;
					}
				}

				pCL = pCLt;
				pCL->rockNum = rocknum;
				pCL->vertexID = pVertex;

				pCL->position.x = xw;
				pCL->position.y = yw;
				if(num < pR->num) {
					pCL->normal = v;
				} else {
					pCL->normal.x = -v.x;
					pCL->normal.y = -v.y;
				}
				pCL->depth = md;
			}
tl:;
#endif
			float sxr = -(yw - pR->center.y) * pR->rSpeed;
			float syr = (xw - pR->center.x) * pR->rSpeed;
			float sxr2 = -(yw - center.y) * rSpeed;
			float syr2 = (xw - center.x) * rSpeed;

			float sx = sxr + pR->speed.x - speed.x - sxr2;
			float sy = syr + pR->speed.y - speed.y - syr2;

			float mdl;
			if((type & type_bind) == type_bind) {
				if(md < 1.2f) {
					mdl = md * md;
				} else {
					mdl = 1.44f + (md - 1.2f) * 0.01f;
				}
			} else {
				if(md < 1.0f) {
					mdl = md * md;
				} else {
					mdl = 1.0f + (md - 1.0f) * 0.01f;
				}
			}

			float x2 = sx * v.y - sy * v.x;
			float y2 = sx * v.x + sy * v.y;

			if(pCL) {
				pCL->slide = x2;
				pCL->bound = y2;
			}

			if(!(group & pR->exceptGroup) && !(pR->group & exceptGroup)) {
				float ff = friction * pR->friction;
				float bb = damper * pR->damper;

				x2 *= ff;
				if(y2 > 0.0f) {
					y2 *= bb * 1.9f;
				} else {
					y2 *= bb * 0.9f;
				}

				float wn = weight / (weight + pR->weight) * pR->weight;
				float x3 = (x2 * v.y + y2 * v.x) * wn;
				float y3 = (-x2 * v.x + y2 * v.y) * wn;

				if(v.y < 0) {
					if(!(pR->type & ROCK::type_explode)) {
						ground |= 1;
					}
				} else if(v.y > 0){
					if(!(type & ROCK::type_explode)) {
						pR->ground |= 1;
					}
				}
				ground |= 2;
				pR->ground |= 2;

				v.x += x3;
				v.y += y3;

				v.x *= mdl;
				v.y *= mdl;

				pR->force.x -= v.x;
				pR->force.y -= v.y;

				force.x += v.x;
				force.y += v.y;

				if(pVertex) {
					float t = (xw - pR->center.x) * v.y - (yw - pR->center.y) * v.x;
					pR->torque -= t;
					pR->torqueV -= t;
					t = (xw - center.x) * v.y - (yw - center.y) * v.x;
					torque += t;
					torqueV += t;
				} else {
					pR->torque -= (xw - pR->center.x) * v.y - (yw - pR->center.y) * v.x;
					torque += (xw - center.x) * v.y - (yw - center.y) * v.x;
				}
			}
		}
	}

	return md;
}

// 岩に含まれる辺と、粒子との判定
float ROCK::collision_fluid(float xw, float yw, FVECTOR2* pFr)
{
	float xd = xw - center.x;
	float yd = yw - center.y;

	if(xd * xd + yd * yd > radius) {
		return - 100.0f;
	}

	float c = cost;
	float s = sint;

	float x = xd * c + yd * s;
	float y = -xd * s + yd * c;

	FSEQ *tSeq = (FSEQ*)0x01, *tSeqr = (FSEQ*)0x01;	// debug
	float md = FLT_MAX;

	if(type & type_inner) {
		float mdr = -FLT_MAX;
		for(FSEQ* pSeq = &base->side[startVertex]; pSeq != &base->side[startVertex + nTotalVertex]; pSeq ++) {
			float d = pSeq->a * x + pSeq->b * y + pSeq->c;
			if(d > 0.0f) {
				if(d < md) {
					md = d;
					tSeq = pSeq;
				}
			} else {
				if(d > mdr) {
					mdr = d;
				}
			}
		}
		if(md == FLT_MAX) {
			return mdr;
		}
	} else {
		float md2;
		int debugCount1, debugCount2[16];	// debug
		FSEQ* pSeq = &base->side[startVertex];
		FSEQ* tSeq2 = (FSEQ*)0x01;	//debug
		float mdr = FLT_MAX;

		debugCount1 = 0;
		char* pNv = &base->areaVertex[startAreaVertex];
		do {
			md2 = FLT_MAX;
			debugCount2[debugCount1] = 0;
			for(; pSeq != &base->side[startVertex + *pNv]; pSeq ++) {
				float d = pSeq->a * x + pSeq->b * y + pSeq->c;
				if(d < md2) {
					md2 = d;
					tSeq2 = pSeq;
				}
				debugCount2[debugCount1] ++;
			}
			if(md2 > 0.0f) {
				if(md2 < md) {
					md = md2;
					tSeq = tSeq2;
				}
			} else {
				if(md2 < mdr) {
					mdr = md2;
					tSeqr = tSeq2;
				}
			}
			pNv ++;
			debugCount1 ++;
		} while(*pNv != 0);

		if(md == FLT_MAX) {
			{
				FVECTOR2 v, v0;
				v0.x = tSeqr->a;
				v0.y = tSeqr->b;

				v.x = v0.x * c - v0.y * s;
				v.y = v0.x * s + v0.y * c;

				pFr->x = v.x;
				pFr->y = v.y;
			}
			return mdr;
		}
	}

	if(tSeq) {
		FVECTOR2 v, v0;
		v0.x = tSeq->a;
		v0.y = tSeq->b;

		v.x = v0.x * c - v0.y * s;
		v.y = v0.x * s + v0.y * c;

		pFr->x = v.x;
		pFr->y = v.y;
	}

	return md;
}

// 岩に含まれる辺と、点との距離を算出
float ROCK::collision_circle(float xw, float yw)
{
	float xd = xw - center.x;
	float yd = yw - center.y;

	float c = cost;
	float s = sint;

	float x = xd * c + yd * s;
	float y = -xd * s + yd * c;

	FSEQ* tSeq;
	float md = FLT_MAX;

	if(type & type_inner) {
		float mdr = -FLT_MAX;
		for(FSEQ* pSeq = &base->side[startVertex]; pSeq != &base->side[startVertex + nTotalVertex]; pSeq ++) {
			float d = pSeq->a * x + pSeq->b * y + pSeq->c;
			if(d > 0.0f) {
				if(d < md) {
					md = d;
					tSeq = pSeq;
				}
			} else {
				if(d > mdr) {
					mdr = d;
				}
			}
		}
		if(md == FLT_MAX) {
			return mdr;
		}
	} else {
		FSEQ* pSeq = &base->side[startVertex];
		FSEQ* tSeq2;
		float mdr = FLT_MAX;

		char* pNv = &base->areaVertex[startAreaVertex];
		do {
			float md2 = FLT_MAX;
			for(; pSeq != &base->side[startVertex + *pNv]; pSeq ++) {
				float d = pSeq->a * x + pSeq->b * y + pSeq->c;
				if(d < md2) {
					md2 = d;
					tSeq2 = pSeq;
				}
			}
			if(md2 > 0.0f) {
				if(md2 < md) {
					md = md2;
					tSeq = tSeq2;
				}
			} else {
				if(md2 < mdr) {
					mdr = md2;
				}
			}
			pNv ++;
		} while(*pNv != 0);

		if(md == FLT_MAX) {
			return mdr;
		}
	}

	return md;
}

// 任意の場所に力を加える
void ROCK::push(float xw, float yw, float ax, float ay)
{
	if((type & type_float) && ay > 0.0f) {
		ay /= 2.0f;
	}

	force.x += ax;
	force.y += ay;
	torque += (xw - center.x) * ay - (yw - center.y) * ax;
}

// 速度変化付のpush
void ROCK::pushMove(float xw, float yw, float ax, float ay)
{
	force.x = force.y = 0.0f;
	torque = 0.0f;

	push(xw, yw, ax, ay);	

	speed.x += force.x / weight;
	speed.y += force.y / weight;
	rSpeed += torque / moment;
}

// (xw, yw)を中心として、距離に反比例する力を加える。
// powerは距離1.0fの際に加わる力の大きさ。
// nearClipより近い位置は、反比例せずnearClipと同じ力の大きさとなる。
// farClipより遠くにある場合は力は加わらない。
void ROCK::blast(float xw, float yw, float power, float nearClip, float farClip)
{
	if(farClip < FLT_MAX) {
		float xc = xw - center.x;
		float yc = yw - center.y;
		float lc = farClip + radiusSqrt;
		if(xc * xc + yc * yc > lc * lc) {
			return;
		}
	}

	FVECTOR2* pV = &base->vertexW[startVertexW];
	FSEQ* pS = &base->side[startVertex];
	char* pNv = &base->areaVertex[startAreaVertex];

	float nearClip2 = nearClip * nearClip;
	float farClip2 = farClip * farClip;

	do {
		float x2 = base->vertexW[startVertexW + *pNv - 1].x;
		float y2 = base->vertexW[startVertexW + *pNv - 1].y;
		float p = 0.0f;

		FVECTOR2* pVe = &base->vertexW[startVertexW + *pNv];
		for(; pV != pVe; pV ++) {
			float td = pS->l2;
			float x1 = x2;
			float y1 = y2;
			x2 = pV->x;
			y2 = pV->y;

			for(; p <= td; p += 4.0f) {
				float xc = (x2 * p + x1 * (td - p)) / td;
				float yc = (y2 * p + y1 * (td - p)) / td;
				float xx = xc - xw;
				float yy = yc - yw;
				float l = xx * xx + yy * yy, l2;
				if(l <= farClip2) {
					if(l < nearClip2) {
						l2 = power / sqrtf(l) / nearClip;
					} else {
						l2 = power / l;
					}
					push(xc, yc, xx * l2, yy * l2);
				}
			}
			p -= td;
			pS ++;
		}
		pNv ++;
	} while(*pNv != 0);
}

// 速度変化付のblast
void ROCK::blastMove(float xw, float yw, float power, float nearClip, float farClip)
{
	force.x = force.y = 0.0f;
	torque = 0.0f;

	blast(xw, yw, power, nearClip, farClip);

	speed.x += force.x / weight;
	speed.y += force.y / weight;
	rSpeed += torque / moment;
}


