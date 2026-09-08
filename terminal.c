#include <asm-generic/ioctls.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>

struct termios original_termios;

void getWindowSize(short *row, short *col) {
    struct winsize ws;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1) {
        *row = 24;
        *col = 80;
        return;
    }
    *row = ws.ws_row - 1;
    *col = ws.ws_col;
}

void disableRawMode() {
    // "\x1b[?1049l" return to main screen
    write(STDOUT_FILENO, "\x1b[?1049l\x1b[?25h", 14);
    tcsetattr(STDOUT_FILENO, TCSANOW, &original_termios);
}

void enableRawMode() {
    tcgetattr(STDIN_FILENO, &original_termios);
    atexit(disableRawMode);

    struct termios raw = original_termios;
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 1;
    // raw.c_lflag &= ~(ECHO | ICANON);

    /////////////////
    // ADDED: IXON (disables Ctrl+S/Ctrl+Q flow control)
    raw.c_iflag &= ~(IXON);

    // ADDED: ISIG (disables Ctrl+C/Ctrl+Z signals)
    raw.c_lflag &= ~(ECHO | ICANON | ISIG);
    /////////////////

    tcsetattr(STDIN_FILENO, TCSANOW, &raw);

    // switch to alternate screen "\x1b[?1049h" 8
    // emptyscreen "\x1b[2J" 4
    // go to top left "\x1b[H" 3
    write(STDOUT_FILENO, "\x1b[?1049h\x1b[2J\x1b[H", 15);
}
