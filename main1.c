// This file is used for testing.
#include <stdio.h>
#include "piece_table.h"

int main() {
    Piece_Table *table= create_table("file", NULL, 0);
    printf("%s\n", table->original_buffer);

    char *final = NULL;

    advance_cursor(table, 0);
    add_text(table, "CCC");
    add_text(table, "D");
    // advance_cursor(table, -1);
    // add_text(table, "DDD");
    // add_text(table, "AAA");
    // advance_cursor(table, -2);
    // add_text(table, "ZZZ");
    // advance_cursor(table, 1);
    // advance_cursor(table, 9);
    // add_text(table, "ZZZZ");
    // advance_cursor(table, -3);
    // delete_text(table, 9);
    // advance_cursor(table, 3);
    // delete_text(table, 2);
    final = extract_current_text(table);
    save_current_text(table, "file.out", NULL, NULL);
    printf("%s\n", final);

    return 0;
}