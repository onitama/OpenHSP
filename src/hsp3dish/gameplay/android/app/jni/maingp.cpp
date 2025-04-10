
#include <jni.h>
#include <unistd.h>
#include <android/sensor.h>

#include "javafunc.h"

#include <stdio.h>
#include <stdlib.h>

#include "hsp3/hsp3config.h"
#include "hsp3/hsp3struct.h"
#include "hsp3/hsp3ext.h"
#include "hsp3embed/hsp3embed.h"
#include "hsp3dish/hgio.h"
#include "hsp3dish/sysreq.h"

//#define USE_SENSOR


void destroyEGLSurface();
int initEGL();
void destroyEGLMain();
unsigned int getEGLDisplayWidth( void );
unsigned int getEGLDisplayHeight( void );


extern struct android_app* __state;

static int *p_runmode;

#ifdef USE_SENSOR
static ASensorManager* __sensorManager;
static ASensorEventQueue* __sensorEventQueue;
static ASensorEvent __sensorEvent;
static const ASensor* __accelerometerSensor;
static const ASensor* __gyroscopeSensor;
#endif



/**
 * 入力イベントを処理する
 */
static int32_t engine_handle_input(struct android_app* app,
        AInputEvent* event) {
    struct engine* engine = (struct engine*) app->userData;
    if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_MOTION) {

        int i,keyid,pid;
		int xx,yy;
		int acttype;
        int action = AKeyEvent_getAction(event);
        acttype = action & AMOTION_EVENT_ACTION_MASK;
        size_t count = AMotionEvent_getPointerCount(event);
        //LOGI("***INPUT(TYPE%d)x%d",acttype, count);
        for (i = 0; i < count; i++){

            if (acttype == AMOTION_EVENT_ACTION_POINTER_DOWN || acttype == AMOTION_EVENT_ACTION_POINTER_UP) {
		        pid = ( action & AMOTION_EVENT_ACTION_POINTER_INDEX_MASK )>>AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT;
            } else {
				pid = i;
			}
            keyid = AMotionEvent_getPointerId(event, pid);
			xx = AMotionEvent_getX(event, pid);
			yy = AMotionEvent_getY(event, pid);
			if ( i == 0 ) {
		        engine->state.x = xx;
		        engine->state.y = yy;
		    }

	        switch (acttype) {
			case AMOTION_EVENT_ACTION_DOWN:
	        case AMOTION_EVENT_ACTION_MOVE:
			case AMOTION_EVENT_ACTION_POINTER_DOWN:
		        //LOGI("***INPUT(#%d,%d)",i,keyid);
		        hgio_mtouchid( keyid, xx, yy, 1, i );
				break;
	        case AMOTION_EVENT_ACTION_UP:
	        case AMOTION_EVENT_ACTION_POINTER_UP:
		        //LOGI("***INPUT(#%d,%d)",i,keyid);
		        hgio_mtouchid( keyid, xx, yy, 0, i );
				break;
			}

	    }
        return 1;
    }
    return 0;
}

/**
 * メインコマンドの処理
 */
static void engine_handle_cmd(struct android_app* app, int32_t cmd) {
    struct engine* engine = (struct engine*) app->userData;
    switch (cmd) {
    case APP_CMD_SAVE_STATE:
        engine->app->savedState = malloc(sizeof(struct saved_state));
        *((struct saved_state*) engine->app->savedState) = engine->state;
        engine->app->savedStateSize = sizeof(struct saved_state);
        break;
    case APP_CMD_INIT_WINDOW:
       	LOGI("***CMD_INIT_WINDOW(%x)",engine->app->window);
        if (engine->app->window != NULL) {

	       	//LOGI("[initEGL]");
            initEGL();
		    engine->width = getEGLDisplayWidth();
		    engine->height = getEGLDisplayHeight();
	       	//LOGI("[initEGL done]");

			javafunc_init( engine );

            //engine_init_display(engine);
            if ( engine->hspctx == NULL ) {
				HSPCTX *ctx;
				InitSysReq();
			  	//LOGI("[HSP hgio_init %d,%d]",engine->width, engine->height);
			    hgio_init( 0, engine->width, engine->height, engine );
				//hgio_view( 600, 1024 );	// screen size
				//hgio_scale( 1.0f, 1.0f );	// scale value
				//hgio_autoscale( 0 );		// auto scale value

		       	//LOGI("[HSP hsp3eb_init]");
				hsp3eb_init();
		       	//LOGI("[HSP hsp3eb_init OK]");
				ctx = hsp3eb_getctx();
				engine->hspctx = ctx;
		       	//LOGI("[HSP p_runmode]");
				p_runmode = &(ctx->runmode);
		       	//LOGI("[HSP hgio_setstorage]");
				hgio_setstorage( "" );
				//hgio_setstorage( j_getinfo(JAVAFUNC_INFO_FILESDIR) );
		       	//LOGI("[HSP hsp3dish_setdevinfo]");
			    hsp3dish_setdevinfo();
		       	//LOGI("[HSP hsp3dish_setdevinfo ok]");

			} else {
		       	LOGI("[HSP Resume]");
		        //hsp3eb_resume();
			}
            //engine_draw_frame(engine);
	        //engine->animating = 1;

        }
        break;

    case APP_CMD_TERM_WINDOW:
       	//LOGI("***CMD_TERM_WINDOW");
        engine->animating = 0;
        //engine_term_display(engine);
        hsp3eb_pause();

        break;
    case APP_CMD_GAINED_FOCUS:
       	LOGI("***CMD_GAINED_FOCUS");
#ifdef USE_SENSOR
        // When our app gains focus, we start monitoring the sensors.
        if (__accelerometerSensor != NULL) 
        {
            ASensorEventQueue_enableSensor(__sensorEventQueue, __accelerometerSensor);
            // We'd like to get 60 events per second (in microseconds).
            ASensorEventQueue_setEventRate(__sensorEventQueue, __accelerometerSensor, (1000L/60)*1000);
        }
        if (__gyroscopeSensor != NULL)
        {
            ASensorEventQueue_enableSensor(__sensorEventQueue, __gyroscopeSensor);
            // We'd like to get 60 events per second (in microseconds).
            ASensorEventQueue_setEventRate(__sensorEventQueue, __gyroscopeSensor, (1000L/60)*1000);
        }
#endif
        engine->animating = 1;
        break;
    case APP_CMD_LOST_FOCUS:
       	LOGI("***CMD_LOST_FOCUS");
#ifdef USE_SENSOR
        // When our app loses focus, we stop monitoring the sensors.
        // This is to avoid consuming battery while not being used.
        if (__accelerometerSensor != NULL)
        {
            ASensorEventQueue_disableSensor(__sensorEventQueue, __accelerometerSensor);
        }
        if (__gyroscopeSensor != NULL)
        {
            ASensorEventQueue_disableSensor(__sensorEventQueue, __gyroscopeSensor);
        }
        engine->animating = 0;
#endif
        break;
    }
}

