/*===================================================================
CEditPositionクラス
エディタ上の位置を指し示すクラス
===================================================================*/

#include "EditPosition.h"

/*-------------------------------------------------------------------
CEditPosition::CEditPosition
コンストラクタです。
-------------------------------------------------------------------*/
CEditPosition::CEditPosition(){
	m_nPosition = 0;
	m_nBackupPosition = 0;
	m_nBackupLine = 0;
}

/*-------------------------------------------------------------------
CEditPosition::operator =
代入演算
-------------------------------------------------------------------*/
bool CEditPosition::operator = (CEditPosition &cPos){
	m_nPosition = cPos.m_nPosition;
	m_nBackupPosition = cPos.m_nBackupPosition;
	m_nBackupLine = cPos.m_nBackupLine;
	m_iterBackupLine = cPos.m_iterBackupLine;
	return *(CPositionBase*)this = cPos;
}

/*-------------------------------------------------------------------
CEditPosition::operator ==
右辺と等しいかどうかを判定する
-------------------------------------------------------------------*/
bool CEditPosition::operator == (CEditPosition &cPos){
	return m_nPosition == cPos.m_nPosition &&
		GetLineNum() == cPos.GetLineNum();
}

/*-------------------------------------------------------------------
CEditPosition::operator !=
右辺と等しくないかどうかを判定する
-------------------------------------------------------------------*/
bool CEditPosition::operator != (CEditPosition &cPos){
	return m_nPosition != cPos.m_nPosition ||
		GetLineNum() != cPos.GetLineNum();
}

/*-------------------------------------------------------------------
CEditPosition::operator <
不等号判定
-------------------------------------------------------------------*/
bool CEditPosition::operator < (CEditPosition &cPos){
	if (GetLineNum() == cPos.GetLineNum())
		return m_nPosition < cPos.m_nPosition;
	else
		return GetLineNum() < cPos.GetLineNum();
}

/*-------------------------------------------------------------------
CEditPosition::operator >
不等号判定
-------------------------------------------------------------------*/
bool CEditPosition::operator > (CEditPosition &cPos){
	if (GetLineNum() == cPos.GetLineNum())
		return m_nPosition > cPos.m_nPosition;
	else
		return GetLineNum() > cPos.GetLineNum();
}

/*-------------------------------------------------------------------
CEditPosition::MoveColumnForward
nPos桁分移動させる。成功時trueです。データはバックアップされます。
-------------------------------------------------------------------*/
bool CEditPosition::MoveColumnForward(LsLines *pLine,size_t nPos){
	if (!pLine)return false;
	m_nPosition += nPos;
	/*次の行に行くかチェック*/
	if (m_nPosition > GetLinePointer()->GetLineLength()){
		if (GetLineNum() + 1 < pLine->size()){/*次の行へ*/
			MoveRealNext(pLine,1);
			m_nPosition = 0;
		}
		else{							/*次の行が存在しない*/
			m_nPosition = GetLinePointer()->GetLineLength();
		}
	}
	BackupAll();
	return true;
}

/*-------------------------------------------------------------------
CEditPosition::MoveColumnBackward
nPos桁分後ろにさせる。成功時trueです。データはバックアップされます。
-------------------------------------------------------------------*/
bool CEditPosition::MoveColumnBackward(LsLines *pLines,size_t nPos){
	if (!pLines)return false;
	/*前の行に行くかチェック*/
	if (m_nPosition < nPos){
		if (GetLineNum() != 0){			/*前の行へ*/
			CPositionBase::MoveRealBack(1);
			m_nPosition = GetLinePointer()->GetLineLength();
		}
		else{							/*前の行が存在しない*/
			m_nPosition = 0;
		}
	}
	else m_nPosition -= nPos;
	BackupAll();
	return true;
}

/*-------------------------------------------------------------------
CEditPosition::SetPosition
代入演算(nLine行目nPos桁目に移動する、両方0ベース)です。
データはバックアップされます。
-------------------------------------------------------------------*/
bool CEditPosition::SetPosition(LsLines *pLines,size_t nLine,size_t nPos){
	if (!SetParam(pLines,nLine))					/*行をセットする*/
		return false;
	if (GetLinePointer()->GetLineLength() < nPos)	/*桁が不正でないかチェックする*/
		return false;
	m_nPosition = nPos;								/*桁をセットする*/
	BackupAll();									/*データを全てバックアップする*/
	return true;
}

