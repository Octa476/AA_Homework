// This file provides the implementation for the Piece Table method.
// The API is implemented in this file.

#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "piece_table.h"

Piece_Table *create_table(const char *filename, const char *text, int global_cursor) {
    Piece_Table *table = (Piece_Table *)calloc(1, sizeof(Piece_Table));
    int ret = 0;

    // Create and populate the original buffer.
    // Filename provided.
    if (filename) {
        int fd = open(filename, O_CREAT | O_RDONLY, 0644);
        if (fd < 0) {
            fprintf(stderr, "The file could not be opened!\n");
        }
        struct stat status;
        fstat(fd, &status);
        int size = status.st_size;
        table->original_buffer = (char *)malloc((size + 1) * sizeof(char));
        if (!table->original_buffer) {
            fprintf(stderr, "Malloc failed!\n");
            exit(1);
        }
        table->original_size = size + 1;
        ret = read(fd, table->original_buffer, table->original_size);
        if (ret != size) {
            fprintf(stderr, "Something went wrong!\n");
            exit(1);
        }
        table->original_buffer[size] = 0;
        close(fd);
    } else if (text) {
        // Text buffer provided.
        int size = strlen(text);
        table->original_buffer = (char *)malloc((size + 1) * sizeof(char));
        if (!table->original_buffer) {
            fprintf(stderr, "Malloc failed!\n");
            exit(1);
        }
        table->original_size = size + 1;
        memcpy(table->original_buffer, text, table->original_size);
        table->original_buffer[size] = 0;
    } else {
        fprintf(stderr, "No filename or text buffer provided!\n");
        exit(1);
    }
    printf("%d\n", table->original_size);
    printf("%s\n", table->original_buffer);

    // Create add buffer.
    table->add_buffer = (char *)malloc(INIT_ADD_SIZE * sizeof(char));
    if (!table->add_buffer) {
        fprintf(stderr, "Malloc failed!\n");
        exit(1);
    }
    table->add_size = INIT_ADD_SIZE;
    table->add_len = 0;

    // The position of the global cursor when the file is opened.
    table->global_cursor = global_cursor;
    table->total_len = table->original_size - 1;

    // Create the sentinel.
    table->current_root = create_piece_cell(-1, -1, NULL);
    // Pieces points to sections without string terminators.
    // Insert the first piece.
    insert_piece(table->current_root, create_piece_cell(0, table->original_size - 1, table->original_buffer));
    if (table->global_cursor == table->total_len) {
        table->current_piece = table->current_root;
        table->piece_cursor = 0;    
    } else {
        table->current_piece = table->current_root->next;
        table->piece_cursor = table->global_cursor;
    }

    return table;
}

void advance_cursor(Piece_Table *table, int advance) {
    Piece_List *curr = table->current_piece;
    printf("%d\n", curr->length);
    printf("%d\n", table->total_len);
    printf("%d\n", table->global_cursor);
    printf("%d\n", table->piece_cursor);
    printf("%d\n", advance);
    int piece_cursor = table->piece_cursor;
    if ((table->total_len < table->global_cursor + advance) || (table->global_cursor + advance < 0)) {
        fprintf(stderr, "Invalid cursor move!\n");
        exit(1);
    }
    if (advance > 0) {
        table->global_cursor += advance;
        while (curr->length - piece_cursor < advance) {
            advance -= (curr->length - piece_cursor);
            curr = curr->next;
            piece_cursor = 0;
        }
        if (curr->length - piece_cursor == advance) {
            table->current_piece = curr->next;
            table->piece_cursor = 0;
        } else {
            table->current_piece = curr;
            table->piece_cursor += advance;
        }

        printf("Cursor: %d\n", table->piece_cursor);
        printf("%d\n", table->current_piece->start);
    } else if (advance < 0) {
        table->global_cursor += advance;
        advance = abs(advance);
        while (piece_cursor < advance) {
            if (piece_cursor == 0) {
                advance--;
                curr = curr->prev;
                piece_cursor = curr->length - 1;
            } else {
                advance -= piece_cursor;
                piece_cursor = 0;
            }
            printf("%d ", piece_cursor);
        }
        table->current_piece = curr;
        table->piece_cursor = piece_cursor - advance;
        printf("Cursor: %d\n", table->piece_cursor);
    }
}

int show_global_cursor(Piece_Table *table) {
    return table->global_cursor;
}

int show_total_len(Piece_Table *table) {
    return table->total_len;
}

