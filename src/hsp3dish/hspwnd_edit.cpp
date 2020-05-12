
//
//	HSP3 editor object manager
//	onion software/onitama 2020
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../hsp3/hsp3config.h"
#include "../hsp3/hsp3debug.h"
#include "../hsp3/dpmread.h"
#include "../hsp3/strbuf.h"
#include "../hsp3/strnote.h"

#include "hgio.h"
#include "supio.h"
#include "hspwnd.h"


/*------------------------------------------------------------*/
/*
		Object base interface
*/
/*------------------------------------------------------------*/

texmesPos::texmesPos(void)
{
	mode = TEXMES_MODE_NONE;
	length = 0;
	maxlength = TEXMESPOS_MAX;
	caret = -1;
	insert_mode = true;
	invalidate();
	for (int i = 0; i < TEXMESPOS_MAX; i++) {
		pos[i] = 0;
	}
	lastcx = 0;
	lastcy = 0;
	sx = 0;
	sy = 0;
	index_offset = 0;
	selection = NULL;
	attribute = NULL;
}


texmesPos::~texmesPos(void)
{
	deleteAttribue();
}


/*------------------------------------------------------------*/
/*
		Object interface
*/
/*------------------------------------------------------------*/

void texmesPos::invalidate(void)
{
	//	テキストの内容を再初期化
	//
	texid = -1;
	caret_cnt = 0;
	printysize = 0;
}

void texmesPos::setMaxLength(int max)
{
	//	最大文字数(byte数)を指定
	if (max <= 0) {
		maxlength = TEXMESPOS_MAX;
		return;
	}
	maxlength = max;
}

void texmesPos::setString(char *str)
{
	//	文字列を指定
	//
	msg = str;
	validateInternalString();
	invalidate();
}

int texmesPos::getPosX(int id)
{
	//	indexが示す文字のX座標を取得する
	//
	if (length == 0) return 0;
	if (id < 0) return 0;
	if (id >= length) return (int)pos[length];
	return (int)pos[id];
}

int texmesPos::getPosFromX(int x)
{
	//	X座標からindex値を求める
	//
	int i;
	if (length == 0) return 0;
	int res = 0;
	for (i = 0; i <= length; i++) {
		if ( pos[i]<=x ) res = i;
	}
	return res;
}


void texmesPos::setCaret(int id)
{
	//	キャレットのindexを指定する(強制指定)
	//
	int i = id;
	if (i < 0) i = 0;
	if (i > length) i = length;
	caret = i;
	caret_cnt = 0;
}


void texmesPos::setCaretHome(bool select)
{
	//	キャレットをhomeへ移動する
	//
	moveCaret(-TEXMESPOS_MAX,select);
}


void texmesPos::setCaretEnd(bool select)
{
	//	キャレットをendへ移動する
	//
	moveCaret(TEXMESPOS_MAX,select);
}


void texmesPos::moveCaret(int value, bool select)
{
	//	キャレット位置の相対移動
	//
	int org = caret;
	setCaret(caret + value);
	if (select == false) {
		clearSelection();
		return;
	}

	int sel_1, sel_2;
	if (getSelection(&sel_1, &sel_2)) {
		//	選択範囲を作成する
		selection[1] = caret;
	}
	else {
		//	新規の選択範囲
		setSelection( org, caret );
	}
}

void texmesPos::setCaretFromX(int x)
{
	//	X座標からキャレット位置を設定する
	//
	setCaret(getPosFromX(x));
}

int texmesPos::getCaretX(void)
{
	//	キャレットのX座標を取得する
	//
	return getPosX(caret);
}

char *texmesPos::getString(void)
{
	//	設定文字列の取得
	//
	return (char *)msg.c_str();
}

