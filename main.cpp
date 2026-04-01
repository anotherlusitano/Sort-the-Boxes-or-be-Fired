#include <GL/freeglut.h>
#include "keys.h"

// Window dimensions in pixels
const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;

// Called by GLUT on every frame redraw.
// Clears the screen and swaps the front and back buffers (double buffering).
void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    glutSwapBuffers();
}

int main(int argc, char** argv) {
    // Initialize GLUT with command-line arguments
    glutInit(&argc, argv);

    // Use double buffering and RGB color mode
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);

    // Set the initial window size
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);

    // Center the window on the screen
    int screenWidth = glutGet(GLUT_SCREEN_WIDTH);
    int screenHeight = glutGet(GLUT_SCREEN_HEIGHT);
    glutInitWindowPosition((screenWidth - WINDOW_WIDTH) / 2, (screenHeight - WINDOW_HEIGHT) / 2);

    // Create the application window with its title
    glutCreateWindow("Sort the Boxes or be Fired");

    // Register the display callback (called each frame)
    glutDisplayFunc(display);

    // Register the keyboard callback (handles W, A, S, D input)
    glutKeyboardFunc(keyboard);

    // Enter the GLUT event loop (runs until the window is closed)
    glutMainLoop();
    return 0;
}
