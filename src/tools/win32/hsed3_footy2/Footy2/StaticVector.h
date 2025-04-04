/*===================================================================
CStaticVectorテンプレート
vectorの配列で、個数が静的なものです。
===================================================================*/

#ifndef _STATIC_VECTOR_TEMPLATE_H_
#define _STATIC_VECTOR_TEMPLATE_H_

template <class T,int ALLOCUNIT>
class CStaticVector{
public:
	CStaticVector(){m_nSize=0;}

	typedef T* iterator;

	void push_back(T NewElement){
		if (m_nSize + 1 == ALLOCUNIT)throw L"vector overflow.";
		m_Elements[m_nSize] = NewElement;
		m_nSize++;
	}
	bool operator = (CStaticVector<T,ALLOCUNIT> &cTarget){
		m_nSize = cTarget.m_nSize;
		for (size_t i=0;i<m_nSize;i++){
			m_Elements[i] = cTarget.m_Elements[i];
		}
		return m_nSize != 0;
	}
	bool operator == (CStaticVector<T,ALLOCUNIT> &cTarget){
		if (m_nSize != cTarget.m_nSize)return false;
		for (size_t i=0;i<m_nSize;i++){
			if (m_Elements[i] != cTarget.m_Elements[i])return  false;
		}
		return true;
	}
	inline T operator[] (int nIndex){return m_Element[nIndex];}
	inline iterator begin(){return &m_Elements[0];}
	inline iterator end(){return &m_Elements[m_nSize];}
	inline size_t size(){return m_nSize;}
	inline void clear(){m_nSize=0;}
	
private:
	size_t m_nSize;
	T m_Elements[ALLOCUNIT + 1];
};

#endif	/*_STATIC_VECTOR_TEMPLATE_H_*/
