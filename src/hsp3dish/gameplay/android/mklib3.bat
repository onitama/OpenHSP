echo off
ndk-build NDK_PROJECT_PATH=. APP_BUILD_SCRIPT=jni/Android.mk APP_ABI=x86 obj/local/x86/libgameplay.a
