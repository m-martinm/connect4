#include "include/raylib.h"
#include "include/connect.h"



void DrawSettings(const Game_info *g);
void SettingsScreen(Game_info *g);

int main(int argc, char **argv){

    // Initialization
    Game_info game = {
        .gamestate = TITLE,
        .compscore = 0,
        .playerscore = 0,
        .screenHeight = 800,
        .screenWidth = 800,
        .max_depth = 7
    };
    memset(game.board, 0, ROWS*COLS*sizeof(game.board[0][0]));

    // Load all the stuff
    InitWindow(game.screenWidth, game.screenHeight, "Connect4");
    SetTargetFPS(60);
    SettingsScreen(&game);

    return 0;
}

void SettingsScreen(Game_info *g){
    Vector2 mouse;
    while (!WindowShouldClose())
    {
        mouse = GetMousePosition();
        if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
            if(inInterval(mouse.y, CELLSIZE, CELLSIZE+BOXSIZE) && inInterval(g->max_depth, 1, 14)){
                if(inInterval(mouse.x, BOX1_X, BOX1_X + BOXSIZE)) g->max_depth--;
                else if(inInterval(mouse.x, BOX2_X, BOX2_X + BOXSIZE)) g->max_depth++;
            }
            printf("depth: %d\n", g->max_depth);
        }
        DrawSettings(g);
    }
    
}

void DrawSettings(const Game_info *g){
    char depth_buffer[128] = "AI seach Depth: ";
    char depth[8];
    itoa(g->max_depth, depth, 10);
    strcat(depth_buffer, depth);
    BeginDrawing();
    ClearBackground(LIGHTGRAY);
        DrawText(depth_buffer, g->screenWidth/2-MeasureText(depth_buffer, 30), CELLSIZE, 30, DARKGRAY);
        Rectangle depth_box[2] = {
            {.width = BOXSIZE, .height = BOXSIZE, .x = g->screenWidth/2 + CELLSIZE, CELLSIZE},
            {.width = BOXSIZE, .height = BOXSIZE, .x = g->screenWidth/2 + CELLSIZE+2*BOXSIZE, CELLSIZE}
        };
        DrawRectangleRec(depth_box[0], MAROON);
        DrawRectangleRec(depth_box[1], DARKGREEN);
    EndDrawing();
}