
//
//		.hs manager class
//			onion software/onitama 2020/05
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../hsp3/hsp3config.h"
#include "hsmanager.h"

//-------------------------------------------------------------
//		Routines
//-------------------------------------------------------------

void HspHelpManager::strsp_ini(char *src)
{
	splc = 0;
	srcstr = src;
}

char *HspHelpManager::strsp_getptr(void)
{
	return srcstr + splc;
}

#ifdef HSPUTF8
int HspHelpManager::strsp_get(char splitchr)
{
	//		split string with parameters
	//
	unsigned char a1;
	unsigned char a2;
	int a;
	int utf8cnt;
	a = 0; utf8cnt = 0;

	char *dststr = spltmp;
	int len = SPLTMP_MAX;

	while (1) {
		utf8cnt = 0;
		a1 = srcstr[splc];
		if (a1 == 0) break;
		splc++;
		if (a1 >= 128) {					// 多バイト文字チェック
			if ((a1 >= 192) && (srcstr[splc + utf8cnt] != 0)) utf8cnt++;
			if ((a1 >= 224) && (srcstr[splc + utf8cnt] != 0)) utf8cnt++;
			if ((a1 >= 240) && (srcstr[splc + utf8cnt] != 0)) utf8cnt++;
			if ((a1 >= 248) && (srcstr[splc + utf8cnt] != 0)) utf8cnt++;
			if ((a1 >= 252) && (srcstr[splc + utf8cnt] != 0)) utf8cnt++;
		}

		if (a1 == splitchr) break;
		if (a1 == 13) {
			a2 = srcstr[splc];
			if (a2 == 10) splc++;
			break;
		}
		if (a1 == 10) {
			a2 = srcstr[splc];
			break;
		}
		dststr[a++] = a1;
		if (utf8cnt > 0) {
			while (utf8cnt > 0) {
				dststr[a++] = srcstr[splc++];
				utf8cnt--;
			}
		}
		if (a >= len - 5) break;
	}
	for (int i = a; i < a + 6; i++) {
		dststr[i] = 0;
	}
	return (int)a1;
}
#else
int HspHelpManager::strsp_get(char splitchr)
{
	//		split string with parameters
	//
	unsigned char a1;
	unsigned char a2;
	int a;
	int sjflg;
	a = 0; sjflg = 0;

	char *dststr = spltmp;
	int len = SPLTMP_MAX;

	while (1) {
		sjflg = 0;
		a1 = srcstr[splc];
		if (a1 == 0) break;
		splc++;
		if (a1 >= 0x81) if (a1 < 0xa0) sjflg++;
		if (a1 >= 0xe0) sjflg++;

		if (a1 == splitchr) break;
		if (a1 == 13) {
			a2 = srcstr[splc];
			if (a2 == 10) splc++;
			break;
		}
		if (a1 == 10) {
			a2 = srcstr[splc];
			break;
		}
		dststr[a++] = a1;
		if (sjflg) {
			dststr[a++] = srcstr[splc++];
		}
		if (a >= len) break;
	}
	dststr[a] = 0;
	return (int)a1;
}
#endif

//-------------------------------------------------------------
//		Interfaces
//-------------------------------------------------------------

HspHelpManager::HspHelpManager()
{
	hsbuf = NULL;
	indexbuf = NULL;
}


HspHelpManager::~HspHelpManager()
{
	terminate();
}


void HspHelpManager::terminate(void)
{
	if (hsbuf) {
		delete hsbuf; hsbuf = NULL;
	}
	if (indexbuf) {
		delete indexbuf; indexbuf = NULL;
	}

	m_pathname.clear();
	m_filename.clear();
}


int HspHelpManager::initalize(char * pathname)
{
	//		indexを初期化して.HSを使用可能にする
	//
	int res;

	terminate();

	indexbuf = new CMemBuf();
	m_pathname = pathname;
	m_filename = m_pathname + "/" + INDEX_FILE;

	m_message.clear();
	m_message = "Initalize : HspHelpManager\r\n";
	m_message += "Loading [";
	m_message += m_filename;
	m_message += "]\r\n";

	res = indexbuf->PutFile( (char *)m_filename.c_str() );
	if (res<0) {
		m_message += "Index not found.\r\n";
		delete indexbuf; indexbuf = NULL;
		return res;
	}
	note.Select( indexbuf->GetBuffer() );
	res = note.GetMaxLine() / 2;

	m_message += std::to_string(res);
	m_message += " keywords.\r\n";

	return 0;
}


