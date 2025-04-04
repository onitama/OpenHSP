
//
//	File Pack manager 2
//	Copyright 2022- ONION software/onitama
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cctype>

#ifdef HSPWIN
#include <direct.h>
#include "windows.h"
#endif

#if defined(HSPLINUX) || defined(HSPNDK) || defined(HSPIOS)
#include<unistd.h>
#define _getcwd getcwd
#endif

#include "hsp3config.h"
#include "supio.h"
#include "filepack.h"
#include "hsp3crypt.h"

#define _MALLOC malloc
#define _FREE free

#define WELCOMEMSG "DPM2 Manager 1.0"
#define DPMFILEEXT ".dpm"
#define DPMENCODE_DEFVAL 0

/*------------------------------------------------------------*/
/*
		UTF8 Util
*/
/*------------------------------------------------------------*/

void FilePack::StrCase(char* str)
{
	//		strをすべて小文字に(utf8/sjis対応版)
	//		('\'を'/'にも変換)
	//
	unsigned char* p;
	unsigned char a1;
	p = (unsigned char*)str;
	while (1) {
		a1 = *p; if (a1 == 0) break;
		if (a1 == '\\') a1 = '/';
		*p = tolower(a1);
		p+=StrCopyLetter((char *)p,NULL);	// 検索位置を移動
	}
}

void FilePack::StrSplit(char *target, char *fpath, char *filename)
{
	//		targetをパスとファイル名に分離(utf8/sjis対応版)
	//
	unsigned char *p;
	unsigned char *div;
	unsigned char a1;
	unsigned char *dst;
	int i;
	int diff;

	fpath[0] = 0;
	filename[0] = 0;
	p = (unsigned char*)target;
	div = p;
	while (1) {
		a1 = *p; if (a1 == 0) break;
		if (a1 == '\\') {
			div = p;
		}
		if (a1 == '/') {
			div = p;
		}
		p += StrCopyLetter((char*)p, NULL);		// 検索位置を移動
	}

	i = 0;
	p = (unsigned char*)target;
	if (div != p) {
		dst = (unsigned char*)fpath;
		while (1) {
			if (p>div) break;
			a1 = *p; if (a1 == 0) break;
			diff = StrCopyLetter((char*)p, (char *)dst);	// 1文字コピー
			p += diff;										// 検索位置を移動
			dst += diff;									// 検索位置を移動
			i++;
			if ( i>= HFP_NAME_MAX ) break;
		}
		*dst++ = 0;
	}
	i = 0;
	dst = (unsigned char*)filename;
	while (1) {
		a1 = *p; if (a1 == 0) break;
		diff = StrCopyLetter((char*)p, (char*)dst);		// 1文字コピー
		p += diff;										// 検索位置を移動
		dst += diff;									// 検索位置を移動
		i++;
		if (i >= HFP_NAME_MAX) break;
	}
	*dst++ = 0;
}

/*------------------------------------------------------------*/
/*
		constructor
*/
/*------------------------------------------------------------*/

FilePack::FilePack()
{
	//		初期化
	//
	int i;
	for(i=0;i<HFP_MAX;i++) {
		buf[i] = NULL;
		hsp3crypt[i] = NULL;
	}
	memfile_enable = true;
	memfile_active = false;
	Reset();
}

FilePack::~FilePack()
{
	//		すべて破棄
	//
	Reset();
}

/*------------------------------------------------------------*/
/*
		interface (File Service)
*/
/*------------------------------------------------------------*/

void FilePack::pack_memenable(bool sw)
{
	memfile_enable = sw;
}


