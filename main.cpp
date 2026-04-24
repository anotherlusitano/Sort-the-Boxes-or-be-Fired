#include <GL/freeglut.h>
#include <cmath>
#include <cstdio>
#include <vector>

// Dimensões da Janela
const int LARGURA_JANELA = 800;
const int ALTURA_JANELA = 600;

// Configurações de exibição da caixa
float TAMANHO_CAIXA = 60.0f;
const float ESPACAMENTO_CAIXAS = 140.0f; // Espaço entre caixas
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
  float x, y, z;  // Coordenadas da caixa
};

std::vector<Caixa *> listaDeCaixas = {};

// Estrutura para representar um caixote (recipiente para as caixas)
struct Caixote {
  float x, y;           // Coordenadas do caixote
  TipoCaixa tipoAceite; // Tipo de caixa que aceita
  Cor cor;              // Cor do caixote
  Cor textura;          // Cor da Textura do caixote
};

// Lista de caixotes com suas coordenadas e tipos que aceitam
// Nota: As coordenadas podem ser ajustadas conforme necessário
const Caixote caixotes[] = {
    // Caixote Vermelho (esquerda)
    {-0.2f, 0.65f, CAIXA_VERMELHA, {1.0f, 0.0f, 0.0f}, {0.91f, 0.65f, 0.23f}},
    // Caixote Azul (centro)
    {0.2f, 0.65f, CAIXA_AZUL, {0.0f, 0.0f, 1.0f}, {0.91f, 0.65f, 0.23f}},
    // Caixote Verde (direita)
    {0.6f, 0.65f, CAIXA_VERDE, {0.0f, 1.0f, 0.0f}, {0.91f, 0.65f, 0.23f}}};
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

// Função para desenhar a base 3D do braço robótico
void desenharBaseBraco() {
  float largura = 0.8f;       // Largura da base
  float altura = 0.1f;        // Altura da base
  float profundidade = 0.20f; // Profundidade da base

  float metadeLargura = largura / 2.0f;
  float metadeAltura = altura / 2.0f;
  float metadeProf = profundidade / 2.0f;

  // Cor cinzenta
  Cor cinzento = {0.5f, 0.5f, 0.5f};

  glPushMatrix();

  glTranslatef(0.0f, -0.05f, 0.9f);

  // Aplica rotação 3D para perspectiva isométrica
  glRotatef(-10.0f, 1.0f, 0.0f, 0.0f); // Rotação em X
  glRotatef(85.0f, 0.0f, -1.0f, 0.0f); // Rotação em Y

  glBegin(GL_QUADS);
  // Face frontal
  glColor3f(cinzento.r, cinzento.g, cinzento.b);
  glVertex3f(-metadeLargura, -metadeAltura, metadeProf);
  glVertex3f(metadeLargura, -metadeAltura, metadeProf);
  glVertex3f(metadeLargura, metadeAltura, metadeProf);
  glVertex3f(-metadeLargura, metadeAltura, metadeProf);

  // Face traseira
  glColor3f(cinzento.r * 0.7f, cinzento.g * 0.7f, cinzento.b * 0.7f);
  glVertex3f(-metadeLargura, -metadeAltura, -metadeProf);
  glVertex3f(-metadeLargura, metadeAltura, -metadeProf);
  glVertex3f(metadeLargura, metadeAltura, -metadeProf);
  glVertex3f(metadeLargura, -metadeAltura, -metadeProf);

  // Face superior
  glColor3f(cinzento.r * 0.9f, cinzento.g * 0.9f, cinzento.b * 0.9f);
  glVertex3f(-metadeLargura, metadeAltura, metadeProf);
  glVertex3f(metadeLargura, metadeAltura, metadeProf);
  glVertex3f(metadeLargura, metadeAltura, -metadeProf);
  glVertex3f(-metadeLargura, metadeAltura, -metadeProf);

  // Face inferior
  glColor3f(cinzento.r * 0.8f, cinzento.g * 0.8f, cinzento.b * 0.8f);
  glVertex3f(-metadeLargura, -metadeAltura, metadeProf);
  glVertex3f(-metadeLargura, -metadeAltura, -metadeProf);
  glVertex3f(metadeLargura, -metadeAltura, -metadeProf);
  glVertex3f(metadeLargura, -metadeAltura, metadeProf);

  // Face direita
  glColor3f(cinzento.r * 0.85f, cinzento.g * 0.85f, cinzento.b * 0.85f);
  glVertex3f(metadeLargura, -metadeAltura, metadeProf);
  glVertex3f(metadeLargura, -metadeAltura, -metadeProf);
  glVertex3f(metadeLargura, metadeAltura, -metadeProf);
  glVertex3f(metadeLargura, metadeAltura, metadeProf);

  // Face esquerda
  glColor3f(cinzento.r * 0.75f, cinzento.g * 0.75f, cinzento.b * 0.75f);
  glVertex3f(-metadeLargura, -metadeAltura, metadeProf);
  glVertex3f(-metadeLargura, metadeAltura, metadeProf);
  glVertex3f(-metadeLargura, metadeAltura, -metadeProf);
  glVertex3f(-metadeLargura, -metadeAltura, -metadeProf);
  glEnd();

  // Desenha as arestas da base
  glColor3f(0.0f, 0.0f, 0.0f);
  glLineWidth(2.0f);

  glBegin(GL_LINE_LOOP);
  glVertex3f(-metadeLargura, -metadeAltura, metadeProf);
  glVertex3f(metadeLargura, -metadeAltura, metadeProf);
  glVertex3f(metadeLargura, metadeAltura, metadeProf);
  glVertex3f(-metadeLargura, metadeAltura, metadeProf);
  glEnd();

  glBegin(GL_LINE_LOOP);
  glVertex3f(-metadeLargura, -metadeAltura, -metadeProf);
  glVertex3f(metadeLargura, -metadeAltura, -metadeProf);
  glVertex3f(metadeLargura, metadeAltura, -metadeProf);
  glVertex3f(-metadeLargura, metadeAltura, -metadeProf);
  glEnd();

  glBegin(GL_LINES);
  glVertex3f(-metadeLargura, -metadeAltura, metadeProf);
  glVertex3f(-metadeLargura, -metadeAltura, -metadeProf);
  glVertex3f(metadeLargura, -metadeAltura, metadeProf);
  glVertex3f(metadeLargura, -metadeAltura, -metadeProf);
  glVertex3f(metadeLargura, metadeAltura, metadeProf);
  glVertex3f(metadeLargura, metadeAltura, -metadeProf);
  glVertex3f(-metadeLargura, metadeAltura, metadeProf);
  glVertex3f(-metadeLargura, metadeAltura, -metadeProf);
  glEnd();
  glLineWidth(1.0f);

  glPopMatrix();
}

