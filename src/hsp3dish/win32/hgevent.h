
//
//	hgevent.cpp structures
//
#ifndef __hgevent_h
#define __hgevent_h

#include "geometry.h"

#define HGEVENT_SOURCE_VEC -1
#define HGEVENT_SOURCE_WORK MOC_WORK
#define HGEVENT_MOCOPT_SRCWORK 16
#define HGEVENT_MOCOPT_TARGET_BIT 15

enum {
HGEVENT_FLAG_NONE = 0,
HGEVENT_FLAG_WAIT,
HGEVENT_FLAG_JUMP,
HGEVENT_FLAG_PRMSET,
HGEVENT_FLAG_PRMON,
HGEVENT_FLAG_PRMOFF,
HGEVENT_FLAG_UVANIM,
HGEVENT_FLAG_MOVETARGET,
HGEVENT_FLAG_MOVETARGET2,
HGEVENT_FLAG_MOVETARGET3,
HGEVENT_FLAG_MOVETARGET4,
HGEVENT_FLAG_PLUSTARGET,
HGEVENT_FLAG_SETTARGET,
HGEVENT_FLAG_SUICIDE,
HGEVENT_FLAG_REGOBJ,
HGEVENT_FLAG_SETEPRIM,
HGEVENT_FLAG_AIM,
HGEVENT_FLAG_OBJACT,
HGEVENT_FLAG_MAX,
};

enum {
HGEVENT_PRMSET_MODE = 0,
HGEVENT_PRMSET_FLAG,
HGEVENT_PRMSET_SHADE,
HGEVENT_PRMSET_TIMER,
HGEVENT_PRMSET_MYGROUP,
HGEVENT_PRMSET_COLGROUP,
HGEVENT_PRMSET_MAX,
};


//
//	hgevent.cpp class
//
class hgevent {
public:
	hgevent();
	~hgevent();
	void Reset( void );
	void SetNextEvent( hgevent *next ) { nextevent = next; }
	hgevent *GetNextEvent( void ) { return nextevent; }
	VECTOR *GetEventVector( void ) { return &vec; }
	void SetEventVector( float x, float y, float z ) { vec.x=x;vec.y=y;vec.z=z;vec.w=1.0f; }
	VECTOR *GetEventVector2( void ) { return &vec2; }
	void SetEventVector2( float x, float y, float z ) { vec2.x=x;vec2.y=y;vec2.z=z;vec2.w=1.0f; }

	//	Parameters
	//
	int		id;
	int		flag;
	float	timer;
	short	target;
	short	source;
	VECTOR	vec;
	VECTOR	vec2;
	VECTOR	*nextvec;
	int		value;
	hgevent *nextevent;
private:

};


#endif
