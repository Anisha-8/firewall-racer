/* Firewall Racer - graphical version using raylib
   Controls: A / Left arrow = move left, D / Right arrow = move right
   ENTER = start, R = retry, Q = back to menu, ESC = exit */

#include "raylib.h"
#include <stdlib.h>
#include <time.h>

#define SCREEN_W 480
#define SCREEN_H 720
#define ROAD_X 60                 /* left edge of the road */
#define LANE_W 120                /* width of one lane */
#define LANES 3
#define MAX_OBS 6

#define CAR_W 56
#define CAR_H 100
#define CAR_Y (SCREEN_H - CAR_H - 30)
#define OBS_W 80
#define OBS_H 80

#define START_SPEED 4.0f          /* pixels per frame */
#define MAX_SPEED 9.0f

#define MENU 0
#define PLAYING 1
#define OVER 2

   /* x position that centres something of width w inside a lane */
#define LANE_X(lane, w) (ROAD_X + (lane) * LANE_W + (LANE_W - (w)) / 2)

/* one falling item */
struct Obstacle {
    float y;
    int lane;
    int type;          /* 0 virus, 1 malware, 2 phishing, 3 patch */
    int active;        /* 1 = on screen, 0 = unused */
};

struct Obstacle obstacles[MAX_OBS];
const char* names[4] = { "VIR", "MAL", "PHI", "PAT" };

int state = MENU;
int playerLane;
int score;
int highScore = 0;
float speed;           /* how fast things fall */
int shield;            /* 1 = shield on */
int gameOver;
float carX;            /* car x position (slides to the lane) */
float lineOffset;      /* moves the white road lines */

/* write text in the middle of the window */
void drawCentered(const char* text, int y, int size, Color color)
{
    DrawText(text, (SCREEN_W - MeasureText(text, size)) / 2, y, size, color);
}

/* reset everything for a new game */
void initGame()
{
    int i;
    playerLane = 1;                       /* middle lane */
    carX = (float)LANE_X(playerLane, CAR_W);
    score = 0;
    speed = START_SPEED;
    shield = 0;
    gameOver = 0;
    lineOffset = 0;
    for (i = 0; i < MAX_OBS; i++)
        obstacles[i].active = 0;
}

/* draw a car made from simple shapes (x, y = top-left corner) */
void drawCar(int x, int y)
{
    /* wheels */
    DrawRectangle(x - 6, y + 12, 10, 24, BLACK);
    DrawRectangle(x + CAR_W - 4, y + 12, 10, 24, BLACK);
    DrawRectangle(x - 6, y + CAR_H - 36, 10, 24, BLACK);
    DrawRectangle(x + CAR_W - 4, y + CAR_H - 36, 10, 24, BLACK);
    /* body */
    DrawRectangleRounded((Rectangle) { (float)x, (float)y, CAR_W, CAR_H }, 0.4f, 8, BLUE);
    /* windows */
    DrawRectangle(x + 8, y + 26, CAR_W - 16, 16, SKYBLUE);
    DrawRectangle(x + 8, y + 62, CAR_W - 16, 12, SKYBLUE);
    /* roof */
    DrawRectangle(x + 8, y + 42, CAR_W - 16, 20, DARKBLUE);
    /* headlights */
    DrawRectangle(x + 6, y + 4, 12, 6, YELLOW);
    DrawRectangle(x + CAR_W - 18, y + 4, 12, 6, YELLOW);
}

/* draw the road, items, car and score */
void drawScreen()
{
    int i, l, x, y, size;
    Color c;

    ClearBackground(DARKGREEN);

    /* road and its edges */
    DrawRectangle(ROAD_X, 0, LANE_W * LANES, SCREEN_H, DARKGRAY);
    DrawRectangle(ROAD_X - 4, 0, 4, SCREEN_H, WHITE);
    DrawRectangle(ROAD_X + LANE_W * LANES, 0, 4, SCREEN_H, WHITE);

    /* dashed lane lines that move down */
    for (l = 1; l < LANES; l++)
        for (y = -80 + (int)lineOffset; y < SCREEN_H; y += 80)
            DrawRectangle(ROAD_X + l * LANE_W - 3, y, 6, 40, WHITE);

    /* falling items */
    for (i = 0; i < MAX_OBS; i++) {
        if (!obstacles[i].active)
            continue;
        x = LANE_X(obstacles[i].lane, OBS_W);
        y = (int)obstacles[i].y;

        if (obstacles[i].type == 3) {                 /* patch: green circle */
            DrawCircle(x + OBS_W / 2, y + OBS_H / 2, 38, GREEN);
            DrawCircleLines(x + OBS_W / 2, y + OBS_H / 2, 38, DARKGREEN);
        }
        else {                                      /* threat: coloured box */
            if (obstacles[i].type == 0) c = RED;
            else if (obstacles[i].type == 1) c = PURPLE;
            else c = ORANGE;
            DrawRectangleRounded((Rectangle) { (float)x, (float)y, OBS_W, OBS_H }, 0.3f, 8, c);
        }
        size = 24;
        DrawText(names[obstacles[i].type],
            x + (OBS_W - MeasureText(names[obstacles[i].type], size)) / 2,
            y + 28, size, WHITE);
    }

    /* the car, with a blue ring when the shield is on */
    drawCar((int)carX, CAR_Y);
    if (shield)
        DrawRing((Vector2) { carX + CAR_W / 2, CAR_Y + CAR_H / 2 }, 62, 68, 0, 360, 36, Fade(SKYBLUE, 0.7f));

    /* score bar */
    DrawRectangle(0, 0, SCREEN_W, 40, Fade(BLACK, 0.6f));
    DrawText(TextFormat("Score: %d", score), 10, 10, 22, WHITE);
    DrawText(TextFormat("Speed: %.1f", speed), 150, 10, 22, WHITE);
    DrawText(shield ? "SHIELD: ON" : "SHIELD: OFF", 300, 10, 20, shield ? SKYBLUE : LIGHTGRAY);
}

