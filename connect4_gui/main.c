#include "include/raylib.h"
#include "include/connect.h"
#include <stdlib.h>
#include <string.h>

#define CELLSIZE 100
#define B_WIDTH (CELLSIZE*COLS)
#define B_HEIGHT (CELLSIZE*ROWS)

typedef enum GameScreen {TITLE, PLAYERTURN, COMPUTERTURN, ENDING} GameScreen;

void titleScreen(Game_info *g);
void renderBoard(const Game_info *g);
void playerTurn(Game_info *g);
void endScreen(Game_info *g);
void computerTurn(Game_info *g);
void animate(int posX, int posY, state player, const state board[ROWS][COLS]);
void drawScore(const Game_info *g);
void drawEnd(const Game_info *g);

int main(void)
{
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

    Image icon = LoadImage("dep/icon.png");
    
    InitWindow(game.screenWidth, game.screenHeight, "Connect4");
    SetWindowIcon(icon);
    UnloadImage(icon);

    SetTargetFPS(60);

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        HideCursor();
        switch(game.gamestate){
            
            case TITLE: titleScreen(&game);
                break;
            case PLAYERTURN: playerTurn(&game);
                break;
            case COMPUTERTURN: titleScreen(&game);
                break;
            case ENDING: endScreen(&game);
                break;
            default: titleScreen(&game);
                break;
        }
    }

    CloseWindow();        // Close window and OpenGL context

    return 0;
}

void titleScreen(Game_info *g){

    while (!WindowShouldClose()){
        if(IsKeyPressed(KEY_ENTER)){
            g->gamestate = PLAYERTURN;
            return;
        }

        BeginDrawing();

            ClearBackground(LIGHTGRAY);
            DrawText("PRESS [ENTER] TO START", g->screenWidth/2 - MeasureText("PRESS [ENTER] TO START", 40)/2, g->screenHeight/2, 40, DARKGRAY);
            DrawText("\n\n\nRules:\n - You have to connect 4 dots in any way\n - Your dots always fall to the bottom\n - Just [click] on the column to place your circle there", GetScreenWidth()/2 - MeasureText("PRESS [ENTER] TO START", 40)/2, GetScreenHeight()/2 - 50, 20, DARKGRAY);

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
        if(IsKeyPressed(KEY_ENTER)){
            memset(g->board, EMPTY, ROWS*COLS*sizeof(g->board[0][0]));
            g->gamestate = TITLE;
            return;
        }

        BeginDrawing();

            ClearBackground(LIGHTGRAY);
            DrawText(text, g->screenWidth - MeasureText(text, 40)/2, (g->screenHeight - B_HEIGHT)/2 - 40, 40, DARKGRAY);
            DrawText("PRESS [ENTER] TO PLAY AGAIN", g->screenWidth - MeasureText("PRESS [ENTER] TO PLAY AGAIN", 20)/2, (g->screenHeight - B_HEIGHT)/2 - 20, 20, DARKGRAY);
            renderBoard(g);
            drawEnd(g);
            drawScore();

        EndDrawing();
    }
    CloseWindow();
}

void playerTurn(Game_info *g){

    int col = 3;

    while (!WindowShouldClose()){

        Rectangle r = {.x = (g->screenWidth - B_WIDTH)/2 + col*CELLSIZE, .y = (g->screenHeight - B_HEIGHT)/2, .height = B_HEIGHT, .width = CELLSIZE};
        BeginDrawing();

            ClearBackground(LIGHTGRAY);
            DrawText("YOUR TURN", g->screenWidth/2 - MeasureText("YOUR TURN", 40)/2, (g->screenHeight - B_HEIGHT)/2 - 40, 40, DARKGRAY);
            renderBoard(g);
            DrawRectangleLinesEx(r, 5.0f, DARKPURPLE);
            drawScore();

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
                        animate((g->screenWidth - B_WIDTH)/2 + col*CELLSIZE, (g->screenHeight - B_HEIGHT)/2 + i*CELLSIZE, PLAYER, g->board);
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

    BeginDrawing();

            ClearBackground(LIGHTGRAY);
            DrawText("WAIT FOR THE COMPUTER", g->screenWidth - MeasureText("WAIT FOR THE COMPUTER", 40)/2, (g->screenHeight - B_HEIGHT)/2 - 40, 40, DARKGRAY);
            renderBoard(g);
            drawScore();

    EndDrawing();
    position temp = findBestmove(g->board);
    animate((g->screenWidth - B_WIDTH)/2 + temp.col*CELLSIZE, (g->screenHeight - B_HEIGHT)/2 + temp.row*CELLSIZE, COMP, g->board);
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
                case PLAYER: DrawCircle(posX+CELLSIZE/2, posY + CELLSIZE/2, CELLSIZE-4, RED); 
                    break;
                case COMP: DrawCircle(posX+CELLSIZE/2, posY+CELLSIZE/2, CELLSIZE-4, ORANGE); 
                    break;
                default: continue;
                    break;
            }
        }
    }
}

