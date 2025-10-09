# Piece Table Implementation

## Rough implementation of the piece table method
This project tries to build some basic API for the piece table method. The API created can then be used in the construction of an interactive text editor.
This part of the implementation deals only with the piece table(no text gui program provided!).
This implementation uses standard C(no third-party library required).

As of this day, creation, cursor move, add text, delete text, save to buffer and save to file functions are implemented.
The redo and undo operations are left to be implemented.

## Integrating the API in the text editor
The UI part is handled using ncurses C library.
The UI is not perfect, it has some bugs when the max length of a line is reached.
Files modified can be saved with ctrl+s.

How to install ncurses on Debian, Ubuntu:
```bash
sudo apt install libncurses5-dev libncursesw5-dev
```