// Function to draw the robotic arm
void desenharRobo() {
  glPushMatrix();
  // Move o robô para a esquerda para caber o ecrã e pegar a primeira caixa
  glTranslatef(0.35f, 0.1f, -1.0f);

  // Desenha a base 3D do braço robótico
  desenharBaseBraco();

  // ==========================================
  // 1. O OMBRO
  // ==========================================
  glRotatef(anguloOmbro, 0.0f, 0.0f, 1.0f);

  glColor3f(1.0f, 0.5f, 0); // Laranja
  glBegin(GL_POLYGON);
  glVertex3f(0.0f, -0.05f, 0.3f);
  glVertex3f(0.4f, -0.05f, 0.3f); // Shoulder length: 0.4
  glVertex3f(0.4f, 0.05f, 0.3f);
  glVertex3f(0.0f, 0.05f, 0.3f);
  glEnd();

  // ==========================================
  // 2. O COTOVELO
  // ==========================================
  glTranslatef(0.4f, 0.0f, 0.0f);

  glPushMatrix();

  glRotatef(anguloCotovelo, 0.0f, 0.0f, 1.0f);

  glColor3f(0.1f, 0, 0.1f); // Cinzento
  glBegin(GL_POLYGON);
  glVertex3f(0.0f, -0.04f, 0.3f);
  glVertex3f(0.3f, -0.04f, 0.3f); // Elbow length: 0.3
  glVertex3f(0.3f, 0.04f, 0.3f);
  glVertex3f(0.0f, 0.04f, 0.3f);
  glEnd();

  // ==========================================
  // 3. A MÃO
  // ==========================================
  glTranslatef(0.3f, 0.0f, 0.0f);
  glRotatef(anguloMao, 0.0f, 0.0f, 1.0f);

  glColor3f(1.0f, 0.5f, 0); // Laranja
  glBegin(GL_POLYGON);
  glVertex3f(0.0f, -0.05f, 0.3f);
  glVertex3f(0.1f, -0.05f, 0.3f); // Hand length: 0.1
  glVertex3f(0.1f, 0.05f, 0.3f);
  glVertex3f(0.0f, 0.05f, 0.3f);
  glEnd();

  // ==========================================
  // 4. OS DEDOS
  // ==========================================
  glTranslatef(0.1f, 0.025f, 0.0f);
  glColor3f(0.1f, 0, 0.1f); // Cinzento

  glBegin(GL_POLYGON);
  glVertex3f(0.0f, -0.025f, 0.3f);
  glVertex3f(0.05f, -0.01f, 0.3f);
  glVertex3f(0.05f, 0.01f, 0.3f);
  glVertex3f(0.0f, 0.025f, 0.3f);
  glEnd();

  glTranslatef(0.0f, -0.05f, 0.0f);

  glBegin(GL_POLYGON);
  glVertex3f(0.0f, -0.025f, 0.3f);
  glVertex3f(0.05f, -0.01f, 0.3f);
  glVertex3f(0.05f, 0.01f, 0.3f);
  glVertex3f(0.0f, 0.025f, 0.3f);
  glEnd();

  glPopMatrix();
  glPopMatrix();
}