void texmesPos::addStringFromCaret(char *str)
{
	//		キャレットの位置から文字列を追加する
	//
	int cur, addcnt;

	deleteStringSelection();	//	選択範囲がある場合は消去する

	cur = getStringByteFromPos(caret);
	addcnt = validateString(str);
	if (addcnt == 0) return;

	if (insert_mode) {
		msg.insert(cur, str);
	}
	else {
		char *p = getString();
		int cur2 = getStringByteFromPos(caret+addcnt);
		p += cur2;
		msg = msg.substr(0, cur) +str + p;
	}

	caret += addcnt;
	validateInternalString();
	invalidate();
}


bool texmesPos::deleteStringSelection(void)
{
	//		選択範囲の文字列を削除する
	//
	int cur, cur2;
	int sel_1, sel_2;

	if (getSelection(&sel_1, &sel_2)) {

		setCaret(sel_1);
		cur = getStringByteFromPos(sel_1);
		cur2 = getStringByteFromPos(sel_2);
		char *p = getString();
		p += cur2;
		msg = msg.substr(0, cur) + p;

		validateInternalString();
		invalidate();
		clearSelection();
		return true;
	}
	return false;
}


int texmesPos::getSelectionString(std::string &out)
{
	//		選択範囲の文字列を取得する
	//
	int res = 0;
	int cur, cur2;
	int sel_1, sel_2;
	if (getSelection(&sel_1, &sel_2)) {
		setCaret(sel_1);
		cur = getStringByteFromPos(sel_1);
		cur2 = getStringByteFromPos(sel_2);
		res = cur2 - cur;
		out.clear();
		if (res > 0) {
			out = msg.substr(cur, res);
		}
	}
	return res;
}


void texmesPos::deleteStringFromCaret(bool backspace)
{
	//		キャレットの位置から文字列を削除する
	//
	int cur, cur2;
	if (deleteStringSelection()) return;	//	選択範囲がある場合は消去する

	if (backspace) {
		if (caret <= 0) return;
		caret--;
	}
	cur = getStringByteFromPos(caret);
	cur2 = getStringByteFromPos(caret+1);
	if (cur == cur2) return;
	char *p = getString();
	p += cur2;
	msg = msg.substr(0, cur) + p;
	validateInternalString();
	invalidate();
}


void texmesPos::toggleInsertMode(void)
{
	//		インサートモードを切り替え
	//
	insert_mode = !insert_mode;
}


int texmesPos::GetMultibyteCharacter(unsigned char *text)
{
	//		マルチバイト文字のサイズを得る
	//
	const unsigned char *p = text;
	unsigned char a1;
	int mulchr = 1;

	a1 = *p;

#ifndef HSPUTF8
	if (a1 >= 129) {				// 全角文字チェック(SJIS)
		if ((a1 <= 159) || (a1 >= 224)) {
			mulchr++;
		}
	}
#else
	if (a1 & 0x80) {				// 全角文字チェック(UTF8)
		int utf8bytes = 0;
		if ((a1 & 0xe0) == 0x0c0) utf8bytes = 1;
		if ((a1 & 0xf0) == 0x0e0) utf8bytes = 2;
		if ((a1 & 0xf8) == 0x0f0) utf8bytes = 3;

		int utf8cnt = 0;
		while (utf8bytes > 0) {
			if ((*(++p) & 0xc0) != 0x80) break;
			utf8cnt++;
			utf8bytes--;
		}
		mulchr += utf8cnt;
	}
#endif

	return mulchr;
}


void texmesPos::validateInternalString(void)
{
	//	不要な文字列を取り除く
	//	(CR/LFなどのコントロールコード)
	//
	char *src = getString();
	unsigned char *p = (unsigned char *)src;
	unsigned char a1;
	int left = maxlength;
	int ptr = 0;
	int count = 0;
	bool modstr = false;
	int mulchr;
	left--;			// null用のバッファ

	while (1) {
		a1 = *p;
		if (a1 == 0) {
			break;
		}
		if ((a1 == 13) || (a1 == 10)) {		// CR/LFの場合はそこで打ち切る
			modstr = true;
			*p = 0;
			break;
		}
		if (a1 < 32) {						// コントロールコードは空白に変換
			*p = 32;
			modstr = true;
		}
		else {
			mulchr = GetMultibyteCharacter(p);
			ptr += mulchr;
			p += mulchr;
		}
		count++;
		if (count >= left) {
			modstr = true;
			*p = 0;
			break;
		}
	}

	length = count;
	setCaret(caret);

	if (modstr) {
		msg = src;
	}
}


