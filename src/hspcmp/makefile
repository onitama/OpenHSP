#/*-------------------------------------------------------------------------*/
#
#   HSPコンパイラのビルド。
#
#   GNU make 用メイクファイル
#
#   (使い方)
#   make ( PLATFORM [ OBJ ... ] [ release ] [ all ] ) | ( PLATFORM clean ) | cleanall
#
#   PLATFORM は
#       linux
#       win32_mingw
#       win32dll_mingw
#   のどれか。all, clean を指定する場合も PLATFORM を指定する。
#
#   OBJ にファイルを指定すると指定したファイルのみをビルド対象にする。
#   指定しなかった場合は PLATFORM に対応した実行ファイルを対象にする。
#
#   release を指定するとリリースビルド、それ以外はデバッグビルド。
#
#   all を指定すると PLATFORM に関係したオブジェクトファイル・実行ファイル全てのリビルド。
#   clean を指定すると PLATFORM に関係したオブジェクトファイル・実行ファイルの削除。
#   cleanall を指定するとそのフォルダ内のすべてのオブジェクトファイル・実行ファイルの削除。
#
#/*-------------------------------------------------------------------------*/

#/*-----------------------------------------------------------*/
#
#   必要ならば環境に合わせて includedir, libdir を設定する。
#   パスを空白で区切って列挙する。(半角スペースを含むパスは""で囲む)
#

includedir =
libdir =

#/*-----------------------------------------------------------*/

RM = rm -f

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

ifneq ($(filter win32dll_mingw,$(MAKECMDGOALS)),)
    PLATFORM := $(PLATFORM) win32dll_mingw
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
#   gcc で linux 用実行ファイルをビルド。
#
#   動作未確認。問題があれば makefile.linux の内容を参照してください。
#

ifneq ($(filter linux,$(PLATFORM)),)

    includedir := $(includedir)
    libdir := $(libdir)

    vpath
    vpath %.c   ./linux ../hsp3
    vpath %.cpp ./linux ../hsp3
    vpath %.h   ./linux ../hsp3
    vpath %.hpp ./linux ../hsp3

    CC = gcc
    CFLAGS_B = -Wall -finput-charset=cp932 -fexec-charset=cp932 \
               -DHSPLINUX $(addprefix -I,$(includedir))
    CFLAGS_D = $(CFLAGS_B) -DHSPDEBUG -g
    CFLAGS_R = $(CFLAGS_B) -O3 -DNDEBUG

    CXX = $(CC)
    CXXFLAGS_B = $(CFLAGS_B)
    CXXFLAGS_D = $(CFLAGS_D)
    CXXFLAGS_R = $(CFLAGS_R)

    LD = $(CC)
    LDFLAGS_B = $(addprefix -L,$(libdir)) $(addprefix -l,$(LIBS))
    LDFLAGS_D = $(LDFLAGS_B)
    LDFLAGS_R = $(LDFLAGS_B) -s

    PF_TARGET_D = hspcmp
    PF_TARGET_R = $(PF_TARGET_D)

    OBJS = ahtmodel.o ahtobj.o codegen.o comutil.o errormsg.o hsc3.o \
           hspcmd.o label.o localinfo.o main.o membuf.o strnote.o \
           tagstack.o token.o \
           supio_linux.o
    LIBS = m stdc++

endif

#/*------------------------------------------*/

#/*------------------------------------------*/
#
#   mingw で Windows 用実行ファイルをビルド。
#
#   mingw32(gcc3.4.2) + msys で動作確認。
#

