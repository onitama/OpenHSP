
//
//	File Pack manager 2 (Write lib)
//	Copyright 2022- ONION software/onitama
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <cctype>
#include <string>

#ifdef HSPWIN
#include <direct.h>
#include "windows.h"
#endif

#include "hsp3config.h"
#include "supio.h"
#include "filepack.h"
#include "strnote.h"
#include "strbuf.h"
#include "hsp3crypt.h"
#include "hsp3pathio.h"
#include "../hspcmp/membuf.h"

#define _MALLOC malloc
#define _FREE free

static int hsp_pack_path_append(char* target, size_t target_size, const char* suffix)
{
	if (target == NULL || suffix == NULL || target_size == 0) return 0;
	size_t target_length = strlen(target);
	size_t suffix_length = strlen(suffix);
	if (target_length >= target_size || suffix_length > target_size - target_length - 1) return 0;
	memcpy(target + target_length, suffix, suffix_length + 1);
	return 1;
}

#define WELCOMEMSG "DPM2 Manager 1.1"
#define DPMFILEEXT ".dpm"
#define DPMENCODE_DEFVAL 0

#if ( WIN32 || _WIN32 ) && ! __CYGWIN__
# define FILEPACK_VSNPRINTF _vsnprintf
#else
# define FILEPACK_VSNPRINTF vsnprintf
#endif

static std::string format_message(const char* format, ...)
{
	size_t capacity = 256;
	while (true) {
		std::string result(capacity, '\0');
		va_list args;
		va_start(args, format);
		int length = FILEPACK_VSNPRINTF(result.data(), result.size(), format, args);
		va_end(args);
		if (length >= 0 && static_cast<size_t>(length) < result.size()) {
			result.resize(length);
			return result;
		}
		if (length >= 0) capacity = static_cast<size_t>(length) + 1;
		else capacity *= 2;
	}
}

/*------------------------------------------------------------*/
/*
		interface (write)
*/
/*------------------------------------------------------------*/

void FilePack::PrepareWrite( int slot, int encode )
{
	//	HFP作成準備
	//
	wrtbuf = new CMemBuf;
	wrtstr = new CMemBuf;
	wrtstr->AddIndexBuffer(1024);
	wrtstr->RegistIndex(0);
	wrtstr->Put((unsigned char)0);
	wrtnum = 0;
	wrtpos = 0;
	curnum = slot;
	Print(WELCOMEMSG);
	seedbase = encode;
	sbInit();
}


