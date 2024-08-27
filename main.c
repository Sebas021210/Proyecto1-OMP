#include <SDL.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <stdio.h>
#include <omp.h>

#define WIDTH 640
#define HEIGHT 480
#define NUM_CIRCLES 1000
#define CIRCLE_RADIUS 5
#define SPEED 2
#define RESET_INTERVAL 5000
#define DURATION 15000

typedef struct {
    float x, y;
    float dx, dy;
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

void resetCircleSpeed(Circle *circle) {
    circle->dx = (rand() % (2 * SPEED)) - SPEED;
    circle->dy = (rand() % (2 * SPEED)) - SPEED;
}

void drawCircle(SDL_Renderer *renderer, Circle *circle) {
    SDL_SetRenderDrawColor(renderer, circle->color.r, circle->color.g, circle->color.b, circle->color.a);
    for (int w = 0; w < CIRCLE_RADIUS * 2; w++) {
        for (int h = 0; h < CIRCLE_RADIUS * 2; h++) {
            int dx = CIRCLE_RADIUS - w;
            int dy = CIRCLE_RADIUS - h;
            if ((dx * dx + dy * dy) <= (CIRCLE_RADIUS * CIRCLE_RADIUS)) {
                SDL_RenderDrawPoint(renderer, circle->x + dx, circle->y + dy);
            }
        }
    }
}

void updateCircles(Circle *circles, int num_circles) {
    #pragma omp parallel for
    for (int i = 0; i < num_circles; i++) {
        circles[i].x += circles[i].dx;
        circles[i].y += circles[i].dy;

        if (circles[i].x < CIRCLE_RADIUS || circles[i].x > WIDTH - CIRCLE_RADIUS) {
            circles[i].dx = -circles[i].dx;
        }
        if (circles[i].y < CIRCLE_RADIUS || circles[i].y > HEIGHT - CIRCLE_RADIUS) {
            circles[i].dy = -circles[i].dy;
        }
    }
}

void checkCollisions(Circle *circles, int num_circles) {
    #pragma omp parallel for schedule(dynamic)
    for (int i = 0; i < num_circles; i++) {
        for (int j = i + 1; j < num_circles; j++) {
            float dx = circles[i].x - circles[j].x;
            float dy = circles[i].y - circles[j].y;
            float distance = sqrt(dx * dx + dy * dy);

            if (distance < 2 * CIRCLE_RADIUS) {
                float angle = atan2(dy, dx);
                float sin_angle = sin(angle);
                float cos_angle = cos(angle);

                float vxi = circles[i].dx * cos_angle + circles[i].dy * sin_angle;
                float vyi = -circles[i].dx * sin_angle + circles[i].dy * cos_angle;
                float vxj = circles[j].dx * cos_angle + circles[j].dy * sin_angle;
                float vyj = -circles[j].dx * sin_angle + circles[j].dy * cos_angle;

                float temp = vxi;
                vxi = vxj;
                vxj = temp;

                circles[i].dx = vxi * cos_angle - vyi * sin_angle;
                circles[i].dy = vxi * sin_angle + vyi * cos_angle;
                circles[j].dx = vxj * cos_angle - vyj * sin_angle;
                circles[j].dy = vxj * sin_angle + vyj * cos_angle;
            }
        }
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
    Uint32 lastResetTime = SDL_GetTicks();
    Uint32 windowStartTime = SDL_GetTicks();
    float fps;
    char title[100];

    float minFps = 1000.0, maxFps = 0.0, avgFps = 0.0;
    int frameCount = 0;

    while (running) {
        startTime = SDL_GetTicks();

        if (SDL_GetTicks() - windowStartTime >= DURATION) {
            running = 0;
            break;
        }

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = 0;
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        updateCircles(circles, NUM_CIRCLES);
        checkCollisions(circles, NUM_CIRCLES);

        #pragma omp parallel for
        for (int i = 0; i < NUM_CIRCLES; i++) {
            #pragma omp critical
            drawCircle(renderer, &circles[i]);
        }

        if (SDL_GetTicks() - lastResetTime > RESET_INTERVAL) {
            #pragma omp parallel for
            for (int i = 0; i < NUM_CIRCLES; i++) {
                resetCircleSpeed(&circles[i]);
            }
            lastResetTime = SDL_GetTicks();
        }

        SDL_RenderPresent(renderer);

        endTime = SDL_GetTicks();
        fps = 1000.0 / (endTime - startTime);
        avgFps += fps;
        frameCount++;

        if (fps < minFps) minFps = fps;
        if (fps > maxFps) maxFps = fps;

        snprintf(title, sizeof(title), "Screensaver - FPS: %.2f", fps);
        SDL_SetWindowTitle(window, title);
    }

    avgFps /= frameCount;

    FILE *file = fopen("fps_data_paralell.txt", "w");
    if (file) {
        fprintf(file, "Min FPS: %.2f\nMax FPS: %.2f\nAvg FPS: %.2f\n", minFps, maxFps, avgFps);
        fclose(file);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}