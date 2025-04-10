//--------------------------------------------------------------------------
//	JNI<->CPP bridge
//	2012/06 onitama/onion software
//--------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <android/log.h>
#include <jni.h>
#include <android/sensor.h>
#include <android/bitmap.h>

#include "javafunc.h"

#include "hsp3/hsp3config.h"
#include "hsp3/strbuf.h"
#include "hsp3dish/hsp3gr.h"
#include "hsp3/supio.h"
#include "hsp3embed/hsp3embed.h"

/*
static int jniRegisterNativeMethods(JNIEnv* env, const char* className,
    const JNINativeMethod* gMethods, int numMethods)
{
    jclass clazz;

    LOGD("Registering %s natives\n", className);
    clazz = (*env)->FindClass(env, className);
    if (clazz == NULL) {
        LOGD("Native registration unable to find class '%s'\n", className);
        return -1;
    }
    if ((*env)->RegisterNatives(env, clazz, gMethods, numMethods) < 0) {
        LOGD("RegisterNatives failed for '%s'\n", className);
        return -1;
    }
    return 0;
}
*/

static engine *m_engine;
static JavaVM *m_vm;

//--------------------------------------------------------------------------

static char *jcall_void_str( const char *name ) {
	//	call JavaMethod ( char*(void) )
    JNIEnv* env;
    char *p;

	m_vm->AttachCurrentThread(&env, NULL);
    jclass clazz = env->GetObjectClass(m_engine->app->activity->clazz);
    jmethodID methodj = env->GetMethodID(clazz, name, "()Ljava/lang/String;");
    jstring jstr = (jstring)env->CallObjectMethod(m_engine->app->activity->clazz, methodj);
    if (jstr != NULL) {
        char* str = (char *)env->GetStringUTFChars(jstr, NULL);
        p = hsp3eb_stmp( str );
        env->ReleaseStringUTFChars(jstr, str);
    }
	env->DeleteLocalRef( clazz );
	m_vm->DetachCurrentThread();
	return p;
}


static int jcall_int_int( const char *name, int val ) {
	//	call JavaMethod ( int (int) )
    JNIEnv* env;
    jint prm;

	prm = (jint)val;
	m_vm->AttachCurrentThread(&env, NULL);
    jclass clazz = env->GetObjectClass(m_engine->app->activity->clazz);

    jmethodID methodj = env->GetMethodID(clazz, name, "(I)I");
    jint value = env->CallIntMethod(m_engine->app->activity->clazz, methodj, prm);

	env->DeleteLocalRef( clazz );
	m_vm->DetachCurrentThread();
	return (int)value;
}


static int jcall_SSI_int( const char *name, char *str1, char *str2, int val ) {
	//	call JavaMethod ( int (char *,char *,int) )
    JNIEnv* env;
    jint prm;

	prm = (jint)val;
	m_vm->AttachCurrentThread(&env, NULL);
    jclass clazz = env->GetObjectClass(m_engine->app->activity->clazz);

    jmethodID methodj = env->GetMethodID(clazz, name, "(Ljava/lang/String;Ljava/lang/String;I)I");
    jint value = env->CallIntMethod(m_engine->app->activity->clazz, methodj, env->NewStringUTF(str1), env->NewStringUTF(str2), val );

	env->DeleteLocalRef( clazz );
	m_vm->DetachCurrentThread();
	return (int)value;
}


JNIEXPORT void JNICALL nativepoke( JNIEnv* env, jobject thiz, jint value, jint value2 )
{
	//		Callback from Java
	//
	HSPCTX *ctx;
	ctx = hsp3eb_getctx(); 
	ctx->runmode = RUNMODE_RUN;
	hsp3eb_setstat( (int)value2 );

	//int i;
	//i = hsp3eb_gettask();
	//LOGI( "[nativepoke(%d)(%d)] %d",value,value2, i );
}

//--------------------------------------------------------------------------

static const JNINativeMethod methods[] = {
	{ "nativepoke", "(II)V", (void *)nativepoke }
};

int javafunc_init( struct engine *en ) {

	//		setup Java enviroment
	//
    JNIEnv* env;
    JavaVM* vm = en->app->activity->vm;
    m_vm = vm;
    m_engine = en;

	//LOGI( "[Native->Java Ready]" );

	//		register native function
	//
	m_vm->AttachCurrentThread(&env, NULL);
	jclass clazz = env->GetObjectClass(m_engine->app->activity->clazz);
    if (env->RegisterNatives( clazz, methods, sizeof(methods)/sizeof(JNINativeMethod) ) < 0) {
        LOGW("RegisterNatives failed");
        return -1;
    }
	m_vm->DetachCurrentThread();

	//j_callVibrator( 400 );
	//LOGI( "DEVICE[%s]", j_getinfo(JAVAFUNC_INFO_DEVICE) );
	//LOGI( "VERSION[%s]", j_getinfo(JAVAFUNC_INFO_VERSION) );
	return 0;
}