// Função para desenhar um único caixote em coordenadas normalizadas
void desenharCaixote(float x, float y, Cor cor, Cor textura) {
  float largura = 0.20f;      // Largura do caixote em coordenadas normalizadas
  float altura = 0.20f;       // Altura do caixote em coordenadas normalizadas
  float profundidade = 0.21f; // Profundidade do caixote

  float metadeLargura = largura / 2.0f;
  float metadeAltura = altura / 2.0f;
  float metadeProf = profundidade / 2.0f;

  glPushMatrix();
  glTranslatef(x, y, 0.25f); // Posiciona o caixote

  // Aplica rotação 3D para perspectiva isométrica
  glRotatef(45.0f, -1.5f, 1.0f, 0.0f); // Rotação em Y

  glBegin(GL_QUADS);
  // Face frontal
  glColor3f(textura.r, textura.g, textura.b);
  glVertex3f(-metadeLargura, -metadeAltura, metadeProf);
  glVertex3f(metadeLargura, -metadeAltura, metadeProf);
  glVertex3f(metadeLargura, metadeAltura, metadeProf);
  glVertex3f(-metadeLargura, metadeAltura, metadeProf);

  // Face traseira
  glColor3f(cor.r * 0.7f, cor.g * 0.7f, cor.b * 0.7f);
  glVertex3f(-metadeLargura, -metadeAltura, -metadeProf);
  glVertex3f(-metadeLargura, metadeAltura, -metadeProf);
  glVertex3f(metadeLargura, metadeAltura, -metadeProf);
  glVertex3f(metadeLargura, -metadeAltura, -metadeProf);

  // Face inferior (fundo)
  glColor3f(textura.r * 0.8f, textura.g * 0.8f, textura.b * 0.8f);
  glVertex3f(-metadeLargura, -metadeAltura, metadeProf);
  glVertex3f(-metadeLargura, -metadeAltura, -metadeProf);
  glVertex3f(metadeLargura, -metadeAltura, -metadeProf);
  glVertex3f(metadeLargura, -metadeAltura, metadeProf);

  // Face direita
  glColor3f(textura.r * 0.85f, textura.g * 0.85f, textura.b * 0.85f);
  glVertex3f(metadeLargura, -metadeAltura, metadeProf);
  glVertex3f(metadeLargura, -metadeAltura, -metadeProf);
  glVertex3f(metadeLargura, metadeAltura, -metadeProf);
  glVertex3f(metadeLargura, metadeAltura, metadeProf);

  // Face esquerda
  glColor3f(textura.r * 0.75f, textura.g * 0.75f, textura.b * 0.75f);
  glVertex3f(-metadeLargura, -metadeAltura, metadeProf);
  glVertex3f(-metadeLargura, metadeAltura, metadeProf);
  glVertex3f(-metadeLargura, metadeAltura, -metadeProf);
  glVertex3f(-metadeLargura, -metadeAltura, -metadeProf);

  // SEM FACE SUPERIOR (abertura para receber as caixas)
  glEnd();

  // Desenha as arestas do caixote
  glColor3f(0.0f, 0.0f, 0.0f);
  glLineWidth(2.0f);

  // Arestas frontal
  glBegin(GL_LINE_LOOP);
  glVertex3f(-metadeLargura, -metadeAltura, metadeProf);
  glVertex3f(metadeLargura, -metadeAltura, metadeProf);
  glVertex3f(metadeLargura, metadeAltura, metadeProf);
  glVertex3f(-metadeLargura, metadeAltura, metadeProf);
  glEnd();

  // Arestas traseira
  glBegin(GL_LINE_LOOP);
  glVertex3f(-metadeLargura, -metadeAltura, -metadeProf);
  glVertex3f(metadeLargura, -metadeAltura, -metadeProf);
  glVertex3f(metadeLargura, metadeAltura, -metadeProf);
  glVertex3f(-metadeLargura, metadeAltura, -metadeProf);
  glEnd();

  // Arestas verticais
  glBegin(GL_LINES);
  glVertex3f(-metadeLargura, -metadeAltura, metadeProf);
  glVertex3f(-metadeLargura, -metadeAltura, -metadeProf);
  glVertex3f(metadeLargura, -metadeAltura, metadeProf);
  glVertex3f(metadeLargura, -metadeAltura, -metadeProf);
  glVertex3f(metadeLargura, metadeAltura, metadeProf);
  glVertex3f(metadeLargura, metadeAltura, -metadeProf);
  glVertex3f(-metadeLargura, metadeAltura, metadeProf);
  glVertex3f(-metadeLargura, metadeAltura, -metadeProf);
  glEnd();

  // Arestas da abertura superior
  glBegin(GL_LINE_LOOP);
  glVertex3f(-metadeLargura, metadeAltura, metadeProf);
  glVertex3f(metadeLargura, metadeAltura, metadeProf);
  glVertex3f(metadeLargura, metadeAltura, -metadeProf);
  glVertex3f(-metadeLargura, metadeAltura, -metadeProf);
  glEnd();

  glLineWidth(1.0f);
  glPopMatrix();
}

