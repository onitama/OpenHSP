import java.util.*;
import hsplet.HSPError;
import hsplet.function.FunctionBase;
import hsplet.variable.*;
import hsplet.Context;
import ec.util.MersenneTwister;
import QuickSort.QuickSort;;

public class hspda extends FunctionBase {

	private static Vector sortget;

	/** このクラスを含むソースファイルのバージョン文字列。 */
	private static final String fileVersionID = "$Id: hspda.java,v 0.11 2008/01/16 08:00:00 OpenHSP Exp $";
	/* v 0.11 OpenHSPへ移行・クラスファイルの構成を変更 */

	// -------------------------------------------
	// sort～系

	public static void sortget(final Context context, final Operand op, final int oi, final int index){

		if (sortget == null) {
			// sortgetが未定義の場合、システムエラー
			context.error(HSPError.SystemError, "sortget", "not sorted");
			return;
		}
		if (index < 0 || sortget.size() <= index) {
			// 未定義の範囲の場合、 エラーにはならず、常に0が返る
			op.assign(oi, Scalar.fromValue(0), 0);
		} else {
			op.assign(oi, Scalar.fromValue(objToInt(sortget.get(index))), 0);
		}
	}

	/**
	 * Object型からint型へ変換
	 * @param object int型へ変換するObject
	 * @return objectをint型へ変換した値
	 * @author eller
	 */
	private static int objToInt(final Object object) {

		return Integer.parseInt(object.toString());
	}

	/**
	 * 数値型（整数型＆実数型）配列変数をソート
	 * @param context
	 * @param op ソートする配列変数
	 * @param oi 
	 * @param sortmode 0以外なら降順にソート
	 */
	public static void sortval(final Context context, final Operand op, final int oi, final int sortmode){

		QuickSort.sort(context, op, sortmode!=0);
		if (QuickSort.existOldIndexes())
			sortget = QuickSort.getOldIndexes();
	}

	/**
	 * 文字列型配列変数をソート
	 * @param context
	 * @param op ソートする配列変数
	 * @param oi 
	 * @param sortmode 0以外なら降順にソート
	 */
	public static void sortstr(final Context context, final Operand op, final int oi, final int sortmode){
		
		QuickSort.sort(context, op, sortmode!=0);
		if (QuickSort.existOldIndexes())
			sortget = QuickSort.getOldIndexes();
	}

	/**
	 * 文字列型変数をソート
	 * @param context
	 * @param op ソートする変数
	 * @param oi 
	 * @param sortmode 0以外なら降順にソート
	 */
	public static void sortnote(final Context context, Operand op, final int oi, final int sortmode){

		// 文字列lineを改行で区切ってsaへ代入する
		String s[] = op.toString(oi).split("\\r\\n|\\n", -1);
		StringArray sa = new StringArray(64, s.length, 1, 1, 1);
		for (int i=0; i<s.length; i++) {
			sa.assign(i, Scalar.fromValue(s[i]), 0);
		}

		// ソートの実施
		QuickSort.sort(context, sa, sortmode!=0);
		if (QuickSort.existOldIndexes())
			sortget = QuickSort.getOldIndexes();

		// bsからsへ代入
		for (int i=0; i<s.length; i++) {
			s[i] = sa.toString(i);
		}

		// 改行区切りに変換
		op.assign(oi, Scalar.fromValue(Arrays.toString(s).replaceAll("\\[|\\]", "").replaceAll(", ", "\r\n")), 0);
		// 最後に空行を追加（HSP3.1の仕様）
		op.assignAdd(oi, Scalar.fromValue("\r\n"), 0);
	}

	/**
	 * CSVの１行を配列に変換
	 * @param context
	 * @param op 結果が代入される文字列型配列変数
	 * @param oi
	 * @param target CSVの１行を取り出した文字列
	 */
	public static void csvnote(final Context context, final Operand op, final int oi, final ByteString target) {

		if (op == null) {
			context.error(HSPError.ParameterCannotBeOmitted, "csvstr");
			return;
		}

		try {
			// 文字列line内の半角カンマを改行に変換してoへ代入
			op.assign(0, Scalar.fromValue(target.toString().replaceAll(",", "\n")), 0);
			context.stat.value = 0;
		} catch (Exception e) {
			context.stat.value = 1;
		}
	}

	/**
	 * CSVの１行をメモリノート変換
	 * @param context
	 * @param op  結果が代入される文字列型変数
	 * @param oi
	 * @param target CSVの１行を取り出した文字列
	 */
	public static void csvstr(final Context context, final Operand op, final int oi, final ByteString target) {

		if (op == null) {
			context.error(HSPError.ParameterCannotBeOmitted, "csvnote");
			return;
		}
		try {
			// 文字列lineをカンマで区切ってs[]に格納する
			String s[] = target.toString().split(",");
			// Operandに格納する
			for(int i=0; i<Math.min(s.length, op.l0()); i++) {
				op.assign(i, Scalar.fromValue(s[i]), 0);
			}
			context.stat.value = 0;
		} catch (Exception e) {
			context.stat.value = 1;
		}
	}

