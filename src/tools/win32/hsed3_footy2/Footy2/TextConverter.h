/*===================================================================
CTextConverterクラス
UNICODEを基点にしたテキストの変換処理が可能なベースクラスです。
===================================================================*/

#pragma once

class  CTextConverter
{
public:
	CTextConverter();
	virtual ~CTextConverter();
	
	/*コンバートメソッド*/
	virtual bool ToUnicode(const char*,size_t nSize) = 0;
	virtual bool ToMulti(const wchar_t*,size_t nSize) = 0;
	inline const char *GetConvData(){return m_pOutputText;}
	inline size_t GetConvTextSize(){return m_nTextSize;}
	
protected:
	bool Alloc(size_t nSize);				//!< メモリ割り当て
	void Free();							//!< メモリ解放

protected:
	inline bool IsSurrogateLead(wchar_t w){return 0xD800 <= w && w <= 0xDBFF;}
	inline bool IsSurrogateTail(wchar_t w){return 0xDC00 <= w && w <= 0xDFFF;}

	char *m_pOutputText;					//!< 出力テキスト
	size_t m_nTextSize;						//!< 保持しているテキストのサイズ(NULL含む)
};

/*[EOF]*/
