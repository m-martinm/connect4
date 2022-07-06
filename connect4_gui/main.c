#include "include/raylib.h"
#include "include/connect.h"
#include <stdlib.h>
#include <string.h>

#define SC_WIDTH 800
#define SC_HEIGHT 800

typedef enum GameScreen {TITLE, PLAYERTURN, COMPUTERTURN, ENDING} GameScreen;

void titleScreen(GameScreen *gamestate);
void renderBoard(state board[ROWS][COLS]);
void playerTurn(GameScreen *gamestate, state board[ROWS][COLS]);
void endScreen(GameScreen *gamestate, state board[ROWS][COLS]);
void computerTurn(GameScreen *gamestate, state board[ROWS][COLS]);
void animate(int posX, int posY, state player, state board[ROWS][COLS]);
void drawScore();
void drawEnd(state board[ROWS][COLS]);

int COMPSCORE = 0;
int PLAYERSCORE = 0;

int main(void)
{
    // Initialization
    const int screenWidth = SC_WIDTH;
    const int screenHeight = SC_HEIGHT;
    Image icon = LoadImage("dep/icon.png");
    
    InitWindow(screenWidth, screenHeight, "Connect4");
    SetWindowIcon(icon);
    UnloadImage(icon);

    state board[ROWS][COLS] = {0};
    board[6][7] = COMP;
    GameScreen gamestate = TITLE;

    SetTargetFPS(60);

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        HideCursor();
        switch(gamestate){
            
            case TITLE: titleScreen(&gamestate);
                break;
            case PLAYERTURN: playerTurn(&gamestate, board);
                break;
            case COMPUTERTURN: computerTurn(&gamestate, board);
                break;
            case ENDING: endScreen(&gamestate, board);
                break;
            default: titleScreen(&gamestate);
                break;
        }
    }

    CloseWindow();        // Close window and OpenGL context

    return 0;
}

void titleScreen(GameScreen *gamestate){

    while (!WindowShouldClose()){
        if(IsKeyPressed(KEY_ENTER)){
            *gamestate = PLAYERTURN;
            return;
        }

        BeginDrawing();

            ClearBackground(LIGHTGRAY);
            DrawText("PRESS [ENTER] TO START", SC_WIDTH/2 - MeasureText("PRESS [ENTER] TO START", 40)/2, SC_HEIGHT/2 - 50, 40, DARKGRAY);
            DrawText("\n\n\nRules:\n - You have to connect 4 dots in any way\n - Your dots always fall to the bottom\n - Just [click] on the column to place your circle there", GetScreenWidth()/2 - MeasureText("PRESS [ENTER] TO START", 40)/2, GetScreenHeight()/2 - 50, 20, DARKGRAY);

        EndDrawing();
    }
    CloseWindow();
}

void endScreen(GameScreen *gamestate, state board[ROWS][COLS]){

    char text[128];
    if(win(board) == 1){
        strcpy(text, "PLAYER WON!");
        PLAYERSCORE++;
    }
    else if(win(board) == 2){
        strcpy(text, "COMPUTER WON!");
        COMPSCORE++;
    }
    else if(win(board) == -1){
        strcpy(text, "DRAW!");;
    }

    while (!WindowShouldClose()){
        if(IsKeyPressed(KEY_ENTER)){
            memset(board, EMPTY, ROWS*COLS*(sizeof(board[0][0])));
            *gamestate = TITLE;
            return;
        }

        BeginDrawing();

            ClearBackground(LIGHTGRAY);
            DrawText(text, GetScreenWidth()/2 - MeasureText(text, 40)/2, 60, 40, DARKGRAY);
            DrawText("PRESS [ENTER] TO PLAY AGAIN", GetScreenWidth()/2 - MeasureText("PRESS [ENTER] TO PLAY AGAIN", 20)/2, 30, 20, DARKGRAY);
            renderBoard(board);
            drawEnd(board);
            drawScore();

        EndDrawing();
    }
    CloseWindow();
}

void playerTurn(GameScreen *gamestate, state board[ROWS][COLS]){

    int col = 3;

    while (!WindowShouldClose()){

        Rectangle r = {.x = 50 + col*(SC_WIDTH - 100)/COLS, .y =  (SC_WIDTH-100)/COLS, .height = SC_HEIGHT-200, .width = (SC_WIDTH-100)/COLS};
        BeginDrawing();

            ClearBackground(LIGHTGRAY);
            DrawText("YOUR TURN", GetScreenWidth()/2 - MeasureText("YOUR TURN", 40)/2, 30, 40, DARKGRAY);
            renderBoard(board);
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
            if(board[0][col] == EMPTY){
                for(int i = ROWS-1; i >= 0; i--){
                    if(board[i][col] == EMPTY){
                        animate(50 + col * (SC_WIDTH - 100)/COLS, 100 + i*(SC_HEIGHT- 200)/ROWS, PLAYER, board);
                        board[i][col] = PLAYER;
                        if(win(board)){
                            *gamestate = ENDING;
                            return;
                        }
                        else{
                            *gamestate = COMPUTERTURN; 
                            return;
                        }
                    }
                }
            }
        }
    }
    CloseWindow();
}

