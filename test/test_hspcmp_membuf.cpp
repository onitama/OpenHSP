#include <assert.h>
#include <climits>

#include "../src/hspcmp/membuf.h"

int main()
{
	CMemBuf buffer;
	char* pointer = NULL;

	assert(!buffer.TryPreparePtr(-1, &pointer));
	assert(!buffer.TryPreparePtr(INT_MAX, &pointer));
	assert(!buffer.TryPreparePtr(1, NULL));
	assert(buffer.TryPreparePtr(8, &pointer));
	assert(pointer != NULL);
	assert(buffer.GetSize() == 8);

	char* original_buffer = buffer.GetBuffer();
	assert(!buffer.TryPreparePtr(INT_MAX, &pointer));
	assert(buffer.GetBuffer() == original_buffer);
	assert(buffer.GetSize() == 8);
	assert(buffer.PreparePtr(-1) == NULL);
	return 0;
}
