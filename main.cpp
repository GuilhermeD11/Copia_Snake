#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include "structs.h"

//GLOBALS==============================
const int WIDTH = 1200;
const int HEIGHT = 864;
const int TAM_COBRA = 8;
const int TAM_CAUDA = 4000;
enum KEYS
{
    UP,
    DOWN,
    LEFT,
    RIGHT,
};

enum STATE
{
    TITLE,
    PLAYING,
    LOST
};

bool keys[4] = {false, false, false, false};

//prototypes

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

void changeState(int &state, int newState);

int verificaColisao();

//primitive variable
bool done = false;
bool redraw = true;
const int FPS = 60;
bool isGameOver = false;

//object variables
int DIR = DOWN;
Cobra cobra;
Comida comidas;
Cobra cauda[TAM_CAUDA];
int curFrame = 2;
int frameHeigth = 8;
int frameWidth = 8;
int state = -1;

//Allegro variables
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

int main(void)
{

    //Initialization Functions
    if (!al_init()) //initialize Allegro
        return -1;

    display = al_create_display(WIDTH, HEIGHT); //create our display object

    if (!display) //test display object
        return -1;

    al_init_primitives_addon();
    al_install_keyboard();
    al_init_font_addon();
    al_init_ttf_addon();
    al_init_image_addon();
    al_install_audio();
    al_init_acodec_addon();

    al_reserve_samples(1);

    event_queue = al_create_event_queue();
    timer = al_create_timer(1.0 / FPS);

    srand(time(NULL));
    IniciarCobra();
    IniciarComida();

    icobra = al_load_bitmap("assets/cobra.png");
    icomida = al_load_bitmap("assets/comida.png");
    paredes = al_load_bitmap("assets/paredes.png");

    sample = al_load_sample("assets/som.ogg");

    al_play_sample(sample, 1, 0, 1, ALLEGRO_PLAYMODE_LOOP, NULL);

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

    changeState(state, TITLE);

    loopJogo();

    al_destroy_event_queue(event_queue);
    al_destroy_timer(timer);
    al_destroy_font(font18);
    al_destroy_font(font36);
    al_destroy_font(font72);
    al_destroy_display(display); //destroy our display object
    al_destroy_bitmap(icobra);
    al_destroy_bitmap(icomida);
    al_destroy_bitmap(paredes);
    al_destroy_sample(sample);

    return 0;
}

