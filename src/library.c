#include "library.h"


#ifdef __cplusplus
extern "C"
#endif

int pow_int(int a, int b) {
    if (a == 0 || a == 1) return a;
    if (b == 0) return 1;
    int p = 1, i;
    for (i = 0; i < b; i++) {
        p *= a;
    }
    return p;
}

int fscanf_note(FILE *opus_text_file, NOTES *n) {

    char c;
    if (n == NULL) {
        return 1;
    }
    n->name = (char)getc(opus_text_file);
    if (getc(opus_text_file) != ':') {
        printf("Opus is written incorrectly");
        exit(1);
    }
    n->acci = (char)getc(opus_text_file);
    if (getc(opus_text_file) != ':') {
        printf("Opus is written incorrectly");
        exit(1);
    }
    n->height = (int)(getc(opus_text_file) - 48);
    if (getc(opus_text_file) != ')') {
        printf("Opus is written incorrectly");
        exit(1);
    }

    return 0;
}
CHORD *fscanf_chord(FILE *opus_text_file) {

    CHORD *chord = NULL;
    int i;
    char c;
    chord = malloc(sizeof(*chord));

    chord->time = getc(opus_text_file) - 48;
    if (getc(opus_text_file) != ':') {
        printf("Opus is written incorrectly");
        exit(1);
    }
    chord->_time_dots_ = getc(opus_text_file) - 48;
    if (getc(opus_text_file) != ':') {
        printf("Opus is written incorrectly");
        exit(1);
    }
    chord->ssp_articulation = getc(opus_text_file) - 48;
    if (getc(opus_text_file) != ':') {
        printf("Opus is written incorrectly");
        exit(1);
    }
    chord->notes_number = getc(opus_text_file) - 48;
    if (getc(opus_text_file) != ':') {
        printf("Opus is written incorrectly");
        exit(1);
    }
    chord->X_position = 0;
    while ((c = (char)getc(opus_text_file)) != ':') {
        chord->X_position *= 10;
        chord->X_position += ((int)c - 48);
    }

    for (i = 0; i < 7; i++) {
        chord->local_serial_key[i] = (char)getc(opus_text_file);
    }
    if (getc(opus_text_file) != ':') {
        printf("Opus is written incorrectly");
        exit(1);
    }

    for (i = 0; i < chord->notes_number; i++) {
        if (getc(opus_text_file) != '(') {
            printf("Opus is written incorrectly");
            exit(1);
        }
        fscanf_note(opus_text_file, &chord->notes_[i]);
    }

    if (getc(opus_text_file) != ']') {
        printf("Opus is written incorrectly");
        exit(1);
    }

    return chord;
}
BARS *fscanf_bar(FILE *opus_text_file) {

    BARS *bar = NULL;
    bar = malloc(sizeof(*bar));
    char c;
    bar->X_of_start_bar = 0;
    while ((c = (char)getc(opus_text_file)) != ':') {
        bar->X_of_start_bar *= 10;
        bar->X_of_start_bar += ((int)c - 48);
    }
    bar->width_ = 0;
    while ((c = (char)getc(opus_text_file)) != ':') {
        bar->width_ *= 10;
        bar->width_ += ((int)c - 48);
    }
    bar->brace = (int)getc(opus_text_file) - 48;
    if (getc(opus_text_file) != ':') {
        printf("Opus is written incorrectly");
        exit(1);
    }

    CHORD *help_chord = NULL;
    bar->prev = NULL;
    bar->next = NULL;

    if (getc(opus_text_file) != '[') {
        printf("Opus is written incorrectly");
        exit(1);
    }

    bar->first_chord_treb = fscanf_chord(opus_text_file);
    help_chord = bar->first_chord_treb;
    help_chord->prev = NULL;
    help_chord->next = NULL;

    while ((c = (char)getc(opus_text_file)) != '|') {

        help_chord->next = fscanf_chord(opus_text_file);
        help_chord->next->prev = help_chord;
        help_chord = help_chord->next;
    }
    help_chord->next = NULL;

    if (getc(opus_text_file) != '[') {
        printf("Opus is written incorrectly");
        exit(1);
    }

    bar->first_chord_bass = fscanf_chord(opus_text_file);
    help_chord = bar->first_chord_bass;
    help_chord->prev = NULL;
    help_chord->next = NULL;
    while ((c = (char)getc(opus_text_file)) != '}') {
        help_chord->next = fscanf_chord(opus_text_file);
        help_chord->next->prev = help_chord;
        help_chord = help_chord->next;
    }
    help_chord->next = NULL;


    return bar;
}
OPUS *fscanf_opus(const char *path) {

    OPUS *opus = NULL;
    opus = malloc(sizeof(*opus));
    char c;
    int i;

    FILE *opus_text_file;
    if (path != NULL) {
        if ((opus_text_file = fopen(path, "r")) == NULL) {
            printf("Opening error\n");
            exit(1);
        }
    } else {
        if ((opus_text_file = fopen("opus.txt", "r")) == NULL) {
            printf("Opening error\n");
            exit(1);
        }
    }
    i = 0;
    while ((c = (char)getc(opus_text_file)) != ':') {
        opus->title[i] = c;
        i++;
    }
    opus->title[i] = '\0';
    i = 0;
    while ((c = (char)getc(opus_text_file)) != ':') {
        opus->author[i] = c;
        i++;
    }
    opus->author[i] = '\0';

    opus->key[0] = (char)getc(opus_text_file);
    if ((c = (char)getc(opus_text_file)) != ':') {
        opus->key[1] = c;
        getc(opus_text_file);
    } else {
        opus->key[1] = '\0';
    }

    for (i = 0; i < 7; i++) {
        opus->default_serial_key[i] = (char)getc(opus_text_file);
    }
    getc(opus_text_file);

    fscanf(opus_text_file, "%d", &(opus->time_sign[0]));
    getc(opus_text_file);
    fscanf(opus_text_file, "%d", &(opus->time_sign[1]));
    getc(opus_text_file);
    fscanf(opus_text_file, "%d", &(opus->temp));
    getc(opus_text_file);

    if (getc(opus_text_file) != '{') {
        printf("Opus is written incorrectly");
        exit(1);
    }

    BARS *help_bar = NULL;
    opus->first_BAR = fscanf_bar(opus_text_file);
    opus->first_BAR->prev = NULL;
    opus->first_BAR->next = NULL;
    help_bar = opus->first_BAR;

    while (getc(opus_text_file) == '{') {

        help_bar->next = fscanf_bar(opus_text_file);
        help_bar->next->prev = help_bar;
        help_bar = help_bar->next;
    }
    help_bar->next = NULL;

    fclose(opus_text_file);
    return opus;
}