FILE* FilePack::pack_fopen(char* name, int offset)
{
	HFPHED* hed;
	HFPOBJ* obj;
	FILE* ff;
	int ofs;
	int encode;
	filebase = HFP_FILEBASE_NORMAL;
	encode = 0;
	int i;

	i = *(int*)name;
	memfile_active = false;
	fopen_crypt = 0;
	if ((i == 0x3a4d454d) || (i == 0x3a6d656d)) {			// 'MEM:'をチェックする
		if (memfile_enable == false) {
			return NULL;
		}
		if (memfile.pt == NULL) {
			return NULL;
		}
		memfile_active = true;
		filebase = HFP_FILEBASE_MEMFILE;
		return (FILE *)name;
	}

	obj = SearchFileObject(name);
	if (obj == NULL) {
		filebase = HFP_FILEBASE_NORMAL;
		return hsp3_fopen(name,offset);
	}

	if (offset < 0) return NULL;
	hed = GetCurrentHeader();
	ofs = hed->filetable + (int)obj->offset;
	ofs += offset;

	if (curnum == exedpm_slot) {
		filebase = HFP_FILEBASE_PACKEXE;
		ofs += exedpm_offset;
	}
	else {
		filebase = HFP_FILEBASE_PACKDPM;
	}
	HSP3Crypt* cm = GetCurrentCryptManager();
	ff = hsp3_fopen(cm->GetBasePath(), ofs);
	if (ff == NULL) return NULL;

	fopen_crypt = obj->crypt;
	if (obj->crypt) {
		char pathname[HFP_PATH_MAX + 1];
		strcpy(pathname, GetFolderName(obj));
		strcat(pathname, GetFileName(obj));

		int enc_crypt = cm->GetCRC32(pathname, strlen(pathname));			// ファイルパスを暗号キーにする
		enc_crypt = cm->GetSalt(enc_crypt);
		if (enc_crypt == 0) enc_crypt = 1;
		int size = (int)obj->size;
		if (enc_crypt != obj->crypt) {
			return NULL;
		}
		cm->DataSet(NULL, size, obj->crypt);
		cm->SetOffset(offset);
	}
	return ff;
}


int FilePack::GetCurrentDPMOffset(void)
{
	if (curnum == exedpm_slot) {
		return exedpm_offset;
	}
	return 0;
}


void FilePack::pack_fclose(FILE* ptr)
{
	if (memfile_active) {
		memfile.pt = NULL;
		memfile_active = false;
		return;
	}
	hsp3_fclose(ptr);
}


int FilePack::pack_fgetc(FILE* ptr)
{
	int i = 0;
	int res = pack_fread(ptr,&i,1);
	if (res <= 0) return -1;
	return i;
}


int FilePack::pack_flength(char* name)
{

	HFPOBJ* obj;
	obj = SearchFileObject(name);
	if (obj) return (int)obj->size;

	int size = hsp3_flength(name);
	if (size>=0) return size;

	FILE* ff = pack_fopen(name);
	if (ff != NULL) {
		if (memfile_active) {					// メモリストリーム時
			return memfile.size;
		}
		pack_fclose(ff);
	}
	return -1;
}


int FilePack::pack_fread(FILE* ptr, void* mem, int size)
{
	int len;
	if (ptr == NULL) return -1;

	if (memfile_active) {
		len = size;
		if ((memfile.cur + size) >= memfile.size) len = memfile.size - memfile.cur;
		if (len > 0) {
			memcpy(mem, memfile.pt + memfile.cur, len);
			memfile.cur += len;
		}
		return len;
	}

	len = (int)hsp3_fread(ptr, mem, size);

	if (filebase == HFP_FILEBASE_NORMAL) {
		return len;
	}
	if (fopen_crypt) {
		int i;
		unsigned char* p = (unsigned char*)mem;
		HSP3Crypt* cm = GetCurrentCryptManager();
		for (i = 0; i < len; i++) {
			*p = cm->Decrypt(*p);
			p++;
		}
	}
	return len;
}


int FilePack::pack_fbase(char* name)
{
	FILE* ff = pack_fopen(name);
	if (ff == NULL) return -1;
	pack_fclose(ff);
	return filebase;
}


