#include "draw.h"
#include "screen.h"
#include "game.h"
#include <stdio.h>

void draw_frame(void) {
    screenClear();
    // desenha borda já feita pelo screenInit(1)
    // desenha score
    screenGotoxy(2, 1);
    printf("Score: %d", score);

    // desenha player
    screenGotoxy(player.x, player.y);
    printf("^");

    // desenha inimigos
    for (Enemy *e = enemies; e; e = e->next) {
        screenGotoxy(e->x, e->y);
        printf("X");
    }
    // desenha tiros
    for (Projectile *s = shots; s; s = s->next) {
        screenGotoxy(s->x, s->y);
        printf("|");
    }
    screenUpdate();
}
