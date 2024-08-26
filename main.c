#include <SDL.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <stdio.h>

#define WIDTH 640
#define HEIGHT 480
#define NUM_CIRCLES 100
#define CIRCLE_RADIUS 10
#define SPEED 3

typedef struct {
    int x, y;
    int dx, dy;
    SDL_Color color;
} Circle;

void initCircle(Circle *circle) {
    circle->x = rand() % (WIDTH - 2 * CIRCLE_RADIUS) + CIRCLE_RADIUS;
    circle->y = rand() % (HEIGHT - 2 * CIRCLE_RADIUS) + CIRCLE_RADIUS;
    circle->dx = (rand() % (2 * SPEED)) - SPEED;
    circle->dy = (rand() % (2 * SPEED)) - SPEED;
    circle->color.r = rand() % 256;
    circle->color.g = rand() % 256;
    circle->color.b = rand() % 256;
    circle->color.a = 255;
}

void drawCircle(SDL_Renderer *renderer, Circle *circle) {
    SDL_SetRenderDrawColor(renderer, circle->color.r, circle->color.g, circle->color.b, circle->color.a);
    for (int w = 0; w < CIRCLE_RADIUS * 2; w++) {
        for (int h = 0; h < CIRCLE_RADIUS * 2; h++) {
            int dx = CIRCLE_RADIUS - w; // Horizontal offset
            int dy = CIRCLE_RADIUS - h; // Vertical offset
            if ((dx * dx + dy * dy) <= (CIRCLE_RADIUS * CIRCLE_RADIUS)) {
                SDL_RenderDrawPoint(renderer, circle->x + dx, circle->y + dy);
            }
        }
    }
}

void updateCircle(Circle *circle) {
    circle->x += circle->dx;
    circle->y += circle->dy;

    if (circle->x < CIRCLE_RADIUS || circle->x > WIDTH - CIRCLE_RADIUS) {
        circle->dx = -circle->dx;
    }
    if (circle->y < CIRCLE_RADIUS || circle->y > HEIGHT - CIRCLE_RADIUS) {
        circle->dy = -circle->dy;
    }
}

int main(int argc, char *argv[]) {
    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window *window = SDL_CreateWindow("Screensaver", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    srand(time(NULL));
    Circle circles[NUM_CIRCLES];
    for (int i = 0; i < NUM_CIRCLES; i++) {
        initCircle(&circles[i]);
    }

    int running = 1;
    Uint32 startTime, endTime;
    float fps;
    char title[100];

    while (running) {
        startTime = SDL_GetTicks();

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = 0;
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        for (int i = 0; i < NUM_CIRCLES; i++) {
            updateCircle(&circles[i]);
            drawCircle(renderer, &circles[i]);
        }

        SDL_RenderPresent(renderer);

        endTime = SDL_GetTicks();
        fps = 1000.0 / (endTime - startTime);

        // Actualizar el título de la ventana con los FPS
        snprintf(title, sizeof(title), "Screensaver - FPS: %.2f", fps);
        SDL_SetWindowTitle(window, title);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