void FilePack::pack_memfile(void* mem, int size)
{
	memfile.pt = (char*)mem;
	memfile.cur = 0;
	memfile.size = size;
}


void FilePack::pack_getinfstr(char* inf)
{
	HFPHED* hed = GetPackHeader(0);
	if (hed == NULL) {
		*inf = 0;
		return;
	}
	int ofs = 0;
	HSP3Crypt* cm = hsp3crypt[0];
	if (exedpm_slot == 0) {
		ofs = exedpm_offset;
	}
	sprintf(inf, "%s,%d", cm->GetBasePath(), ofs );
}


/*------------------------------------------------------------*/
/*
		interface (read)
*/
/*------------------------------------------------------------*/

void FilePack::Reset( void )
{
	int i;
	for (i = 0; i < HFP_MAX; i++) {
		DeleteSlot(i);
	}
	curnum = 0;
	wrtbuf = NULL;
	errbuf = NULL;
	wrtstr = NULL;
	exedpm_slot = -1;
	exedpm_offset = 0;
	filebase = 0;
}


void FilePack::DeleteSlot(int slot)
{
	int i = slot;
	if ((i<0)||(i>=HFP_MAX)) {
		return;
	}
	if (buf[i] != NULL) {
		_FREE(buf[i]);
		buf[i] = NULL;
	}
	if (hsp3crypt[i] != NULL) {
		delete hsp3crypt[i];
		hsp3crypt[i] = NULL;
	}
}


int FilePack::GetEmptySlot(void)
{
	int i;
	for (i = 0; i < HFP_MAX; i++) {
		if (buf[i] == NULL) return i;
	}
	return -1;
}


void FilePack::PrepareRead(int slot, int value)
{
	//		hsp3cryptを準備する
	//
	char pwtmp[128];
	sprintf(pwtmp, "HSP3Encode:%d", value);
	//Print(pwtmp);

	DeleteSlot(slot);
	hsp3crypt[slot] = new HSP3Crypt();
	hsp3crypt[slot]->SetPassword(pwtmp);
}


int FilePack::LoadPackFile( char *fname, int encode, int dpmoffset, int slot)
{
	FILE *ff;
	HFPHED testhed;
	HFPHED *p;
	int i;
	int hedsize;
	char dpmname[HFP_PATH_MAX + 1];

	//Print(WELCOMEMSG);

	curnum = slot;
	if (curnum < 0) {
		curnum = GetEmptySlot();
	}
	if (curnum < 0) return -1;

	*dpmname = 0;
#ifdef HSPWIN
	if (dpmoffset == 0) {
		bool addcurrent = true;
		char a1 = *fname;
		//	fnameがフルパスの場合はパスを補完しない
		if ((a1 == 0x5c) || (a1 == '/')) addcurrent = false;
		if ((a1 != 0)&&(fname[1]==':')) addcurrent = false;
		if (addcurrent) {
			_getcwd(dpmname, HFP_PATH_MAX);
			strcat(dpmname, "/");
		}
	}
#endif
	strcat(dpmname, fname);
	//strcat(dpmname, DPMFILEEXT);
	dpmname[HFP_PATH_MAX] = 0;

	ff = hsp3_fopen(dpmname, dpmoffset);
	if (ff==NULL) return -2;

	hsp3_fread(ff, &testhed, sizeof(HFPHED));
	hsp3_fclose(ff);

	if (dpmoffset > 0) {
		exedpm_slot = curnum;
		exedpm_offset = dpmoffset;
	}

	hedsize = testhed.filetable;
	if (hedsize < sizeof(HFPHED)) return -2;

	p = (HFPHED *)_MALLOC( hedsize );
	ff=hsp3_fopen(dpmname, dpmoffset);
	if (ff == NULL) return -2;
	hsp3_fread( ff, p, hedsize );
	hsp3_fclose(ff);

	if (( p->h1 != HFP_MAGIC1 )||( p->h2 != HFP_MAGIC2 )||
		( p->h3 != HFP_MAGIC3 )||( p->h4 != HFP_MAGIC4 )) {
			_FREE( p );
			return -3;
	}
	PrepareRead(curnum, p->seed + encode);
	buf[curnum] = p;

	HSP3Crypt* cm = GetCurrentCryptManager();

	if (encode>0) {
		if (p->salt != cm->GetSalt(encode + p->seed)) {
			return -4;
		}
	}
	cm->SetBasePath(dpmname);

	HFPOBJ* obj;
	obj = GetCurrentObjectHeader();
	for (i = 0; i < p->max_file; i++) {
		obj->slot = curnum;
		obj++;
	}
	return 0;
}