HSPPTRINT FilePack::RegisterFile(const char* name, int pcrypt, int orig)
{
	//	HFPにファイルを追加
	//
	HSPPTRINT length;
	int index;
	int enc_crypt;
	bool crypt_flag;
	HFPOBJ obj;

	char fname[HFP_PATH_MAX + 1];
	char fname_hsp3[HFP_PATH_MAX + 1];
	char foldername[HFP_PATH_MAX + 1];
	char foldername_hsp3[HFP_PATH_MAX + 1];
	char pathname[HFP_PATH_MAX + 1];
	std::string fname_utf8;
	std::string foldername_utf8;

	if (orig == 0) {
		char* findptr;
		strchr3(const_cast<char*>(name), '*', 0, &findptr);
		if (findptr != NULL) {
			//	ワイルドカード使用時
			CStrNote notelist;
			char ftmp[1024];
			std::string fixname;
			std::string p_fdir;
			int listmax;
			char* flist = sbAlloc(0x4000);
			if (dirlist(name, &flist, 5) < 0) {
				sbFree(flist);
				Print((char*)"#Directory listing failed.");
				return -1;
			}
			notelist.Select(flist);
			listmax = notelist.GetMaxLine();
			// ディレクトリを再帰する
			for (int i = 0; i < listmax; i++) {
				notelist.GetLine(ftmp, i);
				if (name == NULL || !getpath(std::string(name), fixname, 32)) {
					sbFree(flist);
					Print((char*)"#Path is too long or invalid.");
					return -1;
				}
				fixname += ftmp;
				fixname += "/*";
				HSPPTRINT res = RegisterFile(fixname.c_str(), pcrypt);
				if (res < 0) {
					sbFree(flist);
					return res;
				}
			}
			sbFree(flist);

			// すべてのファイルを追加する
			if (name == NULL || !getpath(std::string(name), p_fdir, 32)) {
				Print((char*)"#Path is too long or invalid.");
				return -1;
			}
			flist = sbAlloc(0x4000);
			if (dirlist(name, &flist, 1) < 0) {
				sbFree(flist);
				Print((char*)"#Directory listing failed.");
				return -1;
			}
			notelist.Select(flist);
			listmax = notelist.GetMaxLine();
			for (int i = 0; i < listmax; i++) {
				notelist.GetLine(ftmp, i);
				fixname = p_fdir + ftmp;
				HSPPTRINT res = RegisterFile(fixname.c_str(), pcrypt);
				if (res < 0) {
					sbFree(flist);
					return res;
				}
			}
			sbFree(flist);
			return 0;
		}
	}

	StrSplit(name, foldername_hsp3, fname_hsp3);		// Split Path and File Name
	StrCase(fname_hsp3);
	StrCase(foldername_hsp3);

	fname[0] = 0;
	foldername[0] = 0;
	if (hsp_path_to_utf8(fname_utf8, hsp_path::path_view(fname_hsp3)) != 0 ||
		hsp_path_to_utf8(foldername_utf8, hsp_path::path_view(foldername_hsp3)) != 0 ||
		!hsp_pack_path_append(fname, sizeof(fname), fname_utf8.c_str()) ||
		!hsp_pack_path_append(foldername, sizeof(foldername), foldername_utf8.c_str())) {
		Print((char*)"#Path is too long.");
		return -1;
	}

	pathname[0] = 0;
	if (!hsp_pack_path_append(pathname, sizeof(pathname), foldername) ||
		!hsp_pack_path_append(pathname, sizeof(pathname), fname)) {
		Print((char*)"#Path is too long.");
		return -1;
	}

	HSP3Crypt* cm = GetCurrentCryptManager();
	enc_crypt = 0;
	crypt_flag = false;
	if (pcrypt != 0) {
		enc_crypt = pcrypt;
		if (pcrypt < 0) {
			if (orig) {
				enc_crypt = cm->GetCRC32(fname, strlen(fname));			// ファイル名を暗号キーにする
			}
			else {
				enc_crypt = cm->GetCRC32(pathname, strlen(pathname));			// ファイルパスを暗号キーにする
			}
			enc_crypt = cm->GetSalt(enc_crypt);
			if (enc_crypt == 0) enc_crypt = 1;
		}
		crypt_flag = true;
	}

	length = hsp3_flength(name);
	if (length <= 0) {
		Print(format_message("#File not found [%s]", name).c_str());
		return -1;
	}

	obj.flag = HFPOBJ_FLAG_ENTRY;
	if (crypt_flag) {
		obj.flag |= HFPOBJ_FLAG_ENCRYPT;
	}
	if (orig) {
		obj.flag |= HFPOBJ_FLAG_HSP36;
	}

	index = wrtstr->SearchIndexedData(fname, -1);
	if (index < 0) {
		index = wrtstr->GetSize();
		wrtstr->RegistIndex(index);
		wrtstr->PutStrBlock(fname);
	}
	obj.name = index;

	if (*foldername != 0) {
		index = wrtstr->SearchIndexedData(foldername, -1);
		if (index < 0) {
			index = wrtstr->GetSize();
			wrtstr->RegistIndex(index);
			wrtstr->PutStrBlock(foldername);
		}
		obj.folder = index;
	}
	else {
		obj.folder = 0;
	}
	obj.size = length;
	obj.offset = wrtpos;
	obj.slot = curnum;
	obj.crypt = enc_crypt;

#ifdef HSP64
	Print(format_message("#%d %s (%lld)(%d)", wrtnum, name, length, enc_crypt).c_str());
#else
	Print(format_message("#%d %s (%d)(%d)", wrtnum, name, length, enc_crypt).c_str());
#endif

	wrtpos += length;
	wrtnum++;
	wrtbuf->PutData(&obj, sizeof(HFPOBJ));

	return length;
}