// Função para desenhar todos os caixotes
void desenharTodosCaixotes() {
  for (int i = 0; i < NUM_CAIXOTES; i++) {
    desenharCaixote(caixotes[i].x, caixotes[i].y, caixotes[i].cor,
                    caixotes[i].textura);
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
  float normalizadoX = (2.4f * pixelX / LARGURA_JANELA) - 1.0f;
  float normalizadoY = (2.15f * pixelY / ALTURA_JANELA) - 1.0f;
  float normalizadoZ = -0.25f; // Profundidade padrão

  // Fazer que as caixas azuis sejam desenhadas com tamanho reduzido
  if (cor.b == 1.0f) {
    TAMANHO_CAIXA = 45.0f;
  }

  // Converte tamanho da caixa de pixels para coordenadas normalizadas
  float tamanhonormalizado = (2.0f * TAMANHO_CAIXA / LARGURA_JANELA);
  float profundidade =
      tamanhonormalizado; // Mesma profundidade para manter proporção cúbica

  float meia = tamanhonormalizado / 2.0f;
  float meiaProf = profundidade / 2.0f;

  glPushMatrix();

  // Fazer que as caixas vermelhas sejam desenhadas em modo wireframe
  if (cor.r == 1.0f) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // Ativa modo wireframe
  }

  // Rotação para mostrar perspectiva 3D
  glRotatef(-10.0f, 1.0f, 0.0f, 0.0f); // Rotação em X
  glRotatef(45.0f, 0.0f, 1.0f, 0.0f);  // Rotação em Y

  glColor3f(cor.r, cor.g, cor.b);

  glBegin(GL_QUADS);
  // Face frontal
  glColor3f(cor.r, cor.g, cor.b);
  glVertex3f(normalizadoX - meia, normalizadoY - meia, normalizadoZ + meiaProf);
  glVertex3f(normalizadoX + meia, normalizadoY - meia, normalizadoZ + meiaProf);
  glVertex3f(normalizadoX + meia, normalizadoY + meia, normalizadoZ + meiaProf);
  glVertex3f(normalizadoX - meia, normalizadoY + meia, normalizadoZ + meiaProf);

  // Face traseira
  glColor3f(cor.r * 0.7f, cor.g * 0.7f, cor.b * 0.7f);
  glVertex3f(normalizadoX - meia, normalizadoY - meia, normalizadoZ - meiaProf);
  glVertex3f(normalizadoX - meia, normalizadoY + meia, normalizadoZ - meiaProf);
  glVertex3f(normalizadoX + meia, normalizadoY + meia, normalizadoZ - meiaProf);
  glVertex3f(normalizadoX + meia, normalizadoY - meia, normalizadoZ - meiaProf);

  // Face superior
  glColor3f(cor.r * 0.9f, cor.g * 0.9f, cor.b * 0.9f);
  glVertex3f(normalizadoX - meia, normalizadoY + meia, normalizadoZ + meiaProf);
  glVertex3f(normalizadoX + meia, normalizadoY + meia, normalizadoZ + meiaProf);
  glVertex3f(normalizadoX + meia, normalizadoY + meia, normalizadoZ - meiaProf);
  glVertex3f(normalizadoX - meia, normalizadoY + meia, normalizadoZ - meiaProf);

  // Face inferior
  glColor3f(cor.r * 0.8f, cor.g * 0.8f, cor.b * 0.8f);
  glVertex3f(normalizadoX - meia, normalizadoY - meia, normalizadoZ + meiaProf);
  glVertex3f(normalizadoX - meia, normalizadoY - meia, normalizadoZ - meiaProf);
  glVertex3f(normalizadoX + meia, normalizadoY - meia, normalizadoZ - meiaProf);
  glVertex3f(normalizadoX + meia, normalizadoY - meia, normalizadoZ + meiaProf);

  // Face direita
  glColor3f(cor.r * 0.85f, cor.g * 0.85f, cor.b * 0.85f);
  glVertex3f(normalizadoX + meia, normalizadoY - meia, normalizadoZ + meiaProf);
  glVertex3f(normalizadoX + meia, normalizadoY - meia, normalizadoZ - meiaProf);
  glVertex3f(normalizadoX + meia, normalizadoY + meia, normalizadoZ - meiaProf);
  glVertex3f(normalizadoX + meia, normalizadoY + meia, normalizadoZ + meiaProf);

  // Face esquerda
  glColor3f(cor.r * 0.75f, cor.g * 0.75f, cor.b * 0.75f);
  glVertex3f(normalizadoX - meia, normalizadoY - meia, normalizadoZ + meiaProf);
  glVertex3f(normalizadoX - meia, normalizadoY + meia, normalizadoZ + meiaProf);
  glVertex3f(normalizadoX - meia, normalizadoY + meia, normalizadoZ - meiaProf);
  glVertex3f(normalizadoX - meia, normalizadoY - meia, normalizadoZ - meiaProf);
  glEnd();

  // Desenha as arestas da caixa
  glColor3f(0.0f, 0.0f, 0.0f);
  glLineWidth(2.0f);
  glBegin(GL_LINE_LOOP);
  glVertex3f(normalizadoX - meia, normalizadoY - meia, normalizadoZ + meiaProf);
  glVertex3f(normalizadoX + meia, normalizadoY - meia, normalizadoZ + meiaProf);
  glVertex3f(normalizadoX + meia, normalizadoY + meia, normalizadoZ + meiaProf);
  glVertex3f(normalizadoX - meia, normalizadoY + meia, normalizadoZ + meiaProf);
  glEnd();

  glBegin(GL_LINE_LOOP);
  glVertex3f(normalizadoX - meia, normalizadoY - meia, normalizadoZ - meiaProf);
  glVertex3f(normalizadoX + meia, normalizadoY - meia, normalizadoZ - meiaProf);
  glVertex3f(normalizadoX + meia, normalizadoY + meia, normalizadoZ - meiaProf);
  glVertex3f(normalizadoX - meia, normalizadoY + meia, normalizadoZ - meiaProf);
  glEnd();

  glBegin(GL_LINES);
  glVertex3f(normalizadoX - meia, normalizadoY - meia, normalizadoZ + meiaProf);
  glVertex3f(normalizadoX - meia, normalizadoY - meia, normalizadoZ - meiaProf);
  glVertex3f(normalizadoX + meia, normalizadoY - meia, normalizadoZ + meiaProf);
  glVertex3f(normalizadoX + meia, normalizadoY - meia, normalizadoZ - meiaProf);
  glVertex3f(normalizadoX + meia, normalizadoY + meia, normalizadoZ + meiaProf);
  glVertex3f(normalizadoX + meia, normalizadoY + meia, normalizadoZ - meiaProf);
  glVertex3f(normalizadoX - meia, normalizadoY + meia, normalizadoZ + meiaProf);
  glVertex3f(normalizadoX - meia, normalizadoY + meia, normalizadoZ - meiaProf);
  glEnd();
  glLineWidth(1.0f);

  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // Ativa modo preenchido
  TAMANHO_CAIXA = 60.0f; // Voltar ao tamanho padrão para as próximas caixas

  glPopMatrix();
}

