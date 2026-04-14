#include <GL/freeglut.h>
#include <cstdio>
#include <vector>

// Dimensões da Janela
const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;

// Definições para as caixas
const float CAIXA_TAMANHO = 60.0f;
const float CAIXA_ESPACAMENTO = 100.0f; // Espaçamento entre as caixas
const float CAIXA_POSICAO_Y = 100.0f;   // Distância do chão

enum TipoCaixa { CAIXA_VERMELHA, CAIXA_AZUL, CAIXA_VERDE, CAIXA_FANTASMA };

struct Cor {
  float r;
  float g;
  float b;
};

struct Caixa {
  TipoCaixa tipo;
  Cor cor;
  float x, y;
};

// Lista de caixas (fila)
std::vector<Caixa *> listaDeCaixas = {};

// Função para desenhar uma caixa
void desenharCaixa(float x, float y, Cor cor) {
  // Converte as coordenadas de pixel para
  // coordenadas normalizadas do OpenGL (-1 a 1)
  float normalizedX = (2.0f * x / WINDOW_WIDTH) - 1.0f;
  float normalizedY = (2.0f * y / WINDOW_HEIGHT) - 1.0f;

  // Convertendo o tamanho da caixa de pixels para coordenadas normalizadas
  float normalizedSize = (2.0f * CAIXA_TAMANHO / WINDOW_WIDTH);

  glColor3f(cor.r, cor.g, cor.b);

  glBegin(GL_QUADS);
  // Desenhar o quadrado da caixa
  glVertex2f(normalizedX - normalizedSize / 2,
             normalizedY - normalizedSize / 2);
  glVertex2f(normalizedX + normalizedSize / 2,
             normalizedY - normalizedSize / 2);
  glVertex2f(normalizedX + normalizedSize / 2,
             normalizedY + normalizedSize / 2);
  glVertex2f(normalizedX - normalizedSize / 2,
             normalizedY + normalizedSize / 2);
  glEnd();

  // Desenhar borda preta
  glColor3f(0.0f, 0.0f, 0.0f);
  glLineWidth(2.0f);
  glBegin(GL_LINE_LOOP);
  glVertex2f(normalizedX - normalizedSize / 2,
             normalizedY - normalizedSize / 2);
  glVertex2f(normalizedX + normalizedSize / 2,
             normalizedY - normalizedSize / 2);
  glVertex2f(normalizedX + normalizedSize / 2,
             normalizedY + normalizedSize / 2);
  glVertex2f(normalizedX - normalizedSize / 2,
             normalizedY + normalizedSize / 2);
  glEnd();
  glLineWidth(1.0f);
}

// Função para desenhar todas as caixas no ecrã (máximo 3)
void drawAllBoxes() {
  // Mostra os primeiros 3 elementos da lista
  // Posição 0 (centro-inferior): índice 0 da lista
  // Posição 1 (meio): índice 1 da lista
  // Posição 2 (esquerda): índice 2 da lista

  int tamanhoCaixa = listaDeCaixas.size();
  int caixasVisiveis = (tamanhoCaixa > 3) ? 3 : tamanhoCaixa;

  // Calcular a posição central (centro inferior do ecrã em coordenadas)
  float centroX = WINDOW_WIDTH / 2.0f;
  float baseY = CAIXA_POSICAO_Y;

  // Desenhar as caixas visiveis
  for (int i = 0; i < caixasVisiveis; i++) {
    // Posição: centro, depois esquerda, depois mais à esquerda
    float posX = centroX - (i * CAIXA_ESPACAMENTO);

    desenharCaixa(posX, baseY, listaDeCaixas[i]->cor);
  }
}

// Função de exibição
void display() {
  glClear(GL_COLOR_BUFFER_BIT);

  drawAllBoxes();

  glutSwapBuffers();
}

// Função de teclado para adicionar ou remover caixas
void keyboard(unsigned char key, int x, int y) {
  switch (key) {
  case '1': {
    Caixa *caixaVermelha = new Caixa();
    caixaVermelha->tipo = CAIXA_VERMELHA;
    caixaVermelha->cor = {1.0f, 0.0f, 0.0f};
    caixaVermelha->x = -1.0f;

    listaDeCaixas.push_back(caixaVermelha);

    printf("Caixa Vermelha adicionada (Total: %lu)\n", listaDeCaixas.size());
    break;
  }
  case '2': {
    Caixa *caixaAzul = new Caixa();
    caixaAzul->tipo = CAIXA_AZUL;
    caixaAzul->cor = {0.0f, 0.0f, 1.0f};
    caixaAzul->x = -1.0f;

    listaDeCaixas.push_back(caixaAzul);

    printf("Caixa Azul adicionada (Total: %lu)\n", listaDeCaixas.size());
    break;
  }
  case '3': {
    Caixa *caixaVerde = new Caixa();
    caixaVerde->tipo = CAIXA_VERDE;
    caixaVerde->cor = {0.0f, 1.0f, 0.0f};
    caixaVerde->x = -1.0f;

    listaDeCaixas.push_back(caixaVerde);

    printf("Caixa Verde adicionada (Total: %lu)\n", listaDeCaixas.size());
    break;
  }
  // Remover a caixa mais antiga
  case '4': {
    if (!listaDeCaixas.empty()) {
      delete listaDeCaixas.front();
      listaDeCaixas.erase(listaDeCaixas.begin());
      printf("Caixa removida (Total: %lu)\n", listaDeCaixas.size());
    } else {
      printf("Nenhuma caixa para remover!\n");
    }
    break;
  }
  case 27: // tecla ESC
    exit(0);
    break;
  }

  glutPostRedisplay();
}

int main(int argc, char **argv) {
  // Initialize GLUT
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
  glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);

  // Center the window on the screen
  int screenWidth = glutGet(GLUT_SCREEN_WIDTH);
  int screenHeight = glutGet(GLUT_SCREEN_HEIGHT);
  glutInitWindowPosition((screenWidth - WINDOW_WIDTH) / 2,
                         (screenHeight - WINDOW_HEIGHT) / 2);

  glutCreateWindow("Sort the Boxes or be Fired");

  // Mudar a cor de fundo para cinzento claro
  glClearColor(0.9f, 0.9f, 0.9f, 1.0f);

  // Registar as funções de callback
  glutDisplayFunc(display);
  glutKeyboardFunc(keyboard);

  glutMainLoop();
  return 0;
}
