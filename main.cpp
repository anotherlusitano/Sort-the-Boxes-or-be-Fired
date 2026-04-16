#include <GL/freeglut.h>
#include <cmath>
#include <cstdio>
#include <vector>

// Dimensões da Janela
const int LARGURA_JANELA = 800;
const int ALTURA_JANELA = 600;

// Configurações de exibição da caixa
const float TAMANHO_CAIXA = 60.0f;
const float ESPACAMENTO_CAIXAS = 100.0f; // Espaço entre caixas
const float POSICAO_Y_CAIXA = 100.0f;    // Distância do fundo

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

std::vector<Caixa *> listaDeCaixas = {};

// Variáveis globais para os ângulos do braço robótico
float anguloOmbro = 180.0f;
float anguloCotovelo = 0.0f;
float anguloMao = 0.0f;

// Variável global para rastrear a caixa apanhada
Caixa *caixaApanhada = nullptr;

// Função para obter a posição dos dedos baseada nos ângulos do braço
void obterPosicaoDedos(float &dedoX, float &dedoY) {
  // Posição base do ombro
  float x = 0.35f;
  float y = 0.1f;

  // Adiciona segmento do ombro (0.4 unidades de comprimento)
  float anguloOmbroRad = anguloOmbro * M_PI / 180.0f;
  x += 0.4f * cosf(anguloOmbroRad);
  y += 0.4f * sinf(anguloOmbroRad);

  // Adiciona segmento do cotovelo (0.3 unidades de comprimento)
  float anguloCotoveloRad = (anguloOmbro + anguloCotovelo) * M_PI / 180.0f;
  x += 0.3f * cosf(anguloCotoveloRad);
  y += 0.3f * sinf(anguloCotoveloRad);

  // Adiciona segmento da mão (0.1 unidades de comprimento)
  float anguloMaoRad =
      (anguloOmbro + anguloCotovelo + anguloMao) * M_PI / 180.0f;
  x += 0.1f * cosf(anguloMaoRad);
  y += 0.1f * sinf(anguloMaoRad);

  dedoX = x;
  dedoY = y;
}

// Função para verificar se os dedos estão tocando a caixa
bool dedoEstaATocarCaixa(float dedoX, float dedoY, Caixa *caixa) {
  if (caixa == nullptr)
    return false;

  // Posição da caixa em coordenadas normalizadas (sempre no centro-inferior)
  float caixaCentroX = 0.0f; // Centro do ecrã
  float caixaCentroY = (2.0f * POSICAO_Y_CAIXA / ALTURA_JANELA) - 1.0f;

  // Metade do tamanho da caixa em coordenadas normalizadas
  float caixaMeioTamanho = (TAMANHO_CAIXA / LARGURA_JANELA);

  // Verifica se o dedo está dentro dos limites da caixa (com alguma tolerância)
  float tolerancia = 0.08f;

  return (dedoX >= caixaCentroX - caixaMeioTamanho - tolerancia &&
          dedoX <= caixaCentroX + caixaMeioTamanho + tolerancia &&
          dedoY >= caixaCentroY - caixaMeioTamanho - tolerancia &&
          dedoY <= caixaCentroY + caixaMeioTamanho + tolerancia);
}

