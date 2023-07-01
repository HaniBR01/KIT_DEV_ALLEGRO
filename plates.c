#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#define NUM_PRATOS 8

const float FPS = 100;  

const int SCREEN_W = 960;
const int SCREEN_H = 540;

//largura do jogador
const float JOGADOR_W = 50;
const float JOGADOR_H = 100;

//Pratos e Hastes
const float HASTE_W = 10;
const float HASTE_H = 300;
const float PRATO_W = 50;
const float PRATO_H = 10;

typedef struct Jogador {
	float x;
	int equilibrando;
	int power;
	int mov_esq, mov_dir;
	ALLEGRO_COLOR cor;
	float vel;
} Jogador;
typedef struct Haste {
	float x;
	float y;
	// x = (n+1) * SCREEN_W / 9
	// y = SREEN_H - JOGADOR_H - HASTE_H/2
}Haste;
typedef struct Prato {
	float x;
	float y;
	int indice; //um indice virtual para desenhar os pratos de forma distribuida
	float energia;
	float tempoParaAparecer;
} Prato;

int equilibra(float x, Jogador *j,int max){
	if((j->equilibrando==1) && (j->x>x-HASTE_W/2)&&(j->x<x+HASTE_W/2) && (max))
		return 1;
	else
		return 0;
}

void desenha_cenario() {
	
	ALLEGRO_COLOR BKG_COLOR = al_map_rgb(150,210,167);
	//colore a tela de branco (rgb(255,255,255))
	al_clear_to_color(BKG_COLOR); 
	
}

void inicializa_hastes(Haste hastes[]){
	int i;
	for(i=0;i<NUM_PRATOS;i++){
		hastes[i].x = (i+1)*SCREEN_W/(NUM_PRATOS+1);
		hastes[i].y = SCREEN_H - JOGADOR_H - HASTE_H/2;
	}
}
void desenha_haste(Haste h[],Jogador *j, int max){
	int i, cor;
	for(i=0;i<NUM_PRATOS;i++){
		if(equilibra(h[i].x,j,max))
			cor = 255;
		else
			cor = 100;
	al_draw_filled_rectangle(h[i].x-HASTE_W/2,
							h[i].y-HASTE_H/2,
							h[i].x+HASTE_W/2,
							h[i].y+HASTE_H/2,
							al_map_rgb(100,100, cor));
	}
}

void desenha_jogador(Jogador j) {
	
	al_draw_filled_triangle(j.x, SCREEN_H - JOGADOR_H, 
							j.x - JOGADOR_W/2, SCREEN_H,
							j.x + JOGADOR_W/2, SCREEN_H,
							j.cor);	
	
}
void atualiza_jogador(Jogador *j) {
	if(j->mov_esq) {
		if(j->x - j->vel > 0+JOGADOR_W/2)
			j->x -= j->vel;
	}
	if(j->mov_dir) {
		if(j->x + j->vel < SCREEN_W-JOGADOR_W/2)
			j->x += j->vel;
	}	
}
void inicializa_jogador(Jogador *j) {
	j->x = SCREEN_W / 2;
	j->equilibrando = 0;
	j->cor = al_map_rgb(45, 123, 34);
	j->mov_esq = 0;
	j->mov_dir = 0;
	j->vel = 1;
	j->power =1;
}

float gera_tempo_prato(int i) {
	return i*1200;
}
void inicializa_pratos(Prato pratos[]) {
	int i;
	for(i=0;i<NUM_PRATOS;i++)
		if(i%2==NUM_PRATOS%2){
			pratos[i].indice=((int)NUM_PRATOS/2)-i/2;
		}else if(i%2!=NUM_PRATOS%2){
			pratos[i].indice=i/2+((int)NUM_PRATOS/2)+1;
		}

	for(i=0; i<NUM_PRATOS; i++) {
		pratos[i].x = pratos[i].indice*SCREEN_W/(NUM_PRATOS+1);
		pratos[i].y = SCREEN_H-HASTE_H-JOGADOR_H-PRATO_H/2;
		pratos[i].tempoParaAparecer = gera_tempo_prato(i);
		pratos[i].energia = 0;
	}
}
void desenha_pratos(Prato pratos[]){
	int i=0;
		for(i=0;i<NUM_PRATOS;i++)
			if(pratos[i].tempoParaAparecer==0)
				al_draw_filled_rectangle(pratos[i].x-PRATO_W/2,
							pratos[i].y-PRATO_H/2,
							pratos[i].x+PRATO_W/2,
							pratos[i].y+PRATO_H/2,
							al_map_rgb(255, (255-pratos[i].energia), (255-pratos[i].energia)));
}
void aumenta_energia(Prato pratos[]){
	int i;
	for(i=0;i<NUM_PRATOS;i++)
		if((pratos[i].tempoParaAparecer==0) && (pratos[i].energia<255.0))
			pratos[i].energia += 6*(1/FPS);
}
void atualiza_pratos(Prato pratos[],Jogador *j){
	int i;
	for(i=0;i<NUM_PRATOS;i++){
		if(pratos[i].tempoParaAparecer>0)
			pratos[i].tempoParaAparecer--;
		if(pratos[i].energia>=255.0)
			pratos[i].y+=1;
	}
}
void equilibra_prato(Prato pratos[], Jogador *j,int max){
	int i;
	for(i=0;i<NUM_PRATOS;i++)
		if((pratos[i].energia-(30*(1/FPS))>0.0) && equilibra(pratos[i].x,j,max))
			pratos[i].energia-= 30*(1/FPS);
}
void recuperar_todos_pratos(Prato pratos[]){
	int i;
	for(i=0;i<NUM_PRATOS;i++)
		pratos[i].energia=0;
}

