# Piece Table Implementation

## Rough implementation of the piece table method
This project tries to build some basic API for the piece table method. The API created can then be used in the construction of an interactive text editor.
This part of the implementation deals only with the piece table(no text editor program provided!).
This implementation uses standard C(no third-party library required).

As of this day, creation, cursor move, add text, delete text, save to buffer and save to file functions are implemented.
The redo and undo operations are left to be implemented.

## Integrating the API in the text editor
This part is yet to be implemented; the UI part will be handled using ncurses C library.

How to install ncurses on Debian, Ubuntu:
```bash
sudo apt install libncurses5-dev libncursesw5-dev
```