/*------------------------------------------------------------*/
/*
		interface (info)
*/
/*------------------------------------------------------------*/

int FilePack::GetPackSlot( void )
{
	return curnum;
}


HFPHED *FilePack::GetPackHeader( int slot )
{
	if (( slot < 0 )||( slot >= HFP_MAX)) return NULL;
	return buf[slot];
}


HFPHED* FilePack::GetCurrentHeader(void)
{
	return GetPackHeader(curnum);
}


HFPOBJ* FilePack::GetCurrentObjectHeader(void)
{
	HFPHED *hed = GetCurrentHeader();
	HFPOBJ* obj = (HFPOBJ*)(hed + 1);
	return obj;
}


HSP3Crypt *FilePack::GetCurrentCryptManager(void)
{
	return 	(hsp3crypt[curnum]);

}


int FilePack::GetFileNum( HFPHED *hed )
{
	return hed->max_file;
}


char *FilePack::GetPackName( HFPHED *hed )
{
	char *strbase;
	strbase = ((char *)hed) + hed->strtable;
	return strbase + hed->myname;
}


HFPOBJ* FilePack::GetFileObject(HFPHED* hed, int id)
{
	HFPOBJ* obj;
	if ((id < 0) || (id >= hed->max_file)) return NULL;
	obj = (HFPOBJ*)(hed + 1);
	return &obj[id];
}


char* FilePack::GetString(HFPHED* hed, int ptr)
{
	HFPHED* p = hed;
	if (p == NULL) return NULL;
	char* strbase = ((char*)p) + p->strtable;
	return strbase + ptr;
}


char* FilePack::GetFileName(HFPOBJ* obj)
{
	HFPHED* p = GetPackHeader(curnum);
	return GetString(p, obj->name);
}


char* FilePack::GetFolderName(HFPOBJ* obj)
{
	HFPHED* p = GetPackHeader(curnum);
	if (obj->folder == 0) return "";
	return GetString(p, obj->folder);
}


int FilePack::GetFileSize(char* name)
{
	HFPOBJ* obj;
	obj = SearchFileObject(name);
	if (obj != NULL) {
		return (int)obj->size;
	}
	return -1;
}


HFPOBJ *FilePack::SearchFileObject( HFPHED *hed, char *name )
{
	//	ファイル情報を検索する
	//
	int i;
	HFPOBJ *obj;
	char fname[HFP_PATH_MAX + 1];
	char fname_utf8[HFP_PATH_MAX + 1];
	char foldername[HFP_PATH_MAX + 1];
	char foldername_utf8[HFP_PATH_MAX + 1];

	if (hed == NULL) return NULL;

	StrSplit(name, foldername, fname);		// Split Path and File Name
	StrCase(fname);
	StrCase(foldername);

	// UTF8に変換する
	hsp3_to_utf8(fname_utf8, fname, HFP_PATH_MAX);
	hsp3_to_utf8(foldername_utf8, foldername, HFP_PATH_MAX);

	obj = (HFPOBJ *)(hed+1);
	for(i=0;i<hed->max_file;i++) {
		bool fchk = true;
		if (obj->folder != 0) {
			if (strcmp(foldername_utf8, GetFolderName(obj)) != 0) fchk = false;
		}
		else {
			if (*foldername != 0) fchk = false;
		}
		if (fchk) {
			if (strcmp(fname_utf8, GetFileName(obj)) == 0) return obj;
		}
		obj++;
	}
	return NULL;
}


