#if defined(HSPLINUX) || defined(HSPEMSCRIPTEN)

#include "Base.h"
#include "Platform.h"
#include "FileSystem.h"
#include "Game.h"
#include "Form.h"
#include "ScriptController.h"

#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>


#ifdef HSPLINUX
#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
//#include "SDL2/SDL_opengl.h"
#else
#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
//#include "SDL2/SDL_opengl.h"
#endif

extern SDL_Window *window;

#define TOUCH_COUNT_MAX     4
#define MAX_GAMEPADS 4

using namespace std;

int __argc = 0;
char** __argv = 0;

struct timespec __timespec;
static double __timeStart;
static double __timeAbsolute;
static bool __vsync = WINDOW_VSYNC;
static bool __mouseCaptured = false;
static float __mouseCapturePointX = 0;
static float __mouseCapturePointY = 0;
static bool __multiSampling = false;
static bool __cursorVisible = true;
static int __windowSize[2];
static void* __attachToWindow;

namespace gameplay
{
    extern void print(const char* format, ...)
    {
        GP_ASSERT(format);
        va_list argptr;
        va_start(argptr, format);
        vfprintf(stderr, format, argptr);
        va_end(argptr);
    }

	extern int strcmpnocase(const char* s1, const char* s2)
	{
		return strcasecmp(s1, s2);
	}

    Platform::Platform(Game* game) : _game(game)
    {
    }

    Platform::~Platform()
    {
    }

    Platform* Platform::create(Game* game, void* attachToWindow, int sizex, int sizey, bool fullscreen)
    {

        GP_ASSERT(game);

        __attachToWindow = attachToWindow;
        FileSystem::setResourcePath("./");
        Platform* platform = new Platform(game);

        __windowSize[0] = sizex;
        __windowSize[1] = sizey;

        // Use OpenGL 2.x with GLEW
        glewExperimental = GL_TRUE;
        GLenum glewStatus = glewInit();
        if (glewStatus != GLEW_OK)
        {
            perror("glewInit");
            return NULL;
        }

        // GL Version
        int versionGL[2] = {-1, -1};
        glGetIntegerv(GL_MAJOR_VERSION, versionGL);
        glGetIntegerv(GL_MINOR_VERSION, versionGL + 1);
        printf("GL version: %d.%d\n", versionGL[0], versionGL[1]);

        return platform;
    }

    void cleanupX11()
    {
    }

    double timespec2millis(struct timespec *a)
    {
        GP_ASSERT(a);
        return (1000.0 * a->tv_sec) + (0.000001 * a->tv_nsec);
    }

    void updateWindowSize()
    {
        // GP_ASSERT(__display);
        // GP_ASSERT(__window);
        // XWindowAttributes windowAttrs;
        // XGetWindowAttributes(__display, __window, &windowAttrs);
        // __windowSize[0] = windowAttrs.width;
        // __windowSize[1] = windowAttrs.height;
    }

    int Platform::enterMessagePump()
    {
        GP_ASSERT(_game);

        updateWindowSize();

        // Get the initial time.
        clock_gettime(CLOCK_REALTIME, &__timespec);
        __timeStart = timespec2millis(&__timespec);
        __timeAbsolute = 0L;

        if (_game->getState() != Game::RUNNING)
            _game->run();

        if (__attachToWindow)
            return 0;

        // Do something

        return 0;
    }

    void Platform::signalShutdown()
    {
    }

    bool Platform::canExit()
    {
        return true;
    }

    unsigned int Platform::getDisplayWidth()
    {
        return __windowSize[0];
    }

    unsigned int Platform::getDisplayHeight()
    {
        return __windowSize[1];
    }

    double Platform::getAbsoluteTime()
    {

        clock_gettime(CLOCK_REALTIME, &__timespec);
        double now = timespec2millis(&__timespec);
        __timeAbsolute = now - __timeStart;

        return __timeAbsolute;
    }

    void Platform::setAbsoluteTime(double time)
    {
        __timeAbsolute = time;
    }

    bool Platform::isVsync()
    {
        return __vsync;
    }

    void Platform::setVsync(bool enable)
    {
        __vsync = enable;
    }

    void Platform::swapBuffers()
    {
#ifdef HSPLINUX
	  SDL_GL_SwapWindow(window);
#else
      SDL_GL_SwapBuffers();
#endif
    }

    void Platform::sleep(long ms)
    {
        usleep(ms * 1000);
    }

    void Platform::setMultiSampling(bool enabled)
    {
        if (enabled == __multiSampling)
        {
            return;
        }

        //todo
        __multiSampling = enabled;
    }

    bool Platform::isMultiSampling()
    {
        return __multiSampling;
    }

    void Platform::setMultiTouch(bool enabled)
    {
        // not supported
    }

    bool Platform::isMultiTouch()
    {
        return false;
    }

    bool Platform::hasAccelerometer()
    {
        return false;
    }

    void Platform::getAccelerometerValues(float* pitch, float* roll)
    {
        GP_ASSERT(pitch);
        GP_ASSERT(roll);

        *pitch = 0;
        *roll = 0;
    }

    void Platform::getSensorValues(float* accelX, float* accelY, float* accelZ, float* gyroX, float* gyroY, float* gyroZ)
    {
        if (accelX)
        {
            *accelX = 0;
        }

        if (accelY)
        {
            *accelY = 0;
        }

        if (accelZ)
        {
            *accelZ = 0;
        }

        if (gyroX)
        {
            *gyroX = 0;
        }

        if (gyroY)
        {
            *gyroY = 0;
        }

        if (gyroZ)
        {
            *gyroZ = 0;
        }
    }

    void Platform::getArguments(int* argc, char*** argv)
    {
        if (argc)
            *argc = __argc;
        if (argv)
            *argv = __argv;
    }

    bool Platform::hasMouse()
    {
        return true;
    }

    void Platform::setMouseCaptured(bool captured)
    {
        if (captured != __mouseCaptured)
        {
            // Todo

            __mouseCaptured = captured;
        }
    }

    bool Platform::isMouseCaptured()
    {
        return __mouseCaptured;
    }

    void Platform::setCursorVisible(bool visible)
    {
        if (visible != __cursorVisible)
        {
            // todo
            __cursorVisible = visible;
        }
    }

    bool Platform::isCursorVisible()
    {
        return __cursorVisible;
    }

    void Platform::displayKeyboard(bool display)
    {
        // not supported
    }

    void Platform::shutdownInternal()
    {
        //closeAllGamepads();
        Game::getInstance()->shutdown();
    }

    bool Platform::isGestureSupported(Gesture::GestureEvent evt)
    {
        return false;
    }

    void Platform::registerGesture(Gesture::GestureEvent evt)
    {
    }

    void Platform::unregisterGesture(Gesture::GestureEvent evt)
    {
    }

    bool Platform::isGestureRegistered(Gesture::GestureEvent evt)
    {
        return false;
    }

    void Platform::pollGamepadState(Gamepad* gamepad)
    {
    }

    bool Platform::launchURL(const char* url)
    {
        if (url == NULL || *url == '\0')
            return false;

        int len = strlen(url);

        // todo
        return false;
    }

	std::string Platform::displayFileDialog(size_t mode, const char* title, const char* filterDescription, const char* filterExtensions, const char* initialDirectory)
	{
	    std::string filename = "";
	    return filename;
	}

}

#endif