/////////////////////Functions for menu:
void scroll_updown(SDL_Surface *screen, SDL_Surface *stave, SDL_Rect *current, SDL_Event *occurrence) {

    int y = current->y;
    int s = occurrence->wheel.y;

    y -= s * 10;

    if (y > stave->h - screen->h) y = stave->h - screen->h;
    if (y < 0) y = 0;

    current->y = y;
    //SDL_BlitSurface(stave, current, screen, NULL);

}
int get_path_to_key_bmp(const char chosen_key[2], char path_to_key_bmp[], int hand, int path_prefix_len) {

    if (hand == 0) path_to_key_bmp[path_prefix_len] = 't';
    else path_to_key_bmp[path_prefix_len] = 'b';

    /*
     * For sharp keys
     */
    // G dur, a moll
    if ((chosen_key[0] == 'G' && chosen_key[1] == '\0') || (chosen_key[0] == 'e' && chosen_key[1] == '\0')) {
        path_to_key_bmp[path_prefix_len + 1] = '1';
        path_to_key_bmp[path_prefix_len + 2] = 's';
        return 0;
    }
    // D dur, h moll
    if ((chosen_key[0] == 'D' && chosen_key[1] == '\0') || (chosen_key[0] == 'h' && chosen_key[1] == '\0')) {
        path_to_key_bmp[path_prefix_len + 1] = '2';
        path_to_key_bmp[path_prefix_len + 2] = 's';
        return 0;
    }
    // A dur, fis moll
    if ((chosen_key[0] == 'A' && chosen_key[1] == '\0') || (chosen_key[0] == 'f' && chosen_key[1] == 'i')) {
        path_to_key_bmp[path_prefix_len + 1] = '3';
        path_to_key_bmp[path_prefix_len + 2] = 's';
        return 0;
    }
    // E dur, cis moll
    if ((chosen_key[0] == 'E' && chosen_key[1] == '\0') || (chosen_key[0] == 'c' && chosen_key[1] == 'i')) {
        path_to_key_bmp[path_prefix_len + 1] = '4';
        path_to_key_bmp[path_prefix_len + 2] = 's';
        return 0;
    }
    // H dur, gis moll
    if ((chosen_key[0] == 'H' && chosen_key[1] == '\0') || (chosen_key[0] == 'g' && chosen_key[1] == 'i')) {
        path_to_key_bmp[path_prefix_len + 1] = '5';
        path_to_key_bmp[path_prefix_len + 2] = 's';
        return 0;
    }
    // Fis dur, dis moll
    if ((chosen_key[0] == 'F' && chosen_key[1] == 'i') || (chosen_key[0] == 'd' && chosen_key[1] == 'i')) {
        path_to_key_bmp[path_prefix_len + 1] = '6';
        path_to_key_bmp[path_prefix_len + 2] = 's';
        return 0;
    }
    // Cis dur, ais moll
    if ((chosen_key[0] == 'C' && chosen_key[1] == 'i') || (chosen_key[0] == 'a' && chosen_key[1] == 'i')) {
        path_to_key_bmp[path_prefix_len + 1] = '7';
        path_to_key_bmp[path_prefix_len + 2] = 's';
        return 0;
    }
    //  C dur, a moll
    if ((chosen_key[0] == 'C' && chosen_key[1] == '\0') || (chosen_key[0] == 'a' && chosen_key[1] == '\0')) {
        return 0;
    }

    /*
     *For bmol keys
     */

    // F dur, d moll
    if ((chosen_key[0] == 'F' && chosen_key[1] == '\0') || (chosen_key[0] == 'd' && chosen_key[1] == '\0')) {
        path_to_key_bmp[path_prefix_len + 1] = '1';
        path_to_key_bmp[path_prefix_len + 2] = 'b';
        return 0;
    }
    // B dur, g moll
    if ((chosen_key[0] == 'B' && chosen_key[1] == '\0') || (chosen_key[0] == 'g' && chosen_key[1] == '\0')) {
        path_to_key_bmp[path_prefix_len + 1] = '2';
        path_to_key_bmp[path_prefix_len + 2] = 'b';
        return 0;
    }
    // Es dur, c moll
    if ((chosen_key[0] == 'E' && chosen_key[1] == 's') || (chosen_key[0] == 'c' && chosen_key[1] == '\0')) {
        path_to_key_bmp[path_prefix_len + 1] = '3';
        path_to_key_bmp[path_prefix_len + 2] = 'b';
        return 0;
    }
    // As dur, f moll
    if ((chosen_key[0] == 'A' && chosen_key[1] == 's') || (chosen_key[0] == 'f' && chosen_key[1] == '\0')) {
        path_to_key_bmp[path_prefix_len + 1] = '4';
        path_to_key_bmp[path_prefix_len + 2] = 'b';
        return 0;
    }
    // Des dur, b moll
    if ((chosen_key[0] == 'D' && chosen_key[1] == 'e') || (chosen_key[0] == 'b' && chosen_key[1] == '\0')) {
        path_to_key_bmp[path_prefix_len + 1] = '5';
        path_to_key_bmp[path_prefix_len + 2] = 'b';
        return 0;
    }
    // Ges dur, es moll
    if ((chosen_key[0] == 'G' && chosen_key[1] == 'e') || (chosen_key[0] == 'e' && chosen_key[1] == 's')) {
        path_to_key_bmp[path_prefix_len + 1] = '6';
        path_to_key_bmp[path_prefix_len + 2] = 'b';
        return 0;
    }
    // Ces dur, as moll
    if ((chosen_key[0] == 'C' && chosen_key[1] == 'e') || (chosen_key[0] == 'a' && chosen_key[1] == 's')) {
        path_to_key_bmp[path_prefix_len + 1] = '7';
        path_to_key_bmp[path_prefix_len + 2] = 'b';
        return 0;
    }

    return 1;
}
int put_key(SDL_Surface *stave, const char chosen_key[2], OPUS *current_OPUS, int *X_star_on_line) {

    if (current_OPUS != NULL) {
        current_OPUS->key[0] = chosen_key[0];
        current_OPUS->key[1] = chosen_key[1];
    }

    if (chosen_key[0] == 'C' && chosen_key[1] == '\0') {
        return 0;
    }

    SDL_Surface *key_accidentals = NULL;
    SDL_Rect dst;
    dst.x = X_START_AFTER_KEY + 1;

    // Trzy zera ścieżki zostają podmienione na
    // znaki: ręka{t, b}, liczba znaków{1,..., 7}, typ znaku{s, b}
    char path_to_key_bmp[] = "obrazki/keys/000.bmp";
    const int path_prefix_len = 13;

    int hand, brace, diff_of_hights;

    for (hand = 0; hand < NUMBER_OF_HANDS; hand++) {

        if (get_path_to_key_bmp(chosen_key, path_to_key_bmp, hand, path_prefix_len)) {
            return 1;
        }
        if (key_accidentals != NULL) SDL_FreeSurface(key_accidentals);
        key_accidentals = SDL_LoadBMP(path_to_key_bmp);
        if (key_accidentals == NULL) {
            return 1;
        }

        SDL_SetColorKey(key_accidentals, SDL_TRUE, SDL_MapRGB(key_accidentals->format, 255, 255, 255));

        if (*X_star_on_line < X_START_AFTER_KEY + key_accidentals->w) {
            *X_star_on_line = X_START_AFTER_KEY + key_accidentals->w;
        }

        if (hand == 0) {
            if (path_to_key_bmp[path_prefix_len + 2] == 's') diff_of_hights = Y_OF_FIRST_LINE_TREB_SHARP_KEY;
            else diff_of_hights = Y_OF_FIRST_LINE_TREB_BMOL_KEY;

        } else {
            if (path_to_key_bmp[path_prefix_len + 2] == 's') diff_of_hights = Y_OF_FIRST_LINE_BASS_SHARP_KEY;
            else diff_of_hights = Y_OF_FIRST_LINE_BASS_BMOL_KEY;
        }

        for (brace = 0; brace < NUMBER_OF_BRACES; brace++) {

            dst.y = Y_FIRST_LINE + hand * DISTANCE_BETWEEN_FIRST_LINE_HANDS_STAVES +
                    brace * DISTANCE_BETWEEN_FIRST_LINE_BRACES - diff_of_hights;
            SDL_BlitSurface(key_accidentals, NULL, stave, &dst);

        }
    }

    SDL_FreeSurface(key_accidentals);
    return 0;
}
int get_path_to_metre_bmp(const int chosen_metre[2], char path_to_metre_bmp[], int path_prefix_len) {

    if (chosen_metre[0] < 10) {
        path_to_metre_bmp[path_prefix_len] = (char) (chosen_metre[0] + 48);
    } else {
        path_to_metre_bmp[path_prefix_len] = (char) ((int) ('g') + chosen_metre[0] - 16);
    }

    if (chosen_metre[1] < 10) {
        path_to_metre_bmp[path_prefix_len + 1] = (char) (chosen_metre[1] + 48);
    } else {
        path_to_metre_bmp[path_prefix_len + 1] = (char) ((int) ('g') + chosen_metre[1] - 16);
    }

    return 0;
}
int put_metre(SDL_Surface *stave, const int chosen_metre[2], OPUS *current_OPUS, int *X_start_on_line, int brace) {

    if (current_OPUS != NULL) {
        current_OPUS->time_sign[0] = chosen_metre[0];
        current_OPUS->time_sign[1] = chosen_metre[1];
    }

    SDL_Surface *metre_sign = NULL;
    SDL_Rect dst;
    dst.x = *X_start_on_line + DISTANCE_BETWEEN_KEY_AND_METRE;
    dst.y = Y_FIRST_LINE;

    //// Two zeros are nunmerator and denomiantor metre.
    char path_to_metre_bmp[] = "obrazki/metre/metre_00.bmp";
    const int path_prefix_len = 20;
    get_path_to_metre_bmp(chosen_metre, path_to_metre_bmp, path_prefix_len);


    metre_sign = SDL_LoadBMP(path_to_metre_bmp);
    if (metre_sign == NULL) {
        return 1;
    }
    SDL_SetColorKey(metre_sign, SDL_TRUE, SDL_MapRGB(metre_sign->format, 255, 255, 255));

    int hands;
    for (hands = 0; hands < NUMBER_OF_HANDS; hands++) {

        dst.y = Y_FIRST_LINE + hands * DISTANCE_BETWEEN_FIRST_LINE_HANDS_STAVES +
                brace * DISTANCE_BETWEEN_FIRST_LINE_BRACES;
        SDL_BlitSurface(metre_sign, NULL, stave, &dst);
    }

    *X_start_on_line = dst.x + metre_sign->w;
    SDL_FreeSurface(metre_sign);
    return 0;
}
int help_window(SDL_Surface *screen, SDL_Window *window) {

    SDL_Surface *instructions_ = NULL;
    instructions_ = SDL_LoadBMP("obrazki/menu/instructions.bmp");
    SDL_Event occurrence;
    int may_exit = 0, current_menu_opt = MENU_WINDOW_CODE;
    SDL_BlitSurface(instructions_, NULL, screen, NULL);
    SDL_UpdateWindowSurface(window);

    while (may_exit == 0) {
        while (SDL_PollEvent(&occurrence)) {
            if (occurrence.type == SDL_QUIT) {
                may_exit = 1;
                current_menu_opt = MENU_EXIT_CODE;
            }
            if (occurrence.key.type != SDL_KEYUP) {
                SDL_Delay(80);
            }
            if (occurrence.key.type != SDL_KEYDOWN && occurrence.key.keysym.sym == SDLK_ESCAPE) {
                may_exit = 1;
                current_menu_opt = MENU_WINDOW_CODE;
            }

            SDL_UpdateWindowSurface(window);
            SDL_Delay(20);
        }
    }
    return current_menu_opt;
}
////It returns option code
int metre_choosing(SDL_Surface *screen, SDL_Window *window, int chosen_metre[2])  {

    chosen_metre[0] = 0;
    chosen_metre[1] = 0;
    SDL_Event occurrence;
    SDL_Surface *metre_chose = NULL;
    SDL_Surface *stav = NULL;
    SDL_Surface *digits[10];
    char path[33] = "obrazki/metre_choose/digit_0.bmp";
    int pos = 27, i, may_exit = 0, tr = 0, key, current_opt = METRE_WINDOW_CODE;
    for (i = 0; i < 10; i++) {
        path[pos] = (char) (i + 48);
        digits[i] = SDL_LoadBMP(path);
        SDL_SetColorKey(digits[i], SDL_TRUE, SDL_MapRGB(digits[i]->format, 255, 255, 255));

    }
    metre_chose = SDL_LoadBMP("obrazki/metre_choose/metre_chosing.bmp");
    stav = SDL_LoadBMP("obrazki/metre_choose/stave.bmp");


    SDL_Rect RECT_STAVE_SRC[2], RECT_STAVE_DST[2], RECT_DIGIT_DST[8];
    RECT_STAVE_SRC[0].x = 0;
    RECT_STAVE_SRC[0].y = 0;
    RECT_STAVE_SRC[0].w = stav->w;
    RECT_STAVE_SRC[0].h = stav->h / 2;
    RECT_STAVE_SRC[1].x = 0;
    RECT_STAVE_SRC[1].y = stav->h / 2;
    RECT_STAVE_SRC[1].w = stav->w;
    RECT_STAVE_SRC[1].h = stav->h / 2;

    RECT_STAVE_DST[0].x = X_STAVE_METRE_CHOOSING;
    RECT_STAVE_DST[0].y = FIRST_Y_METRE_CHOOSE - 2;
    RECT_STAVE_DST[1].x = X_STAVE_METRE_CHOOSING;
    RECT_STAVE_DST[1].y = SECOND_Y_METRE_CHOOSE - 1;

    RECT_DIGIT_DST[0].x = FIRST_X_METRE_CHOOSE;
    RECT_DIGIT_DST[0].y = FIRST_Y_METRE_CHOOSE;
    RECT_DIGIT_DST[1].x = SECOND_X_METRE_CHOOSE;
    RECT_DIGIT_DST[1].y = FIRST_Y_METRE_CHOOSE;
    RECT_DIGIT_DST[2].x = THIRD_X_METRE_CHOOSE;
    RECT_DIGIT_DST[2].y = FIRST_Y_METRE_CHOOSE;
    RECT_DIGIT_DST[3].x = FIRST_X_METRE_CHOOSE;
    RECT_DIGIT_DST[3].y = SECOND_Y_METRE_CHOOSE;
    RECT_DIGIT_DST[4].x = SECOND_X_METRE_CHOOSE;
    RECT_DIGIT_DST[4].y = SECOND_Y_METRE_CHOOSE;
    RECT_DIGIT_DST[5].x = THIRD_X_METRE_CHOOSE;
    RECT_DIGIT_DST[5].y = SECOND_Y_METRE_CHOOSE;

    SDL_BlitSurface(metre_chose, NULL, screen, NULL);
    SDL_BlitSurface(stav, NULL, screen, &RECT_STAVE_DST[0]);

    while (!may_exit) {
        while (SDL_PollEvent(&occurrence)) {
            if (occurrence.type == SDL_QUIT) {
                may_exit = 1;
                current_opt = MENU_EXIT_CODE;
            }


            if (tr == 0 && occurrence.key.type == SDL_KEYDOWN && occurrence.key.keysym.sym == SDLK_DOWN) {
                tr = 1;
            } else if (tr == 1 && occurrence.key.type == SDL_KEYDOWN && occurrence.key.keysym.sym == SDLK_UP) {
                tr = 0;
            }

            if (occurrence.key.type != SDL_KEYUP) {
                SDL_Delay(60);
            }

            if (occurrence.key.type == SDL_KEYDOWN) {
                key = occurrence.key.keysym.sym;
                if ((SDLK_0 <= key && key <= SDLK_9) || key == SDLK_BACKSPACE) {

                    if (SDLK_0 <= key && key <= SDLK_9) {
                        if (chosen_metre[tr] < 10) {
                            chosen_metre[tr] = chosen_metre[tr] * 10 + (key - 48);
                        }
                    } else {
                        chosen_metre[tr] /= 10;
                    }

                    SDL_BlitSurface(stav, &RECT_STAVE_SRC[tr], screen, &RECT_STAVE_DST[tr]);


                    //// When numerator or denominator is single digit it
                    //// is placed at the middle of top or button of stave,

                    //// or it built with two digits, then at left and right.
                    if (0 < chosen_metre[tr] && chosen_metre[tr] < 10) {
                        SDL_BlitSurface(digits[chosen_metre[tr]], NULL, screen, &RECT_DIGIT_DST[3 * tr + 1]);

                    } else if (chosen_metre[tr] >= 10) {

                        SDL_BlitSurface(digits[chosen_metre[tr] % 10], NULL, screen, &RECT_DIGIT_DST[3 * tr + 2]);
                        SDL_BlitSurface(digits[chosen_metre[tr] / 10], NULL, screen, &RECT_DIGIT_DST[3 * tr]);
                    }
                }
            }


            if (occurrence.key.type == SDL_KEYDOWN && occurrence.key.keysym.sym == SDLK_ESCAPE) {
                may_exit = 1;
                current_opt = MENU_NEW_STAVE_CODE;
            }
            if (occurrence.key.type == SDL_KEYDOWN && occurrence.key.keysym.sym == SDLK_RETURN) {
                may_exit = 1;
                current_opt = CREATING_WINDOW_CODE;
            }

            SDL_UpdateWindowSurface(window);
        }
    }

//    SDL_FreeSurface(stav);
//    SDL_FreeSurface(metre_chose);
//    for (i = 0; i < 10; i++) {
//        SDL_FreeSurface(digits[i]);
//    }

    return current_opt;
}
////It returns option code
int menu_window(SDL_Surface *screen, SDL_Window *window) {

    int may_exit = 0;
    int current_menu_opt = 0, prev_menu_opt = -1;
    SDL_Event occurrence;

    SDL_Surface *title = NULL;
    SDL_Surface *creating_option = NULL;
    SDL_Surface *loading_option = NULL;
    SDL_Surface *exit_option = NULL;
    SDL_Surface *creating_option_highlight = NULL;
    SDL_Surface *loading_option_highlight = NULL;
    SDL_Surface *exit_option_highlight = NULL;
    SDL_Surface *menu_background = NULL;
    SDL_Surface *help_option = NULL;
    SDL_Surface *help_highlight_option = NULL;

    menu_background = SDL_LoadBMP("obrazki/menu/menu_background.bmp");
    creating_option = SDL_LoadBMP("obrazki/menu/option_new.bmp");
    loading_option = SDL_LoadBMP("obrazki/menu/option_open.bmp");
    exit_option = SDL_LoadBMP("obrazki/menu/option_exit.bmp");
    creating_option_highlight = SDL_LoadBMP("obrazki/menu/option_new_highlight.bmp");
    loading_option_highlight = SDL_LoadBMP("obrazki/menu/option_open_highlight.bmp");
    exit_option_highlight = SDL_LoadBMP("obrazki/menu/option_exit_highlight.bmp");
    title = SDL_LoadBMP("obrazki/menu/title.bmp");
    help_option = SDL_LoadBMP("obrazki/menu/option_help.bmp");
    help_highlight_option = SDL_LoadBMP("obrazki/menu/option_help_highlight.bmp");

    SDL_SetColorKey(help_option, SDL_TRUE, SDL_MapRGB(help_option->format, 255, 255, 255));
    SDL_SetColorKey(help_highlight_option, SDL_TRUE, SDL_MapRGB(help_highlight_option->format, 255, 255, 255));
    SDL_SetColorKey(title, SDL_TRUE, SDL_MapRGB(title->format, 255, 255, 255));
    SDL_SetColorKey(exit_option_highlight, SDL_TRUE, SDL_MapRGB(exit_option_highlight->format, 255, 255, 255));
    SDL_SetColorKey(exit_option, SDL_TRUE, SDL_MapRGB(exit_option->format, 255, 255, 255));
    SDL_SetColorKey(loading_option_highlight, SDL_TRUE, SDL_MapRGB(loading_option_highlight->format, 255, 255, 255));
    SDL_SetColorKey(loading_option, SDL_TRUE, SDL_MapRGB(loading_option->format, 255, 255, 255));
    SDL_SetColorKey(creating_option_highlight, SDL_TRUE, SDL_MapRGB(creating_option_highlight->format, 255, 255, 255));
    SDL_SetColorKey(creating_option, SDL_TRUE, SDL_MapRGB(creating_option->format, 255, 255, 255));

    SDL_Rect R_load_dst, R_new_dst, R_exit_dst, R_title_dst, R_help_dst;
    R_title_dst.x = X_TO_INSERT_OPTION_TITLE;
    R_title_dst.y = Y_TO_INSERT_OPTION_TITLE;
    R_new_dst.x = X_TO_INSERT_OPTION_NEW;
    R_new_dst.y = Y_TO_INSERT_OPTION_NEW;
    R_load_dst.x = X_TO_INSERT_OPTION_OPEN;
    R_load_dst.y = Y_TO_INSERT_OPTION_OPEN;
    R_exit_dst.x = X_TO_INSERT_OPTION_EXIT;
    R_exit_dst.y = Y_TO_INSERT_OPTION_EXIT;
    R_help_dst.x = X_TO_INSERT_OPTION_HELP;
    R_help_dst.y = Y_TO_INSERT_OPTION_HELP;

    current_menu_opt = MENU_NEW_STAVE_CODE;
    prev_menu_opt = -1;

    SDL_BlitSurface(menu_background, NULL, screen, NULL);
    SDL_BlitSurface(title, NULL, screen, &R_title_dst);
    SDL_BlitSurface(creating_option_highlight, NULL, screen, &R_new_dst);
    SDL_BlitSurface(loading_option, NULL, screen, &R_load_dst);
    SDL_BlitSurface(exit_option, NULL, screen, &R_exit_dst);
    SDL_BlitSurface(help_option, NULL, screen, &R_help_dst);

    SDL_UpdateWindowSurface(window);


    while (may_exit == 0) {
        while (SDL_PollEvent(&occurrence)) {
            if (occurrence.type == SDL_QUIT) {
                may_exit = 1;
                current_menu_opt = 2;
            }


            if (occurrence.key.type != SDL_KEYUP) {
                SDL_Delay(80);
            }
            switch (current_menu_opt) {

                case MENU_NEW_STAVE_CODE:
                    if (prev_menu_opt != current_menu_opt) {
                        SDL_BlitSurface(creating_option_highlight, NULL, screen, &R_new_dst);
                        prev_menu_opt = MENU_NEW_STAVE_CODE;
                    }
                    if (occurrence.key.type == SDL_KEYDOWN && occurrence.key.keysym.sym == SDLK_DOWN) {
                        SDL_BlitSurface(creating_option, NULL, screen, &R_new_dst);
                        current_menu_opt = MENU_LOAD_CODE;
                        prev_menu_opt = MENU_NEW_STAVE_CODE;
                    }
                    break;
                case MENU_LOAD_CODE:
                    if (prev_menu_opt != current_menu_opt) {
                        SDL_BlitSurface(loading_option_highlight, NULL, screen, &R_load_dst);
                        prev_menu_opt = MENU_LOAD_CODE;
                    }
                    if (occurrence.key.type == SDL_KEYDOWN && occurrence.key.keysym.sym == SDLK_UP) {
                        SDL_BlitSurface(loading_option, NULL, screen, &R_load_dst);
                        current_menu_opt = MENU_NEW_STAVE_CODE;
                        prev_menu_opt = MENU_LOAD_CODE;
                    } else if (occurrence.key.type == SDL_KEYDOWN && occurrence.key.keysym.sym == SDLK_DOWN) {
                        SDL_BlitSurface(loading_option, NULL, screen, &R_load_dst);
                        current_menu_opt = MENU_EXIT_CODE;
                        prev_menu_opt = MENU_LOAD_CODE;
                    }
                    break;
                case MENU_EXIT_CODE:
                    if (prev_menu_opt != current_menu_opt) {
                        SDL_BlitSurface(exit_option_highlight, NULL, screen, &R_exit_dst);
                        prev_menu_opt = MENU_EXIT_CODE;
                    }
                    if (occurrence.key.type == SDL_KEYDOWN && occurrence.key.keysym.sym == SDLK_UP) {
                        SDL_BlitSurface(exit_option, NULL, screen, &R_exit_dst);
                        current_menu_opt = MENU_LOAD_CODE;
                        prev_menu_opt = MENU_EXIT_CODE;
                    } else if (occurrence.key.type == SDL_KEYDOWN && occurrence.key.keysym.sym == SDLK_DOWN) {
                        SDL_BlitSurface(exit_option, NULL, screen, &R_exit_dst);
                        current_menu_opt = MENU_HELP_CODE;
                        prev_menu_opt = MENU_EXIT_CODE;
                    }
                    break;
                case MENU_HELP_CODE:
                    if (prev_menu_opt != current_menu_opt) {
                        SDL_BlitSurface(help_highlight_option, NULL, screen, &R_help_dst);
                        prev_menu_opt = MENU_HELP_CODE;
                    }
                    if (occurrence.key.type == SDL_KEYDOWN && occurrence.key.keysym.sym == SDLK_UP) {
                        SDL_BlitSurface(help_option, NULL, screen, &R_help_dst);
                        current_menu_opt = MENU_EXIT_CODE;
                        prev_menu_opt = MENU_HELP_CODE;
                    }
                    break;
                default:
                    break;
            }


            if (prev_menu_opt == current_menu_opt && occurrence.key.keysym.sym == SDLK_RETURN) {
                may_exit = 1;
            }

            SDL_UpdateWindowSurface(window);
            SDL_Delay(20);
        }
    }

    SDL_FreeSurface(menu_background);
    SDL_FreeSurface(title);
    SDL_FreeSurface(creating_option);
    SDL_FreeSurface(loading_option);
    SDL_FreeSurface(exit_option);
    SDL_FreeSurface(creating_option_highlight);
    SDL_FreeSurface(loading_option_highlight);
    SDL_FreeSurface(exit_option_highlight);

    return current_menu_opt;
}
////It returns option code
int fifths_choosing(SDL_Surface *screen, SDL_Window *window, char chosen_key[2]) {

    SDL_Rect RECT_fifths_dots[15];
    RECT_fifths_dots[0].x = 390;
    RECT_fifths_dots[0].y = 4;
    RECT_fifths_dots[1].x = 559;
    RECT_fifths_dots[1].y = 50;
    RECT_fifths_dots[2].x = 680;
    RECT_fifths_dots[2].y = 173;
    RECT_fifths_dots[3].x = 723;
    RECT_fifths_dots[3].y = 338;
    RECT_fifths_dots[4].x = 684;
    RECT_fifths_dots[4].y = 509;
    RECT_fifths_dots[5].x = 564;
    RECT_fifths_dots[5].y = 628;
    RECT_fifths_dots[6].x = 590;
    RECT_fifths_dots[6].y = 722;
    RECT_fifths_dots[7].x = 392;
    RECT_fifths_dots[7].y = 679;
    RECT_fifths_dots[8].x = 407;
    RECT_fifths_dots[8].y = 761;
    RECT_fifths_dots[9].x = 215;
    RECT_fifths_dots[9].y = 632;
    RECT_fifths_dots[10].x = 156;
    RECT_fifths_dots[10].y = 712;
    RECT_fifths_dots[11].x = 98;
    RECT_fifths_dots[11].y = 513;
    RECT_fifths_dots[12].x = 59;
    RECT_fifths_dots[12].y = 340;
    RECT_fifths_dots[13].x = 100;
    RECT_fifths_dots[13].y = 160;
    RECT_fifths_dots[14].x = 224;
    RECT_fifths_dots[14].y = 51;

    int current_fifths_opt = 0, prev_fifths_opt = -1, may_exit = 0, current_opt = 1;
    SDL_Surface *fifths_circle = NULL;
    SDL_Surface *dot = NULL;
    SDL_Surface *anty_dot = NULL;
    SDL_Event occurrence;

    fifths_circle = SDL_LoadBMP("obrazki/fifths_circle/fifths_circle_large.bmp");
    dot = SDL_LoadBMP("obrazki/fifths_circle/dot.bmp");
    anty_dot = SDL_LoadBMP("obrazki/fifths_circle/anty_dot.bmp");
    SDL_SetColorKey(dot, SDL_TRUE, SDL_MapRGB(dot->format, 255, 255, 255));


    SDL_BlitSurface(fifths_circle, NULL, screen, NULL);
    if (occurrence.key.type != SDL_KEYUP) {
        SDL_Delay(50);
    }
    while (may_exit == 0) {
        while (SDL_PollEvent(&occurrence)) {
            if (occurrence.type == SDL_QUIT) {
                may_exit = 1;
                current_opt = MENU_EXIT_CODE;
            }
        }

        if (current_fifths_opt != prev_fifths_opt) {
            SDL_BlitSurface(dot, NULL, screen, &RECT_fifths_dots[current_fifths_opt]);
            prev_fifths_opt = current_fifths_opt;
        }
        if (occurrence.key.type == SDL_KEYDOWN &&
            (occurrence.key.keysym.sym == SDLK_DOWN || occurrence.key.keysym.sym == SDLK_RIGHT)) {
            SDL_BlitSurface(anty_dot, NULL, screen, &RECT_fifths_dots[current_fifths_opt]);
            prev_fifths_opt = current_fifths_opt;
            current_fifths_opt = (current_fifths_opt + 1) % 15;

        } else if (occurrence.key.type == SDL_KEYDOWN &&
                   (occurrence.key.keysym.sym == SDLK_UP || occurrence.key.keysym.sym == SDLK_LEFT)) {
            SDL_BlitSurface(anty_dot, NULL, screen, &RECT_fifths_dots[current_fifths_opt]);
            prev_fifths_opt = 0;
            current_fifths_opt = (current_fifths_opt + 14) % 15;
        }

        if (occurrence.key.type == SDL_KEYDOWN && occurrence.key.keysym.sym == SDLK_RETURN) {
            chosen_key[1] = '\0';
            switch (current_fifths_opt) {

                case 0:
                    chosen_key[0] = 'C';
                    break;
                case 1:
                    chosen_key[0] = 'G';
                    break;
                case 2:
                    chosen_key[0] = 'D';
                    break;
                case 3:
                    chosen_key[0] = 'A';
                    break;
                case 4:
                    chosen_key[0] = 'E';
                    break;
                case 5:
                    chosen_key[0] = 'C';
                    chosen_key[1] = 'e';
                    break;
                case 6:
                    chosen_key[0] = 'H';
                    break;
                case 7:
                    chosen_key[0] = 'G';
                    chosen_key[1] = 'e';
                    break;
                case 8:
                    chosen_key[0] = 'F';
                    chosen_key[1] = 'i';
                    break;
                case 9:
                    chosen_key[0] = 'D';
                    chosen_key[1] = 'e';
                    break;
                case 10:
                    chosen_key[0] = 'C';
                    chosen_key[1] = 'i';
                    break;
                case 11:
                    chosen_key[0] = 'A';
                    chosen_key[1] = 's';
                    break;
                case 12:
                    chosen_key[0] = 'E';
                    chosen_key[1] = 's';
                    break;
                case 13:
                    chosen_key[0] = 'B';
                    break;
                case 14:
                    chosen_key[0] = 'F';
                    break;
                default:
                    break;
            }

            current_opt = METRE_WINDOW_CODE;
            may_exit = 1;

        }

        SDL_UpdateWindowSurface(window);
        if (occurrence.key.type != SDL_KEYUP) {
            SDL_Delay(60);
        }
        if (occurrence.type == SDL_KEYDOWN && occurrence.key.keysym.sym == SDLK_ESCAPE) {
            may_exit = 1;
            current_opt = MENU_WINDOW_CODE;
        }
        SDL_Delay(20);

    }

//    SDL_FreeSurface(fifths_circle);
//    SDL_FreeSurface(dot);
//    SDL_FreeSurface(anty_dot);

    return current_opt;
}
////It uses options codes
int menu_open(char chosen_key[2], int chosen_metre[2]) {

    SDL_Event occurrence;
    int may_exit = 0, current_opt = MENU_WINDOW_CODE;

    SDL_Window *window = NULL;
    SDL_Surface *screen = NULL;

    window = SDL_CreateWindow("MUZOWNIK", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, MENU_SCREEN_WIDTH,
                              SCREEN_HIGHT, SDL_WINDOW_SHOWN);
    screen = SDL_GetWindowSurface(window);

    while (!may_exit) {
        //////////////// Menu
        if (current_opt == MENU_WINDOW_CODE) {
            current_opt = menu_window(screen, window);
        }
        ////// When creating new stave chosen
        if (current_opt == MENU_NEW_STAVE_CODE) {
            current_opt = fifths_choosing(screen, window, chosen_key);

            if (current_opt == METRE_WINDOW_CODE) {
                current_opt = metre_choosing(screen, window, chosen_metre);
                if (current_opt == CREATING_WINDOW_CODE) {
                    may_exit = 1;
                }
            }
        }
        ////// When opening existing stove
        if (current_opt == MENU_LOAD_CODE) {
            may_exit = 1;
        }
        ////// When go exit
        if (current_opt == MENU_EXIT_CODE) {
            may_exit = 1;

            SDL_FreeSurface(screen);
            SDL_DestroyWindow(window);

            SDL_Quit();
            exit(0);
        }
        ////// When need help
        if (current_opt == MENU_HELP_CODE) {
            current_opt = help_window(screen, window);
        }
    }

    SDL_DestroyWindow(window);
    //SDL_FreeSurface(screen);

    return current_opt;
}


