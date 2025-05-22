/* main.c: Terminal Invaders com menu e Game Over */
#include "keyboard.h"
#include "screen.h"
#include "timer.h"
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>

#define MAXX 80
#define MAXY 24
#define ALIEN_ROWS 3
#define ALIEN_COLS 10
#define ALIEN_SPACING_X 6
#define ALIEN_SPACING_Y 2
#define SHIP_CHAR '^'
#define ALIEN_CHAR 'W'
#define BULLET_CHAR '|'
#define FRAME_DELAY_US 50000  // ~20 FPS

typedef struct { int x, y; bool alive; } Alien;
typedef struct Bullet { int x, y; struct Bullet* next; } Bullet;

// Inicializa invasores
void init_aliens(Alien aliens[ALIEN_ROWS][ALIEN_COLS]) {
  for(int r=0;r<ALIEN_ROWS;r++)
    for(int c=0;c<ALIEN_COLS;c++) {
      aliens[r][c].alive = true;
      aliens[r][c].x = 2 + c * ALIEN_SPACING_X;
      aliens[r][c].y = 2 + r * ALIEN_SPACING_Y;
    }
}

// Atualiza invasores e retorna false se atingirem a linha da nave
bool update_aliens(Alien aliens[ALIEN_ROWS][ALIEN_COLS], int* dx) {
  bool drop = false;
  for(int r=0;r<ALIEN_ROWS && !drop;r++)
    for(int c=0;c<ALIEN_COLS;c++) {
      Alien* a = &aliens[r][c];
      if(!a->alive) continue;
      int nx = a->x + *dx;
      if(nx < 1 || nx >= MAXX-1) { drop = true; break; }
    }
  if(drop) *dx = -*dx;
  for(int r=0;r<ALIEN_ROWS;r++)
    for(int c=0;c<ALIEN_COLS;c++) {
      Alien* a = &aliens[r][c];
      if(!a->alive) continue;
      a->x += *dx;
      if(drop) a->y++;
      if(a->y >= MAXY-2) return false;  // game over
    }
  return true;
}

// Atualiza balas e marca colisões
void update_bullets(Bullet** head, Alien aliens[ALIEN_ROWS][ALIEN_COLS]) {
  Bullet** p = head;
  while(*p) {
    Bullet* b = *p;
    b->y--;
    if(b->y < 1) { *p = b->next; free(b); continue; }
    bool hit = false;
    for(int r=0;r<ALIEN_ROWS && !hit;r++)
      for(int c=0;c<ALIEN_COLS;c++) {
        Alien* a = &aliens[r][c];
        if(a->alive && a->x == b->x && a->y == b->y) {
          a->alive = false;
          hit = true;
          break;
        }
      }
    if(hit) { *p = b->next; free(b); }
    else p = &b->next;
  }
}

// Checa se ainda há invasores vivos
bool aliens_exist(Alien aliens[ALIEN_ROWS][ALIEN_COLS]) {
  for(int r=0;r<ALIEN_ROWS;r++)
    for(int c=0;c<ALIEN_COLS;c++)
      if(aliens[r][c].alive) return true;
  return false;
}

// Desenha o estado do jogo
void draw_game(Alien aliens[ALIEN_ROWS][ALIEN_COLS], Bullet* bullets, int ship_x) {
  screenInit(1);
  for(int r=0;r<ALIEN_ROWS;r++)
    for(int c=0;c<ALIEN_COLS;c++) {
      Alien* a = &aliens[r][c];
      if(a->alive) {
        screenGotoxy(a->x, a->y);
        putchar(ALIEN_CHAR);
      }
    }
  for(Bullet* b=bullets; b; b=b->next) {
    screenGotoxy(b->x, b->y);
    putchar(BULLET_CHAR);
  }
  screenGotoxy(ship_x, MAXY-2);
  putchar(SHIP_CHAR);
  screenUpdate();
}

// Exibe menu inicial
void show_menu() {
  screenInit(0);
  printf("=== TERMINAL INVADERS ===\n");
  printf("[ENTER] Iniciar   [Q] Sair\n");
}

// Exibe tela de Game Over
void show_game_over() {
  screenInit(0);
  printf("### GAME OVER ###\n");
  printf("[ENTER] Menu\n");
}

int main() {
  keyboardInit();
  timerInit(1);
  srand(time(NULL));
  bool exit = false;

  while(!exit) {
    show_menu();
    int k;
    do { k = readch(); } while(k != '\n' && k != 'q' && k != 'Q');
    if(k=='q' || k=='Q') break;

    Alien aliens[ALIEN_ROWS][ALIEN_COLS]; init_aliens(aliens);
    Bullet* bullets = NULL;
    int ship_x = MAXX/2;
    int dx = 1;
    bool running = true;

    while(running) {
      screenClear();

      if(keyhit()) {
        int kk = readch();
        if(kk=='a' && ship_x>1) ship_x--;
        if(kk=='d' && ship_x<MAXX-2) ship_x++;
        if(kk==' ') {
          Bullet* b = malloc(sizeof *b);
          b->x = ship_x; b->y = MAXY-3; b->next = bullets;
          bullets = b;
        }
        if(kk=='q') running = false;
      }

      if(!update_aliens(aliens, &dx)) break;
      update_bullets(&bullets, aliens);
      draw_game(aliens, bullets, ship_x);
      usleep(FRAME_DELAY_US);
      if(!aliens_exist(aliens)) break;
    }

    // libera balas
    while(bullets) { Bullet* t = bullets; bullets = bullets->next; free(t); }

    show_game_over();
    do { k = readch(); } while(k != '\n');
  }

  screenDestroy();
  keyboardDestroy();
  return 0;
}
