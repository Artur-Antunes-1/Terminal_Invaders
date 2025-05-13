#ifndef GAME_H
#define GAME_H

#include <stddef.h>

// parâmetros
#define WIDTH  80
#define HEIGHT 24

// structs
typedef struct Projectile {
    int x, y;
    struct Projectile *next;
} Projectile;

typedef struct Enemy {
    int x, y;
    struct Enemy *next;
} Enemy;

typedef struct Player {
    int x, y;
} Player;

// lista de entidades
extern Player player;
extern Enemy *enemies;
extern Projectile *shots;
extern int score;

// funções
void init_game(void);
void update_game(void);
void spawn_enemy(void);
void cleanup_entities(void);

#endif // GAME_H
