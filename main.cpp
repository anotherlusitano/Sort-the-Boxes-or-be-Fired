#include <GL/freeglut.h>

// Window dimensions
const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;

// Handles the display
void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    glutSwapBuffers();
}

// Handles the keys to control the robotic arm
void keyboard(unsigned char key, int x, int y) {
    switch (key) {
        case 'w':
        case 'W':
            break;
        case 'a':
        case 'A':
            break;
        case 's':
        case 'S':
            break;
        case 'd':
        case 'D':
            break;
    }
}

int main(int argc, char** argv) {
    // Initialize GLUT
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);

    // Center the window on the screen
    int screenWidth = glutGet(GLUT_SCREEN_WIDTH);
    int screenHeight = glutGet(GLUT_SCREEN_HEIGHT);
    glutInitWindowPosition((screenWidth - WINDOW_WIDTH) / 2, (screenHeight - WINDOW_HEIGHT) / 2);

    glutCreateWindow("Sort the Boxes or be Fired");

    // Register callbacks
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);

    glutMainLoop();
    return 0;
}
