#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <cstdint>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <string>
#include <vector>

using HspInt = intptr_t;
using PathFn = int(WINAPI *)(void *, char *, HspInt, HspInt);
using MessageFn = int(WINAPI *)(char *, HspInt, HspInt, HspInt);
using IntFn = int(WINAPI *)(HspInt, HspInt, HspInt, HspInt);

static FARPROC load_export(HMODULE module, const char *name) {
#ifdef _WIN64
    return GetProcAddress(module, name);
#else
    std::string decorated = "_" + std::string(name) + "@16";
    return GetProcAddress(module, decorated.c_str());
#endif
}

static bool to_acp(const wchar_t *input, std::vector<char> &output) {
    BOOL used_default = FALSE;
    int size = WideCharToMultiByte(CP_ACP, WC_NO_BEST_FIT_CHARS, input, -1,
                                   nullptr, 0, nullptr, &used_default);
    if (size <= 0 || used_default) return false;
    output.resize(static_cast<size_t>(size));
    used_default = FALSE;
    return WideCharToMultiByte(CP_ACP, WC_NO_BEST_FIT_CHARS, input, -1,
                               output.data(), size, nullptr, &used_default) > 0 &&
           !used_default;
}

static int fail(const char *message, int code) {
    std::fprintf(stderr, "WORKER_ERROR %s\n", message);
    return code;
}

static int worker_main(int argc, wchar_t **argv) {
    if (argc == 2 && std::wstring(argv[1]) == L"self-crash") {
        RaiseException(0xE0000001, 0, 0, nullptr);
    }
    if (argc < 4) {
        return fail("usage: dll_worker compile|pack DLL ...", 2);
    }

    HMODULE module = LoadLibraryW(argv[2]);
    if (!module) return fail("LoadLibraryW failed", 3);

    auto path_fn = [&](const char *name) -> PathFn {
        return reinterpret_cast<PathFn>(load_export(module, name));
    };
    auto int_fn = [&](const char *name) -> IntFn {
        return reinterpret_cast<IntFn>(load_export(module, name));
    };
    PathFn hsc_ini = path_fn("hsc_ini");
    PathFn hsc_compath = path_fn("hsc_compath");
    PathFn hsc_objname = path_fn("hsc_objname");
    IntFn hsc_comp = int_fn("hsc_comp");
    MessageFn hsc_getmes = reinterpret_cast<MessageFn>(load_export(module, "hsc_getmes"));
    IntFn hsc_bye = int_fn("hsc_bye");
    PathFn pack_ini = path_fn("pack_ini");
    IntFn pack_make = int_fn("pack_make");
    IntFn pack_view = int_fn("pack_view");
    PathFn pack_get = path_fn("pack_get");
    if (!hsc_ini || !hsc_compath || !hsc_objname || !hsc_comp ||
        !hsc_getmes || !hsc_bye || !pack_ini || !pack_make ||
        !pack_view || !pack_get) {
        FreeLibrary(module);
        return fail("required export not found", 4);
    }

    auto message = [&]() {
        char buffer[32768] = {};
        hsc_getmes(buffer, 0, 0, 0);
        std::fwrite(buffer, 1, std::strlen(buffer), stderr);
        if (buffer[0] && buffer[std::strlen(buffer) - 1] != '\n')
            std::fputc('\n', stderr);
    };
    auto call_path = [&](PathFn fn, const wchar_t *value) -> int {
        std::vector<char> encoded;
        if (!to_acp(value, encoded)) return -1000;
        return fn(nullptr, encoded.data(), 0, 0);
    };

    int result = 0;
    std::wstring action = argv[1];
    if (action == L"compile" || action == L"runtime-compile") {
        if (argc != 8) result = 2;
        else {
            int utf8_input = _wtoi(argv[7]);
            result = call_path(hsc_ini, argv[3]);
            if (!result) result = call_path(hsc_compath, argv[4]);
            if (!result) result = call_path(hsc_objname, argv[5]);
            // Compiler-path tests intentionally request UTF-8 string output.
            // Runtime tests need bytecode matching the selected runtime:
            // ANSI for HSP 3.7, UTF-8 plus the 64-bit runtime flag for HSP 3.8.
            HspInt output_mode = action == L"runtime-compile"
                ? (utf8_input ? 128 : 0) : 4;
            if (!result) result = hsc_comp(output_mode, utf8_input ? 32 : 0, 0, 0);
        }
    } else if (action == L"compile-pack") {
        if (argc != 8) result = 2;
        else {
            int utf8_input = _wtoi(argv[7]);
            result = call_path(hsc_ini, argv[3]);
            if (!result) result = call_path(hsc_compath, argv[4]);
            if (!result) result = call_path(hsc_objname, argv[5]);
            if (!result) result = hsc_comp(4, (utf8_input ? 32 : 0) | 4, 0, 0);
        }
    } else if (action == L"pack") {
        if (argc != 5) result = 2;
        else {
            result = call_path(pack_ini, argv[3]);
            if (!result) result = pack_make(1, 0, 0, 0);
            if (!result && !DeleteFileW(argv[4])) result = -1001;
            if (!result) result = call_path(pack_ini, argv[3]);
            if (!result) result = pack_view(0, 0, 0, 0);
            if (!result) result = call_path(pack_get, argv[4]);
        }
    } else if (action == L"pack-flow") {
        // source, common, output, utf8-input, pack-base, asset-name, extract
        if (argc != 10) result = 2;
        else {
            int utf8_input = _wtoi(argv[6]);
            int extract = _wtoi(argv[9]);
            result = call_path(hsc_ini, argv[3]);
            if (!result) result = call_path(hsc_compath, argv[4]);
            if (!result) result = call_path(hsc_objname, argv[5]);
            if (!result) result = hsc_comp(4, (utf8_input ? 32 : 0) | 4, 0, 0);
            if (!result) result = call_path(pack_ini, argv[7]);
            if (!result) result = pack_make(1, 0, 0, 0);
            if (!result && extract && !DeleteFileW(argv[8])) result = -1001;
            if (!result && extract) result = call_path(pack_ini, argv[7]);
            if (!result && extract) result = pack_view(0, 0, 0, 0);
            if (!result && extract) result = call_path(pack_get, argv[8]);
        }
    } else {
        result = 2;
    }

    if (result) message();
    hsc_bye(0, 0, 0, 0);
    FreeLibrary(module);
    if (result == -1000) return fail("path is not representable in Windows ACP", 5);
    if (result == -1001) return fail("failed to remove packed asset before extraction", 6);
    if (result) {
        std::fprintf(stderr, "HSPCMP_RESULT %d\n", result);
        return 1;
    }
    std::printf("WORKER_OK ACP=%u BITS=%u\n", GetACP(),
                static_cast<unsigned>(sizeof(void *) * 8));
    return 0;
}

int wmain(int argc, wchar_t **argv) {
    const UINT error_mode = SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX |
                            SEM_NOOPENFILEERRORBOX;
    SetErrorMode(error_mode);
    SetThreadErrorMode(error_mode, nullptr);
    _set_abort_behavior(0, _WRITE_ABORT_MSG | _CALL_REPORTFAULT);

    __try {
        return worker_main(argc, argv);
    } __except (EXCEPTION_EXECUTE_HANDLER) {
        std::fprintf(stderr, "WORKER_CRASH 0x%08lX\n", GetExceptionCode());
        return 128;
    }
}
