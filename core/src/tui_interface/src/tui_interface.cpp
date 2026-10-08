#include "tui_interface.hpp"

void TUI_Interface::clear_display() {
    printf("\x1B[2J\x1B[H");
}

void TUI_Interface::refresh_display(int numArgs, ...) {
    display_mutex.lock();

    va_list args;
    va_start(args, numArgs);

    tcflush(STDIN_FILENO, TCIFLUSH);
    current_input = "";
    cursor_pos = 0;
    num_read = 0;
    display_tui(args);
    display_mutex.unlock();
}

void TUI_Interface::insert_into_input(char c) {
    char new_val = c;
    int orig_pos = cursor_pos;
    while (cursor_pos < num_read) {
        char old_val = current_input[cursor_pos];
        current_input[cursor_pos] = new_val;
        printf("%c", new_val);
        fflush(stdout);
        cursor_pos++;
        new_val = old_val;
    }
    current_input.push_back(new_val);
    printf("%c", new_val);
    fflush(stdout);
    cursor_pos++;
    while (cursor_pos > orig_pos + 1) {
        write(STDOUT_FILENO, "\x1B[1D", 4);
        cursor_pos--;
    }
    num_read++;
}


void TUI_Interface::backspace() {
    int orig_pos = cursor_pos;
    write(STDOUT_FILENO, "\x1B[1D", 4); // move left to prepare for deletion
    while (cursor_pos < num_read) {
        char old_val = current_input[cursor_pos];
        current_input[cursor_pos - 1] = old_val;
        printf("%c", old_val);
        fflush(stdout);
        cursor_pos++;
    }
    current_input.pop_back();
    write(STDOUT_FILENO, " ", 1);
    while (cursor_pos >= orig_pos) {
        write(STDOUT_FILENO, "\x1B[1D", 4);
        cursor_pos--;
    }
    num_read--;
}

/* Here we make the totally not dangerous assumption that some identifiers don't matter. :) */
void TUI_Interface::delete_or_direction() {
    char c = 0;
    read(STDIN_FILENO, &c, 1);
    read(STDIN_FILENO, &c, 1);
    if (c == 51) { // delete key
        read(STDIN_FILENO, &c, 1);
        if (c == 126 && cursor_pos < num_read) { // standard delete
            write(STDOUT_FILENO, "\x1B[1C", 4);
            cursor_pos++;
            backspace();
        }
        if (c == 59) { // ctrl + delete
            read(STDIN_FILENO, &c, 1); // clear extra identifier
            read(STDIN_FILENO, &c, 1); // clear extra identifier
            write(STDOUT_FILENO, "\x1B[0K", 4); // erase from cursor to end of line
            num_read -= (current_input.size() - cursor_pos);
            current_input.erase(cursor_pos, std::string::npos);
        }
    }
    else { // direction key
        if (c == 67 && cursor_pos < num_read) { // right
            write(STDOUT_FILENO, "\x1B[1C", 4);
            cursor_pos++;
        }
        if (c == 68 && cursor_pos > 0) { // left
            write(STDOUT_FILENO, "\x1B[1D", 4);
            cursor_pos--;
        }
    }
}

/*
 * Known limitations:
    * ctrl + arrow keys missing
    * page up / page down missing
    * line wrapping missing and breaks backspace
    * delete_or_direction() doesn't check full scancodes
*/

void TUI_Interface::process_input() {
    display_mutex.lock();
    cursor_pos = 0;
    num_read = 0;
    int c = 0;
    current_input = "";
    display_mutex.unlock();
    while (true) {
	usleep(10000);
        while (read(STDIN_FILENO, &c, 1) != 0) {
            display_mutex.lock();
            if ((c >= 32 && c <= 126) || c == '\n') {
                insert_into_input(c);
                if (c == '\n') {
                    display_mutex.unlock();
                    return;
                }
            }
            if (c == 127 && cursor_pos > 0) {
                backspace();
            }
            if (c == 8) { // ctrl + backspace
                while (cursor_pos > 0) {
                    backspace();
                }
            }
            if (c == 27) {
                delete_or_direction();
            }
            display_mutex.unlock();
        }
    }
}

void TUI_Interface::init_terminal() {
    tcgetattr(STDIN_FILENO, &orig_termios);
    struct termios raw = orig_termios;
    raw.c_lflag &= ~(ICANON | ECHO);  // no line buffering or echoing
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 0;
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

void TUI_Interface::restore_terminal() {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}

void TUI_Interface::freeze_display() {
    display_mutex.lock();
}

void TUI_Interface::unfreeze_display() {
    display_mutex.unlock();
}

std::string TUI_Interface::get_current_input() {
    return current_input;
}

void TUI_Interface::clear_input() {
    current_input.clear();
    cursor_pos = 0;
}
