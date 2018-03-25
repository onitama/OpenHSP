#/*-------------------------------------------------------------------------*/
#
#   HSPランタイムのビルド。(mingw用)
#
#   GNU make 用メイクファイル
#
#   (使い方)
#   make ( <PLATFORM> [ <OBJ> ... ] [ release ] [ all ] ) | ( <PLATFORM> clean ) | cleanall
#
#   <PLATFORM> は
#       linux
#       win32_mingw
#       win32gui_mingw
#       win64gui_mingw
#   のどれか。
#   makeにall, cleanを渡す場合もPLATFORMは必要です。
#
#   <OBJ> にファイルを指定すると指定したファイルのみをビルド対象にする。
#   指定しなかった場合は <PLATFORM> に対応した実行ファイルを対象にする。
#
#   release を指定するとリリースビルド、そうでなければデバッグビルドを作成できます。
#
#   all を指定すると <PLATFORM> に関係したオブジェクトファイル・実行ファイル全てのリビルド。
#   clean を指定すると <PLATFORM> に関係したオブジェクトファイル・実行ファイルの削除。
#   cleanallを指定するとオブジェクトファイルと実行ファイルの削除。
#
#   HSPランタイムはWindows 2000以降用です
#
#
#
#/*-------------------------------------------------------------------------*/

#/*-----------------------------------------------------------*/
#
#   追加インクルードパス (オプション)

includedir =
libdir =

#/*-----------------------------------------------------------*/
#
#   C++のバージョン

CXXVER = -std=gnu++11

#/*-----------------------------------------------------------*/
#
#   環境フラグ
#   それぞれのフラグはお互いと使用可能です
#

ENVFLAGS =

#コンパクト版ランタイム (win64gui_mingwの場合、これを使用しなければならない)
#ENVFLAGS = -DHSP_COMPACT -DHSP_COM_UNSUPPORTED

#ユニコード版ランタイム (win32gui_mingw、win64gui_mingw用)
#ENVFLAGS =  -DHSPUTF8 -DUNICODE -D_UNICODE

#/*-----------------------------------------------------------*/
#
#   削除コマンド
#

LINUXRM = rm -rf
WINRM = del /Q /F

#/*-----------------------------------------------------------*/
#
#   プラットフォーム判定
#

PLATFORM =

#/*------------------------------------------*/

ifneq ($(filter linux,$(MAKECMDGOALS)),)
    PLATFORM := $(PLATFORM) linux
endif

ifneq ($(filter win32_mingw,$(MAKECMDGOALS)),)
    PLATFORM := $(PLATFORM) win32_mingw
endif

ifneq ($(filter win32gui_mingw,$(MAKECMDGOALS)),)
    PLATFORM := $(PLATFORM) win32gui_mingw
endif

ifneq ($(filter win64gui_mingw,$(MAKECMDGOALS)),)
    PLATFORM := $(PLATFORM) win64gui_mingw
endif

#/*------------------------------------------*/

ifneq ($(words $(PLATFORM)),1)
    PLATFORM =
endif

#/*------------------------------------------*/

#/*-----------------------------------------------------------*/
#
#   プラットフォームごとの設定
#

#/*------------------------------------------*/
#
#   gcc で linux 版ランタイムをビルド。
#
#   動作未確認。問題があれば makefile.linux の内容を参照してください。
#