HFPOBJ* FilePack::SearchFileObject(char* name)
{
	int i;
	HFPOBJ* obj;
	int baknum = GetPackSlot();
	for (i = 0; i < HFP_MAX; i++) {
		HFPHED* hed = buf[i];
		if (hed) {
			SetCurrentSlot(i);
			obj = SearchFileObject(hed, name);
			if (obj != NULL) {
				return obj;
			}
		}
	}
	SetCurrentSlot(baknum);
	return NULL;
}


int FilePack::pack_fread(char* name, void* mem, int size, int seekofs)
{
	FILE *pt = pack_fopen(name, seekofs);
	if (pt == NULL) {
		return hsp3_rawload(name, mem, size, seekofs);
	}
	int len = pack_fread(pt, mem, size);
	pack_fclose(pt);
	return len;
}


/*------------------------------------------------------------*/
/*
		interface (stream)
*/
/*------------------------------------------------------------*/

DpmFile::DpmFile()
{
	filebase = -1;
	_file = NULL;
	filepack = NULL;
}


DpmFile::~DpmFile()
{
	close();
}


bool DpmFile::open(FilePack* pack, char* fname)
{
	HFPHED* hed;
	HFPOBJ* obj;
	FILE* ff;
	int ofs, ofs2;

	filepack = pack;
	if (filepack == NULL) return false;
	cur = 0;
	size = 0;
	_file = NULL;
	endflag = false;
	baseoffset = 0;
	fopen_crypt = 0;

	obj = filepack->SearchFileObject(fname);
	if (obj == NULL) {
		ff = hsp3_fopen(fname, 0);
		if (ff == NULL) {
			return false;
		}
		_file = ff;
		filebase = HFP_FILEBASE_NORMAL;
		return true;
	}

	hed = filepack->GetCurrentHeader();
	ofs = hed->filetable + (int)obj->offset;
	ofs2 = filepack->GetCurrentDPMOffset();
	baseoffset = ofs + ofs2;
	if (ofs2>0) {
		filebase = HFP_FILEBASE_PACKEXE;
	}
	else {
		filebase = HFP_FILEBASE_PACKDPM;
	}
	crypt = filepack->GetCurrentCryptManager();
	ff = hsp3_fopen(crypt->GetBasePath(), baseoffset );
	if (ff == NULL) {
		filebase = -1;
		return false;
	}

	fopen_crypt = obj->crypt;
	size = (int)obj->size;
	if (obj->crypt) {
		char pathname[HFP_PATH_MAX + 1];
		strcpy(pathname, filepack->GetFolderName(obj));
		strcat(pathname, filepack->GetFileName(obj));

		int enc_crypt = crypt->GetCRC32(pathname, strlen(pathname));			// ファイルパスを暗号キーにする
		enc_crypt = crypt->GetSalt(enc_crypt);
		if (enc_crypt == 0) enc_crypt = 1;
		if (enc_crypt != obj->crypt) {
			filebase = -1;
			return false;
		}
		crypt->DataSet(NULL, size, obj->crypt);
		crypt->SetOffset(0);
	}
	_file = ff;
	return true;
}

void DpmFile::close(void)
{
	if (filepack == NULL) return;
	if (_file) {
		hsp3_fclose(_file);
		_file = NULL;
	}
	filebase = -1;
	filepack = NULL;
	endflag = true;
}