// Function to draw the robotic arm
void desenharRobo() {
  glPushMatrix();
  // Move o robô para a esquerda para caber o ecrã e pegar a primeira caixa
  glTranslatef(0.35f, 0.1f, 0.0f);

  // ==========================================
  // 1. O OMBRO
  // ==========================================
  glRotatef(anguloOmbro, 0.0f, 0.0f, 1.0f);

  glColor3f(0.2f, 0.5f, 0.8f); // Blue
  glBegin(GL_POLYGON);
  glVertex2f(0.0f, -0.05f);
  glVertex2f(0.4f, -0.05f); // Shoulder length: 0.4
  glVertex2f(0.4f, 0.05f);
  glVertex2f(0.0f, 0.05f);
  glEnd();

  // ==========================================
  // 2. O COTOVELO
  // ==========================================
  glTranslatef(0.4f, 0.0f, 0.0f);

  glPushMatrix();

  glRotatef(anguloCotovelo, 0.0f, 0.0f, 1.0f);

  glColor3f(0.8f, 0.2f, 0.2f); // Red
  glBegin(GL_POLYGON);
  glVertex2f(0.0f, -0.04f);
  glVertex2f(0.3f, -0.04f); // Elbow length: 0.3
  glVertex2f(0.3f, 0.04f);
  glVertex2f(0.0f, 0.04f);
  glEnd();

  // ==========================================
  // 3. A MÃO
  // ==========================================
  glTranslatef(0.3f, 0.0f, 0.0f);
  glRotatef(anguloMao, 0.0f, 0.0f, 1.0f);

  glColor3f(0.0f, 0.5f, 0.0f); // Green
  glBegin(GL_POLYGON);
  glVertex2f(0.0f, -0.05f);
  glVertex2f(0.1f, -0.05f); // Hand length: 0.1
  glVertex2f(0.1f, 0.05f);
  glVertex2f(0.0f, 0.05f);
  glEnd();

  // ==========================================
  // 4. OS DEDOS
  // ==========================================
  glTranslatef(0.1f, 0.025f, 0.0f);
  glColor3f(0.5f, 0.1f, 0.1f); // Dark Red

  glBegin(GL_POLYGON);
  glVertex2f(0.0f, -0.025f);
  glVertex2f(0.05f, -0.01f);
  glVertex2f(0.05f, 0.01f);
  glVertex2f(0.0f, 0.025f);
  glEnd();

  glTranslatef(0.0f, -0.05f, 0.0f);

  glBegin(GL_POLYGON);
  glVertex2f(0.0f, -0.025f);
  glVertex2f(0.05f, -0.01f);
  glVertex2f(0.05f, 0.01f);
  glVertex2f(0.0f, 0.025f);
  glEnd();

  glPopMatrix();
  glPopMatrix();
}

// Função para desenhar uma única caixa numa posição determinada (em coordenadas
// de pixel)
void desenharCaixa(float pixelX, float pixelY, Cor cor) {
  // Converte coordenadas de pixel para coordenadas normalizadas de OpenGL (-1 a
  // 1)
  float normalizadoX = (2.0f * pixelX / LARGURA_JANELA) - 1.0f;
  float normalizadoY = (2.0f * pixelY / ALTURA_JANELA) - 1.0f;

  // Converte tamanho da caixa de pixels para coordenadas normalizadas
  float tamanhonormalizado = (2.0f * TAMANHO_CAIXA / LARGURA_JANELA);

  glColor3f(cor.r, cor.g, cor.b);

  glBegin(GL_QUADS);
  // Face frontal
  glVertex2f(normalizadoX - tamanhonormalizado / 2,
             normalizadoY - tamanhonormalizado / 2);
  glVertex2f(normalizadoX + tamanhonormalizado / 2,
             normalizadoY - tamanhonormalizado / 2);
  glVertex2f(normalizadoX + tamanhonormalizado / 2,
             normalizadoY + tamanhonormalizado / 2);
  glVertex2f(normalizadoX - tamanhonormalizado / 2,
             normalizadoY + tamanhonormalizado / 2);
  glEnd();

  // Desenha a borda
  glColor3f(0.0f, 0.0f, 0.0f);
  glLineWidth(2.0f);
  glBegin(GL_LINE_LOOP);
  glVertex2f(normalizadoX - tamanhonormalizado / 2,
             normalizadoY - tamanhonormalizado / 2);
  glVertex2f(normalizadoX + tamanhonormalizado / 2,
             normalizadoY - tamanhonormalizado / 2);
  glVertex2f(normalizadoX + tamanhonormalizado / 2,
             normalizadoY + tamanhonormalizado / 2);
  glVertex2f(normalizadoX - tamanhonormalizado / 2,
             normalizadoY + tamanhonormalizado / 2);
  glEnd();
  glLineWidth(1.0f);
}

