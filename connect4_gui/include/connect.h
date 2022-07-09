#ifndef CONNECT_H
#define CONNECT_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#define WIN 100
#define LOSE (-WIN)
#define ROWS 6
#define COLS 7
#define CELLSIZE 100
#define B_WIDTH (CELLSIZE*COLS)
#define B_HEIGHT (CELLSIZE*ROWS)
#define BOXSIZE 30
#define BOX1_X (g->screenWidth/2 + CELLSIZE)
#define BOX2_X (g->screenWidth/2 + CELLSIZE+2*BOXSIZE)

typedef enum {TITLE, PLAYERTURN, COMPUTERTURN, ENDING, SETTINGS} GameScreen;

typedef enum{
    EMPTY = 0,
    PLAYER = 1,
    COMP = 2,
}state;

typedef struct{
    int row;
    int col;
}position;

typedef struct{
    GameScreen gamestate;
    state board[ROWS][COLS];
    int compscore;
    int playerscore;
    int screenHeight;
    int screenWidth;
    int max_depth;
}Game_info;

bool inInterval(int x, int min, int max);
int checkRow(const state board[ROWS][COLS], int startRow, int startCol);
int checkCol(const state board[ROWS][COLS], int startRow, int startCol);
int checkDiag(const state board[ROWS][COLS], int startRow, int startCol);
int checkDiagb(const state board[ROWS][COLS], int startRow, int startCol);
int win(const state board[ROWS][COLS]);
bool boardFull(const state board[ROWS][COLS]);
void legalmoves(const state board[ROWS][COLS], position moves[COLS]);
int minmax(state arr[ROWS][COLS], int isMaximizing, int depth, int alpha, int beta, int max_depth);
position findBestmove(state arr[ROWS][COLS], int max_depth);
void computermove(state arr[ROWS][COLS], position move);
int newevaluate(const state arr[ROWS][COLS]);

#endif
