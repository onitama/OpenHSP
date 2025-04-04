/*===================================================================
CCharsetDetectiveクラス
文字コードの判別処理を行うクラス
===================================================================*/

#pragma once

class  CCharsetDetective{
public:
	CCharsetDetective();
	
	virtual CharSetMode GetCodepage() = 0;
	void NewCharacter(unsigned char cChar){
		if (!m_bIsImpossible)
			return NewCharInner(cChar);
	}
	inline int GetPoint(){
		if (m_bIsImpossible)return 0;
		else if (HasByteOrderMark())return 9999;
		else{
			//これだけだと変化に乏しい
			//return 100 + (int)((float)((m_nCommonLetter - m_nRareLetter) * 100 / m_nLetterCount));
			return 100 + (m_nCommonLetter - m_nRareLetter);
		}
	}


protected:	
	virtual void NewCharInner(unsigned char cChar) = 0;
	virtual bool HasByteOrderMark(){return false;}

	//! ASCII文字またはテキストにふさわしいコントロールコードか取得する処理
	inline bool IsAsciiChar(unsigned char cChar){
		return ((0x20 <= cChar && cChar <= 0x7f) ||
				 cChar == '\t' || cChar == '\r' || cChar == '\n');
	}
	
	//! よく出る文字列インクリメントする処理
	inline void IncCommon(){
		m_nCommonLetter++;
		m_nLetterCount++;
	}
	
	//! あまり出ない文字をインクリメントする処理
	inline void IncRare(){
		m_nRareLetter++;
		m_nLetterCount++;
	}
	
	//! 文字の数をインクリメントする処理
	inline void IncLetter(){
		m_nLetterCount++;
	}
	
	//! 不可能状態にする
	inline void SetImpossible(){m_bIsImpossible = true;}

private:
	bool m_bIsImpossible;			//!< これである可能性はない
	int m_nLetterCount;				//!< "文字"のカウント
	int m_nCommonLetter;			//!< よく存在する"文字"のカウント
	int m_nRareLetter;				//!< あんまりない"文字"のカウント
};

/*[EOF]*/
