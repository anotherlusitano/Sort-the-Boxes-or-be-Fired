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

// Variáveis que guardam o ângulo de cada articulação
float anguloOmbro = 180.0f;
float anguloCotovelo = 0.0f;
float anguloMao = 0.0f;

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

void desenharRobo() {
  glPushMatrix();
  // Movemos o robô um pouco para a esquerda para caber no ecrã
  // e pegar apenas a primeira caixa
  glTranslatef(0.35f, 0.1f, 0.0f);

  // ==========================================
  // 1. O OMBRO (O "Pai")
  // ==========================================
  // Roda o ombro (e tudo o que vier depois)
  glRotatef(anguloOmbro, 0.0f, 0.0f, 1.0f);

  glColor3f(0.2f, 0.5f, 0.8f); // Azul
  glBegin(GL_POLYGON);
  glVertex2f(0.0f, -0.05f); // Começa na origem (0,0)
  glVertex2f(0.4f, -0.05f); // Comprimento do ombro: 0.4
  glVertex2f(0.4f, 0.05f);
  glVertex2f(0.0f, 0.05f);
  glEnd();

  // ==========================================
  // 2. O COTOVELO (O "Filho")
  // ==========================================
  // Movemos o eixo para a ponta do ombro (0.4) para o cotovelo nascer lá
  glTranslatef(0.4f, 0.0f, 0.0f);

  // A GAVETA MÁGICA: Guardamos o estado aqui para que a rotação do cotovelo não
  // afete o que vier depois!
  glPushMatrix();

  glRotatef(anguloCotovelo, 0.0f, 0.0f, 1.0f); // Roda apenas o cotovelo

  glColor3f(0.8f, 0.2f, 0.2f); // Vermelho
  glBegin(GL_POLYGON);
  glVertex2f(0.0f, -0.04f);
  glVertex2f(0.3f, -0.04f); // Comprimento do cotovelo: 0.3
  glVertex2f(0.3f, 0.04f);
  glVertex2f(0.0f, 0.04f);
  glEnd();

  // ==========================================
  // 3. A MÃO (O "Neto")
  // ==========================================
  glTranslatef(0.3f, 0.0f, 0.0f);
  glRotatef(anguloMao, 0.0f, 0.0f, 2.0f); // Roda apenas a MÃO

  glColor3f(0.0f, 0.5f, 0.0f); // Vermelho
  glBegin(GL_POLYGON);
  glVertex2f(0.0f, -0.05f);
  glVertex2f(0.1f, -0.05f); // Comprimento do cotovelo: 0.1
  glVertex2f(0.1f, 0.05f);
  glVertex2f(0.0f, 0.05f);
  glEnd();

  // ==========================================
  // 4. OS DEDOS (Os "Bisnetos")
  // ==========================================
  glTranslatef(0.1f, 0.025f, 0.0f);
  glColor3f(0.5f, 0.1f, 0.1f); // Vermelho

  glBegin(GL_POLYGON);
  glVertex2f(0.0f, -0.025f);
  glVertex2f(0.05f, -0.01f); // Comprimento do cotovelo: 0.1
  glVertex2f(0.05f, 0.01f);
  glVertex2f(0.0f, 0.025f);
  glEnd();

  glTranslatef(0.0f, -0.05f, 0.0f);

  glBegin(GL_POLYGON);
  glVertex2f(0.0f, -0.025f);
  glVertex2f(0.05f, -0.01f); // Comprimento do cotovelo: 0.1
  glVertex2f(0.05f, 0.01f);
  glVertex2f(0.0f, 0.025f);
  glEnd();

  // FECHA A GAVETA
  glPopMatrix();
  glPopMatrix();
}

// Função de exibição
void display() {
  glClear(GL_COLOR_BUFFER_BIT);

  drawAllBoxes();
  desenharRobo();

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
  // Robot Keys
  case 'h':
    anguloOmbro += 5.0f;
    break;
  case 'l':
    anguloOmbro -= 5.0f;
    break;
  case 'j':
    anguloCotovelo += 5.0f;
    break;
  case 'k':
    anguloCotovelo -= 5.0f;
    break;
  case 's':
    anguloMao += 5.0f;
    break;
  case 'w':
    anguloMao -= 5.0f;
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
