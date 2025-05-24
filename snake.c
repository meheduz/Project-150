#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#define CELL_SIZE 20
#define SCREEN_WIDTH 1200
#define SCREEN_HEIGHT 800
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

char playerName[64] = "";
char department[64] = "";
char regNo[64] = "";
char gamerID[128] = "";
bool inputComplete = false;
bool gameStarted = false;

int score = 0, highScore = 0, level = 1;
bool gameOver = false, paused = false;

void SaveHighScore(int highScore, const char *gamerID) {
    FILE *file = fopen("highscore.json", "w");
    if (file) {
        fprintf(file, "{\"highscore\": %d, \"gamer_id\": \"%s\"}\n", highScore, gamerID);
        fclose(file);
    }
}

void LoadHighScore() {
    FILE *file = fopen("highscore.json", "r");
    if (file) {
        fscanf(file, "{\\\"highscore\\\": %d, \\\"gamer_id\\\": \\\"%127[^\"]\\\"}", &highScore, gamerID);
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

void GenerateGamerID() {
    char first[32] = "";
    sscanf(playerName, "%s", first);
    snprintf(gamerID, sizeof(gamerID), "%s_%s", first, department);

    if (strlen(regNo) >= 10) {
        char regPart[16];
        snprintf(regPart, sizeof(regPart), "_%c%c_%c%c%c",
                 regNo[2], regNo[3], regNo[7], regNo[8], regNo[9]);
        strncat(gamerID, regPart, sizeof(gamerID) - strlen(gamerID) - 1);
    }

    for (int i = 0; gamerID[i]; i++) gamerID[i] = tolower((unsigned char)gamerID[i]);
}

int main() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Snake Game - by Meheduz");
    InitAudioDevice();
    SetTargetFPS(10);

    Snake snake = {0};
    Food food = {0};

    LoadHighScore();

    Sound eatSound = LoadSound("eat.wav");
    Sound gameOverSound = LoadSound("gameover.wav");

    int inputStage = 0;
    char inputBuffer[64] = "";
    int inputIndex = 0;

    Rectangle startButton = { SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 + 180, 200, 50 };

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        if (!inputComplete) {
            DrawText("Enter your details:", 100, 50, 30, BLACK);
            if (inputStage == 0) {
                DrawText("Name:", 100, 120, 20, BLACK);
                DrawText(inputBuffer, 250, 120, 20, DARKBLUE);
            } else if (inputStage == 1) {
                DrawText(TextFormat("Name: %s", playerName), 100, 120, 20, BLACK);
                DrawText("Department:", 100, 160, 20, BLACK);
                DrawText(inputBuffer, 300, 160, 20, DARKBLUE);
            } else if (inputStage == 2) {
                DrawText(TextFormat("Name: %s", playerName), 100, 120, 20, BLACK);
                DrawText(TextFormat("Department: %s", department), 100, 160, 20, BLACK);
                DrawText("Registration No:", 100, 200, 20, BLACK);
                DrawText(inputBuffer, 350, 200, 20, DARKBLUE);
            }

            int key = GetCharPressed();
            if (key >= 32 && key <= 125 && inputIndex < 63) {
                inputBuffer[inputIndex++] = (char)key;
                inputBuffer[inputIndex] = '\0';
            }
            if (IsKeyPressed(KEY_BACKSPACE) && inputIndex > 0) {
                inputBuffer[--inputIndex] = '\0';
            }

            if (IsKeyPressed(KEY_ENTER)) {
                if (inputStage == 0) {
                    strncpy(playerName, inputBuffer, sizeof(playerName));
                } else if (inputStage == 1) {
                    strncpy(department, inputBuffer, sizeof(department));
                } else if (inputStage == 2) {
                    strncpy(regNo, inputBuffer, sizeof(regNo));
                }
                inputStage++;
                inputBuffer[0] = '\0';
                inputIndex = 0;
            }

            if (inputStage > 2) {
                DrawRectangleRec(startButton, LIGHTGRAY);
                DrawText("Start Game", startButton.x + 30, startButton.y + 15, 20, DARKGRAY);
                bool mouseStart = CheckCollisionPointRec(GetMousePosition(), startButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
                bool keyboardStart = IsKeyPressed(KEY_ENTER);

                if (mouseStart || keyboardStart) {
                    GenerateGamerID();
                    inputComplete = true;
                    gameStarted = true;
                    ResetGame(&snake, &food);
                }
            }

            EndDrawing();
            continue;
        }

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

        if (!gameOver && !paused) {
            for (int i = snake.length - 1; i > 0; i--) snake.tail[i] = snake.tail[i - 1];
            if (snake.length > 0) snake.tail[0] = snake.position;

            snake.position.x += snake.direction.x;
            snake.position.y += snake.direction.y;

            if (snake.position.x < 0 || snake.position.y < 0 ||
                snake.position.x >= SCREEN_WIDTH / CELL_SIZE ||
                snake.position.y >= SCREEN_HEIGHT / CELL_SIZE) {
                gameOver = true;
                PlaySound(gameOverSound);
            }

            for (int i = 0; i < snake.length; i++) {
                if (snake.tail[i].x == snake.position.x && snake.tail[i].y == snake.position.y) {
                    gameOver = true;
                    PlaySound(gameOverSound);
                    break;
                }
            }

            if (snake.position.x == food.position.x && snake.position.y == food.position.y) {
                if (snake.length < MAX_SNAKE_LENGTH) snake.length++;
                score++;
                PlaySound(eatSound);

                if (score > highScore) {
                    highScore = score;
                    SaveHighScore(highScore, gamerID);
                }

                if (score % SCORE_PER_LEVEL == 0) {
                    level++;
                    SetTargetFPS(10 + level * 2);
                }

                food.position = (Vector2){GetRandomValue(0, SCREEN_WIDTH / CELL_SIZE - 1),
                                          GetRandomValue(0, SCREEN_HEIGHT / CELL_SIZE - 1)};
            }
        }

        ClearBackground((Color){240, 248, 255, 255});
        for (int x = 0; x < SCREEN_WIDTH; x += CELL_SIZE) DrawLine(x, 0, x, SCREEN_HEIGHT, LIGHTGRAY);
        for (int y = 0; y < SCREEN_HEIGHT; y += CELL_SIZE) DrawLine(0, y, SCREEN_WIDTH, y, LIGHTGRAY);

        DrawRectangle(snake.position.x * CELL_SIZE, snake.position.y * CELL_SIZE, CELL_SIZE, CELL_SIZE, DARKGREEN);
        for (int i = 0; i < snake.length; i++) {
            DrawRectangle(snake.tail[i].x * CELL_SIZE, snake.tail[i].y * CELL_SIZE, CELL_SIZE, CELL_SIZE, GREEN);
        }

        DrawRectangle(food.position.x * CELL_SIZE, food.position.y * CELL_SIZE, CELL_SIZE, CELL_SIZE, RED);

        DrawText(TextFormat("Score: %d", score), 10, 10, 20, BLACK);
        DrawText(TextFormat("High Score: %d", highScore), 10, 30, 20, DARKGRAY);
        DrawText(TextFormat("Level: %d", level), SCREEN_WIDTH - 120, 10, 20, MAROON);

        if (paused) DrawText("PAUSED", SCREEN_WIDTH / 2 - 50, SCREEN_HEIGHT / 2 - 20, 40, ORANGE);

        if (gameOver) {
            DrawText("GAME OVER! Press R to Restart", SCREEN_WIDTH / 2 - 180, SCREEN_HEIGHT / 2, 20, RED);
            DrawText(playerName, SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 + 40, 20, BLACK);
            DrawText(TextFormat("Department: %s", department), SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 + 70, 20, BLACK);
            DrawText(TextFormat("Registration No: %s", regNo), SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 + 100, 20, BLACK);
            DrawText(TextFormat("Gamer ID: %s", gamerID), SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 + 130, 20, DARKBLUE);
        }

        EndDrawing();
    }

    UnloadSound(eatSound);
    UnloadSound(gameOverSound);
    CloseAudioDevice();
    CloseWindow();
    return 0;
}