//////////////////////Functions for writing new stave:
void swap_notes(NOTES *n1, NOTES *n2) {
    int tmp;
    char c_tmp;
    tmp = n1->height;
    n1->height = n2->height;
    n2->height = tmp;

    c_tmp = n1->acci;
    n1->acci = n2->acci;
    n2->acci = c_tmp;

    c_tmp = n1->name;
    n1->name = n2->name;
    n2->name = c_tmp;

}
int get_note_index(NOTES *n) {

    int index = 0;
    switch (n->name) {
        case 'C':
            index = 0;
            break;
        case 'D':
            index = 2;
            break;
        case 'E':
            index = 4;
            break;
        case 'F':
            index = 5;
            break;
        case 'G':
            index = 7;
            break;
        case 'A':
            index = 9;
            break;
        case 'H':
            index = 11;
            break;
        default:
            break;
    }

    if (n->acci == 's') index = index + 1;
    if (n->acci == 'b') index = index - 1;

    index = index + n->height * 12;

    return index;
}
int compare_notes(NOTES *n1, NOTES *n2) {

    int i1, i2;
    i1 = get_note_index(n1);
    i2 = get_note_index(n2);

    if (i1 > i2) return 1;
    if (i1 == i2) return 0;
    if (i1 < i2) return -1;
}
int sort_uniq_notes(CHORD *chord_to_sort) {
    if (chord_to_sort->notes_number <= 1) return 0;
    int is_sorted = 0, i, opt, j;

    while (!is_sorted) {
        is_sorted = 1;
        for (i = 0; i < chord_to_sort->notes_number - 1; i++) {
            opt = compare_notes(&chord_to_sort->notes_[i], &chord_to_sort->notes_[i + 1]);
            if (opt == 1) {
                swap_notes(&chord_to_sort->notes_[i], &chord_to_sort->notes_[i + 1]);
                is_sorted = 0;
            } else if (opt == 0) {
                for (j = i + 1; j < chord_to_sort->notes_number - 1; j++) {
                    swap_notes(&chord_to_sort->notes_[j], &chord_to_sort->notes_[j + 1]);
                }
                is_sorted = 0;
                chord_to_sort->notes_number--;
            }
        }
    }
    return 0;
}
int get_serial_key(const char *chosen_key, char serial_key[7]) {

    int i;
    for (i = 0; i < 7; i++) {
        serial_key[i] = 'n';
    }

    switch (chosen_key[0]) {
        case 'C':
            switch (chosen_key[1]) {
                case '\0':
                    break;
                case 'i':
                    for (i = 0; i < 7; i++) {
                        serial_key[i] = 's';
                    }
                    break;
                case 'e':
                    for (i = 0; i < 7; i++) {
                        serial_key[i] = 'b';
                    }
                    break;
                default:
                    return 1;
                    break;
            }
            break;
        case 'D':
            switch (chosen_key[1]) {
                case '\0':
                    serial_key[0] = 's';
                    serial_key[3] = 's';
                    break;
                case 'e':
                    for (i = 0; i < 7; i++) {
                        serial_key[i] = 'b';
                    }
                    serial_key[0] = 'n';
                    serial_key[3] = 'n';
                    break;
                default:
                    return 1;
                    break;
            }
            break;
        case 'E':
            switch (chosen_key[1]) {
                case '\0':
                    serial_key[0] = 's';
                    serial_key[3] = 's';
                    serial_key[4] = 's';
                    serial_key[1] = 's';
                    break;
                case 'e':
                    serial_key[2] = 'b';
                    serial_key[5] = 'b';
                    serial_key[6] = 'b';
                    break;
                default:
                    return 1;
                    break;
            }
            break;
        case 'F':
            switch (chosen_key[1]) {
                case '\0':
                    serial_key[6] = 'b';
                    break;
                case 'i':
                    for (i = 0; i < 6; i++) {
                        serial_key[i] = 's';
                    }
                    break;
                default:
                    return 1;
                    break;
            }
            break;
        case 'G':
            switch (chosen_key[1]) {
                case '\0':
                    serial_key[3] = 's';
                    break;
                case 'e':
                    for (i = 0; i < 7; i++) {
                        serial_key[i] = 'b';
                    }
                    serial_key[3] = 'n';
                    break;
                default:
                    return 1;
                    break;
            }
            break;
        case 'A':
            switch (chosen_key[1]) {
                case '\0':
                    serial_key[3] = 's';
                    serial_key[0] = 's';
                    serial_key[4] = 's';
                    break;
                case 's':
                    serial_key[6] = 'b';
                    serial_key[2] = 'b';
                    serial_key[5] = 'b';
                    serial_key[1] = 'b';
                    break;
                default:
                    return 1;
                    break;
            }
            break;
        case 'H':
            for (i = 0; i < 6; i++) {
                serial_key[i] = 's';
            }
            serial_key[2] = 'n';
            break;
        case 'B':
            serial_key[6] = 'b';
            serial_key[2] = 'b';
            break;
        default:
            return 1;
            break;
    }

    return 0;
}
int is_acci_req(CHORD *chord_to_put, int k, const char *defauly_serial_key) {

    int i = 0;
    NOTES *n = &(chord_to_put->notes_[k]);
    switch (n->name) {
        case 'C':
            i = 0;
            break;
        case 'D':
            i = 1;
            break;
        case 'E':
            i = 2;
            break;
        case 'F':
            i = 3;
            break;
        case 'G':
            i = 4;
            break;
        case 'A':
            i = 5;
            break;
        case 'H':
            i = 6;
            break;
        default:
            break;
    }
    if (//n->acci != chord_to_put->local_serial_key[i] ||
        (chord_to_put->prev == NULL && n->acci != defauly_serial_key[i]) ||
        (chord_to_put->prev != NULL && n->acci != chord_to_put->prev->local_serial_key[i])) {
        return 1;
    }

    return 0;
}
int get_space_for_chord(BARS *bar, BARS_SPACE *b_space, const int *metre) {

    int available_width = bar->width_ - DISTANCE_BETWEEN_BAR_AND_FIRST_NOTE - DISTANCE_BEWTWEEN_LAST_NOTE_AND_BAR, i, j;
    double notes_periods[6][6], metre_quatient = (double)metre[0] / (double)metre[1];

    notes_periods[0][0] = 1;
    for (i = 1; i < 6; i++) {
        notes_periods[i][0] = notes_periods[i - 1][0] / 2;
        for (j = 1; j < 6; j++) {
            notes_periods[i][j] = notes_periods[i][0] * (2 - pow(2, -(double) j));
        }
    }


    for (i = 0; i < 6; i++) {
        for (j = 0; j < 6; j++) {
            b_space->widths_ni_[i][j] = (int) ((available_width * notes_periods[i][j]) / metre_quatient);
        }
    }
    return 0;
}

