/**
 * @file FootyLine.cpp
 * @brief CFootyLineの実装を行います。
 * @author Shinji Watanabe
 * @date Oct.30.2008
 */

#include "FootyLine.h"

#define USERFUNC_BUFFERSIZE 512// ユーザー定義命令・関数を入れるバッファのサイズ(文字数)

wchar_t *funclist;// 命令・関数のリスト by Tetr@pod
int *kindlist;// その種類 by Tetr@pod
int funcnum;// その数 by Tetr@pod
bool defined_funclist = false;// と、それが確保されたか by Tetr@podc
bool DrawAllUserFuncs = false;// すべての行を描画する？ by Tetr@pod
bool IsUFUpdateTiming = true;// 更新すべき時か？ by Tetr@pod

//-----------------------------------------------------------------------------
/**
 * @brief コンストラクタです。最初の行を構築します。
 */
CFootyLine::CFootyLine()
{
	m_nEthicLine = 1;
	m_nLineOffset = 0;
	m_nRealLineNum = 0;
	m_bEmphasisChached = false;
	m_nLineIcons = 0;
}

//-----------------------------------------------------------------------------
/**
 * @brief 前の行のオフセット値と倫理行数を元にこのオフセット値を設定します。
 * @param pPrevLine 前の行へのイテレータ
 * @return もしも前回と変わっていればtrueが返る
 */
bool CFootyLine::SetPrevLineInfo(LinePt pPrevLine)
{
	// バックアップ
	size_t nOffsetBefore = m_nLineOffset;
	size_t nRealBefore = m_nRealLineNum;

	// 行番号データを計算
	m_nLineOffset = pPrevLine->m_nLineOffset + pPrevLine->m_nEthicLine;
	m_nRealLineNum = pPrevLine->m_nRealLineNum + 1;

	// 前のと一緒かどうかチェックする
	return  m_nLineOffset != nOffsetBefore || m_nRealLineNum != nRealBefore;
}

//-----------------------------------------------------------------------------
/**
 * @brief 行のデータをフラッシュして各種計算を行います。
 * @param nTabLen タブ幅
 * @param nColumn 折り返し桁
 * @param nMode 折り返しモード
 * @return 論理行数が変更された場合trueが返る
 * @note データを変更したら最後にこの処理を呼び出す必要があります。
 */
