
//
//	filepack.cpp header
//
#ifndef __filepack_h
#define __filepack_h

#include <cstdint>
#include "hsp3utfcnv.h"

#define HFP_MAX 16				// HFP slotの同時使用最大数
#define HFP_MAGIC1 'D'
#define HFP_MAGIC2 'P'
#define HFP_MAGIC3 'M'
#define HFP_MAGIC4 '2'

#define HFP_NAME_MAX (259)						// ファイル名の最大文字数
#define HFP_PATH_MAX ((HFP_NAME_MAX*8)+1)		// dpmパス名の最大バッファサイズ(byte)

#define HFPOBJ_FLAG_ENTRY 1		// ファイルあり
#define HFPOBJ_FLAG_ENCRYPT 2	// 暗号化済みファイル
#define HFPOBJ_FLAG_ARCHIVE 4	// 圧縮済みファイル(未サポート)
#define HFPOBJ_FLAG_EXE 8		// EXEファイル埋め込み
#define HFPOBJ_FLAG_HSP36 16	// HSP3.6までの形式(階層なし)

#define HFPSIZE uint64_t

#define HFP_FILEBASE_NORMAL 0
#define HFP_FILEBASE_PACKEXE 1
#define HFP_FILEBASE_PACKDPM 2
#define HFP_FILEBASE_MEMFILE 3

typedef struct HFPOBJ
{
	//		FPACK1.0 file object structure
	//
	short		flag;				// file flags
	short		slot;				// HFP slot ID
	int			name;				// file name (StrTable ID)
	HFPSIZE		size;				// file size
	HFPSIZE		offset;				// packed file offset
	int			folder;				// folder name (StrTable ID)
	int			crypt;				// encryption code (0=none)

} HFPOBJ;

typedef struct HFPHED
{
	//		FPACK1.0 header structure
	//
	char	h1;					// magic code1
	char	h2;					// magic code2
	char	h3;					// magic code3
	char	h4;					// magic code4
	int		max_file;			// max count of File Object
	int		strtable;			// string table pointer (32bit)
	int		filetable;			// file table pointer (32bit)
	int		myname;				// HFP file name (StrTable ID)
	int		crc32;				// CRC32 value (0=none)
	int		seed;				// file encryption seed
	int		salt;				// file encryption salt
} HFPHED;

typedef struct MEMFILE {
	char* pt;				// target ptr
	int		cur;				// current ptr
	int		size;				// size
} MEMFILE;

class HSP3Crypt;
class CMemBuf;

//	FilePack class
//
class FilePack {
public:
	//	Functions
	//
	FilePack( void );
	~FilePack( void );

	//	For Load Process
	//
	void Reset(void);
	void DeleteSlot(int slot = 0);
	int GetEmptySlot(void);

	int LoadPackFile(char* name, int encode = 0, int dpmoffset = 0, int slot = 0);
	void PrintFiles(void);
	int GetFileSize( char *name );
	int GetCurrentDPMOffset(void);
	void SetCurrentSlot(int slot) { curnum = slot; };

	//	File Service
	//
	FILE* pack_fopen(char* name, int offset=0);
	void pack_fclose(FILE* ptr);
	int pack_fgetc(FILE* ptr);
	int pack_flength(char* name);
	int pack_fread(FILE* ptr, void* mem, int size);
	int pack_fread(char* name, void* mem, int size, int seekofs);
	int pack_fbase(char* name);

	void pack_memenable(bool sw);
	void pack_memfile(void *mem, int size);
	void pack_getinfstr(char* mem);

	//	For Save Process
	//
	int SavePackFile( char *fname, char *packname, int encode=0, int opt_encode=0 );
	int ExtractFile( char *fname, char *savename=NULL, int encode=0 );
	int ExtractFile(HFPHED* hed, char* name, char* savename = NULL, int encode = 0 );
	int MakeEXEFile(int mode, char* hspexe, char *basename, int deckey=0, int opt1=640, int opt2=480, int opt3=0);

	//	For Info
	//
	int GetPackSlot( void );
	HFPHED *GetCurrentHeader(void);
	HFPOBJ *GetCurrentObjectHeader(void);
	HSP3Crypt *GetCurrentCryptManager(void);
	HFPHED *GetPackHeader( int slot );
	char *GetPackName( HFPHED *hed );
	int GetFileNum( HFPHED *hed );
	HFPOBJ *GetFileObject( HFPHED *hed, int id );
	HFPOBJ *SearchFileObject( HFPHED *hed, char *name );
	HFPOBJ *SearchFileObject(char* name);
	void SetErrorBuffer(CMemBuf *err);

	void Print(char* mes);
	char *GetString(HFPHED *hed, int ptr);
	char* GetFileName(HFPOBJ* obj);
	char* GetFolderName(HFPOBJ* obj);

private:
	//	Data
	//
	int curnum;
	HFPHED *buf[HFP_MAX];
	HSP3Crypt *hsp3crypt[HFP_MAX];

	CMemBuf *wrtbuf;
	CMemBuf *wrtstr;
	CMemBuf* errbuf;

	MEMFILE memfile = { NULL, 0, -1 };
	bool memfile_enable;
	bool memfile_active;

	int	wrtnum;
	int seedbase;
	HFPSIZE wrtpos;

	int exedpm_slot;
	int exedpm_offset;
	int filebase;
	int fopen_crypt;

	//	Internal Use
	//
	void PrepareRead(int slot = 0, int encode = 0);
	int CopyFileToDPM( FILE *ff, char *filename, HFPSIZE size, int encode );
	void PrepareWrite(int slot = 0, int encode = 0);
	int RegisterFromPacklist(char* name, int crypt = 1);
	int RegisterFile(char* name, int crypt = 0, int orig = 0);

	//	Utility
	//
	void StrCase(char* str);
	void StrSplit(char* target, char* fpath, char* filename);

};


//	DpmFile stream class
//
class DpmFile {
public:
	//	Functions
	//
	DpmFile();
	~DpmFile();

	bool open(FilePack* pack, char* fname);
	void close(void);
	size_t read(void* readmem, size_t size, size_t count);
	char* readLine(char* str, int num);
	bool rewind(void);
	bool seek(int offset, int origin);
	int position(void);
	bool eof(void);
	size_t length(void);

private:
	int filebase;
	int cur;
	int size;
	int baseoffset;
	int fopen_crypt;
	bool endflag;
	FILE* _file;
	FilePack* filepack;
	HSP3Crypt* crypt;
};

#endif
