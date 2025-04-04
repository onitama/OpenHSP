/*===================================================================
CVSplitter
垂直スプリットバーの管理クラスです。
===================================================================*/

#pragma once

#include "SplitBase.h"						/*スプリットバーベースクラス*/

class CVSplitter : public CSplitBase{
public:
	CVSplitter();

	bool MoveWin(int x,int y,int nWidth,int nHeight);

private:
	void OnMouseMove(int x,int y);

	void OnMouseDown(int x,int y);// by Tetr@pod
	void OnMouseDouble(int x,int y);// by Tetr@pod

public:
private:
};

/*[EOF]*/
