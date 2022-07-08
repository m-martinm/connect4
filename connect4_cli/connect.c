#include "connect.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define ROWS 6
#define COLS 7
#define CLEAR "\e[1;1H\e[2J"

bool inInterval(int x, int min, int max){ // inclusive
    if(x <= max && x >= min){
        return true;
    }
    return false;
}

void render_board(state board[ROWS][COLS]){
    printf(CLEAR);
    printf("\n\t  1   2   3   4   5   6   7  \n");
    printf("\t+---+---+---+---+---+---+---+\n");
    for(int i = 0; i < ROWS; i++){
    printf("\t|");
        for(int j = 0; j < COLS; j++){
            switch(board[i][j]){
                case EMPTY: printf("   |");
                    break;
                case PLAYER: printf(" O |");
                    break;
                case COMP: printf(" X |");
                    break;
            }
        }
        printf("\n\t+---+---+---+---+---+---+---+\n");
    }
    printf("\t  1   2   3   4   5   6   7  \n");
}

void playermove(state board[ROWS][COLS]){
    char BUFFER[5];
    int col;
    bool run = true;

    while(run){
        printf(CLEAR);
        render_board(board);
        printf("\n\tENTER COLUMN: ");
        if(fgets(BUFFER, 5, stdin) != NULL ) {
            int ret = sscanf(BUFFER, "%d", &col);
            if(ret == 1 && inInterval(col, 1, 7) && board[0][col-1] == EMPTY){
                run = false;
            }
        }
    }

    for(int i = ROWS-1; i >= 0; i--){
        if(board[i][col-1] == EMPTY){
            board[i][col-1] = PLAYER;
            return;
        }
    }
}

int checkCol(state board[ROWS][COLS], int startRow, int startCol){
    int cnt = 0;
    int i = startRow;
    while(board[i][startCol] == board[startRow][startCol]){
        cnt++;
        i--;
        if(i < 0) return cnt;
    }
    return cnt;
}

int checkRow(state board[ROWS][COLS], int startRow, int startCol){
    int cnt = 0;
    int i = startCol;
    while(board[startRow][i] == board[startRow][startCol]){
        cnt++;
        i++;
        if(i > COLS-1) return cnt;
    }
    return cnt;
}

int checkDiag(state board[ROWS][COLS], int startRow, int startCol){
    int cnt = 0;
    int row = startRow;
    int col = startCol;
    while(board[row][col] == board[startRow][startCol]){
        cnt++;
        row--;
        col++;
        if(row < 0 || col > COLS - 1) return cnt;
    }
    return cnt;
}

int checkDiagb(state board[ROWS][COLS], int startRow, int startCol){
    int cnt = 0;
    int row = startRow;
    int col = startCol;
    while(board[row][col] == board[startRow][startCol]){
        cnt++;
        row--;
        col--;
        if(row < 0 || col < 0) return cnt;
    }
    return cnt;
}

bool boardFull(state board[ROWS][COLS]){
    for(int i = 0; i < ROWS; i++){
        for(int j = 0; j < COLS; j++){
            if(board[i][j] != EMPTY){
                return false;
            }
        }
    }
    return true;
}

int win(state board[ROWS][COLS]){
    if(boardFull(board)) return -1;
    for(int i = ROWS; i >= 0; i--){
        for(int j = 0; j < COLS; j++){
            if(board[i][j] != EMPTY){
                if(checkCol(board, i, j) == 4 || checkRow(board, i, j) == 4 || checkDiag(board, i, j) == 4 || checkDiagb(board, i,j) == 4){
                    switch(board[i][j]){
                        case PLAYER: return PLAYER;
                            break;
                        case COMP: return COMP;
                            break;
                        case EMPTY: continue;
                            break;
                    }
                }
            }
        }
    }
    return 0;
}

void randommove(state board[ROWS][COLS]){
    int col;
    do{
        col = rand() % 7;
    }while(board[0][col] != EMPTY);

    for(int i = ROWS-1; i >= 0; i--){
        if(board[i][col] == EMPTY){
            board[i][col] = COMP;
            return;
        }
    }
}

int max(int a, int b){
    if(a>b) return a;
    else return b;
}

int min(int a, int b){
    if(a>b) return b;
    else return a;
}