void add_text(Piece_Table *table, char *text) {
    int size = strlen(text);
    if (!size)
        return;
    table->total_len += size;

    // Add the new text to the add_buffer.
    // Reallocation of the add_buffer is triggered.
    if (size + table->add_len >= table->add_size) {
        table->add_buffer = (char *)realloc(table->add_buffer, 2 * table->add_size);
        if (!table->add_buffer) {
            fprintf(stderr, "Malloc failed!\n");
            exit(1);
        }
        table->add_size = 2 * table->add_size;
    }

    memcpy(table->add_buffer + table->add_len, text, size + 1);
    table->add_len += size;

    printf("okk\n");
    Piece_List *new_cell = create_piece_cell(table->add_len - size, size, table->add_buffer);

    // Insertion without fragmentation.
    if (table->piece_cursor == 0) {
        printf("okkkkk\n");
        Piece_List *prev = table->current_piece->prev;
        printf("%d\n", prev->length);
        insert_piece(prev, new_cell);
    } else {
        // Insertion with fragmentation.
        Piece_List *prev = table->current_piece->prev;
        Piece_List *curr = table->current_piece;
        int frag = table->piece_cursor;
        Piece_List *new_frag = create_piece_cell(curr->start, frag, curr->source);
        curr->start += frag;
        curr->length -= frag;
        insert_piece(prev, new_frag);
        insert_piece(new_frag, new_cell);
        table->piece_cursor = 0;
    }
    table->global_cursor += size;
}

void delete_text(Piece_Table *table, int length) {
    if (table->global_cursor < length) {
        fprintf(stderr, "Invalid delete, too many characters to delete!\n");
        exit(1);
    }
    table->global_cursor -= length;

    Piece_List *curr = table->current_piece;
    int piece_cursor = table->piece_cursor;

    if (!length)
        return;

    int end_cursor = table->piece_cursor;
    Piece_List *end_piece = table->current_piece;

    while (piece_cursor < length) {
        length -= (piece_cursor + 1);
        curr = curr->prev;
        // Deallocation required.

        piece_cursor = curr->length - 1;
    }

    // No fragmentation.
    if (piece_cursor == length) {
        printf("Fara fragmentare\n");
        Piece_List *prev = curr->prev;

        // Deallocation required.
        if (curr != end_piece) {

        }

        prev->next = end_piece;
        end_piece->prev = prev;
    } else if (end_cursor == 0) {
        printf("Aici\n");
        printf("%d\n", piece_cursor);
        // Fragmentation when the cursor is placed at the begging of the current piece.
        curr->length = piece_cursor - length;
        curr->next = end_piece;
        end_piece->prev = curr;
    } else {
        // Fragmentation.
        printf("Fragmentare\n");
        Piece_List *prev = curr->prev;

        // Fragmentation in the end_piece.
        if (curr == end_piece) {
            Piece_List *new_piece = (Piece_List *)malloc(sizeof(Piece_List));
            new_piece->source = curr->source;
            new_piece->start = curr->start;
            new_piece->length = piece_cursor - length;

            new_piece->prev = prev;
            new_piece->next = end_piece;
            
            end_piece->prev = new_piece;
            prev->next = new_piece;
        } else {
            printf("Aicea\n");
            // Fragmentation in a previous piece.
            // No deallocation required.
            curr->length -= (length + 1);
            curr->next = end_piece;
            end_piece->prev = curr;
        }

    }

    end_piece->length -= end_cursor;
    end_piece->start += end_cursor;
    table->piece_cursor = 0;
    table->current_piece = end_piece;
}

// Terminator included.
char *extract_current_text(Piece_Table *table) {
    char *final_text = (char *)calloc((table->total_len + 1), sizeof(char));
    int len = 0;
    Piece_List *curr = table->current_root->next;

    while (curr != table->current_root) {
        memcpy(final_text + len, curr->source + curr->start, curr->length);
        len += curr->length;
        curr = curr->next;
    }

    final_text[table->total_len] = 0;
    return final_text;
}

void save_current_text(Piece_Table *table, const char *filename, char **text, int *global_cursor) {
    int ret = 0;
    if (global_cursor)
        *global_cursor = table->global_cursor;
    char *final_text = extract_current_text(table);
    int size = strlen(final_text);

    if (filename) {
        int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (fd < 0) {
            fprintf(stderr, "The file could not be opened!\n");
        }
        
        ret = write(fd, final_text, size);
        if (ret != size) {
            fprintf(stderr, "Something went wrong!\n");
            exit(1);
        }

        close(fd);
    } else if (text) {
        *text = final_text;
    } else {
        fprintf(stderr, "No filename or text buffer provided!\n");
        exit(1);
    }
}

Piece_List *create_piece_cell(int start, int length, char *source) {
    Piece_List *new_cell = (Piece_List *)calloc(1, sizeof(Piece_List));
    if (!new_cell) {
        fprintf(stderr, "Calloc failed!\n");
        exit(1);
    }
    new_cell->start = start;
    new_cell->length = length;
    new_cell->source = source;

    new_cell->next = new_cell;
    new_cell->prev = new_cell;

    return new_cell;
}

// Insert a new piece after cell.
void insert_piece(Piece_List *cell, Piece_List *new_cell) {
    Piece_List *next = cell->next;
    new_cell->prev = cell;
    new_cell->next = next;

    cell->next = new_cell;
    next->prev = new_cell;
}


