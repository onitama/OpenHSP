package QuickSort.Comparator;

import QuickSort.Container.*;

public class DoubleComparator implements java.util.Comparator {
	public int compare(final Object o1, final Object o2) {
		double sub = ((DoubleContainer)o1).value - ((DoubleContainer)o2).value;
		if (sub == 0) return 0;
		return sub > 0 ? 1 : -1;
	}
}