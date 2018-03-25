
//
//	gpevent.cpp structures
//
#ifndef __gpevent_h
#define __gpevent_h

#include "gameplay.h"

using namespace gameplay;

#define GPEVENT_SOURCE_VEC -1
#define GPEVENT_SOURCE_WORK MOC_WORK
#define GPEVENT_MOCOPT_SRCWORK 16
#define GPEVENT_MOCOPT_TARGET_BIT 15

enum {
GPEVENT_FLAG_NONE = 0,
GPEVENT_FLAG_WAIT,
GPEVENT_FLAG_JUMP,
GPEVENT_FLAG_PRMSET,
GPEVENT_FLAG_PRMON,
GPEVENT_FLAG_PRMOFF,
GPEVENT_FLAG_PRMADD,
GPEVENT_FLAG_UVANIM,
GPEVENT_FLAG_MOVETARGET,
GPEVENT_FLAG_MOVETARGET2,
GPEVENT_FLAG_MOVETARGET3,
GPEVENT_FLAG_MOVETARGET4,
GPEVENT_FLAG_PLUSTARGET,
GPEVENT_FLAG_TRIMTARGET,
GPEVENT_FLAG_SETTARGET,
GPEVENT_FLAG_SUICIDE,
GPEVENT_FLAG_REGOBJ,
GPEVENT_FLAG_SETEPRIM,
GPEVENT_FLAG_AIM,
GPEVENT_FLAG_OBJACT,
GPEVENT_FLAG_MAX,
};

//
//	gpevent.cpp class
//
class gpevent {
public:
	gpevent();
	~gpevent();
	void Reset( int flag );
	void SetNextEvent( gpevent *next ) { nextevent = next; }
	gpevent *GetNextEvent( void ) { return nextevent; }
	Vector3 *GetEventVector(void) { return &vec; }
	void SetEventVector( float x, float y, float z ) { vec.set( x,y,z ); }
	Vector3 *GetEventVector2(void) { return &vec2; }
	void SetEventVector2( float x, float y, float z ) { vec2.set( x,y,z ); }

	//	Parameters
	//
	int		_id;
	int		_flag;
	float	_timer;
	int		_target;
	int		_source;
	int		_root;
	int		_value;
	int		_value_low;
	int		_value_high;
	Vector3	vec;
	Vector3	vec2;
	Vector3	*nextvec;
	gpevent *nextevent;
private:

};


#endif