ifneq ($(filter win32_mingw,$(PLATFORM)),)

    includedir := $(includedir) "/mingw/include" "/mingw/include/c++/3.4.2"
    libdir := $(libdir) "/mingw/lib"

    vpath
    vpath %.c   ./win32 ../hsp3
    vpath %.cpp ./win32 ../hsp3
    vpath %.h   ./win32 ../hsp3
    vpath %.hpp ./win32 ../hsp3

    CC = gcc
    CFLAGS_B = -Wall -Wno-unused -fexceptions -fsigned-char \
               -finput-charset=cp932 -fexec-charset=cp932 -mconsole \
               -DWINVER=0x0400 -D_WIN32_IE=0x0500 -DHSPWIN \
               $(addprefix -I,$(includedir))
    CFLAGS_D = $(CFLAGS_B)
    CFLAGS_R = $(CFLAGS_B) -O3 -DNDEBUG

    CXX = g++
    CXXFLAGS_B = $(CFLAGS_B)
    CXXFLAGS_D = $(CFLAGS_D)
    CXXFLAGS_R = $(CFLAGS_R)

    LD = g++
    LDFLAGS_B = $(addprefix -L,$(libdir)) $(addprefix -l,$(LIBS))
    LDFLAGS_D = $(LDFLAGS_B)
    LDFLAGS_R = $(LDFLAGS_B) -s

    PF_TARGET_D = hspcmp.exe
    PF_TARGET_R = $(PF_TARGET_D)

    OBJS = ahtmodel.o ahtobj.o codegen.o comutil.o errormsg.o hsc3.o \
           hspcmd.o label.o localinfo.o main.o membuf.o strnote.o \
           hsmanager.o tagstack.o token.o \
           supio_win.o
    LIBS = ole32

endif

#/*------------------------------------------*/

#/*------------------------------------------*/
#
#   mingw で Windows 用 DLL ファイルをビルド。
#
#   mingw32(gcc3.4.2) + msys で動作確認。
#

ifneq ($(filter win32dll_mingw,$(PLATFORM)),)

    includedir := $(includedir) "/mingw/include" "/mingw/include/c++/3.4.2"
    libdir := $(libdir) "/mingw/lib"

    vpath
    vpath %.c   ./win32 ./win32dll ../hsp3
    vpath %.cpp ./win32 ./win32dll ../hsp3
    vpath %.h   ./win32 ./win32dll ../hsp3
    vpath %.hpp ./win32 ./win32dll ../hsp3

    CC = gcc
    CFLAGS_B = -Wall -Wno-unused -fexceptions -fsigned-char \
               -finput-charset=cp932 -fexec-charset=cp932 -mdll \
               -DWINVER=0x0400 -D_WIN32_IE=0x0500 -DHSPWIN \
               $(addprefix -I,$(includedir))
    CFLAGS_D = $(CFLAGS_B)
    CFLAGS_R = $(CFLAGS_B) -O3 -DNDEBUG

    CXX = g++
    CXXFLAGS_B = $(CFLAGS_B)
    CXXFLAGS_D = $(CFLAGS_D)
    CXXFLAGS_R = $(CFLAGS_R)

    LD = dllwrap
    LDFLAGS_B = --def $(DEFFILE) --driver-name $(CXX) \
                $(addprefix -L,$(libdir)) $(addprefix -l,$(LIBS))
    LDFLAGS_D = $(LDFLAGS_B)
    LDFLAGS_R = $(LDFLAGS_B) -s

    PF_TARGET_D = hspcmp.dll
    PF_TARGET_R = $(PF_TARGET_D)

    OBJS = ahtmodel.o ahtobj.o codegen.o comutil.o errormsg.o hsc3.o \
           hspcmd.o label.o localinfo.o membuf.o strnote.o tagstack.o token.o \
           supio_win.o \
           hspcmp3.o
    LIBS = ole32
    DEFFILE = win32dll/hspcmp3.def

endif

#/*------------------------------------------*/

#/*-----------------------------------------------------------*/

#/*------------------------------------------*/

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

TARGET = $(filter *.o,$(MAKECMDGOALS)) $(filter *.exe,$(MAKECMDGOALS)) $(filter *.dll,$(MAKECMDGOALS))

ifeq ($(words $(TARGET)),0)
    TARGET = $(PF_TARGET)
endif

#/*-----------------------------------------------------------*/

EXECMODE =
RMFILE =

