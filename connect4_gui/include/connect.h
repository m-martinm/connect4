#ifndef CONNECT_H
#define CONNECT_H

#include <stdbool.h>
#define ROWS 6
#define COLS 7

typedef enum{
    EMPTY = 0,
    PLAYER = 1,
    COMP = 2,
}state;

typedef struct{
    int row;
    int col;
}position;

bool inInterval(int x, int min, int max); // inclusive
int checkRow(state board[ROWS][COLS], int startRow, int startCol);
int checkCol(state board[ROWS][COLS], int startRow, int startCol);
int checkDiag(state board[ROWS][COLS], int startRow, int startCol);
int checkDiagb(state board[ROWS][COLS], int startRow, int startCol);
int win(state board[ROWS][COLS]);
void legalmoves(state board[ROWS][COLS], position moves[COLS]);
int minmax(state arr[ROWS][COLS], int isMaximizing, int depth, int alpha, int beta);
position findBestmove(state arr[ROWS][COLS]);
void computermove(state arr[ROWS][COLS], position move);
int newevaluate(state arr[ROWS][COLS]);

#endif
