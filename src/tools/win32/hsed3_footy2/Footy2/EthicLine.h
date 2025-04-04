/*===================================================================
CEthicLineクラス
倫理行管理機能付きのポジションクラス(行のみを管理する)
===================================================================*/

#pragma once

#include "PositionBase.h"

class CEthicLine : public CPositionBase{
public:
	/*コンストラクタ*/
	CEthicLine();
	
	/*パラメータ設定*/
	bool SetPosition(LsLines *pLines,size_t nLine,size_t nEthic);
	bool SetPosition(LinePt pLine);
	
	/*オペレータ*/
	bool operator = (CEthicLine &cEthic);
	inline bool operator != (CEthicLine &cEthic){
		return m_nNowEthicLine != cEthic.m_nNowEthicLine ||
			   GetLineNum() != cEthic.GetLineNum();
	}
	inline bool operator <= (CEthicLine &cEthic){
		if (GetLineNum() == cEthic.GetLineNum())
			return m_nNowEthicLine <= cEthic.m_nNowEthicLine;
		else
			return GetLineNum() <= cEthic.GetLineNum();
	}
	
	/*移動*/
	bool MoveEthicNext(LsLines *pLines,size_t nPos);
	bool MoveEthicBack(size_t nPos);
	
	/*取得*/
	inline size_t GetEthicNum(){return m_nNowEthicLine;}
	int GetDifference(CEthicLine &lnEthic,LsLines *pLines);

private:
	size_t m_nNowEthicLine;		/*現在の論理行*/
};

/*[EOF]*/
