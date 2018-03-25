
//
//	hgemitter.cpp structures
//
#ifndef __hgemitter_h
#define __hgemitter_h

#include "geometry.h"

class hgdx;

#define EMITMODE_NONE		0
#define EMITMODE_STATIC		1
#define EMITMODE_CIRCLE		2
#define EMITMODE_RANDOM		3
#define EMITMODE_LOOKAT		16

class hgemitter {
public:
	hgemitter();
	~hgemitter();
	void Reset( void );
	void SetSize( float sx, float sy, float sz ) { SetVector( &size, sx, sy, sz, 0.0f ); };
	void SetSpeed( float n_speed1, float n_speed2 ) { speed = n_speed1; speedopt = n_speed2; };
	void SetAngleMul( float angx, float angy, float angz ) { SetVector( &angmul, angx, angy, angz, 0.0f ); };
	void SetAngleOpt( float angx, float angy, float angz ) { SetVector( &angopt, angx, angy, angz, 0.0f ); };
	void SetModel( int model, int modelnum, int n_mode, int efx ) { out_model = model; out_modelnum = modelnum; objmode = n_mode; out_efx = efx; };
	void SetEvent( int eventid, int num ) { out_event = eventid; out_eventnum = num; };
	void SetNum( int num, int numopt ) { out_num = num; out_numopt = numopt; };
	void SetGroup( int my, int ene ) { mygroup = my; enegroup = ene; };
	void SetOrder( int order ) { rotorder = order; };

	int Emit( hgdx *dx, VECTOR *in );

	//	Parameters
	//
	int mode, objmode;
	int out_num,out_numopt;
	int out_model, out_modelnum;
	int out_efx;
	int out_event, out_eventnum;
	float speed, speedopt;
	VECTOR angmul, angopt, size;
	int mygroup, enegroup;
	int rotorder;

private:
	void GetBasePosition( VECTOR *res, VECTOR *in );
	void GetAngle( VECTOR *res );
	void GetCircleAngle( VECTOR *res, float rot );
	void SetDirectionFromAngle( int flag, VECTOR *res, VECTOR *ang );

};


#endif
