#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include "structs.h"

//ENUMERANDO VARIAVEIS

enum DIRECOES
{
    CIMA,
    BAIXO,
    ESQUERDA,
    DIREITA,
};

enum estado
{
    COMECO,
    JOGO,
    FIM
};

//VARIAVEIS GLOBAIS
Cobra cobra;
Comida comidas;
Cobra cauda[TAM_CAUDA];

const int largura = 1200;
const int altura = 864;
const int TAM_COBRA = 8;
const int TAM_CAUDA = 4000;
const int FPS = 60;

bool controle = false;
bool redesenhar = true;
bool fimDeJogo = false;

int DIR = BAIXO;
int direcaoAtual = 2;
int tamQuadrado = 8;
int estado = -1;

bool direcoes[4] = {false, false, false, false};

//DECLARANDO AS FUNCOES

void comecoJogo();
void loopJogo();
void fimJogo();

void desenharParede();

void IniciarCobra();
void desenharCobra();

void Movimento();

void IniciarComida();
void desenharComida();

void desenharPontuacao();
void desenharTelaFinal();

void MoverCima();
void MoverBaixo();
void MoverEsquerda();
void MoverDireita();

void trocarEstado(int &estado, int novoEstado);

int verificaColisao();


//VARIAVEIS DO ALLEGRO
ALLEGRO_DISPLAY *display = NULL;
ALLEGRO_EVENT_QUEUE *event_queue = NULL;
ALLEGRO_TIMER *timer = NULL;
ALLEGRO_FONT *font18 = NULL;
ALLEGRO_FONT *font36 = NULL;
ALLEGRO_FONT *font72 = NULL;
ALLEGRO_BITMAP *icobra = NULL;
ALLEGRO_BITMAP *icomida = NULL;
ALLEGRO_BITMAP *paredes = NULL;
ALLEGRO_SAMPLE *sample = NULL;


//FUNCAO INICIAL
int main(void)
{

    //INICIALIZANDO ALLEGRO
    if (!al_init())
        return -1;

    //CRIANDO DISPLAY
    display = al_create_display(largura, altura);

    if (!display)
        return -1;

    
    //INICIALIZANDO ALGUMAS FUNCOES DO ALLEGRO
    al_init_primitives_addon();
    al_install_keyboard();
    al_init_font_addon();
    al_init_ttf_addon();
    al_init_image_addon();
    al_install_audio();
    al_init_acodec_addon();

    al_reserve_samples(1);

    event_queue = al_create_event_queue();

    //CRIANDO O TIMER
    timer = al_create_timer(1.0 / FPS);

    //CRIANDO SEMENTE PARA NÚMEROS ALEATÓRIOS
    srand(time(NULL));

    //INICIANDO COBRA E COMIDA
    IniciarCobra();
    IniciarComida();

    //CARREGANDO SPRITES
    icobra = al_load_bitmap("assets/cobra.png");
    icomida = al_load_bitmap("assets/comida.png");
    paredes = al_load_bitmap("assets/paredes.png");

    //CARREGANDO SOM
    sample = al_load_sample("assets/som.ogg");

    //TOCANDO SOM
    al_play_sample(sample, 1, 0, 1, ALLEGRO_PLAYMODE_LOOP, NULL);

    //CARREGANDO FONTES
    font18 = al_load_font("font.ttf", 18, 0);
    font36 = al_load_font("font.ttf", 36, 0);
    font72 = al_load_font("font.ttf", 72, 0);
    if (!font18)
        return -1;
    if (!font36)
        return -1;
    if (!font72)
        return -1;

    al_register_event_source(event_queue, al_get_keyboard_event_source());
    al_register_event_source(event_queue, al_get_timer_event_source(timer));
    al_register_event_source(event_queue, al_get_display_event_source(display));

    al_start_timer(timer);

    //INICIANDO O JOGO E ENTRANDO NO LOOP
    trocarEstado(estado, COMECO);
    loopJogo();

    //DESTRUINDO EVENTOS, FONTES, DISPLAYS, BITMAPS E SOM
    al_destroy_event_queue(event_queue);
    al_destroy_timer(timer);
    al_destroy_font(font18);
    al_destroy_font(font36);
    al_destroy_font(font72);
    al_destroy_display(display);
    al_destroy_bitmap(icobra);
    al_destroy_bitmap(icomida);
    al_destroy_bitmap(paredes);
    al_destroy_sample(sample);

    return 0;
}

