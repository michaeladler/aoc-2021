struct BT {
};

typedef BT *data; /* type to pass data to backtrack */

#define MAXCANDIDATES 100 /* max possible next extensions */
#define NMAX 100          /* maximum solution size */

static bool finished = false; /* found all solutions yet? */

/* Check if a[0], ... a[k] is a solution */
static bool is_a_solution(int a[], int k, data ctx) {
    return false;  // TODO
}

static void process_solution(int a[], int k, data ctx) {
    // TODO
}

/* What are possible candidates for position k?
 * The candidates are stored in c[] and its length is stored in ncandidates.
 */
static void construct_candidates(int a[], int k, data ctx, int c[], int *ncandidates) {
    assert(k > 0);
    c[k] = 42;  // TODO
    *ncandidates = 1;
}

void make_move(int a[], int k, data input) {
    //
}

void unmake_move(int a[], int k, data input) {
    //
}

/*
Copyright 2003 by Steven S. Skiena; all rights reserved.

Permission is granted for use in non-commerical applications
provided this copyright notice remains intact and unchanged.

This program appears in my book:

"Programming Challenges: The Programming Contest Training Manual"
by Steven Skiena and Miguel Revilla, Springer-Verlag, New York 2003.

See our website www.programming-challenges.com for additional information.

This book can be ordered from Amazon.com at

http://www.amazon.com/exec/obidos/ASIN/0387001638/thealgorithmrepo/

*/
static void backtrack(int a[], int k, data input) {
    int c[MAXCANDIDATES]; /* candidates for next position */
    int ncandidates;      /* next position candidate count */
    int i;                /* counter */

    if (is_a_solution(a, k, input)) {
        process_solution(a, k, input);
    } else {
        k = k + 1;
        construct_candidates(a, k, input, c, &ncandidates);
        for (i = 0; i < ncandidates; i++) {
            a[k] = c[i];
            make_move(a, k, input);
            backtrack(a, k, input);
            unmake_move(a, k, input);
            if (finished) return; /* terminate early */
        }
    }
}
