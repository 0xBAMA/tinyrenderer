#include "tinyrenderer.h"
#include "debug.h"
// This contains the very high level expression of what's going on

tinyrenderer::tinyrenderer()
{
    pquit = false;

    create_window();
    gl_debug_enable();
    gl_setup();

    while(!pquit)
    {
        draw_everything();
    }
}

tinyrenderer::~tinyrenderer()
{
    quit();
}
