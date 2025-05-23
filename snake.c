#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#define CELL_SIZE 20
#define SCREEN_WIDTH 1600
#define SCREEN_HEIGHT 1600
#define MAX_SNAKE_LENGTH 200
#define SCORE_PER_LEVEL 5



typedef struct {
    Vector2 position;
    Vector2 direction;
    int length;
    Vector2 tail[MAX_SNAKE_LENGTH];
} Snake;

typedef struct {
    Vector2 position;
} Food;

int score = 0, highScore = 0, level = 1;
bool gameOver = false, paused = false;

void SaveHighScore(int highScore) {
    FILE *file = fopen("highscore.json", "w");
    if (file) {
        fprintf(file, "{\"highscore\": %d}\n", highScore);
        fclose(file);
    }
}

void LoadHighScore() {
    FILE *file = fopen("highscore.json", "r");
    if (file) {
        fscanf(file, "{\"highscore\": %d}", &highScore);
        fclose(file);
    }
}

void ResetGame(Snake *snake, Food *food) {
    snake->position = (Vector2){10, 10};
    snake->direction = (Vector2){1, 0};
    snake->length = 1;
    score = 0;
    level = 1;
    gameOver = false;

    food->position = (Vector2){GetRandomValue(0, SCREEN_WIDTH / CELL_SIZE - 1),
                               GetRandomValue(0, SCREEN_HEIGHT / CELL_SIZE - 1)};
}

int main() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Snake Game - by ");
    InitAudioDevice();
    SetTargetFPS(10);

    Snake snake = {0};
    Food food = {0};

    LoadHighScore();
    ResetGame(&snake, &food);

    Sound eatSound = LoadSound("eat.wav");
    Sound gameOverSound = LoadSound("gameover.wav");

    while (!WindowShouldClose()) {
        // Controls
        if (!gameOver && !paused) {
            if ((IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) && snake.direction.y != 1)
                snake.direction = (Vector2){0, -1};
            if ((IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) && snake.direction.y != -1)
                snake.direction = (Vector2){0, 1};
            if ((IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A)) && snake.direction.x != 1)
                snake.direction = (Vector2){-1, 0};
            if ((IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D)) && snake.direction.x != -1)
                snake.direction = (Vector2){1, 0};
        }

        if (IsKeyPressed(KEY_P)) paused = !paused;
        if (gameOver && IsKeyPressed(KEY_R)) ResetGame(&snake, &food);

        // Game update
        if (!gameOver && !paused) {
            for (int i = snake.length - 1; i > 0; i--) {
                snake.tail[i] = snake.tail[i - 1];
            }
            if (snake.length > 0) snake.tail[0] = snake.position;

            snake.position.x += snake.direction.x;
            snake.position.y += snake.direction.y;

            // Check walls
            if (snake.position.x < 0 || snake.position.y < 0 ||
                snake.position.x >= SCREEN_WIDTH / CELL_SIZE ||
                snake.position.y >= SCREEN_HEIGHT / CELL_SIZE) {
                gameOver = true;
                PlaySound(gameOverSound);
            }

            // Check self collision
            for (int i = 0; i < snake.length; i++) {
                if (snake.tail[i].x == snake.position.x &&
                    snake.tail[i].y == snake.position.y) {
                    gameOver = true;
                    PlaySound(gameOverSound);
                    break;
                }
            }

            // Eat food
            if (snake.position.x == food.position.x &&
                snake.position.y == food.position.y) {
                if (snake.length < MAX_SNAKE_LENGTH) snake.length++;
                score++;
                PlaySound(eatSound);

                if (score > highScore) {
                    highScore = score;
                    SaveHighScore(highScore);
                }

                if (score % SCORE_PER_LEVEL == 0) {
                    level++;
                    SetTargetFPS(10 + level * 2);
                }

                food.position = (Vector2){GetRandomValue(0, SCREEN_WIDTH / CELL_SIZE - 1),
                                          GetRandomValue(0, SCREEN_HEIGHT / CELL_SIZE - 1)};
            }
        }

        // Drawing
        BeginDrawing();
        ClearBackground((Color){240, 248, 255, 255}); // Alice blue

        // Draw grid
        for (int x = 0; x < SCREEN_WIDTH; x += CELL_SIZE) {
            DrawLine(x, 0, x, SCREEN_HEIGHT, LIGHTGRAY);
        }
        for (int y = 0; y < SCREEN_HEIGHT; y += CELL_SIZE) {
            DrawLine(0, y, SCREEN_WIDTH, y, LIGHTGRAY);
        }

        // Draw snake
        DrawRectangle(snake.position.x * CELL_SIZE, snake.position.y * CELL_SIZE, CELL_SIZE, CELL_SIZE, DARKGREEN);
        for (int i = 0; i < snake.length; i++) {
            DrawRectangle(snake.tail[i].x * CELL_SIZE, snake.tail[i].y * CELL_SIZE, CELL_SIZE, CELL_SIZE, GREEN);
        }

        // Draw food
        DrawRectangle(food.position.x * CELL_SIZE, food.position.y * CELL_SIZE, CELL_SIZE, CELL_SIZE, RED);

        // Draw UI
        DrawText(TextFormat("Score: %d", score), 10, 10, 20, BLACK);
        DrawText(TextFormat("High Score: %d", highScore), 10, 30, 20, DARKGRAY);
        DrawText(TextFormat("Level: %d", level), SCREEN_WIDTH - 120, 10, 20, MAROON);

        if (paused) DrawText("PAUSED", SCREEN_WIDTH / 2 - 50, SCREEN_HEIGHT / 2 - 20, 40, ORANGE);
        if (gameOver) DrawText("GAME OVER! Press R to Restart", SCREEN_WIDTH / 2 - 180, SCREEN_HEIGHT / 2, 20, RED);

        EndDrawing();
    }



    UnloadSound(eatSound);
    UnloadSound(gameOverSound);
    CloseAudioDevice();
    CloseWindow();
    return 0;
}