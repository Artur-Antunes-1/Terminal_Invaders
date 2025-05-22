/* main.c: Terminal Invaders com menu, game over e seleção de dificuldade */
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

// Cores (preservadas)
#define RESET          "\x1b[0m"
#define PRETO          "\x1b[30m"
#define VERMELHO       "\x1b[31m"
#define VERDE          "\x1b[32m"
#define AMARELO        "\x1b[33m"
#define AZUL           "\x1b[34m"
#define MAGENTA        "\x1b[35m"
#define CIANO          "\x1b[36m"
#define BRANCO         "\x1b[37m"
#define CINZA          "\x1b[90m"
#define VERMELHO_CLARO "\x1b[91m"

typedef struct { int x, y; bool alive; } Alien;
typedef struct Bullet { int x, y; struct Bullet* next; } Bullet;

// Protótipos
void init_aliens(Alien aliens[ALIEN_ROWS][ALIEN_COLS]);
bool update_aliens(Alien aliens[ALIEN_ROWS][ALIEN_COLS], int* dx);
void update_bullets(Bullet** head, Alien aliens[ALIEN_ROWS][ALIEN_COLS]);
bool aliens_exist(Alien aliens[ALIEN_ROWS][ALIEN_COLS]);
void draw_game(Alien aliens[ALIEN_ROWS][ALIEN_COLS], Bullet* bullets, int ship_x);
void show_menu(int* delay_us);
void show_game_over(void);

// Inicializa invasores
void init_aliens(Alien aliens[ALIEN_ROWS][ALIEN_COLS]) {
  for(int r=0; r<ALIEN_ROWS; r++)
    for(int c=0; c<ALIEN_COLS; c++) {
      aliens[r][c].alive = true;
      aliens[r][c].x = 2 + c * ALIEN_SPACING_X;
      aliens[r][c].y = 2 + r * ALIEN_SPACING_Y;
    }
}

// Atualiza invasores; retorna false se atingirem a linha da nave
bool update_aliens(Alien aliens[ALIEN_ROWS][ALIEN_COLS], int* dx) {
  bool drop = false;
  for(int r=0; r<ALIEN_ROWS && !drop; r++)
    for(int c=0; c<ALIEN_COLS; c++) {
      Alien* a = &aliens[r][c]; if(!a->alive) continue;
      int nx = a->x + *dx;
      if(nx < 1 || nx >= MAXX-1) { drop = true; break; }
    }
  if(drop) *dx = -*dx;
  for(int r=0; r<ALIEN_ROWS; r++)
    for(int c=0; c<ALIEN_COLS; c++) {
      Alien* a = &aliens[r][c]; if(!a->alive) continue;
      a->x += *dx;
      if(drop) a->y++;
      if(a->y >= MAXY-2) return false;
    }
  return true;
}

// Atualiza balas e colisões
void update_bullets(Bullet** head, Alien aliens[ALIEN_ROWS][ALIEN_COLS]) {
  Bullet** p = head;
  while(*p) {
    Bullet* b = *p;
    b->y--;
    if(b->y < 1) { *p = b->next; free(b); continue; }
    bool hit = false;
    for(int r=0; r<ALIEN_ROWS && !hit; r++)
      for(int c=0; c<ALIEN_COLS; c++) {
        Alien* a = &aliens[r][c];
        if(a->alive && a->x == b->x && a->y == b->y) { a->alive = false; hit = true; break; }
      }
    if(hit) { *p = b->next; free(b); }
    else p = &b->next;
  }
}

// Verifica se ainda há invasores vivos
bool aliens_exist(Alien aliens[ALIEN_ROWS][ALIEN_COLS]) {
  for(int r=0; r<ALIEN_ROWS; r++)
    for(int c=0; c<ALIEN_COLS; c++)
      if(aliens[r][c].alive) return true;
  return false;
}

// Desenha o jogo
void draw_game(Alien aliens[ALIEN_ROWS][ALIEN_COLS], Bullet* bullets, int ship_x) {
  screenInit(1);
  for(int r=0; r<ALIEN_ROWS; r++)
    for(int c=0; c<ALIEN_COLS; c++) {
      Alien* a = &aliens[r][c];
      if(a->alive) { screenGotoxy(a->x, a->y); putchar(ALIEN_CHAR); }
    }
  for(Bullet* b=bullets; b; b=b->next) { screenGotoxy(b->x, b->y); putchar(BULLET_CHAR); }
  screenGotoxy(ship_x, MAXY-2); putchar(SHIP_CHAR);
  screenUpdate();
}

// Menu de dificuldade
void show_menu(int* delay_us) {
  screenInit(0);
  printf("=== TERMINAL INVADERS ===\n");
  printf("1 - Fácil (20 FPS)\n");
  printf("2 - Médio (30 FPS)\n");
  printf("3 - Difícil (40 FPS)\n");
  printf("Q - Sair\n");
  int k;
  do { k = readch(); } while(k!='1'&&k!='2'&&k!='3'&&k!='q'&&k!='Q');
  if(k=='q'||k=='Q') { *delay_us = -1; return; }
  *delay_us = (k=='1'?50000:(k=='2'?33333:25000));
}

// Tela de Game Over
void show_game_over(void) {
  screenInit(0);
  printf("### GAME OVER ###\n");
  printf("Pressione ENTER para retornar\n");
}

// --- Função principal ---
int main() {
  keyboardInit(); timerInit(1); srand(time(NULL));
  bool exit = false;
  int frame_delay = 50000;

  while(!exit) {
    show_menu(&frame_delay);
    if(frame_delay < 0) break;

    Alien aliens[ALIEN_ROWS][ALIEN_COLS]; init_aliens(aliens);
    Bullet* bullets = NULL;
    int ship_x = MAXX/2;
    int dx = 1;
    bool running = true;

    while(running) {
      screenClear();
      if(keyhit()) {
        int kk = readch(); if(kk=='a'&&ship_x>1) ship_x--;
        if(kk=='d'&&ship_x<MAXX-2) ship_x++;
        if(kk==' ') { Bullet* b = malloc(sizeof *b); b->x = ship_x; b->y = MAXY-3; b->next = bullets; bullets = b; }
        if(kk=='q') running = false;
      }
      if(!update_aliens(aliens, &dx)) break;
      update_bullets(&bullets, aliens);
      draw_game(aliens, bullets, ship_x);
      usleep(frame_delay);
      if(!aliens_exist(aliens)) break;
    }

    while(bullets) { Bullet* t = bullets; bullets = bullets->next; free(t); }
    show_game_over();
    while(readch()!='\n');
  }
  screenDestroy(); keyboardDestroy();
  return 0;
}