int HspHelpManager::searchIndex(char * key)
{
	//		keywordをindexから検索する
	//		(返値が0の場合は成功、詳細が取得可能になる、それ以外はエラー)
	//
	int offset;
	if (indexbuf == NULL) return -1;

	m_message.clear();
	//m_message = "search : ";
	//m_message += key;
	//m_message += "\r\n";

	offset = getKeywordFromIndex(key);
	if (offset < 0) {
		m_message += "Key not found [";
		m_message += key;
		m_message += "]\r\n";
		return -2;
	}

	//m_message += "offset : ";
	//m_message += std::to_string(offset);
	//m_message += "\r\n";

	int res = openHSFile((char *)m_id_hsfile.c_str(), offset);
	if (res < 0) return -3;

	//m_message += "result : \r\n";
	//m_message += m_id_line + "\r\n";
	//m_message += m_id_hsfile + "\r\n";
	//m_message += m_id_plugin + "\r\n";
	//m_message += m_id_title + "\r\n";

	return 0;
}


int HspHelpManager::getKeywordFromIndex(char * key)
{
	//		keywordをindexから検索する
	//		(返値が0の場合は成功、それ以外はエラー)
	//
	int i, cur, max;

	m_key.clear();
	m_dll.clear();
	m_title.clear();

	if (indexbuf == NULL) return -1;
	max = note.GetMaxLine();
	cur = -1;
	i = 0;
	while (1) {
		if (i >= max) break;
		if (cur >= 0) break;
		char *p = note.GetLineDirect(i);
		if (strcmp(key, p) == 0) { cur = i; }
		note.ResumeLineDirect();
		i += 2;
	}

	if (cur < 0) {
		return -2;
	}

	m_key = key;

	char *inf = note.GetLineDirect(cur+1);
	strsp_ini(inf);
	strsp_get(','); m_id_line = spltmp;
	strsp_get(','); m_id_hsfile = spltmp;
	strsp_get(','); m_id_plugin = spltmp;
	strsp_get(','); m_id_title = spltmp;

	note.ResumeLineDirect();

	m_title = m_id_title;
	m_dll = m_id_plugin;

	cur = atoi(m_id_line.c_str());
	return cur;
}


int HspHelpManager::openHSFile(char * fname, int offset)
{
	//		.HSファイルを読み込む
	//
	int res;
	if (hsbuf) {
		delete hsbuf; hsbuf = NULL;
	}
	hsbuf = new CMemBuf();

	m_hsfilename = m_pathname + "/" + fname;

	res = hsbuf->PutFile((char *)m_hsfilename.c_str());
	if (res<=0) {
		m_message += "File not found [";
		m_message += m_hsfilename;
		m_message += "]\r\n";
		delete hsbuf; hsbuf = NULL;
		return res;
	}

	//	文字列の初期化
	m_prm.clear();
	m_group.clear();
	m_info.clear();
	m_sample.clear();
	m_ref.clear();
	m_type.clear();
	m_ver.clear();
	m_note.clear();
	m_date.clear();
	m_author.clear();
	m_url.clear();
	m_port.clear();

	if (offset>0) {
		char *buf = hsbuf->GetBuffer();
		res = getTypesFromHS( buf, buf + offset );

		hsnote.Select(buf + offset);
		return getInfoFromHS();
	}
	return 0;
}


char *HspHelpManager::getMessage(void)
{
	return (char *)m_message.c_str();
}