int FilePack::RegisterFromPacklist( const char *name, int def_crypt)
{
	//	regist from file list
	//
	int max,index,crypt;
	char s1[1024];

	CMemBuf packlist;
	if (packlist.PutFile(name) < 1) {
		Print("#No fie.");
		return -1;
	}

	PrepareRead(0, seedbase+def_crypt);

	crypt = -1;
	CStrNote note;
	note.Select( packlist.GetBuffer() );
	max = note.GetMaxLine();
	index = 0;
	while (1) {
		char a1;
		char* fn;
		int enc;
		if (index >= max) break;
		enc = 0;
		note.GetLine(s1, index, 1023);
		index++;
		fn = s1;
		a1 = s1[0];
		switch (a1) {
		case ';':					// comment
			break;
		case '#':					// crypt value
			crypt = atoi(s1+1);
			break;
		case '+':					// encode flag
			fn++; enc = crypt;
			//	through
		default:
			HSPPTRINT length;
			a1 = *fn;
			if (a1 == '>') {
				fn++;
				length = RegisterFile(fn, enc);
			}
			else {
				length = RegisterFile(fn, enc, 1);
			}
			if (length < 0) return -1;
			break;
		}
	}

	return wrtnum;
}


int FilePack::CopyFileToDPM( FILE *ff, const char *filename, HFPSIZE psize, int encode )
{
	//	copy file to pack
	//
	int a;
	FILE *ff2;
	unsigned char ch;
	int count = 0;
	int size = (int)psize;

	ff2 = hsp3_fopen( filename );
	if (ff2==NULL) return -1;

	HSP3Crypt *cm = GetCurrentCryptManager();
	cm->DataSet(NULL, size, encode );

	if ( encode==0 ) {								// 暗号化なし
		while(1) {
			if (count >= size) break;
			a=fgetc(ff2);if (a<0) break;
			fputc(a,ff);
			count++;
		}
	}
	else {											// 暗号化
		while (1) {
			if (count >= size) break;
			a = fgetc(ff2); if (a < 0) break;
			ch = (unsigned char)a;
			ch = cm->Encrypt(ch);
			fputc((int)ch, ff);
			count++;
		}
	}
	hsp3_fclose(ff2);
	return count;
}