void animate(int posX, int posY, state player, const state board[ROWS][COLS]){
    Vector2 position= {.x = posX, .y = -100};
    Vector2 speed = { 0.0f, 25.0f };
    Color color;
    if(player == COMP) color = ORANGE;
    else if(player == PLAYER) color = RED;
    while(position.y < posY){
        position.y += speed.y;
        BeginDrawing();

            ClearBackground(LIGHTGRAY);
            renderBoard(board);
            DrawCircle(position.x+CELLSIZE/2, position.y+CELLSIZE/2, CELLSIZE-4, color);
            drawScore();

        EndDrawing();
    }
}


// this must be rewritten too
void drawScore(const Game_info *g){
    char pscore[10];
    char cscore[10];
    itoa(g->playerscore, pscore, 10);
    itoa(g->compscore, cscore, 10);
    // DrawCircle(40,47,30,Fade(RED, 0.5f));
    // DrawCircle(SC_WIDTH-40,47,30,Fade(YELLOW, 0.5f));
    DrawText(pscore, 30, 30, 40, BLACK); // itt ezeket javítsd
    DrawText(cscore, g->screenWidth - 30 - MeasureText(cscore, 40), 30, 40, BLACK);
}


// continue here
void drawEnd(const Game_info *g){
    for(int i = ROWS-1; i >=0; i--){
        for(int j = 0; j < COLS; j++){
            if(g->board[i][j] != EMPTY){
                // int posX = (g->screenWidth - B_WIDTH)/2 + j*CELLSIZE + CELLSIZE/2;
                // int posY = (g->screenWidth - B_WIDTH)/2 + (i-3)*CELLSIZE + CELLSIZE/2
                if (checkCol(g->board, i, j) == 4){                    
                    Vector2 st = {.x = (g->screenWidth - B_WIDTH)/2 + j*CELLSIZE + CELLSIZE/2, .y = (g->screenWidth - B_WIDTH)/2 + i*CELLSIZE + CELLSIZE/2;
                    Vector2 en = {.x = (g->screenWidth - B_WIDTH)/2 + j*CELLSIZE + CELLSIZE/2, .y = (g->screenWidth - B_WIDTH)/2 + (i-3)*CELLSIZE + CELLSIZE/2};
                    DrawLineEx(st, en, 10.0f, DARKGREEN);
                }
                else if(checkRow(g->board, i, j) == 4){
                    Vector2 st = {.x = 100 + j*(SC_WIDTH - 100)/COLS, .y = 150 + i*(SC_HEIGHT- 200)/ROWS};
                    Vector2 en = {.x = 100 + (j+3)*(SC_WIDTH - 100)/COLS, .y = 150 + i*(SC_HEIGHT- 200)/ROWS};
                    DrawLineEx(st, en, 10.0f, DARKGREEN);
                }
                else if(checkDiag(g->board, i, j) == 4){
                    Vector2 st = {.x = 100 + j*(SC_WIDTH - 100)/COLS, .y = 150 + i*(SC_HEIGHT- 200)/ROWS};
                    Vector2 en = {.x = 100 + (j+3)*(SC_WIDTH - 100)/COLS, .y = 150 + (i-3)*(SC_HEIGHT- 200)/ROWS};
                    DrawLineEx(st, en, 10.0f, DARKGREEN);
                }
                else if(checkDiagb(g->board, i, j) == 4){
                    Vector2 st = {.x = 100 + j*(SC_WIDTH - 100)/COLS, .y = 150 + i*(SC_HEIGHT- 200)/ROWS};
                    Vector2 en = {.x = 100 + (j-3)*(SC_WIDTH - 100)/COLS, .y = 150 + (i-3)*(SC_HEIGHT- 200)/ROWS};
                    DrawLineEx(st, en, 10.0f, DARKGREEN);
                }
            }
        }
    }
}
