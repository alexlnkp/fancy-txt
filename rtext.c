#include "font8x8/font8x8.h"

#define NCURSES_WIDECHAR 1

#include <stdlib.h>
#include <string.h>
#include <ncurses.h>
#include <locale.h>

#define PREMISE_STR "TextDisplay"
#define USAGE_STR "Usage: "

const wchar_t* DRAW_CHAR = L"█";
const wchar_t* EMPTY_CHAR = L" ";

#define BITMAP_RES 8

void usage(char* name, size_t rows, size_t cols) {
    size_t center = (cols >> 1);
    mvprintw(0, center - strlen(PREMISE_STR), PREMISE_STR);
    mvprintw(1, center - (strlen(USAGE_STR) + 3 + strlen(name)), USAGE_STR "%s <TEXT>", name);
}

void render(size_t start_x, size_t start_y, size_t n_char, char *bitmap) {
    bool set;

    for (size_t y = 0; y < BITMAP_RES; ++y) {
        for (size_t x = 0; x < BITMAP_RES; ++x) {
            set = bitmap[y] & 1 << x;
            mvprintw(y + start_y, x + start_x + n_char * BITMAP_RES, "%ls", set ? DRAW_CHAR : EMPTY_CHAR);
        }
    }
}

void draw_text(char* restrict text, size_t cols, size_t rows) {
    size_t arglen = strlen(text);
    for (size_t i = 0; i < arglen; ++i) {
        size_t center_x = (cols - (arglen * BITMAP_RES)) >> 1;
        size_t center_y = (rows - BITMAP_RES) >> 1;
        render(center_x, center_y, i, font8x8_basic[text[i]]);
    }
}

char* get_text_from_argv(size_t argc, char* argv[]) {
    size_t total_length = 0;

    // start from 1 since we don't care about program name
    for (size_t i = 1; i < argc; ++i) {
        total_length += strlen(argv[i]) + 1; // +1 for each space between an arg plus '\0'
    }

    char* result = malloc(total_length * sizeof(char));
    if (result == NULL) { return NULL; }

    char* current = result;

    // start from 1 since we don't care about program name
    for (size_t i = 1; i < argc; ++i) {
        size_t arg_length = strlen(argv[i]);
        memcpy(current, argv[i], arg_length);

        // move the pointer to the end of the current argument
        current += arg_length;

        // add a space between arguments, except for the last one
        if (i < argc - 1) {
            *current = ' '; ++current;
        }
    }

    // add a null terminator at the end of the result buffer
    *current = '\0';

    return result;
}

int main(int argc, char* argv[]) {
    setlocale(LC_ALL, "");

    int8_t __exit_code = 0;

    size_t rows = 0; size_t cols = 0;

    initscr();
    getmaxyx(stdscr, rows, cols);

    curs_set(0); clear();

    if (argc < 2) {
        usage(argv[0], rows, cols);
        __exit_code = -1; goto exit;
    } else {
        draw_text(get_text_from_argv(argc, argv), cols, rows);
    }

exit:
    getch(); endwin(); curs_set(1);
    return __exit_code;
}