bool CFootyLine::FlushString(int m_nFootyID, size_t nTabLen,size_t nColumn,int nMode, bool *NeedRedrawAll, bool ForceListUpdate/*改行を含むか by Tetr@pod*/)
{
	// 宣言
	CUrlInfo stInsert;

	// 論理行数の情報を更新する
	size_t nBeforeEthicLine = m_nEthicLine;
	m_nEthicLine = CalcEthicLine(GetLineLength(),nColumn,nTabLen,nMode).m_nEthicLine + 1;

	// if (!DrawAllUserFuncs) {
		// 目的はユーザー定義命令・関数だけなのだけど、ないと描画されない。
		// バックアップを作成してそっから読むのとどっちが効率いいだろうか。
		
		// クリッカブルURLの検索
		stInsert.m_nEndPos = -1;
		m_vecUrlInfo.clear();
		forever{
			if (!FindURL(stInsert.m_nEndPos + 1,
				&stInsert.m_nStartPos,&stInsert.m_nEndPos))break;
			m_vecUrlInfo.push_back(stInsert);
		}

		// クリッカブルメールアドレスの検索
		stInsert.m_nEndPos = -1;
		m_vecMailInfo.clear();
		forever{
			if (!FindMail(stInsert.m_nEndPos + 1,
				&stInsert.m_nStartPos,&stInsert.m_nEndPos))break;
			m_vecMailInfo.push_back(stInsert);
		}

		// クリッカブルラベルの検索 by Tetr@pod
		// 本当はコールバックとかいろいろやって、Footy2自体には汎用性を
		// 持たせるようにした方がいいんだろうけど、いかんせん技量が…
		// あっでも、汎用性がないのはクリッカブルラベルとユーザー定義命令・関数の処理だけなので、
		// 修正する際はちょっと楽かも。
		// 修正するときは、(ここだけじゃなく)他のところのグローバル変数とかも忘れずに直してください。
		stInsert.m_nEndPos = -1;
		m_vecLabelInfo.clear();
		if (UseClickableLabel) {
			FOOTY2_PRINTF(L"クリッカブルラベルの検索\n");
			forever{
				if (!FindLabel(stInsert.m_nEndPos + 1,
					&stInsert.m_nStartPos,&stInsert.m_nEndPos))break;
				m_vecLabelInfo.push_back(stInsert);
			}
		}
	// }


	// ユーザー定義命令・関数の検索(非クリッカブル) by Tetr@pod
	// ここでやってるのは、Footy2AddEmphasisよりコードを書くのが楽そうで処理速度が速そうだから。
	// (Footy2AddEmphasisだと、命令・関数名が変更された時の処理が面倒くさそう、命令・関数が消えた時とかも面倒くさそう)
	// あと、超もっさもっさ。要高速化。だれかやって。ください。
	// 本当はコールバックとかいろいろやって、Footy2自体には汎用性を
	// 持たせるようにした方がいいんだろうけど、いかんせん技量が…
	// あっでも、汎用性がないのはクリッカブルラベルとユーザー定義命令・関数の処理だけなので、
	// 修正する際はちょっと楽かも。
	// 修正するときは、(ここだけじゃなく)他のところのグローバル変数とかも忘れずに直してください。

	m_vecFuncInfo.clear();// 初期化。中じゃなくここでやるべき
	stInsert.m_nEndPos = -1;
	if (UseSearchFunction && ( IsUFUpdateTiming || DrawAllUserFuncs )) {
		// 処理中に処理を行わない(高速化のため)
		if (!DrawAllUserFuncs) {
			IsUFUpdateTiming = false;
		}

		FOOTY2_PRINTF(L"ユーザー定義命令・関数の検索\n");
		
		int len;

		len = Footy2GetTextLengthW(m_nFootyID, LM_CRLF);
		if(len > 5){// 最も短い命令・関数定義プリプロセッサの"#CMD "が
					// スペース含めて5文字、
					// ということは命令・関数の定義には最低6文字必要
					// なので総文字数5文字以下なら処理を飛ばす

			// 命令・関数定義プリプロセッサのリスト
			// 今後追加されたりなんだりしたら、こちらに追加する。
			// #defineは最後にすること
			static const wchar_t * func_define_pp[] = {
				L"deffunc", L"defcfunc",
				L"modinit", L"modterm",
				L"modfunc", L"modcfunc",
				L"func", L"cfunc",
				L"cmd",
				L"comfunc",
				L"define",// 必ず最後に書く
			};
			static const int colorkind[] = {// 色の種類(設定ウィンドウのアイテムリストと対応しています)
				0, 0, 
				1, 1, 
				2, 2, 
				3, 3, 
				4, 
				5, 
				6, // #define、下のctypeの関係で、最後に書く
				7, // #define ctype
			};
			static const bool pp_isfunction[] = {// 命令か関数か。わからなければtrueでOK。(trueで関数)
				false, true, 
				false, false, 
				false, true, 
				false, true, 
				false, 
				false, 
				false, // #define、下のctypeの関係で、最後に書く
				true, // #define ctype
			};

			bool PreprocessorFound = false;// キャレットのある行にプリプロセッサが見つかったか
			// もしキャレットのある行にプリプロセッサが見つかったら、
			// 多分命令・関数名や種類が変わっているだろうと決めつけて、
			// 命令・関数名のリストを更新する
			// (ちゃんと更新されたか確認すると、2倍くらいの時間がかかる？)

			if (!DrawAllUserFuncs) {
				if (defined_funclist && !ForceListUpdate) {// 定義済みで強制リスト更新フラグが立っていないなら
					size_t CaretLine = 0;
					if (Footy2GetCaretPosition(m_nFootyID, &CaretLine, NULL) == FOOTY2ERR_NONE) {// キャレットのある行を取得
						wchar_t *CaretLineData = (wchar_t *)Footy2GetLineW(m_nFootyID, CaretLine);// その行の内容を取得
						if (CaretLineData != NULL) {// 取得できたら
							const wchar_t *PreprocessorPosition = wcschr(CaretLineData, L'#');// #を探す
							if (PreprocessorPosition != NULL) {// 見つかったら
								for (; L'\t' == *PreprocessorPosition || L' ' == *PreprocessorPosition; PreprocessorPosition++);// まず空白を読み飛ばす

								for(int i = 0; i < sizeof(func_define_pp)/sizeof(func_define_pp[0]); i++) {// そして一致するプリプロセッサがあるか探す
									if (!ChangeColor_func && ( i == 6 || i == 7 || i == 8 ))// #func、#cfunc、#cmd を色分けしない設定の場合
										continue;
									if (!ChangeColor_define && i == 10)// #define、#define ctype を色分けしない設定の場合
										continue;
									if (wcslen(PreprocessorPosition) >= wcslen(func_define_pp[i])) {// プリプロセッサ以上の長さなら比較する
										if (wcsncmp(PreprocessorPosition + 1, func_define_pp[i], wcslen(func_define_pp[i])) == 0) {// プリプロセッサの長さ分比較して、一致してるなら
											if (
												*(PreprocessorPosition + wcslen(func_define_pp[i]) + 1) == L'\t' ||// そしてプリプロセッサの直後がタブか
												*(PreprocessorPosition + wcslen(func_define_pp[i]) + 1) == L' '// スペースなら
												) {
													PreprocessorFound = true;// プリプロセッサ発見！
													break;// これ以上やっても無駄なので抜ける
											}
										}
									}
								}
							}
						}
					}
				}
				else {// 初回か強制リスト更新フラグが立っているなら
					PreprocessorFound = true;// こうしてリストを作成する
				}

				if (PreprocessorFound) {// 見つかったら命令・関数リスト更新
					// 命令・関数探索
					FOOTY2_PRINTF(L"命令・関数のリストアップ\n");
					// 元々の命令・関数リストを解放
					if (defined_funclist) {// 定義済みなら
						defined_funclist = false;// とりあえずフラグをおろして

						// 解放
						free(funclist);
						free(kindlist);
					}

					// リストアップ
					// 宣言
					wchar_t lname[USERFUNC_BUFFERSIZE];
					wchar_t *buffer;
					wchar_t *wp;
					// wchar_t *funclist;// グローバル化(高速化のため)
					// int *kindlist;// グローバル化(高速化のため)
					int line;

					funcnum = 0;

					buffer = (wchar_t *) calloc( len + 2 /* 文字列終端と番兵 */, sizeof(wchar_t) );
					Footy2GetTextW(m_nFootyID, buffer, LM_CRLF, len + 1);

					for (int getmode=0; getmode<2; getmode++) {
						int nowindex = 0;
						line = 1;
						wp = buffer;
						for (; *wp; line++, wp++) {
							// 1行読み込み
							for(; *wp && 0x0d != *wp; wp++) {
								// 先頭の空白を無視
								if( L' ' == *wp || L'\t' == *wp ) {
									continue;
								}
								// プリプロセッサ読み飛ばし
								if( L'#' == *wp ) {
									int namelen = 0;
									do {
										for(wp++; *wp && 0x0d != *wp ; wp++) {
											if( (0 < namelen || (L' ' != *wp && L'\t' != *wp)) &&
												(L'\\' != *wp || (0x0a != wp[1] && 0x0d != wp[1])) &&
												0x0a != *wp && 0x0d != *wp &&
												namelen < sizeof(lname)/sizeof(lname[0]) - 1 ) {
												lname[namelen++] = *wp;
											}
										}
										line++;
									} while ( L'\\' == *(wp - 1) && *wp );
									line--;
									lname[namelen] = L'\0';
									//
									for(int i = 0; i < sizeof(func_define_pp)/sizeof(func_define_pp[0]); i++) {
										if (!ChangeColor_func && ( i == 6 || i == 7 || i == 8 ))// #func、#cfunc、#cmd を色分けしない設定の場合
											continue;
										if (!ChangeColor_define && i == 10)// #define、#define ctype を色分けしない設定の場合
											continue;

										int comp_len = min((int)wcslen(func_define_pp[i]), namelen);
										if( !wcsncmp(lname, func_define_pp[i], comp_len) ) {
											wchar_t *pa, *pb;
											pb = lname + comp_len;
											int kind = i;
											do {
												pa = pb;
												for(; L'\t' == *pa || L' ' == *pa; pa++, pb++);	// '#'の直後の空白をスキップ
												for(; L'\t' != *pb && L' ' != *pb && L',' != *pb && L'(' != *pb && L'@' != *pb && L'-' != *pb && L'*' != *pb && L'/' != *pb && L'\\' != *pb && L'^' != *pb && L'&' != *pb && L'|' != *pb && L';' != *pb && *pb; pb++);	// 空白までを取得 @、(、-、+、*、/、\\、^、&、|、;を追加 by Tetr@pod
												namelen = (int)(pb - pa);
												if( sizeof(lname) <= (int)(pa - lname) + namelen ) { DebugBreak(); }
												wcsncpy(lname, pa, namelen);
												/*memcpy(lname, pa, namelen * sizeof(wchar_t));*/
												lname[namelen] = L'\0';
												
												if (i == sizeof(func_define_pp)/sizeof(func_define_pp[0])-1 && _wcsicmp(lname, L"ctype") == 0 ) {// #define ctypeなら
													kind = sizeof(func_define_pp)/sizeof(func_define_pp[0]);
												}
											} while( (
												!_wcsicmp(lname, L"local") || 
												( i == sizeof(func_define_pp)/sizeof(func_define_pp[0])-1 && ( _wcsicmp(lname, L"ctype") == 0 || _wcsicmp(lname, L"global") == 0 ) )// #define の ctype、global は飛ばす
												) );
											//
											if( *lname ) {
												// reallocを使用しないようにするためこうなっています
												// 多分高速化の余地があるので、誰か修正してください＞＜
												if (getmode == 0) {// 最初はカウントアップだけ
													funcnum++;
												}
												else {// 二回目は実際に取得する
													wcscpy(funclist + nowindex * USERFUNC_BUFFERSIZE, lname);// 命令・関数名をコピー
													kindlist[nowindex] = kind;// 種類。色ではない
													nowindex++;
												}
											}
											break;
										}
									}
									break;
								}
								// コメント読み飛ばし
								if( L';' == *wp || (L'/' == *wp && L'/' == wp[1]) ) {// ;または//ではじまるコメント
									// 行末まで読み飛ばし
									for(wp++; *wp && 0x0d != *wp ; wp++);
									break;
								}
								if( L'/' == *wp && L'*' == wp[1] ) {//  / *ではじまるコメント
									for(wp++; *wp ; wp++) {
										if( L'*' == *wp && L'/' == wp[1] ) {
											wp++;
											break;
										}
										if( 0x0d == *wp ) {
											line++;
										}
									}
									continue;
								}
								// 次のステートメントまで読み飛ばし
								for(bool bEscape = false;
									*wp && 0x0d != *wp && L':' != *wp &&
									L';' != *wp && (L'/' != *wp || L'*' != wp[1]) && (L'/' != *wp || L'/' != wp[1]); wp++)
								{
									if( L'\"' == *wp && !bEscape ) {
										bool bDameChk = false;
										// 文字列読み飛ばし
										if( L'{' == *(wp - 1) ) { // 複数行文字列
											wp += 2;
											for(bool bEscape = false; *wp && (L'\"' != *wp || L'}' != wp[1] || bEscape); ) {
												if( L'\\' == *wp ) {
													bEscape = !bEscape;
												}
												wp++;
											}
										} else {
											wp++;
											for(bool bEscape = false; *wp && (L'\"' != *wp || bEscape) && 0x0d != *wp; ) {
												if( L'\\' == *wp ) {
													bEscape = !bEscape;
												}
												wp++;
											}
										}
										wp--;
										continue;
									}
								}
								if( 0x0d == *wp ) {
									break;
								}
								if( L':' != *wp ) {
									wp--;
								}
							}
							if( 0x0d == *wp && 0x0a == wp[1] ) {
								wp++;
							}
						}// 括弧の外… for (; *wp; line++, wp++)
						if (funcnum < 1) {
							break;
						}
						if (getmode == 0) {
							// 確保
							funclist = (wchar_t *)malloc(funcnum * sizeof(lname) + 4);// 念のため+4
							kindlist = (int *)malloc(funcnum * sizeof(int));

							// 失敗してないか確認
							if (funclist == NULL || kindlist == NULL) {// どちらかか両方が失敗
								if (funclist != NULL) {
									free(funclist);
									funclist = NULL;
								}
								if (kindlist != NULL) {// ちゃんと書くこと、あり得るから
									free(kindlist);
									kindlist = NULL;
								}

								funcnum = 0;// こうすることで、次の処理に進まない
								break;
							}
							else {
								defined_funclist = true;// 確保したのでフラグを立てる
							}

							// 0フィル
							// やっぱ要らない
							/*memset(funclist, 0, sizeof(funclist));
							memset(kindlist, 0, sizeof(kindlist));*/
						}
					}// 括弧の外… for (int getmode=0; getmode<2; getmode++)

					// もう使わないので解放
					free(buffer);


					if (funcnum > 0) {// 命令・関数が見つかった
						// 再登録
						// 全削除はすでにやっているので書かなくてOK
						DrawAllUserFuncs = true;// フラグを立てて

						// この関数から抜けたところでやってもらおう
						//   →CFootyView::Refreshのところでやってもらいました
						/*// すべての行を
						for (LinePt Line = GetFooty(m_nFootyID)->m_cDoc.GetTopLine(); Line != GetFooty(m_nFootyID)->m_cDoc.GetLastLine(); Line++) {
							GetFooty(m_nFootyID)->m_cDoc.SetLineInfo2(Line);// 登録する
							// Line->GetEthicLine();// なくてもいいみたい
						}
						DrawAllUserFuncs = false;// そしてフラグをおろす*/

						// 全て再描画のフラグを立てておく
						if (NeedRedrawAll) {
							*NeedRedrawAll = true;
						}
					}
				}// 括弧の外… if (PreprocessorFound)
			}// 括弧の外… if (DrawAllUserFuncs)

			if (!PreprocessorFound && funcnum > 0) {// DrawAllUserFuncsのフラグ担当のものは免除
				FOOTY2_PRINTF(L"命令・関数の検索・登録\n");

				bool IsFirst2 = false;
				for (int i=0; i<funcnum; i++) {
					bool IsFirst;
					stInsert.m_nEndPos = -1;
					forever{
						bool bIsFunction = pp_isfunction[kindlist[i]];// それは関数か
						if (!FindFunc(stInsert.m_nEndPos + 1,
							&stInsert.m_nStartPos, &stInsert.m_nEndPos, funclist + i * USERFUNC_BUFFERSIZE, bIsFunction, CustomMatchCase))break;
						// ここに来る回数が変なら、検索で失敗している可能性がある。
						// 回数が正常で描画が変なら、stInsert値が変になっている可能性がある。
						stInsert.m_nKind = colorkind[kindlist[i]];
						if (!IsFirst2) {
							m_vecFuncInfo.push_back(stInsert);// 最初の関数なら先頭からでOK(来るのは必ず昇順のものだから)
							IsFirst = true;
						}
						else {
							UrlIterator itFuncInfo;
							for(itFuncInfo = m_vecFuncInfo.begin(); itFuncInfo != m_vecFuncInfo.end(); ++itFuncInfo) {
								if ((*itFuncInfo).m_nStartPos > stInsert.m_nStartPos) {
									break;
								}
							}
							m_vecFuncInfo.insert(itFuncInfo, stInsert);// 二回目以降は正しい順番(昇順)になるように入れる必要がある
						}
					}
					IsFirst2 = IsFirst;
				}
			}
		}// 括弧の外… if (len > 5)
		else {
			funcnum = 0;
		}

		if (!DrawAllUserFuncs) {
			IsUFUpdateTiming = true;// 処理中以外のときは処理OK（高速化のため）
		}
	}// 括弧の外… if (UseSearchFunction && ( IsUFUpdateTiming || DrawAllUserFuncs ))

	return m_nEthicLine != nBeforeEthicLine;
}


