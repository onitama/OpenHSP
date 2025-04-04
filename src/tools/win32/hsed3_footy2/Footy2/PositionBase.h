/*===================================================================
PositionBaseクラス
エディタ上のポジションベースクラス
===================================================================*/

#pragma once

#include "FootyLine.h"

class CPositionBase{
public:
	/*コンストラクタ*/
	CPositionBase();
	
	/*オペレータ*/
	virtual bool operator = (CPositionBase &cPos);
	virtual bool operator <= (CPositionBase &cPos){return GetLineNum() <= cPos.GetLineNum();}
	virtual bool operator >= (CPositionBase &cPos){return GetLineNum() >= cPos.GetLineNum();}
	virtual bool operator < (CPositionBase &cPos){return GetLineNum() < cPos.GetLineNum();}
	virtual bool operator > (CPositionBase &cPos){return GetLineNum() > cPos.GetLineNum();}
	virtual bool operator == (CPositionBase &cPos){return GetLineNum() == cPos.GetLineNum();}
	virtual bool operator != (CPositionBase &cPos){return GetLineNum() != cPos.GetLineNum();}
	
	/*取得*/
	inline size_t GetLineNum(){return m_iterLine->GetRealLineNum();}
	inline LinePt GetLinePointer(){return m_iterLine;}
	
	/*行を移動する*/
	virtual bool MoveRealNext(LsLines *pLines,size_t nPos);
	virtual bool MoveRealBack(size_t nPos);

	static void OnLineDelete(int nLineNum);

protected:	
	/*ポジションをセットします*/
	bool SetParam(LsLines *pLines,size_t nLine);
	void SetParam(LinePt pLine);

private:
	LinePt m_iterLine;						/*その位置の行*/
};

/*[EOF]*/
