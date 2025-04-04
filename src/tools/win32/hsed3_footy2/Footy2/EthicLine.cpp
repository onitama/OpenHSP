/*===================================================================
CEthicLineクラス
倫理行管理機能付きのポジションクラス
===================================================================*/

#include "EthicLine.h"

/*-------------------------------------------------------------------
CEthicLine::CEthicLine
コンストラクタ
-------------------------------------------------------------------*/
CEthicLine::CEthicLine(){
	m_nNowEthicLine = 0;
}


/*-------------------------------------------------------------------
CEthicLine::SetPosition
ポジション位置を設定
-------------------------------------------------------------------*/
bool CEthicLine::SetPosition(LsLines *pLines,size_t nLine,size_t nEthic){
	if (!SetParam(pLines,nLine))
		return false;
	if (nEthic < GetLinePointer()->GetEthicLine()){
		m_nNowEthicLine = nEthic;
		return true;
	}
	else return false;
}

/*-------------------------------------------------------------------
CEthicLine::operator = 
代入オペレータ
-------------------------------------------------------------------*/
bool CEthicLine::operator = (CEthicLine & cEthic){
	m_nNowEthicLine = cEthic.m_nNowEthicLine;
	return (*(CPositionBase*)this) = cEthic;
}

/**
 * CEthicLine::MoveEthicNext
 * @brief 倫理行のデータも含めて指定した行数進みます。
 * @param pLines 行リスト
 * @param nPos 進む数
 * @return 失敗したらfalse、成功したらtrue
 */
bool CEthicLine::MoveEthicNext(LsLines *pLines,size_t nPos)
{
	if (!pLines)return false;
	size_t nThisEthic = GetLinePointer()->GetEthicLine();
	// ループしていく
	for (size_t i=0;i<nPos;i++){
		if (m_nNowEthicLine == nThisEthic - 1){			// それが最終論理行のとき
			if (GetLineNum()+1 != pLines->size()){		// それが最終行でないとき
				MoveRealNext(pLines,1);//				// 次の行へ
				nThisEthic = GetLinePointer()->GetEthicLine();
				m_nNowEthicLine = 0;
			}
			else return false;
		}
		else m_nNowEthicLine++;
	}
	return true;
}

/*-------------------------------------------------------------------
CEthicLine::MoveEthicBack
倫理行のデータも含めて前の行へ移動します。もしもnPos分移動できない
(終端についた)場合や失敗した場合はfalseが戻ります。
-------------------------------------------------------------------*/
bool CEthicLine::MoveEthicBack(size_t nPos){
	/*ループしていく*/
	for (size_t i=0;i<nPos;i++){
		if (m_nNowEthicLine == 0){
			if (GetLineNum() != 0){
				MoveRealBack(1);
				m_nNowEthicLine = GetLinePointer()->GetEthicLine() - 1;
			}
			else return false;
		}
		else m_nNowEthicLine --;
	}
	return true;
}

/*-------------------------------------------------------------------
CEthicLine::SetPosition
ポジションをセットします。論理行は0にセットされます。
-------------------------------------------------------------------*/
bool CEthicLine::SetPosition(LinePt pLine){
	SetParam(pLine);
	m_nNowEthicLine = 0;
	return true;
}

int CEthicLine::GetDifference(CEthicLine &lnEthic,LsLines *pLines){
	int nRetValue;
	CEthicLine lnWork = *this;
	if (lnWork <= lnEthic){
		for (nRetValue=0;lnWork != lnEthic;nRetValue++){
			lnWork.MoveEthicNext(pLines,1);
		}
	}
	else{
		for (nRetValue=0;lnWork != lnEthic;nRetValue--){
			lnWork.MoveEthicBack(1);
		}
	}
	return nRetValue;
}


/*[EOF]*/
