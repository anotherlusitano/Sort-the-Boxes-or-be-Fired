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

// Função para converter o tipo da caixa em uma string legível
const char *tipoCaixaToString(TipoCaixa tipo) {
  switch (tipo) {
  case CAIXA_VERMELHA:
    return "Vermelho";
  case CAIXA_AZUL:
    return "Azul";
  case CAIXA_VERDE:
    return "Verde";
  case CAIXA_FANTASMA:
    return "Fantasma";
  default:
    return "Desconecido";
  }
}

struct Cor {
  float r;
  float g;
  float b;
};

struct Caixa {
  TipoCaixa tipo; // Tipo da caixa
  Cor cor;        // Cor da caixa
  float x, y;     // Coordenadas da caixa
};

std::vector<Caixa *> listaDeCaixas = {};

// Estrutura para representar um caixote (recipiente para as caixas)
struct Caixote {
  float x, y;           // Coordenadas do caixote
  TipoCaixa tipoAceite; // Tipo de caixa que aceita
  Cor cor;              // Cor do caixote
};

// Lista de caixotes com suas coordenadas e tipos que aceitam
// Nota: As coordenadas podem ser ajustadas conforme necessário
const Caixote caixotes[] = {
    // Caixote Vermelho (esquerda)
    {-0.2f, 0.65f, CAIXA_VERMELHA, {1.0f, 0.0f, 0.0f}},
    // Caixote Azul (centro)
    {0.2f, 0.65f, CAIXA_AZUL, {0.0f, 0.0f, 1.0f}},
    // Caixote Verde (direita)
    {0.6f, 0.65f, CAIXA_VERDE, {0.0f, 1.0f, 0.0f}}};
const int NUM_CAIXOTES = 3;

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

// Função para desenhar um único caixote em coordenadas normalizadas
void desenharCaixote(float x, float y, Cor cor) {
  float larguraCaixote =
      0.15f;                   // Largura do caixote em coordenadas normalizadas
  float alturaCaixote = 0.15f; // Altura do caixote em coordenadas normalizadas
  float larguraBorda = 0.01f;  // Espessura da borda

  // Desenha o interior do caixote
  glColor3f(cor.r * 0.7f, cor.g * 0.7f,
            cor.b * 0.7f); // Cor mais escura para o interior
  glBegin(GL_QUADS);
  glVertex2f(x - larguraCaixote / 2, y - alturaCaixote / 2);
  glVertex2f(x + larguraCaixote / 2, y - alturaCaixote / 2);

  // Usamos o `-0.05` para criar um pequeno espaço entre o fundo e a borda
  // superior do caixote
  glVertex2f(x + larguraCaixote / 2, y + alturaCaixote / 2 - 0.05);
  glVertex2f(x - larguraCaixote / 2, y + alturaCaixote / 2 - 0.05);
  glEnd();

  // Desenha as bordas (fundo e lados)
  glColor3f(0, 0, 0); // Cor completa para as bordas
  glLineWidth(3.0f);
  glBegin(GL_LINE_STRIP);
  // Linha de baixo (fundo)
  glVertex2f(x - larguraCaixote / 2, y - alturaCaixote / 2);
  glVertex2f(x + larguraCaixote / 2, y - alturaCaixote / 2);
  // Linha da direita
  glVertex2f(x + larguraCaixote / 2, y + alturaCaixote / 2);
  glEnd();
  // Linha da esquerda
  glBegin(GL_LINE_STRIP);
  glVertex2f(x - larguraCaixote / 2, y + alturaCaixote / 2);
  glVertex2f(x - larguraCaixote / 2, y - alturaCaixote / 2);
  glEnd();
  glLineWidth(1.0f);
}

// Função para desenhar todos os caixotes
void desenharTodosCaixotes() {
  for (int i = 0; i < NUM_CAIXOTES; i++) {
    desenharCaixote(caixotes[i].x, caixotes[i].y, caixotes[i].cor);
  }
}

// Função para verificar se a caixa apanhada está acima de um caixote
// Retorna o índice do caixote se estiver acima, ou -1 caso contrário
// NOTA: Esta função itera sobre todos os caixotes (máximo 3)
// Para nosso caso de uso, não há preocupação com a performance pois o número de
// caixotes é pequeno
int verificarSobreCaixote(float caixaX, float caixaY) {
  float margem = 0.05f; // Margem de tolerância em coordenadas normalizadas

  for (int i = 0; i < NUM_CAIXOTES; i++) {
    float caixoteLargura = 0.15f;
    float caixoteAltura = 0.15f;

    // Verifica se a caixa está dentro dos limites horizontais e verticais do
    // caixote
    if (caixaX >= caixotes[i].x - caixoteLargura / 2 - margem &&
        caixaX <= caixotes[i].x + caixoteLargura / 2 + margem &&
        caixaY >= caixotes[i].y - caixoteAltura / 2 - margem &&
        caixaY <= caixotes[i].y + caixoteAltura / 2 + margem) {
      return i; // Retorna o índice do caixote
    }
  }

  return -1; // Não encontrou nenhum caixote
}