/**
 * アプリケーション開始
 */
void android_main(struct android_app* state) {
    struct engine engine;
 
    // glueが削除されないように
    app_dummy();

    memset(&engine, 0, sizeof(engine));
    state->userData = &engine;
    state->onAppCmd = engine_handle_cmd;
    state->onInputEvent = engine_handle_input;
    engine.app = state;
    __state = state;

    engine.hspctx = NULL;
    p_runmode = NULL;

#ifdef USE_SENSOR
    // Prepare to monitor accelerometer.
    __sensorManager = ASensorManager_getInstance();
    __accelerometerSensor = ASensorManager_getDefaultSensor(__sensorManager, ASENSOR_TYPE_ACCELEROMETER);
    __gyroscopeSensor = ASensorManager_getDefaultSensor(__sensorManager, ASENSOR_TYPE_GYROSCOPE);
    __sensorEventQueue = ASensorManager_createEventQueue(__sensorManager, __state->looper, LOOPER_ID_USER, NULL, NULL);
#endif

    if (state->savedState != NULL) {
        // 以前の状態に戻す
        engine.state = *(struct saved_state*) state->savedState;
    }
    while (1) {

        int ident;
        int events;
        struct android_poll_source* source;

        // アプリケーションが動作することになれば、これらセンサーの制御を行う
        while ((ident = ALooper_pollAll(engine.animating ? 0 : -1, NULL,
                &events, (void**) &source)) >= 0) {

            // イベントを処理する
            if (source != NULL) {
                source->process(state, source);
            }

#ifdef USE_SENSOR
            // If a sensor has data, process it now.
            if (ident == LOOPER_ID_USER && __accelerometerSensor != NULL)
            {
                ASensorEventQueue_getEvents(__sensorEventQueue, &__sensorEvent, 1);
                if (__sensorEvent.type == ASENSOR_TYPE_ACCELEROMETER)
                {
                    hgio_setinfo( GINFO_EXINFO_ACCEL_X, __sensorEvent.acceleration.x );
                    hgio_setinfo( GINFO_EXINFO_ACCEL_Y, __sensorEvent.acceleration.y );
                    hgio_setinfo( GINFO_EXINFO_ACCEL_Z, __sensorEvent.acceleration.z );
                }
                else if (__sensorEvent.type == ASENSOR_TYPE_GYROSCOPE)
                {
                    hgio_setinfo( GINFO_EXINFO_GYRO_X, __sensorEvent.vector.x );
                    hgio_setinfo( GINFO_EXINFO_GYRO_Y, __sensorEvent.vector.y );
                    hgio_setinfo( GINFO_EXINFO_GYRO_Z, __sensorEvent.vector.z );
                }
            }
#endif

            // 破棄要求があったか
            if (state->destroyRequested != 0) {
				*p_runmode = RUNMODE_END;
				break;
            }

        }

        if (engine.animating) {
            // 画面描画
			hsp3eb_exectime( hgio_gettick() );
        }
        
        
         
        
			if ( (*p_runmode) >= RUNMODE_END ) {
//				if ( (*p_runmode) == RUNMODE_ERROR ) {
//					j_dispDialog( "HSPError", "Bye!", 0 );
//				}
				break;
			}
			
			
    }

    // 破棄
//	LOGI("[HSP Close]");

	hsp3eb_bye();
    hgio_term();
	destroyEGLMain();
//	engine_term_display(&engine);

	exit(0);
	//return;
}



