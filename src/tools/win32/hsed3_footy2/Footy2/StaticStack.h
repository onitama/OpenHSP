/*===================================================================
CStaticStackテンプレート
スタックの数を静的に管理することが出来るクラスです。
===================================================================*/

#ifndef _STATIC_STACK_TEMPLATE_H_
#define _STATIC_STACK_TEMPLATE_H_

template <class T,int ALLOCUNIT>
class CStaticStack{
public:
	CStaticStack(){m_nSize=0;}

	T top(){
		if (!m_nSize)throw L"No Element available.";
		return m_Elements[m_nSize-1];
	}
	void push(T NewElement){
		if (m_nSize + 1 == ALLOCUNIT)throw L"Stack Overflow.";
		m_Elements[m_nSize] = NewElement;
		m_nSize++;
	}
	void pop(){
		if (!m_nSize)throw L"Stack Underflow.";
		m_nSize--;
	}
	inline size_t size(){return m_nSize;}
	
	
private:
	size_t m_nSize;
	T m_Elements[ALLOCUNIT];
};

#endif	/*_STATIC_STACK_TEMPLATE_H_*/
