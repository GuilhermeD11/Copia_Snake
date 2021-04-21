#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include "structs.h"

//GLOBALS==============================
const int WIDTH = 1250;
const int HEIGHT = 900;
const int TAM_COBRA = 50;
const int TAM_CAUDA = 400;
enum KEYS
{
    UP,
    DOWN,
    LEFT,
    RIGHT,
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

void MoverCima();
void MoverBaixo();
void MoverEsquerda();
void MoverDireita();

int verificaColisao(Cobra cobra, Comida &comida, int largura, int altura);

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

//Allegro variables
ALLEGRO_DISPLAY *display = NULL;
ALLEGRO_EVENT_QUEUE *event_queue = NULL;
ALLEGRO_TIMER *timer = NULL;
ALLEGRO_FONT *font18 = NULL;

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

    event_queue = al_create_event_queue();
    timer = al_create_timer(1.0 / FPS);

    srand(time(NULL));
    IniciarCobra();
    IniciarComida();

    font18 = al_load_font("font.ttf", 18, 0);
    if (!font18)
        return -1;

    al_register_event_source(event_queue, al_get_keyboard_event_source());
    al_register_event_source(event_queue, al_get_timer_event_source(timer));
    al_register_event_source(event_queue, al_get_display_event_source(display));

    al_start_timer(timer);

    loopJogo();

    al_destroy_event_queue(event_queue);
    al_destroy_timer(timer);
    al_destroy_font(font18);
    al_destroy_display(display); //destroy our display object

    return 0;
}

void comecoJogo()
{
    desenharParede();
    desenharCobra();
    desenharComida();
    Movimento();
}
void loopJogo()
{

    while (!done)
    {
        ALLEGRO_EVENT ev;
        al_wait_for_event(event_queue, &ev);

        comecoJogo();

        if (ev.type == ALLEGRO_EVENT_TIMER)
        {
            redraw = true;
            if (keys[UP])
            {
                if (DIR != DOWN)
                    DIR = UP;
            }
            if (keys[DOWN])
            {
                if (DIR != UP)
                    DIR = DOWN;
            }
            if (keys[LEFT])
            {
                if (DIR != RIGHT)
                    DIR = LEFT;
            }
            if (keys[RIGHT])
            {
                if (DIR != LEFT)
                    DIR = RIGHT;
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
                break;
            case ALLEGRO_KEY_DOWN:
                keys[DOWN] = true;
                break;
            case ALLEGRO_KEY_LEFT:
                keys[LEFT] = true;
                break;
            case ALLEGRO_KEY_RIGHT:
                keys[RIGHT] = true;
                break;
            case ALLEGRO_KEY_D:
                comidas.vida = false;
                cobra.tam++;
                break;
            }
        }
        else if (ev.type == ALLEGRO_EVENT_KEY_UP)
        {
            switch (ev.keyboard.keycode)
            {
            case ALLEGRO_KEY_ESCAPE:
                done = true;
                break;
            case ALLEGRO_KEY_UP:
                keys[UP] = false;
                break;
            case ALLEGRO_KEY_DOWN:
                keys[DOWN] = false;
                break;
            case ALLEGRO_KEY_LEFT:
                keys[LEFT] = false;
                break;
            case ALLEGRO_KEY_RIGHT:
                keys[RIGHT] = false;
                break;
            }
        }

        if (redraw && al_is_event_queue_empty(event_queue))
        {
            redraw = false;

            if (!isGameOver)
            {
                desenharPontuacao();
                desenharComida;
                verificaColisao(cobra, comidas, WIDTH, HEIGHT);
            }
            else
            {
            }

            al_flip_display();
            al_clear_to_color(al_map_rgb(0, 0, 0));
        }
    }
}
void fimJogo();

void desenharParede()
{
    al_draw_rectangle(TAM_COBRA / 2, TAM_COBRA / 2, WIDTH - (TAM_COBRA / 2), HEIGHT - (TAM_COBRA / 2), al_map_rgb(100, 100, 100), TAM_COBRA);
}

void IniciarCobra()
{
    cauda[0].x = 500;
    cauda[0].y = 200;
    cobra.vida = 500;
    cobra.velocidade = 5;
    cobra.tam = 2;
    for (int i = 1; i <= cobra.tam; i++)
    {
        cauda[i].x = cobra.x;
        cauda[i].y = cobra.y;
    }
}
void desenharCobra()
{
    for (int i = 0; i <= cobra.tam; i++)
    {
        al_draw_filled_rectangle(cauda[i].x, cauda[i].y, cauda[i].x + TAM_COBRA, cauda[i].y + TAM_COBRA, al_map_rgb(0, 255, 0));
    }
    al_draw_filled_rectangle(cauda[0].x, cauda[0].y, cauda[0].x + TAM_COBRA, cauda[0].y + TAM_COBRA, al_map_rgb(0, 200, 0));
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
        if (cauda[0].y < TAM_COBRA)
            cauda[0].y = TAM_COBRA;
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
    comidas.pontuacao = 10;
}
void desenharComida()
{

    if (comidas.vida == false)
    {
        comidas.x = (rand() % (WIDTH / TAM_COBRA - 2)) + 1;
        comidas.y = (rand() % (HEIGHT / TAM_COBRA - 3)) + 1;
        al_draw_filled_rectangle(comidas.x * TAM_COBRA, comidas.y * TAM_COBRA, comidas.x * TAM_COBRA + TAM_COBRA, comidas.y * TAM_COBRA + TAM_COBRA, al_map_rgb(255, 0, 0));
        comidas.vida = true;
    }
    al_draw_filled_rectangle(comidas.x * TAM_COBRA, comidas.y * TAM_COBRA, comidas.x * TAM_COBRA + TAM_COBRA, comidas.y * TAM_COBRA + TAM_COBRA, al_map_rgb(255, 0, 0));
}

void desenharPontuacao()
{
    al_draw_textf(font18, al_map_rgb(0, 0, 0), 5, 5, 0, "Pontuacao %i", comidas.pontuacao);
}

int verificaColisao(Cobra cobra, Comida &comida, int largura, int altura){

    if(comida.x == cobra.x){

        al_draw_textf(font18, al_map_rgb(0, 0, 0), 5, 5, 0, "Colidiu");
        comida.pontuacao++;
        comida.vida = false;
        cobra.tam += 50;

    }



}