int prato_caiu(Prato pratos[]){
	int i;
	for(i=0;i<NUM_PRATOS;i++){
		if(pratos[i].y>SCREEN_H-PRATO_H/2)
			return 1;
	}
	return 0;
} 
int energia_prato_max(Prato pratos[]){
	int i;
	for(i=0;i<NUM_PRATOS;i++){
		if(pratos[i].energia>=255)
			return 0;
	}
	return 1;
}
 
int main(int argc, char **argv){
	
	ALLEGRO_DISPLAY *display = NULL;	
	ALLEGRO_TIMER *timer = NULL;
	ALLEGRO_EVENT_QUEUE *event_queue = NULL;
	
	FILE *arquivo;
	long long int recorde;
	arquivo = fopen("platesScore.txt","r");
	if (arquivo == NULL)
      	printf("Erro ao abrir o arquivo.\n");
	fscanf(arquivo,"%lld",&recorde);
	fclose(arquivo);

	//inicializa o Allegro
	if(!al_init()) {
		fprintf(stderr, "failed to initialize allegro!\n");
		return -1;
	}
	
	//cria um temporizador que incrementa uma unidade a cada 1.0/FPS segundos
    timer = al_create_timer(1.0 / FPS);
    if(!timer) {
		fprintf(stderr, "failed to create timer!\n");
		return -1;
	}	
	
    //inicializa o módulo de primitivas do Allegro
    if(!al_init_primitives_addon()){
		fprintf(stderr, "failed to initialize primitives!\n");
        return -1;
    }	
	
	//inicializa o modulo que permite carregar imagens no jogo
	if(!al_init_image_addon()){
		fprintf(stderr, "failed to initialize image module!\n");
		return -1;
	}	
	
	//cria uma tela com dimensoes de SCREEN_W, SCREEN_H pixels
	display = al_create_display(SCREEN_W, SCREEN_H);
	if(!display) {
		fprintf(stderr, "failed to create display!\n");
		al_destroy_timer(timer);
		return -1;
	}	
	
	//instala o teclado
	if(!al_install_keyboard()) {
		fprintf(stderr, "failed to install keyboard!\n");
		return -1;
	}
	
	//inicializa o modulo allegro que carrega as fontes
	al_init_font_addon();

	//inicializa o modulo allegro que entende arquivos tff de fontes
	if(!al_init_ttf_addon()) {
		fprintf(stderr, "failed to load tff font module!\n");
		return -1;
	}
	
	//carrega o arquivo arial.ttf da fonte Arial e define que sera usado o tamanho 32 (segundo parametro)
    ALLEGRO_FONT *size_32 = al_load_font("arial.ttf", 32, 1);   
	if(size_32 == NULL) {
		fprintf(stderr, "font file does not exist or cannot be accessed!\n");
	}	
	
 	//cria a fila de eventos
	event_queue = al_create_event_queue();
	if(!event_queue) {
		fprintf(stderr, "failed to create event_queue!\n");
		al_destroy_display(display);
		al_destroy_timer(timer);
		return -1;
	}	
	
	//registra na fila os eventos de tela (ex: clicar no X na janela)
	al_register_event_source(event_queue, al_get_display_event_source(display));
	//registra na fila os eventos de tempo: quando o tempo altera de t para t+1
	al_register_event_source(event_queue, al_get_timer_event_source(timer));
	//registra na fila os eventos de teclado (ex: pressionar uma tecla)
	al_register_event_source(event_queue, al_get_keyboard_event_source()); 	
	
	
	//JOGADOR
	Jogador jogador;
	inicializa_jogador(&jogador);
	
	//PRATOS
	Haste hastes[NUM_PRATOS];
	inicializa_hastes(hastes);

	Prato pratos[NUM_PRATOS];
	inicializa_pratos(pratos);
	
	long long int score=0;
	char scoreS[50];
	int playing=1;
	
	//inicia o temporizador
	al_start_timer(timer);	
	
	while(playing) {
		
		ALLEGRO_EVENT ev;
		//espera por um evento e o armazena na variavel de evento ev
		al_wait_for_event(event_queue, &ev);
		
		//se o tipo de evento for um evento do temporizador, ou seja, se o tempo passou de t para t+1
		if(ev.type == ALLEGRO_EVENT_TIMER) {
			
			desenha_cenario();
			
			if(energia_prato_max(pratos)){
				score++;
				sprintf(scoreS,"%lld",score);
				al_draw_text(size_32,al_map_rgb(255,0,0),50.0,50.0,ALLEGRO_ALIGN_LEFT,scoreS);
			}
			sprintf(scoreS,"B to Recover all :%d",jogador.power);
			if(jogador.power && energia_prato_max(pratos))
				al_draw_text(size_32,al_map_rgb(255,0,0),900.0,50.0,ALLEGRO_ALIGN_RIGHT,scoreS);
			
			desenha_haste(hastes,&jogador,energia_prato_max(pratos));
			
			if(energia_prato_max(pratos))
				atualiza_jogador(&jogador);
			desenha_jogador(jogador);	

			atualiza_pratos(pratos,&jogador);
			aumenta_energia(pratos);
			equilibra_prato(pratos, &jogador,energia_prato_max(pratos));
			desenha_pratos(pratos);

			playing = !prato_caiu(pratos);
			
			if(!energia_prato_max(pratos)){
				sprintf(scoreS,"Score: %lld",score);
				al_draw_text(size_32,al_map_rgb(255,0,0),SCREEN_W/2,50.0,ALLEGRO_ALIGN_CENTRE,scoreS);
				sprintf(scoreS,"Recorde: %lld",recorde);
				al_draw_text(size_32,al_map_rgb(255,0,0),SCREEN_W/2,100.0,ALLEGRO_ALIGN_CENTRE,scoreS);			
			}

			//atualiza a tela (quando houver algo para mostrar)
			al_flip_display();
			
			if(al_get_timer_count(timer)%(int)FPS == 0)
				printf("\n%d segundos se passaram...", (int)(al_get_timer_count(timer)/FPS));
		}
		//se o tipo de evento for o fechamento da tela (clique no x da janela)
		else if(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
			playing = 0;
		}		
		//se o tipo de evento for um pressionar de uma tecla
		else if(ev.type == ALLEGRO_EVENT_KEY_DOWN) {
			//imprime qual tecla foi
			//printf("\ncodigo tecla: %d", ev.keyboard.keycode);
			
			if(ev.keyboard.keycode == ALLEGRO_KEY_SPACE) {
				if(!jogador.mov_dir && !jogador.mov_esq)
				jogador.equilibrando = 1;
			}
			else if(ev.keyboard.keycode == ALLEGRO_KEY_D) {
				jogador.mov_dir = 1;
				jogador.equilibrando = 0;
			}
			else if(ev.keyboard.keycode == ALLEGRO_KEY_A){
				jogador.mov_esq = 1;
				jogador.equilibrando = 0;
			}
			else if(ev.keyboard.keycode == ALLEGRO_KEY_B){
				if(energia_prato_max(pratos)&&jogador.power==1){
					jogador.power = 0;
					recuperar_todos_pratos(pratos);
				}
			}
		}
		//se o tipo de evento for soltar uma tecla
		else if(ev.type == ALLEGRO_EVENT_KEY_UP) {

			if(ev.keyboard.keycode == ALLEGRO_KEY_A) {
				jogador.mov_esq = 0;
			}
			else if(ev.keyboard.keycode == ALLEGRO_KEY_D) {
				jogador.mov_dir = 0;
			}	
			else if(ev.keyboard.keycode == ALLEGRO_KEY_SPACE){
				jogador.equilibrando = 0;
			}		
		}		
			
	}
	
	if(score>recorde)
		recorde=score;
	arquivo = fopen("platesScore.txt","w");
	if (arquivo == NULL)
		printf("Erro ao abrir o arquivo.\n");
	fprintf(arquivo,"%lld",recorde);
	fclose(arquivo);

	al_destroy_timer(timer);
	al_destroy_display(display);
	al_destroy_event_queue(event_queue);
	
 
	return 0;
}