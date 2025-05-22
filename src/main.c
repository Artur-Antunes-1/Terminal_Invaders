/* main.c: Terminal Invaders com menu, game over, vitória e seleção de dificuldade a 60 FPS */

/* Includes */
#include "keyboard.h"
#include "screen.h"
#include "timer.h"
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>

/* Constantes */
#define MAXX 80
#define MAXY 24
#define ALIEN_ROWS 3
#define ALIEN_COLS 10
#define ALIEN_SPACING_X 6
#define ALIEN_SPACING_Y 2
#define SHIP_CHAR '^'
#define ALIEN_CHAR 'W'
#define BULLET_CHAR '|'

/* Tipos */
typedef struct { int x, y; bool alive; } Alien;
typedef struct Bullet { int x, y; struct Bullet* next; } Bullet;

/* Protótipos */
void init_aliens(Alien aliens[ALIEN_ROWS][ALIEN_COLS]);
bool update_aliens(Alien aliens[ALIEN_ROWS][ALIEN_COLS], int* dx);
void update_bullets(Bullet** head, Alien aliens[ALIEN_ROWS][ALIEN_COLS]);
bool aliens_exist(Alien aliens[ALIEN_ROWS][ALIEN_COLS]);
void draw_game(Alien aliens[ALIEN_ROWS][ALIEN_COLS], Bullet* bullets, int ship_x);
void show_menu(int* alien_interval);
void show_game_over(void);
void show_victory(void);

/* Implementações */

void init_aliens(Alien aliens[ALIEN_ROWS][ALIEN_COLS]) {
    for(int r = 0; r < ALIEN_ROWS; r++)
        for(int c = 0; c < ALIEN_COLS; c++) {
            aliens[r][c].alive = true;
            aliens[r][c].x = 2 + c * ALIEN_SPACING_X;
            aliens[r][c].y = 2 + r * ALIEN_SPACING_Y;
        }
}

bool update_aliens(Alien aliens[ALIEN_ROWS][ALIEN_COLS], int* dx) {
    bool drop = false;
    for(int r = 0; r < ALIEN_ROWS && !drop; r++)
        for(int c = 0; c < ALIEN_COLS; c++) {
            Alien* a = &aliens[r][c];
            if(!a->alive) continue;
            int nx = a->x + *dx;
            if(nx < 1 || nx >= MAXX - 1) { drop = true; break; }
        }
    if(drop) *dx = -(*dx);
    for(int r = 0; r < ALIEN_ROWS; r++)
        for(int c = 0; c < ALIEN_COLS; c++) {
            Alien* a = &aliens[r][c];
            if(!a->alive) continue;
            a->x += *dx;
            if(drop) a->y++;
            if(a->y >= MAXY - 2) return false;
        }
    return true;
}

void update_bullets(Bullet** head, Alien aliens[ALIEN_ROWS][ALIEN_COLS]) {
    Bullet** p = head;
    while(*p) {
        Bullet* b = *p;
        b->y--;
        if(b->y < 1) {
            *p = b->next;
            free(b);
            continue;
        }
        bool hit = false;
        for(int r = 0; r < ALIEN_ROWS && !hit; r++)
            for(int c = 0; c < ALIEN_COLS; c++) {
                Alien* a = &aliens[r][c];
                if(a->alive && a->x == b->x && a->y == b->y) {
                    a->alive = false;
                    hit = true;
                    break;
                }
            }
        if(hit) {
            *p = b->next;
            free(b);
        } else {
            p = &b->next;
        }
    }
}

bool aliens_exist(Alien aliens[ALIEN_ROWS][ALIEN_COLS]) {
    for(int r = 0; r < ALIEN_ROWS; r++)
        for(int c = 0; c < ALIEN_COLS; c++)
            if(aliens[r][c].alive) return true;
    return false;
}

void draw_game(Alien aliens[ALIEN_ROWS][ALIEN_COLS], Bullet* bullets, int ship_x) {
    screenInit(1);
    for(int r = 0; r < ALIEN_ROWS; r++)
        for(int c = 0; c < ALIEN_COLS; c++) {
            Alien* a = &aliens[r][c];
            if(a->alive) {
                screenGotoxy(a->x, a->y);
                putchar(ALIEN_CHAR);
            }
        }
    for(Bullet* b = bullets; b; b = b->next) {
        screenGotoxy(b->x, b->y);
        putchar(BULLET_CHAR);
    }
    screenGotoxy(ship_x, MAXY - 2);
    putchar(SHIP_CHAR);
    screenUpdate();
}

void show_menu(int* alien_interval) {
    screenInit(0);
    printf("=== TERMINAL INVADERS ===\n");
    printf("1 - Fácil\n");      // invasores a cada 6 frames
    printf("2 - Médio\n");      // invasores a cada 4 frames
    printf("3 - Difícil\n");    // invasores a cada 3 frames    
    printf("Q - Sair\n");
    int k;
    do {
        k = readch();
    } while(k!='1' && k!='2' && k!='3' && k!='q' && k!='Q');
    if(k=='q' || k=='Q') {
        *alien_interval = -1;
        return;
    }
    switch(k) {
        case '1': *alien_interval = 6; break;
        case '2': *alien_interval = 4; break;
        case '3': *alien_interval = 3; break;
    }
}

void show_game_over(void) {
    screenInit(0);
    printf("### GAME OVER ###\n");
    printf("Pressione ENTER para retornar\n");
}

void show_victory(void) {
    screenInit(0);
    printf("*** VOCÊ VENCEU! ***\n");
    printf("Pressione ENTER para retornar\n");
}

int main(void) {
    keyboardInit();
    timerInit(1);
    srand((unsigned) time(NULL));

    bool exit_program = false;
    const int frame_delay = 1000000 / 60;
    int alien_interval = 6;
    int dx = 1;

    while(!exit_program) {
        show_menu(&alien_interval);
        if(alien_interval < 0) break;

        Alien aliens[ALIEN_ROWS][ALIEN_COLS];
        init_aliens(aliens);
        Bullet* bullets = NULL;
        int ship_x = MAXX / 2;
        bool running = true;
        int frame_count = 0;

        while(running) {
            screenClear();
            if(keyhit()) {
                int kk = readch();
                if(kk=='a' && ship_x>1)      ship_x--;
                if(kk=='d' && ship_x<MAXX-2) ship_x++;
                if(kk==' ') {
                    Bullet* b = malloc(sizeof *b);
                    b->x = ship_x;
                    b->y = MAXY - 3;
                    b->next = bullets;
                    bullets = b;
                }
                if(kk=='q') running = false;
            }

            if(frame_count % alien_interval == 0)
                if(!update_aliens(aliens, &dx)) break;

            update_bullets(&bullets, aliens);
            draw_game(aliens, bullets, ship_x);
            usleep(frame_delay);
            if(!aliens_exist(aliens)) break;
            frame_count++;
        }

        /* Limpa balas */
        while(bullets) {
            Bullet* t = bullets;
            bullets = bullets->next;
            free(t);
        }

        /* Mostra tela apropriada */
        if(!aliens_exist(aliens))
            show_victory();
        else
            show_game_over();

        while(readch()!='\n');
    }

    screenDestroy();
    keyboardDestroy();
    return 0;
}