ifneq ($(filter linux,$(PLATFORM)),)

    includedir := $(includedir)
    libdir := $(libdir)

    vpath
    vpath %.c   ./linux
    vpath %.cpp ./linux
    vpath %.h   ./linux
    vpath %.hpp ./linux

    RM = $(LINUXRM)

    CC = gcc
    ENVFLAGS    +=  -DHSPLINUX -DHSPUTF8
    CFLAGS_B    =   -Wall -finput-charset=cp932 -fexec-charset=cp932 \
                    $(ENVFLAGS) $(addprefix -I,$(includedir))
    CFLAGS_D    =   $(CFLAGS_B) -DHSPDEBUG
    CFLAGS_R    =   $(CFLAGS_B) -O3 -DNDEBUG

    CXX = $(CC)
    CXXFLAGS_B  =   $(CFLAGS_B) $(CXXVER)
    CXXFLAGS_D  =   $(CFLAGS_D) $(CXXVER)
    CXXFLAGS_R  =   $(CFLAGS_R) $(CXXVER)

    AS          =   nasm
    ASFLAGS     =   $(ASMOPTS) -i$(asmdir) -f elf32 -o

    LD = $(CC)
    LDFLAGS_B   =   $(LINKOPTS) -s $(addprefix -L,$(libdir)) $(addprefix -l,$(LIBS))
    LDFLAGS_D   =   $(LDFLAGS_B)
    LDFLAGS_R   =   $(LDFLAGS_B)

    PF_TARGET_D =   hsp3
    PF_TARGET_R =   $(PF_TARGET_D)

    OBJS = linux/main.o hsp3.o hsp3code.o hsp3debug.o hsp3int.o \
           hspvar_core.o hspvar_double.o hspvar_int.o hspvar_label.o \
           hspvar_str.o hspvar_struct.o stack.o strbuf.o strnote.o \
           dpmread.o linux/supio_linux.o linux/hsp3cl.o \
           linux/hsp3ext_linux.o linux/hsp3gr_linux.o
    RESES =
    LIBS = m stdc++

endif

#/*------------------------------------------*/

#/*------------------------------------------*/
#
#   mingw で Windows コンソール版ランタイムをビルド。
#
#   TDM-GCC (GCC 5.10) でこれは確認

ifneq ($(filter win32_mingw,$(PLATFORM)),)

    includedir := $(includedir)
    libdir := $(libdir)

    asmdir := ./win32gui/

    vpath
    vpath %.c   ./win32 ./win32gui
    vpath %.cpp ./win32 ./win32gui
    vpath %.h   ./win32 ./win32gui $(asmdir)
    vpath %.hpp ./win32 ./win32gui
    vpath %.rc  ./win32 ./win32gui
    vpath %.mac $(asmdir)
    vpath %.asm $(asmdir)

    RM = $(WINRM)

    CC = gcc
    ENVFLAGS    +=  -DWINVER=0x0500 -D_WIN32_IE=0x0500 -DHSPWIN -DHSPCL_WIN
    CFLAGS_B    =   -m32 -Wall -Wno-unused -fexceptions -fsigned-char -finput-charset=cp932 -fexec-charset=cp932 -mwindows -mconsole -Os \
                    $(ENVFLAGS) $(addprefix -I,$(includedir))
    CFLAGS_D    =   $(CFLAGS_B) -DHSPDEBUG
    CFLAGS_R    =   $(CFLAGS_B) -DNDEBUG

    CXX = g++
    CXXFLAGS_B  =   $(CFLAGS_B) $(CXXVER)
    CXXFLAGS_D  =   $(CFLAGS_D) $(CXXVER)
    CXXFLAGS_R  =   $(CFLAGS_R) $(CXXVER)

    AS          =   nasm
    ASFLAGS     =   -i$(asmdir) -f win32 -o

    LD = g++
    LDFLAGS_B   =   -m32 -s -mwindows -mconsole \
                    $(addprefix -L,$(libdir)) $(addprefix -l,$(LIBS))
    LDFLAGS_D   =   $(LDFLAGS_B)
    LDFLAGS_R   =   $(LDFLAGS_B)

    PF_TARGET_D =   hsp3cl.exe
    PF_TARGET_R =   $(PF_TARGET_D)

    RESES       =
    ASMOBJS     =

    OBJS := dpmread.o hsp3.o hsp3code.o hsp3debug.o hsp3int.o hspvar_core.o \
            hspvar_double.o hspvar_int.o hspvar_label.o hspvar_str.o \
            hspvar_struct.o stack.o strbuf.o strnote.o \
            win32/hsp3cl.o win32/hsp3gr_win.o win32/main.o \
            win32gui/comobj.o win32gui/comobj_event.o win32gui/filedlg.o win32gui/hsp3ext_win.o win32gui/hsp3extlib.o \
            win32gui/hspvar_comobj.o win32gui/hspvar_variant.o win32gui/supio_win.o win32gui/supio_win_unicode.o \
            $(RESES) $(ASMOBJS)

    asmdir = ./win32asm/
    LIBS = comctl32 ole32 oleaut32 uuid