// Função para desenhar todas as caixas visíveis (máximo 3)
void desenharTodasAsCaixas() {
  // Exibe os primeiros 3 elementos da lista
  // Posição 0 (centro-inferior): índice 0 da lista
  // Posição 1 (meio): índice 1 da lista
  // Posição 2 (esquerda): índice 2 da lista

  int tamanho = listaDeCaixas.size();
  int contaExibicao = (tamanho > 3) ? 3 : tamanho;

  // Calcula posição central
  float centroX = LARGURA_JANELA / 2.0f;
  float baseY = POSICAO_Y_CAIXA;

  // Desenha as primeiras caixas da contaExibicao
  for (int i = 0; i < contaExibicao; i++) {
    // Posição: centro, depois esquerda, depois mais à esquerda
    float posicaoX = centroX - (i * ESPACAMENTO_CAIXAS);

    desenharCaixa(posicaoX, baseY, listaDeCaixas[i]->cor);
  }
}

// Função de exibição
void display() {
  glClear(GL_COLOR_BUFFER_BIT);

  desenharTodasAsCaixas();
  desenharRobo();

  // Desenha a caixa apanhada na posição dos dedos
  if (caixaApanhada != nullptr) {
    float dedoX, dedoY;
    obterPosicaoDedos(dedoX, dedoY);

    // Converte de coordenadas normalizadas para coordenadas de pixel
    float pixelX = (dedoX + 1.0f) * LARGURA_JANELA / 2.0f;
    float pixelY = (dedoY + 1.0f) * ALTURA_JANELA / 2.0f;

    desenharCaixa(pixelX, pixelY, caixaApanhada->cor);
  }

  glutSwapBuffers();
}

// Função para lidar com os comandos do teclado
void teclado(unsigned char tecla, int x, int y) {
  switch (tecla) {
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
  case ' ': {
    // Tecla de espaço para apanhar/pegar a caixa
    if (!listaDeCaixas.empty()) {
      float dedoX, dedoY;
      obterPosicaoDedos(dedoX, dedoY);

      bool tocando = dedoEstaATocarCaixa(dedoX, dedoY, listaDeCaixas[0]);

      if (caixaApanhada == nullptr && tocando) {
        // Cria uma NOVA caixa com uma CÓPIA dos dados (não apenas um ponteiro)
        caixaApanhada = new Caixa(*listaDeCaixas[0]);
        delete listaDeCaixas[0];
        listaDeCaixas.erase(listaDeCaixas.begin());
        printf("Caixa apanhada!\n");
      }
    }
    break;
  }
  // Controles do braço robótico
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
  case 'S':
    anguloMao += 5.0f;
    break;
  case 'w':
  case 'W':
    anguloMao -= 5.0f;
    break;
  case 27: // Tecla ESC
    exit(0);
    break;
  }

  glutPostRedisplay();
}

int main(int argc, char **argv) {
  // Inicializa GLUT
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
  glutInitWindowSize(LARGURA_JANELA, ALTURA_JANELA);

  // Centraliza a janela no ecrã
  int larguraEcra = glutGet(GLUT_SCREEN_WIDTH);
  int alturaEcra = glutGet(GLUT_SCREEN_HEIGHT);
  glutInitWindowPosition((larguraEcra - LARGURA_JANELA) / 2,
                         (alturaEcra - ALTURA_JANELA) / 2);

  glutCreateWindow("Robotic Arm Program");

  // Define a cor de fundo
  glClearColor(0.9f, 0.9f, 0.9f, 1.0f);

  // Registra os callbacks
  glutDisplayFunc(display);
  glutKeyboardFunc(teclado);

  glutMainLoop();
  return 0;
}