void comecoJogo()
{
    desenharComida();
    desenharCobra();
    Movimento();
    desenharParede();
}
void loopJogo()
{

    while (!done)
    {

        ALLEGRO_EVENT ev;
        al_wait_for_event(event_queue, &ev);

        if (state == PLAYING)
        {

            comecoJogo();
        }

        if (ev.type == ALLEGRO_EVENT_TIMER)
        {
            redraw = true;
            if (keys[UP])
            {
                if (DIR != DOWN)
                {
                    DIR = UP;
                    curFrame = 0;
                }
            }
            if (keys[DOWN])
            {
                if (DIR != UP)
                {
                    DIR = DOWN;
                    curFrame = 2;
                }
            }
            if (keys[LEFT])
            {
                if (DIR != RIGHT)
                {
                    DIR = LEFT;
                    curFrame = 1;
                }
            }
            if (keys[RIGHT])
            {
                if (DIR != LEFT)
                {
                    DIR = RIGHT;
                    curFrame = 3;
                }
            }

            if (!isGameOver)
            {

                if (cobra.vida <= 0)
                    isGameOver = true;
            }
        }
        else if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
        {
            done = true;
        }
        else if (ev.type == ALLEGRO_EVENT_KEY_DOWN)
        {
            switch (ev.keyboard.keycode)
            {
            case ALLEGRO_KEY_ESCAPE:
                done = true;
                break;
            case ALLEGRO_KEY_UP:
                keys[UP] = true;
                keys[DOWN] = false;
                keys[LEFT] = false;
                keys[RIGHT] = false;

                break;
            case ALLEGRO_KEY_DOWN:
                keys[UP] = false;
                keys[DOWN] = true;
                keys[LEFT] = false;
                keys[RIGHT] = false;

                break;
            case ALLEGRO_KEY_LEFT:
                keys[UP] = false;
                keys[DOWN] = false;
                keys[LEFT] = true;
                keys[RIGHT] = false;
                break;
            case ALLEGRO_KEY_RIGHT:
                keys[UP] = false;
                keys[DOWN] = false;
                keys[LEFT] = false;
                keys[RIGHT] = true;
                break;
            case ALLEGRO_KEY_C:
                if (state == TITLE)
                {
                    changeState(state, PLAYING);
                }

                if (state == LOST)
                {
                    IniciarComida();
                    changeState(state, PLAYING);
                }
                break;
            }
        }

        if (redraw && al_is_event_queue_empty(event_queue))
        {
            redraw = false;

            if (state == TITLE)
            {
                al_draw_textf(font72, al_map_rgb(0, 255, 0), WIDTH / 2, HEIGHT / 2 - 72, ALLEGRO_ALIGN_CENTER, "SNAKE");
                al_draw_textf(font36, al_map_rgb(255, 255, 255), WIDTH / 2, HEIGHT / 2, ALLEGRO_ALIGN_CENTER, "APERTE C PARA INICIAR");
            }

            if (state == PLAYING)
            {
                verificaColisao();
                desenharPontuacao();
                desenharComida();
            }

            if (state == LOST)
            {

                al_draw_textf(font36, al_map_rgb(255, 255, 255), WIDTH / 2, HEIGHT / 2, ALLEGRO_ALIGN_CENTER, "APERTE C PARA REINICIAR");
                al_draw_textf(font36, al_map_rgb(255, 255, 255), WIDTH / 2, HEIGHT / 2 + 40, ALLEGRO_ALIGN_CENTER, "PONTOS: %d", comidas.pontuacao);
            }

            al_flip_display();
            al_clear_to_color(al_map_rgb(0, 0, 0));
        }

        if (isGameOver)
        {

            for (int i = 1; i <= cobra.tam; i++)
            {
                cauda[i].x = -TAM_COBRA;
                cauda[i].y = -TAM_COBRA;
            }

            IniciarCobra();
            changeState(state, LOST);
            isGameOver = false;
        }
    }
}

void desenharParede()
{

    for (int i = 0; i < HEIGHT / TAM_COBRA; i++)
    {
        al_draw_bitmap_region(paredes, 1 * frameWidth, 0, frameWidth, frameHeigth, 0, TAM_COBRA * i, 0);
        al_draw_bitmap_region(paredes, 2 * frameWidth, 0, frameWidth, frameHeigth, WIDTH - TAM_COBRA, TAM_COBRA * i, 0);
    }
    for (int i = 0; i < WIDTH / TAM_COBRA; i++)
    {
        al_draw_bitmap_region(paredes, 0 * frameWidth, 0, frameWidth, frameHeigth, TAM_COBRA * i, 0, 0);
        al_draw_bitmap_region(paredes, 3 * frameWidth, 0, frameWidth, frameHeigth, TAM_COBRA * i, HEIGHT - 8, 0);
        al_draw_bitmap_region(paredes, 4 * frameWidth, 0, frameWidth, frameHeigth, TAM_COBRA * i, TAM_COBRA * 3, 0);
    }
    al_draw_bitmap_region(paredes, 6 * frameWidth, 0, frameWidth, frameHeigth, 0, 0, 0);
    al_draw_bitmap_region(paredes, 7 * frameWidth, 0, frameWidth, frameHeigth, WIDTH - TAM_COBRA, 0, 0);
    al_draw_bitmap_region(paredes, 7 * frameWidth, 0, frameWidth, frameHeigth, 0, HEIGHT - TAM_COBRA, 0);
    al_draw_bitmap_region(paredes, 6 * frameWidth, 0, frameWidth, frameHeigth, WIDTH - TAM_COBRA, HEIGHT - TAM_COBRA, 0);
    al_draw_bitmap_region(paredes, 6 * frameWidth, 0, frameWidth, frameHeigth, 0, TAM_COBRA * 3, 0);
    al_draw_bitmap_region(paredes, 7 * frameWidth, 0, frameWidth, frameHeigth, WIDTH - TAM_COBRA, TAM_COBRA * 3, 0);
}

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
void desenharCobra()
{
    for (int i = 0; i <= cobra.tam; i++)
    {
        al_draw_bitmap_region(icobra, 4 * frameWidth, 0, frameWidth, frameHeigth, cauda[i].x, cauda[i].y, 0);
    }
    al_draw_bitmap_region(icobra, curFrame * frameWidth, 0, frameWidth, frameHeigth, cauda[0].x, cauda[0].y, 0);
}

