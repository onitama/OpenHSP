/**
 * @file DllFileFunc.cpp
 * @brief ファイル操作関連のDLL関数の実装
 * @author Shinji Watanabe
 * @version 1.0
 * @note DllMain.cppから移動
 * @date Oct.01.2007
 */

#include "ConvFactory.h"
#include "CharsetDetector.h"

/**
 * TextFromFile
 * @brief 指定されたファイルポインタからファイルを読み込みます。
 * @param nID 展開するFootyのID番号
 * @param pFile 展開元ファイルポインタ
 * @param nCharSets 読み込むキャラクタセット
 */
static int TextFromFile(int nID,FILE *pFile,CharSetMode nCharSets){
	// Footyを取得する
	CFooty *pFooty = GetFooty(nID);
	if (!pFooty)return FOOTY2ERR_NOID;

	// ファイルの長さを取得する
	UINT nFileSize;
	fseek(pFile,0,SEEK_END);
	nFileSize=ftell(pFile);
	fseek(pFile,0,SEEK_SET);
	
	if (nFileSize == 0){
		// ファイルが空なら新規作成扱い
		Footy2CreateNew(nID);
	}
	else{
		// ファイルから生データを取得する
		std::vector<char> vecRawData;
		vecRawData.assign(nFileSize,'\0');
		fread(&vecRawData[0],nFileSize,1,pFile);

		// テキストコンバーターを用意する
		if (nCharSets == CSM_AUTOMATIC){
			CCharsetDetector cDetector;
			cDetector.Detect(&vecRawData[0],nFileSize);
			nCharSets = cDetector.GetCodepage();
		}
		CConvFactory cConvFactory(nCharSets);
		if (!cConvFactory.GetConv())return FOOTY2ERR_ENCODER;

		// テキストを変換して代入
		if (!cConvFactory.GetConv()->ToUnicode(&vecRawData[0],nFileSize)){
			fclose(pFile);
			return FOOTY2ERR_MEMORY;
		}
		pFooty->SetText((const wchar_t*)cConvFactory.GetConv()->GetConvData());
		pFooty->SetCharSet(cConvFactory.GetCharset());
	}
	
	// ファイルを閉じる
	fclose(pFile);
	return FOOTY2ERR_NONE;
}

/**
 * SaveToFile
 * @brief 指定されたファイルに保存する処理
 * @param nID データを持つFootyID
 * @param pFile 書き込み先ファイルポインタ
 * @param nCharSets 書き込みキャラクタセット
 * @param nLineMode 書き込み改行コード
 */
static int SaveToFile(int nID,FILE *pFile,CharSetMode nCharSets,int nLineMode){
	// Footyを取得する
	CFooty *pFooty = GetFooty(nID);
	if (!pFooty)return FOOTY2ERR_NOID;

	// 指定したFootyからデータを取得する
	int nLength = Footy2GetTextLengthW(nID,nLineMode);
	wchar_t *pTexts = new wchar_t[nLength+1];
	Footy2GetTextW(nID,pTexts,nLineMode,nLength+1);

	// テキストコンバーターを用意する
	if (nCharSets == CSM_AUTOMATIC)
		nCharSets = pFooty->GetCharSet();
	CConvFactory cConvFactory(nCharSets);
	if (!cConvFactory.GetConv())return FOOTY2ERR_ENCODER;
	if (!cConvFactory.GetConv()->ToMulti(pTexts,nLength)){
		if (pTexts){
			delete [] pTexts;
			pTexts = NULL;
		}
		fclose(pFile);
		return FOOTY2ERR_MEMORY;
	}
	fwrite(cConvFactory.GetConv()->GetConvData(),					// 変換した文字列を保存する
		   cConvFactory.GetConv()->GetConvTextSize()-sizeof(char),	// NULL文字は保存しない
		   1,pFile);												// 渡されたファイルポインタへ書き込み

	// セーブした事を保存しておく
	pFooty->m_cDoc.SetSavePoint();

	// メモリを解放する
	if (pTexts){
		delete [] pTexts;
		pTexts = NULL;
	}
	fclose(pFile);
	return FOOTY2ERR_NONE;
}

/**
 * Footy2SaveToFileW
 * @brief ファイルへ保存します
 * @param nID 読み込むFootyのID
 * @param pPath 読み込むファイル名
 * @param nCharSets キャラクタセット
 * @param nLineMode 改行コード
 */
FOOTYEXPORT(int)Footy2SaveToFileW(int nID,const wchar_t *pPath,int nCharSets,int nLineMode){
	// ファイルはバイナリモード展開
	FILE *pFile = NULL;
#ifdef UNDER_CE
	pFile = _wfopen(pPath, L"wb");
#else	/*UNDER_CE*/
	_wfopen_s(&pFile, pPath, L"wb");
#endif	/*UNDER_CE*/

	// ファイルが展開できなかった
	if (!pFile)return FOOTY2ERR_404;

	// ファイルへ保存する
	return SaveToFile(nID,pFile,(CharSetMode)nCharSets,nLineMode);
}

/**
 * Footy2TextFromFileW
 * @brief ファイルから読み込みます
 * @param pPath 読み込み元ファイルパス
 * @param nCharSets キャラクタセット
 */
FOOTYEXPORT(int)Footy2TextFromFileW(int nID,const wchar_t *pPath,int nCharSets){
	// ファイルはバイナリモード展開
	FILE *pFile = NULL;
#ifdef UNDER_CE
	pFile = _wfopen(pPath, L"rb");
#else	/*UNDER_CE*/
	_wfopen_s(&pFile, pPath, L"rb");
#endif	/*UNDER_CE*/

	// ファイルが開けなかった
	if (!pFile)return FOOTY2ERR_404;

	// ファイルを開く
	return TextFromFile(nID,pFile,(CharSetMode)nCharSets);
}

#ifndef UNDER_CE
/**
 * Footy2SaveToFileA
 * @brief ファイルへ書き込みます
 * @param pPath 書き込み先ファイルパス
 * @param nCharSets 書き込み文字コード
 * @param nLineMode 書き込み改行コード
 */
FOOTYEXPORT(int)Footy2SaveToFileA(int nID,const char *pPath,int nCharSets,int nLineMode){
	// ファイルはバイナリモード展開
	FILE *pFile = NULL;
	fopen_s(&pFile, pPath, "wb");

	// ファイルへアクセス出来ない
	if (!pFile)return FOOTY2ERR_404;

	// ファイルを保存する
	return SaveToFile(nID,pFile,(CharSetMode)nCharSets,nLineMode);
}

/**
 * Footy2TextFromFileA
 * @brief ファイルから読み込みます
 * @param nID 読み込み先のFootyID
 * @param pPath 読み込みもとファイルパス
 * @param nCharSets キャラクタセット
 */
FOOTYEXPORT(int)Footy2TextFromFileA(int nID,const char *pPath,int nCharSets){
	// ファイルはバイナリモード展開
	FILE *pFile = NULL;
	fopen_s(&pFile, pPath, "rb");

	// ファイルへアクセス出来ない
	if (!pFile)return FOOTY2ERR_404;

	// ファイルを読み込む
	return TextFromFile(nID,pFile,(CharSetMode)nCharSets);
}

#endif	/*UNDER_CE*/



/*[EOF]*/