struct engine *javafunc_engine( void ) {
	return m_engine;
}


char *j_getinfo( int type )
{
	switch( type ) {
	case JAVAFUNC_INFO_DEVICE:
		return jcall_void_str( "getInfo_Device" );
	case JAVAFUNC_INFO_VERSION:
		return jcall_void_str( "getInfo_Version" );
	case JAVAFUNC_INFO_FILESDIR:
		return jcall_void_str( "getInfo_FilesDir" );
	case JAVAFUNC_INFO_LOCALE:
		return jcall_void_str( "getInfo_Locale" );
	default:
		break;
	}
	return (char *)"";
}


int j_callVibrator( int val )
{
	return jcall_int_int( "callVibrator", val );
}


int j_dispDialog( char *msg1, char *msg2, int type )
{
	HSPCTX *ctx;
	ctx = hsp3eb_getctx(); 
	ctx->runmode = RUNMODE_STOP;

	return jcall_SSI_int( "dispDialog", msg1, msg2, type );
}


int j_callActivity( char *msg1, char *msg2, int type )
{
	int res;
	res = jcall_SSI_int( "callActivity", msg1, msg2, type );
	return res;
}


int j_addWindowFlag( int val )
{
	return jcall_int_int( "addWindowFlag", val );
}


int j_clearWindowFlag( int val )
{
	return jcall_int_int( "clearWindowFlag", val );
}


//--------------------------------------------------------------------------

char *j_callFontBitmap( const char *in_str, int fontSize, int style, int *o_width, int *o_height )
{
    JNIEnv* env;
	AndroidBitmapInfo info;
	bool bBold;
	void *pixels;
	char* rgba;
	int dsize;
	int ret;

	m_vm->AttachCurrentThread(&env, NULL);
    jclass clazz = env->GetObjectClass(m_engine->app->activity->clazz);

    jmethodID methodj = env->GetMethodID(clazz, "getFontBitmap", "([BIZ)Landroid/graphics/Bitmap;" );
	jbyteArray text = env->NewByteArray( strlen( in_str ) );
	env->SetByteArrayRegion( text, 0, strlen( in_str ), (jbyte*)in_str );

	bBold = true;
	jobject bitmap = env->CallObjectMethod( m_engine->app->activity->clazz, methodj, text, fontSize, bBold );

	ret = AndroidBitmap_getInfo( env, bitmap, &info );
	if( ret < 0 ){
		return NULL;
	}
	if ( info.format != ANDROID_BITMAP_FORMAT_A_8 &&
		 info.format != ANDROID_BITMAP_FORMAT_RGBA_8888 &&
		 info.format != ANDROID_BITMAP_FORMAT_RGBA_4444 ) {
		return NULL;
	}
	ret = AndroidBitmap_lockPixels( env, bitmap, &pixels );
	if( ret < 0 ){
		return NULL;
	}

	rgba  = NULL;
	dsize = 0;

	if( info.format == ANDROID_BITMAP_FORMAT_A_8){
		dsize = info.width * info.height;
	}
	else if( info.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ){
		dsize = info.width * info.height * 4;
	}
	else if( info.format == ANDROID_BITMAP_FORMAT_RGBA_4444 ){
		dsize = info.width * info.height * 2;
	}
	
	if ( dsize ) {
		rgba = (char *)malloc( dsize );
		memcpy( rgba, (char *)pixels, dsize );
	}

	AndroidBitmap_unlockPixels( env, bitmap );

	if( o_width != NULL ) *o_width = info.width;
	if( o_height != NULL ) *o_height = info.height;

	env->DeleteLocalRef( text );
	env->DeleteLocalRef( bitmap );
	env->DeleteLocalRef( clazz );

	m_vm->DetachCurrentThread();
	return rgba;
}


static jobject m_bitmap;
static jclass m_clazz;
static JNIEnv *m_env;