int put_chord_on_treb_without_beam(CHORD *chord_to_put, SDL_Surface *stave, SDL_Surface *blank_stave, int *X_start,
                                   int brace, const char serial_key[7], BARS_SPACE *b_space, struct current_OPUS_edits_ *COE) {

    int k, i, any_put = 0, j;
    int hand = 0;
    SDL_Rect blank_stave_rect;
    SDL_Rect line_rect;
    SDL_Rect insert_note_rect;
    SDL_Rect acci_rect;
    SDL_Rect dots_rect;

    blank_stave_rect.x = *X_start - WIDTH_NOTES_WITH_FLAGS / 2;
    blank_stave_rect.y = Y_FIRST_LINE + hand * DISTANCE_BETWEEN_FIRST_LINE_HANDS_STAVES +
                         brace * DISTANCE_BETWEEN_FIRST_LINE_BRACES - 3 * DISTANCE_BETWEEN_LINES;
    blank_stave_rect.w = WIDTH_NOTES_WITH_FLAGS * 2;
    blank_stave_rect.h = DISTANCE_BETWEEN_LINES * 8;

    line_rect.w = WIDTH_NOTES_WITHOUT_FLAG * 2;
    line_rect.h = 1;
    line_rect.x = *X_start - WIDTH_NOTES_WITHOUT_FLAG / 2;

    insert_note_rect.x = *X_start;
    insert_note_rect.y = 0;

    acci_rect.x = *X_start - DISTANCE_BETWEEN_ACCI_AND_NOTE;

    SDL_Surface *note_without_flag = NULL;
    SDL_Surface *note_with_flag = NULL;
    SDL_Surface *note_to_put = NULL;
    SDL_Surface *sign_note_without_flag = NULL;
    SDL_Surface *sign_note_with_flag = NULL;

    SDL_Surface *acci_b = SDL_LoadBMP("obrazki/accidental/acci_b.bmp");
    SDL_Surface *acci_s = SDL_LoadBMP("obrazki/accidental/acci_s.bmp");
    SDL_Surface *acci_n = SDL_LoadBMP("obrazki/accidental/acci_n.bmp");
    SDL_Surface *acci_to_put = NULL;

    SDL_Surface *time_dot = SDL_LoadBMP("obrazki/notes/time_dot.bmp");

    //SDL_BlitSurface(blank_stave, &blank_stave_rect, stave, &blank_stave_rect);


    char path[28] = "obrazki/notes/note_00.bmp";
    int prefix_len = 19;
    path[prefix_len] = (char) (chord_to_put->time + 48);
    if (chord_to_put->time == 0) {
        path[prefix_len + 1] = 'n';
    } else {
        int lower_note_index = get_note_index(&chord_to_put->notes_[0]);
        int treb_sec_D_index = D_note + 5 * 12;

        if (lower_note_index >= treb_sec_D_index) {
            path[prefix_len + 1] = 'd';
        } else {
            path[prefix_len + 1] = 'u';
        }
    }


    if (chord_to_put->time <= 2) {
        note_without_flag = SDL_LoadBMP(path);
        note_with_flag = SDL_LoadBMP(path);
        if (chord_to_put == COE->current_C){
            path[prefix_len - 2] = 'b';
            sign_note_with_flag = SDL_LoadBMP(path);
            sign_note_without_flag = SDL_LoadBMP(path);
        }
    } else {
        note_with_flag = SDL_LoadBMP(path);
        if (chord_to_put == COE->current_C){
            path[prefix_len - 2] = 'b';
            sign_note_with_flag = SDL_LoadBMP(path);
            path[prefix_len - 2] = 'e';
        }
        path[prefix_len] = '2';
        note_without_flag = SDL_LoadBMP(path);
        if (chord_to_put == COE->current_C){
            path[prefix_len - 2] = 'b';
            sign_note_without_flag = SDL_LoadBMP(path);
        }
    }
    SDL_SetColorKey(acci_b, SDL_TRUE, SDL_MapRGB(acci_b->format, 255, 255, 255));
    SDL_SetColorKey(acci_n, SDL_TRUE, SDL_MapRGB(acci_n->format, 255, 255, 255));
    SDL_SetColorKey(acci_s, SDL_TRUE, SDL_MapRGB(acci_s->format, 255, 255, 255));
    SDL_SetColorKey(note_with_flag, SDL_TRUE, SDL_MapRGB(note_with_flag->format, 255, 255, 255));
    SDL_SetColorKey(note_without_flag, SDL_TRUE, SDL_MapRGB(note_without_flag->format, 255, 255, 255));
    if (chord_to_put == COE->current_C) {
        SDL_SetColorKey(sign_note_with_flag, SDL_TRUE, SDL_MapRGB(sign_note_with_flag->format, 255, 255, 255));
        SDL_SetColorKey(sign_note_without_flag, SDL_TRUE, SDL_MapRGB(sign_note_without_flag->format, 255, 255, 255));
    }
    SDL_SetColorKey(time_dot, SDL_TRUE, SDL_MapRGB(time_dot->format, 255, 255, 255));


    for (i = 0; i < chord_to_put->notes_number; i++) {

        if (path[prefix_len + 1] == 'u' || path[prefix_len + 1] == 'n') k = i;
        else k = chord_to_put->notes_number - 1 - i;
        note_to_put = note_without_flag;
        if (i == chord_to_put->notes_number - 1) note_to_put = note_with_flag;

        if (chord_to_put == COE->current_C && i == COE->current_note_index){
            note_to_put = sign_note_without_flag;
            if (i == chord_to_put->notes_number - 1) note_to_put = sign_note_with_flag;

        }


        switch (chord_to_put->notes_[k].name) {
            case 'C':
                if (chord_to_put->notes_[k].height == 4) {
                    insert_note_rect.y =
                            Y_FIRST_LINE + DISTANCE_BETWEEN_FIRST_LINE_BRACES * brace + DISTANCE_BETWEEN_LINES * 5 -
                            Y_OF_ON_LINE_NOTES;
                    line_rect.y =
                            Y_FIRST_LINE + DISTANCE_BETWEEN_FIRST_LINE_BRACES * brace + DISTANCE_BETWEEN_LINES * 5;
                    SDL_FillRect(stave, &line_rect, 0x00);
                } else if (chord_to_put->notes_[k].height == 5) {
                    insert_note_rect.y =
                            Y_FIRST_LINE + DISTANCE_BETWEEN_FIRST_LINE_BRACES * brace + DISTANCE_BETWEEN_LINES * 1 -
                            Y_OF_BETWEEN_LINE_NOTES;
                } else if (chord_to_put->notes_[k].height == 6) {
                    insert_note_rect.y =
                            Y_FIRST_LINE + DISTANCE_BETWEEN_FIRST_LINE_BRACES * brace - DISTANCE_BETWEEN_LINES * 2 -
                            Y_OF_ON_LINE_NOTES;
                    line_rect.y =
                            Y_FIRST_LINE + DISTANCE_BETWEEN_FIRST_LINE_BRACES * brace - DISTANCE_BETWEEN_LINES * 1;
                    SDL_FillRect(stave, &line_rect, 0x00);
                    line_rect.y =
                            Y_FIRST_LINE + DISTANCE_BETWEEN_FIRST_LINE_BRACES * brace - DISTANCE_BETWEEN_LINES * 2;
                    SDL_FillRect(stave, &line_rect, 0x00);
                }
                break;
            case 'D':
                if (chord_to_put->notes_[k].height == 4) {
                    insert_note_rect.y =
                            Y_FIRST_LINE + DISTANCE_BETWEEN_FIRST_LINE_BRACES * brace + DISTANCE_BETWEEN_LINES * 4 -
                            Y_OF_BETWEEN_LINE_NOTES;
                } else if (chord_to_put->notes_[k].height == 5) {
                    insert_note_rect.y =
                            Y_FIRST_LINE + DISTANCE_BETWEEN_FIRST_LINE_BRACES * brace + DISTANCE_BETWEEN_LINES -
                            Y_OF_ON_LINE_NOTES;
                } else if (chord_to_put->notes_[k].height == 6) {
                    insert_note_rect.y =
                            Y_FIRST_LINE + DISTANCE_BETWEEN_FIRST_LINE_BRACES * brace - DISTANCE_BETWEEN_LINES * 3 -
                            Y_OF_BETWEEN_LINE_NOTES;
                    line_rect.y =
                            Y_FIRST_LINE + DISTANCE_BETWEEN_FIRST_LINE_BRACES * brace - DISTANCE_BETWEEN_LINES * 1;
                    SDL_FillRect(stave, &line_rect, 0x00);
                    line_rect.y =
                            Y_FIRST_LINE + DISTANCE_BETWEEN_FIRST_LINE_BRACES * brace - DISTANCE_BETWEEN_LINES * 2;
                    SDL_FillRect(stave, &line_rect, 0x00);
                }
                break;
            case 'E':
                if (chord_to_put->notes_[k].height == 4) {
                    insert_note_rect.y =
                            Y_FIRST_LINE + DISTANCE_BETWEEN_FIRST_LINE_BRACES * brace + DISTANCE_BETWEEN_LINES * 4 -
                            Y_OF_ON_LINE_NOTES;
                } else if (chord_to_put->notes_[k].height == 5) {
                    insert_note_rect.y =
                            Y_FIRST_LINE + DISTANCE_BETWEEN_FIRST_LINE_BRACES * brace -
                            Y_OF_BETWEEN_LINE_NOTES;
                } else if (chord_to_put->notes_[k].height == 6) {
                    insert_note_rect.y =
                            Y_FIRST_LINE + DISTANCE_BETWEEN_FIRST_LINE_BRACES * brace - DISTANCE_BETWEEN_LINES * 3 -
                            Y_OF_ON_LINE_NOTES;
                    SDL_BlitSurface(note_to_put, NULL, stave, &insert_note_rect);
                    line_rect.y =
                            Y_FIRST_LINE + DISTANCE_BETWEEN_FIRST_LINE_BRACES * brace - DISTANCE_BETWEEN_LINES * 1;
                    SDL_FillRect(stave, &line_rect, 0x00);
                    line_rect.y =
                            Y_FIRST_LINE + DISTANCE_BETWEEN_FIRST_LINE_BRACES * brace - DISTANCE_BETWEEN_LINES * 2;
                    SDL_FillRect(stave, &line_rect, 0x00);
                    line_rect.y =
                            Y_FIRST_LINE + DISTANCE_BETWEEN_FIRST_LINE_BRACES * brace - DISTANCE_BETWEEN_LINES * 3;
                    SDL_FillRect(stave, &line_rect, 0x00);
                }
                break;
            case 'F':
                if (chord_to_put->notes_[k].height == 4) {
                    insert_note_rect.y =
                            Y_FIRST_LINE + DISTANCE_BETWEEN_FIRST_LINE_BRACES * brace + DISTANCE_BETWEEN_LINES * 3 -
                            Y_OF_BETWEEN_LINE_NOTES;
                } else if (chord_to_put->notes_[k].height == 5) {
                    insert_note_rect.y =
                            Y_FIRST_LINE + DISTANCE_BETWEEN_FIRST_LINE_BRACES * brace -
                            Y_OF_ON_LINE_NOTES;
                } else if (chord_to_put->notes_[k].height == 6) {
                    insert_note_rect.y =
                            Y_FIRST_LINE + DISTANCE_BETWEEN_FIRST_LINE_BRACES * brace - DISTANCE_BETWEEN_LINES * 4 -
                            Y_OF_BETWEEN_LINE_NOTES;
                    SDL_BlitSurface(note_to_put, NULL, stave, &insert_note_rect);
                    line_rect.y =
                            Y_FIRST_LINE + DISTANCE_BETWEEN_FIRST_LINE_BRACES * brace - DISTANCE_BETWEEN_LINES * 1;
                    SDL_FillRect(stave, &line_rect, 0x00);
                    line_rect.y =
                            Y_FIRST_LINE + DISTANCE_BETWEEN_FIRST_LINE_BRACES * brace - DISTANCE_BETWEEN_LINES * 2;
                    SDL_FillRect(stave, &line_rect, 0x00);
                    line_rect.y =
                            Y_FIRST_LINE + DISTANCE_BETWEEN_FIRST_LINE_BRACES * brace - DISTANCE_BETWEEN_LINES * 3;
                    SDL_FillRect(stave, &line_rect, 0x00);
                }
                break;
            case 'G':
                if (chord_to_put->notes_[k].height == 3) {
                    insert_note_rect.y =
                            Y_FIRST_LINE + DISTANCE_BETWEEN_FIRST_LINE_BRACES * brace + DISTANCE_BETWEEN_LINES * 6 -
                            Y_OF_BETWEEN_LINE_NOTES;
                    line_rect.y =
                            Y_FIRST_LINE + DISTANCE_BETWEEN_FIRST_LINE_BRACES * brace + DISTANCE_BETWEEN_LINES * 5;
                    SDL_FillRect(stave, &line_rect, 0x00);
                    line_rect.y =
                            Y_FIRST_LINE + DISTANCE_BETWEEN_FIRST_LINE_BRACES * brace + DISTANCE_BETWEEN_LINES * 6;
                    SDL_FillRect(stave, &line_rect, 0x00);
                } else if (chord_to_put->notes_[k].height == 4) {
                    insert_note_rect.y =
                            Y_FIRST_LINE + DISTANCE_BETWEEN_FIRST_LINE_BRACES * brace + DISTANCE_BETWEEN_LINES * 3 -
                            Y_OF_ON_LINE_NOTES;
                } else if (chord_to_put->notes_[k].height == 5) {
                    insert_note_rect.y =
                            Y_FIRST_LINE + DISTANCE_BETWEEN_FIRST_LINE_BRACES * brace - DISTANCE_BETWEEN_LINES -
                            Y_OF_BETWEEN_LINE_NOTES;
                }
/////                    if (chord_to_put->notes_[k].height == 6)...
                break;
            case 'A':
                if (chord_to_put->notes_[k].height == 3) {
                    insert_note_rect.y =
                            Y_FIRST_LINE + DISTANCE_BETWEEN_FIRST_LINE_BRACES * brace + DISTANCE_BETWEEN_LINES * 6 -
                            Y_OF_ON_LINE_NOTES;
                    line_rect.y =
                            Y_FIRST_LINE + DISTANCE_BETWEEN_FIRST_LINE_BRACES * brace + DISTANCE_BETWEEN_LINES * 5;
                    SDL_FillRect(stave, &line_rect, 0x00);
                    line_rect.y =
                            Y_FIRST_LINE + DISTANCE_BETWEEN_FIRST_LINE_BRACES * brace + DISTANCE_BETWEEN_LINES * 6;
                    SDL_FillRect(stave, &line_rect, 0x00);
                } else if (chord_to_put->notes_[k].height == 4) {
                    insert_note_rect.y =
                            Y_FIRST_LINE + DISTANCE_BETWEEN_FIRST_LINE_BRACES * brace + DISTANCE_BETWEEN_LINES * 2 -
                            Y_OF_BETWEEN_LINE_NOTES;

                } else if (chord_to_put->notes_[k].height == 5) {
                    insert_note_rect.y =
                            Y_FIRST_LINE + DISTANCE_BETWEEN_FIRST_LINE_BRACES * brace - DISTANCE_BETWEEN_LINES -
                            Y_OF_ON_LINE_NOTES;
                    line_rect.y =
                            Y_FIRST_LINE + DISTANCE_BETWEEN_FIRST_LINE_BRACES * brace - DISTANCE_BETWEEN_LINES * 1;
                    SDL_FillRect(stave, &line_rect, 0x00);
                }
/////                    if (chord_to_put->notes_[k].height == 6) {

                break;
            case 'H':
                if (chord_to_put->notes_[k].height == 3) {
                    insert_note_rect.y =
                            Y_FIRST_LINE + DISTANCE_BETWEEN_FIRST_LINE_BRACES * brace + DISTANCE_BETWEEN_LINES * 5 -
                            Y_OF_BETWEEN_LINE_NOTES;
                    line_rect.y =
                            Y_FIRST_LINE + DISTANCE_BETWEEN_FIRST_LINE_BRACES * brace + DISTANCE_BETWEEN_LINES * 5;
                    SDL_FillRect(stave, &line_rect, 0x00);
                } else if (chord_to_put->notes_[k].height == 4) {
                    insert_note_rect.y =
                            Y_FIRST_LINE + DISTANCE_BETWEEN_FIRST_LINE_BRACES * brace + DISTANCE_BETWEEN_LINES * 2 -
                            Y_OF_ON_LINE_NOTES;
                } else if (chord_to_put->notes_[k].height == 5) {
                    insert_note_rect.y =
                            Y_FIRST_LINE + DISTANCE_BETWEEN_FIRST_LINE_BRACES * brace - DISTANCE_BETWEEN_LINES * 2 -
                            Y_OF_BETWEEN_LINE_NOTES;
                    line_rect.y =
                            Y_FIRST_LINE + DISTANCE_BETWEEN_FIRST_LINE_BRACES * brace - DISTANCE_BETWEEN_LINES * 1;
                    SDL_FillRect(stave, &line_rect, 0x00);
                }
/////                    if (chord_to_put->notes_[k].height == 6)...
                break;
            default:
                break;
        }
        ///// Kropki przedłużające nute
        if (chord_to_put->_time_dots_){
            dots_rect.y = insert_note_rect.y;
            dots_rect.x = insert_note_rect.x + WIDTH_NOTES_WITHOUT_FLAG;
            for (j = 0; j < chord_to_put->_time_dots_; j++){
                SDL_BlitSurface(time_dot, NULL, stave, &dots_rect);
                dots_rect.x += time_dot->w;
            }
        }
        ///// Znaki artykulacji
        if (chord_to_put->ssp_articulation) {

            if (chord_to_put->ssp_articulation == 1) {
                if (path[prefix_len + 1] == 'u' && k == 0) {
                    dots_rect.x = insert_note_rect.x + WIDTH_NOTES_WITHOUT_FLAG / 3;
                    dots_rect.y = insert_note_rect.y + 15;
                    SDL_BlitSurface(time_dot, NULL, stave, &dots_rect);
                }  else if (path[prefix_len + 1] == 'd' && k == chord_to_put->notes_number - 1) {
                    dots_rect.x = insert_note_rect.x + WIDTH_NOTES_WITHOUT_FLAG / 3;
                    dots_rect.y = insert_note_rect.y - 15;
                    SDL_BlitSurface(time_dot, NULL, stave, &dots_rect);
                }
            }
        }

        if (insert_note_rect.y != 0) {
            SDL_BlitSurface(note_to_put, NULL, stave, &insert_note_rect);
            any_put = 1;
            if (is_acci_req(chord_to_put, k, COE->current_O->default_serial_key) || chord_to_put->notes_[k].may_print_acci) {
                acci_rect.y = insert_note_rect.y + Y_OF_ACCI_S_ON_LINE + 10;
                switch (chord_to_put->notes_[i].acci) {
                    case 's':
                        acci_to_put = acci_s;
                        break;
                    case 'b':
                        acci_to_put = acci_b;
                        break;
                    case 'n':
                        acci_to_put = acci_n;
                        break;
                    default:
                        break;
                }

                if (chord_to_put->notes_[i].acci == 'b') {
                    acci_rect.y -= DISTANCE_BETWEEN_LINES / 2;
                }

                SDL_BlitSurface(acci_to_put, NULL, stave, &acci_rect);
            }
        }
    }
    if (!any_put) {
        insert_note_rect.y = Y_FIRST_LINE + brace * DISTANCE_BETWEEN_FIRST_LINE_BRACES + hand * DISTANCE_BETWEEN_FIRST_LINE_HANDS_STAVES;
        insert_note_rect.h = 4 * DISTANCE_BETWEEN_LINES;
        insert_note_rect.w = WIDTH_NOTES_WITHOUT_FLAG;
        if (chord_to_put == COE->current_C){
            SDL_FillRect(stave, &insert_note_rect, 0x0f);

        } else {
            SDL_FillRect(stave, &insert_note_rect, 0x05);
        }
    }

    chord_to_put->X_position = *X_start;
    *X_start += b_space->widths_ni_[chord_to_put->time][chord_to_put->_time_dots_];



//    SDL_FreeSurface(note_without_flag);
//    SDL_FreeSurface(note_with_flag);
//    SDL_FreeSurface(note_to_put);
//    SDL_FreeSurface(acci_b);
//    SDL_FreeSurface(acci_s);
//    SDL_FreeSurface(acci_n);
//    SDL_FreeSurface(acci_to_put);



    return 0;
}
int put_chord_on_bass_without_beam(CHORD *chord_to_put, SDL_Surface *stave, SDL_Surface *blank_stave, int *X_start,
                                   int brace, const char serial_key[7], BARS_SPACE *b_space, struct current_OPUS_edits_ *COE) {

    int k, i, any_put = 0, j;
    int hand = 1;
    SDL_Rect blank_stave_rect;
    SDL_Rect line_rect;
    SDL_Rect insert_note_rect;
    SDL_Rect acci_rect;
    SDL_Rect dots_rect;

    blank_stave_rect.x = *X_start - WIDTH_NOTES_WITH_FLAGS / 2;
    blank_stave_rect.y = Y_FIRST_LINE + hand * DISTANCE_BETWEEN_FIRST_LINE_HANDS_STAVES +
                         brace * DISTANCE_BETWEEN_FIRST_LINE_BRACES - 3 * DISTANCE_BETWEEN_LINES;
    blank_stave_rect.w = WIDTH_NOTES_WITH_FLAGS * 2;
    blank_stave_rect.h = DISTANCE_BETWEEN_LINES * 8;

    line_rect.w = WIDTH_NOTES_WITHOUT_FLAG * 2;
    line_rect.h = 1;
    line_rect.x = *X_start - WIDTH_NOTES_WITHOUT_FLAG / 2;

    insert_note_rect.x = *X_start;
    insert_note_rect.y = 0;

    acci_rect.x = *X_start - DISTANCE_BETWEEN_ACCI_AND_NOTE;

    SDL_Surface *note_without_flag = NULL;
    SDL_Surface *note_with_flag = NULL;
    SDL_Surface *note_to_put = NULL;
    SDL_Surface *sign_note_without_flag = NULL;
    SDL_Surface *sign_note_with_flag = NULL;

    SDL_Surface *acci_b = SDL_LoadBMP("obrazki/accidental/acci_b.bmp");
    SDL_Surface *acci_s = SDL_LoadBMP("obrazki/accidental/acci_s.bmp");
    SDL_Surface *acci_n = SDL_LoadBMP("obrazki/accidental/acci_n.bmp");
    SDL_Surface *acci_to_put = NULL;

    SDL_Surface *time_dot = SDL_LoadBMP("obrazki/notes/time_dot.bmp");

    //SDL_BlitSurface(blank_stave, &blank_stave_rect, stave, &blank_stave_rect);

    char path[28] = "obrazki/notes/note_00.bmp";
    int prefix_len = 19;
    path[prefix_len] = (char) (chord_to_put->time + 48);
    if (chord_to_put->time == 0) {
        path[prefix_len + 1] = 'n';
    } else {
        int lower_note_index = get_note_index(&chord_to_put->notes_[0]);
        int treb_sec_D_index = H_note + 3 * 12;

        if (lower_note_index >= treb_sec_D_index) {
            path[prefix_len + 1] = 'd';
        } else {
            path[prefix_len + 1] = 'u';
        }
    }


    if (chord_to_put->time <= 2) {
        note_without_flag = SDL_LoadBMP(path);
        note_with_flag = SDL_LoadBMP(path);
        if (chord_to_put == COE->current_C) {
            path[prefix_len - 2] = 'b';
            sign_note_with_flag = SDL_LoadBMP(path);
            sign_note_without_flag = SDL_LoadBMP(path);
        }
    } else {
        note_with_flag = SDL_LoadBMP(path);
        if (chord_to_put == COE->current_C) {
            path[prefix_len - 2] = 'b';
            sign_note_with_flag = SDL_LoadBMP(path);
            path[prefix_len - 2] = 'e';
        }
        path[prefix_len] = '2';
        note_without_flag = SDL_LoadBMP(path);
        if (chord_to_put == COE->current_C) {
            path[prefix_len - 2] = 'b';
            sign_note_without_flag = SDL_LoadBMP(path);
        }
    }

    SDL_SetColorKey(acci_b, SDL_TRUE, SDL_MapRGB(acci_b->format, 255, 255, 255));
    SDL_SetColorKey(acci_n, SDL_TRUE, SDL_MapRGB(acci_n->format, 255, 255, 255));
    SDL_SetColorKey(acci_s, SDL_TRUE, SDL_MapRGB(acci_s->format, 255, 255, 255));
    SDL_SetColorKey(note_with_flag, SDL_TRUE, SDL_MapRGB(note_with_flag->format, 255, 255, 255));
    SDL_SetColorKey(note_without_flag, SDL_TRUE, SDL_MapRGB(note_without_flag->format, 255, 255, 255));
    if (chord_to_put == COE->current_C) {
        SDL_SetColorKey(sign_note_with_flag, SDL_TRUE, SDL_MapRGB(sign_note_with_flag->format, 255, 255, 255));
        SDL_SetColorKey(sign_note_without_flag, SDL_TRUE, SDL_MapRGB(sign_note_without_flag->format, 255, 255, 255));
    }
    SDL_SetColorKey(time_dot, SDL_TRUE, SDL_MapRGB(time_dot->format, 255, 255, 255));



    for (i = 0; i < chord_to_put->notes_number; i++) {
        if (path[prefix_len + 1] == 'u' || path[prefix_len + 1] == 'n') k = i;
        else k = chord_to_put->notes_number - 1 - i;
        note_to_put = note_without_flag;
        if (i == chord_to_put->notes_number - 1) note_to_put = note_with_flag;

        if (chord_to_put == COE->current_C && i == COE->current_note_index) {
            note_to_put = sign_note_without_flag;
            if (i == chord_to_put->notes_number - 1) note_to_put = sign_note_with_flag;

        }


        switch (chord_to_put->notes_[k].name) {
            case 'C':
                if (chord_to_put->notes_[k].height == 2) {
                    insert_note_rect.y =
                            Y_FIRST_LINE + DISTANCE_BETWEEN_FIRST_LINE_HANDS_STAVES +
                            DISTANCE_BETWEEN_FIRST_LINE_BRACES * brace + DISTANCE_BETWEEN_LINES * 6 -
                            Y_OF_ON_LINE_NOTES;
                    line_rect.y =
                            Y_FIRST_LINE + DISTANCE_BETWEEN_FIRST_LINE_HANDS_STAVES +
                            DISTANCE_BETWEEN_FIRST_LINE_BRACES * brace + DISTANCE_BETWEEN_LINES * 6;
                    SDL_FillRect(stave, &line_rect, 0x00);
                    line_rect.y =
                            Y_FIRST_LINE + DISTANCE_BETWEEN_FIRST_LINE_HANDS_STAVES +
                            DISTANCE_BETWEEN_FIRST_LINE_BRACES * brace + DISTANCE_BETWEEN_LINES * 5;
                    SDL_FillRect(stave, &line_rect, 0x00);

                } else if (chord_to_put->notes_[k].height == 3) {
                    insert_note_rect.y =
                            Y_FIRST_LINE + DISTANCE_BETWEEN_FIRST_LINE_HANDS_STAVES +
                            DISTANCE_BETWEEN_FIRST_LINE_BRACES * brace + DISTANCE_BETWEEN_LINES * 2 -
                            Y_OF_BETWEEN_LINE_NOTES;
                } else if (chord_to_put->notes_[k].height == 4) {
                    insert_note_rect.y =
                            Y_FIRST_LINE + DISTANCE_BETWEEN_FIRST_LINE_HANDS_STAVES +
                            DISTANCE_BETWEEN_FIRST_LINE_BRACES * brace - DISTANCE_BETWEEN_LINES -
                            Y_OF_ON_LINE_NOTES;
                    line_rect.y =
                            Y_FIRST_LINE + DISTANCE_BETWEEN_FIRST_LINE_HANDS_STAVES +
                            DISTANCE_BETWEEN_FIRST_LINE_BRACES * brace - DISTANCE_BETWEEN_LINES;
                    SDL_FillRect(stave, &line_rect, 0x00);
                }
                break;
            case 'D':
                if (chord_to_put->notes_[k].height == 2) {
                    insert_note_rect.y =
                            Y_FIRST_LINE + DISTANCE_BETWEEN_FIRST_LINE_HANDS_STAVES +
                            DISTANCE_BETWEEN_FIRST_LINE_BRACES * brace + DISTANCE_BETWEEN_LINES * 5 -
                            Y_OF_BETWEEN_LINE_NOTES;
                    line_rect.y =
                            Y_FIRST_LINE + DISTANCE_BETWEEN_FIRST_LINE_HANDS_STAVES +
                            DISTANCE_BETWEEN_FIRST_LINE_BRACES * brace + DISTANCE_BETWEEN_LINES * 5;
                    SDL_FillRect(stave, &line_rect, 0x00);

                } else if (chord_to_put->notes_[k].height == 3) {
                    insert_note_rect.y =
                            Y_FIRST_LINE + DISTANCE_BETWEEN_FIRST_LINE_HANDS_STAVES +
                            DISTANCE_BETWEEN_FIRST_LINE_BRACES * brace + DISTANCE_BETWEEN_LINES * 2 -
                            Y_OF_ON_LINE_NOTES;
                } else if (chord_to_put->notes_[k].height == 4) {
                    insert_note_rect.y =
                            Y_FIRST_LINE + DISTANCE_BETWEEN_FIRST_LINE_HANDS_STAVES +
                            DISTANCE_BETWEEN_FIRST_LINE_BRACES * brace - DISTANCE_BETWEEN_LINES * 2 -
                            Y_OF_BETWEEN_LINE_NOTES;
                    line_rect.y =
                            Y_FIRST_LINE + DISTANCE_BETWEEN_FIRST_LINE_HANDS_STAVES +
                            DISTANCE_BETWEEN_FIRST_LINE_BRACES * brace - DISTANCE_BETWEEN_LINES;
                    SDL_FillRect(stave, &line_rect, 0x00);
                }
                break;
            case 'E':
                if (chord_to_put->notes_[k].height == 2) {
                    insert_note_rect.y =
                            Y_FIRST_LINE + DISTANCE_BETWEEN_FIRST_LINE_HANDS_STAVES +
                            DISTANCE_BETWEEN_FIRST_LINE_BRACES * brace + DISTANCE_BETWEEN_LINES * 5 -
                            Y_OF_ON_LINE_NOTES;
                    line_rect.y =
                            Y_FIRST_LINE + DISTANCE_BETWEEN_FIRST_LINE_HANDS_STAVES +
                            DISTANCE_BETWEEN_FIRST_LINE_BRACES * brace + DISTANCE_BETWEEN_LINES * 5;
                    SDL_FillRect(stave, &line_rect, 0x00);

                } else if (chord_to_put->notes_[k].height == 3) {
                    insert_note_rect.y =
                            Y_FIRST_LINE + DISTANCE_BETWEEN_FIRST_LINE_HANDS_STAVES +
                            DISTANCE_BETWEEN_FIRST_LINE_BRACES * brace + DISTANCE_BETWEEN_LINES * 1 -
                            Y_OF_BETWEEN_LINE_NOTES;
                } else if (chord_to_put->notes_[k].height == 4) {
                    insert_note_rect.y =
                            Y_FIRST_LINE + DISTANCE_BETWEEN_FIRST_LINE_HANDS_STAVES +
                            DISTANCE_BETWEEN_FIRST_LINE_BRACES * brace - DISTANCE_BETWEEN_LINES * 2 -
                            Y_OF_ON_LINE_NOTES;
                    line_rect.y =
                            Y_FIRST_LINE + DISTANCE_BETWEEN_FIRST_LINE_HANDS_STAVES +
                            DISTANCE_BETWEEN_FIRST_LINE_BRACES * brace - DISTANCE_BETWEEN_LINES;
                    SDL_FillRect(stave, &line_rect, 0x00);
                    line_rect.y =
                            Y_FIRST_LINE + DISTANCE_BETWEEN_FIRST_LINE_HANDS_STAVES +
                            DISTANCE_BETWEEN_FIRST_LINE_BRACES * brace - DISTANCE_BETWEEN_LINES * 2;
                    SDL_FillRect(stave, &line_rect, 0x00);
                }
                break;
            case 'F':
                if (chord_to_put->notes_[k].height == 1) {
                    insert_note_rect.y =
                            Y_FIRST_LINE + DISTANCE_BETWEEN_FIRST_LINE_HANDS_STAVES +
                            DISTANCE_BETWEEN_FIRST_LINE_BRACES * brace + DISTANCE_BETWEEN_LINES * 8 -
                            Y_OF_ON_LINE_NOTES;
                    line_rect.y =
                            Y_FIRST_LINE + DISTANCE_BETWEEN_FIRST_LINE_HANDS_STAVES +
                            DISTANCE_BETWEEN_FIRST_LINE_BRACES * brace + DISTANCE_BETWEEN_LINES * 8;
                    SDL_FillRect(stave, &line_rect, 0x00);
                    line_rect.y =
                            Y_FIRST_LINE + DISTANCE_BETWEEN_FIRST_LINE_HANDS_STAVES +
                            DISTANCE_BETWEEN_FIRST_LINE_BRACES * brace + DISTANCE_BETWEEN_LINES * 7;
                    SDL_FillRect(stave, &line_rect, 0x00);
                    line_rect.y =
                            Y_FIRST_LINE + DISTANCE_BETWEEN_FIRST_LINE_HANDS_STAVES +
                            DISTANCE_BETWEEN_FIRST_LINE_BRACES * brace + DISTANCE_BETWEEN_LINES * 6;
                    SDL_FillRect(stave, &line_rect, 0x00);
                    line_rect.y =
                            Y_FIRST_LINE + DISTANCE_BETWEEN_FIRST_LINE_HANDS_STAVES +
                            DISTANCE_BETWEEN_FIRST_LINE_BRACES * brace + DISTANCE_BETWEEN_LINES * 5;
                    SDL_FillRect(stave, &line_rect, 0x00);

                } else if (chord_to_put->notes_[k].height == 2) {
                    insert_note_rect.y =
                            Y_FIRST_LINE + DISTANCE_BETWEEN_FIRST_LINE_HANDS_STAVES +
                            DISTANCE_BETWEEN_FIRST_LINE_BRACES * brace + DISTANCE_BETWEEN_LINES * 4 -
                            Y_OF_BETWEEN_LINE_NOTES;
                } else if (chord_to_put->notes_[k].height == 3) {
                    insert_note_rect.y =
                            Y_FIRST_LINE + DISTANCE_BETWEEN_FIRST_LINE_HANDS_STAVES +
                            DISTANCE_BETWEEN_FIRST_LINE_BRACES * brace + DISTANCE_BETWEEN_LINES * 1 -
                            Y_OF_ON_LINE_NOTES;
                } else if (chord_to_put->notes_[k].height == 4) {
                    insert_note_rect.y =
                            Y_FIRST_LINE + DISTANCE_BETWEEN_FIRST_LINE_HANDS_STAVES +
                            DISTANCE_BETWEEN_FIRST_LINE_BRACES * brace - DISTANCE_BETWEEN_LINES * 3 -
                            Y_OF_ON_LINE_NOTES;
                    line_rect.y =
                            Y_FIRST_LINE + DISTANCE_BETWEEN_FIRST_LINE_HANDS_STAVES +
                            DISTANCE_BETWEEN_FIRST_LINE_BRACES * brace - DISTANCE_BETWEEN_LINES;
                    SDL_FillRect(stave, &line_rect, 0x00);
                    line_rect.y =
                            Y_FIRST_LINE + DISTANCE_BETWEEN_FIRST_LINE_HANDS_STAVES +
                            DISTANCE_BETWEEN_FIRST_LINE_BRACES * brace - DISTANCE_BETWEEN_LINES * 2;
                    SDL_FillRect(stave, &line_rect, 0x00);
                }
                break;
            case 'G':
                if (chord_to_put->notes_[k].height == 1) {
                    insert_note_rect.y =
                            Y_FIRST_LINE + DISTANCE_BETWEEN_FIRST_LINE_HANDS_STAVES +
                            DISTANCE_BETWEEN_FIRST_LINE_BRACES * brace + DISTANCE_BETWEEN_LINES * 7 -
                            Y_OF_BETWEEN_LINE_NOTES;
                    line_rect.y =
                            Y_FIRST_LINE + DISTANCE_BETWEEN_FIRST_LINE_HANDS_STAVES +
                            DISTANCE_BETWEEN_FIRST_LINE_BRACES * brace + DISTANCE_BETWEEN_LINES * 7;
                    SDL_FillRect(stave, &line_rect, 0x00);
                    line_rect.y =
                            Y_FIRST_LINE + DISTANCE_BETWEEN_FIRST_LINE_HANDS_STAVES +
                            DISTANCE_BETWEEN_FIRST_LINE_BRACES * brace + DISTANCE_BETWEEN_LINES * 6;
                    SDL_FillRect(stave, &line_rect, 0x00);
                    line_rect.y =
                            Y_FIRST_LINE + DISTANCE_BETWEEN_FIRST_LINE_HANDS_STAVES +
                            DISTANCE_BETWEEN_FIRST_LINE_BRACES * brace + DISTANCE_BETWEEN_LINES * 5;
                    SDL_FillRect(stave, &line_rect, 0x00);
                } else if (chord_to_put->notes_[k].height == 2) {
                    insert_note_rect.y =
                            Y_FIRST_LINE + DISTANCE_BETWEEN_FIRST_LINE_HANDS_STAVES +
                            DISTANCE_BETWEEN_FIRST_LINE_BRACES * brace + DISTANCE_BETWEEN_LINES * 4 -
                            Y_OF_ON_LINE_NOTES;
                } else if (chord_to_put->notes_[k].height == 3) {
                    insert_note_rect.y =
                            Y_FIRST_LINE + DISTANCE_BETWEEN_FIRST_LINE_HANDS_STAVES +
                            DISTANCE_BETWEEN_FIRST_LINE_BRACES * brace -
                            Y_OF_BETWEEN_LINE_NOTES;
                } else if (chord_to_put->notes_[k].height == 4) {
                    insert_note_rect.y =
                            Y_FIRST_LINE + DISTANCE_BETWEEN_FIRST_LINE_HANDS_STAVES +
                            DISTANCE_BETWEEN_FIRST_LINE_BRACES * brace - DISTANCE_BETWEEN_LINES * 4 -
                            Y_OF_BETWEEN_LINE_NOTES;
                    line_rect.y =
                            Y_FIRST_LINE + DISTANCE_BETWEEN_FIRST_LINE_HANDS_STAVES +
                            DISTANCE_BETWEEN_FIRST_LINE_BRACES * brace - DISTANCE_BETWEEN_LINES;
                    SDL_FillRect(stave, &line_rect, 0x00);
                    line_rect.y =
                            Y_FIRST_LINE + DISTANCE_BETWEEN_FIRST_LINE_HANDS_STAVES +
                            DISTANCE_BETWEEN_FIRST_LINE_BRACES * brace - DISTANCE_BETWEEN_LINES * 2;
                    SDL_FillRect(stave, &line_rect, 0x00);
                    line_rect.y =
                            Y_FIRST_LINE + DISTANCE_BETWEEN_FIRST_LINE_HANDS_STAVES +
                            DISTANCE_BETWEEN_FIRST_LINE_BRACES * brace - DISTANCE_BETWEEN_LINES * 3;
                    SDL_FillRect(stave, &line_rect, 0x00);
                }
                break;
            case 'A':
                if (chord_to_put->notes_[k].height == 1) {
                    insert_note_rect.y =
                            Y_FIRST_LINE + DISTANCE_BETWEEN_FIRST_LINE_HANDS_STAVES +
                            DISTANCE_BETWEEN_FIRST_LINE_BRACES * brace + DISTANCE_BETWEEN_LINES * 7 -
                            Y_OF_ON_LINE_NOTES;
                    line_rect.y =
                            Y_FIRST_LINE + DISTANCE_BETWEEN_FIRST_LINE_HANDS_STAVES +
                            DISTANCE_BETWEEN_FIRST_LINE_BRACES * brace + DISTANCE_BETWEEN_LINES * 7;
                    SDL_FillRect(stave, &line_rect, 0x00);
                    line_rect.y =
                            Y_FIRST_LINE + DISTANCE_BETWEEN_FIRST_LINE_HANDS_STAVES +
                            DISTANCE_BETWEEN_FIRST_LINE_BRACES * brace + DISTANCE_BETWEEN_LINES * 6;
                    SDL_FillRect(stave, &line_rect, 0x00);
                    line_rect.y =
                            Y_FIRST_LINE + DISTANCE_BETWEEN_FIRST_LINE_HANDS_STAVES +
                            DISTANCE_BETWEEN_FIRST_LINE_BRACES * brace + DISTANCE_BETWEEN_LINES * 5;
                    SDL_FillRect(stave, &line_rect, 0x00);
                } else if (chord_to_put->notes_[k].height == 2) {
                    insert_note_rect.y =
                            Y_FIRST_LINE + DISTANCE_BETWEEN_FIRST_LINE_HANDS_STAVES +
                            DISTANCE_BETWEEN_FIRST_LINE_BRACES * brace + DISTANCE_BETWEEN_LINES * 3 -
                            Y_OF_BETWEEN_LINE_NOTES;
                } else if (chord_to_put->notes_[k].height == 3) {
                    insert_note_rect.y =
                            Y_FIRST_LINE + DISTANCE_BETWEEN_FIRST_LINE_HANDS_STAVES +
                            DISTANCE_BETWEEN_FIRST_LINE_BRACES * brace -
                            Y_OF_ON_LINE_NOTES;
                }
                break;
            case 'H':
                if (chord_to_put->notes_[k].height == 1) {
                    insert_note_rect.y =
                            Y_FIRST_LINE + DISTANCE_BETWEEN_FIRST_LINE_HANDS_STAVES +
                            DISTANCE_BETWEEN_FIRST_LINE_BRACES * brace + DISTANCE_BETWEEN_LINES * 6 -
                            Y_OF_BETWEEN_LINE_NOTES;
                    line_rect.y =
                            Y_FIRST_LINE + DISTANCE_BETWEEN_FIRST_LINE_HANDS_STAVES +
                            DISTANCE_BETWEEN_FIRST_LINE_BRACES * brace + DISTANCE_BETWEEN_LINES * 6;
                    SDL_FillRect(stave, &line_rect, 0x00);
                    line_rect.y =
                            Y_FIRST_LINE + DISTANCE_BETWEEN_FIRST_LINE_HANDS_STAVES +
                            DISTANCE_BETWEEN_FIRST_LINE_BRACES * brace + DISTANCE_BETWEEN_LINES * 5;
                    SDL_FillRect(stave, &line_rect, 0x00);
                } else if (chord_to_put->notes_[k].height == 2) {
                    insert_note_rect.y =
                            Y_FIRST_LINE + DISTANCE_BETWEEN_FIRST_LINE_HANDS_STAVES +
                            DISTANCE_BETWEEN_FIRST_LINE_BRACES * brace + DISTANCE_BETWEEN_LINES * 3 -
                            Y_OF_ON_LINE_NOTES;
                } else if (chord_to_put->notes_[k].height == 3) {
                    insert_note_rect.y =
                            Y_FIRST_LINE + DISTANCE_BETWEEN_FIRST_LINE_HANDS_STAVES +
                            DISTANCE_BETWEEN_FIRST_LINE_BRACES * brace - DISTANCE_BETWEEN_LINES -
                            Y_OF_BETWEEN_LINE_NOTES;
                }
                break;
            default:
                break;
        }

        ///// Kropki przedłużające nute
        if (chord_to_put->_time_dots_) {
            dots_rect.y = insert_note_rect.y;
            dots_rect.x = insert_note_rect.x + WIDTH_NOTES_WITHOUT_FLAG;
            for (j = 0; j < chord_to_put->_time_dots_; j++) {
                SDL_BlitSurface(time_dot, NULL, stave, &dots_rect);
                dots_rect.x += time_dot->w;
            }
        }
        ///// Znaki artykulacji
        if (chord_to_put->ssp_articulation) {

            if (chord_to_put->ssp_articulation == 1) {
                if (path[prefix_len + 1] == 'u' && k == 0) {
                    dots_rect.x = insert_note_rect.x + WIDTH_NOTES_WITHOUT_FLAG / 3;
                    dots_rect.y = insert_note_rect.y + 15;
                    SDL_BlitSurface(time_dot, NULL, stave, &dots_rect);
                }  else if (path[prefix_len + 1] == 'd' && k == chord_to_put->notes_number - 1) {
                    dots_rect.x = insert_note_rect.x + WIDTH_NOTES_WITHOUT_FLAG / 3;
                    dots_rect.y = insert_note_rect.y - 15;
                    SDL_BlitSurface(time_dot, NULL, stave, &dots_rect);
                }
            }
        }

        if (insert_note_rect.y != 0) {
            SDL_BlitSurface(note_to_put, NULL, stave, &insert_note_rect);
            any_put = 1;
            ////// Znaki chromatyczne
            if (is_acci_req(chord_to_put, k, COE->current_O->default_serial_key) || chord_to_put->notes_[k].may_print_acci) {
                acci_rect.y = insert_note_rect.y + Y_OF_ACCI_S_ON_LINE + 10;
                switch (chord_to_put->notes_[i].acci) {
                    case 's':
                        acci_to_put = acci_s;
                        break;
                    case 'b':
                        acci_to_put = acci_b;
                        acci_rect.y -= DISTANCE_BETWEEN_LINES / 2;
                        break;
                    case 'n':
                        acci_to_put = acci_n;
                        break;
                    default:
                        break;
                }

                SDL_BlitSurface(acci_to_put, NULL, stave, &acci_rect);
            }
        }
    }

    ///// Jescze się przyda funkcja odległości od (CHORDS->time)

    if (!any_put) {
        insert_note_rect.y = Y_FIRST_LINE + brace * DISTANCE_BETWEEN_FIRST_LINE_BRACES + hand * DISTANCE_BETWEEN_FIRST_LINE_HANDS_STAVES;
        insert_note_rect.h = 4 * DISTANCE_BETWEEN_LINES;
        insert_note_rect.w = WIDTH_NOTES_WITHOUT_FLAG;
        if (chord_to_put == COE->current_C){
            SDL_FillRect(stave, &insert_note_rect, 0x0f);

        } else {
            SDL_FillRect(stave, &insert_note_rect, 0x05);
        }
    }

    chord_to_put->X_position = *X_start;
    *X_start += b_space->widths_ni_[chord_to_put->time][chord_to_put->_time_dots_];


//    SDL_FreeSurface(note_without_flag);
//    SDL_FreeSurface(note_with_flag);
//    SDL_FreeSurface(note_to_put);
//    SDL_FreeSurface(acci_b);
//    SDL_FreeSurface(acci_s);
//    SDL_FreeSurface(acci_n);
//    SDL_FreeSurface(acci_to_put);

    return 0;
}
int put_pause_on_stave_without_beam(CHORD *chord_to_put, SDL_Surface *stave, SDL_Surface *blank_stave, int *X_start,
                                    int brace, int hand, BARS_SPACE *b_space, struct current_OPUS_edits_ *COE){


    SDL_Rect blank_stave_rect;

    blank_stave_rect.x = *X_start - WIDTH_NOTES_WITH_FLAGS / 2;
    blank_stave_rect.y = Y_FIRST_LINE + hand * DISTANCE_BETWEEN_FIRST_LINE_HANDS_STAVES +
                         brace * DISTANCE_BETWEEN_FIRST_LINE_BRACES - 3 * DISTANCE_BETWEEN_LINES;
    blank_stave_rect.w = WIDTH_NOTES_WITH_FLAGS * 2;
    blank_stave_rect.h = DISTANCE_BETWEEN_LINES * 8;
    //SDL_BlitSurface(blank_stave, &blank_stave_rect, stave, &blank_stave_rect);

    SDL_Rect pause_rect;
    pause_rect.x = *X_start;
    pause_rect.y = Y_FIRST_LINE + hand * DISTANCE_BETWEEN_FIRST_LINE_HANDS_STAVES + brace * DISTANCE_BETWEEN_FIRST_LINE_BRACES;
    SDL_Surface *pause = NULL;
    char path[26] = "obrazki/notes/paus_0n.bmp";
    int prefix_len = 19;
    path[prefix_len] = (char)(chord_to_put->time + 48);

    if (chord_to_put == COE->current_C){
        path[prefix_len - 2] = 'b';
    }

    pause = SDL_LoadBMP(path);

    SDL_BlitSurface(pause, NULL, stave, &pause_rect);

    chord_to_put->X_position = *X_start;
    *X_start += b_space->widths_ni_[chord_to_put->time][chord_to_put->_time_dots_];

    SDL_FreeSurface(pause);
    return 0;
}
int put_chord_on_stave_without_beam(CHORD *chord_to_put, SDL_Surface *stave, SDL_Surface *blank_stave, int *X_start,
                                    int hand, int brace, const char *serial_key, BARS_SPACE *b_space, struct current_OPUS_edits_ *COE) {

    if (chord_to_put->notes_[0].name == 'P') {
        put_pause_on_stave_without_beam(chord_to_put, stave, blank_stave, X_start, brace, hand, b_space, COE);
    } else {

        if (hand == 0) {
            put_chord_on_treb_without_beam(chord_to_put, stave, blank_stave, X_start, brace, serial_key, b_space, COE);
        } else {
            put_chord_on_bass_without_beam(chord_to_put, stave, blank_stave, X_start, brace, serial_key, b_space, COE);
        }
    }
    return 0;
}

