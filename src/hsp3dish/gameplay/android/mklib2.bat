echo off
ndk-build NDK_PROJECT_PATH=. APP_BUILD_SCRIPT=jni/Android.mk APP_ABI=armeabi-v7a obj/local/armeabi-v7a/libgameplay.a
