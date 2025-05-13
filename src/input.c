// input.c
#include <stdlib.h>
#include "input.h"
#include "keyboard.h"
#include "game.h"

void process_input(int *running) {
    if (!keyhit()) return;
    int c = readch();
    switch (c) {
        case 'q': *running = 0; break;
        case 'a': if (player.x > 1) player.x--; break;
        case 'd': if (player.x < WIDTH-2) player.x++; break;
        case ' ': {  // espaço
            Projectile *s = malloc(sizeof *s);
            s->x = player.x;
            s->y = player.y - 1;
            s->next = shots;
            shots = s;
        } break;
    }
}