int put_bar_on_stave(BARS *bar_to_put, SDL_Surface *stave, SDL_Surface *blank_stave, int hand, int brace,
        const char serial_key[7], const int chosen_metre[2],
                     struct current_OPUS_edits_ *COE) {

    int X_start = bar_to_put->X_of_start_bar;
    int i, j, may_edit_whole_bar = 0, was_chord_deleted = 0;
    int X_start_hands[2];
    X_start_hands[0] = X_start + DISTANCE_BETWEEN_BAR_AND_FIRST_NOTE;
    X_start_hands[1] = X_start + DISTANCE_BETWEEN_BAR_AND_FIRST_NOTE;


    SDL_Rect start_bar_rect;
    start_bar_rect.x = X_start + bar_to_put->width_;
    start_bar_rect.y = Y_FIRST_LINE + DISTANCE_BETWEEN_FIRST_LINE_BRACES * brace;
    start_bar_rect.w = WIDTH_BAR_LINE;
    start_bar_rect.h = DISTANCE_BETWEEN_FIRST_LINE_HANDS_STAVES + 4 * DISTANCE_BETWEEN_LINES;

    SDL_Rect whole_bar_rect;
    whole_bar_rect.x = X_start;
    whole_bar_rect.y = Y_FIRST_LINE + brace * DISTANCE_BETWEEN_FIRST_LINE_BRACES - 4 * DISTANCE_BETWEEN_LINES;
    whole_bar_rect.w = bar_to_put->width_ + STEP_IN_BAR_WIDTH_CHANGING_WITH_SHIFT + WIDTH_BAR_LINE;
    whole_bar_rect.h =
            DISTANCE_BETWEEN_FIRST_LINE_HANDS_STAVES + 4 * DISTANCE_BETWEEN_LINES + 11 * DISTANCE_BETWEEN_LINES;

    SDL_Rect treb_bar_rect;
    treb_bar_rect.x = X_start;
    treb_bar_rect.y = Y_FIRST_LINE + brace * DISTANCE_BETWEEN_FIRST_LINE_BRACES - 4 * DISTANCE_BETWEEN_LINES;
    treb_bar_rect.w = bar_to_put->width_ + STEP_IN_BAR_WIDTH_CHANGING_WITH_SHIFT + WIDTH_BAR_LINE;
    treb_bar_rect.h = 4 * DISTANCE_BETWEEN_LINES + 11 * DISTANCE_BETWEEN_LINES;

    SDL_Rect bass_bar_rect;
    bass_bar_rect.x = X_start;
    bass_bar_rect.y =
            Y_FIRST_LINE + brace * DISTANCE_BETWEEN_FIRST_LINE_BRACES + DISTANCE_BETWEEN_FIRST_LINE_HANDS_STAVES -
            4 * DISTANCE_BETWEEN_LINES;
    bass_bar_rect.w = bar_to_put->width_ + STEP_IN_BAR_WIDTH_CHANGING_WITH_SHIFT + WIDTH_BAR_LINE;
    bass_bar_rect.h = 4 * DISTANCE_BETWEEN_LINES + 12 * DISTANCE_BETWEEN_LINES;


    CHORD *help_chord = NULL;
    double chords_periods[6][6], max_metre_sum, metre_sum = 0;
    max_metre_sum = (double) chosen_metre[0] / (double) chosen_metre[1];
    chords_periods[0][0] = 1;
    for (i = 1; i < 6; i++) {
        chords_periods[i][0] = chords_periods[i - 1][0] / 2;
        for (j = 1; j < 6; j++) {
            chords_periods[i][j] = chords_periods[i][0] * (2 - pow(2, -(double)j));
        }
    }
    //////Trzeba usunąć nadmiarowe chord-y dla treb
    help_chord = bar_to_put->first_chord_treb;


    while (help_chord != NULL) {
        metre_sum += chords_periods[help_chord->time][help_chord->_time_dots_];
        help_chord = help_chord->next;
    }

    while (metre_sum > max_metre_sum) {
        was_chord_deleted = 1;
        help_chord = bar_to_put->first_chord_treb;
        while (help_chord->next != NULL) {
            help_chord = help_chord->next;
        }
        if (help_chord->prev != NULL) {
            help_chord->prev->next = NULL;
            free(help_chord);
        } else {
            help_chord->time++;
        }

        help_chord = bar_to_put->first_chord_treb;
        metre_sum = 0;
        while (help_chord != NULL) {
            metre_sum += chords_periods[help_chord->time][help_chord->_time_dots_];
            help_chord = help_chord->next;
        }
    }


    //////Trzeba usunąć nadmiarowe chord-y dla bass
    metre_sum = 0;
    help_chord = bar_to_put->first_chord_bass;

    while (help_chord != NULL) {
        metre_sum += chords_periods[help_chord->time][help_chord->_time_dots_];
        help_chord = help_chord->next;
    }

    while (metre_sum > max_metre_sum) {
        was_chord_deleted = 1;
        help_chord = bar_to_put->first_chord_bass;
        while (help_chord->next != NULL) {
            help_chord = help_chord->next;
        }

        if (help_chord->prev != NULL) {
            help_chord->prev->next = NULL;
            free(help_chord);
        } else {
            help_chord->time++;
        }

        help_chord = bar_to_put->first_chord_bass;
        metre_sum = 0;
        while (help_chord != NULL) {
            metre_sum += chords_periods[help_chord->time][help_chord->_time_dots_];
            help_chord = help_chord->next;
        }
    }
    if (was_chord_deleted) {
        if (COE->current_hand == 0) {
            help_chord = bar_to_put->first_chord_treb;
            while (help_chord->next != NULL) {
                help_chord = help_chord->next;
            }
            COE->current_C = help_chord;
        } else {
            help_chord = bar_to_put->first_chord_bass;
            while (help_chord->next != NULL) {
                help_chord = help_chord->next;
            }
            COE->current_C = help_chord;
        }
        COE->current_note_index = COE->current_C->notes_number - 1;
    }

    //////////////////////////////
    BARS_SPACE b_space;

    get_space_for_chord(bar_to_put, &b_space, chosen_metre);

    for (i = 0; i < 6; i++) {
        for (j = 0; j < 6; j++) {
            if (bar_to_put->b_space.widths_ni_[i][j] != b_space.widths_ni_[i][j]) {
                bar_to_put->b_space.widths_ni_[i][j] = b_space.widths_ni_[i][j];
                may_edit_whole_bar = 1;
            }
        }
    }

    if (may_edit_whole_bar || hand == 2) {
        SDL_BlitSurface(blank_stave, &whole_bar_rect, stave, &whole_bar_rect);
    } else {
        if (hand == 0) {
            SDL_BlitSurface(blank_stave, &treb_bar_rect, stave, &treb_bar_rect);
        } else if (hand == 1) {
            SDL_BlitSurface(blank_stave, &bass_bar_rect, stave, &bass_bar_rect);
        }
    }
    SDL_FillRect(stave, &start_bar_rect, 0x00);

    ///Odkładanie chord-ów
    if (may_edit_whole_bar || hand == 2) {
        help_chord = bar_to_put->first_chord_treb;
        while (help_chord != NULL) {
            put_chord_on_stave_without_beam(help_chord, stave, blank_stave, &X_start_hands[0], 0, brace, serial_key,
                                            &b_space, COE);
            help_chord = help_chord->next;
        }
        help_chord = bar_to_put->first_chord_bass;
        while (help_chord != NULL) {
            put_chord_on_stave_without_beam(help_chord, stave, blank_stave, &X_start_hands[1], 1, brace, serial_key,
                                            &b_space, COE);
            help_chord = help_chord->next;
        }
    } else {
        if (hand == 0) {
            help_chord = bar_to_put->first_chord_treb;
            while (help_chord != NULL) {
                put_chord_on_stave_without_beam(help_chord, stave, blank_stave, &X_start_hands[0], 0, brace, serial_key,
                                                &b_space, COE);
                help_chord = help_chord->next;
            }
        } else if (hand == 1) {
            help_chord = bar_to_put->first_chord_bass;
            while (help_chord != NULL) {
                put_chord_on_stave_without_beam(help_chord, stave, blank_stave, &X_start_hands[1], 1, brace, serial_key,
                                                &b_space, COE);
                help_chord = help_chord->next;
            }
        }
    }


    return 0;
}
int put_all_bars_on_stave(BARS *first_bar_to_put, SDL_Surface *stave, SDL_Surface *blank_stave, const char serial_key[7],
        const int chosen_metre[2],
                          struct current_OPUS_edits_ *COE){
    SDL_BlitSurface(blank_stave, NULL, stave, NULL);

    while (first_bar_to_put != NULL){
        put_bar_on_stave(first_bar_to_put, stave, blank_stave, 2, first_bar_to_put->brace, serial_key, chosen_metre, COE);
        first_bar_to_put = first_bar_to_put->next;
    }
    return 0;
}

