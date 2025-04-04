/*===================================================================
CUndoBufferクラス
アンドゥー情報を保持するクラス
===================================================================*/

#pragma once

#include "EditPosition.h"

class CUndoBuffer{
public:
	enum UndoType{
		UNDOTYPE_INSERT,				//!< 文字列が挿入された
		UNDOTYPE_DELETE,				//!< 文字列が削除された
		UNDOTYPE_REPLACE,				//!< 文字列が置換された
		UNDOTYPE_MOVE,					//!< 移動された
	};
	
	UndoType m_nUndoType;				//!< アンドゥの情報

	/*
	 * CEditPositionを使うと、その行が削除されてしまったときに
	 * おかしくなってしまうことがあるので専用のクラスを使用します。
	 */
	class CPosInfo{
	public:
		CPosInfo();

		bool SetPosition(CEditPosition *pPos,LsLines *pLines);
		size_t operator = (CEditPosition&);

	private:
		size_t m_nLineNum;
		size_t m_nPosition;
	};

	/*位置情報*/
	CPosInfo m_cBeforeStart;
	CPosInfo m_cBeforeEnd;
	CPosInfo m_cAfterStart;
	CPosInfo m_cAfterEnd;
	
	/*文字列情報*/
	std::wstring m_strBefore;				//!< 実行前(削除された)文字列の情報
	std::wstring m_strAfter;				//!< 実行後(挿入された)文字列の情報
};

/*[EOF]*/
