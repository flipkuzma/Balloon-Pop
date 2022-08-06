
/** #include statements... **/
#include <stdio.h>
#include <stdlib.h>
#include "bpgame.h"
#include <time.h>

#include "bpgame.h"

/** TYPEDEFS / STRUCTs HERE ***/

typedef struct bpstate BPState;

struct bpstate {
    char** matrix;
    int score;
    BPState * previous;
};


struct bpgame { // CAPTURES THE STATE OF A "RUN" OF THE GAME

    int rows;
    int cols;
    char **matrix;
    int score; //total score
    int n; //cluster count of a move
    BPState * previous;

};//End Struct

void mtx_destroy(char** matrix, int nrows){
    for (int i = 0; i < nrows; i++){
        free(matrix[i]);
    }
    free(matrix);
}

BPGame *bp_create(int nrows, int ncols) {

    //srand(time(0));
    BPGame *bp = malloc(sizeof *bp);

    if (ncols > MAX_COLS || nrows > MAX_ROWS) {
        // printf("Rows or Cols are larger then 40");
        return 0;
    }
    bp->rows = nrows;
    bp->cols = ncols;
    bp->matrix = (char **) malloc(sizeof(char *) * nrows);
    bp->score = 0;
    bp->n = 0;
    bp->previous = NULL;
    for (int k = 0; k < nrows; k++) {
        bp->matrix[k] = (char *) malloc(sizeof(char) * ncols);
    }

    int i, j;
    for (i = 0; i < nrows; i++) {
        for (j = 0; j < ncols; j++) {
            int randNum = rand() % 4;
            if (randNum == 0) {
                bp->matrix[i][j] = Red;
            } else if (randNum == 1) {
                bp->matrix[i][j] = Yellow;
            } else if (randNum == 2) {
                bp->matrix[i][j] = Blue;
            } else if (randNum == 3) {
                bp->matrix[i][j] = Green;
            }
        }
    }
    return bp;
}//End Function

BPGame *bp_create_from_mtx(char mtx[][MAX_COLS], int nrows, int ncols) {

    struct bpgame *bp = malloc(sizeof *bp);

    if (ncols > MAX_COLS || nrows > MAX_ROWS) {
        //printf("Rows or Cols are larger then 40");
        return 0;
    }
    bp->rows = nrows;
    bp->cols = ncols;
    bp->matrix = (char **) malloc(sizeof(char *) * nrows);
    bp->score = 0;
    bp->n = 0;
    bp->previous = NULL;

    for (int k = 0; k < nrows; k++) {
        bp->matrix[k] = (char *) malloc(sizeof(char) * ncols);
    }
    // printf("test");
    for (int i = 0; i < nrows; i++) {
        for (int j = 0; j < ncols; j++) {
            bp->matrix[i][j] = mtx[i][j];
        }
    }
    return bp;
}//End Function

void bp_display(BPGame *b) {

    int nrows = b->rows;
    int ncols = b->cols;

    //first bracket
    printf("%s", "  +");
    for (int i = 0; i < ((2 * ncols) + 1); i++) { printf("%s", "-"); }
    printf("%s\n", "+");

    //center (includes main matrix)
    for (int i = 0; i < nrows; i++) {
        printf("%i | ", i);
        for (int j = 0; j < ncols; j++) { printf("%c ", b->matrix[i][j]); }
        printf("|\n");
    }

    //second bracket
    printf("%s", "  +");
    for (int i = 0; i < ((2 * ncols) + 1); i++) { printf("%s", "-"); }
    printf("%s\n", "+");

    //numbers beneath matrix
    printf("%s", "    ");
    for (int i = 0; i < ncols; i++) { //first row of nums (first digit)
        int first = i;
        if (first >= 10) {
            while (first >= 10) {
                first = first / 10;
            }
        } else {
            first = 0;
        }
        printf("%i ", first);
    }
    printf("\n%s", "    ");
    for (int i = 0; i < ncols; i++) { //second row of nums (second digit)
        int last = i;
        last = last % 10;
        printf("%i ", last);
    }
    printf("\n");
}//End Function

int helps_pop(BPGame *b, int r, int c, char color) {

    if( r < 0 || c < 0 || r >= b->rows || c >= b->cols){
        return 0;
    }
    int total = 0;
    //neighbors---------------------------------------

    if (r - 1 >= 0 && bp_get_balloon(b, r - 1, c) == color) {
        total++;
        b->matrix[r-1][c] = None;
        total+=helps_pop(b, r - 1, c, color); //call again for recursivity

    }

    if ( r + 1 < b->rows && bp_get_balloon(b, r + 1, c) == color ) {
        total++;
        b->matrix[r+1][c] = None;
        total+= helps_pop(b, r + 1, c, color); //call again for recursivity

    }

    if ( c - 1 >= 0 && bp_get_balloon(b, r, c - 1) == color ) {
        total++;
        b->matrix[r][c - 1] = None;
        total+= helps_pop(b, r, c - 1, color); //call again for recursivity

    }

    if (c + 1 < b->cols && bp_get_balloon(b, r, c + 1) == color ) {
        total++;
        b->matrix[r][c + 1] = None;
        total+= helps_pop(b, r, c + 1, color); //call again for recursivity

    }

    return total;


}//End Function