CHORD *malloc_new_chord(CHORD *prev, CHORD *next, int X_position, const char *serial_key){

    int i;
    CHORD *new_chord = malloc(sizeof(*new_chord));
    new_chord->prev = prev;
    new_chord->next = next;
    new_chord->notes_number = 0;
    new_chord->X_position = X_position;
    new_chord->time = 5;
    new_chord->_time_dots_ = 0;
    new_chord->ssp_articulation = 0;
    for (i = 0; i < 7; i++) {
        new_chord->local_serial_key[i] = serial_key[i];
    }


    return new_chord;
}
BARS *malloc_new_bar(BARS *prev, BARS *next, int X_of_start_bar, int width, int brace,  const char *treb_serial_key,
        const char *bass_serial_key) {

    BARS *new_bar = malloc(sizeof(*new_bar));
    new_bar->prev = prev;
    new_bar->next = next;
    new_bar->brace = brace;
    new_bar->X_of_start_bar = X_of_start_bar;
    new_bar->first_chord_bass = malloc_new_chord(NULL, NULL, X_of_start_bar + DISTANCE_BETWEEN_BAR_AND_FIRST_NOTE, bass_serial_key);
    new_bar->first_chord_treb = malloc_new_chord(NULL, NULL, X_of_start_bar + DISTANCE_BETWEEN_BAR_AND_FIRST_NOTE, treb_serial_key);
    if (X_of_start_bar + width > X_END_OF_STAVE) {
        new_bar->width_ = (X_END_OF_STAVE - X_of_start_bar);
    } else {
        new_bar->width_ = width;
    }
    new_bar->brace = brace;

    return new_bar;
}
int free_bar(BARS *bar_to_del) {

    if (bar_to_del == NULL) {
        return 1;
    }
    CHORD * help_chord = bar_to_del->first_chord_treb;
    while (help_chord != NULL) {
        free(help_chord);
        help_chord = help_chord->next;
    }
    help_chord = bar_to_del->first_chord_bass;
    while (help_chord != NULL) {
        free(help_chord);
        help_chord = help_chord->next;
    }

    return 0;
}
int free_opus(OPUS *opus_to_del) {

    BARS *bar_to_del = opus_to_del->first_BAR;
    while (bar_to_del->next != NULL) {
        bar_to_del = bar_to_del->next;
        free_bar(bar_to_del->prev);
        bar_to_del = bar_to_del->next;
    }
    free_bar(bar_to_del);
    free(opus_to_del);
    return 0;
}

