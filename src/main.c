#include "library.h"
#include "OPUS_LOADSAVE/OPUS_loadsave.h"
#include "WINDOWS/windows.h"
//author: Lukasz Dubiel
#define SDL_main main

void sdl_init() {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        printf("Error\n");
        atexit(SDL_Quit);
        exit(1);
    }
}
void run() {
    char chosen_key[2];
    int chosen_metre[2];
    int opt;
    OPUS *current_OPUS = NULL, *prev_OPUS = NULL;
    while (1) {

        opt = menu_open(chosen_key, chosen_metre);
        if (opt == CREATING_WINDOW_CODE) {
            current_OPUS = create_new_OPUS(chosen_key, chosen_metre, NULL);
            save_OPUS_as_TextFile(current_OPUS);
            free(current_OPUS);
        } else if (opt == MENU_LOAD_CODE) {
            prev_OPUS = fscanf_opus(NULL);
            current_OPUS = create_new_OPUS(prev_OPUS->key, prev_OPUS->time_sign, prev_OPUS);
            save_OPUS_as_TextFile(current_OPUS);
            free_opus(current_OPUS);
            //free_opus(prev_OPUS);
        } else if (opt == MENU_EXIT_CODE) {
            break;
        }
    }
}


int main(int argc, char *argv[]) {

    sdl_init();
    run();
    SDL_Quit();

    return 0;
}
