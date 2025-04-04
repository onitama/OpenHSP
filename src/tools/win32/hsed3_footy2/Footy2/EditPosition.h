/*===================================================================
CEditPositionクラス
行を指し示すデータを格納することができます。
===================================================================*/

#pragma once 

#include "PositionBase.h"

class CEditPosition : public CPositionBase{
public:
	/*コンストラクタ*/
	CEditPosition();
	
	/*オペレーション(代入演算)*/
	bool operator = (CEditPosition &cPos);
	bool operator == (CEditPosition &cPos);
	bool operator != (CEditPosition &cPos);
	bool operator < (CEditPosition &cPos);
	bool operator > (CEditPosition &cPos);
	
	/*位置設定*/
	bool SetPosition(LsLines *pLines,size_t nLine,size_t nPos);
	bool SetPosition(LinePt pLine,size_t nPos);

	/*データ処理*/
	bool MoveColumnForward(LsLines *pLine,size_t nPos);
	bool MoveColumnBackward(LsLines *pLine,size_t nPos);
	bool MoveLineForwardEthic(LsLines *pLine,size_t nPos,size_t nColumn,size_t nTabLen,int nMode);
	bool MoveLineBackwardEthic(LsLines *pLine,size_t nPos,size_t nColumn,size_t nTabLen,int nMode);
	
	/*オーバーライド*/
	bool MoveRealNext(LsLines *pLine,size_t nLine);
	
	/*データ取得*/
	inline size_t GetPosition(){return m_nPosition;}

private:
	void BackupAll();

public:
private:
	size_t m_nPosition;						/*桁数(0ベース)*/
	
	/*バックアップ用*/
	size_t m_nBackupPosition;				/*バックアップポジション*/
	size_t m_nBackupLine;					/*バックアップ行番号*/
	LinePt m_iterBackupLine;				/*バックアップ行へのポインタ*/
};

/*[EOF]*/
