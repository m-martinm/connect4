#include "include/raylib.h"
#include "include/connect.h"

void titleScreen(Game_info *g);
void renderBoard(const Game_info *g);
void playerTurn(Game_info *g);
void endScreen(Game_info *g);
void computerTurn(Game_info *g);
void animate(int posX, int posY, state player, const Game_info *g);
void drawScore(const Game_info *g);
void drawEnd(const Game_info *g);
void SettingsScreen(Game_info *g);
void DrawSettings(const Game_info *g);

int game_play(Game_info *g);

int main(int argc, char **argv){

    // Initialization
    Game_info game = {
        .gamestate = SETTINGS,
        .compscore = 0,
        .playerscore = 0,
        .screenHeight = 800,
        .screenWidth = 800,
        .max_depth = 7
    };
    memset(game.board, 0, ROWS*COLS*sizeof(game.board[0][0]));

    // Load all the stuff
    Image icon = LoadImage("dep/icon.png");
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(game.screenWidth, game.screenHeight, "Connect4");
    SetWindowMinSize(800, 800);
    SetWindowIcon(icon);
    UnloadImage(icon);

    // Gameplay
    SetTargetFPS(60);

    return game_play(&game);
}

int game_play(Game_info *g){

    while (!WindowShouldClose()){
        g->screenWidth = GetScreenWidth();
        g->screenHeight = GetScreenHeight();
        switch(g->gamestate){

            case SETTINGS: SettingsScreen(g);
                break;
            case TITLE: titleScreen(g);
                break;
            case PLAYERTURN: playerTurn(g);
                break;
            case COMPUTERTURN: computerTurn(g);
                break;
            case ENDING: endScreen(g);
                break;
            default: titleScreen(g);
                break;
        }
    }
    CloseWindow();
    return 0;
}

void titleScreen(Game_info *g){

    while (!WindowShouldClose()){
        if(IsKeyPressed(KEY_Q)) MaximizeWindow();
        g->screenWidth = GetScreenWidth();
        g->screenHeight = GetScreenHeight();
        if(IsKeyPressed(KEY_ENTER)){
            g->gamestate = PLAYERTURN;
            return;
        }

        BeginDrawing();

            ClearBackground(LIGHTGRAY);
            DrawText("PRESS [ENTER] TO START", g->screenWidth/2 - MeasureText("PRESS [ENTER] TO START", 40)/2, g->screenHeight/2, 40, DARKGRAY);
            DrawText("\n\n\nRules:\n - You have to connect 4 dots in any way\n - Your dots always fall to the bottom\n - Just [click] on the column to place your circle there", 
            GetScreenWidth()/2 - MeasureText("PRESS [ENTER] TO START", 40)/2, GetScreenHeight()/2 - 50, 20, DARKGRAY);

        EndDrawing();
    }
    CloseWindow();
}

void endScreen(Game_info *g){

    char text[128];
    if(win(g->board) == 1){
        strcpy(text, "PLAYER WON!");
        g->playerscore ++;
    }
    else if(win(g->board) == 2){
        strcpy(text, "COMPUTER WON!");
        g->compscore++;
    }
    else if(win(g->board) == -1){
        strcpy(text, "DRAW!");;
    }

    while (!WindowShouldClose()){
        if(IsKeyPressed(KEY_Q)) MaximizeWindow();
        g->screenWidth = GetScreenWidth();
        g->screenHeight = GetScreenHeight();
        if(IsKeyPressed(KEY_ENTER)){
            memset(g->board, EMPTY, ROWS*COLS*sizeof(g->board[0][0]));
            g->gamestate = SETTINGS;
            return;
        }

        BeginDrawing();

            ClearBackground(LIGHTGRAY);
            DrawText(text, g->screenWidth/2 - MeasureText(text, 40)/2, (g->screenHeight - B_HEIGHT)/2 - 80, 40, DARKGRAY);
            DrawText("PRESS [ENTER] TO PLAY AGAIN",
            g->screenWidth/2 - MeasureText("PRESS [ENTER] TO PLAY AGAIN", 20)/2,
            (g->screenHeight - B_HEIGHT)/2 - 20, 20, DARKGRAY);
            renderBoard(g);
            drawEnd(g);
            drawScore(g);

        EndDrawing();
    }
    CloseWindow();
}

