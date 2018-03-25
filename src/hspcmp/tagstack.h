
//
//	tagstack.cpp structures
//
#ifndef __tagstack_h
#define __tagstack_h

#define TAGSTK_MAX 256
#define TAGSTK_SIZE 124
#define TAGSTK_TAGMAX 256
#define TAGSTK_TAGSIZE 56

// tag info storage
typedef struct TAGINF {
	char	name[TAGSTK_TAGSIZE];	// tag name
	int		check;					// resolve check flag
	int		uid;					// unique ID
} TAGINF;

// tag data storage
typedef struct TAGDATA {
	int		tagid;					// tag ID
	char	data[TAGSTK_SIZE];		// data
} TAGDATA;


//  class
class CTagStack {
public:
	CTagStack();
	~CTagStack();

	int GetTagID( char *tag );
	char *GetTagName( int tagid );
	int PushTag( int tagid, char *str );
	char *PopTag( int tagid );
	char *LookupTag( int tagid, int level );
	void GetTagUniqueName( int tagid, char *outname );
	int StackCheck( char *res );

private:
	//		Data
	//
	TAGINF	mem_tag[TAGSTK_TAGMAX];	// Tag info
	TAGDATA	mem_buf[TAGSTK_MAX];	// Main Buffer
	char	tagerr[8];				// Tag Error String
	int		tagent;					// Tag Entry
	int		lastidx;				// Current last index
	int		gcount;					// Global Counter

	//		Private members
	//
	int StrCmp( char *str1, char *str2 );
	int SearchTagID( char *tag );
	int RegistTagID( char *tag );
};


#endif