	// -------------------------------------------
	// xnote～系
	private static Operand xNote;

	public static void xnotesel(final Context context, final Operand op, final int oi){

		if (op == null) {
			context.error(HSPError.ParameterCannotBeOmitted, "xnotesel", "1st parameter cannot be omitted");
			return;
		}

		// 文字列型への変換などは行わない（HSP3.1時点での仕様）
		xNote = op.ref(oi);

		// sortgetの初期化
		sortget = new Vector();
	}

	public static void xnoteadd(final Context context, final ByteString str){

		if (str.indexOf(new ByteString("\n"), 0) != -1) {
			// add内に改行が含まれていた場合
			// HSP3.1ではエラーにならないが、厳密には誤り（hsファイルに明記）なのでエラー（パラメータの値が異常）とする
			context.error(HSPError.InvalidParameterValue, "xnoteadd", "1st parameter cannot be multi line string");
			return;
		}
		if (xNote == null) {
			// xnotesel未実行の場合
			// システムエラー
			context.error(HSPError.SystemError, "xnoteadd", "please command 'xnotesel' in advance");
			return;
		}
		if (xNote.getType() != Operand.Type.STRING) {
			// xnoteselで指定した変数が文字列型でなかった場合
			// システムエラー
			context.error(HSPError.SystemError, "xnoteadd", "vartype(xNote)==" + xNote.getType());
			return;
		}

		// 一致する文字列の行インデックス（何行目か）を調べる
		int sameStringIndex = getSameStringIndex(str);

		if (sameStringIndex < 0) {
			// 含まれていないならば最後の列に加える
			sameStringIndex = -sameStringIndex-1;
			xNote.assignAdd(0, Scalar.fromValue(str), 0);
			xNote.assignAdd(0, Scalar.fromValue("\r\n"), 0);
		}

		context.stat.value = sameStringIndex;
		try {
			sortget.setElementAt(new Integer(objToInt(sortget.get(sameStringIndex))+1), sameStringIndex);
		} catch (ArrayIndexOutOfBoundsException e) {
			setElementAtEx(sortget, sameStringIndex, 1);
		}
	}

	/**
	 * 一致する文字列の行インデックス（何行目か）を調べる
	 * 線形検索
	 * @param bs
	 * @return
	 */
	private static int getSameStringIndex(final ByteString bs) {
		int
			result = -1,
			start = 0,
			next = 0,
			lineLength = 0,
			lineNumber = 0;
		ByteString
			xnote = xNote.toByteString(0);

		while (start < xnote.length()) {
			next = xnote.nextLineIndex(start);
			lineLength = next - start;

			// lineNumber行目の文字列（改行と復帰を含む）
			ByteString line = xnote.substring(start, lineLength);
			// 改行と復帰を削除
			deleteBR(line);

			if (line.compareTo(bs) == 0) {
				// 一致する文字列が見つかった場合
				result = lineNumber;
				break;
			}
			start = next;
			lineNumber++;
		}

		// 一致する文字が見つからなかった場合、(-最後行のインデックス-1)を返す
		return result == -1 ? -lineNumber-1 : result;
	}
	/**
	 * 終端の改行と復帰を削除
	 * @param target 対象の文字列
	 */
	private static void deleteBR(ByteString target) {
		if (target.length() >= 1 && target.get(target.length() - 1) == '\n') {
			target.set(target.length() - 1, (byte) 0);
			if (target.length() >= 1 && target.get(target.length() - 1) == '\r') {
				target.set(target.length() - 1, (byte) 0);
			}
		}
	}

	/**
	 * Vectorのindex番目にIntegerクラスのインスタンスを代入
	 * @param vector 代入先であるVectorクラスのインスタンス
	 * @param index 代入先インデックス
	 * @param value 代入するIntegerクラスのインスタンスが保持する値
	 */
	private static void setElementAtEx(final Vector vector, final int index, final int value) {
		vector.ensureCapacity(index + 1);
		while (vector.size() <= index) {
			vector.add(new Integer(0));
		}
		vector.setElementAt(new Integer(value), index);
	}

	// -------------------------------------------
	// csv～系 実装中
	private static Operand csvSel;
	private static Operand csvRes;
	private static ByteString csvSeparator = new ByteString(",");
	private static int csvOption = 0;
	private static Vector csvFlags = new Vector();

	private static final class Option {