void playerTurn(Game_info *g){

    int col = 3;

    while (!WindowShouldClose()){
        if(IsKeyPressed(KEY_Q)) MaximizeWindow();
        g->screenWidth = GetScreenWidth();
        g->screenHeight = GetScreenHeight();

        Rectangle r = {.x = (g->screenWidth - B_WIDTH)/2 + col*CELLSIZE, .y = (g->screenHeight - B_HEIGHT)/2, .height = B_HEIGHT, .width = CELLSIZE};
        BeginDrawing();

            ClearBackground(LIGHTGRAY);
            DrawText("YOUR TURN", g->screenWidth/2 - MeasureText("YOUR TURN", 40)/2, (g->screenHeight - B_HEIGHT)/2 - 40, 40, DARKGRAY);
            renderBoard(g);
            DrawRectangleLinesEx(r, 5.0f, DARKPURPLE);
            drawScore(g);

        EndDrawing();

        if((IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D)) && col < COLS-1){
            col++;
        }
        else if ((IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A)) && col > 0){
            col--;
        }
        if(IsKeyPressed(KEY_SPACE)){
            if(g->board[0][col] == EMPTY){
                for(int i = ROWS-1; i >= 0; i--){
                    if(g->board[i][col] == EMPTY){
                        animate((g->screenWidth - B_WIDTH)/2 + col*CELLSIZE, (g->screenHeight - B_HEIGHT)/2 + i*CELLSIZE, PLAYER, g);
                        g->board[i][col] = PLAYER;
                        if(win(g->board)){
                            g->gamestate = ENDING;
                            return;
                        }
                        else{
                            g->gamestate = COMPUTERTURN; 
                            return;
                        }
                    }
                }
            }
        }
    }
    CloseWindow();
}

void computerTurn(Game_info *g){
    if(IsKeyPressed(KEY_Q)) MaximizeWindow();
    g->screenWidth = GetScreenWidth();
    g->screenHeight = GetScreenHeight();

    BeginDrawing();

            ClearBackground(LIGHTGRAY);
            DrawText("WAIT FOR THE COMPUTER", g->screenWidth/2 - MeasureText("WAIT FOR THE COMPUTER", 40)/2, (g->screenHeight - B_HEIGHT)/2 - 40, 40, DARKGRAY);
            renderBoard(g);
            drawScore(g);

    EndDrawing();
    position temp = findBestmove(g->board, g->max_depth);
    animate((g->screenWidth - B_WIDTH)/2 + temp.col*CELLSIZE, (g->screenHeight - B_HEIGHT)/2 + temp.row*CELLSIZE, COMP, g);
    computermove(g->board, temp);
    if(win(g->board)){
        g->gamestate = ENDING;
        return;
    }
    else{
        g->gamestate = PLAYERTURN;
    }
}

void renderBoard(const Game_info *g){
    for(int i = ROWS-1; i >=0; i--){
        for(int j = 0; j < COLS; j++){
            int posX = (g->screenWidth - B_WIDTH)/2 + j*CELLSIZE;
            int posY = (g->screenHeight - B_HEIGHT)/2 + i*CELLSIZE;
            DrawRectangleLines(posX, posY, CELLSIZE, CELLSIZE, BLACK);
            switch(g->board[i][j]){
                case PLAYER: DrawCircle(posX+CELLSIZE/2, posY + CELLSIZE/2, CELLSIZE/2-4, RED); 
                    break;
                case COMP: DrawCircle(posX+CELLSIZE/2, posY+CELLSIZE/2, CELLSIZE/2-4, ORANGE); 
                    break;
                default: continue;
                    break;
            }
        }
    }
}