ifneq ($(filter cleanall,$(MAKECMDGOALS)),)
    EXECMODE = exec_clean
    RMFILE = *.o *.exe *.out *.dll
else
    ifeq ($(words $(PLATFORM)),1)
        EXECMODE = exec_target
    endif

    ifneq ($(filter clean,$(MAKECMDGOALS)),)
        EXECMODE = exec_clean
        RMFILE = $(OBJS) $(TARGET)
    endif

    ifneq ($(filter all,$(MAKECMDGOALS)),)
        EXECMODE = exec_alltarget
    endif

    ifneq ($(words $(PLATFORM)),1)
        EXECMODE = exec_error_platform
    endif
endif

#/*-----------------------------------------------------------*/

.SUFFIXES:
.SUFFIXES: .c .cpp .h .hpp .o

.PHONY: all clean cleanall release \
        linux win32_mingw win32dll_mingw \
        exec_alltarget exec_clean exec_error_platform exec_pre_alltarget exec_target

#/*-----------------------------------------------------------*/

linux win32_mingw win32dll_mingw all clean cleanall release: $(EXECMODE)
	@echo -n

#/*-------------------------------------------*/

exec_error_platform:
	@echo only one platform must be specified.
	@exit 1

exec_clean:
	$(RM) $(RMFILE)

exec_target: $(TARGET)
	@echo -n

exec_alltarget: exec_pre_alltarget $(TARGET)
	@echo -n

exec_pre_alltarget:
	$(RM) $(OBJS) $(TARGET)

#/*-------------------------------------------*/

hspcmp: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(LDFLAGS) -o $@

hspcmp.exe: $(OBJS)
	$(LD) $(OBJS) $(LDFLAGS) -o $@

hspcmp.dll: $(OBJS)
	$(LD) $(OBJS) $(LDFLAGS) -o $@

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

.cpp.o:
	$(CXX) $(CXXFLAGS) -c $< -o $@

#/*-------------------------------------------*/

#/*-----------------------------------------------------------*/

#/*-------------------------------------------*/
#
# ./
#

ahtmodel.o: ahtmodel.cpp supio.h ahtmodel.h membuf.h
ahtobj.o: ahtobj.cpp supio.h strnote.h ahtobj.h membuf.h ahtmodel.h
codegen.o: codegen.cpp hsp3config.h hsp3debug.h hsp3struct.h hspvar_core.h \
  hsp3debug.h supio.h token.h label.h tagstack.h membuf.h strnote.h \
  comutil.h errormsg.h
comutil.o: comutil.cpp hsp3config.h comutil.h
errormsg.o: errormsg.cpp hsp3config.h hsc3.h errormsg.h
hsc3.o: hsc3.cpp hsp3config.h hsp3debug.h hsp3struct.h hspvar_core.h \
  hsp3debug.h supio.h hsc3.h membuf.h strnote.h label.h token.h localinfo.h
hspcmd.o: hspcmd.cpp
label.o: label.cpp label.h
localinfo.o: localinfo.cpp localinfo.h
main.o: main.cpp hsp3config.h supio.h hsc3.h token.h
membuf.o: membuf.cpp membuf.h
strnote.o: strnote.cpp strnote.h
tagstack.o: tagstack.cpp tagstack.h
token.o: token.cpp hsp3config.h supio.h token.h label.h tagstack.h \
  membuf.h strnote.h ahtobj.h ahtmodel.h

#/*-------------------------------------------*/
#
# linux/
#

linux/supio_linux.o: supio_linux.h

#/*-------------------------------------------*/
#
# win32/
#

win32/supio_win.o: supio_win.cpp hsp3config.h supio_win.h

#/*-------------------------------------------*/
#
# win32dll/
#

win32dll/hspcmp3.o: hspcmp3.cpp hsp3config.h hsp3debug.h hsp3struct.h \
  hspvar_core.h hsp3debug.h hspwnd.h supio.h hsc3.h token.h ahtobj.h \
  membuf.h ahtmodel.h