int change_bar_width(int pressed_key, int *any_change, struct current_OPUS_edits_ *COE, const Uint8 *KEY_STATE, int X_after_key) {

    int i;
    struct current_OPUS_edits_ COE_HELP;
    if (KEY_STATE != NULL && !KEY_STATE[SDL_SCANCODE_LSHIFT] && !KEY_STATE[SDL_SCANCODE_RSHIFT]) {
        i = STEP_IN_BAR_WIDTH_CHANGING_WITHOUT_SHIFT;
    } else {
        i = STEP_IN_BAR_WIDTH_CHANGING_WITH_SHIFT;
    }
    if (pressed_key == SDLK_j) {
        COE->current_B->width_ -= i;
        if (COE->current_B->width_ < WIDTH_NOTES_WITH_FLAGS) {
            COE->current_B->width_ = WIDTH_NOTES_WITH_FLAGS;
            if (any_change != NULL) *any_change = 0;
        }
    } else if (pressed_key == SDLK_m) {
        COE->current_B->width_ += i;
        if (COE->current_B->X_of_start_bar + COE->current_B->width_ > X_END_OF_STAVE) {
            COE->current_B->width_ = X_END_OF_STAVE - COE->current_B->X_of_start_bar;
            if (any_change != NULL) *any_change = 0;
        }
    }

    COE_HELP.current_B = COE->current_B->next;
    if (any_change == NULL || *any_change) {
        while (COE_HELP.current_B != NULL) {

            COE_HELP.current_B->X_of_start_bar =
                    COE_HELP.current_B->prev->width_ + COE_HELP.current_B->prev->X_of_start_bar + 3;
            COE_HELP.current_B->width_ = __min(X_END_OF_STAVE - COE_HELP.current_B->X_of_start_bar,
                                               COE_HELP.current_B->width_);
            //COE_HELP.current_B->width_ = __min(DEFAULT_BAR_WIDTH, COE_HELP.current_B->width_);

            COE_HELP.current_B->brace = COE_HELP.current_B->prev->brace;
            if (COE_HELP.current_B->width_ < MIN_BAR_WIDTH) {
                if (COE_HELP.current_B->brace < 4) {
                    COE_HELP.current_B->brace += 1;
                    COE_HELP.current_B->X_of_start_bar = X_after_key;
                    COE_HELP.current_B->width_ = DEFAULT_BAR_WIDTH;
                } else {
                    COE_HELP.current_B->prev->next = NULL;
                    free_bar(COE_HELP.current_B);
                    COE_HELP.current_B = NULL;
                }
            }
            COE_HELP.current_B = COE_HELP.current_B->next;
        }
    }

    return 0;
}
int change_chord_len(int pressed_key, int *any_change, struct current_OPUS_edits_ *COE, const Uint8 *KEY_STATE){
    int help_tmp_1, help_tmp_2;
    if (!KEY_STATE[SDL_SCANCODE_RSHIFT] && !KEY_STATE[SDL_SCANCODE_LSHIFT]) {
        help_tmp_1 = COE->current_C->time;
        if (pressed_key == SDLK_k) {
            COE->current_C->time--;
            if (COE->current_C->time < 0) {
                COE->current_C->time = 0;
            }
        } else {
            COE->current_C->time++;
            if (COE->current_C->time > 5) {
                COE->current_C->time = 5;
            }
        }
        if (help_tmp_1 == COE->current_C->time) {
            *any_change = 0;
        }
    } else {
        help_tmp_1 = COE->current_C->_time_dots_;
        help_tmp_2 = COE->current_C->ssp_articulation;
        if (pressed_key == SDLK_l) {
            if (COE->current_C->_time_dots_ > 0) {
                COE->current_C->_time_dots_--;
            } else {
                if (COE->current_C->ssp_articulation < 3) {
                    COE->current_C->ssp_articulation++;
                }
            }

        } else {
            if (COE->current_C->ssp_articulation > 0) {
                COE->current_C->ssp_articulation--;
            } else {
                if (COE->current_C->_time_dots_ < 5) {
                    COE->current_C->_time_dots_++;
                }
            }
        }
        if (!(help_tmp_1 != COE->current_C->_time_dots_ ||
              help_tmp_2 != COE->current_C->ssp_articulation)) {
            *any_change = 0;
        }
    }
    return 0;
}
int change_hand(struct current_OPUS_edits_ *COE) {

    if (COE->current_hand == 0) {
        COE->current_C = COE->current_B->first_chord_bass;
        COE->current_hand = 1;
        COE->current_note_index = COE->current_C->notes_number - 1;

    } else {
        COE->current_C = COE->current_B->first_chord_treb;
        COE->current_hand = 0;
        COE->current_note_index = COE->current_C->notes_number - 1;
    }
    return 0;
}
int change_note(int pressed_key, struct current_OPUS_edits_ *COE) {
    if (COE->current_C->notes_number > 0) {

        if (pressed_key == SDLK_UP) {
            COE->current_note_index++;
            if (COE->current_note_index >= COE->current_C->notes_number) {
                COE->current_note_index--;
            }
        } else {
            COE->current_note_index--;
            if (COE->current_note_index < 0) {
                COE->current_note_index++;
            }
        }
    } else {
        COE->current_note_index = -1;
    }
    return 0;
}
int change_chord_or_bar(int pressed_key, struct current_OPUS_edits_ *COE, const Uint8 *KEY_STATE) {

    struct current_OPUS_edits_ COE_HELP;

    if (pressed_key == SDLK_RIGHT) {
        if (!KEY_STATE[SDL_SCANCODE_LSHIFT]) {
            if (COE->current_C->next != NULL) {
                COE->current_C = COE->current_C->next;
            } else if (COE->current_B->next != NULL) {
                COE->current_B = COE->current_B->next;
                if (COE->current_hand == 0) {
                    COE->current_C = COE->current_B->first_chord_treb;
                } else {
                    COE->current_C = COE->current_B->first_chord_bass;
                }

            }
        } else {
            if (COE->current_B->next != NULL) {
                COE->current_B = COE->current_B->next;
                if (COE->current_hand == 0) {
                    COE->current_C = COE->current_B->first_chord_treb;
                } else {
                    COE->current_C = COE->current_B->first_chord_bass;
                }

            } else {
                COE_HELP.current_C = COE->current_C;
                while (COE_HELP.current_C->next != NULL) {
                    COE_HELP.current_C = COE_HELP.current_C->next;
                }
                COE->current_C = COE_HELP.current_C;
            }
        }
    }
    if (pressed_key == SDLK_LEFT) {
        if (!KEY_STATE[SDL_SCANCODE_LSHIFT]) {
            if (COE->current_C->prev != NULL) {
                COE->current_C = COE->current_C->prev;
            } else if (COE->current_B->prev != NULL) {
                COE->current_B = COE->current_B->prev;

                if (COE->current_hand == 0) {
                    COE->current_C = COE->current_B->first_chord_treb;
                } else {
                    COE->current_C = COE->current_B->first_chord_bass;
                }
                COE_HELP.current_C = COE->current_C;
                while (COE_HELP.current_C->next != NULL) {
                    COE_HELP.current_C = COE_HELP.current_C->next;
                }
                COE->current_C = COE_HELP.current_C;
            }
        } else {
            if (COE->current_B->prev != NULL) {
                COE->current_B = COE->current_B->prev;

                if (COE->current_hand == 0) {
                    COE->current_C = COE->current_B->first_chord_treb;
                } else {
                    COE->current_C = COE->current_B->first_chord_bass;
                }
                COE_HELP.current_C = COE->current_C;
                while (COE_HELP.current_C->next != NULL) {
                    COE_HELP.current_C = COE_HELP.current_C->next;
                }
                COE->current_C = COE_HELP.current_C;
            } else {
                if (COE->current_hand == 0) {
                    COE->current_C = COE->current_B->first_chord_treb;
                } else {
                    COE->current_C = COE->current_B->first_chord_bass;
                }
            }
        }
    }

    COE->current_note_index = COE->current_C->notes_number - 1;
    return 0;
}
int put_note_or_pause(int pressed_key, struct current_OPUS_edits_ *COE, const Uint8 *KEY_STATE, const char *serial_key) {

    int help_tmp_1, i;
    if (COE->current_C->notes_number < MAX_NOTES_IN_CHORD) {
        COE->current_C->notes_number++;
        COE->current_note_index = COE->current_C->notes_number - 1;
    }

    help_tmp_1 = -1;
    for (i = SDL_SCANCODE_1; i <= SDL_SCANCODE_7; i++) {
        if (KEY_STATE[i]) {
            help_tmp_1 = i + 1 - SDL_SCANCODE_1;
            break;
        }
    }

    if (help_tmp_1 > -1 || pressed_key == SDLK_p) {
        if (pressed_key != SDLK_p) {
            char name, acci;
            name = (char) (pressed_key - 32);
            COE->current_C->notes_[COE->current_note_index].name = name;
            COE->current_C->notes_[COE->current_note_index].height = help_tmp_1;
            COE->current_C->notes_[COE->current_note_index].may_print_acci = 0;
            switch (name) {
                case 'C':
                    acci = serial_key[0];
                    break;
                case 'D':
                    acci = serial_key[1];
                    break;
                case 'E':
                    acci = serial_key[2];
                    break;
                case 'F':
                    acci = serial_key[3];
                    break;
                case 'G':
                    acci = serial_key[4];
                    break;
                case 'A':
                    acci = serial_key[5];
                    break;
                case 'H':
                    acci = serial_key[6];
                    break;
                default:
                    acci = '\0';
                    break;
            }
            COE->current_C->notes_[COE->current_note_index].acci = acci;
            sort_uniq_notes(COE->current_C);
        } else {
            COE->current_C->notes_[0].name = 'P';
            COE->current_C->notes_[0].acci = 's';            COE->current_C->notes_[0].height = 4;
            COE->current_C->notes_number = 1;
            COE->current_note_index = 0;
        }
    }
    return 0;
}
int del_note_chord_bar(struct current_OPUS_edits_ *COE, const Uint8 *KEY_STATE, int X_after_key) {

    int i;
    struct current_OPUS_edits_ COE_HELP;

    if (!KEY_STATE[SDL_SCANCODE_LSHIFT] && !KEY_STATE[SDL_SCANCODE_RSHIFT]) {
        if (COE->current_C->notes_number > 0) {
            if (COE->current_note_index == COE->current_C->notes_number - 1) {
                COE->current_C->notes_number--;
                COE->current_note_index--;
            } else {
                for (i = COE->current_note_index; i < COE->current_C->notes_number - 1; i++) {
                    swap_notes(&(COE->current_C->notes_[i]), &(COE->current_C->notes_[i + 1]));
                }
                COE->current_C->notes_number--;
            }
        }
    } else {
        // Usuwanie pojedynczego chord-u
        if (COE->current_C->prev != NULL) {
            COE_HELP.current_C = COE->current_C;
            COE->current_C = COE->current_C->prev;
            COE->current_C->next = COE->current_C->next->next;
            if (COE->current_C->next != NULL) {
                COE->current_C->next->prev = COE->current_C;
            }
            free(COE_HELP.current_C);
            // jeżeli jest pierwszy i nie jest jedyny
        } else if (COE->current_C->next != NULL) {
            COE->current_C = COE->current_C->next;
            free(COE->current_C->prev);
            COE->current_C->prev = NULL;
            if (COE->current_hand == 0) {
                COE->current_B->first_chord_treb = COE->current_C;
            } else {
                COE->current_B->first_chord_bass = COE->current_C;
            }

            // jeśli jest on pierwszym i jedynym chord-em w bar-rze, to jeśli nie jest to jedyny bar
        } else {
            if (COE->current_B->prev != NULL) {
                COE->current_B = COE->current_B->prev;

                COE_HELP.current_B = COE->current_B->next;
                COE->current_B->next = COE->current_B->next->next;
                if (COE->current_B->next != NULL) {
                    COE->current_B->next->prev = COE->current_B;
                }
                free_bar(COE_HELP.current_B);
                if (COE->current_hand == 0) {
                    COE->current_C = COE->current_B->first_chord_treb;
                } else {
                    COE->current_C = COE->current_B->first_chord_bass;
                }

                // Jesli bar jest pierwszy ale nie jest jedyny
            } else if (COE->current_B->next != NULL) {
                COE->current_B = COE->current_B->next;
                free_bar(COE->current_B->prev);
                COE->current_O->first_BAR = COE->current_B;
                if (COE->current_hand == 0) {
                    COE->current_C = COE->current_B->first_chord_treb;
                } else {
                    COE->current_C = COE->current_B->first_chord_bass;
                }
            }
        }
        COE->current_note_index = COE->current_C->notes_number - 1;
    }
    change_bar_width(0, NULL, COE, NULL, X_after_key);
    return 0;
}
int create_new_chord_bar(struct current_OPUS_edits_ *COE, const Uint8 *KEY_STATE, int X_after_key, const char *default_serial_key) {

    int brace_help, X_st_help, width_help;
    if (!KEY_STATE[SDL_SCANCODE_LSHIFT] && !KEY_STATE[SDL_SCANCODE_RSHIFT]) {
        if (COE->current_C->next == NULL) {
            COE->current_C->next = malloc_new_chord(COE->current_C, NULL,
                                                    COE->current_C->X_position + WIDTH_NOTES_WITH_FLAGS, COE->current_C->local_serial_key);
        } else {
            COE->current_C->next = malloc_new_chord(COE->current_C, COE->current_C->next,
                                                    COE->current_C->X_position + WIDTH_NOTES_WITH_FLAGS, COE->current_C->local_serial_key);
            COE->current_C->next->prev = COE->current_C;
            COE->current_C->next->next->prev = COE->current_C->next;
        }
        COE->current_C = COE->current_C->next;
        COE->current_note_index = -1;
    } else {

        brace_help = COE->current_B->brace;
        X_st_help = COE->current_B->X_of_start_bar + COE->current_B->width_ + 2;
        width_help = __min(X_END_OF_STAVE - (COE->current_B->X_of_start_bar + COE->current_B->width_),
                           DEFAULT_BAR_WIDTH);
        if (width_help < MIN_BAR_WIDTH) {
            brace_help++;
            width_help = DEFAULT_BAR_WIDTH;
            X_st_help = X_after_key;
        }

        COE->current_B->next = malloc_new_bar(COE->current_B, COE->current_B->next, X_st_help, width_help, brace_help,
                default_serial_key, default_serial_key);
        COE->current_B = COE->current_B->next;
        if (COE->current_B->next != NULL) {
            COE->current_B->next->prev = COE->current_B;
        }
        if (COE->current_hand == 0) {
            COE->current_C = COE->current_B->first_chord_treb;
        } else {
            COE->current_C = COE->current_B->first_chord_bass;
        }
    }

    COE->current_note_index = -1;
    change_bar_width(0, NULL, COE, NULL, X_after_key);

    return 0;
}
int put_acci(int pressed_key, struct current_OPUS_edits_ *COE, const Uint8 *KEY_STATE, const char *default_serial_key) {


    if (pressed_key == SDLK_x) {
        COE->current_C->notes_[COE->current_note_index].may_print_acci += 1;
        COE->current_C->notes_[COE->current_note_index].may_print_acci %= 2;
    } else if (COE->current_C->notes_[COE->current_note_index].name != 'p') {
        int i = 0;
        COE->current_C->notes_[COE->current_note_index].acci = (char) pressed_key;
        sort_uniq_notes(COE->current_C);
        switch (COE->current_C->notes_[COE->current_note_index].name) {
            case 'C':
                i = 0;
                break;
            case 'D':
                i = 1;
                break;
            case 'E':
                i = 2;
                break;
            case 'F':
                i = 3;
                break;
            case 'G':
                i = 4;
                break;
            case 'A':
                i = 5;
                break;
            case 'H':
                i = 6;
                break;
            default:
                break;
        }
        COE->current_C->local_serial_key[i] = (char) pressed_key;
        CHORD *help_chord = COE->current_C->next;
        while (help_chord != NULL) {

            if (help_chord->local_serial_key[i] == default_serial_key[i]) {
                help_chord->local_serial_key[i] = help_chord->prev->local_serial_key[i];
            } else {
                break;
            }
            help_chord = help_chord->next;
        }
    }

    return 0;
}