void computerTurn(GameScreen *gamestate, state board[ROWS][COLS]){

    BeginDrawing();

            ClearBackground(LIGHTGRAY);
            DrawText("WAIT FOR THE COMPUTER", GetScreenWidth()/2 - MeasureText("WAIT FOR THE COMPUTER", 40)/2, 30, 40, DARKGRAY);
            renderBoard(board);
            drawScore();

    EndDrawing();
    position temp = findBestmove(board);
    animate(50 + temp.col * (SC_WIDTH - 100)/COLS, 100 + temp.row*(SC_HEIGHT- 200)/ROWS, COMP, board);
    computermove(board, temp);
    if(win(board)){
        *gamestate = ENDING;
        return;
    }
    else{
        *gamestate = PLAYERTURN;
    }
}

void renderBoard(state board[ROWS][COLS]){
    for(int i = ROWS-1; i >=0; i--){
        for(int j = 0; j < COLS; j++){
            int posX = 50 + j*(SC_WIDTH - 100)/COLS;
            int posY = 100 + i*(SC_HEIGHT- 200)/ROWS;
            DrawRectangleLines(posX, posY, (SC_WIDTH - 100)/COLS, (SC_HEIGHT-200)/ROWS, BLACK);
            switch(board[i][j]){
                case PLAYER: DrawCircle(posX+50, posY+50, 48, RED); 
                    break;
                case COMP: DrawCircle(posX+50, posY+50, 48, ORANGE); 
                    break;
                default: continue;
                    break;
            }
        }
    }
}

void animate(int posX, int posY, state player, state board[ROWS][COLS]){
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
            DrawCircle(position.x+50, position.y+50, 48, color);
            drawScore();

        EndDrawing();
    }
}

void drawScore(){
    char pscore[10];
    char cscore[10];
    itoa(PLAYERSCORE, pscore, 10);
    itoa(COMPSCORE, cscore, 10);
    DrawCircle(40,47,30,Fade(RED, 0.5f));
    DrawCircle(SC_WIDTH-40,47,30,Fade(YELLOW, 0.5f));
    DrawText(pscore, 30, 30, 40, BLACK);
    DrawText(cscore, SC_WIDTH - 30 - MeasureText(cscore, 40), 30, 40, BLACK);
}

void drawEnd(state board[ROWS][COLS]){
    for(int i = ROWS-1; i >=0; i--){
        for(int j = 0; j < COLS; j++){
            if(board[i][j] != EMPTY){
                if (checkCol(board, i, j) == 4){
                    Vector2 st = {.x = 100 + j*(SC_WIDTH - 100)/COLS, .y = 150 + i*(SC_HEIGHT- 200)/ROWS};
                    Vector2 en = {.x = 100 + j*(SC_WIDTH - 100)/COLS, .y = 150 + (i-3)*(SC_HEIGHT- 200)/ROWS};
                    DrawLineEx(st, en, 10.0f, DARKGREEN);
                }
                else if(checkRow(board, i, j) == 4){
                    Vector2 st = {.x = 100 + j*(SC_WIDTH - 100)/COLS, .y = 150 + i*(SC_HEIGHT- 200)/ROWS};
                    Vector2 en = {.x = 100 + (j+3)*(SC_WIDTH - 100)/COLS, .y = 150 + i*(SC_HEIGHT- 200)/ROWS};
                    DrawLineEx(st, en, 10.0f, DARKGREEN);
                }
                else if(checkDiag(board, i, j) == 4){
                    Vector2 st = {.x = 100 + j*(SC_WIDTH - 100)/COLS, .y = 150 + i*(SC_HEIGHT- 200)/ROWS};
                    Vector2 en = {.x = 100 + (j+3)*(SC_WIDTH - 100)/COLS, .y = 150 + (i-3)*(SC_HEIGHT- 200)/ROWS};
                    DrawLineEx(st, en, 10.0f, DARKGREEN);
                }
                else if(checkDiagb(board, i, j) == 4){
                    Vector2 st = {.x = 100 + j*(SC_WIDTH - 100)/COLS, .y = 150 + i*(SC_HEIGHT- 200)/ROWS};
                    Vector2 en = {.x = 100 + (j-3)*(SC_WIDTH - 100)/COLS, .y = 150 + (i-3)*(SC_HEIGHT- 200)/ROWS};
                    DrawLineEx(st, en, 10.0f, DARKGREEN);
                }
            }
        }
    }
}
