package QuickSort.Comparator;

import QuickSort.Container.*;

public class IntComparator implements java.util.Comparator {
	public int compare(final Object o1, final Object o2) {
		return ((IntContainer)o1).value - ((IntContainer)o2).value;
	}
}