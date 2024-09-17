#ifndef WINDOW_H
#define WINDOW_H

#include <SDL.h>

typedef struct Window Window;
struct Window
{
    // Window size
    int width;
    int height;

    // SDL pointers
    SDL_Window *sdl_window;
    SDL_Renderer *renderer;
    SDL_GLContext context;
};

struct AWindow
{
    /**
     * Initializes a new window
     *
     * @param width Width of the window
     * @param height Height of the window
     * @param title Title of the window
     *
     * @return Window* Pointer to the window
     */
    Window *(*Init)(int width, int height, const char *title);

    // Destroys the window
    void (*Destroy)(Window *window);

    // Render the window's contents
    void (*Render)(Window *window);
};

extern struct AWindow AWindow;

#endif