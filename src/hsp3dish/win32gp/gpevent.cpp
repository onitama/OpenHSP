
#include <stdio.h>
#include <string.h>
#include "gamehsp.h"

#include "../../hsp3/hsp3config.h"
#include "../supio.h"
#include "../sysreq.h"

/*------------------------------------------------------------*/
/*
		initalizer
*/
/*------------------------------------------------------------*/

/*------------------------------------------------------------*/
/*
		gpevent interface
*/
/*------------------------------------------------------------*/

gpevent::gpevent( void )
{
	_id = -1;
	_flag = GPEVENT_FLAG_NONE;
}


gpevent::~gpevent( void )
{
}

/*------------------------------------------------------------*/
/*
		interface
*/
/*------------------------------------------------------------*/

void gpevent::Reset( int flag )
{
	_flag = flag;
	_timer = 0.0f;
	_target = 0;
	_source = GPEVENT_SOURCE_VEC;
	_value = 0;
	_root = 0;
	nextvec = NULL;
	SetEventVector( 0.0f, 0.0f, 0.0f );
	SetEventVector2( 0.0f, 0.0f, 0.0f );
	SetNextEvent( NULL );
}

/*------------------------------------------------------------*/
/*
	Event process
*/
/*------------------------------------------------------------*/

gpevent *gamehsp::getEvent(int id)
{
	int base_id;
	base_id = id;
	if ((base_id < 0) || (base_id >= _maxevent)) return NULL;
	if (_gpevent[base_id]._flag == GPEVENT_FLAG_NONE) return NULL;
	return &_gpevent[base_id];
}


int gamehsp::deleteEvent(int id)
{
	gpevent *ev = getEvent(id);
	if (ev == NULL) return -1;

	while (1) {
		if (ev == NULL) break;
		ev->_flag = GPEVENT_FLAG_NONE;
		ev = ev->GetNextEvent();
	}
	return 0;
}


gpevent *gamehsp::addNewEvent(void)
{
	int i;
	gpevent *ev = _gpevent;
	for (i = 0; i<_maxevent; i++) {
		if (ev->_flag == GPEVENT_FLAG_NONE) {
			ev->_id = i;
			ev->_root = 0;
			return ev;
		}
		ev++;
	}
	return NULL;
}

gpevent *gamehsp::addEvent(int id)
{
	gpevent *ev_next;
	gpevent *ev = getEvent(id);
	gpevent *newev = addNewEvent();
	newev->_root = id;
	if (ev == NULL) {
		return newev;
	}
	while (1) {								// 既存のイペントに追加する
		ev_next = ev->GetNextEvent();
		if (ev_next == NULL) break;
		ev = ev_next;
	}
	ev->SetNextEvent(newev);
	return newev;
}


int gamehsp::GetEmptyEventId(void)
{
	gpevent *ev = addNewEvent();
	if (ev == NULL) return -1;
	return ev->_id;
}


float gamehsp::GetTimerFromFrame(int frame)
{
	return 1.0f / ((float)frame);
}


int gamehsp::AttachEvent(int objid, int eventid, int entry)
{
	gpevent *ev;
	gpobj *obj;
	int i;

	obj = getObj(objid);
	if (obj == NULL) return -1;

	if (eventid < 0) {
		obj->DeleteEvent(entry);
		return entry;
	}

	ev = getEvent(eventid);
	if (ev == NULL) return -1;

	i = entry;
	if (i < 0) {
		i = obj->GetEmptyEvent();
		if (i < 0) return -1;
	}
	obj->StartEvent(ev, entry);
	return i;
}

int gamehsp::AddWaitEvent(int eventid, int frame)
{
	gpevent *ev;
	ev = addEvent(eventid);
	if (ev == NULL) return -1;
	ev->Reset(GPEVENT_FLAG_WAIT);
	if (frame >= 0) {
		ev->_timer = GetTimerFromFrame(frame);
	}
	else {
		ev->_source = GPEVENT_SOURCE_WORK;
	}
	return ev->_id;
}

int gamehsp::AddJumpEvent(int eventid, int gonum, int rate)
{
	gpevent *ev;
	ev = addEvent(eventid);
	if (ev == NULL) return -1;
	ev->Reset(GPEVENT_FLAG_JUMP);
	//Alertf("##%d #ID%d jump%d", eventid, ev->_id, ev->_root);
	ev->_target = gonum;
	ev->_value = rate;
	ev->_root = eventid;
	return ev->_id;
}

int gamehsp::AddUVEvent(int eventid, int ysize, int count)
{
	gpevent *ev;
	ev = addEvent(eventid);
	if (ev == NULL) return -1;
	ev->Reset(GPEVENT_FLAG_UVANIM);
	ev->_target = ysize;
	ev->_value = count;
	return ev->_id;
}

