#include <window.h>

#include <logger.h>

#ifdef _WIN32
#include <windows.h>
#endif

static Window *Init(int width, int height, char *title)
{
    Window *window = malloc(sizeof(Window));
    if (!window)
    {
        log_message(LOG_ERROR, "Failed to allocate memory for Window");
        return NULL;
    }

    window->width = width;
    window->height = height;

    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        log_message(LOG_ERROR, "SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return NULL;
    }

    log_message(LOG_INFO, "SDL initialized successfully");

    // Create transparent window
    window->sdl_window = SDL_CreateWindow(title,
                                          SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                          width, height,
                                          SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

    if (!window->sdl_window)
    {
        log_message(LOG_ERROR, "Failed to create window SDL_Error: %s", SDL_GetError());
        free(window);
        return NULL;
    }

    // Create a renderer with transparent capability
    window->renderer = SDL_CreateRenderer(window->sdl_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!window->renderer)
    {
        log_message(LOG_ERROR, "Failed to create renderer");
        return NULL;
    }

    log_message(LOG_INFO, "Window and renderer initialized successfully");

    return window;
}

extern struct AWindow AWindow;
struct AWindow AWindow =
    {
        .Init = Init,
};