//FUNCAO PRINCIPAL DO JOGO
void loopJogo()
{

    //LOOP DO JOGO
    while (!controle)
    {

        ALLEGRO_EVENT ev;
        al_wait_for_event(event_queue, &ev);

        //COMECANDO O JOGO
        if (estado == JOGO)
        {
            comecoJogo();
        }

        //VERIFICANDO TECLAS PRESSIONADAS
        if (ev.type == ALLEGRO_EVENT_TIMER)
        {
            redesenhar = true;
            if (direcoes[CIMA])
            {
                if (DIR != BAIXO)
                {
                    DIR = CIMA;
                    direcaoAtual = 0;
                }
            }
            if (direcoes[BAIXO])
            {
                if (DIR != CIMA)
                {
                    DIR = BAIXO;
                    direcaoAtual = 2;
                }
            }
            if (direcoes[ESQUERDA])
            {
                if (DIR != DIREITA)
                {
                    DIR = ESQUERDA;
                    direcaoAtual = 1;
                }
            }
            if (direcoes[DIREITA])
            {
                if (DIR != ESQUERDA)
                {
                    DIR = DIREITA;
                    direcaoAtual = 3;
                }
            }

            if (!fimDeJogo)
            {

                if (cobra.vida <= 0)
                    fimDeJogo = true;
            }
        }
        else if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
        {
            controle = true;
        }
        else if (ev.type == ALLEGRO_EVENT_KEY_DOWN)
        {
            switch (ev.keyboard.keycode)
            {
            case ALLEGRO_KEY_ESCAPE:
                controle = true;
                break;
            case ALLEGRO_KEY_UP:
                direcoes[CIMA] = true;
                direcoes[BAIXO] = false;
                direcoes[ESQUERDA] = false;
                direcoes[DIREITA] = false;

                break;
            case ALLEGRO_KEY_DOWN:
                direcoes[CIMA] = false;
                direcoes[BAIXO] = true;
                direcoes[ESQUERDA] = false;
                direcoes[DIREITA] = false;

                break;
            case ALLEGRO_KEY_LEFT:
                direcoes[CIMA] = false;
                direcoes[BAIXO] = false;
                direcoes[ESQUERDA] = true;
                direcoes[DIREITA] = false;
                break;
            case ALLEGRO_KEY_RIGHT:
                direcoes[CIMA] = false;
                direcoes[BAIXO] = false;
                direcoes[ESQUERDA] = false;
                direcoes[DIREITA] = true;
                break;
            case ALLEGRO_KEY_C:
                if (estado == COMECO)
                {
                    trocarEstado(estado, JOGO);
                }

                if (estado == FIM)
                {
                    IniciarComida();
                    trocarEstado(estado, JOGO);
                }
                break;
            }
        }

        //DESENHANDO, VERIFICANDO ESTADO E COLISÃO
        if (redesenhar && al_is_event_queue_empty(event_queue))
        {
            redesenhar = false;

            if (estado == COMECO)
            {
                al_draw_textf(font72, al_map_rgb(0, 255, 0), largura / 2, altura / 2 - 72, ALLEGRO_ALIGN_CENTER, "SNAKE");
                al_draw_textf(font36, al_map_rgb(255, 255, 255), largura / 2, altura / 2, ALLEGRO_ALIGN_CENTER, "APERTE C PARA INICIAR");
            }

            if (estado == JOGO)
            {
                verificaColisao();
                desenharPontuacao();
                desenharComida();
            }

            if (estado == FIM)
            {

                al_draw_textf(font36, al_map_rgb(255, 255, 255), largura / 2, altura / 2, ALLEGRO_ALIGN_CENTER, "APERTE C PARA REINICIAR");
                al_draw_textf(font36, al_map_rgb(255, 255, 255), largura / 2, altura / 2 + 40, ALLEGRO_ALIGN_CENTER, "PONTOS: %d", comidas.pontuacao);
            }

            al_flip_display();
            al_clear_to_color(al_map_rgb(0, 0, 0));
        }

        //FIM DO JOGO
        if (fimDeJogo)
        {

            for (int i = 1; i <= cobra.tam; i++)
            {
                cauda[i].x = -TAM_COBRA;
                cauda[i].y = -TAM_COBRA;
            }

            IniciarCobra();
            trocarEstado(estado, FIM);
            fimDeJogo = false;
        }
    }
}