int gamehsp::AddParamEvent(int eventid, int mode, int target, int param)
{
	gpevent *ev;
	ev = addEvent(eventid);
	if (ev == NULL) return -1;
	ev->Reset(GPEVENT_FLAG_PRMSET + mode);
	ev->_target = target;
	ev->_value = param;
	return ev->_id;
}

int gamehsp::AddParamEvent2(int eventid, int target, int param, int low, int high)
{
	gpevent *ev;
	ev = addEvent(eventid);
	if (ev == NULL) return -1;
	ev->Reset(GPEVENT_FLAG_PRMADD);
	ev->_target = target;
	ev->_value = param;
	ev->_value_low = low;
	ev->_value_high = high;
	return ev->_id;
}

int gamehsp::AddMoveEvent(int eventid, int target, float x, float y, float z, int frame, int sw)
{
	gpevent *ev;
	ev = addEvent(eventid);
	if (ev == NULL) return -1;
	if (sw == 0) {
		ev->Reset(GPEVENT_FLAG_MOVETARGET);
	}
	else {
		ev->Reset(GPEVENT_FLAG_MOVETARGET3);
	}
	ev->_target = target & GPEVENT_MOCOPT_TARGET_BIT;
	if (target & GPEVENT_MOCOPT_SRCWORK) ev->_source = GPEVENT_SOURCE_WORK;
	ev->SetEventVector(x, y, z);
	ev->_timer = GetTimerFromFrame(frame);
	return ev->_id;
}

int gamehsp::AddSplineMoveEvent(int eventid, int target, float x, float y, float z, int frame, int sw)
{
	gpevent *ev;
	ev = addEvent(eventid);
	if (ev == NULL) return -1;
	if (sw == 0) {
		ev->Reset(GPEVENT_FLAG_MOVETARGET2);
	}
	else {
		ev->Reset(GPEVENT_FLAG_MOVETARGET4);
	}
	ev->_target = target & GPEVENT_MOCOPT_TARGET_BIT;
	if (target & GPEVENT_MOCOPT_SRCWORK) {
		ev->_source = GPEVENT_SOURCE_WORK;
	}
	ev->SetEventVector(x, y, z);
	ev->_timer = GetTimerFromFrame(frame);
	return ev->_id;
}

int gamehsp::AddPlusEvent(int eventid, int target, float x, float y, float z)
{
	gpevent *ev;
	ev = addEvent(eventid);
	if (ev == NULL) return -1;
	ev->Reset(GPEVENT_FLAG_PLUSTARGET);
	ev->_target = target & GPEVENT_MOCOPT_TARGET_BIT;
	if (target & GPEVENT_MOCOPT_SRCWORK) ev->_source = GPEVENT_SOURCE_WORK;
	ev->SetEventVector(x, y, z);
	return ev->_id;
}

int gamehsp::AddChangeEvent(int eventid, int target, float x1, float y1, float z1, float x2, float y2, float z2)
{
	float f;
	gpevent *ev;
	f = 1.0f / 1024.0f;
	ev = addEvent(eventid);
	if (ev == NULL) return -1;
	ev->Reset(GPEVENT_FLAG_SETTARGET);
	ev->_target = target & GPEVENT_MOCOPT_TARGET_BIT;
	ev->SetEventVector(x1, y1, z1);
	ev->SetEventVector2((x2 - x1)*f, (y2 - y1)*f, (z2 - z1)*f);
	return ev->_id;
}

int gamehsp::AddSuicideEvent(int eventid, int mode)
{
	gpevent *ev;
	ev = addEvent(eventid);
	if (ev == NULL) return -1;
	ev->Reset(GPEVENT_FLAG_SUICIDE);
	ev->_value = mode;
	return ev->_id;
}

void gamehsp::storeNextVector(gpevent *myevent)
{
	//		次のmovevectorを検索して補完する
	//
	int flag, fl;
	int target;
	gpevent *lastjp;
	gpevent *ev;
	gpevent *nextev;
	target = myevent->_target;
	lastjp = NULL;

	ev = myevent->GetNextEvent();
	while (1) {
		if (ev == NULL) break;
		if (ev == myevent) break;
		flag = ev->_flag;
		if ((flag == GPEVENT_FLAG_MOVETARGET2) || (flag == GPEVENT_FLAG_MOVETARGET4)) {			// スプライン移動か?
			if (ev->_target == target) {															// 自分と同じターゲットか?
				myevent->nextvec = ev->GetEventVector(); return;
			}
		}
		if (flag == GPEVENT_FLAG_JUMP) {
			nextev = getEvent(ev->_root);
			for (fl = 0; fl<ev->_target; fl++) { nextev = nextev->GetNextEvent(); }
			ev = nextev;
			if (lastjp == ev) break;
			lastjp = ev;
		}
		else {
			ev = ev->GetNextEvent();
		}
	}
	myevent->nextvec = myevent->GetEventVector();			// 次のデータがない場合は自分のvectorを入れる
}