// Função para desenhar todas as caixas visíveis (máximo 3)
void desenharTodasAsCaixas() {
  // Exibe os primeiros 3 elementos da lista
  // Posição 0 (centro-inferior): índice 0 da lista
  // Posição 1 (meio): índice 1 da lista
  // Posição 2 (esquerda): índice 2 da lista

  int tamanho = listaDeCaixas.size();
  int contaExibicao = (tamanho > 4) ? 4 : tamanho;

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
  const float larguraTapete = 1.65f;
  const float alturaTapete = 0.3f;
  const int etapaDoTapete = 0;
  const int NUM_LINHAS_TAPETE = 24;

  // Tapete
  glPushMatrix();
  glTranslatef(-0.85f, -0.68f, 0.0f);  // Position the tapete
  glRotatef(-10.0f, 1.0f, 0.0f, 0.0f); // Rotate around X
  glRotatef(45.0f, 0.0f, 1.0f, 0.0f);  // Rotate around Y
  glColor3f(0.2f, 0.2f, 0.2f);

  // Base do tapete
  glBegin(GL_QUADS);
  glVertex3f(-larguraTapete / 2, -alturaTapete / 2, 0.6f);
  glVertex3f(larguraTapete / 2, -alturaTapete / 2, 0.6f);
  glVertex3f(larguraTapete / 2, alturaTapete / 2, 0.6f);
  glVertex3f(-larguraTapete / 2, alturaTapete / 2, 0.6f);
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
    glVertex3f(x, -alturaTapete / 2, 0.59f);
    glVertex3f(x + larguraLinha * 0.8f, -alturaTapete / 2, 0.59f);
    glVertex3f(x + larguraLinha * 0.8f, alturaTapete / 2, 0.59f);
    glVertex3f(x, alturaTapete / 2, 0.59f);
    glEnd();
  }
  glPopMatrix();

  // Desenhar Barreira do tapete
  glPushMatrix();
  const float larguraBarreira = 0.1f;
  const float alturaBarreira = 0.4f;

  glTranslatef(0.2f, -0.70f, 0.0f); // Move a barreira para baixo
  glColor3f(1.0f, 0.6f, 0);

  // Barreira do tapete
  glBegin(GL_QUADS);
  glVertex3f(-larguraBarreira / 2, -alturaBarreira / 2, -0.18f);
  glVertex3f(larguraBarreira / 2, -alturaBarreira / 2, -0.18f);
  glVertex3f(larguraBarreira / 2, alturaBarreira / 2, -0.18f);
  glVertex3f(-larguraBarreira / 2, alturaBarreira / 2, -0.18f);
  glEnd();

  glPopMatrix();
}

