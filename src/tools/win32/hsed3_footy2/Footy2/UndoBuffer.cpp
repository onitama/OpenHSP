/*===================================================================
CUndoBufferクラス
アンドゥー情報を保持するクラス
===================================================================*/

#include "UndoBuffer.h"

/*-------------------------------------------------------------------
CPosInfo::CPosInfo
コンストラクタ
-------------------------------------------------------------------*/
CUndoBuffer::CPosInfo::CPosInfo(){
	m_nLineNum = 0;
	m_nPosition = 0;
}

/*-------------------------------------------------------------------
CPosInfo::CPosInfo
コンストラクタ
-------------------------------------------------------------------*/
size_t CUndoBuffer::CPosInfo::operator = (CEditPosition &cPos){
	m_nLineNum = cPos.GetLineNum();
	m_nPosition = cPos.GetPosition();
	return m_nLineNum;
}

/*-------------------------------------------------------------------
CPosInfo::SetPosition
コンストラクタ
-------------------------------------------------------------------*/
bool CUndoBuffer::CPosInfo::SetPosition(CEditPosition *pPos,LsLines *pLines){
	if (!pPos || !pLines)return false;
	return pPos->SetPosition(pLines,m_nLineNum,m_nPosition);
}