// Função para depositar a caixa apanhada num caixote
// Verifica se o tipo da caixa corresponde ao tipo que o caixote aceita
void depositarCaixa() {
  if (caixaApanhada == nullptr) {
    printf("Nenhuma caixa apanhada para depositar!\n");
    return;
  }

  // Obtém a posição atual da caixa apanhada
  float dedoX, dedoY;
  obterPosicaoDedos(dedoX, dedoY);

  // Verifica se está acima de algum caixote
  int indiceCaixote = verificarSobreCaixote(dedoX, dedoY);

  if (indiceCaixote == -1) {
    printf("Caixa não está em cima de nenhum caixote!\n");
    return;
  }

  // Verifica se o tipo da caixa corresponde ao tipo que o caixote aceita
  if (caixaApanhada->tipo == caixotes[indiceCaixote].tipoAceite) {
    // Sucesso! Deposita a caixa
    delete caixaApanhada;
    caixaApanhada = nullptr;
    printf("Caixa depositada com sucesso!\n");
    // Descomentar a linha abaixo para mostrar quantas caixas faltam.
    // printf("Caixas restantes: %lu\n", listaDeCaixas.size());
  } else {
    printf("Erro: A caixa do tipo %s não pode ser depositada no caixote que "
           "aceita o tipo %s!\n",
           tipoCaixaToString(caixaApanhada->tipo),
           tipoCaixaToString(caixotes[indiceCaixote].tipoAceite));
  }
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

void desenharTapete(void) {
  const float larguraTapete = 0.8f;
  const float alturaTapete = 0.3f;
  const int etapaDoTapete = 0;
  const int NUM_LINHAS_TAPETE = 24;

  // Tapete
  glPushMatrix();
  glTranslatef(-0.2f, -0.65f, 0.0f); // Move o tapete para baixo
  glColor3f(0.2f, 0.2f, 0.2f);

  // Base do tapete
  glBegin(GL_QUADS);
  glVertex2f(-larguraTapete / 2, -alturaTapete / 2);
  glVertex2f(larguraTapete / 2, -alturaTapete / 2);
  glVertex2f(larguraTapete / 2, alturaTapete / 2);
  glVertex2f(-larguraTapete / 2, alturaTapete / 2);
  glEnd();

  glColor3f(0.35f, 0.35f, 0.35f);

  float larguraLinha = larguraTapete / NUM_LINHAS_TAPETE;

  // Linhas do tapete
  for (int i = 0; i < NUM_LINHAS_TAPETE; ++i) {
    // Calcula o índice da linha com deslocamento circular.
    // `etapaDoTapete` indica quantas posições o tapete avançou.
    // O operador % garante que o índice volta ao início ao ultrapassar o
    // limite.
    int indiceDeslocado = (i + etapaDoTapete) % NUM_LINHAS_TAPETE;

    // Determina a posição horizontal da linha.
    // O tapete está centrado na origem; começamos na borda esquerda
    // e avançamos conforme o índice deslocado.
    float x = -larguraTapete / 2 + indiceDeslocado * larguraLinha;

    // Desenha uma linha do tapete
    glBegin(GL_QUADS);
    glVertex2f(x, -alturaTapete / 2);
    glVertex2f(x + larguraLinha * 0.8f, -alturaTapete / 2);
    glVertex2f(x + larguraLinha * 0.8f, alturaTapete / 2);
    glVertex2f(x, alturaTapete / 2);
    glEnd();
  }
  glPopMatrix();

  // Desenhar Barreira do tapete
  glPushMatrix();
  const float larguraBarreira = 0.1f;
  const float alturaBarreira = 0.5f;

  glTranslatef(0.2f, -0.65f, 0.0f); // Move a barreira para baixo
  glColor3f(1.0f, 0.6f, 0);

  // Base do tapete
  glBegin(GL_QUADS);
  glVertex2f(-larguraBarreira / 2, -alturaBarreira / 2);
  glVertex2f(larguraBarreira / 2, -alturaBarreira / 2);
  glVertex2f(larguraBarreira / 2, alturaBarreira / 2);
  glVertex2f(-larguraBarreira / 2, alturaBarreira / 2);
  glEnd();

  glPopMatrix();
}

void desenharMaquina() {
  // Coordenadas base para a máquina
  float baseX = -0.9f;
  float baseY = -0.3f;

  float larguraMaquina = 0.5f;
  float alturaMaquina = 0.8f;
  float alturaTelhado = 0.25f;

  glPushMatrix();
  glTranslatef(-0.2f, -0.70f, 0.0f);

  // Corpo da máquina
  glColor3f(0.3f, 0.3f, 0.3f);
  glBegin(GL_QUADS);
  glVertex2f(baseX, baseY);
  glVertex2f(baseX + larguraMaquina, baseY);
  glVertex2f(baseX + larguraMaquina, baseY + alturaMaquina);
  glVertex2f(baseX, baseY + alturaMaquina);
  glEnd();

  // Telhado da máquina
  glColor3f(0.25f, 0.25f, 0.25f);
  glBegin(GL_TRIANGLES);
  glVertex2f(baseX, baseY + alturaMaquina);
  glVertex2f(baseX + larguraMaquina, baseY + alturaMaquina);
  glVertex2f(baseX + larguraMaquina / 2.0f,
             baseY + alturaMaquina + alturaTelhado);
  glEnd();

  float larguraPorta = 0.12f;
  float alturaPorta = 0.42f;

  // right aligned
  float xPorta = baseX + larguraMaquina - larguraPorta;
  float yPorta = alturaMaquina * 0.10f - alturaPorta * 0.5f;

  // Porta da máquina
  glColor3f(0.0f, 0.0f, 0.0f);
  glBegin(GL_QUADS);
  glVertex2f(xPorta, yPorta);
  glVertex2f(xPorta + larguraPorta, yPorta);
  glVertex2f(xPorta + larguraPorta, yPorta + alturaPorta);
  glVertex2f(xPorta, yPorta + alturaPorta);
  glEnd();

  glPopMatrix();
}

// Função de exibição
void display() {
  glClear(GL_COLOR_BUFFER_BIT);

  desenharTapete();
  desenharMaquina();
  desenharTodasAsCaixas();
  desenharRobo();
  desenharTodosCaixotes();

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
    // Verifica se a caixa apanhada está acima de um caixote e tenta deposita-la
    depositarCaixa();

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