/* A / D or the arrow keys change lane (one move per key press) */
void handleInput()
{
    if ((IsKeyPressed(KEY_A) || IsKeyPressed(KEY_LEFT)) && playerLane > 0)
        playerLane--;
    if ((IsKeyPressed(KEY_D) || IsKeyPressed(KEY_RIGHT)) && playerLane < LANES - 1)
        playerLane++;
}

/* slide the car, move the items down, add new ones */
void updateObstacles()
{
    int i, t, clear;
    float targetX = (float)LANE_X(playerLane, CAR_W);

    carX = carX + (targetX - carX) * 0.25f;     /* slide towards the lane */

    lineOffset = lineOffset + speed;
    if (lineOffset >= 80)
        lineOffset = lineOffset - 80;

    for (i = 0; i < MAX_OBS; i++) {
        if (obstacles[i].active) {
            obstacles[i].y = obstacles[i].y + speed;
            if (obstacles[i].y > SCREEN_H) {            /* passed the bottom */
                obstacles[i].active = 0;
                if (obstacles[i].type != 3) {           /* dodged a threat */
                    score++;
                    if (score % 5 == 0 && speed < MAX_SPEED)
                        speed = speed + 0.4f;           /* speed up */
                }
            }
        }
    }

    /* maybe create a new item, only if the top of the road is clear */
    clear = 1;
    for (i = 0; i < MAX_OBS; i++)
        if (obstacles[i].active && obstacles[i].y < 260)
            clear = 0;

    if (clear && GetRandomValue(0, 30) == 0) {
        for (i = 0; i < MAX_OBS; i++) {
            if (!obstacles[i].active) {
                t = GetRandomValue(0, 5);
                obstacles[i].y = -OBS_H;
                obstacles[i].lane = GetRandomValue(0, LANES - 1);
                obstacles[i].type = (t == 5) ? 3 : t % 3;    /* patch is rare */
                obstacles[i].active = 1;
                break;
            }
        }
    }
}

/* check if the car touched something */
void checkCollision()
{
    int i;
    Rectangle car, item;

    car.x = carX + 4;
    car.y = CAR_Y + 4;
    car.width = CAR_W - 8;
    car.height = CAR_H - 8;

    for (i = 0; i < MAX_OBS; i++) {
        if (!obstacles[i].active)
            continue;
        item.x = (float)LANE_X(obstacles[i].lane, OBS_W) + 6;
        item.y = obstacles[i].y + 6;
        item.width = OBS_W - 12;
        item.height = OBS_H - 12;

        if (CheckCollisionRecs(car, item)) {
            obstacles[i].active = 0;
            if (obstacles[i].type == 3)
                shield = 1;               /* collected a patch */
            else if (shield)
                shield = 0;               /* shield blocks one hit */
            else {
                gameOver = 1;             /* hit a threat */
                if (score > highScore)
                    highScore = score;
            }
        }
    }
}

/* draw the game over screen on top of the road */
void showGameOver()
{
    DrawRectangle(0, 0, SCREEN_W, SCREEN_H, Fade(BLACK, 0.7f));
    drawCentered("GAME OVER", 220, 50, RED);
    drawCentered("Hit by a cyber threat!", 285, 24, WHITE);
    drawCentered(TextFormat("Score: %d", score), 340, 30, WHITE);
    drawCentered(TextFormat("High score: %d", highScore), 380, 30, YELLOW);
    drawCentered("R = retry     Q = menu", 460, 24, LIGHTGRAY);
}

/* draw the start menu */
void drawMenu()
{
    ClearBackground(DARKGRAY);
    drawCentered("FIREWALL RACER", 90, 44, SKYBLUE);
    drawCentered("Dodge the cyber threats!", 150, 24, WHITE);
    drawCar((SCREEN_W - CAR_W) / 2, 220);
    drawCentered("A / D or arrow keys = change lane", 380, 22, WHITE);
    drawCentered("Avoid VIR, MAL and PHI", 420, 22, RED);
    drawCentered("PAT gives a shield (blocks one hit)", 460, 22, GREEN);
    drawCentered("Press ENTER to start", 560, 28, YELLOW);
    drawCentered("ESC = exit", 610, 20, LIGHTGRAY);
}

int main(void)
{
    InitWindow(SCREEN_W, SCREEN_H, "Firewall Racer");
    SetTargetFPS(60);                          /* 60 frames per second */
    SetRandomSeed((unsigned int)time(NULL));

    while (!WindowShouldClose()) {             /* ESC closes the window */

        /* 1. update */
        if (state == MENU) {
            if (IsKeyPressed(KEY_ENTER)) {
                initGame();
                state = PLAYING;
            }
        }
        else if (state == PLAYING) {
            handleInput();
            updateObstacles();
            checkCollision();
            if (gameOver)
                state = OVER;
        }
        else {                               /* OVER */
            if (IsKeyPressed(KEY_R)) {
                initGame();
                state = PLAYING;
            }
            if (IsKeyPressed(KEY_Q))
                state = MENU;
        }

        /* 2. draw */
        BeginDrawing();
        if (state == MENU)
            drawMenu();
        else {
            drawScreen();
            if (state == OVER)
                showGameOver();
        }
        EndDrawing();
    }

    CloseWindow();
    return 0;
}