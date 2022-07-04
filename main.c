#include "connect.h"
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#define ROWS 6
#define COLS 7
#define RESIZE "\e[8;19;45t"

int main(){

    srand(time(NULL));
    printf(RESIZE);
    state board[ROWS][COLS] = {0};
    render_board(board);
    FILE *stream = fopen("log.txt", "w");
    int run = 1;
    while(run){
        playermove(board);
        render_board(board);
        if(win(board)){
            run = 0;
            break;
        } 
        computermove(board, findBestmove(board, stream));
        render_board(board);
        if(win(board)){
            run = 0;
            break;
        }
    }
    if(win(board) == PLAYER){
        printf("PLAYER WON!\n");
    }
    else if(win(board) == COMP) printf("COMPUTER WON!\n");
    else if(win(board) == -1) printf("DRAW\n");

    return 0;
}