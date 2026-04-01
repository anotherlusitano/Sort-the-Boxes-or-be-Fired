#include "keys.h"

// Called by GLUT whenever a key is pressed.
// 'key' is the ASCII character; 'x' and 'y' are the mouse cursor position
// at the time of the event (unused for now, required by the GLUT callback signature).
void keyboard(unsigned char key, int x, int y) {
    // Suppress unused-parameter warnings for x and y
    (void)x;
    (void)y;
    switch (key) {
        // Move forward
        case 'w':
        case 'W':
            break;

        // Move left
        case 'a':
        case 'A':
            break;

        // Move backward
        case 's':
        case 'S':
            break;

        // Move right
        case 'd':
        case 'D':
            break;
    }
}