// Função para desenhar a caixa apanhada pelo braço robótico
// Aplica as mesmas transformações do braço para que a caixa siga a mão
void desenharCaixaApanhada() {
  if (caixaApanhada == nullptr) {
    return;
  }

  glPushMatrix();

  // Fazer que as caixas vermelhas sejam desenhadas em modo wireframe
  if (caixaApanhada->cor.r == 1.0f) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // Ativa modo wireframe
  }
  // Fazer que as caixas azuis sejam desenhadas com tamanho reduzido
  if (caixaApanhada->cor.b == 1.0f) {
    TAMANHO_CAIXA = 45.0f;
  }

  // Aplica as MESMAS transformações do braço robótico
  glTranslatef(0.35f, 0.1f, -0.4f); // Posição inicial do ombro
  glRotatef(anguloOmbro, 0.0f, 0.0f, 1.0f);

  glTranslatef(0.4f, 0.0f, 0.0f); // Move até o cotovelo
  glRotatef(anguloCotovelo, 0.0f, 0.0f, 1.0f);

  glTranslatef(0.3f, 0.0f, 0.0f); // Move até a mão
  glRotatef(anguloMao, 0.0f, 0.0f, 1.0f);

  glTranslatef(0.1f, 0.0f, 0.0f); // Move até os dedos

  // Aplica rotação 3D para perspectiva isométrica
  glRotatef(-65.0f, 1.0f, 0.0f, 0.0f); // Rotação em X
  glRotatef(45.0f, 0.0f, 1.0f, 0.0f);  // Rotação em Y

  // Cálculos de dimensão
  float tamanhoNormalizado = (2.0f * TAMANHO_CAIXA / LARGURA_JANELA);
  float meia = tamanhoNormalizado / 2.0f;
  float meiaProf = tamanhoNormalizado / 2.0f;

  // Desenha o CUBO 3D completo
  glBegin(GL_QUADS);
  // Face frontal
  glColor3f(caixaApanhada->cor.r, caixaApanhada->cor.g, caixaApanhada->cor.b);
  glVertex3f(-meia, -meia, meiaProf);
  glVertex3f(meia, -meia, meiaProf);
  glVertex3f(meia, meia, meiaProf);
  glVertex3f(-meia, meia, meiaProf);

  // Face traseira
  glColor3f(caixaApanhada->cor.r * 0.7f, caixaApanhada->cor.g * 0.7f,
            caixaApanhada->cor.b * 0.7f);
  glVertex3f(-meia, -meia, -meiaProf);
  glVertex3f(-meia, meia, -meiaProf);
  glVertex3f(meia, meia, -meiaProf);
  glVertex3f(meia, -meia, -meiaProf);

  // Face superior
  glColor3f(caixaApanhada->cor.r * 0.9f, caixaApanhada->cor.g * 0.9f,
            caixaApanhada->cor.b * 0.9f);
  glVertex3f(-meia, meia, meiaProf);
  glVertex3f(meia, meia, meiaProf);
  glVertex3f(meia, meia, -meiaProf);
  glVertex3f(-meia, meia, -meiaProf);

  // Face inferior
  glColor3f(caixaApanhada->cor.r * 0.8f, caixaApanhada->cor.g * 0.8f,
            caixaApanhada->cor.b * 0.8f);
  glVertex3f(-meia, -meia, meiaProf);
  glVertex3f(-meia, -meia, -meiaProf);
  glVertex3f(meia, -meia, -meiaProf);
  glVertex3f(meia, -meia, meiaProf);

  // Face direita
  glColor3f(caixaApanhada->cor.r * 0.85f, caixaApanhada->cor.g * 0.85f,
            caixaApanhada->cor.b * 0.85f);
  glVertex3f(meia, -meia, meiaProf);
  glVertex3f(meia, -meia, -meiaProf);
  glVertex3f(meia, meia, -meiaProf);
  glVertex3f(meia, meia, meiaProf);

  // Face esquerda
  glColor3f(caixaApanhada->cor.r * 0.75f, caixaApanhada->cor.g * 0.75f,
            caixaApanhada->cor.b * 0.75f);
  glVertex3f(-meia, -meia, meiaProf);
  glVertex3f(-meia, meia, meiaProf);
  glVertex3f(-meia, meia, -meiaProf);
  glVertex3f(-meia, -meia, -meiaProf);
  glEnd();

  // Desenha as arestas da caixa
  glColor3f(0.0f, 0.0f, 0.0f);
  glLineWidth(2.0f);
  glBegin(GL_LINE_LOOP);
  glVertex3f(-meia, -meia, meiaProf);
  glVertex3f(meia, -meia, meiaProf);
  glVertex3f(meia, meia, meiaProf);
  glVertex3f(-meia, meia, meiaProf);
  glEnd();

  glBegin(GL_LINE_LOOP);
  glVertex3f(-meia, -meia, -meiaProf);
  glVertex3f(meia, -meia, -meiaProf);
  glVertex3f(meia, meia, -meiaProf);
  glVertex3f(-meia, meia, -meiaProf);
  glEnd();

  glBegin(GL_LINES);
  glVertex3f(-meia, -meia, meiaProf);
  glVertex3f(-meia, -meia, -meiaProf);
  glVertex3f(meia, -meia, meiaProf);
  glVertex3f(meia, -meia, -meiaProf);
  glVertex3f(meia, meia, meiaProf);
  glVertex3f(meia, meia, -meiaProf);
  glVertex3f(-meia, meia, meiaProf);
  glVertex3f(-meia, meia, -meiaProf);
  glEnd();
  glLineWidth(1.0f);

  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // Ativa modo preenchido
  TAMANHO_CAIXA = 60.0f; // Voltar ao tamanho padrão para as próximas caixas

  glPopMatrix();
}

