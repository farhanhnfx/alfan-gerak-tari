#include <alfan_gerak_tari/utils/terminal_helper.h>

#include <termios.h> // For termios structure and related functions
#include <unistd.h>  // For STDIN_FILENO and other POSIX functions
#include <cstdlib>   // For atexit function
#include <cstdio>    // For printf function
#include <iostream>  // For cout

// Terminal state
termios originalTermios;

bool TerminalHelper::is_running = false;
int TerminalHelper::key_pressed = 0;
void TerminalHelper::setNonBlockingInput() {
    termios term;
    tcgetattr(STDIN_FILENO, &term);
    term.c_lflag &= ~ICANON;  // Disable canonical mode
    term.c_lflag &= ~ECHO;   // Disable echo
    tcsetattr(STDIN_FILENO, TCSANOW, &term);
}

void TerminalHelper::saveOriginalTerminal() {
    tcgetattr(STDIN_FILENO, &originalTermios);
}

void TerminalHelper::resetTerminal() {
    tcsetattr(STDIN_FILENO, TCSANOW, &originalTermios);
}

void TerminalHelper::buildTerminalLoop(void (*function)()) {
    saveOriginalTerminal();
    is_running = true;

    atexit(resetTerminal);
    
    ((void (*)())function)();
    printf("\n");

    while (is_running) {
        key_pressed = getKeyPress();
        if (key_pressed != -1) {
            std::cout << "Key pressed: " << char(key_pressed) << std::endl;
            if (key_pressed == 'q') {
                printf("Quit...\n");
                is_running = false;
                break;
            }

            ((void (*)())function)();
            printf("\n");
        }
    }

    resetTerminal();
    key_pressed = 0;
}
int TerminalHelper::getKeyPress() {
    setNonBlockingInput();
    int ch = -1;
    struct timeval tv = {0, 0};
    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(STDIN_FILENO, &readfds);
    if (select(STDIN_FILENO + 1, &readfds, NULL, NULL, &tv) > 0) {
        ch = getchar();
    }
    return ch;
}