//-----------------------------------------------------------------------------
/**
 * @brief nPosの位置がどこら辺にくるか計算するルーチンです。
 */
CFootyLine::EthicInfo CFootyLine::CalcEthicLine(size_t nPos,size_t nColumn,size_t nTab,int nMode) const
{
	// 宣言
	const wchar_t *pWork = GetLineData();
	EthicInfo stRet;
	// 初期化
	stRet.m_nEthicLine = 0;
	stRet.m_nEthicColumn = 0;
	// 文字列走査
	for (size_t i=0;i<nPos;i++,pWork++)
	{
		// タブ
		if (*pWork == L'\t')
		{
			for (stRet.m_nEthicColumn++;;stRet.m_nEthicColumn++)
			{
				if (stRet.m_nEthicColumn % nTab == 0)break;
			}
		}
		else
		{
			// 横二倍かどうか判定する
			bool bIsDualWidth = false;
			if (IsSurrogateLead(*pWork))	// サロゲートペアの１バイト目
			{
				bIsDualWidth = IsDualChar(*pWork,*(pWork+1));
				pWork++;i++;
			}
			else							// サロゲートペアではないとき
			{
				bIsDualWidth = IsDualChar(*pWork);
			}
			// 位置をずらす
			if (bIsDualWidth)stRet.m_nEthicColumn += 2;
			else stRet.m_nEthicColumn ++;
		}
		// 次の文字へ行きます？
		if (IsGoNext(pWork,i,stRet.m_nEthicColumn,nColumn,nMode))
		{
			stRet.m_nEthicColumn = 0;
			stRet.m_nEthicLine ++ ;
		}
	}
	// 値を返す
	return stRet;
}