char *j_callFontBitmap_s( const char *in_str, int fontSize, int style, int *o_width, int *o_height, int *o_size, int *o_color )
{
	AndroidBitmapInfo info;
	bool bBold;
	void *pixels;
	int color;
	int dsize;
	int ret;

	m_vm->AttachCurrentThread(&m_env, NULL);
	m_clazz = m_env->GetObjectClass(m_engine->app->activity->clazz);

    jmethodID methodj = m_env->GetMethodID(m_clazz, "getFontBitmap", "([BIZ)Landroid/graphics/Bitmap;" );
	jbyteArray text = m_env->NewByteArray( strlen( in_str ) );
	m_env->SetByteArrayRegion( text, 0, strlen( in_str ), (jbyte*)in_str );

	bBold = true;
	m_bitmap = m_env->CallObjectMethod( m_engine->app->activity->clazz, methodj, text, fontSize, bBold );

	ret = AndroidBitmap_getInfo( m_env, m_bitmap, &info );
	if( ret < 0 ){
		return NULL;
	}
	if ( info.format != ANDROID_BITMAP_FORMAT_A_8 &&
		 info.format != ANDROID_BITMAP_FORMAT_RGBA_8888 &&
		 info.format != ANDROID_BITMAP_FORMAT_RGBA_4444 ) {
		return NULL;
	}
	ret = AndroidBitmap_lockPixels( m_env, m_bitmap, &pixels );
	if( ret < 0 ){
		return NULL;
	}

	dsize = 0;
	color = 0;

	if( info.format == ANDROID_BITMAP_FORMAT_A_8){
		dsize = info.width * info.height;
		color = 1;
	}
	else if( info.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ){
		dsize = info.width * info.height * 4;
		color = 4;
	}
	else if( info.format == ANDROID_BITMAP_FORMAT_RGBA_4444 ){
		dsize = info.width * info.height * 2;
		color = 2;
	}
	if( o_width != NULL ) *o_width = info.width;
	if( o_height != NULL ) *o_height = info.height;
	if( o_size != NULL ) *o_size = dsize;
	if( o_color != NULL ) *o_color = color;

	m_env->DeleteLocalRef( text );
	return (char *)pixels;
}

void j_callFontBitmap_e( void )
{
	AndroidBitmap_unlockPixels( m_env, m_bitmap );

	m_env->DeleteLocalRef( m_bitmap );
	m_env->DeleteLocalRef( m_clazz );

	m_vm->DetachCurrentThread();
}

//--------------------------------------------------------------------------

static void makePostParam( char *pair )
{
	unsigned char *p;
	unsigned char *st1;
	unsigned char *st2;
	unsigned char a1;
	unsigned char a2;
	unsigned char bak_a1;

	p = (unsigned char *)pair;
	st1 = p;
	st2 = NULL;

	while(1) {
		a1 = *p;
		if ( a1 == 0 ) break;
		if ( a1 == '=' ) {
			bak_a1 = a1; *p = 0;		// 終端を仮設定
			st2 = p + 1;
			break;
		}
/*		
		if (a1&128) {					// UTF8チェック
			while(1) {
				a2 = *p;
				if ( a2==0 ) break;
				if ( ( a2 & 0xc0 ) != 0x80 ) break;
				p++;
			}
		} else {
			p++;
		}
*/
		p++;
	}

	if ( st2 == NULL ) {
		jcall_SSI_int( "httpParamSet", (char *)st1, "", 1 );
		return;
	}
	//Alertf( "%s:%s", (char *)st1, (char *)st2 );
	jcall_SSI_int( "httpParamSet", (char *)st1, (char *)st2, 1 );
	*p = bak_a1;
}


static void makePostParamPair( char *param )
{
	int spcur;
	unsigned char *p;
	unsigned char *st;
	unsigned char a1;
	unsigned char a2;
	unsigned char bak_a1;


	p = (unsigned char *)param;
	st = p;
	spcur = 0;
	jcall_SSI_int( "httpParamSet", "", "", 0 );	// パラメーターをリセット

	while(1) {
		a1 = *p;
		if ( a1 == 0 ) break;
		if ( a1 == '&' ) {
			bak_a1 = a1; *p = 0;		// 終端を仮設定
			makePostParam( (char *)st );
			*p = bak_a1;
			p++; st = p; spcur = 0;		// 終端を戻す
			continue;
		}
/*		
		if (a1&128) {					// UTF8チェック
			while(1) {
				a2 = *p;
				if ( a2==0 ) break;
				if ( ( a2 & 0xc0 ) != 0x80 ) break;
				p++; spcur++;
			}
		} else {
			p++; spcur++;
		}
*/
		p++; spcur++;
	}

	if ( spcur > 0 ) {
		makePostParam( (char *)st );
	}
}


int j_httpRequest( char *msg1, char *msg2, int type )
{
	if ( msg2 == NULL ) {
		return jcall_SSI_int( "httpRequestGET", msg1, "", type );
	}

	makePostParamPair( msg2 );
	return jcall_SSI_int( "httpRequestPOST", msg1, "", type );
}

char *j_getHttpInfo( void )
{
	return jcall_void_str( "getHttpResult" );
}

//--------------------------------------------------------------------------

int j_callAdMob( int val )
{
		return jcall_int_int( "callAdMob", val );
}



