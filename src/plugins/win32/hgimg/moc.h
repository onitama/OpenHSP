#ifndef		__MOC_H__
#define		__MOC_H__

#if defined(_LANGUAGE_C_PLUS_PLUS)||defined(__cplusplus)||defined(c_plusplus)
extern "C" {
#endif

/*---------------------------------------------------------------------------*/

#define RXM_FV FVECTOR
#define RXM_IV IVECTOR
#define RXM_FM MATRIX

typedef struct KFRAME {
	unsigned short	index;		// key data index
	unsigned short	flag;		// frame option
	int		length;				// time to reach
} KFRAME;

typedef struct KFRAME2 {
	unsigned short	index;		// key data index
	unsigned short	flag;		// frame option
	float	advance;			// (1/length)
} KFRAME2;

typedef struct KFRAME3 {
	unsigned int	index;		// key data index
	float	advance;			// (1/length)
} KFRAME3;

#define MOT_KEYOPT_AUTOKEY 0x400
#define MOT_KEYOPT_PARENT 0x2000
#define MOT_KEYOPT_REVERSE 0x4000
#define MOT_KEYOPT_ETERNAL 0x8000

#define MOT_FLAG_NEEDCALC 0x200
#define MOT_FLAG_AUTOKEY 0x400
#define MOT_FLAG_ACTION 0x100

#define MOT_ATTR_STATIC 1
#define MOT_ATTR_LINEAR 2
#define MOT_ATTR_SPLINE 16

void MocSetMax( int max );
int MocInit( void );
void MocTerm( void );
void MocMove( int id1, int id2 );
void MocMoveAll( void );
int MocRegist( int opt );
void MocDelete( int id );
void MocDeleteAll( void );

void MocNewKey( int id, int subid, float fval, int times );
void MocNewKeyR( int id, int subid, float fval, int times );
void MocNewKeyI( int id, int subid, int ival, int times );
void MocNewKeyFV( int id, VECTOR *fv, int times );
void MocNewKeyIV( int id, IVECTOR *ivector, int times );

void MocSetKey( int id, int subid, float fval );
void MocSetKeyI( int id, int subid, int ival );
void MocSetKeyD( int id, int subid, float fval );
void MocSetKeyFV( int id, VECTOR *fv );
void MocSetKeyFVD( int id, VECTOR *fv );
void MocSetKeyIV( int id, IVECTOR *ivector );
void MocSetKeyF3( int id, VECTOR *fv );

float MocGetKey( int id, int subid );
int MocGetKeyI( int id, int subid );
void MocGetKeyFV( int id, VECTOR *fv );
void MocGetKeyIV( int id, IVECTOR *ivector );
void MocGetKeyFP( int id, float *fptr );
void MocGetKeyFP2( int id, int num, float *fptr );
void MocGetKeyFP3( int id, int num, float *fptr );
void *MocGetPtr( int id );

int MocGetFinishAll( int id );
int MocGetFinish( int id, int subid );
void MocSetAttr( int id, int opt );
void MocSetOption( int id, int subid, int opt );

int MocCnvKeyArray( int kfnum, void *kfdata, void *kfreal );
int MocAssignKeyArray( int id, int opt, void *fdata, void *kfdata );

void MocSetType( int id, int type );

/*---------------------------------------------------------------------------*/

#if defined(_LANGUAGE_C_PLUS_PLUS)||defined(__cplusplus)||defined(c_plusplus)
}
#endif

#endif