/*-------------------------------------------------------------------
CEditPosition::SetPosition
データをバックアップしつつ
-------------------------------------------------------------------*/
bool CEditPosition::SetPosition(LinePt pLine,size_t nPos){
	SetParam(pLine);
	if (GetLinePointer()->GetLineLength() < nPos)	/*桁が不正でないかチェックする*/
		return false;
	m_nPosition = nPos;								/*桁をセットする*/
	BackupAll();
	return true;
}


/*-------------------------------------------------------------------
CEditPosition::MoveRealNext
(倫理行ではなく)行を先に進めます。データはバックアップされます。
-------------------------------------------------------------------*/
bool CEditPosition::MoveRealNext(LsLines *pLine,size_t nLine){
	if (!pLine)return false;
	/*先へすすめる*/
	if (!CPositionBase::MoveRealNext(pLine,nLine)){
		m_nPosition = GetLinePointer()->GetLineLength();
		BackupAll();
		return false;
	}
	else{
		m_nPosition = 0;
		BackupAll();
		return true;
	}
}

/*-------------------------------------------------------------------
CEditPosition::MoveLineForwardEthic
バックアップした行数、桁数を元に一論理行下へ移動します。
データはバックアップされません。
-------------------------------------------------------------------*/
bool CEditPosition::MoveLineForwardEthic(LsLines *pLine,size_t nPos,
										 size_t nColumn,size_t nTabLen,int nMode){
	if (!pLine)return false;
	/*宣言*/
	CFootyLine::EthicInfo cBackupInfo =
		m_iterBackupLine->CalcEthicLine(m_nBackupPosition,nColumn,nTabLen,nMode);
	CFootyLine::EthicInfo cNowInfo =
		GetLinePointer()->CalcEthicLine(m_nPosition,nColumn,nTabLen,nMode);
	/*一番下の論理行か？*/
	if (GetLinePointer()->GetEthicLine() == cNowInfo.m_nEthicLine + 1){
		/*次の行へ行く、次の行があるか？*/
		if (GetLineNum() + 1 != pLine->size()){
			CPositionBase::MoveRealNext(pLine,1);		/*次の行へ*/
			m_nPosition = GetLinePointer()->CalcRealPosition(0,cBackupInfo.m_nEthicColumn,nColumn,nTabLen,nMode);
		}
		else	/*無いならばその行の最後の桁へ*/
			m_nPosition = GetLinePointer()->GetLineLength();
	}
	else{
		/*その行の次の論理行へ*/
		m_nPosition = GetLinePointer()->CalcRealPosition
			(cNowInfo.m_nEthicLine + 1,cBackupInfo.m_nEthicColumn,nColumn,nTabLen,nMode);
	}
	return true;
}

/*-------------------------------------------------------------------
CEditPosition::MoveLineBackwardEthic
バックアップした行数、桁数を元に一論理行上へ移動します。
データはバックアップされません。
-------------------------------------------------------------------*/
bool CEditPosition::MoveLineBackwardEthic(LsLines *pLines,size_t nPos,
										  size_t nColumn,size_t nTabLen,int nMode){
	if (!pLines)return false;
	/*宣言*/
	CFootyLine::EthicInfo cBackupInfo =
		m_iterBackupLine->CalcEthicLine(m_nBackupPosition,nColumn,nTabLen,nMode);
	CFootyLine::EthicInfo cNowInfo =
		GetLinePointer()->CalcEthicLine(m_nPosition,nColumn,nTabLen,nMode);
	/*一番下の論理行か？*/
	if (cNowInfo.m_nEthicLine == 0){
		/*前の行へ行く、前の行があるか？*/
		if (GetLineNum() != 0){
			CPositionBase::MoveRealBack(1);	/*前の行へ*/
			m_nPosition = GetLinePointer()->CalcRealPosition(GetLinePointer()->
				GetEthicLine()-1,cBackupInfo.m_nEthicColumn,nColumn,nTabLen,nMode);
		}
		else				/*無いならば最初の桁へ*/
			m_nPosition = 0;
	}
	else{
		/*その行の次の論理行へ*/
		m_nPosition = GetLinePointer()->CalcRealPosition
			(cNowInfo.m_nEthicLine - 1,cBackupInfo.m_nEthicColumn,nColumn,nTabLen,nMode);
	}
	return true;
}

/*-------------------------------------------------------------------
CEditPosition::BackupAll
上下キーを押したときに押し始めた位置の桁数を中心にキャレット移動が
起こるようにするために全てのデータをメンバ変数にバックアップします。
-------------------------------------------------------------------*/
void CEditPosition::BackupAll(){
	m_nBackupPosition = m_nPosition;
	m_nBackupLine = GetLineNum();
	m_iterBackupLine = GetLinePointer();
}