		public static final int CCSV_OPT_NOCASE = 1;
		public static final int CCSV_OPT_ANDMATCH = 2;
		public static final int CCSV_OPT_ZENKAKU = 4;
		public static final int CCSV_OPT_ADDLINE = 8;
		public static final int CCSV_OPT_EXPRESSION = 128;
	}

	/**
	 * CSV検索の対象バッファを選択
	 * @param context
	 * @param op CSV検索の対象バッファ
	 * @param oi
	 * @param sep 区切り文字
	 * @param sepi
	 */
	public static void csvsel(Context context, Operand op, final int oi, final Operand sep, final int sepi) {

		final ByteString defaultSeparator = new ByteString(",");
		if (op == null) {
			context.error(HSPError.ParameterCannotBeOmitted, "csvsel");
			return;
		}

		csvSel = op.ref(oi);
		// 区切り文字の指定（デフォルトは半角カンマ）
		byte[] separator = {(byte)toInt(sep, sepi, defaultSeparator.get(0))};
		csvSeparator = new ByteString(separator, 0, false);
	}

	/**
	 * CSV検索の結果出力バッファを指定
	 * @param context
	 * @param op 結果出力先変数
	 * @param oi
	 */
	public static void csvres(Context context, Operand op, final int oi) {

		if (op == null) {
			context.error(HSPError.ParameterCannotBeOmitted, "csvres");
			return;
		}

		csvRes = op.ref(oi);
	}

	/**
	 * CSV検索の項目フラグを設定
	 * @param id 項目ID
	 * @param val 設定値
	 */
	public static void csvflag(final int id, final int val) {

		setElementAtEx(csvFlags, id, val);
	}

	/**
	 * CSV検索のオプションを設定
	 * @param op オプション値
	 * @param oi
	 */
	public static void csvopt(final Operand op, final int oi) {

		csvOption = toInt(op, oi, 0);
	}

	/**
	 * CSV検索を実行
	 * @param context
	 * @param keyword 検索キーワード
	 * @param resMax 結果出力の最大数
	 * @param resMaxi 
	 * @param index 結果出力の開始インデックス
	 * @param indexi
	 */
	public static void csvfind(final Context context, final ByteString keyword, final Operand resMax, final int resMaxi, final Operand index, final int indexi){

		if (csvSel == null || csvRes == null) {
			// csvsel, csvresが指定されていない場合
			// HSP3.1ではエラーにならないが、厳密には異常動作なのでエラーとする
			context.error(HSPError.SystemError, "csvfind");
			return;
		}

		if (csvSel.getType() != Operand.Type.STRING || csvRes.getType() != Operand.Type.STRING) {
			// csvsel, csvresで選択した変数の型が文字列以外のとき
			context.error(HSPError.SystemError, "csvfind");
			return;
		}

		// 文字列lineを改行で区切ってkeywords[]へ代入する
		String s[] = keyword.toString().split("\\r\\n|\\n", -1);
		ByteString[] keywords = new ByteString[s.length];
		for(int i=0; i<s.length; i++) {
			keywords[i] = new ByteString(s[i]);
		}
		
		ByteString csv = csvSel.toByteString(0);
		
		int
			csvIndex = 0,
			serchedLine = 0;
		while(csvIndex < csv.length()) {
			// 1行ずつ取り出す
			boolean match = false;
			int
				lineLength = csv.nextLineIndex(csvIndex) - csvIndex,
				lineIndex = 0,
				serchedCol = 0;
			ByteString line = csv.substring(csvIndex, lineLength);
			// 改行と復帰を削除
			deleteBR(line);

			boolean[] matches = new boolean[s.length];
			for (int i=0; i<s.length; i++) {
				matches[i] = false;
			}
			while (lineIndex < line.length()) {
				// 1セルずつ取り出す
				int cellLength = line.indexOf(csvSeparator, lineIndex);
				if(cellLength == -1) {
					cellLength = line.length() - lineIndex;
				}
				ByteString cell = line.substring(lineIndex, cellLength);
				
				if(csvFlags.size() <= serchedCol || objToInt(csvFlags.get(serchedCol)) != 0) {
					// フラグが0以外ならばこのセルを調べる必要がある
					if((csvOption & Option.CCSV_OPT_ANDMATCH) != 0) {
						for (int i=0; i<s.length; i++) {
							matches[i] |= serchCell(cell, keywords[i]);
						}
					} else {
						for(int i=0; i<s.length; i++) {
							if(serchCell(cell, keywords[i])) {
								match = true;
								break;
							}
						}
					}
				}
				serchedCol++;
				lineIndex += cellLength + csvSeparator.length();
			}
			// 1行調べ終わり
			if ((csvOption & Option.CCSV_OPT_ANDMATCH) != 0) {
				match = true;
				for(int i=0; i < s.length; i++) {
					if (!matches[i]) {
						match = false;
						break;
					}
				}
			}
			
			if (match) {
				// マッチする場合はcsvResへ反映
				if ((csvOption & Option.CCSV_OPT_ADDLINE) != 0) {
					// オプション・行番号を先頭に付加
					final ByteString zero = new ByteString("0");
					ByteString header = new ByteString(new Integer(serchedLine).toString().concat(":"));
					while (header.length() < 6){
						header = ByteString.concat(zero, header);
					}
					csvRes.assignAdd(0, Scalar.fromValue("#".concat(header.toString())), 0);
				}
				csvRes.assignAdd(0, Scalar.fromValue(line), 0);
				csvRes.assignAdd(0, Scalar.fromValue("\r\n"), 0);
			}
			serchedLine++;
			csvIndex += lineLength;
		}
	}

