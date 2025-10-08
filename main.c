#include <ncurses.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include "piece_table.h"

#define MAX_LINES 10
#define MAX_COLS 50
FILE *log_file;

// typedef struct TGui {
//     int x;
//     int y;
//     int global_cursor;
//     int total_len;
//     int lines_lengths[MAX_LINES];
//     int num_lines;
// } Gui;

// Compute the positition (y, x) from the global cursor saved in the file(maybe).
// this feature is yet to be implemented(we don't have a way to store the global cursor into the file, yet).
void compute_pos_from_global_cursor(int *x, int *y, int global_cursor, int total_len, int *lines_lengths, int num_lines) {

}

void draw_border() {
    char border[MAX_COLS + 1] = {0};
    memset(border, '#', MAX_COLS);
    mvprintw(MAX_LINES, 0, "%s", border);

    for (int i = 0; i < MAX_LINES; i++)
        mvprintw(i, MAX_COLS, "%c", '#');
}

void compute_new_pos(int *x, int *y, int *global_cursor, int total_len, int move, int *lines_lengts, int num_lines) {
    if (move == 0) {
        fprintf(log_file, "left\n");
        fflush(log_file);
        // Left.
        if ((*global_cursor) == 0)
            return;

        *global_cursor = (*global_cursor) - 1;
        if (*x)
            *x = (*x) - 1;
        else {
            if ((*y) == 0)
                return;
            *y = (*y) - 1;
            *x = lines_lengts[*y] - 1;
        }
    } else if (move == 1) {
        // Right.
        fprintf(log_file, "global: %d\n", *global_cursor);
        fprintf(log_file, "total: %d\n", total_len);
        fflush(log_file);
        if ((*global_cursor == total_len)) {
            fprintf(log_file, "MAXXX");
            fflush(log_file);
            return;
        }
        fprintf(log_file, "%d\n", lines_lengts[*y]);
        fflush(log_file);
        *global_cursor = (*global_cursor) + 1;
        if (((*x) < lines_lengts[*y] - 1) || ((*y) == num_lines - 1))
            *x = (*x) + 1;
        else {
            if ((*y) == num_lines - 1)
                return;
            *y = (*y) + 1;
            *x = 0;
        }
    } else if (move == 2) {
        // Up.
        if ((*y) == 0)
            return;

        int new_y = (*y) - 1;
        if (lines_lengts[new_y] > (*x)) {
            *global_cursor = (*global_cursor) - (*x) - (lines_lengts[new_y] - (*x));
            *y = new_y;
        } else {
            *global_cursor = (*global_cursor) - (*x) - 1;
            *y = new_y;
            *x = lines_lengts[new_y] - 1;
        }
    } else if (move == 3) {
        // Down.
        if ((*y) == num_lines - 1)
            return;

        int new_y = (*y) + 1;
        if (lines_lengts[new_y] > (*x)) {
            *global_cursor = (*global_cursor) + (*x) + (lines_lengts[*y] - (*x));
            *y = new_y;
        } else {
            if (new_y == num_lines - 1) {
                *global_cursor = (*global_cursor) + (lines_lengts[*y] - (*x)) + lines_lengts[new_y];
                *x = lines_lengts[new_y];
            } else {
                *global_cursor = (*global_cursor) + (lines_lengts[*y] - (*x)) + lines_lengts[new_y] - 1;
                *x = lines_lengts[new_y] - 1;
            }
            *y = new_y;
        }
    }
    fprintf(log_file, "%d\n", *global_cursor);
    fflush(log_file);
}

void build_displayed_text(Piece_Table *table, char *displayed_text, int *lines_lengths, int *num_lines, int *total_len) {
    char *text = extract_current_text(table);
    fprintf(log_file, "%s\n", text);
    fflush(log_file);
    if (strlen(text) == 0) {
        displayed_text[strlen(text)] = 0;
        *num_lines = 1;
        lines_lengths[0] = 0;
        printf("%s", "NIMIC AICI\n");
        *total_len = 0;
        // free(text);
        return;
    }

    *total_len = strlen(text);

    // Populate the vector of line lengths.
    int num = 0;
    int j = 0;
    int k = 0;
    for (int i = 0; i < strlen(text); i++) {
        if (num == MAX_COLS) {
            displayed_text[k++] = '\n';
            lines_lengths[j++] = num;
            num = 0;
        }
        if (text[i] == '\n') {
            lines_lengths[j++] = num + 1;
            num = 0;
        } else {
            num++;
        }
        displayed_text[k++] = text[i];
    }
    displayed_text[strlen(text)] = 0;

    if (text[strlen(text) - 1] != '\n') {
        lines_lengths[j] = num;
        *num_lines = j + 1;
    } else {
        lines_lengths[j] = 0;
        *num_lines = j + 1;
    }
    free(text);
}

int add_text_wrapper(Piece_Table *table, char c, int num_lines, int *lines_lengths) {
    char add[2] = {0};
    add[0] = c;
    if ((num_lines == MAX_LINES) && (lines_lengths[num_lines - 1] == MAX_COLS))
        return 1;
    if ((c == '\n') && (num_lines == MAX_LINES))
        return 1;
    add_text(table, add);
    return 0;
}

int delete_text_wrapper(Piece_Table *table) {
    if (show_global_cursor(table) == 0)
        return 1;
    fprintf(log_file, "delete\n");
    fflush(log_file);
    delete_text(table, 1);
    return 0;
}

