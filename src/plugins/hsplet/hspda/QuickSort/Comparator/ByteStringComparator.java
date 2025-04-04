package QuickSort.Comparator;

import hsplet.variable.ByteString;
import QuickSort.Container.*;

public class ByteStringComparator implements java.util.Comparator {
	public int compare(final Object o1, final Object o2) {
		ByteString
			bs1 = ((ByteStringContainer)o1).value,
			bs2 = ((ByteStringContainer)o2).value;
		int sub = bs1.compareSub(0, bs2);

		if (sub != 0) {
			return sub;
		} else {
			return bs1.length()>bs2.length() ? 1 : 0;
		}
	}
}