	/**
	 * 文字列cellがkeywordを含むかどうか判別する。
	 * オプションCCSV_OPT_NOCASE・CCSV_OPT_ZENKAKUおよびCCSV_OPT_EXPRESSIONは
	 * ここで反映させること。
	 * 現バージョンではオプションCCSV_OPT_ZENKAKUおよびCCSV_OPT_EXPRESSIONは未対応。
	 * オプションCCSV_OPT_NOCASEは全角半角区別なく動作。
	 * @param cell セルの文字列
	 * @param keyword 検索するキーワード
	 * @return セルがキーワードを含むか否か
	 */
	private static boolean serchCell(final ByteString cell, final ByteString keyword){
		if((csvOption & Option.CCSV_OPT_NOCASE) != 0) {
			// 大文字小文字を同一視（全角半角区別なく動作）
			ByteString sourceString = new ByteString(cell.toString().toLowerCase());
			ByteString keywordString = new ByteString(keyword.toString().toLowerCase());
			return sourceString.indexOf(keywordString, 0) != -1;
		} else {
			return cell.indexOf(keyword, 0) != -1;
		}
	}

	// -------------------------------------------
	// rndf_～系
	//   Mersenne Twisterのライセンスが分かった（Charlotteさんに感謝!）のでそれを使用。
	//   オリジナル：http://www.cs.umd.edu/users/seanl/gp/mersenne/MersenneTwister.java
	static Random randomGenerator = new MersenneTwister();

	public static void rndf_ini(Operand seed, final int seedi){
		if(toInt(seed, 0, -1) == -1) {
			randomGenerator.setSeed(System.currentTimeMillis());
		} else {
			randomGenerator.setSeed((long)toInt(seed, 0, -1));
		}
	}

	public static void rndf_get(final Operand o, final int oi){
		
		o.assign(oi, Scalar.fromValue(randomGenerator.nextDouble()), 0);
	}

	public static void rndf_geti(final Operand o, final int oi, final int limit){
		
		o.assign(oi, Scalar.fromValue(randomGenerator.nextInt(limit)), 0);
	}

	// -------------------------------------------
	// getvar～系
	//   当分後回し
	public static void getvarid(final Context context, final Operand o, final int oi, final String varname){
		
		context.error(HSPError.UnsupportedOperation, "getvarid");
	}
	
	public static void getvarname(final Context context, final Operand o, final int oi, final int varid){
		
		context.error(HSPError.UnsupportedOperation, "getvarname");
	}
	
	public static void getmaxvar(final Context context, final Operand o, final int oi){
		
		context.error(HSPError.UnsupportedOperation, "getmaxvar");
	}

	// -------------------------------------------
	// vsave, vload系
	//   当分後回し
	public static void vsave(final Context context, final String filename){
		
		context.error(HSPError.UnsupportedOperation, "vsave");
	}
	
	public static void vload(final Context context, final String filename){
		
		context.error(HSPError.UnsupportedOperation, "vload");
	}
	
	public static void vsave_start(final Context context){
		
		context.error(HSPError.UnsupportedOperation, "vsave_start");
	}
	
	public static void vsave_put(final Context context, final Operand o, final int oi){
		
		context.error(HSPError.UnsupportedOperation, "vsave_put");
	}
	
	public static void vsave_end(final Context context, final String filename){
		
		context.error(HSPError.UnsupportedOperation, "vsave_end");
	}
	
	public static void vload_start(final Context context, final String filename){
		
		context.error(HSPError.UnsupportedOperation, "vload_start");
	}
	
	public static void vload_get(final Context context, final Operand o, final int oi){
		
		context.error(HSPError.UnsupportedOperation, "vload_get");
	}
	
	public static void vload_end(final Context context){
		
		context.error(HSPError.UnsupportedOperation, "vload_end");
	}
}