#include <stdlib.h>
#include <stdio.h>
#include <keyboard.h>
#include <screen.h>
#include <timer.h>


#include "game.h"
#include "input.h"
#include "draw.h"
#include "score.h"

int main(void) {
    int running = 1;
    keyboardInit();
    screenInit(1);
    timerInit(100);

    load_highscores();
    init_game();

    while (running && score >= 0) {
        process_input(&running);
        update_game();
        draw_frame();
        while (!timerTimeOver()) { /* espera */ }
        timerUpdateTimer(100);
    }

    screenClear();
    screenGotoxy(WIDTH/2 - 5, HEIGHT/2);
    if (score < 0) printf("GAME OVER\n");
    else           printf("BYE\n");
    screenGotoxy(WIDTH/2 - 7, HEIGHT/2 + 1);
    printf("Final Score: %d\n", score<0? 0: score);
    screenUpdate();

    save_highscores();

    timerDestroy();
    screenDestroy();
    keyboardDestroy();
    return 0;
}
