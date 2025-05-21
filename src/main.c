/* main.c: Terminal Invaders (Space Invaders adaptado) */
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

typedef struct {
  int x, y;
  bool alive;
} Alien;

typedef struct Bullet {
  int x, y;
  struct Bullet *next;
} Bullet;

// --- Inicialização ---
void init_aliens(Alien aliens[ALIEN_ROWS][ALIEN_COLS]) {
  for(int row=0; row<ALIEN_ROWS; row++)
    for(int col=0; col<ALIEN_COLS; col++) {
      aliens[row][col].alive = true;
      aliens[row][col].x = 2 + col*ALIEN_SPACING_X;
      aliens[row][col].y = 2 + row*ALIEN_SPACING_Y;
    }
}

// --- Atualização ---
void update_aliens(Alien aliens[ALIEN_ROWS][ALIEN_COLS], int *dx, bool *drop) {
  *drop = false;
  // verifica colisão borda
  for(int row=0; row<ALIEN_ROWS; row++)
    for(int col=0; col<ALIEN_COLS; col++) {
      Alien *a = &aliens[row][col];
      if(!a->alive) continue;
      int nx = a->x + *dx;
      if(nx<1 || nx>=MAXX-1) { *drop = true; break; }
    }
  if(*drop) *dx = -*dx;
  // move
  for(int row=0; row<ALIEN_ROWS; row++)
    for(int col=0; col<ALIEN_COLS; col++) {
      Alien *a = &aliens[row][col];
      if(!a->alive) continue;
      a->x += *dx;
      if(*drop) a->y++;
    }
}

void update_bullets(Bullet **head, Alien aliens[ALIEN_ROWS][ALIEN_COLS]) {
  Bullet **p = head;
  while(*p) {
    Bullet *b = *p;
    b->y--;
    if(b->y<1) { *p = b->next; free(b); continue; }
    bool hit=false;
    for(int row=0; row<ALIEN_ROWS && !hit; row++)
      for(int col=0; col<ALIEN_COLS; col++) {
        Alien *a = &aliens[row][col];
        if(a->alive && a->x==b->x && a->y==b->y) {
          a->alive=false;
          hit=true;
          break;
        }
      }
    if(hit) { *p = b->next; free(b); }
    else p=&b->next;
  }
}

// --- Desenho ---
void draw_screen(Alien aliens[ALIEN_ROWS][ALIEN_COLS], Bullet *bullets, int ship_x) {
  screenInit(1);
  // desenha aliens
  for(int row=0; row<ALIEN_ROWS; row++)
    for(int col=0; col<ALIEN_COLS; col++) {
      Alien *a = &aliens[row][col];
      if(a->alive) {
        screenGotoxy(a->x, a->y);
        putchar(ALIEN_CHAR);
      }
    }
  // desenha balas
  for(Bullet *b=bullets; b; b=b->next) {
    screenGotoxy(b->x, b->y);
    putchar(BULLET_CHAR);
  }
  // desenha nave
  screenGotoxy(ship_x, MAXY-2);
  putchar(SHIP_CHAR);

  screenUpdate();
}

bool aliens_exist(Alien aliens[ALIEN_ROWS][ALIEN_COLS]) {
  for(int r=0;r<ALIEN_ROWS;r++)
    for(int c=0;c<ALIEN_COLS;c++)
      if(aliens[r][c].alive) return true;
  return false;
}

// --- Main ---
int main() {
  keyboardInit();
  screenInit(0);
  timerInit(1);  // we'll use usleep manually
  srand(time(NULL));

  Alien aliens[ALIEN_ROWS][ALIEN_COLS];
  init_aliens(aliens);
  int dx = 1; bool drop=false;
  Bullet *bullets=NULL;
  int ship_x = MAXX/2;
  bool running = true;

  while(running) {
    screenClear();
    // input
    if(keyhit()) {
      int k=readch();
      if(k=='a' && ship_x>1) ship_x--;
      if(k=='d' && ship_x<MAXX-2) ship_x++;
      if(k==' '){
        Bullet *b=malloc(sizeof(Bullet));
        b->x=ship_x; b->y=MAXY-3; b->next=bullets;
        bullets=b;
      }
      if(k=='q') running=false;
    }
    // update
    update_aliots:
    update_aliens(aliens,&dx,&drop);
    update_bullets(&bullets,aliens);
    // draw
    draw_screen(aliens,bullets,ship_x);
    // delay
    usleep(FRAME_DELAY_US);
    if(!aliens_exist(aliens)) break;
  }

  // fim de jogo
  screenDestroy();
  keyboardDestroy();
  return 0;
}