void Movimento()
{

    for (int i = cobra.tam; i > 0; i--)
    {
        cauda[i] = cauda[i - 1];
    }
    switch (DIR)
    {
    case UP:

        cauda[0].y -= cobra.velocidade;
        if (cauda[0].y < (TAM_COBRA * 3 + TAM_COBRA))
            cauda[0].y = (TAM_COBRA * 3 + TAM_COBRA);
        break;
    case DOWN:

        cauda[0].y += cobra.velocidade;
        if (cauda[0].y > HEIGHT - TAM_COBRA * 2)
            cauda[0].y = HEIGHT - TAM_COBRA * 2;
        break;
    case LEFT:

        cauda[0].x -= cobra.velocidade;
        if (cauda[0].x < TAM_COBRA)
            cauda[0].x = TAM_COBRA;
        break;
    case RIGHT:

        cauda[0].x += cobra.velocidade;
        if (cauda[0].x > WIDTH - TAM_COBRA * 2)
            cauda[0].x = WIDTH - TAM_COBRA * 2;
        break;

    default:
        break;
    }
}

void IniciarComida()
{
    comidas.vida = false;
    comidas.pontuacao = 0;
}
void desenharComida()
{

    if (comidas.vida == false)
    {
        comidas.x = (rand() % (WIDTH / TAM_COBRA - 2)) + 1;
        comidas.y = (rand() % (HEIGHT / TAM_COBRA - 5)) + 4;
        al_draw_bitmap(icomida, comidas.x * TAM_COBRA, comidas.y * TAM_COBRA, 0);
        comidas.vida = true;
    }
    al_draw_bitmap(icomida, comidas.x * TAM_COBRA, comidas.y * TAM_COBRA, 0);
}

void desenharPontuacao()
{
    al_draw_textf(font18, al_map_rgb(255, 255, 255), 5, 5, 0, "Pontos: %i ", comidas.pontuacao);
}

int verificaColisao()
{

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

    if (cauda[0].x + TAM_COBRA >= WIDTH - TAM_COBRA || cauda[0].x <= TAM_COBRA || cauda[0].y <= TAM_COBRA * 3 + 8 || cauda[0].y + TAM_COBRA >= HEIGHT - TAM_COBRA)
    {

        isGameOver = true;
    }

    for (int i = 8; i <= cobra.tam; i++)
    {

        if (cauda[0].x == cauda[i + 1].x && cauda[0].y == cauda[i].y + 8)
        {

            isGameOver = true;
        }
    }

    return 0;
}

void desenharTelaFinal()
{
    al_draw_filled_rectangle(0, 0, WIDTH, HEIGHT, al_map_rgb(255, 255, 255));
    al_draw_textf(font18, al_map_rgb(255, 255, 255), 5, 5, 0, "Pontos: %i ", comidas.pontuacao);
    al_draw_textf(font18, al_map_rgb(255, 255, 255), 55, 55, 0, " %d %d ", comidas.x * TAM_COBRA, comidas.y * TAM_COBRA);
    al_draw_textf(font18, al_map_rgb(255, 255, 255), 105, 105, 0, " %d %d ", cauda[0].x, cauda[0].y);
}

void changeState(int &state, int newState)
{
    state = newState;
}