endif

#/*------------------------------------------*/

#/*------------------------------------------*/
#
#   mingw で Windows GUI 版ランタイムをビルド。
#
#   TDM-GCC (GCC 5.10) でこれは確認。
#

ifneq ($(filter win32gui_mingw,$(PLATFORM)),)

    includedir := $(includedir)
    libdir := $(libdir)

    asmdir := ./win32gui/

    vpath
    vpath %.c   ./win32gui
    vpath %.cpp ./win32gui
    vpath %.h   ./win32gui $(asmdir)
    vpath %.hpp ./win32gui
    vpath %.rc  ./win32gui
    vpath %.mac $(asmdir)
    vpath %.asm $(asmdir)

    RM = $(WINRM)

    CC = gcc
    ENVFLAGS    +=  -DWINVER=0x0500 -D_WIN32_IE=0x0500 -D_WIN32_WINNT=0x0500 -DHSPWIN -DHSPWINGUI
    CFLAGS_B    =   -m32 -Wall -Wno-unused -fexceptions -fsigned-char -finput-charset=cp932 -fexec-charset=cp932 -mwindows -Os \
                    $(ENVFLAGS) $(addprefix -I,$(includedir))
    CFLAGS_D    =   $(CFLAGS_B) -DHSPDEBUG
    CFLAGS_R    =   $(CFLAGS_B) -DNDEBUG

    CXX = g++
    CXXFLAGS_B  =   $(CFLAGS_B) $(CXXVER)
    CXXFLAGS_D  =   $(CFLAGS_D) $(CXXVER)
    CXXFLAGS_R  =   $(CFLAGS_R) $(CXXVER)

    AS          =   nasm
    ASFLAGS     =   -i$(asmdir) -f win32 -o

    WINDRES     =   windres
    RCFLAGS     =   -J rc -O coff -F pe-i386

    LD = g++
    LDFLAGS_B   =   -m32 -s -mwindows $(addprefix -L,$(libdir)) $(addprefix -l,$(LIBS))
    LDFLAGS_D   =   $(LDFLAGS_B)
    LDFLAGS_R   =   $(LDFLAGS_B)

    PF_TARGET_D =   hsp3.exe
    PF_TARGET_R =   hsprt.exe

    RESES       =   hsp3.res
    ASMOBJS     =

    OBJS := dpmread.o hsp3.o hsp3code.o hsp3debug.o hsp3int.o hspvar_core.o \
            hspvar_double.o hspvar_int.o hspvar_label.o hspvar_str.o \
            hspvar_struct.o stack.o strbuf.o strnote.o \
            win32gui/comobj.o win32gui/comobj_event.o win32gui/fcpoly.o win32gui/filedlg.o win32gui/hsp3ext_win.o \
            win32gui/hsp3extlib.o win32gui/hsp3gr_wingui.o win32gui/hsp3win.o win32gui/hspvar_comobj.o \
            win32gui/hspvar_variant.o win32gui/hspwnd_obj.o win32gui/hspwnd_win.o win32gui/main.o win32gui/mmman.o win32gui/supio_win.o win32gui/supio_win_unicode.o win32gui/stb_image.o \
            $(RESES) $(ASMOBJS)

    asmdir = ./win32asm/
    LIBS = comctl32 ole32 oleaut32 uuid winmm msimg32

