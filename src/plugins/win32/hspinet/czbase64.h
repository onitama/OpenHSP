//
//	cbase64/rc4 header
//
#ifndef __cbase64_h
#define __cbase64_h

class CzBase64 {
public:
	CzBase64();
	~CzBase64();
	int Encode( char *dstptr, char *srcptr, int size );
	int Decode( char *dstptr, char *srcptr, int size );
	int GetSize( int len );

private:
	//		private data
	//
};


class CzRC4 {
public:
	CzRC4();
	~CzRC4();
	void SetKey( char *key );
	int Encode( char *ptr, int size );
	void Swap(unsigned char *a, unsigned char *b);

private:
	//		private data
	//
	unsigned char state[256];
	unsigned char xs;
	unsigned char ys;
};


#endif