int FilePack::SavePackFile( const char *name, const char *packname, int encode, int opt_encode )
{
	//	HFPファイルを作成
	//
	int res, i;
	int bufsize, strsize, pt_file, myname;
	int checksize;
	FILE *fp;
	HFPHED hed;
	HFPOBJ *obj;
	char *strbase;
	char *p;
	char fname[HFP_PATH_MAX +1];
	char refname[(HFP_PATH_MAX + 1)];
	std::string refname_hsp3;
	HFPOBJ* obj_bak;

	fname[0] = 0;
	if (!hsp_pack_path_append(fname, sizeof(fname), name) ||
		!hsp_pack_path_append(fname, sizeof(fname), DPMFILEEXT)) {
		Print((char*)"#Path is too long.");
		return -1;
	}
	StrCase( fname );

	PrepareWrite( 0, encode );
	if (RegisterFromPacklist(packname, opt_encode) <= 0) {
		return -1;
	}

	HSP3Crypt *cm = GetCurrentCryptManager();

	res = 0;
	checksize = 0;
	fp=hsp3_fopenwrite( fname );
	if (fp != NULL) {

		myname = wrtstr->GetSize();			// HFPファイル名を保存
		wrtstr->PutStrBlock( fname );

		strsize = wrtstr->GetSize() & 15;
		if ( strsize > 0 ) {				// strbufを切りのいいサイズにする
			for(i=0;i<(16-strsize);i++) { wrtstr->Put( (char)0 ); }
		}

		bufsize = wrtbuf->GetSize();
		strsize = wrtstr->GetSize();
		pt_file = sizeof(HFPHED) + bufsize + strsize;
		obj = (HFPOBJ *)wrtbuf->GetBuffer();
		strbase = wrtstr->GetBuffer();

		HFPOBJ* obj_bak = (HFPOBJ *)mem_ini(bufsize);
		memcpy( obj_bak, obj, bufsize );
		for (i = 0; i < wrtnum; i++) {
			HFPOBJ* obj_save = &obj_bak[i];
			if (obj_save->flag & HFPOBJ_FLAG_HSP36) {
				obj_save->folder = 0;		//	HSP3.6互換の場合はフォルダを無効にする
			}
		}
		obj = (HFPOBJ*)wrtbuf->GetBuffer();

		hed.h1 = HFP_MAGIC1;
		hed.h2 = HFP_MAGIC2;
		hed.h3 = HFP_MAGIC3;
		hed.h4 = HFP_MAGIC4;
		hed.max_file = wrtnum;
		hed.strtable = sizeof(HFPHED) + bufsize;
		hed.filetable = pt_file;
		hed.myname = myname;
		hed.crc32 = 0;
		hed.seed = encode;
		hed.salt = cm->GetSalt(encode+ opt_encode);

		checksize = bufsize + strsize;

		fwrite( &hed, sizeof(HFPHED), 1, fp );
		fwrite( obj_bak, bufsize, 1, fp );
		fwrite( strbase, strsize, 1, fp );

		for(i=0;i<wrtnum;i++) {
			p = strbase + obj->name;
			refname[0] = 0;
			if (obj->folder == 0) {
				// no folder
			}
			else if (!hsp_pack_path_append(refname, sizeof(refname), strbase + obj->folder)) {
				res = -1;
				break;
			}
			if (!hsp_pack_path_append(refname, sizeof(refname), p)) {
				res = -1;
				break;
			}
			if (hsp_path_from_utf8(refname_hsp3, hsp_path::utf8_view(refname)) != 0) {
				res = -1;
				break;
			}

			//printf( "#%d : %x : %s ( %d bytes ) %s packing...\n", i, obj->offset, p, obj->size, refname );
			int sz = CopyFileToDPM( fp, refname_hsp3.c_str(), obj->size, obj->crypt );
			if (sz < 0) {
				res = -1;
			}
			checksize += sz;
			obj++;
		}
		hsp3_fclose(fp);
		mem_bye(obj_bak);

#ifdef HSPWIN
		_fcloseall();
#endif

	} else {
		res = -1;
	}

	delete wrtbuf;
	delete wrtstr;
	sbBye();
	return res;
}


int FilePack::ExtractFile( HFPHED *hed, const char *fname, char *savename, int encode )
{
	HFPOBJ *obj;
	FILE *ff;
	FILE *fp;
	int pt_file;
	int bufsize;
	int i;
	int a2;
	const char* sname = savename;
	std::string namebuf;
	char namebuf_utf8[HFP_PATH_MAX];

	obj = SearchFileObject( hed, fname );
	if (obj == NULL) {
		Print(format_message("#Not found [%s](%d).", fname, encode).c_str());
		return -1;
	}

	pt_file = hed->filetable;
	bufsize = (int)obj->size;

	HSP3Crypt* cm = GetCurrentCryptManager();
	namebuf_utf8[0] = 0;
	if (!hsp_pack_path_append(namebuf_utf8, sizeof(namebuf_utf8), GetFolderName(obj)) ||
		!hsp_pack_path_append(namebuf_utf8, sizeof(namebuf_utf8), GetFileName(obj))) {
		Print((char*)"#Path is too long.");
		return -1;
	}
	int enc_crypt = cm->GetCRC32(namebuf_utf8, strlen(namebuf_utf8));			// ファイルパスを暗号キーにする
	enc_crypt = cm->GetSalt(enc_crypt);
	if (enc_crypt == 0) enc_crypt = 1;

	bool locked = false;
	if (encode != 0) {
		if (encode != obj->crypt) locked = true;
	}
	else {
		if (obj->crypt) {
			if (obj->crypt != enc_crypt) locked = true;
		}
	}
	if (locked){
			Print("#Locked file.");
			return -4;
	}
	cm->DataSet(NULL, bufsize, obj->crypt);

	if (sname == NULL) {
		if (hsp_path_from_utf8(namebuf, hsp_path::utf8_view(GetFileName(obj))) != 0) return -2;
		sname = namebuf.c_str();
	}
	fp = hsp3_fopenwrite( sname );
	if ( fp == NULL ) return -2;

	ff = hsp3_fopen( GetPackName(hed), (int)obj->offset + pt_file );
	if ( ff == NULL ) return -3;

	for(i=0;i<bufsize;i++) {
		a2 = fgetc( ff );if ( a2 < 0 ) break;
		if (obj->crypt) {
			a2 = (int)cm->Decrypt((unsigned char)a2);
		}
		fputc( a2, fp );
	}

	hsp3_fclose(ff);
	hsp3_fclose(fp);
#ifdef HSPWIN
	_fcloseall();
#endif

	Print(format_message("#%s extracted.(%d)", sname, bufsize).c_str());

	return 0;
}


