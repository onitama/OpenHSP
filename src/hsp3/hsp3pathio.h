//
//	UTF-8 path and file I/O support
//
#ifndef __hsp3pathio_h
#define __hsp3pathio_h

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <string>

#include "hsp3config.h"

#if defined(HSPWIN) || defined(_WIN32)
#include <wchar.h>
#endif

namespace hsp_path {

struct utf8_tag {};
struct ansi_tag {};

template <typename Encoding>
class view {
public:
	explicit view(const char* value) : value_(value) {}

	const char* c_str() const { return value_; }

private:
	const char* value_;
};

typedef view<utf8_tag> utf8_view;
typedef view<ansi_tag> ansi_view;

#ifdef HSP_PATHIO_UTF8
typedef utf8_view path_view;
#else
typedef ansi_view path_view;
#endif

}

// Open a file whose path is a NUL-terminated UTF-8 string.
// On Windows, the path is converted to UTF-16 before calling _wfopen.
// On POSIX systems, the validated UTF-8 byte sequence is passed to fopen.
FILE* hsp_path_fopen_utf8(hsp_path::utf8_view path, const char* mode);

// Append a UTF-8 filename to an HSPTV directory. The directory may be empty
// for targets whose HSPTV files are relative to the current filesystem root.
int hsp_path_get_hsptv_path_utf8(std::string& result, hsp_path::utf8_view directory,
	hsp_path::utf8_view name);

#if defined(HSPWIN) || defined(_WIN32)
// Return the current module filename or directory as UTF-8.
int hsp_path_get_module_filename_utf8(std::string& result);
int hsp_path_get_module_filename_utf8(std::string& result, void* module);
int hsp_path_get_module_directory_utf8(std::string& result);
int hsp_path_get_current_directory_utf8(std::string& result);
int hsp_path_get_hsptv_path_utf8(std::string& result, hsp_path::utf8_view name);
int hsp_path_get_hsptv_path_utf8(std::string& result, hsp_path::ansi_view name);

// Return the same paths in the target's default internal representation.
int hsp_path_get_module_filename(std::string& result);
int hsp_path_get_module_filename(std::string& result, void* module);
int hsp_path_get_module_directory(std::string& result);
int hsp_path_get_current_directory(std::string& result);
int hsp_path_get_hsptv_path(std::string& result, hsp_path::path_view name);
#endif

// Open a file using the target's default internal path representation.
FILE* hsp_path_fopen(hsp_path::path_view path, const char* mode);

// Return the size of a regular file, or -1 when the path is invalid or unavailable.
int64_t hsp_path_filesize_utf8(hsp_path::utf8_view path);
int64_t hsp_path_filesize(hsp_path::path_view path);

// Return nonzero when a filesystem entry exists at the path.
int hsp_path_file_exists_utf8(hsp_path::utf8_view path);
int hsp_path_file_exists(hsp_path::path_view path);

// Remove a filesystem entry at the path, returning zero on success.
int hsp_path_remove_utf8(hsp_path::utf8_view path);
int hsp_path_remove(hsp_path::path_view path);

// Enumerate entries matching a path in the target's default representation.
int dirlist(const char* pattern, char** target, int flags);

// Convert a Windows ACP string at a compatibility boundary to UTF-8.
// Returns zero on success and -1 on invalid input or conversion failure.
int hsp_path_from_ansi(std::string& result, hsp_path::ansi_view path);

// Convert a path in the target's default representation to UTF-8.
int hsp_path_to_utf8(std::string& result, hsp_path::path_view path);

// Convert a UTF-8 path to the target's default representation.
int hsp_path_from_utf8(std::string& result, hsp_path::utf8_view path);

// Convert a Windows wide string at a UTF-16 boundary to UTF-8.
#if defined(HSPWIN) || defined(_WIN32)
// Returns zero on success and -1 on invalid input or conversion failure.
int hsp_path_utf8_from_wide(std::string& result, const wchar_t* text);

// Launch a process using a UTF-8 command line. Returns a WinExec-compatible
// success value (>= 32) or zero on failure.
int hsp_path_exec_utf8(hsp_path::utf8_view command);
#endif

// Convert an internal UTF-8 path to the legacy Windows ACP contract.
// Returns zero on success and -1 when the path cannot be represented.
int hsp_path_to_ansi(std::string& result, hsp_path::utf8_view path);

// Remove the extension from a UTF-8 path in place. Falls back to a manual
// strip (rather than leaving the extension untouched) when the path cannot
// be parsed as filesystem UTF-8, since silently keeping the extension could
// make a compiled output path collide with its source file.
void hsp_path_cut_extension(std::string& path);

// Return nonzero when path is an absolute filesystem path in the target's
// path convention: a leading '/' on every platform, plus a leading '\\' or
// a drive-letter prefix ("C:...") on Windows.
int hsp_path_is_absolute(const char* path);

// Extract a path component using the target's default internal string
// representation. The operation is lexical and does not access the
// filesystem. Returns false for an invalid encoding or conversion failure.
bool getpath(const std::string& source, std::string& result, int mode);

#endif