endif

#/*------------------------------------------*/

#/*------------------------------------------*/
#
#   mingw で Windows GUI 版ランタイムをビルド。
#   Win64ランタイム
#
#   TDM-GCC (GCC 5.10) でこれは確認。(コンパクト版のみ確認)
#

ifneq ($(filter win64gui_mingw,$(PLATFORM)),)

    includedir := $(includedir)
    libdir := $(libdir)

    asmdir := ./win32gui/

    vpath
    vpath %.c   ./win32gui
    vpath %.cpp ./win32gui
    vpath %.h   ./win32gui $(asmdir)
    vpath %.hpp ./win32gui
    vpath %.rc  ./win32gui
    vpath %.mac $(asmdir)
    vpath %.asm $(asmdir)

    RM = $(WINRM)

    CC = gcc
    ENVFLAGS    +=  -DHSP64 -DHSPWIN -DHSPWINGUI
    CFLAGS_B    =   -m64 -Wall -Wno-unused -fexceptions -fsigned-char -finput-charset=cp932 -fexec-charset=cp932 -mwindows -Os \
                    $(ENVFLAGS) $(addprefix -I,$(includedir))
    CFLAGS_D    =   $(CFLAGS_B) -DHSPDEBUG
    CFLAGS_R    =   $(CFLAGS_B) -DNDEBUG

    CXX = g++
    CXXFLAGS_B  =   $(CFLAGS_B) $(CXXVER)
    CXXFLAGS_D  =   $(CFLAGS_D) $(CXXVER)
    CXXFLAGS_R  =   $(CFLAGS_R) $(CXXVER)

    AS          =   nasm
    ASFLAGS     =   -i$(asmdir) -f win64 -o

    WINDRES     =   windres
    RCFLAGS     =   -J rc -O coff

    LD = g++
    LDFLAGS_B   =   -m64 -s -mwindows $(addprefix -L,$(libdir)) $(addprefix -l,$(LIBS))
    LDFLAGS_D   =   $(LDFLAGS_B)
    LDFLAGS_R   =   $(LDFLAGS_B)

    PF_TARGET_D =   hsp3_64.exe
    PF_TARGET_R =   hsprt.exe

    RESES       =   hsp3.res
    ASMOBJS     =   win32gui/callex64_nasm.o

    OBJS := dpmread.o hsp3.o hsp3code.o hsp3debug.o hsp3int.o hspvar_core.o \
            hspvar_double.o hspvar_int.o hspvar_label.o hspvar_str.o \
            hspvar_struct.o stack.o strbuf.o strnote.o \
            win32gui/comobj.o win32gui/comobj_event.o win32gui/fcpoly.o win32gui/filedlg.o win32gui/hsp3ext_win.o \
            win32gui/hsp3extlib.o win32gui/hsp3gr_wingui.o win32gui/hsp3win.o win32gui/hspvar_comobj.o \
            win32gui/hspvar_variant.o win32gui/hspwnd_obj.o win32gui/hspwnd_win.o win32gui/main.o win32gui/mmman.o win32gui/supio_win.o win32gui/supio_win_unicode.o win32gui/stb_image.o \
            $(RESES) $(ASMOBJS)

    LIBS = comctl32 ole32 oleaut32 uuid winmm msimg32

endif

#/*------------------------------------------*/

#/*-----------------------------------------------------------*/

#/*------------------------------------------*/
#
# リリース・デバッグの切り替え。
#

ifneq ($(filter release,$(MAKECMDGOALS)),)
    CFLAGS = $(CFLAGS_R)
    CXXFLAGS = $(CXXFLAGS_R)
    LDFLAGS = $(LDFLAGS_R)
    PF_TARGET = $(PF_TARGET_R)
else
    CFLAGS = $(CFLAGS_D)
    CXXFLAGS = $(CXXFLAGS_D)
    LDFLAGS = $(LDFLAGS_D)
    PF_TARGET = $(PF_TARGET_D)
