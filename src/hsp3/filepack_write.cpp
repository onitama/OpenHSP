
//
//	File Pack manager 2 (Write lib)
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

#include "hsp3config.h"
#include "supio.h"
#include "filepack.h"
#include "strnote.h"
#include "strbuf.h"
#include "hsp3crypt.h"
#include "../hspcmp/membuf.h"

#define _MALLOC malloc
#define _FREE free

#define WELCOMEMSG "DPM2 Manager 1.1"
#define DPMFILEEXT ".dpm"
#define DPMENCODE_DEFVAL 0

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


int FilePack::RegisterFile(char* name, int pcrypt, int orig)
{
	//	HFPにファイルを追加
	//
	int length;
	int index;
	int enc_crypt;
	bool crypt_flag;
	HFPOBJ obj;

	char fname[HFP_PATH_MAX + 1];
	char fname_hsp3[HFP_PATH_MAX + 1];
	char foldername[HFP_PATH_MAX + 1];
	char foldername_hsp3[HFP_PATH_MAX + 1];
	char pathname[HFP_PATH_MAX + 1];

	if (orig == 0) {
		char* findptr;
		strchr3(name, '*', 0, &findptr);
		if (findptr != NULL) {
			//	ワイルドカード使用時
			CStrNote notelist;
			char ftmp[1024];
			char fixname[1024];
			char p_fdir[HSP_MAX_PATH];
			int listmax;
			char* flist = sbAlloc(0x4000);
			dirlist(name, &flist, 5);
			notelist.Select(flist);
			listmax = notelist.GetMaxLine();
			// ディレクトリを再帰する
			for (int i = 0; i < listmax; i++) {
				notelist.GetLine(ftmp, i);
				getpath(name, fixname, 32);
				strcat(fixname, ftmp);
				strcat(fixname, "/*");
				int res = RegisterFile(fixname, pcrypt);
				if (res < 0) return res;
			}
			sbFree(flist);

			// すべてのファイルを追加する
			getpath(name, p_fdir, 32);
			flist = sbAlloc(0x4000);
			dirlist(name, &flist, 1);
			notelist.Select(flist);
			listmax = notelist.GetMaxLine();
			for (int i = 0; i < listmax; i++) {
				notelist.GetLine(ftmp, i);
				strcpy(fixname, p_fdir);
				strcat(fixname, ftmp);
				int res = RegisterFile(fixname, pcrypt);
				if (res < 0) return res;
			}
			sbFree(flist);
			return 0;
		}
	}

	StrSplit(name, foldername_hsp3, fname_hsp3);		// Split Path and File Name
	StrCase(fname_hsp3);
	StrCase(foldername_hsp3);

	// UTF8に変換する
	hsp3_to_utf8(fname, fname_hsp3, HFP_PATH_MAX);
	hsp3_to_utf8(foldername, foldername_hsp3, HFP_PATH_MAX);

	strcpy(pathname, foldername);
	strcat(pathname, fname);

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
		char msg[1024];
		sprintf(msg, "#File not found [%s]", name);
		Print(msg);
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

	char msg[1024];
	sprintf(msg, "#%d %s (%d)(%d)", wrtnum, name, length, enc_crypt);
	Print(msg);

	wrtpos += length;
	wrtnum++;
	wrtbuf->PutData(&obj, sizeof(HFPOBJ));

	return length;
}


int FilePack::RegisterFromPacklist( char *name, int def_crypt)
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
			int length;
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


int FilePack::CopyFileToDPM( FILE *ff, char *filename, HFPSIZE psize, int encode )
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


int FilePack::SavePackFile( char *name, char *packname, int encode, int opt_encode )
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
	char refname_hsp3[(HFP_PATH_MAX + 1)];
	HFPOBJ* obj_bak;

	PrepareWrite( 0, encode );
	if (RegisterFromPacklist(packname, opt_encode) <= 0) {
		return -1;
	}

	strcpy( fname, name );
	strcat( fname, DPMFILEEXT);
	fname[HFP_PATH_MAX] = 0;
	StrCase( fname );

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
			if (obj->folder == 0) {
				refname[0] = 0;
			}
			else {
				strcpy(refname, strbase + obj->folder);
			}
			strcat(refname, p);
			utf8_to_hsp3(refname_hsp3, refname, HFP_PATH_MAX);

			//printf( "#%d : %x : %s ( %d bytes ) %s packing...\n", i, obj->offset, p, obj->size, refname );
			int sz = CopyFileToDPM( fp, refname_hsp3, obj->size, obj->crypt );
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


