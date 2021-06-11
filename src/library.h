#ifndef LIBS_PI_PROJECTSDL2_LIBRARY_H
#define LIBS_PI_PROJECTSDL2_LIBRARY_H



#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <SDL2/SDL.h>
#include <time.h>
#include "geometry_def.h"

#ifndef __min
#define __min(a, b) (((a) < (b)) ? (a) : (b))
#endif

#ifndef __max
#define __max(a, b) (((a) < (b)) ? (b) : (a))
#endif

struct notes{

    char name;
    char acci;
    int height;
    int may_print_acci;
};

////W przypadku poniższych struktur nie używa się wartowników
struct chords{

    int time;
    int _time_dots_;
    int ssp_articulation;
    int notes_number;
    struct notes notes_[MAX_NOTES_IN_CHORD];
    struct chords *prev;
    struct chords *next;
    int X_position;
    char local_serial_key[7];
};
struct bars_space{

    int widths_ni_[6][6];
};
struct bars{

    struct chords *first_chord_treb;
    struct chords *first_chord_bass;
    struct bars *prev;
    struct bars *next;
    int X_of_start_bar;
    int width_;
    struct bars_space b_space;
    int brace;
};
struct OPUS{

    char title[50];
    char author[50];
    char key[2];
    int time_sign[2];
    int temp;
    struct bars *first_BAR;
    char default_serial_key[7];
};

typedef struct notes NOTES;
typedef struct chords CHORDS;
typedef struct bars BARS;
typedef struct OPUS OPUS;
typedef struct bars_space BARS_SPACE;

struct current_OPUS_edits_{

    OPUS *current_O;
    BARS *current_B;
    CHORDS *current_C;
    int current_hand;
    int current_note_index;

};

int fscanf_note(FILE *opus_text_file, NOTES *n);
CHORDS *fscanf_chord(FILE *opus_text_file);
BARS *fscanf_bar(FILE *opus_text_file);
OPUS *fscanf_opus(const char *path);


int pow_int(int a, int b);
void scroll_updown(SDL_Surface *screen, SDL_Surface *stave, SDL_Rect *current, SDL_Event *occurrence);
int get_path_to_key_bmp(const char chosen_key[2], char path_to_key_bmp[], int hand, int path_prefix_len);
int put_key(SDL_Surface *stave, const char chosen_key[2], OPUS *current_OPUS, int *X_star_on_lines);
int get_path_to_metre_bmp(const int chosen_metre[2], char path_to_metre_bmp[], int path_prefix_len);
int put_metre(SDL_Surface *stave, const int chosen_metre[2], OPUS *current_OPUS, int *X_start_on_line, int brace);
int help_window(SDL_Surface *screen, SDL_Window *window);
int menu_open(char chosen_key[2], int chosen_metre[2]);

int fifths_choosing(SDL_Surface *screen, SDL_Window *window, char chosen_key[2]);
int menu_window(SDL_Surface *screen, SDL_Window *window);
int metre_choosing(SDL_Surface *screen, SDL_Window *window, int chosen_metre[2]);

//////////////////////Functions for writing new stave:
void swap_notes(NOTES *n1, NOTES *n2);
int get_note_index(NOTES *n);
int compare_notes(NOTES *n1, NOTES *n2);
int sort_uniq_notes(CHORDS *chord_to_sort);
int get_serial_key(const char *chosen_key, char serial_key[7]);
int is_acci_req(CHORDS *chord_to_put, int k, const char *defauly_serial_key);
int get_space_for_chord(BARS *bar, BARS_SPACE *b_space, const int *metre);


int put_chord_on_treb_without_beam(CHORDS *chord_to_put, SDL_Surface *stave, SDL_Surface *blank_stave, int *X_start, int brace, const char serial_key[7], BARS_SPACE *b_space, struct current_OPUS_edits_ *COE);
int put_chord_on_bass_without_beam(CHORDS *chord_to_put, SDL_Surface *stave, SDL_Surface *blank_stave, int *X_start, int brace, const char serial_key[7], BARS_SPACE *b_space, struct current_OPUS_edits_ *COE);
int put_pause_on_stave_without_beam(CHORDS *chord_to_put, SDL_Surface *stave, SDL_Surface *blank_stave, int *X_start, int brace, int hand, BARS_SPACE *b_space, struct current_OPUS_edits_ *COE);
int put_chord_on_stave_without_beam(CHORDS *chord_to_put, SDL_Surface *stave, SDL_Surface *blank_stave, int *X_start, int hand, int brace, const char *serial_key, BARS_SPACE *b_space, struct current_OPUS_edits_ *COE);
int put_bar_on_stave(BARS *bar_to_put, SDL_Surface *stave, SDL_Surface *blank_stave, int hand, int brace, const char serial_key[7], const int chosen_metre[2], struct current_OPUS_edits_ *COE);
int put_all_bars_on_stave(BARS *first_bar_to_put, SDL_Surface *stave, SDL_Surface *blank_stave, const char serial_key[7], const int chosen_metre[2], struct current_OPUS_edits_ *COE);

CHORDS *malloc_new_chord(CHORDS *prev, CHORDS *next, int X_position, const char *serial_key);
BARS *malloc_new_bar(BARS *prev, BARS *next, int X_of_start_bar, int width, int brace,   const char *treb_serial_key,  const char *bass_serial_key);
int free_bar(BARS *bar_to_del);
int free_opus(OPUS *opus_to_del);

int change_bar_width(int pressed_key, int *any_change, struct current_OPUS_edits_ *COE, const Uint8 *KEY_STATE, int X_after_key);
int change_chord_len(int pressed_key, int *any_change, struct current_OPUS_edits_ *COE, const Uint8 *KEY_STATE);
int change_hand(struct current_OPUS_edits_ *COE);
int change_note(int pressed_key, struct current_OPUS_edits_ *COE);
int change_chord_or_bar(int pressed_key, struct current_OPUS_edits_ *COE, const Uint8 *KEY_STATE);
int put_note_or_pause(int pressed_key, struct current_OPUS_edits_ *COE, const Uint8 *KEY_STATE, const char *serial_key);
int del_note_chord_bar(struct current_OPUS_edits_ *COE, const Uint8 *KEY_STATE, int X_after_key);
int create_new_chord_bar(struct current_OPUS_edits_ *COE, const Uint8 *KEY_STATE, int X_after_key, const char *default_serial_key);


OPUS *create_new_OPUS(char chosen_key[2], int chosen_metre[2], OPUS *prev_opus);

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
int fprint_note(FILE *opus_text_file, NOTES *n);
int fprint_chord(FILE *opus_text_file, CHORDS *chord_to_print);
int fprint_bar(FILE *opus_text_file, BARS *bar_to_print);
int save_OPUS_as_TextFile(OPUS *OPUS_to_save);

#endif //LIBS_PI_PROJECTSDL2_LIBRARY_H