int texmesPos::validateString(char *str, int max)
{
	//	不要な文字列を取り除く(validateInternalStringの外部文字列用)
	//	(CR/LFなどのコントロールコード)
	//
	unsigned char *p = (unsigned char *)str;
	unsigned char a1;
	int left = max;
	int count = 0;
	int mulchr;
	if (left <= 0) { left = TEXMESPOS_MAX; }
	left--;			// null用のバッファ

	while (1) {
		a1 = *p;
		if (a1 == 0) {
			break;
		}
		if ((a1 == 13)||(a1 == 10)) {		// CR/LFの場合はそこで打ち切る
			*p = 0; break;
		}
		if (a1 < 32) {						// コントロールコードは空白に変換
			*p = 32;
		}
		else {
			mulchr = GetMultibyteCharacter(p);
			p += mulchr;
		}
		count++;
		if (count >= left) {
			*p = 0;	break;
		}
	}
	return count;
}


int texmesPos::getStringByteFromPos(int pos)
{
	//	Caretの論理位置から文字列の先頭byteを求める
	//
	unsigned char *p = (unsigned char *)getString();
	unsigned char a1;
	int mptr = 0;
	int count = pos;
	int mulchr;

	while (1) {
		if (count <= 0) break;
		a1 = *p;
		if (a1 == 0) {
			break;
		}
		mulchr = GetMultibyteCharacter(p);
		p += mulchr;
		mptr += mulchr;
		count--;
	}
	return mptr;
}


void texmesPos::deleteAttribue(void)
{
}


int texmesPos::addAttribue(int startid, int attr)
{
	return 0;
}


void texmesPos::setSize(int sizex, int sizey)
{
	sx = sizex;
	sy = sizey;
}


void texmesPos::setSelection(int *sel)
{
	selection = sel;
}


void texmesPos::setSelection(int start, int end)
{
	//		選択範囲を設定にする(caret値を指定する)
	//
	if (selection) {
		if (start < 0) {
			selection[0] = 0;
		}
		else {
			selection[0] = start + index_offset;
		}
		if (end < 0) {
			selection[1] = 0;
		}
		else {
			selection[1] = end + index_offset;
		}
	}
}


void texmesPos::clearSelection(void)
{
	//		選択範囲を無効にする
	//
	setSelection(-1, -1);
}


bool texmesPos::getSelection(int *start, int *end)
{
	//		選択範囲を返す
	//		(有効な場合はtrueを返して、start,endを設定する)
	//
	bool selok = true;
	if (selection == NULL) {
		return false;
	}
	//	選択範囲
	int sel_1, sel_2;
	sel_1 = selection[0] - index_offset;
	sel_2 = selection[1] - index_offset;
	if (sel_1 == sel_2) selok = false;
	if (sel_1 > sel_2) {
		int tmp = sel_1;
		sel_1 = sel_2; sel_2 = tmp;
	}

	if (sel_1 < 0) sel_1 = 0;
	if (sel_2 > length) sel_2 = length;
	if (sel_2 <= 0) selok = false;
	if (sel_1 > length) selok = false;

	if (selok) {
		*start = sel_1;
		*end = sel_2;
	}
	return selok;
}


void texmesPos::allSelection(void)
{
	//		すべてを選択する
	//
	if (selection == NULL) {
		return;
	}
	int orgcaret;
	orgcaret = caret;
	clearSelection();
	setCaret(0);
	setCaretEnd(true);
	setCaret(orgcaret);
}