int evaluate(state arr[ROWS][COLS]){
    int tmp = win(arr);
    int scoreP = 0;
    int scoreC = 0;
    if (tmp == 2) return 100;
    else if (tmp == 1) return -100;
    else if (tmp == -1) return 0;
    else{
        for(int i = ROWS-1; i >= 0; i--){
            for(int j = 0; j < COLS ; j++){
                if(arr[i][j] == PLAYER){
                    // scoreP = max(scoreP, exp(checkCol(arr, i, j)));
                    // scoreP = max(scoreP, exp(checkRow(arr, i, j)));
                    // scoreP = max(scoreP, exp(checkDiag(arr, i, j)));
                    // scoreP = max(scoreP, exp(checkDiagb(arr, i, j)));
                    scoreP += checkDiagb(arr, i, j);
                    scoreP += checkCol(arr, i, j);
                    scoreP += checkRow(arr, i, j);
                    scoreP += checkDiag(arr, i, j);
                }
                else if(arr[i][j] == COMP){
                    // scoreC = max(scoreC, exp(checkCol(arr, i, j)));
                    // scoreC = max(scoreC, exp(checkRow(arr, i, j)));
                    // scoreC = max(scoreC, exp(checkDiag(arr, i, j)));
                    // scoreC = max(scoreC, exp(checkDiagb(arr, i, j)));
                    scoreC += checkDiagb(arr, i, j);
                    scoreC += checkCol(arr, i, j);
                    scoreC += checkRow(arr, i, j);
                    scoreC += checkDiag(arr, i, j);
                }
            }
        }
    }
    return scoreC - scoreP;
}

void legalmoves(state board[ROWS][COLS], position moves[COLS]){
    int cnt = 0;
    for(int i = 0; i < COLS; i++){
        for(int j = ROWS-1; j >= 0; j--){
            if(board[j][i] == EMPTY) {
                moves[cnt].row = j;
                moves[cnt].col = i;
                cnt++;
                if(cnt >= COLS) return;
                break;
            }
        }
    }
}

int minmax(state arr[ROWS][COLS], int isMaximizing, int depth, int alpha, int beta){
// működik eval functiont tudod javítani
    int score = evaluate(arr);
    if(score == -100 || score == 100 || depth > 9) return score;
    else if(boardFull(arr)) return 0;

    position moves[COLS];
    memset(moves, -1, sizeof(moves));

    if(isMaximizing){
        int best = -10000;
        legalmoves(arr, moves);
        for(int i = 0; i < COLS; i++){
            if(moves[i].row != -1){
                arr[moves[i].row][moves[i].col] = COMP;
                int eval = minmax(arr, 0, depth + 1, alpha, beta);
                best = max(best, eval);
                arr[moves[i].row][moves[i].col] = EMPTY;
                alpha = max(alpha, eval);
                if(beta <= alpha) break;
            }
            else break;
        }
        return best - depth;
    }
    else{
        int best = 10000;
        legalmoves(arr, moves);
        for(int i = 0; i < COLS; i++){
            if(moves[i].row != -1){
                arr[moves[i].row][moves[i].col] = PLAYER;
                int eval = minmax(arr, 1, depth + 1, alpha, beta);
                best = min(best, eval);
                arr[moves[i].row][moves[i].col] = EMPTY;
                beta = min(beta, eval);
                if(beta <= alpha) break;
            }
            else break;
        }
        return best + depth;
    }
}

position findBestmove(state arr[ROWS][COLS], FILE *stream){
    int bestVal = -10000;
    position bestMove;
    position moves[COLS];
    memset(moves, -1, sizeof(moves));

    legalmoves(arr, moves);
    for(int i = 0; i < COLS; i++){
        if(moves[i].row != -1){
            arr[moves[i].row][moves[i].col] = COMP;
            fprintf(stream, "trying move: row = %d, col=%d\n", moves[i].row+1, moves[i].col+1);
            int moveVal = minmax(arr, 0, 0, -10000000, 10000000);
            if(moveVal > bestVal){
                bestMove.row = moves[i].row;
                bestMove.col = moves[i].col;
                bestVal = moveVal;
            }
            arr[moves[i].row][moves[i].col] = EMPTY;
        } 
    }
    fprintf(stream, "chose move: row = %d, col=%d\n", bestMove.row + 1, bestMove.col + 1);
    return bestMove;
}

void computermove(state arr[ROWS][COLS], position move){
    arr[move.row][move.col] = COMP;
}


