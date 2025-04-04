/*===================================================================
CRegExpElementÉNÉâÉX
ê≥ãKï\åªÇÃóvëfÉNÉâÉX
===================================================================*/

#pragma once

#include "EditPosition.h"

class CRegExpElement
{
public:
	CRegExpElement();
	virtual ~CRegExpElement();

	bool Search(LinePt pStartLine,size_t nStartPos,CEditPosition *pEnd,LinePt pEndLine);
	virtual void ResetSearch(){}

protected:
	virtual bool SearchInner(LinePt pStartLine,size_t nStartPos,CEditPosition *pEnd,LinePt pEndLine) = 0;

public:

protected:
	typedef enum _tLoopType{
		LOOPTYPE_NONE,				//!< åJÇËï‘ÇµÇ»Çµ
		LOOPTYPE_LONG,				//!< ç≈í∑àÍív
		LOOPTYPE_SHORT,				//!< ç≈íZàÍív
		LOOPTYPE_EQUALS,			//!< êîÇ∆àÍívÇµÇΩÇ∆Ç´ÇÃÇ›
	} tLoopType;
	
	tLoopType m_nLoopType;
};

/*[EOF]*/