size_t DpmFile::read(void* mem, size_t sz, size_t count)
{
	int readsize = (int)(sz * count);
	int len;

	if ((filepack == NULL)||(_file == NULL)) return -1;

	len = (int)hsp3_fread(_file, mem, readsize);
	if ( len <= 0 ) {
		len = 0;
		endflag = true;
	}
	if (filebase == HFP_FILEBASE_NORMAL) {
		cur += len;
		return (size_t)(len/sz);
	}
	if (fopen_crypt) {
		int i;
		unsigned char* p = (unsigned char*)mem;
		crypt->DataSet(NULL, size, fopen_crypt);
		crypt->SetOffset(cur);
		for (i = 0; i < len; i++) {
			*p = crypt->Decrypt(*p);
			p++;
		}
	}
	cur += len;
	if (cur >= size) {
		endflag = true;
		cur = size;
	}
	return (size_t)(len/sz);
}


char* DpmFile::readLine(char* str, int num)
{
	if ((filepack == NULL) || (_file == NULL)) return NULL;
/*
	if (filebase == HFP_FILEBASE_NORMAL) {
		char *res = fgets(str, num, _file);
		Alertf("Read ok[%s]",res);
		if (res) {
			cur += strlen(str);
		} else {
			endflag = true;
		}
		return res;
	}
*/
	if (fopen_crypt) {
		crypt->DataSet(NULL, size, fopen_crypt);
		crypt->SetOffset(cur);
	}
	int i = num-1;
	char* p = str;
	int a1=0;
	if ((p == NULL)||(num<1)) return NULL;
	while (1) {
		if (i <= 0) break;
		if (size>0) {
			if ( cur>=size ) break;
		}
		int res = hsp3_fread(_file, &a1, 1);
		if ( res <= 0 ) {
			endflag = true;
			break;
		}
		cur++;
		i--;
		if (fopen_crypt) {
			a1 = crypt->Decrypt(a1);
		}
		*p++ = (char)a1;
		if (a1 == 10) {
			break;
		}
	}
	*p = 0;
	return str;
}


bool DpmFile::rewind(void)
{
	if ((filepack == NULL) || (_file == NULL)) return false;
	//if (filebase == HFP_FILEBASE_NORMAL) {
	//	::rewind(_file);
	//	return true;
	//}
	seek(0, SEEK_SET);
	return true;
}


bool DpmFile::seek(int offset, int origin)
{
	if ((filepack == NULL) || (_file == NULL)) return false;
	//if (filebase == HFP_FILEBASE_NORMAL) {
		//Alertf("SEEK:%d", offset);
		//return hsp3_fseek(_file, offset, origin) == 0;
	//}
	int newpos;
	switch (origin) {
	case SEEK_CUR:
		newpos = cur;
		break;
	case SEEK_END:
		newpos = size;
		break;
	default:
		newpos = 0;
		break;
	}
	cur = newpos + offset;
	if (cur < 0) {
		cur = 0;
	}
	if (size > 0) {
		if (cur >= size) {
			cur = size;
			endflag = true;
		}
	}
	if (filebase == HFP_FILEBASE_NORMAL) {
		return hsp3_fseek(_file, cur, SEEK_SET) == 0;
	}
	hsp3_fseek(_file, baseoffset+cur, SEEK_SET);
	return true;
}


bool DpmFile::eof(void)
{
	if ((filepack == NULL) || (_file == NULL)) return true;
	if (filebase == HFP_FILEBASE_NORMAL) {
		//int res = feof(_file);
		//Alertf("eof %d",res);
		//if (res) return true;
		//return ((size_t)position()) >= length();
		return endflag;
	}
	if (cur>=size) return true;
	return false;
}


int DpmFile::position(void)
{
	if ((filepack == NULL) || (_file == NULL)) return -1;
	//if (filebase == HFP_FILEBASE_NORMAL) {
	//	return ftell(_file);
	//}
	return cur;
}


size_t DpmFile::length(void)
{
	if ((filepack == NULL) || (_file == NULL)) return -1;
	if (filebase == HFP_FILEBASE_NORMAL) {
		if ( size == 0 ) {
		}
		return (size_t)size;
	}
	return (size_t)size;
}