//FUNÇÃO PARA INICIAR O JOGO
void comecoJogo()
{
    desenharComida();
    desenharCobra();
    Movimento();
    desenharParede();
}

//FUNÇÃO PARA DESENHAR PAREDE
void desenharParede()
{

    for (int i = 0; i < altura / TAM_COBRA; i++)
    {
        al_draw_bitmap_region(paredes, 1 * tamQuadrado, 0, tamQuadrado, tamQuadrado, 0, TAM_COBRA * i, 0);
        al_draw_bitmap_region(paredes, 2 * tamQuadrado, 0, tamQuadrado, tamQuadrado, largura - TAM_COBRA, TAM_COBRA * i, 0);
    }
    for (int i = 0; i < largura / TAM_COBRA; i++)
    {
        al_draw_bitmap_region(paredes, 0 * tamQuadrado, 0, tamQuadrado, tamQuadrado, TAM_COBRA * i, 0, 0);
        al_draw_bitmap_region(paredes, 3 * tamQuadrado, 0, tamQuadrado, tamQuadrado, TAM_COBRA * i, altura - 8, 0);
        al_draw_bitmap_region(paredes, 4 * tamQuadrado, 0, tamQuadrado, tamQuadrado, TAM_COBRA * i, TAM_COBRA * 3, 0);
    }
    al_draw_bitmap_region(paredes, 6 * tamQuadrado, 0, tamQuadrado, tamQuadrado, 0, 0, 0);
    al_draw_bitmap_region(paredes, 7 * tamQuadrado, 0, tamQuadrado, tamQuadrado, largura - TAM_COBRA, 0, 0);
    al_draw_bitmap_region(paredes, 7 * tamQuadrado, 0, tamQuadrado, tamQuadrado, 0, altura - TAM_COBRA, 0);
    al_draw_bitmap_region(paredes, 6 * tamQuadrado, 0, tamQuadrado, tamQuadrado, largura - TAM_COBRA, altura - TAM_COBRA, 0);
    al_draw_bitmap_region(paredes, 6 * tamQuadrado, 0, tamQuadrado, tamQuadrado, 0, TAM_COBRA * 3, 0);
    al_draw_bitmap_region(paredes, 7 * tamQuadrado, 0, tamQuadrado, tamQuadrado, largura - TAM_COBRA, TAM_COBRA * 3, 0);
}

//FUNÇÃO PARA DEFINIR PARÂMETROS INICAIS DA COBRA
void IniciarCobra()
{
    cauda[0].x = 200;
    cauda[0].y = 144;
    cobra.vida = 500;
    cobra.velocidade = 2;
    cobra.tam = 5;
    for (int i = 1; i <= cobra.tam; i++)
    {
        cauda[i].x = cauda[0].x;
        cauda[i].y = cauda[0].y;
    }
}

//FUNÇÃO PARA DESENHAR A COBRA
void desenharCobra()
{
    for (int i = 0; i <= cobra.tam; i++)
    {
        al_draw_bitmap_region(icobra, 4 * tamQuadrado, 0, tamQuadrado, tamQuadrado, cauda[i].x, cauda[i].y, 0);
    }
    al_draw_bitmap_region(icobra, direcaoAtual * tamQuadrado, 0, tamQuadrado, tamQuadrado, cauda[0].x, cauda[0].y, 0);
}

