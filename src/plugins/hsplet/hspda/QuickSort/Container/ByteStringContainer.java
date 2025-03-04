package QuickSort.Container;

import hsplet.variable.ByteString;

final public class ByteStringContainer extends Container {
	public ByteString value;
	public ByteStringContainer(final ByteString value, final int index) {
		this.value = value;
		this.index = index;
	}
}