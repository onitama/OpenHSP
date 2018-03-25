
//	localinfo.cpp functions

#ifndef __localinfo_h
#define __localinfo_h

//  class
class CLocalInfo {
public:
	CLocalInfo();
	~CLocalInfo();

	int GetTime( int index );
	char *CurrentTime( void );
	char *CurrentDate( void );

private:
	char curtime[16];
	char curdate[16];
};

#endif