endif

#/*------------------------------------------*/

TARGET = $(filter *.o *.exe *.dll,$(MAKECMDGOALS))

ifeq ($(words $(TARGET)),0)
    TARGET = $(PF_TARGET)
endif

#/*-----------------------------------------------------------*/

EXECMODE =
FILESRM =

ifeq ($(words $(PLATFORM)),1)
    EXECMODE = exec_target
endif

ifneq ($(filter cleanall,$(MAKECMDGOALS)),)
    EXECMODE = exec_clean
    FILESRM = $(TARGET)
    MAKECMDGOALS := $(patsubst cleanall,clean,$(MAKECMDGOALS))
endif

ifneq ($(filter clean,$(MAKECMDGOALS)),)
    EXECMODE = exec_clean
    FILESRM += $(OBJS)
    ifneq "$(PLATFORM)" "linux"
        FILESRM := $(subst /,\,$(FILESRM))
    endif
endif

ifneq ($(filter all,$(MAKECMDGOALS)),)
    EXECMODE = exec_alltarget
endif

ifneq ($(words $(PLATFORM)),1)
    EXECMODE = exec_error_platform
endif

#/*-----------------------------------------------------------*/

.SUFFIXES:
.SUFFIXES: .c .cpp .h .hpp .mac .asm .o .rc .res

.PHONY: all clean cleanall release \
        linux win32_mingw win32gui_mingw win64gui_mingw \
        exec_alltarget exec_clean exec_error_platform exec_pre_alltarget exec_target

#/*-----------------------------------------------------------*/

linux win32_mingw win32gui_mingw win64gui_mingw all clean cleanall release: $(EXECMODE)
	@echo -n

#/*-------------------------------------------*/

exec_error_platform:
	@echo Only one platform must be specified.
	@exit 1

exec_clean:
	$(RM) $(FILESRM)

exec_target: $(TARGET)
	@echo -n

exec_alltarget: exec_pre_alltarget $(TARGET)
	@echo -n

exec_pre_alltarget:
	$(RM) $(OBJS) $(TARGET)

#/*-------------------------------------------*/

$(TARGET): $(OBJS)
	$(LD) $(OBJS) $(LDFLAGS) -o $@

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

.cpp.o:
	$(CXX) $(CXXFLAGS) -c $< -o $@

.asm.o:
	$(AS) $(ASFLAGS) $@ $<

.rc.res:
	$(WINDRES) $(RCFLAGS) -i $< -o $@ -I $(<D)

#	$(WINDRES) -i $< --input-format=rc -o $@ -O coff --include-dir $(<D)

#/*-------------------------------------------*/

#/*-----------------------------------------------------------*/

#/*-------------------------------------------*/
#
#  ../hsp3
#

dpmread.o: dpmread.cpp hsp3config.h dpmread.h supio.h
hsp3.o: hsp3.cpp hsp3.h hsp3debug.h hsp3struct.h hspvar_core.h hsp3ext.h \
  hsp3code.h hsp3config.h dpmread.h supio.h
hsp3code.o: hsp3code.cpp hspwnd.h supio.h dpmread.h stack.h strbuf.h \
  hsp3code.h hsp3debug.h hsp3struct.h hspvar_core.h hsp3config.h \
  hsp3int.h
hsp3debug.o: hsp3debug.cpp hsp3config.h hsp3.h hsp3debug.h hsp3struct.h \
  hspvar_core.h hsp3ext.h hsp3code.h
hsp3int.o: hsp3int.cpp hsp3config.h hspwnd.h supio.h dpmread.h strbuf.h \
  strnote.h hsp3int.h hsp3struct.h hspvar_core.h hsp3debug.h hsp3code.h
hspvar_core.o: hspvar_core.cpp hspvar_core.h hspvar_label.h hsp3debug.h \
  strbuf.h supio.h