int FilePack::ExtractFile( const char *fname, char *savename, int encode )
{
	HFPOBJ *obj = SearchFileObject(fname);
	if (obj == NULL) {
		return -1;
	}
	return ExtractFile(GetCurrentHeader(), fname, savename, encode);
}


void FilePack::PrintFiles(void)
{
	HFPHED* hed = GetCurrentHeader();
	if (hed == NULL) {
		Print("#No files.");
		return;
	}
	int i;
	HFPOBJ* obj;
	obj = GetCurrentObjectHeader();
	for (i = 0; i < hed->max_file; i++) {
		std::string name;
		std::string foldername;
		if (hsp_path_from_utf8(name, hsp_path::utf8_view(GetFileName(obj))) != 0 ||
			hsp_path_from_utf8(foldername, hsp_path::utf8_view(GetFolderName(obj))) != 0) return;
		Print(format_message("#%d [%s%s] %d", i, foldername.c_str(), name.c_str(), (int)obj->size).c_str());
		obj++;
	}
}


void FilePack::SetErrorBuffer(CMemBuf* err)
{
	errbuf = err;
}


void FilePack::Print(const char* mes)
{
	if (errbuf) {
		errbuf->PutStr(mes);
		errbuf->PutStr("\r\n");
	}
}


int FilePack::MakeEXEFile(int mode, const char* hspexe, const char* basename, int deckey, int opt1, int opt2, int opt3 )
{
	//		EXEファイルにHFPを埋め込み
	//		mode : 0=normal
	//		       1=full screen
	//		       2=ssaver
	//		opt1,opt2 : x,y axis
	//		opt3      : disp_sw
	//
	FILE* fp, * fp2, * fp3;
	char hrtfile[HFP_PATH_MAX];
	char filename[HFP_PATH_MAX];
	char foldername[HFP_PATH_MAX];
	char sname[HFP_PATH_MAX];
	char dpmname[HFP_PATH_MAX];
	long sidx, sidx2, x0, x1;
	int a1;
	int b;
	unsigned char s4[64];
	char c;
	int* ip;
	int chksum, sum, sumseed, sumsize;

	//		HSPランタイムを検索
	//
	hrtfile[0] = 0;
	if (!hsp_pack_path_append(hrtfile, sizeof(hrtfile), hspexe)) {
		Print((char*)"#Path is too long.");
		return -1;
	}
	StrSplit(hspexe, foldername, filename);

	fp = hsp3_fopen(hrtfile);
	if (fp == NULL) {
		hrtfile[0] = 0;
		if (!hsp_pack_path_append(hrtfile, sizeof(hrtfile), foldername) ||
			!hsp_pack_path_append(hrtfile, sizeof(hrtfile), "runtime\\") ||
			!hsp_pack_path_append(hrtfile, sizeof(hrtfile), filename)) {
			Print((char*)"#Path is too long.");
			return -1;
		}
		fp = hsp3_fopen(hrtfile);
		//
		if (fp == NULL) {
			hrtfile[0] = 0;
			if (!hsp_pack_path_append(hrtfile, sizeof(hrtfile), filename)) {
				Print((char*)"#Path is too long.");
				return -1;
			}
			fp = hsp3_fopen(hrtfile);
			if (fp == NULL) {
				Print(format_message("#No file [%s].", hspexe).c_str());
				return -1;
			}
		}
	}

	//		HSPヘッダーを検索
	//
	strcpy((char*)s4, "HSPHED~~");
	b = 0; x1 = 0; sidx = 0; sidx2 = 0;
	while (1) {
		a1 = fgetc(fp); if (a1 < 0) break;
		if (a1 != s4[b]) b = 0; else b++;
		if (b == 8) sidx = x1 - 7;
		x1++;
	}
	hsp3_fclose(fp);

	if (sidx == 0) {
		Print("#Not found hsp index.");
		return -1;
	}
	Print(format_message("#Found hsp index in $%05lx/$%05lx.", sidx, x1).c_str());

	//		作成される実行ファイル名
	//
	sname[0] = 0;
	if (!hsp_pack_path_append(sname, sizeof(sname), basename)) {
		Print((char*)"#Path is too long.");
		return -1;
	}
	if (mode == 2) {
		if (!hsp_pack_path_append(sname, sizeof(sname), ".scr")) {
			Print((char*)"#Path is too long.");
			return -1;
		}
	}
	else {
		if (!hsp_pack_path_append(sname, sizeof(sname), ".exe")) {
			Print((char*)"#Path is too long.");
			return -1;
		}
	}

	//		DPMのチェックサムを作成
	//
	dpmname[0] = 0;
	if (!hsp_pack_path_append(dpmname, sizeof(dpmname), basename) ||
		!hsp_pack_path_append(dpmname, sizeof(dpmname), DPMFILEEXT)) {
		Print((char*)"#Path is too long.");
		return -1;
	}
	fp = hsp3_fopen(dpmname);
	if (fp == NULL) {
		Print(format_message("#No file [%s].", dpmname).c_str());
		return -1;
	}
	sum = 0; sumsize = 0;
	sumseed = 123;
	while (1) {
		a1 = fgetc(fp); if (a1 < 0) break;
		sum += a1 + sumseed; sumsize++;
	}
	hsp3_fclose(fp);
	chksum = sum & 0xffff;				// lower 16bit sum


	//		ヘッダ情報を書き込み
	//
	sidx2 = sidx + 36;
	for (a1 = 0; a1 < 32; a1++) { s4[a1] = 0; }
	sprintf((char*)s4 + 9, "%5ld", x1 - 0x10000);
	c = 0;
	if (mode == 1) c = 'f';
	if (mode == 2) c = 's';
	s4[17] = c;
	s4[19] = 'x'; s4[20] = opt1 & 0xff; s4[21] = (opt1 >> 8) & 0xff;
	s4[22] = 'y'; s4[23] = opt2 & 0xff; s4[24] = (opt2 >> 8) & 0xff;
	s4[25] = 'd'; s4[26] = opt3 & 0xff; s4[27] = (opt3 >> 8) & 0xff;
	s4[28] = 's'; s4[29] = chksum & 0xff; s4[30] = (chksum >> 8) & 0xff;
	s4[31] = 'k'; ip = (int*)(s4 + 32); *ip = deckey;

	fp2 = hsp3_fopen(dpmname);
	if (fp2 == NULL) {
		Print(format_message("#No file [%s].", dpmname).c_str());
		return -1;
	}
	fp = hsp3_fopen(hrtfile);
	if (fp == NULL) {
		hsp3_fclose(fp2);
		Print(format_message("#No file [%s].", hspexe).c_str());
		return -1;
	}
	fp3 = hsp3_fopenwrite(sname);
	if (fp3 == NULL) {
		hsp3_fclose(fp2); hsp3_fclose(fp);
		Print(format_message("#Write error [%s].", sname).c_str());
		return -1;
	}

	x0 = 0;
	while (1) {
		if (x0 < x1) {
			a1 = fgetc(fp);
			if (x0 >= sidx) if (x0 < sidx2) a1 = s4[x0 - sidx];
		}
		else {
			a1 = fgetc(fp2);
		}
		if (a1 < 0) break;
		fputc(a1, fp3);
		x0++;
	}

	hsp3_fclose(fp2); hsp3_fclose(fp);
	hsp3_fclose(fp3);
#ifdef HSPWIN
	_fcloseall();
#endif

	Print(format_message("Make custom execute file [%s].", sname).c_str());
	return 0;
}