int main(int argc, char *argv[]) {
    // Open a file in writing mode
    log_file = fopen("log_file", "wt");
    fprintf(log_file, "Start log_file\n");
    fflush(log_file);
    if (argc != 2) {
        fprintf(stderr, "Provide just the filename!\n");
        exit(1);
    }

    char *filename = argv[1];  
    int global_cursor = 0;
    int total_len = 0;
    int lines_lengths[MAX_LINES] = {0}; // The number of characters on every line.
    int num_lines = 0;

    // As we don't have a method to save the previous global cursor, yet
    // The global cursor will be 0 by default.
    Piece_Table *table = create_table(filename, NULL, 0);
    global_cursor = show_global_cursor(table);
    total_len = show_total_len(table);
    // char *text = extract_current_text(table);

    char displayed_text[MAX_COLS * MAX_LINES + 1] = {0};

    // stores the current position
    // As the global cursor will be 0 as default at the beginning for the moment,
    // The postitions will be 0 as well.
	int x = 0;
    int y = 0;

    // // Populate the vector of line lengths.
    // int num = 0;
    // int j = 0;
    // for (int i = 0; i < strlen(text); i++) {
    //     if (text[i] == '\n') {
    //         lines_lengts[j++] = num + 1;
    //         num = 0;
    //     } else {
    //         num++;
    //     }
    // }
    // if (text[strlen(text) - 1] != '\n') {
    //     lines_lengts[j] = num;
    //     num_lines = j + 1;
    // } else {
    //     num_lines = j;
    // }

    // printf("%d\n", global_cursor);
    build_displayed_text(table, displayed_text, lines_lengths, &num_lines, &total_len);
    printf("UITE\n");
    printf("%d\n", total_len);
    printf("%d\n", num_lines);
    printf("%d\n", lines_lengths[0]);
    printf("%s %d\n", displayed_text, strlen(displayed_text));
    // while (1) {}


	initscr();            // Start ncurses mode
	raw();                // Disable line buffering
	keypad(stdscr, TRUE); // Enable arrow keys
	noecho();             // Don't echo typed chars
	curs_set(2);          // Show cursor

    int change = 1;
	while (1) {
        int old_global_cursor = global_cursor;
        clear();
        
		// Display buffer using the function:
		// mvprintw
		// /* to be deleted:
        // if (change) {
            mvprintw(0, 0, "%s", displayed_text);
            draw_border();
            change = 0;
        // }
		// */

		move(y, x); // Move cursor to current position
		refresh();

		int ch = getch();

		if (ch == 3) { // Ctrl-C
			endwin();  // restore terminal
			raise(SIGINT);  // behave like normal Ctrl-C
		}
		else if (ch == 26) { // Ctrl-Z
			endwin();
			raise(SIGTSTP);  // suspend like normal Ctrl-Z
		}
		else if (ch == 17) { // Ctrl-Q to quit
			break;
        } else if (ch == 19) { // Ctrl-S to save to current file
            save_current_text(table, filename, NULL, NULL);
            advance_cursor(table, global_cursor - old_global_cursor);
		} else if (ch == KEY_UP) {
            compute_new_pos(&x, &y, &global_cursor, total_len, 2, lines_lengths, num_lines);
            advance_cursor(table, global_cursor - old_global_cursor);
        } else if (ch == KEY_DOWN) {
            compute_new_pos(&x, &y, &global_cursor, total_len, 3, lines_lengths, num_lines);
            advance_cursor(table, global_cursor - old_global_cursor);
        } else if (ch == KEY_LEFT) {
            compute_new_pos(&x, &y, &global_cursor, total_len, 0, lines_lengths, num_lines);
            advance_cursor(table, global_cursor - old_global_cursor);
        } else if (ch == KEY_RIGHT) {
            compute_new_pos(&x, &y, &global_cursor, total_len, 1, lines_lengths, num_lines);
            advance_cursor(table, global_cursor - old_global_cursor);
        } else if (ch == KEY_BACKSPACE) {
            // Delete.
            int ret = delete_text_wrapper(table);
            if (!ret) {
                compute_new_pos(&x, &y, &global_cursor, total_len, 0, lines_lengths, num_lines);
            }
            build_displayed_text(table, displayed_text, lines_lengths, &num_lines, &total_len);
            // change = 1;
        } else {
            // Add.
            int moves = 1;
            int ret = add_text_wrapper(table, ch, num_lines, lines_lengths);
            build_displayed_text(table, displayed_text, lines_lengths, &num_lines, &total_len);
            if (!ret) {
                fprintf(log_file, "TAB: %d\n", ch);
                fflush(log_file);
                if (ch == 9)
                    moves = TABSIZE;
                for (int k = 0; k < moves; k++)
                    compute_new_pos(&x, &y, &global_cursor, total_len, 1, lines_lengths, num_lines);
            }
            // change = 1;
        }

        // Advance the table's internal cursor.
        fprintf(log_file, "%d %d\n", global_cursor, old_global_cursor);

        fprintf(log_file, "Intern: %d\n", table->global_cursor);
        fprintf(log_file, "Advance: %d\n", global_cursor - old_global_cursor);
        fflush(log_file);
	}

    fclose(log_file);
	endwin(); // Exit ncurses mode
	return 0;
}