hspvar_double.o: hspvar_double.cpp hspvar_core.h hsp3debug.h strbuf.h
hspvar_int.o: hspvar_int.cpp hspvar_core.h hsp3debug.h strbuf.h supio.h
hspvar_label.o: hspvar_label.cpp hsp3code.h hsp3debug.h hsp3struct.h \
  hspvar_core.h hspvar_label.h strbuf.h supio.h
hspvar_str.o: hspvar_str.cpp hspvar_core.h hsp3debug.h strbuf.h supio.h
hspvar_struct.o: hspvar_struct.cpp hsp3code.h hsp3debug.h hsp3struct.h \
  hspvar_core.h strbuf.h supio.h
stack.o: stack.cpp stack.h hspvar_core.h hsp3debug.h
strbuf.o: strbuf.cpp supio.h strbuf.h hsp3debug.h
strnote.o: strnote.cpp strnote.h


#/*-------------------------------------------*/
#
#  ../hsp3/linux
#

linux/main.o: hsp3cl.h hsp3config.h
linux/hsp3cl.o: strbuf.h hsp3.h hsp3debug.h \
  hsp3struct.h hspvar_core.h hsp3ext.h hsp3code.h \
  hsp3config.h supio.h hsp3gr.h
linux/hsp3ext_linux.o: hsp3ext.h hsp3code.h hsp3debug.h \
  hsp3struct.h hspvar_core.h
linux/hsp3gr_linux.o: hsp3code.h hsp3debug.h hsp3struct.h \
  hspvar_core.h supio.h strbuf.h hsp3gr.h
hsp3.o: hsp3.h hsp3debug.h hsp3struct.h \
  hspvar_core.h hsp3ext.h hsp3code.h hsp3config.h \
  dpmread.h supio.h
hsp3code.o: hspwnd.h hsp3code.h hsp3debug.h \
  hsp3struct.h hspvar_core.h supio.h dpmread.h stack.h \
  strbuf.h hsp3config.h hsp3int.h
hsp3debug.o: hsp3config.h hsp3.h hsp3debug.h \
  hsp3struct.h hspvar_core.h hsp3ext.h hsp3code.h
hsp3int.o: hspwnd.h hsp3code.h hsp3debug.h \
  hsp3struct.h hspvar_core.h supio.h dpmread.h strbuf.h \
  strnote.h hsp3int.h
hspvar_core.o: hspvar_core.h hspvar_label.h \
  hsp3debug.h strbuf.h supio.h
hspvar_double.o: hspvar_core.h hsp3debug.h \
  strbuf.h
hspvar_int.o: hspvar_core.h hsp3debug.h \
  strbuf.h supio.h
hspvar_label.o: hsp3code.h hsp3debug.h \
  hsp3struct.h hspvar_core.h hspvar_label.h strbuf.h \
  supio.h
hspvar_str.o: hspvar_core.h hsp3debug.h \
  strbuf.h supio.h
hspvar_struct.o: hsp3code.h hsp3debug.h \
  hsp3struct.h hspvar_core.h strbuf.h supio.h
stack.o: stack.h hspvar_core.h hsp3debug.h
strbuf.o: supio.h strbuf.h hsp3debug.h
strnote.o: strnote.h
dpmread.o: dpmread.h supio.h
linux/supio_linux.o: linux/supio_linux.h supio.h dpmread.h strbuf.h


#/*-------------------------------------------*/
#
#  ../hsp3/win32
#

win32/hsp3cl.o: hsp3cl.cpp hsp3gr_win.h hsp3struct.h hspvar_core.h \
  hsp3debug.h hsp3config.h strbuf.h hsp3.h hsp3struct.h hsp3ext.h hsp3code.h \
  hsp3gr.h supio.h hsp3ext_win.h hsp3code.h hspvar_comobj.h comobj.h \
  hsp3extlib.h hspvar_comobj.h hspvar_variant.h
