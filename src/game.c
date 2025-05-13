#include <stdlib.h>
#include "game.h"

// variáveis globais
Player player;
Enemy *enemies = NULL;
Projectile *shots = NULL;
int score = 0;

static int frameCount = 0;

void init_game(void) {
    player.x = WIDTH/2;
    player.y = HEIGHT - 2;
    score = 0;
}

void spawn_enemy(void) {
    if (frameCount % 20 != 0) return;
    Enemy *e = malloc(sizeof *e);
    e->x = rand() % (WIDTH-2) + 1;
    e->y = 1;
    e->next = enemies;
    enemies = e;
}

void update_game(void) {
    frameCount++;
    // 1) mover inimigos
    for (Enemy *e = enemies; e; e = e->next) {
        e->y++;
    }
    // 2) mover tiros
    Projectile **pp = &shots;
    while (*pp) {
        Projectile *s = *pp;
        s->y--;
        if (s->y < 1) {
            *pp = s->next;
            free(s);
        } else pp = &s->next;
    }
    // 3) colisão tiro–inimigo
    for (Projectile **ps = &shots; *ps; ) {
        Projectile *s = *ps;
        Enemy **pe = &enemies;
        int hit = 0;
        while (*pe) {
            Enemy *e = *pe;
            if (e->x == s->x && e->y == s->y) {
                // remove inimigo
                *pe = e->next;
                free(e);
                hit = 1;
                score += 10;
                break;
            }
            pe = &e->next;
        }
        if (hit) {
            // remove tiro
            *ps = s->next;
            free(s);
        } else {
            ps = &s->next;
        }
    }
    // 4) game over?
    for (Enemy *e = enemies; e; e = e->next) {
        if (e->y >= player.y) {
            // força saída
            cleanup_entities();
            // score atualizado, game over sinalizado via score<0
            score = -1;
            return;
        }
    }
    // 5) spawn
    spawn_enemy();
}

void cleanup_entities(void) {
    while (enemies) {
        Enemy *t = enemies;
        enemies = t->next;
        free(t);
    }
    while (shots) {
        Projectile *t = shots;
        shots = t->next;
        free(t);
    }
}
