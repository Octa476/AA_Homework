// This file contains the structs and function declarations needed for the Piece Table method API.

#define INIT_ADD_SIZE 1024

typedef struct TPiece_List Piece_List;
typedef struct TPiece_Table TPiece_Table;

// Sentinel list for easy checking.
typedef struct TPiece_List {
    int start;
    int length;
    char *source;

    struct TPiece_List *next;
    struct TPiece_List *prev;
} Piece_List;

typedef struct TPiece_Table {
    // Original fixed buffer.
    char *original_buffer; // Terminator included.
    int original_size; // Terminator included.

    // Append only buffer.
    char *add_buffer; // Terminator included.
    int add_size;
    int add_len; // Terminator included.

    // Current piece list.
    Piece_List *current_root;
    int total_len; // No terminator included. 

    // Cursor position within the current text buffer.
    int global_cursor; // No terminator included.

    // Cursor position within the current Piece cell.
    int piece_cursor; // No terminator included.

    // Cursor piece list position.
    Piece_List *current_piece;

} Piece_Table;

// API functions //
// The time complexities for these operation in command mode(no GUI integrated) are listed below.
// O(N) - N is the length of the text.
Piece_Table *create_table(const char *filename, const char *text, int global_cursor);
// O(K) - k - the number of pieces that need to be traversed.
void advance_cursor(Piece_Table *table, int advance);
// O(1) - the cursor position relative to the beginning of the file.
int show_global_cursor(Piece_Table *table);
// O(1) - returns the total length of the current displayed text.
int show_total_len(Piece_Table *table);
// O(N) - N - the length of the text.
void add_text(Piece_Table *table, char *text);
// O(1) - nothing is realy deleted, just a piece from a list.
void delete_text(Piece_Table *table, int length);
// O(N) - N - the length of the current displayed text.
char *extract_current_text(Piece_Table *table);
// O(N) - N - the length of the current displayed text.
void save_current_text(Piece_Table *table, const char *filename, char **text, int *global_cursor);

// Helper functions //
// O(1)
Piece_List *create_piece_cell(int start, int length, char *source);
// O(1)
void insert_piece(Piece_List *cell, Piece_List *new_cell);
