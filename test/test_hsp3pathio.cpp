#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <type_traits>
#include <string>

#include "../src/hsp3/hsp3pathio.h"
#include "../src/hsp3/strbuf.h"

int main()
{
	static_assert(!std::is_same<hsp_path::utf8_view, hsp_path::ansi_view>::value, "UTF-8 and ANSI views must differ");

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
	sbInit();
	char* dirlist_result = sbAlloc(0x1000);
	assert(dirlist("hsp3pathio-*.tmp", &dirlist_result, 1) >= 1);
	assert(strstr(dirlist_result, path) != NULL);
	sbFree(dirlist_result);

	std::string wildcard_path(64, 'a');
	FILE* wildcard_output = hsp_path_fopen(hsp_path::path_view(wildcard_path.c_str()), "wb");
	assert(wildcard_output != NULL);
	assert(fclose(wildcard_output) == 0);
	std::string adversarial_pattern = "*";
	for (int i = 0; i < 32; ++i) adversarial_pattern += "a*";
	adversarial_pattern += "b";
	dirlist_result = sbAlloc(0x1000);
	assert(dirlist(adversarial_pattern.c_str(), &dirlist_result, 1) >= 0);
	assert(dirlist_result[0] == '\0');
	sbFree(dirlist_result);

	const char invalid_utf8[] = "hsp3pathio-\xf0\x28\x8c\x28.tmp";
	assert(hsp_path_fopen_utf8(hsp_path::utf8_view(invalid_utf8), "rb") == NULL);
	assert(!hsp_path_file_exists_utf8(hsp_path::utf8_view(invalid_utf8)));
	assert(hsp_path_filesize_utf8(hsp_path::utf8_view(invalid_utf8)) < 0);
	std::string compatibility_path;
	assert(hsp_path_from_ansi(compatibility_path,
		hsp_path::ansi_view("compatibility-日本語")) == 0);
	assert(strcmp(compatibility_path.c_str(), "compatibility-日本語") == 0);
	std::string compatibility_output;
	assert(hsp_path_to_ansi(compatibility_output,
		hsp_path::utf8_view(compatibility_path.c_str())) == 0);
	assert(strcmp(compatibility_output.c_str(), compatibility_path.c_str()) == 0);
	assert(hsp_path_from_ansi(compatibility_path, hsp_path::ansi_view("")) == 0);
	assert(compatibility_path.empty());
	assert(hsp_path_from_ansi(compatibility_path, hsp_path::ansi_view(NULL)) != 0);

#if defined(HSPWIN) || defined(_WIN32)
	const wchar_t wide_path[] = L"wide-日本語-😀";
	std::string wide_path_utf8;
	assert(hsp_path_utf8_from_wide(wide_path_utf8, wide_path) == 0);
	assert(strcmp(wide_path_utf8.c_str(), "wide-日本語-😀") == 0);
	std::string module_filename;
	assert(hsp_path_get_module_filename(module_filename) == 0);
	assert(!module_filename.empty());
	std::string module_directory;
	assert(hsp_path_get_module_directory(module_directory) == 0);
	assert(!module_directory.empty());
	std::string current_directory;
	assert(hsp_path_get_current_directory(current_directory) == 0);
	assert(!current_directory.empty());
	std::string command_line_argument;
	assert(hsp_path_get_command_line_argument_utf8(command_line_argument, 0) == 0);
	assert(!command_line_argument.empty());
	assert(hsp_path_get_command_line_argument_utf8(command_line_argument, -1) != 0);
	assert(command_line_argument.empty());
	assert(hsp_path_get_command_line_argument_utf8(command_line_argument, 9999) != 0);
	assert(command_line_argument.empty());
	std::string native_hsptv_path;
	assert(hsp_path_get_hsptv_path(native_hsptv_path,
		hsp_path::path_view("native-test.dat")) == 0);
	assert(native_hsptv_path.find("native-test.dat") != std::string::npos);
#endif

	std::string component;
	assert(getpath("dir/日本語-😀.hsp", component, 8));
	assert(component == "日本語-😀.hsp");
	assert(getpath("dir/日本語-😀.hsp", component, 1 | 8));
	assert(component == "日本語-😀");
	assert(getpath("dir/日本語-😀.hsp", component, 2));
	assert(component == ".hsp");
	assert(getpath("dir/日本語-😀.hsp", component, 32));
	assert(component == "dir/");
	std::string aliased_path = "dir/aliased.hsp";
	assert(getpath(aliased_path, component, 8));
	assert(component == "aliased.hsp");
	assert(getpath(".bashrc", component, 1 | 8));
	assert(component.empty());
	assert(getpath(".bashrc", component, 2));
	assert(component == ".bashrc");
	assert(getpath(".bashrc", component, 1));
	assert(component.empty());
	std::string leading_dot_extension = ".bashrc";
	hsp_path_cut_extension(leading_dot_extension);
	assert(leading_dot_extension.empty());
	std::string nested_leading_dot_extension = "dir/.bashrc";
	hsp_path_cut_extension(nested_leading_dot_extension);
	assert(nested_leading_dot_extension == "dir/");
	std::string hsptv_path;
	assert(hsp_path_get_hsptv_path_utf8(hsptv_path,
		hsp_path::utf8_view("/runtime/hsptv"), hsp_path::utf8_view("素材-日本語.dat")) == 0);
	assert(hsptv_path == "/runtime/hsptv/素材-日本語.dat");
	assert(hsp_path_get_hsptv_path_utf8(hsptv_path,
		hsp_path::utf8_view(""), hsp_path::utf8_view("素材-日本語.dat")) == 0);
	assert(hsptv_path == "素材-日本語.dat");
	const char invalid_hsptv_name[] = "素材-\xf0\x28\x8c\x28.dat";
	assert(hsp_path_get_hsptv_path_utf8(hsptv_path,
		hsp_path::utf8_view("/runtime/hsptv"), hsp_path::utf8_view(invalid_hsptv_name)) != 0);
	assert(getpath("C:foo.hsp", component, 8));
	assert(component == "foo.hsp");
	assert(getpath("C:", component, 32));
	assert(component == "C:");
	assert(getpath("C:dir/foo.hsp", component, 8));
	assert(component == "foo.hsp");
	assert(getpath("C:dir/foo.hsp", component, 32));
	assert(component == "C:dir/");
#if defined(HSPWIN) || defined(_WIN32)
	assert(getpath("dir\\file.hsp", component, 32));
	assert(component == "dir\\");
#endif
	assert(getpath("dir/日本語-😀.hsp", component, 8));
#ifdef HSP_PATHIO_UTF8
	assert(!getpath(invalid_utf8, component, 8));
#endif
	const char truncated_utf8_3[] = "truncated-\xe2\x82";
	const char truncated_utf8_4[] = "truncated-\xf0\x9f\x98";
#ifdef HSP_PATHIO_UTF8
	assert(!getpath(truncated_utf8_3, component, 8));
	assert(!getpath(truncated_utf8_4, component, 8));
#endif
	std::string long_path(512, 'a');
	long_path += "/long-日本語-😀.hsp";
	assert(getpath(long_path, component, 8));
	assert(component == "long-日本語-😀.hsp");

	assert(hsp_path_remove(hsp_path::path_view(path)) == 0);
	assert(hsp_path_remove(hsp_path::path_view(wildcard_path.c_str())) == 0);
	assert(!hsp_path_file_exists(hsp_path::path_view(path)));
	sbBye();
	return 0;
}
