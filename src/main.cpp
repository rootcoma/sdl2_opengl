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
    if (!CreateWindow()) {
        Error("Failed to create a window, "
            "that's going to be a hinderance.");
        return -1;
    }
    if(!SceneInit()) {
        Error("Failed to initialize the scene");
        return -2;
    }
    RunEventLoop();
    DestroyWindow();
    Success("Bye!");
    return EXIT_SUCCESS;
}
