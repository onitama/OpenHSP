/*===================================================================
PositionBaseクラス
エディタ上のポジションベースクラス
===================================================================*/

#include "PositionBase.h"

/*-------------------------------------------------------------------
CPositionBase::CPositionBase
ポジションベースクラスのコンストラクタ
-------------------------------------------------------------------*/
CPositionBase::CPositionBase(){
}

/*-------------------------------------------------------------------
CPositionBase::operator = 
ポジションベースクラスのコピーオペレータ
-------------------------------------------------------------------*/
bool CPositionBase::operator = (CPositionBase &cPos){
	m_iterLine = cPos.m_iterLine;
	return true;
}

/*-------------------------------------------------------------------
CPositionBase::SetParam
パラメータセット(指定行に)
-------------------------------------------------------------------*/
bool CPositionBase::SetParam(LsLines *pLines,size_t nLine){
	/*エラーチェック*/
	if (!pLines)return false;
	if (pLines->size() <= nLine)return false;
	/*行ポインタ取得*/
	LinePt pLine = pLines->begin();
	for (size_t i=0;i<nLine;i++)pLine++;
	/*代入する*/
	m_iterLine = pLine;
	return true;
}

/*-------------------------------------------------------------------
CPositionBase::SetParam
パラメータセット
-------------------------------------------------------------------*/
void CPositionBase::SetParam(LinePt pLine){
	m_iterLine = pLine;
}

/*-------------------------------------------------------------------
CPositionBase::MoveRealNext
実際の行で次へ行く
-------------------------------------------------------------------*/
bool CPositionBase::MoveRealNext(LsLines *pLines,size_t nPos){
	if (!pLines)return false;
	LineSize nSize = pLines->size();
	for (size_t i=0;i<nPos;i++){
		if (GetLineNum() + 1 == nSize)return false;
		m_iterLine++;
	}
	return true;
}

/*-------------------------------------------------------------------
CPositionBase::MoveRealBack
実際の行で前に行く
-------------------------------------------------------------------*/
bool CPositionBase::MoveRealBack(size_t nPos){
	for (size_t i=0;i<nPos;i++){
		if (GetLineNum() == 0)return false;
		m_iterLine--;
	}
	return true;
}
