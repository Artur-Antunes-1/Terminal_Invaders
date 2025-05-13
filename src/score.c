#include "score.h"
#include <stdio.h>
#include <stdlib.h>
#include "game.h"

static int best = 0;

void load_highscores(void) {
    FILE *f = fopen("highscores.txt", "r");
    if (!f) return;
    fscanf(f, "%d", &best);
    fclose(f);
}

void save_highscores(void) {
    if (score <= best) return;
    best = score;
    FILE *f = fopen("highscores.txt", "w");
    if (!f) return;
    fprintf(f, "%d\n", best);
    fclose(f);
}