//-----------------------------------------------------------------------------
/**
 * @brief 倫理行から実際の桁数を計算するルーチンです。
 * @note nEthicLineもnEthicPosも0ベースで渡します。
 */
size_t CFootyLine::CalcRealPosition(size_t nEthicLine,size_t nEthicPos, size_t nColumn,size_t nTab,int nMode) const
{
	// 宣言
	const wchar_t *pWork = GetLineData();
	size_t nLineLength = GetLineLength();
	size_t nVisPosition = 0;
	size_t nNowEthic = 0;
	size_t i;
	// 文字列走査
	for (i=0;i<nLineLength;i++,pWork++)
	{
		// 適合するか(終了？)
		if (nNowEthic == nEthicLine)
		{
			if (nVisPosition >= nEthicPos)break;
		}
		// タブ
		if (*pWork == L'\t')
		{
			for (nVisPosition++;;nVisPosition++)
			{
				if (nVisPosition % nTab == 0)break;
			}
		}
		else
		{
			// 横二倍かどうか判定する
			bool bIsDualWidth = false;
			if (IsSurrogateLead(*pWork))	// サロゲートペアの１バイト目
			{
				bIsDualWidth = IsDualChar(*pWork,*(pWork+1));
				pWork++;i++;
			}
			else							// サロゲートペアではないとき
			{
				bIsDualWidth = IsDualChar(*pWork);
			}
			// 位置をずらす
			if (bIsDualWidth)nVisPosition += 2;
			else nVisPosition ++;
		}
		// 次の行へ行きます？
		if (IsGoNext(pWork,i,nVisPosition,nColumn,nMode))
		{
			nVisPosition = 0;
			nNowEthic ++ ;
		}
	}
	// 値を返す
	return i;
}

//-----------------------------------------------------------------------------
/**
 * @brief 行の最初から指定された位置までの空白数を計算する
 * @param	nPos	[in] 計算位置
 * @return 文字数
 */
size_t CFootyLine::CalcAutoIndentPos( size_t nPos ) const
{
	size_t i = 0;
	for ( i = 0; i < nPos; i++ )
	{
		wchar_t c = m_strLineData[ i ];
		if ( c != '\t' && c != ' ' )
		{
			break;
		}
	}
	return i;
}

/*[EOF]*/
