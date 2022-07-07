#include "include/raylib.h"
#include "include/connect.h"
#include <stdlib.h>
#include <string.h>

typedef enum GameScreen {TITLE, PLAYERTURN, COMPUTERTURN, ENDING} GameScreen;

void titleScreen(Game_info *g);
void renderBoard(state board[ROWS][COLS]);
void playerTurn(Game_info *g);
void endScreen(Game_info *g);
void computerTurn(GameScreen *gamestate, state board[ROWS][COLS]);
void animate(int posX, int posY, state player, state board[ROWS][COLS]);
void drawScore();
void drawEnd(state board[ROWS][COLS]);

int main(void)
{
    // Initialization
    Game_info game = {
        .gamestate = TITLE,
        .compscore = 0,
        .playerscore = 0,
        .screenHeight = 800,
        .screenWidth = 800,
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
            case PLAYERTURN: endScreen(&game);
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
            DrawText(text, g->screenWidth - MeasureText(text, g->screenHeight/20)/2, g->screenHeight/14, g->screenHeight/20, DARKGRAY);
            DrawText("PRESS [ENTER] TO PLAY AGAIN", g->screenWidth - MeasureText("PRESS [ENTER] TO PLAY AGAIN", g->screenHeight/40)/2, g->screenHeight/27, g->screenHeight/40, DARKGRAY);
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