int bp_pop(BPGame *b, int r, int c) {

    b->n = 0;

    int nrows = b->rows;
    int ncols = b->cols;
    if (r >= nrows || c >= ncols || b->matrix[r][c] == None){
        return 0;
    }


    //Check if air is above balloons and move them up until it cant
    //  while(bp_is_compact(b) == 0){ bp_float_one_step(b); }


    char** mcopy = malloc(b->rows*sizeof(char*));
    for (int i = 0; i < b->rows; i++){
        mcopy[i]= malloc(b->cols*(sizeof(char)));
    }

    for (int row = 0; row < b -> rows; row++){
        for (int col = 0; col < b-> cols; col++){
            mcopy[row][col] = b->matrix[row][col];
        }
    }

    char color = b->matrix[r][c];
    b->matrix[r][c] = None;
    b->n += helps_pop(b, r, c, color);

    char location = b->matrix[r][c];
    if(b->n > 1){
        BPState * state = malloc(sizeof(BPState));
        state-> matrix = mcopy;
        state->previous = b->previous;
        state->score = b->score;
        b->previous = state;
    }else{
        mtx_destroy(mcopy, b->rows);
    }
    if(b->n == 1){
        b->matrix[r][c] = location;
    }

    if ((b->n) > 0) { (b->n)++; } //finalizes cluster count for one move
    b->score+= (b->n * (b->n-1));

    return b->n;
}//End Function

int bp_can_pop(BPGame *b) {

    int nrows = b->rows;
    int ncols = b->cols;

    char current, top, bottom, left, right;

    for (int i = 0; i < nrows; i++) {
        for (int j = 0; j < ncols; j++) {

            //the current char in the matrix
            current = b->matrix[i][j];
            if(current == None){
                continue;
            }
            //initialize checkpoints for edge cases
            int c1 = 0; //top
            int c2 = 0; //bottom
            int c3 = 0; //left
            int c4 = 0; //right

            //reinitialize chars
            top = 'X';
            bottom = 'X';
            left = 'X';
            right = 'X';

            //switch on checkpoints for edge cases
            if (i == 0) { //not spot above
                c1 = 1;
            } else {
                top = b->matrix[i - 1][j];
            }

            if (i == (nrows - 1)) { //not spot below
                c2 = 1;
            } else {
                bottom = b->matrix[i + 1][j];
            }

            if (j == 0) { //not spot to the left
                c3 = 1;
            } else {
                left = b->matrix[i][j - 1];
            }

            if (j == (ncols - 1)) {  //not spot to the right
                c4 = 1;
            } else {
                right = b->matrix[i][j + 1];
            }

            //check for friend on all sides
            if (c1 == 0) {
                if (current == top) { return 1; }
            }
            if (c2 == 0) {
                if (current == bottom) { return 1; }
            }
            if (c3 == 0) {
                if (current == left) { return 1; }
            }
            if (c4 == 0) {
                if (current == right) { return 1; }
            }

        }
    }

    return 0;

}//End Function

int bp_get_balloon(BPGame *b, int r, int c) {

    return b->matrix[r][c];

}//End Function

int bp_is_compact(BPGame *b) {

    int nrows = b->rows;
    int ncols = b->cols;

    for (int i = 1; i < nrows; i++) {
        for (int j = 0; j < ncols; j++) {
            if ((b->matrix[i][j] != '.')) { //if the row does not contain air
                if (b->matrix[i - 1][j] == '.') { //there should be no air above it
                    return 0;
                }
            }
        }
    }

    return 1;

}//End Function

void bp_float_one_step(BPGame *b) {

    int nrows = b->rows;
    int ncols = b->cols;

    for (int i = 1; i < nrows; ++i) {
        for (int j = 0; j < ncols; ++j) {
            if ((b->matrix[i][j] != '.')) { //if the row does not contain air
                if (b->matrix[i - 1][j] == '.') { //and there is air above it
                    b->matrix[i - 1][j] = b->matrix[i][j];
                    b->matrix[i][j] = None;
                }
            }
        }
    }

}//End Function

int bp_score(BPGame *b) {

    return b->score;
}//End Function

int bp_undo(BPGame *b) {
    if (b->previous == NULL){
        return 0;
    }

    BPState * previous = b->previous;
    b->previous = b->previous->previous;
    for (int row = 0; row < b->rows; row++){
        for (int col = 0; col < b->cols; col++){
            b->matrix[row][col] = previous -> matrix[row][col];
        }
    }
    b->score = previous->score;

    mtx_destroy(previous->matrix, b->rows);
    free(previous);

    return 1;
}//End Function


void bp_destroy(BPGame * b){
    /*mtx_destroy(matrix, b->rows);
     free(b);*/
}


//End Function
//
void bp_display_STD(BPGame *b) {
    int nrows = b->rows;
    int ncols = b->cols;

    for (int i = 0; i < nrows; i++) {
        for (int j = 0; j < ncols; j++) {
            printf("%c", b->matrix[i][j]);
        }
        printf("\n");
    }
}
/*** IMPLEMENTATION OF bp_XXXX FUNCTIONS HERE  ****/