// Função de exibição
void display() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  desenharTapete();
  desenharTodasAsCaixas();
  desenharRobo();
  desenharTodosCaixotes();
  desenharCaixaApanhada();

  glutSwapBuffers();
}

void menu() {
  printf("\n");
  printf("=============================================================\n");
  printf("                    PAINEL DE BORDO\n");
  printf("               Manual do Operador - Controlo\n");
  printf("=============================================================\n");
  printf("\n");

  printf("  MOVIMENTO DO BRAÇO ROBÓTICO\n");
  printf("  -----------------------------------------------------------\n");
  printf("    Setas Direcionais   -> Rotacao principal\n");
  printf("    W / S               -> Movimento do Ombro\n");
  printf("    A / D               -> Movimento do Cotovelo\n");
  printf("    Q / E               -> Movimento da Mão\n");
  printf("\n");

  printf("  INTERAÇÃO\n");
  printf("  -----------------------------------------------------------\n");
  printf("    ESPAÇO              -> Agarrar / Largar Pacote\n");
  printf("\n");

  printf("  GERAÇÃO DE PACOTES (Modo de Teste)\n");
  printf("  -----------------------------------------------------------\n");
  printf("    Tecla 1             -> Gerar Pacote Tipo Vermelho\n");
  printf("    Tecla 2             -> Gerar Pacote Tipo Azul\n");
  printf("    Tecla 3             -> Gerar Pacote Tipo Verde\n");
  printf("\n");

  printf("=============================================================\n");
  printf("  NOTA: Utilize as teclas conforme indicado acima.\n");
  printf("        Consulte este painel clicando na tecla H.\n");
  printf("=============================================================\n");
  printf("\n");
}

