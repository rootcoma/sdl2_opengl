#include "gui/event.h"
#include "gui/window.h"
#include "scene.h"
#include "util/log.h"

//#ifdef _WIN32
// This undef workaround is because of SDL2 "magic"
#undef main
//#endif
int main(int argc, char **argv)
{
    Success("Hello, World!");
    CreateWindow();
    SceneInit();
    RunEventLoop();
    DestroyWindow();
    Success("Bye!");
    return 0;
}