int FilePack::ExtractFile( HFPHED *hed, char *fname, char *savename, int encode )
{
	HFPOBJ *obj;
	FILE *ff;
	FILE *fp;
	int pt_file;
	int bufsize;
	int i;
	int a2;
	char* sname = savename;
	char namebuf[HFP_PATH_MAX];
	char namebuf_utf8[HFP_PATH_MAX];

	obj = SearchFileObject( hed, fname );
	if (obj == NULL) {
		char msg[1024];
		sprintf(msg, "#Not found [%s](%d).", fname, encode);
		Print(msg);
		return -1;
	}

	pt_file = hed->filetable;
	bufsize = (int)obj->size;

	HSP3Crypt* cm = GetCurrentCryptManager();
	strcpy(namebuf_utf8, GetFolderName(obj));
	strcat(namebuf_utf8, GetFileName(obj));
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
		utf8_to_hsp3(namebuf, GetFileName(obj), HFP_PATH_MAX);
		sname = namebuf;
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

	char msg[1024];
	sprintf(msg, "#%s extracted.(%d)", sname, bufsize);
	Print(msg);

	return 0;
}


int FilePack::ExtractFile( char *fname, char *savename, int encode )
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
		char msg[1024];
		char name[HFP_PATH_MAX];
		char foldername[HFP_PATH_MAX];
		utf8_to_hsp3(name, GetFileName(obj), HFP_PATH_MAX);
		utf8_to_hsp3(foldername, GetFolderName(obj), HFP_PATH_MAX);
		sprintf(msg, "#%d [%s%s] %d", i, foldername, name, (int)obj->size);
		Print(msg);
		obj++;
	}
}


void FilePack::SetErrorBuffer(CMemBuf* err)
{
	errbuf = err;
}


void FilePack::Print(char* mes)
{
	if (errbuf) {
		errbuf->PutStr(mes);
		errbuf->PutStr("\r\n");
	}
}


int FilePack::MakeEXEFile(int mode, char* hspexe, char* basename, int deckey, int opt1, int opt2, int opt3 )
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
	char tmp[1024];

	//		HSPランタイムを検索
	//
	strcpy(hrtfile, hspexe);
	StrSplit(hspexe, foldername, filename);

	fp = hsp3_fopen(hrtfile);
	if (fp == NULL) {
		sprintf(hrtfile, "%sruntime\\%s", foldername, filename);
		fp = hsp3_fopen(hrtfile);
		//
		if (fp == NULL) {
			strcpy(hrtfile, filename);
			fp = hsp3_fopen(hrtfile);
			if (fp == NULL) {
				sprintf(tmp, "#No file [%s].", hspexe);
				Print(tmp);
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
	sprintf(tmp, "#Found hsp index in $%05lx/$%05lx.", sidx, x1);
	Print(tmp);

	//		作成される実行ファイル名
	//
	strcpy(sname, basename);
	if (mode == 2) {
		strcat(sname, ".scr");
	}
	else {
		strcat(sname, ".exe");
	}

	//		DPMのチェックサムを作成
	//
	strcpy(dpmname, basename);
	strcat(dpmname, DPMFILEEXT);
	fp = hsp3_fopen(dpmname);
	if (fp == NULL) {
		sprintf(tmp, "#No file [%s].", dpmname);
		Print(tmp);
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
		sprintf(tmp, "#No file [%s].", dpmname);
		Print(tmp);
		return -1;
	}
	fp = hsp3_fopen(hrtfile);
	if (fp == NULL) {
		hsp3_fclose(fp2);
		sprintf(tmp, "#No file [%s].", hspexe);
		Print(tmp);
		return -1;
	}
	fp3 = hsp3_fopenwrite(sname);
	if (fp3 == NULL) {
		hsp3_fclose(fp2); hsp3_fclose(fp);
		sprintf(tmp, "#Write error [%s].", sname);
		Print(tmp);
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

	sprintf(tmp, "Make custom execute file [%s].", sname);
	Print(tmp);
	return 0;
}