// Função para lidar com os comandos do teclado
void teclado(unsigned char tecla, int x, int y) {
  switch (tecla) {
  case '1': {
    Caixa *caixaVermelha = new Caixa();
    caixaVermelha->tipo = CAIXA_VERMELHA;
    caixaVermelha->cor = {1.0f, 0.0f, 0.0f};
    caixaVermelha->x = -1.0f;
    caixaVermelha->z = 0.0f;

    listaDeCaixas.push_back(caixaVermelha);

    printf("Caixa Vermelha adicionada (Total: %lu)\n", listaDeCaixas.size());
    break;
  }
  case '2': {
    Caixa *caixaAzul = new Caixa();
    caixaAzul->tipo = CAIXA_AZUL;
    caixaAzul->cor = {0.0f, 0.0f, 1.0f};
    caixaAzul->x = -1.0f;
    caixaAzul->z = 0.0f;

    listaDeCaixas.push_back(caixaAzul);

    printf("Caixa Azul adicionada (Total: %lu)\n", listaDeCaixas.size());
    break;
  }
  case '3': {
    Caixa *caixaVerde = new Caixa();
    caixaVerde->tipo = CAIXA_VERDE;
    caixaVerde->cor = {0.0f, 1.0f, 0.0f};
    caixaVerde->x = -1.0f;
    caixaVerde->z = 0.0f;

    listaDeCaixas.push_back(caixaVerde);

    printf("Caixa Verde adicionada (Total: %lu)\n", listaDeCaixas.size());
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
  case 's':
  case 'S':
    anguloOmbro += 5.0f;
    break;
  case 'w':
  case 'W':
    anguloOmbro -= 5.0f;
    break;
  case 'a':
  case 'A':
    anguloCotovelo += 5.0f;
    break;
  case 'd':
  case 'D':
    anguloCotovelo -= 5.0f;
    break;
  case 'e':
  case 'E':
    anguloMao += 5.0f;
    break;
  case 'q':
  case 'Q':
    anguloMao -= 5.0f;
    break;
  case 'h':
  case 'H':
    menu();
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
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
  glutInitWindowSize(LARGURA_JANELA, ALTURA_JANELA);

  // Centraliza a janela no ecrã
  int larguraEcra = glutGet(GLUT_SCREEN_WIDTH);
  int alturaEcra = glutGet(GLUT_SCREEN_HEIGHT);
  glutInitWindowPosition((larguraEcra - LARGURA_JANELA) / 2,
                         (alturaEcra - ALTURA_JANELA) / 2);

  menu();

  glutCreateWindow("Robotic Arm Program");

  // Define a cor de fundo
  glClearColor(0.9f, 0.9f, 0.9f, 1.0f);

  // Ativa o teste de profundidade para renderização 3D
  glEnable(GL_DEPTH_TEST);

  // Registra os callbacks
  glutDisplayFunc(display);
  glutKeyboardFunc(teclado);

  glutMainLoop();
  return 0;
}