void gamehsp::putEventError(gpobj *obj, gpevent *ev, char *msg)
{
	Alertf("イベント実行中のエラーです。\n(ObjectID:%d/EventID:%d)\n%s", obj->_id, ev->_id, msg);
}

void gamehsp::ExecuteObjEvent(gpobj *obj, float timepass, int entry)
{
	//	イベント実行
	gpevent *ev;
	gpevent *nextev;
	float tbase;
	float tval;
	int fl;
	gameplay::Vector4 v1;

	ev = obj->GetEvent(entry);
	tbase = obj->_time[entry];
	obj->_time[entry] += timepass;
	while (ev != NULL) {
		switch (ev->_flag) {
		case GPEVENT_FLAG_WAIT:
			if (ev->_source != GPEVENT_SOURCE_VEC) {
				if (getObjectVector(obj->_id, ev->_source, &v1)==0) {
					if (v1.x == 0.0f) tval = 1.0f; else tval = tbase / v1.x;
				}
				else {
					tval = 1.0f;
				}
			}
			else {
				tval = tbase * ev->_timer;
			}
			if (tval >= 1.0f) {
				obj->SetEvent(ev->GetNextEvent(), entry);		// 次のイベントへ進む
				return;
			}
			return;
		case GPEVENT_FLAG_JUMP:
			if (ev->_value) {
				fl = rand() % 100;
				if (fl >= ev->_value) {
					obj->SetEvent(ev->GetNextEvent(), entry);	// イベントをパスする
					break;
				}
			}
			nextev = getEvent(ev->_root);
			for (fl = 0; fl<ev->_target; fl++) { nextev = nextev->GetNextEvent(); }
			obj->SetEvent(nextev, entry);
			return;
		case GPEVENT_FLAG_UVANIM:
		{
			if (obj->_spr) {
				obj->_spr->_celid = ev->_target;
			}
			obj->SetEvent(ev->GetNextEvent(), entry);	// イベントをパスする
			break;
		}
		case GPEVENT_FLAG_SUICIDE:
			deleteObj(obj->_id);
			obj->SetEvent(NULL, entry);
			return;

		case GPEVENT_FLAG_PRMSET:
			if (setObjectPrm(obj->_id, ev->_target, ev->_value) < 0) {
				putEventError(obj, ev, "無効なパラメーターID");
			}
			obj->SetEvent(ev->GetNextEvent(), entry);	// イベントをパスする
			break;
		case GPEVENT_FLAG_PRMON:
			if (getObjectPrm(obj->_id, ev->_target, &fl) < 0) {
				putEventError(obj, ev, "無効なパラメーターID");
			}
			fl |= ev->_value;
			setObjectPrm(obj->_id, ev->_target, fl);
			obj->SetEvent(ev->GetNextEvent(), entry);	// イベントをパスする
			break;
		case GPEVENT_FLAG_PRMOFF:
			if (getObjectPrm(obj->_id, ev->_target, &fl) < 0) {
				putEventError(obj, ev, "無効なパラメーターID");
			}
			fl &= ~ev->_value;
			setObjectPrm(obj->_id, ev->_target, fl);
			obj->SetEvent(ev->GetNextEvent(), entry);	// イベントをパスする
			break;
		case GPEVENT_FLAG_PRMADD:
			if (getObjectPrm(obj->_id, ev->_target, &fl) < 0) {
				putEventError(obj, ev, "無効なパラメーターID");
			}
			fl += ev->_value;
			if (fl < ev->_value_low) fl = ev->_value_low;
			if (fl > ev->_value_high) fl = ev->_value_high;
			setObjectPrm(obj->_id, ev->_target, fl);
			obj->SetEvent(ev->GetNextEvent(), entry);	// イベントをパスする
			break;
		case GPEVENT_FLAG_PLUSTARGET:
		{
			// target+=dir:dir+=vec を実行する
			gameplay::Vector3 *pv1;
			pv1 = ev->GetEventVector();
			v1.set( pv1->x, pv1->y, pv1->z, 1.0f );
			if (addObjectVector(obj->_id, ev->_target, &v1) < 0) {
				putEventError(obj, ev, "無効なMOC設定");
			}
			break;
		}
		case GPEVENT_FLAG_TRIMTARGET:
		{
			// targetの範囲を限定する
			gameplay::Vector3 *pv1;
			gameplay::Vector3 *pv2;
			pv1 = ev->GetEventVector();
			pv2 = ev->GetEventVector2();

			if (getObjectVector(obj->_id, ev->_target, &v1) < 0) {
				putEventError(obj, ev, "無効なMOC設定");
			}
			if (v1.x < pv1->x) { v1.x = pv1->x; }
			if (v1.y < pv1->y) { v1.y = pv1->y; }
			if (v1.z < pv1->z) { v1.z = pv1->z; }
			if (v1.x > pv2->x) { v1.x = pv2->x; }
			if (v1.y > pv2->y) { v1.y = pv2->y; }
			if (v1.z > pv2->z) { v1.z = pv2->z; }
			setObjectVector(obj->_id, ev->_target, &v1);
			obj->SetEvent(ev->GetNextEvent(), entry);	// イベントをパスする
			break;
		}
		case GPEVENT_FLAG_SETTARGET:
		{
			// targetを再設定する
			float t1, t2, t3;
			gameplay::Vector3 *pv1;
			gameplay::Vector3 *pv2;
			pv1 = ev->GetEventVector();
			pv2 = ev->GetEventVector2();
			if (pv2->x == 0.0f) { v1.x = pv1->x; }
			else {
				t1 = (float)(rand() & 1023);
				v1.x = pv2->x * t1 + pv1->x;
			}
			if (pv2->y == 0.0f) { v1.y = pv1->y; }
			else {
				t2 = (float)(rand() & 1023);
				v1.y = pv2->y * t2 + pv1->y;
			}
			if (pv2->z == 0.0f) { v1.z = pv1->z; }
			else {
				t3 = (float)(rand() & 1023);
				v1.z = pv2->z * t3 + pv1->z;
			}
			if (setObjectVector(obj->_id, ev->_target, &v1) < 0) {
				putEventError(obj, ev, "無効なMOC設定");
			}
			obj->SetEvent(ev->GetNextEvent(), entry);	// イベントをパスする
			break;
		}
		case GPEVENT_FLAG_MOVETARGET:
		{
			// targetにlinear補間
			gameplay::Vector3 *pv1;
			gameplay::Vector4 *pv2;
			if (getObjectVector(obj->_id, ev->_target, &v1) < 0) {
				putEventError(obj, ev, "無効なMOC設定");
			}

			pv1 = ev->GetEventVector();
			pv2 = &(obj->_evvec[entry]);

			if (tbase == 0.0f) {
				pv2->set( v1.x, v1.y, v1.z, v1.w );
			}
			tval = tbase * ev->_timer;
			if (tval > 1.0f) tval = 1.0f;

			v1.x = (pv1->x - pv2->x) * tval + pv2->x;
			v1.y = (pv1->y - pv2->y) * tval + pv2->y;
			v1.z = (pv1->z - pv2->z) * tval + pv2->z;

			setObjectVector(obj->_id, ev->_target, &v1);
			break;
		}
		case GPEVENT_FLAG_MOVETARGET3:
		{
			// targetにlinear補間(相対)
			gameplay::Vector3 *pv1;
			gameplay::Vector4 *pv2;
			if (getObjectVector(obj->_id, ev->_target, &v1) < 0) {
				putEventError(obj, ev, "無効なMOC設定");
			}

			pv1 = ev->GetEventVector();
			pv2 = &(obj->_evvec[entry]);

			if (tbase == 0.0f) {
				pv2->set(v1.x, v1.y, v1.z, v1.w);
			}
			tval = tbase * ev->_timer;
			if (tval > 1.0f) tval = 1.0f;

			v1.x = pv1->x * tval + pv2->x;
			v1.y = pv1->y * tval + pv2->y;
			v1.z = pv1->z * tval + pv2->z;

			setObjectVector(obj->_id, ev->_target, &v1);
			break;
		}
		case GPEVENT_FLAG_MOVETARGET2:
		{
			// targetにspline補間
			Vector3	key0;
			Vector3	key1;
			Vector3	key2;
			Vector3	key3;
			Vector3	fa;
			Vector3	fb;
			Vector3	tmp;
			Vector3	*nextv;
			gameplay::Vector3 *pv1;
			gameplay::Vector4 *pv2;
			float t1, t2, t3;
			if (ev->nextvec == NULL) storeNextVector(ev);

			if (getObjectVector(obj->_id, ev->_target, &v1) < 0) {
				putEventError(obj, ev, "無効なMOC設定");
			}
			pv1 = ev->GetEventVector();
			pv2 = &(obj->_evvec[entry]);

			nextv = ev->nextvec;
			if (tbase == 0.0f) {
				pv2->set(v1.x, v1.y, v1.z, v1.w);
			}
			tval = tbase * ev->_timer;
			if (tval > 1.0f) tval = 1.0f;
			t1 = tval;
			t2 = t1 * t1;
			t3 = t2 * t1;

			//	r0 = kframe[i];
			//	r1 = kframe[i+1];
			//	r2 = kframe[i+1]-kframe[i];
			//	r3 = kframe[i+2]-kframe[i+1];
			key0.set(pv2->x, pv2->y, pv2->z);
			key1.set(pv1->x, pv1->y, pv1->z);
			key2.set(key1.x-key0.x,key1.y-key0.y,key1.z-key0.z);
			key3.set(nextv->x - key1.x, nextv->y - key1.y, nextv->z - key1.z);

			//	fa = 2.0f * r0 - 2.0f * r1 + r2 + r3;
			fa = key0 * 2.0f;
			tmp = key1 * 2.0f;
			fa -= tmp;
			fa += key2;
			fa += key3;

			//	fb = -3.0f * r0 + 3.0f * r1 - 2.0f * r2 - r3;
			fb = key0 * -3.0f;
			tmp = key1 * 3.0f;
			fb += tmp;
			tmp = key2 * 2.0f;
			fb -= tmp;
			fb -= key3;

			//	fc = r2;
			//	fd = r0;
			//	res = fa * t3 + fb * t2 + fc * t + fd;
			fa *= t3;
			fb *= t2;
			key2 *= t1;
			tmp = fa + fb;
			tmp += key2;
			tmp += key0;

			v1.set( tmp.x, tmp.y, tmp.z, 1.0f );
			setObjectVector(obj->_id, ev->_target, &v1);
			break;
		}
		case GPEVENT_FLAG_MOVETARGET4:
		{
			// targetにspline補間(相対)
			Vector3	key0;
			Vector3	key1;
			Vector3	key2;
			Vector3	key3;
			Vector3	fa;
			Vector3	fb;
			Vector3	tmp;
			Vector3	*nextv;
			gameplay::Vector3 *pv1;
			gameplay::Vector4 *pv2;
			float t1, t2, t3;
			if (ev->nextvec == NULL) storeNextVector(ev);

			if (getObjectVector(obj->_id, ev->_target, &v1) < 0) {
				putEventError(obj, ev, "無効なMOC設定");
			}
			pv1 = ev->GetEventVector();
			pv2 = &(obj->_evvec[entry]);

			nextv = ev->nextvec;
			if (tbase == 0.0f) {
				pv2->set(v1.x, v1.y, v1.z, v1.w);
			}
			tval = tbase * ev->_timer;
			if (tval > 1.0f) tval = 1.0f;
			t1 = tval;
			t2 = t1 * t1;
			t3 = t2 * t1;

			//	r0 = kframe[i];
			//	r1 = kframe[i+1];
			//	r2 = kframe[i+1]-kframe[i];
			//	r3 = kframe[i+2]-kframe[i+1];
			key0.set(pv2->x, pv2->y, pv2->z);
			key1.set(pv2->x + pv1->x, pv2->y + pv1->y, pv2->z+pv1->z);
			key2.set(pv1->x, pv1->y, pv1->z);
			key3.set(nextv->x, nextv->y, nextv->z);

			//	fa = 2.0f * r0 - 2.0f * r1 + r2 + r3;
			fa = key0 * 2.0f;
			tmp = key1 * 2.0f;
			fa -= tmp;
			fa += key2;
			fa += key3;

			//	fb = -3.0f * r0 + 3.0f * r1 - 2.0f * r2 - r3;
			fb = key0 * -3.0f;
			tmp = key1 * 3.0f;
			fb += tmp;
			tmp = key2 * 2.0f;
			fb -= tmp;
			fb -= key3;

			//	fc = r2;
			//	fd = r0;
			//	res = fa * t3 + fb * t2 + fc * t + fd;
			fa *= t3;
			fb *= t2;
			key2 *= t1;
			tmp = fa + fb;
			tmp += key2;
			tmp += key0;

			v1.set(tmp.x, tmp.y, tmp.z, 1.0f);
			setObjectVector(obj->_id, ev->_target, &v1);
			break;
		}


		default:
			putEventError(obj,ev,"無効なイベント");
			obj->SetEvent(NULL, entry);
			return;
		}
		ev = ev->GetNextEvent();
	}
	obj->SetEvent(NULL, entry);
}
