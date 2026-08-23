#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <type_traits>

#include "../src/hsp3/hsp3pathio.h"

static int count_matching_path(hsp_path::utf8_view name, void* user_data)
{
	int* count = (int*)user_data;
	if (strcmp(name.c_str(), "hsp3pathio-日本語-😀.tmp") == 0) ++*count;
	return 0;
}

int main()
{
	static_assert(!std::is_same<hsp_path::utf8_view, hsp_path::ansi_view>::value, "UTF-8 and ANSI views must differ");
	static_assert(!std::is_copy_constructible<hsp_path::utf8_string>::value, "Owned paths must not be copied");
	static_assert(std::is_move_constructible<hsp_path::utf8_string>::value, "Owned paths must be movable");

	const char* path = "hsp3pathio-日本語-😀.tmp";
	const char* expected = "UTF-8 path I/O\n";

	FILE* output = hsp_path_fopen(hsp_path::path_view(path), "wb");
	assert(output != NULL);
	assert(fwrite(expected, 1, strlen(expected), output) == strlen(expected));
	assert(fclose(output) == 0);
	assert(hsp_path_file_exists(hsp_path::path_view(path)));
	assert(hsp_path_filesize(hsp_path::path_view(path)) == (int64_t)strlen(expected));

	char buffer[64] = {};
	FILE* input = hsp_path_fopen(hsp_path::path_view(path), "rb");
	assert(input != NULL);
	assert(fread(buffer, 1, sizeof(buffer) - 1, input) == strlen(expected));
	assert(fclose(input) == 0);
	assert(strcmp(buffer, expected) == 0);
	int matching_paths = 0;
	assert(hsp_path_dirlist_utf8(hsp_path::utf8_view("hsp3pathio-*.tmp"), 1, count_matching_path, &matching_paths) >= 1);
	assert(matching_paths == 1);

	const char invalid_utf8[] = "hsp3pathio-\xf0\x28\x8c\x28.tmp";
	assert(hsp_path_fopen_utf8(hsp_path::utf8_view(invalid_utf8), "rb") == NULL);
	assert(!hsp_path_file_exists_utf8(hsp_path::utf8_view(invalid_utf8)));
	assert(hsp_path_filesize_utf8(hsp_path::utf8_view(invalid_utf8)) < 0);
	hsp_path::utf8_string compatibility_path = hsp_path_from_ansi(hsp_path::ansi_view("compatibility-日本語"));
	assert(compatibility_path);
	assert(strcmp(compatibility_path.c_str(), "compatibility-日本語") == 0);
	hsp_path::ansi_string compatibility_output = hsp_path_to_ansi(compatibility_path.as_view());
	assert(compatibility_output);
	assert(strcmp(compatibility_output.c_str(), compatibility_path.c_str()) == 0);

#if defined(HSPWIN) || defined(_WIN32)
	const wchar_t wide_path[] = L"wide-日本語-😀";
	hsp_path::utf8_string wide_path_utf8 = hsp_path_utf8_from_wide(wide_path);
	assert(wide_path_utf8);
	assert(strcmp(wide_path_utf8.c_str(), "wide-日本語-😀") == 0);
#endif

	char component[64];
	assert(hsp_path_getpath(hsp_path::path_view("dir/日本語-😀.hsp"), component, sizeof(component), 8));
	assert(strcmp(component, "日本語-😀.hsp") == 0);
	assert(hsp_path_getpath(hsp_path::path_view("dir/日本語-😀.hsp"), component, sizeof(component), 1 | 8));
	assert(strcmp(component, "日本語-😀") == 0);
	assert(hsp_path_getpath(hsp_path::path_view("dir/日本語-😀.hsp"), component, sizeof(component), 2));
	assert(strcmp(component, ".hsp") == 0);
	assert(hsp_path_getpath(hsp_path::path_view("dir/日本語-😀.hsp"), component, sizeof(component), 32));
	assert(strcmp(component, "dir/") == 0);
	assert(!hsp_path_getpath(hsp_path::path_view("dir/日本語-😀.hsp"), component, 4, 8));
	assert(!hsp_path_getpath_utf8(hsp_path::utf8_view(invalid_utf8), component, sizeof(component), 8));

	assert(hsp_path_remove(hsp_path::path_view(path)) == 0);
	assert(!hsp_path_file_exists(hsp_path::path_view(path)));
	return 0;
}
