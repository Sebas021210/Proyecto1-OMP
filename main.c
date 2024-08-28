#include <SDL.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <stdio.h>
#include <omp.h>

#define WIDTH 640
#define HEIGHT 480
#define NUM_CIRCLES 50
#define CIRCLE_RADIUS 10
#define MAX_RADIUS 30
#define SPEED 3
#define RESET_INTERVAL 5000
#define DURATION 15000
#define COLLISIONS_TO_GROW 10

typedef struct {
    float x, y;
    float dx, dy;
    SDL_Color color;
    int radius;
    int collisionCount;
    int alive;
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
    circle->radius = CIRCLE_RADIUS;
    circle->collisionCount = 0;
    circle->alive = 1;
}

void resetCircleSpeed(Circle *circle) {
    circle->dx = (rand() % (2 * SPEED)) - SPEED;
    circle->dy = (rand() % (2 * SPEED)) - SPEED;
}

void drawCircle(SDL_Renderer *renderer, Circle *circle) {
    SDL_SetRenderDrawColor(renderer, circle->color.r, circle->color.g, circle->color.b, circle->color.a);
    for (int w = 0; w < circle->radius * 2; w++) {
        for (int h = 0; h < circle->radius * 2; h++) {
            int dx = circle->radius - w;
            int dy = circle->radius - h;
            if ((dx * dx + dy * dy) <= (circle->radius * circle->radius)) {
                SDL_RenderDrawPoint(renderer, circle->x + dx, circle->y + dy);
            }
        }
    }
}

void updateCircles(Circle *circles, int num_circles) {
#pragma omp parallel for
    for (int i = 0; i < num_circles; i++) {
        if (circles[i].alive) {
            circles[i].x += circles[i].dx;
            circles[i].y += circles[i].dy;

            if (circles[i].x < circles[i].radius || circles[i].x > WIDTH - circles[i].radius) {
                circles[i].dx = -circles[i].dx;
            }
            if (circles[i].y < circles[i].radius || circles[i].y > HEIGHT - circles[i].radius) {
                circles[i].dy = -circles[i].dy;
            }
        }
    }
}

void checkCollisions(Circle *circles, int num_circles) {
#pragma omp parallel for schedule(static)
    for (int i = 0; i < num_circles; i++) {
        if (!circles[i].alive) continue;
        for (int j = i + 1; j < num_circles; j++) {
            if (!circles[j].alive) continue;

            float dx = circles[i].x - circles[j].x;
            float dy = circles[i].y - circles[j].y;
            float distance = sqrt(dx * dx + dy * dy);

            if (distance < circles[i].radius + circles[j].radius) {
                circles[i].collisionCount++;
                circles[j].collisionCount++;

                if (circles[i].collisionCount >= COLLISIONS_TO_GROW && circles[i].radius < MAX_RADIUS) {
                    circles[i].radius += 5;
                    if (circles[i].radius > MAX_RADIUS) circles[i].radius = MAX_RADIUS;
                    circles[i].collisionCount = 0;
                    circles[j].alive = 0;

                    int newCircleIndex = -1;
                    for (int k = 0; k < num_circles; k++) {
                        if (!circles[k].alive) {
                            newCircleIndex = k;
                            break;
                        }
                    }
                    if (newCircleIndex != -1) {
                        initCircle(&circles[newCircleIndex]);
                    }
                } else if (circles[j].collisionCount >= COLLISIONS_TO_GROW && circles[j].radius < MAX_RADIUS) {
                    circles[j].radius += 5;
                    if (circles[j].radius > MAX_RADIUS) circles[j].radius = MAX_RADIUS;
                    circles[j].collisionCount = 0;
                    circles[i].alive = 0;

                    int newCircleIndex = -1;
                    for (int k = 0; k < num_circles; k++) {
                        if (!circles[k].alive) {
                            newCircleIndex = k;
                            break;
                        }
                    }
                    if (newCircleIndex != -1) {
                        initCircle(&circles[newCircleIndex]);
                    }
                }

                float angle = atan2(dy, dx);
                float sin_angle = sin(angle);
                float cos_angle = cos(angle);

                float vxi = circles[i].dx * cos_angle + circles[i].dy * sin_angle;
                float vyi = -circles[i].dx * sin_angle + circles[i].dy * cos_angle;
                float vxj = circles[j].dx * cos_angle + circles[j].dy * sin_angle;
                float vyj = -circles[j].dx * sin_angle + circles[j].dy * cos_angle;

                float new_dxi = vxi * cos_angle - vyi * sin_angle;
                float new_dyi = vxi * sin_angle + vyi * cos_angle;
                float new_dxj = vxj * cos_angle - vyj * sin_angle;
                float new_dyj = vxj * sin_angle + vyj * cos_angle;

#pragma omp critical
                {
                    circles[i].dx = new_dxi;
                    circles[i].dy = new_dyi;
                    circles[j].dx = new_dxj;
                    circles[j].dy = new_dyj;
                }
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

    double total_update_time = 0.0;
    double total_collision_time = 0.0;
    double total_draw_time = 0.0;
    int measurement_count = 0;

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

        double update_start_time = omp_get_wtime();
        updateCircles(circles, NUM_CIRCLES);
        double update_end_time = omp_get_wtime();
        total_update_time += update_end_time - update_start_time;

        double collision_start_time = omp_get_wtime();
        checkCollisions(circles, NUM_CIRCLES);
        double collision_end_time = omp_get_wtime();
        total_collision_time += collision_end_time - collision_start_time;

        double draw_start_time = omp_get_wtime();
        for (int i = 0; i < NUM_CIRCLES; i++) {
            if (circles[i].alive) {
                drawCircle(renderer, &circles[i]);
            }
        }
        double draw_end_time = omp_get_wtime();
        total_draw_time += draw_end_time - draw_start_time;

        measurement_count++;

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

    double avg_draw_time = total_draw_time / measurement_count;

    FILE *file = fopen("performance_data_parallel.txt", "w");
    if (file) {
        fprintf(file, "Performance Data:\n");
        fprintf(file, "Min FPS: %.2f\n", minFps);
        fprintf(file, "Max FPS: %.2f\n", maxFps);
        fprintf(file, "Avg FPS: %.2f\n", avgFps);
        fprintf(file, "Avg Draw Time: %.6f seconds\n", avg_draw_time);
        fclose(file);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
