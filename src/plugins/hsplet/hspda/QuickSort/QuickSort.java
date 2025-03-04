/* HSPDAで利用するためのソートアルゴリズム
 * ソートアルゴリズムにはJava標準の修正クイックソート（Arraysクラスのsortメソッド）を使用
 * 2007/12/29 eller ver. 1.0
 * */

package QuickSort;

import java.util.Arrays;
import java.util.Vector;
import hsplet.variable.*;
import hsplet.HSPError;
import hsplet.Context;
import QuickSort.Container.*;
import QuickSort.Comparator.*;

public class QuickSort {
	private static Container[] containers = null;
	private static Vector oldIndexes = null;

	public static void sort(final Context context, final Operand op, final boolean sortmode) {

		// 変数の型によって使用するメソッドを切り替える
		switch (op.getType()) {
			case Operand.Type.INTEGER:
				sortInt(op, sortmode);
				break;
			case Operand.Type.DOUBLE:
				sortDouble(op, sortmode);
				break;
			case Operand.Type.STRING:
				sortByteString(op, sortmode);
				break;
			default:
				// 数値ではない場合エラーを返す
				context.error(HSPError.ParameterTypeMismatch, "sortval", "vartype( op )==" + op.getType());
				break;
		}

		// 以前のインデックスを代入したVectorクラスを作成
		oldIndexes = new Vector(containers.length);
		for (int i=0; i<containers.length; i++) {
			oldIndexes.addElement(new Integer(containers[i].index));
		}
	}

	/**
	 * 整数型配列変数のソート
	 * @param target ソートする数値型配列変数
	 * @param sortmode trueなら降順にソート
	 */
	private static void sortInt(final Operand target, final boolean sortmode) {

		containers = new IntContainer[target.l0()];
		for(int i=0; i<target.l0(); i++) {
			containers[i] = new IntContainer(target.toInt(i), i);
		}
		Arrays.sort(containers, new IntComparator());

		// 配列変数に結果を代入
		if (sortmode) {
			// 降順にソートする場合は順序を入れ替える
			for(int i=0; i<target.l0(); i++) {
				target.assign(target.l0() - i - 1, Scalar.fromValue(((IntContainer)containers[i]).value), 0);
			}
		} else {
			for(int i=0; i<target.l0(); i++) {
				target.assign(i, Scalar.fromValue(((IntContainer)containers[i]).value), 0);
			}
		}
	}

	/**
	 * 実数型配列変数のソート
	 * @param target 並び替える配列変数
	 * @param sortmode trueなら降順にソート
	 */
	private static void sortDouble(final Operand target, final boolean sortmode) {

		containers = new DoubleContainer[target.l0()];
		for(int i=0; i<target.l0(); i++) {
			containers[i] = new DoubleContainer(target.toDouble(i), i);
		}
		Arrays.sort(containers, new DoubleComparator());

		// 配列変数に結果を代入
		if (sortmode) {
			// 降順にソートする場合は順序を入れ替える
			for(int i=0; i<target.l0(); i++) {
				target.assign(target.l0() - i - 1, Scalar.fromValue(((DoubleContainer)containers[i]).value), 0);
			}
		} else {
			for(int i=0; i<target.l0(); i++) {
				target.assign(i, Scalar.fromValue(((DoubleContainer)containers[i]).value), 0);
			}
		}
	}

	/**
	 * 文字列型配列変数のソート
	 * @param target 並び替える配列変数
	 * @param sortmode trueなら降順にソート
	 */
	private static void sortByteString(final Operand target, final boolean sortmode) {

		containers = new ByteStringContainer[target.l0()];
		for(int i=0; i<target.l0(); i++) {
			// ByteStringクラスがCloneableでないのでStringクラスに変えたものを利用してインスタンスを生成する
			containers[i] = new ByteStringContainer(
					new ByteString(target.toByteString(i).toString()), i);
		}
		Arrays.sort(containers, new ByteStringComparator());

		// 配列変数に結果を代入
		if (sortmode) {
			// 降順にソートする場合は順序を入れ替える
			for(int i=0; i<target.l0(); i++) {
				target.assign(target.l0() - i - 1, Scalar.fromValue(((ByteStringContainer)containers[i]).value), 0);
			}
		} else {
			for(int i=0; i<target.l0(); i++) {
				target.assign(i, Scalar.fromValue(((ByteStringContainer)containers[i]).value), 0);
			}
		}
	}

	public static boolean existOldIndexes() {

		return oldIndexes != null;
	}

	public static Vector getOldIndexes() {

		return oldIndexes;
	}
}