void animate(int posX, int posY, state player, const Game_info *g){
    Vector2 position= {.x = posX, .y = -100};
    Vector2 speed = { 0.0f, 25.0f };
    Color color;
    if(player == COMP) color = ORANGE;
    else if(player == PLAYER) color = RED;
    while(position.y < posY){
        position.y += speed.y;
        BeginDrawing();

            ClearBackground(LIGHTGRAY);
            renderBoard(g);
            DrawCircle(position.x+CELLSIZE/2, position.y+CELLSIZE/2, CELLSIZE/2-4, color);
            drawScore(g);

        EndDrawing();
    }
}


void drawScore(const Game_info *g){
    char pscore[10];
    char cscore[10];
    itoa(g->playerscore, pscore, 10);
    itoa(g->compscore, cscore, 10);
    DrawText(pscore, 30, 30, 40, BLACK);
    DrawText(cscore, g->screenWidth - 30 - MeasureText(cscore, 40), 30, 40, BLACK);
}


void drawEnd(const Game_info *g){
    for(int i = ROWS-1; i >=0; i--){
        for(int j = 0; j < COLS; j++){
            if(g->board[i][j] != EMPTY){
                if (checkCol(g->board, i, j) == 4){                    
                    Vector2 st = {.x = (g->screenWidth - B_WIDTH)/2 + j*CELLSIZE, .y = (g->screenHeight - B_HEIGHT)/2 + i*CELLSIZE + CELLSIZE/2};
                    Vector2 en = {.x = (g->screenWidth - B_WIDTH)/2 + j*CELLSIZE, .y = (g->screenHeight - B_HEIGHT)/2 + (i-3)*CELLSIZE + CELLSIZE/2};
                    DrawLineEx(st, en, 10.0f, DARKGREEN);
                }
                else if(checkRow(g->board, i, j) == 4){
                    Vector2 st = {.x = (g->screenWidth - B_WIDTH)/2 + j*CELLSIZE + CELLSIZE/2, .y = (g->screenHeight - B_HEIGHT)/2 + i*CELLSIZE + CELLSIZE/2};
                    Vector2 en = {.x = (g->screenWidth - B_WIDTH)/2 + (j+3)*CELLSIZE + CELLSIZE/2, .y = (g->screenHeight - B_HEIGHT)/2 + i*CELLSIZE +CELLSIZE/2};
                    DrawLineEx(st, en, 10.0f, DARKGREEN);
                }
                else if(checkDiag(g->board, i, j) == 4){
                    Vector2 st = {.x = (g->screenWidth - B_WIDTH)/2 + j*CELLSIZE + CELLSIZE/2, .y = (g->screenHeight - B_HEIGHT)/2 + i*CELLSIZE + CELLSIZE/2};
                    Vector2 en = {.x = (g->screenWidth - B_WIDTH)/2 + (j+3)*CELLSIZE + CELLSIZE/2, .y = (g->screenHeight - B_HEIGHT)/2 + (i-3)*CELLSIZE +CELLSIZE/2};
                    DrawLineEx(st, en, 10.0f, DARKGREEN);
                }
                else if(checkDiagb(g->board, i, j) == 4){
                    Vector2 st = {.x = (g->screenWidth - B_WIDTH)/2 + j*CELLSIZE + CELLSIZE/2, .y = (g->screenHeight - B_HEIGHT)/2 + i*CELLSIZE + CELLSIZE/2};
                    Vector2 en = {.x = (g->screenWidth - B_WIDTH)/2 + (j-3)*CELLSIZE + CELLSIZE/2, .y = (g->screenHeight - B_HEIGHT)/2 + (i-3)*CELLSIZE + CELLSIZE/2};
                    DrawLineEx(st, en, 10.0f, DARKGREEN);
                }
            }
        }
    }
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
        }
        if(IsKeyPressed(KEY_SPACE)){
            g->gamestate = TITLE;
            return;
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