//FUNÇÃO PARA CONTROLAR O MOVIMENTO
void Movimento()
{

    for (int i = cobra.tam; i > 0; i--)
    {
        cauda[i] = cauda[i - 1];
    }
    switch (DIR)
    {
    case CIMA:

        cauda[0].y -= cobra.velocidade;
        if (cauda[0].y < (TAM_COBRA * 3 + TAM_COBRA))
            cauda[0].y = (TAM_COBRA * 3 + TAM_COBRA);
        break;
    case BAIXO:

        cauda[0].y += cobra.velocidade;
        if (cauda[0].y > altura - TAM_COBRA * 2)
            cauda[0].y = altura - TAM_COBRA * 2;
        break;
    case ESQUERDA:

        cauda[0].x -= cobra.velocidade;
        if (cauda[0].x < TAM_COBRA)
            cauda[0].x = TAM_COBRA;
        break;
    case DIREITA:

        cauda[0].x += cobra.velocidade;
        if (cauda[0].x > largura - TAM_COBRA * 2)
            cauda[0].x = largura - TAM_COBRA * 2;
        break;

    default:
        break;
    }
}

//FUNÇÃO PARA DEFINIR PARÂMETROS INICAIS DA COMIDA
void IniciarComida()
{
    comidas.vida = false;
    comidas.pontuacao = 0;
}

//FUNÇÃO PARA DESENHAR A COMIDA
void desenharComida()
{

    if (comidas.vida == false)
    {
        comidas.x = (rand() % (largura / TAM_COBRA - 2)) + 1;
        comidas.y = (rand() % (altura / TAM_COBRA - 5)) + 4;
        al_draw_bitmap(icomida, comidas.x * TAM_COBRA, comidas.y * TAM_COBRA, 0);
        comidas.vida = true;
    }
    al_draw_bitmap(icomida, comidas.x * TAM_COBRA, comidas.y * TAM_COBRA, 0);
}

//FUNÇÃO PARA DESENHAR A PONTUAÇÃO
void desenharPontuacao()
{
    al_draw_textf(font18, al_map_rgb(255, 255, 255), 5, 5, 0, "Pontos: %i ", comidas.pontuacao);
}

//FUNÇÃO PARA VERIFICAR A COLISÃO
int verificaColisao()
{

    //COLISÃO COM A COMIDA
    if (cauda[0].x >= comidas.x * TAM_COBRA && cauda[0].x <= comidas.x * TAM_COBRA + 8 && cauda[0].y >= comidas.y * TAM_COBRA && cauda[0].y <= comidas.y * TAM_COBRA + 8)
    {

        comidas.pontuacao++;
        comidas.vida = false;
        cobra.tam += 10;
    }
    else if (cauda[0].x + 8 >= comidas.x * TAM_COBRA && cauda[0].x + 8 <= comidas.x * TAM_COBRA + 8 && cauda[0].y + 8 >= comidas.y * TAM_COBRA && cauda[0].y + 8 <= comidas.y * TAM_COBRA + 8)
    {

        comidas.pontuacao++;
        comidas.vida = false;
        cobra.tam += 10;
    }

    //COLISÃO COM A PAREDE
    if (cauda[0].x + TAM_COBRA >= largura - TAM_COBRA || cauda[0].x <= TAM_COBRA || cauda[0].y <= TAM_COBRA * 3 + 8 || cauda[0].y + TAM_COBRA >= altura - TAM_COBRA)
    {

        fimDeJogo = true;
    }

    //COLISÃO COM A CAUDA
    for (int i = 8; i <= cobra.tam; i++)
    {

        if (cauda[0].x == cauda[i + 1].x && cauda[0].y == cauda[i].y + 8)
        {

            fimDeJogo = true;
        }
    }

    return 0;
}

//FUNÇÃO PARA DESENHAR A TELA FINAL
void desenharTelaFinal()
{
    al_draw_filled_rectangle(0, 0, largura, altura, al_map_rgb(255, 255, 255));
    al_draw_textf(font18, al_map_rgb(255, 255, 255), 5, 5, 0, "Pontos: %i ", comidas.pontuacao);
    al_draw_textf(font18, al_map_rgb(255, 255, 255), 55, 55, 0, " %d %d ", comidas.x * TAM_COBRA, comidas.y * TAM_COBRA);
    al_draw_textf(font18, al_map_rgb(255, 255, 255), 105, 105, 0, " %d %d ", cauda[0].x, cauda[0].y);
}

//FUNÇÃO PARA TROCAR DE ESTADO
void trocarEstado(int &estado, int novoEstado)
{
    estado = novoEstado;
}