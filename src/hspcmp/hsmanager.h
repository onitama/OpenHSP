
//
//	hsmanager.cpp structures
//
#ifndef __hsmanager_h
#define __hsmanager_h

#include <string>
#include "membuf.h"
#include "strnote.h"

#define INDEX_FILE "hsphelp.idx"
#define SPLTMP_MAX 255

//  class
class HspHelpManager {
public:
	HspHelpManager();
	~HspHelpManager();

	void terminate( void );
	int initalize( char *pathname );

	int searchIndex( char *key );
	char *getMessage(void);

	//		Data
	//
	std::string	m_pathname;		// hsphelp path
	std::string	m_filename;		// index file name
	std::string	m_hsfilename;	// .HS file name
	std::string	m_message;		// system message

	std::string	m_id_line;		// (index) line
	std::string	m_id_hsfile;	// (index) file
	std::string	m_id_plugin;	// (index) plugin
	std::string	m_id_title;		// (index) title

	std::string	m_key;			// keyword
	std::string	m_title;		// title
	std::string	m_group;		// group
	std::string	m_prm;			// parameter
	std::string	m_info;			// information
	std::string	m_dll;			// dll plugin/module
	std::string	m_sample;		// sample
	std::string	m_ref;			// reference
	std::string	m_type;			// type
	std::string	m_ver;			// version
	std::string	m_note;			// note
	std::string	m_date;			// date
	std::string	m_author;		// author
	std::string	m_url;			// url
	std::string	m_port;			// port

private:
	//		Private Data
	//
	CStrNote note;				// note object
	CMemBuf *indexbuf;			// index buffer

	CStrNote hsnote;			// note object for .HS
	CMemBuf *hsbuf;				// .HS buffer

	std::string	infbuf;			// info buffer for work
	std::string	tagname;		// tag name buffer

	int splc;					// split pointer
	char *srcstr;				// split source
	char spltmp[SPLTMP_MAX+1];	// temp buffer

	//		Private members
	//
	int openHSFile(char *fname, int offset=0);
	int getTaggedInfoFromHS(int target_line = 0);
	int getInfoFromHS(int target_line = 0);
	int getTypesFromHS(char *start, char *limit_addr);
	int getKeywordFromIndex(char * key);

	void strsp_ini(char *src);
	char *strsp_getptr(void);
	int strsp_get(char splitchr);

};


#endif