OPUS *create_new_OPUS(char chosen_key[2], int chosen_metre[2], OPUS *prev_opus) {

    int i;

    OPUS *current_OPUS = NULL;
    if (prev_opus == NULL) {
        current_OPUS = malloc(sizeof(*current_OPUS));
        current_OPUS->key[0] = chosen_key[0];
        current_OPUS->key[1] = chosen_key[1];
        current_OPUS->time_sign[0] = chosen_metre[0];
        current_OPUS->time_sign[1] = chosen_metre[1];

        ///////
        for (i = 0; i < 10; i++) {
            current_OPUS->title[i] = 'x';
            current_OPUS->author[i] = 'x';
        }
        current_OPUS->title[10] = '\0';
        current_OPUS->author[10] = '\0';
        current_OPUS->temp = 60;
        //////
    } else {
        current_OPUS = prev_opus;
    }

    SDL_Event occurrence;
    int may_exit = 0, is_instruction_open = 0, page_number = 0;
    char default_serial_key[7];
    get_serial_key(chosen_key, default_serial_key);

    int X_start_on_treb = X_START_AFTER_KEY;
    int X_start_on_bass = X_START_AFTER_KEY;
    int X_after_key;

    SDL_Window *window = NULL;
    SDL_Surface *screen = NULL;
    SDL_Surface *stave = NULL;
    SDL_Surface *blank_stave = NULL;
    SDL_Surface *stave_with_key_and_metre = NULL;

    stave = SDL_LoadBMP("obrazki/wiolbas.bmp");
    blank_stave = SDL_LoadBMP("obrazki/wiolbas.bmp");
    stave_with_key_and_metre = SDL_LoadBMP("obrazki/wiolbas.bmp");
    window = SDL_CreateWindow("MUZOWNIK", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, stave->w,
                              SCREEN_HIGHT, SDL_WINDOW_SHOWN);
    screen = SDL_GetWindowSurface(window);

    SDL_Rect Rect_current_view;
    Rect_current_view.x = 0;
    Rect_current_view.y = 0;
    Rect_current_view.w = stave->w;
    Rect_current_view.h = SCREEN_HIGHT;

    SDL_Surface *instructions_[3];
    instructions_[0] = SDL_LoadBMP("obrazki/menu/stave_instructions_str1.bmp");
    instructions_[1] = SDL_LoadBMP("obrazki/menu/stave_instructions_str2.bmp");
    instructions_[2] = SDL_LoadBMP("obrazki/menu/stave_instructions_str3.bmp");


    put_key(stave, chosen_key, NULL, &X_start_on_treb);
    X_after_key = X_start_on_treb;
    put_metre(stave, chosen_metre, NULL, &X_start_on_treb, 0);
    SDL_BlitSurface(stave, NULL, stave_with_key_and_metre, NULL);
    X_start_on_treb += 5;
    X_start_on_bass = X_start_on_treb;

    //// Alokowanie pierwszego bar-u

    struct current_OPUS_edits_ COE;

    COE.current_O = current_OPUS;
    if (prev_opus == NULL) {
        COE.current_O->first_BAR = malloc_new_bar(NULL, NULL, X_start_on_treb, DEFAULT_BAR_WIDTH, 0, default_serial_key,
                                                  default_serial_key);
    }
    COE.current_B = COE.current_O->first_BAR;
    COE.current_B->X_of_start_bar = X_start_on_treb;
    COE.current_C = COE.current_B->first_chord_treb;



    COE.current_note_index = COE.current_C->notes_number - 1;
    COE.current_hand = 0;

    for (i = 0; i < 7; i++){
        COE.current_O->default_serial_key[i] = default_serial_key[i];
    }

    int pressed_key, any_change;
    const Uint8 *KEY_STATE = SDL_GetKeyboardState(NULL);
    put_all_bars_on_stave(COE.current_B, stave, stave_with_key_and_metre, default_serial_key, chosen_metre, &COE);
    SDL_BlitSurface(stave, &Rect_current_view, screen, NULL);
    SDL_UpdateWindowSurface(window);
    SDL_Delay(5);
    SDL_UpdateWindowSurface(window);

    while (!may_exit) {
        while (SDL_PollEvent(&occurrence)) {
            if (occurrence.type == SDL_QUIT) {
                may_exit = 1;
            }
            any_change = 0;

            if (!is_instruction_open && occurrence.type == SDL_MOUSEWHEEL) {
                any_change = 1;
                scroll_updown(screen, stave, &Rect_current_view, &occurrence);
            }

            if ((KEY_STATE[SDL_SCANCODE_LCTRL] && occurrence.type == SDL_KEYDOWN && occurrence.key.keysym.sym == SDLK_h)) {
                is_instruction_open += 1;
                is_instruction_open %= 2;
                page_number = 0;
                SDL_Delay(300);
            }

            if (occurrence.type == SDL_KEYDOWN && occurrence.key.keysym.sym == SDLK_ESCAPE) {
                may_exit = 1;
                SDL_Delay(200);
            }

            if (is_instruction_open) {
                if (occurrence.type == SDL_KEYDOWN && occurrence.key.keysym.sym == SDLK_RIGHT) {
                    page_number = __min(page_number + 1, 2);
                    SDL_Delay(200);
                } else if (occurrence.type == SDL_KEYDOWN && occurrence.key.keysym.sym == SDLK_LEFT) {
                    page_number = __max(page_number - 1, 0);
                    SDL_Delay(200);
                }
                SDL_BlitSurface(instructions_[page_number], NULL, screen, NULL);
                SDL_UpdateWindowSurface(window);
            }

            if (!is_instruction_open && occurrence.type == SDL_KEYDOWN) {
                any_change = 1;

                SDL_Delay(75);

                pressed_key = occurrence.key.keysym.sym;
                /////////// Zmiana szerokości aktualnego taktu
                if (pressed_key == SDLK_j || pressed_key == SDLK_m) {
                    change_bar_width(pressed_key, &any_change, &COE, KEY_STATE, X_after_key);

                }
                ////////// Zmiana długości chord-u
                if (pressed_key == SDLK_k || pressed_key == SDLK_l) {
                    change_chord_len(pressed_key, &any_change, &COE, KEY_STATE);
                }
                ///////// Zmiana ręki
                if (pressed_key == SDLK_LALT) {
                    change_hand(&COE);
                }
                //////// Zmiana edytowanej nuty
                if ((pressed_key == SDLK_UP || pressed_key == SDLK_DOWN)) {
                    change_note(pressed_key, &COE);
                }
                //////// Zmiana chord-u lub bar-u
                if (pressed_key == SDLK_LEFT || pressed_key == SDLK_RIGHT) {
                    change_chord_or_bar(pressed_key, &COE, KEY_STATE);
                }
                /////// Odkładanie nuty lub pauzy
                if (pressed_key == SDLK_a || pressed_key == SDLK_p ||
                    (SDLK_c <= pressed_key && pressed_key <= SDLK_h)) {
                    put_note_or_pause(pressed_key, &COE, KEY_STATE, COE.current_C->local_serial_key);
                }
                /////// Odkładanie znaku chromatycznego
                if ((pressed_key == SDLK_s || pressed_key == SDLK_b || pressed_key == SDLK_n ||
                     pressed_key == SDLK_x) && COE.current_C->notes_number > 0) {
                    put_acci(pressed_key, &COE, KEY_STATE, COE.current_C->local_serial_key);
                }
                /////// Usuwanie nut i chord-ów
                if (pressed_key == SDLK_BACKSPACE) {
                    del_note_chord_bar(&COE, KEY_STATE, X_after_key);
                }
                ////// Tworzenie noewgo chord-u lub bar-u
                if (pressed_key == SDLK_SPACE) {
                    create_new_chord_bar(&COE, KEY_STATE, X_after_key, default_serial_key);
                }

            }
            if (any_change && !is_instruction_open) {
                put_all_bars_on_stave(COE.current_O->first_BAR, stave, stave_with_key_and_metre, default_serial_key,
                                      chosen_metre, &COE);
                SDL_BlitSurface(stave, &Rect_current_view, screen, NULL);
                SDL_UpdateWindowSurface(window);
                SDL_Delay(10);
            }
        }
    }

    COE.current_C = NULL;
    put_all_bars_on_stave(COE.current_O->first_BAR, stave, stave_with_key_and_metre, default_serial_key, chosen_metre, &COE);
    SDL_SaveBMP(stave, "wynik.bmp");
    SDL_DestroyWindow(window);
    //SDL_FreeSurface(screen);

    return current_OPUS;
}

/*
    opus_text_file:

    title:author:key:serial_key:metre:temp:
    {X_of_start_bar:width_:brace:
        [time:_time_dots_:ssp_articulation:notes_number:X_positon:local_serial_key:
            (name:acci:height)(...)...(...)
        ][...
        ]...[...
        ]
        | // pipe, left hand above, right hand beneath
        [...
        ]...[...
        ]
    }{...
    }...{...
    }EOF
*/

int fprint_note(FILE *opus_text_file, NOTES *n) {

    fprintf(opus_text_file, "%c", '(');
    fprintf(opus_text_file, "%c:", n->name);
    fprintf(opus_text_file, "%c:", n->acci);
    fprintf(opus_text_file, "%d", n->height);
    fprintf(opus_text_file, "%c", ')');

    return 0;
}
int fprint_chord(FILE *opus_text_file, CHORD *chord_to_print) {

    int i;
    fprintf(opus_text_file, "%c", '[');
    ///
    fprintf(opus_text_file, "%d:", chord_to_print->time);
    fprintf(opus_text_file, "%d:", chord_to_print->_time_dots_);
    fprintf(opus_text_file, "%d:", chord_to_print->ssp_articulation);
    fprintf(opus_text_file, "%d:", chord_to_print->notes_number);
    fprintf(opus_text_file, "%d:", chord_to_print->X_position);
    for (i = 0; i < 7; i++) {
        fprintf(opus_text_file, "%c", chord_to_print->local_serial_key[i]);
    }
    fprintf(opus_text_file, "%c", ':');
    for (i = 0; i < chord_to_print->notes_number; i++) {
        fprint_note(opus_text_file, &(chord_to_print->notes_[i]));
    }
    ///
    fprintf(opus_text_file, "%c", ']');

    return 0;
}
int fprint_bar(FILE *opus_text_file, BARS *bar_to_print) {

    CHORD *help_chord = NULL;
    fprintf(opus_text_file, "%c", '{');
    fprintf(opus_text_file, "%d:", bar_to_print->X_of_start_bar);
    fprintf(opus_text_file, "%d:", bar_to_print->width_);
    fprintf(opus_text_file, "%d:", bar_to_print->brace);

    help_chord = bar_to_print->first_chord_treb;
    while (help_chord != NULL) {
        fprint_chord(opus_text_file, help_chord);
        help_chord = help_chord->next;
    }
    fprintf(opus_text_file, "%c", '|');
    help_chord = bar_to_print->first_chord_bass;
    while (help_chord != NULL) {
        fprint_chord(opus_text_file, help_chord);
        help_chord = help_chord->next;
    }
    fprintf(opus_text_file, "%c", '}');

    return 0;
}
int save_OPUS_as_TextFile(OPUS *OPUS_to_save) {

    int i;
    FILE *opus_text_file = NULL;
    if ((opus_text_file = fopen("opus.txt", "w")) == NULL) {
        printf("Nie mogę otworzyć pliku test.txt do zapisu!\n");
        exit(1);
    }

    fprintf(opus_text_file, "%s:", OPUS_to_save->title);
    fprintf(opus_text_file, "%s:", OPUS_to_save->author);
    fprintf(opus_text_file, "%c", OPUS_to_save->key[0]);
    if (OPUS_to_save->key[1] == '\0') {
        fprintf(opus_text_file, "%c", ':');
    } else {
        fprintf(opus_text_file, "%c%c", OPUS_to_save->key[1],':');
    }
    for (i = 0; i < 7; i++){
        fprintf(opus_text_file, "%c", OPUS_to_save->default_serial_key[i]);
    }
    fprintf(opus_text_file, "%c", ':');

    fprintf(opus_text_file, "%d,%d:", OPUS_to_save->time_sign[0], OPUS_to_save->time_sign[1]);
    fprintf(opus_text_file, "%d:", OPUS_to_save->temp);

    BARS *bar_to_put = OPUS_to_save->first_BAR;

    while (bar_to_put != NULL) {
        fprint_bar(opus_text_file, bar_to_put);
        bar_to_put = bar_to_put->next;
    }

    fclose (opus_text_file);
    return 0;
}



