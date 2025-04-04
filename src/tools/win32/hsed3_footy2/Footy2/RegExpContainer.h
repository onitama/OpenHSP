/*===================================================================
CRegExpContainerクラス
正規表現のクラス。子供の要素を複数持つことが可能。
===================================================================*/

#pragma once

#include "RegExpElement.h"

class CRegExpContainer : public CRegExpElement
{
public:
	CRegExpContainer();
	virtual ~CRegExpContainer();

	inline void AddRegExp(CRegExpElement *pElement){
		m_vecChildElements.push_back(pElement);
	}

protected:
	virtual bool SearchInner(LinePt pStartLine,size_t nStartPos,CEditPosition *pEnd,LinePt pEndLine);

private:
public:
protected:
	typedef std::vector<CRegExpElement*>::iterator ItElement;
	std::vector<CRegExpElement*> m_vecChildElements;
};


/*[EOF]*/
