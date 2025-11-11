#ifndef SIMULADOR_H
#define SIMULADOR_H

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <limits.h>

typedef struct {
    unsigned int pagina_virtual_mapeada;
    int valido;
    int suja;
    unsigned long long ultimo_acesso;
    unsigned int frequencia;
} Quadro;

extern Quadro* memoria_fisica;
extern unsigned int num_quadros;
extern unsigned int s;
extern unsigned long long contador_tempo;
extern unsigned int modo_depuracao_global;
extern unsigned int num_pags_sujas_global;

unsigned int calcular_s(unsigned int tam_pags_kb);

void inicializar_memoria_fisica(void);

int encontrar_quadro_livre(void);

int substitui_random(void);

int substitui_lru(void);

int substitui_lfu(void);

int substitui_mru(void);

void* alocar_tabela_densa(unsigned long num_paginas, unsigned long* custo_memoria);
int lookup_tabela_densa(void* tabela, unsigned int num_pagina);
void atualizar_tabela_densa(void* tabela, unsigned int num_pagina, int quadro);
void liberar_tabela_densa(void* tabela);

void* alocar_tabela_hierarquica_2n(unsigned int bits_l1, unsigned int bits_l2, unsigned long* custo_memoria);
int lookup_tabela_hierarquica_2n(void* tabela, unsigned int num_pagina, unsigned int bits_l1, unsigned int bits_l2);
void atualizar_tabela_hierarquica_2n(void* tabela, unsigned int num_pagina, int quadro, unsigned int bits_l1, unsigned int bits_l2);
void liberar_tabela_hierarquica_2n(void* tabela, unsigned int bits_l1);

void* alocar_tabela_hierarquica_3n(unsigned int bits_l1, unsigned int bits_l2, unsigned int bits_l3, unsigned long* custo_memoria);
int lookup_tabela_hierarquica_3n(void* tabela, unsigned int num_pagina, unsigned int bits_l1, unsigned int bits_l2, unsigned int bits_l3);
void atualizar_tabela_hierarquica_3n(void* tabela, unsigned int num_pagina, int quadro, unsigned int bits_l1, unsigned int bits_l2, unsigned int bits_l3);
void liberar_tabela_hierarquica_3n(void* tabela, unsigned int bits_l1, unsigned int bits_l2);

void* alocar_tabela_invertida(unsigned long* custo_memoria);
int lookup_tabela_invertida(void* tabela, unsigned int num_pagina);
void atualizar_tabela_invertida(void* tabela, unsigned int num_pagina, int quadro);
void liberar_tabela_invertida(void* tabela);


void processar_acesso(
    unsigned int addr,
    char rw,
    char* algoritmo_usado,
    void* tabela_paginas,
    char* tipo_tabela,
    unsigned int* num_pags_fault,
    unsigned int bits_l1,
    unsigned int bits_l2,
    unsigned int bits_l3
);

void print_start(char* tipo_tabela);

void print_info(unsigned int addr, char rw, unsigned int num_pagina, int quadro, int foi_page_fault, int quadro_substituido, int escreveu_disco);

void print_report(char* tipo_tabela, char* arquivo_entrada, unsigned int tam_memo, unsigned int tam_pags, char* algoritmo_usado, unsigned int num_acessos_memo, unsigned int num_pags_fault, unsigned int num_pags_sujas, double tempo_gasto, unsigned long custo_memoria_tabela);

#endif