win32/hsp3gr_win.o: hsp3gr_win.cpp hsp3code.h hsp3debug.h hsp3struct.h \
  hspvar_core.h hsp3debug.h supio.h strbuf.h filedlg.h hsp3gr_win.h \
  hsp3struct.h
win32/main.o: main.cpp hsp3cl.h


#/*-------------------------------------------*/
#
#  ../hsp3/win32gui
#

win32gui/comobj.o: comobj.cpp hsp3code.h hsp3debug.h hsp3struct.h \
  hspvar_core.h comobj.h comobj_event.h hsp3ext.h strbuf.h
win32gui/comobj_event.o: comobj_event.cpp hsp3code.h hsp3debug.h \
  hsp3struct.h hspvar_core.h comobj_event.h hspvar_comobj.h \
  comobj.h
win32gui/fcpoly.o: fcpoly.cpp fcpoly.h supio.h
win32gui/filedlg.o: filedlg.cpp
win32gui/hsp3ext_win.o: hsp3ext_win.cpp supio.h hsp3ext.h hsp3extlib.h \
  hsp3code.h hsp3debug.h hsp3struct.h hspvar_core.h \
  hspvar_comobj.h comobj.h hspvar_variant.h comobj_event.h hsp3win.h \
  hspwnd.h strbuf.h
win32gui/hsp3extlib.o: hsp3extlib.cpp supio.h dpmread.h hsp3ext.h \
  hsp3extlib.h hsp3code.h hsp3debug.h hsp3struct.h \
  hspvar_core.h hspvar_comobj.h comobj.h hsp3win.h hspwnd.h \
  strbuf.h
win32gui/hsp3gr_wingui.o: hsp3gr_wingui.cpp hspwnd.h mmman.h filedlg.h \
  supio.h dpmread.h stack.h strbuf.h fcpoly.h hsp3gr.h \
  hsp3code.h hsp3debug.h hsp3struct.h hspvar_core.h \
  hsp3debug.h
win32gui/hsp3win.o: hsp3win.cpp strbuf.h hsp3.h hsp3debug.h \
  hsp3struct.h hspvar_core.h hsp3ext.h hsp3code.h \
  hsp3ext.h hsp3gr.h hspvar_comobj.h comobj.h hspvar_variant.h \
  supio.h
win32gui/hspvar_comobj.o: hspvar_comobj.cpp hsp3code.h hsp3debug.h \
  hsp3struct.h hspvar_core.h hsp3struct.h hsp3ext.h \
  hspvar_core.h comobj.h hspvar_comobj.h comobj_event.h strbuf.h \
  supio.h
win32gui/hspvar_variant.o: hspvar_variant.cpp hsp3code.h hsp3debug.h \
  hsp3struct.h hspvar_core.h hsp3struct.h hsp3ext.h \
  hspvar_core.h comobj.h hspvar_comobj.h hspvar_variant.h \
  comobj_event.h strbuf.h supio.h
win32gui/hspwnd_obj.o: hspwnd_obj.cpp hsp3config.h hsp3debug.h hspwnd.h \
  dpmread.h strbuf.h strnote.h supio.h
win32gui/hspwnd_win.o: hspwnd_win.cpp hsp3config.h hsp3debug.h hspwnd.h \
  dpmread.h strbuf.h strnote.h supio.h
win32gui/main.o: main.cpp hsp3win.h
win32gui/mmman.o: mmman.cpp supio.h dpmread.h strbuf.h mmman.h
win32gui/supio_win.o: supio_win.cpp hsp3config.h supio_win.h dpmread.h \
  strbuf.h
win32gui/supio_win_unicode.o: supio_win_unicode.cpp hsp3config.h supio_win_unicode.h dpmread.h \
  strbuf.h

win32gui/stb_image.o: stb_image.c stb_image.h

win32gui/callex64_nasm.o: callex64_nasm.asm sehhdr64_nasm.h

hsp3.res: hsp3.rc