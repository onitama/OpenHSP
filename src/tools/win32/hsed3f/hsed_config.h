
//
//		Configure for HSED3
//
#ifndef __hsed_config_h
#define __hsed_config_h

//		移植用のラベル
//
#define HSPWIN		// Windows version flag
//#define HSPMAC		// Macintosh version flag
//#define HSPLINUX	// Linux version flag

#define HSPVER3		// HSP version3.0 flag

#ifdef HSPVER3
#include "../../../hsp3/hsp3config.h"
#else
#define vercode 0x0206		// HSP2 version code
#endif

#define HSED_VER 0x0300 // HSP version3.0 for registry
//#define HSED_VER 0x0261 // HSP version2.61 for registry

#define HSED_PUBLIC_VER vercode // hsed public version
#define HSED_PRIVATE_VER 0x10404 // hsed private version

//		システム関連ラベル
//
#ifdef HSPVER3
#define FILE_HSPCMP "hspcmp.dll"
#define DEFAULT_RUNTIME "hsp3.exe"   // HSP3 runtime
#define HSP_VERSION 3
#define REG_SUFFIX "3"                        // Suffix of registry section name 
#define INI_FILE "hsp3config.ini"             // INI section name
#define MUTEX_NAME  "HSPEditor3_Mutex"        // Name of Mutex object
#define PROP_NAME   "HSPEditor3_Property"     // Name of Property list

#else

#define FILE_HSPCMP "hspcmp.dll"
#define DEFAULT_RUNTIME "hsp2.exe"   // HSP2 runtime
#define REG_SUFFIX                            // Suffix of registry section name
#define INI_FILE "hsp2config.ini"             // INI file name
#define MUTEX_NAME  "HSPEditor2_Mutex"        // Name of Mutex object
#define PROP_NAME   "HSPEditor2_Property"     // Name of Property list
#endif

//		デフォルトの文字列
//
#define UNTITLED "(untitled)"
#define TABUNTITLED "untitled"

//		デフォルトの値
#define TMPSIZE 2048

//		インターフェイス用
//
#define HSED_INTERFACE_NAME "HSPEditorInterface"

#endif