int HspHelpManager::getTaggedInfoFromHS(int target_line)
{
	//	読み込んだ.HS内で任意のlineから1つのtag情報を取得する
	//
	int i, max;
	bool addline;
	bool tabbed;
	bool tagline;
	char *p;

	i = target_line;

	infbuf.clear();
	tagname.clear();

	tagline = false;
	p = hsnote.GetLineDirect(i);
	if (p[0] == 0x25) {
		tagline = true;
		tagname = p + 1;
	}
	else {
		tagname = p;
	}
	hsnote.ResumeLineDirect();
	//m_message += "tag : ";
	//m_message += tagname;
	//m_message += "\r\n";

	if (tagline==false) return -1;				// %タグではない
	i++;

	max = hsnote.GetMaxLine();
	tabbed = false;

	while (1) {
		if (i >= max) break;

		addline = true;
		tagline = false;
		p = hsnote.GetLineDirect(i);
		if (p[0] == 0x25) {			// %の場合
			addline = false;
			tagline = true;
			if (strcmp(p+1, "index") == 0) i = 0;
		}
		if (p[0] == 0x3b) {			// ;の場合
			addline = false;
		}
		if (p[0] == 0x5e) {			// ^の場合
			if ((p[1] == 'p') || (p[1] == 'P')) {
				tabbed = !tabbed;
				p = " ";
			}
		}

		if (p[0]==0) addline = false;
		if (addline) {
			if (infbuf!="") infbuf += "\r\n";
			if (tabbed) infbuf += "    ";
			infbuf += p;
		}

		hsnote.ResumeLineDirect();

		if (tagline) {
			return i;
		}
		i++;
	}
	return 0;
}


int HspHelpManager::getInfoFromHS(int target_line)
{
	//	読み込んだ.HS内で任意のlineからの情報を取得する
	//
	int i;

	if (hsbuf == NULL) return -1;

	i = target_line;

	i = getTaggedInfoFromHS(i);
	if (tagname != "index") {
		m_message += "Bad Index.\r\n";
		return -2;	// 最初のタグは%indexのはず
	}

	while (1) {
		if ( i<=0 ) break;
		i = getTaggedInfoFromHS(i);
		if (tagname == "group") m_group = infbuf;
		if (tagname == "prm") m_prm = infbuf;
		if (tagname == "inst") m_info = infbuf;
		if (tagname == "sample") m_sample = infbuf;
		if (tagname == "href") m_ref = infbuf;
	}

	m_message += m_title + " \r\n\r\n";
	m_message += m_key + " " + m_prm + "\r\n";

	if (m_dll != "") {
		m_message += "\r\n( プラグイン/モジュール : ";
		m_message += m_dll + " )\r\n";
	}

	m_message += "\r\n解説 :\r\n";
	m_message += m_info + "\r\n";

	if (m_ref != "") {
		m_message += "\r\n関連項目 :\r\n";
		m_message += m_ref + "\r\n";
	}

	if (m_sample != "") {
		m_message += "\r\nサンプル :\r\n";
		m_message += m_sample + "\r\n";
	}

	m_message += "\r\n";
	m_message += "group : ";
	m_message += m_group + "\r\n";
	m_message += "type : ";
	m_message += m_type + "\r\n";
	m_message += "ver : ";
	m_message += m_ver + "\r\n";
	m_message += "note : ";
	m_message += m_note + "\r\n";
	m_message += "date : ";
	m_message += m_date + "\r\n";
	m_message += "author : ";
	m_message += m_author + "\r\n";
	m_message += "URL : ";
	m_message += m_url + "\r\n";

	return 0;
}


int HspHelpManager::getTypesFromHS(char *start, char *limit_addr)
{
	//	読み込んだ.HS内でstart～limit_addrまでの定義を取得する
	//
	int i,max;
	char *p;
	bool tagline;
	hsnote.Select(start);
	i = 0;
	max = hsnote.GetMaxLine();
	while (1) {
		if (i >= max) break;
		tagline = false;
		p = hsnote.GetLineDirect(i);
		if (p[0] == 0x25) {			// %の場合
			tagname = p + 1;
			tagline = true;
		}
		if (p >= limit_addr) {
			i = max;
			tagline = false;
		}
		hsnote.ResumeLineDirect();
		i++;
		if (tagline) {
			p = hsnote.GetLineDirect(i);
			infbuf = p;
			hsnote.ResumeLineDirect();
			i++;
			if (tagname == "type") m_type = infbuf;
			if (tagname == "ver") m_ver = infbuf;
			if (tagname == "note") m_note = infbuf;
			if (tagname == "date") m_date = infbuf;
			if (tagname == "author") m_author = infbuf;
			if (tagname == "url") m_url = infbuf;
		}
	}
	return 0;
}

