#include "include/connect.h"

#define ROWS 6
#define COLS 7

bool inInterval(int x, int min, int max){
    return x <= max && x >= min;
}

int checkCol(const state board[ROWS][COLS], int startRow, int startCol){
    int cnt = 0;
    int i = startRow;
    if (i-3 < 0) return 0;
    while(board[i][startCol] == board[startRow][startCol]){
        cnt++;
        i--;
    }
    return cnt;
}

int checkRow(const state board[ROWS][COLS], int startRow, int startCol){
    int cnt = 0;
    int i = startCol;
    if(i > COLS-1-3) return 0;
    while(board[startRow][i] == board[startRow][startCol]){
        cnt++;
        i++;
    }
    return cnt;
}

int checkDiag(const state board[ROWS][COLS], int startRow, int startCol){
    int cnt = 0;
    int row = startRow;
    int col = startCol;
    if(row - 3 < 0 || col + 3 > COLS-1) return 0;
    while(board[row][col] == board[startRow][startCol]){
        cnt++;
        row--;
        col++;
    }
    return cnt;
}

int checkDiagb(const state board[ROWS][COLS], int startRow, int startCol){
    int cnt = 0;
    int row = startRow;
    int col = startCol;
    if(row-3 < 0 || col -3 < 0) return 0;
    while(board[row][col] == board[startRow][startCol]){
        cnt++;
        row--;
        col--;
    }
    return cnt;
}

bool boardFull(const state board[ROWS][COLS]){
    for(int i = 0; i < ROWS; i++){
        for(int j = 0; j < COLS; j++){
            if(board[i][j] == EMPTY){
                return false;
            }
        }
    }
    return true;
}

int win(const state board[ROWS][COLS]){
    for(int i = ROWS-1; i >= 0; i--){
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
    if(boardFull(board)) return -1;
    return 0;
}

int max(int a, int b){
    if(a>b) return a;
    else return b;
}

int min(int a, int b){
    if(a>b) return b;
    else return a;
}

int newevaluate(const state arr[ROWS][COLS]){
    int won = win(arr);
    if(won == 2) return WIN;
    else if(won == 1) return LOSE;
    else if(won == -1) return 0;

    int blockP = 0, threeP = 0, twoP = 0, blockP2 = 0; 
    int blockC = 0, threeC = 0, twoC = 0, blockC2 = 0;

    for(int i = ROWS-1; i >= 0; i--){
        for(int j = 0; j < COLS ; j++){
            if(arr[i][j] == COMP){
                if(checkDiag(arr, i, j) == 3){
                    threeC++;
                    if(arr[i-3][j+3] == PLAYER) blockP++;
                }
                else if(checkDiag(arr, i, j) == 2){
                    twoC++;
                    if(arr[i-2][i+2] == PLAYER) blockP2++;
                }
                if(checkDiagb(arr, i, j) == 3){
                    threeC++;
                    if(arr[i-3][j-3] == PLAYER) blockP++;
                }
                else if(checkDiagb(arr, i, j) == 2){
                    twoC++;
                    if(arr[i-2][j-2] == PLAYER) blockP2++;
                }
                if(checkRow(arr, i, j) == 3){
                    threeC++;
                    if(arr[i][j+3] == PLAYER) blockP++;
                }
                else if(checkRow(arr, i, j) == 2){
                    twoC++;
                    if(arr[i][j+2] == PLAYER) blockP2++;
                } 
                if(checkCol(arr, i, j) == 3){
                    threeC++;
                    if(arr[i-3][j] == PLAYER) blockP++;
                }
                else if(checkCol(arr, i, j) == 2){
                    twoC++;
                    if(arr[i-2][j] == PLAYER) blockP2++;
                }
            }
            else if(arr[i][j] == PLAYER){
                if(checkDiag(arr, i, j) == 3){
                    threeP++;
                    if(arr[i-3][j+3] == COMP) blockC++;
                }
                else if(checkDiag(arr, i, j) == 2){
                    twoP++;
                    if(arr[i-2][j+2] == COMP) blockC2++;
                }
                if(checkDiagb(arr, i, j) == 3){
                    threeP++;
                    if(arr[i-3][j-3] == COMP) blockC++;
                }
                else if(checkDiagb(arr, i, j) == 2){
                    twoP++;
                    if(arr[i-2][j-2] == COMP) blockC2++;
                }
                if(checkRow(arr, i, j) == 3){
                    threeP++;
                    if(arr[i][j+3] == COMP) blockC++;
                }
                else if(checkRow(arr, i, j) == 2){
                    twoP++;
                    if(arr[i][j+2] == COMP) blockC2++;
                }
                if(checkCol(arr, i, j) == 3){
                    threeP++;
                    if(arr[i-3][j] == COMP) blockC++;
                }
                else if(checkCol(arr, i, j) == 2){
                    twoP++;
                    if(arr[i-2][j] == COMP) blockC2++;
                } 
            }
        }
    }
    int ret = (2*blockC2 + blockC + 2*threeC + twoC) - (2*blockP2 + blockP + 2*threeP + twoP);
    return ret;
}

void legalmoves(const state board[ROWS][COLS], position moves[COLS]){
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

int minmax(state arr[ROWS][COLS], int isMaximizing, int depth, int alpha, int beta, int max_depth){

    int score = newevaluate(arr);
    if(score == LOSE || score == WIN || depth > max_depth) return score;
    else if(boardFull(arr)) return 0;

    position moves[COLS];  
    memset(moves, -1, sizeof(moves));

    if(isMaximizing){
        int best = INT_MIN;
        legalmoves(arr, moves);
        for(int i = 0; i < COLS; i++){
            if(moves[i].row != -1){
                arr[moves[i].row][moves[i].col] = COMP;
                int eval = minmax(arr, 0, depth + 1, alpha, beta, max_depth);
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
        int best = INT_MAX;
        legalmoves(arr, moves);
        for(int i = 0; i < COLS; i++){
            if(moves[i].row != -1){
                arr[moves[i].row][moves[i].col] = PLAYER;
                int eval = minmax(arr, 1, depth + 1, alpha, beta, max_depth);
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

position findBestmove(state arr[ROWS][COLS], int max_depth){
    int bestVal = INT_MIN;
    position bestMove;
    position moves[COLS];
    memset(moves, -1, sizeof(moves));

    legalmoves(arr, moves);
    for(int i = 0; i < COLS; i++){
        if(moves[i].row != -1){
            arr[moves[i].row][moves[i].col] = COMP;
            int moveVal = minmax(arr, 0, 0, INT_MIN, INT_MAX, max_depth);
            if(moveVal > bestVal){
                bestMove.row = moves[i].row;
                bestMove.col = moves[i].col;
                bestVal = moveVal;
            }
            arr[moves[i].row][moves[i].col] = EMPTY;
        } 
    }
    return bestMove;
}

void computermove(state arr[ROWS][COLS], position move){
    arr[move.row][move.col] = COMP;
}


