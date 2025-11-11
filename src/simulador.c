#include "../include/simulador.h"

 /**
 * criar a tabela das paginas (pode ser multinivel)
 * ler um arquivo deve pegar o endereco e a opcao de escrita ou leitura
 *
 * entrada:
 *  arquivo
 *  tamanho da pagina: kb
 *  tamanho da mem física: kb
 */ 

Quadro* memoria_fisica = NULL;
unsigned int num_quadros = 0;
unsigned int s = 0;
unsigned long long contador_tempo = 0;
unsigned int modo_depuracao_global = 0;
unsigned int num_pags_sujas_global = 0;

void print_start(char* tipo_tabela)
{
    printf("---- DEBUG DO TIPO DE TABELA DE PÁGINAS: %s ----\n", tipo_tabela);
}

void print_info(unsigned int addr, char rw, unsigned int num_pagina, int quadro, int foi_page_fault, int quadro_substituido, int escreveu_disco)
 /*
Impressão do passo a passo caso argv[5] == 1
*/ 
{
    if (modo_depuracao_global == 0) return;

    printf("Acesso: %08x (%c) - Pag: %u - ", addr, rw, num_pagina);
    if (foi_page_fault)
    {
        printf("Page Fault - ");
        if (quadro_substituido != -1)
        {
            printf("Quadro %d substituído (Pag: %u) - ", quadro_substituido, memoria_fisica[quadro_substituido].pagina_virtual_mapeada);
            if (escreveu_disco)
            {
                printf("Página suja escrita - ");
            }
        }
        printf("Alocado no Quadro %d\n", quadro);
    }
    else
    {
        printf("Hit - Quadro %d\n", quadro);
    }
}

void print_report(char* tipo_tabela, char* arquivo_entrada, unsigned int tam_memo, unsigned int tam_pags, char* algoritmo_usado, unsigned int num_acessos_memo, unsigned int num_pags_fault, unsigned int num_pags_sujas, double tempo_gasto, unsigned long custo_memoria_tabela)
 /*
Impressão do relatório final
Obs: lembrando-se que páginas sujas que existirem no final da execução não precisam ser escritas)
*/ 
{
    printf("\n------------- Relatório Final (%s) -------------\n", tipo_tabela);
    printf("Arquivo de entrada: %s\n", arquivo_entrada);
    printf("Tamanho da memória: %u KB\n", tam_memo);
    printf("Tamanho das páginas: %u KB\n", tam_pags);
    printf("Algoritmo usado: %s\n", algoritmo_usado);
    printf("Tipo de tabela de páginas: %s\n", tipo_tabela);
    printf("\nResultados:\n");
    printf("Número de acessos à memória: %u\n", num_acessos_memo);
    printf("Número de page faults: %u\n", num_pags_fault);
    printf("Número de páginas sujas escritas: %u\n", num_pags_sujas);
    printf("\nDesempenho:\n");
    printf("Custo de memória da tabela: %lu bytes (%.2f MB)\n", custo_memoria_tabela, (double)custo_memoria_tabela / (1024.0 * 1024.0));
    printf("Tempo de simulação (CPU): %.4f segundos\n", tempo_gasto);
    printf("--------------------------------------------------\n\n");
}

unsigned int calcular_s(unsigned int tam_pags_kb)
/*
Código sugerido na doc
*/
{
    unsigned int tmp = tam_pags_kb * 1024;
    unsigned int s_calculado = 0;
    while (tmp > 1) {
        tmp = tmp >> 1;
        s_calculado++;
    }
    return s_calculado;
}

void inicializar_memoria_fisica(void)
{
    memoria_fisica = (Quadro*) malloc(sizeof(Quadro) * num_quadros);
    if (memoria_fisica == NULL) {
        perror("Falha ao alocar memória física");
        exit(1);
    }
    for (unsigned int i = 0; i < num_quadros; i++) {
        memoria_fisica[i].valido = 0;
        memoria_fisica[i].suja = 0;
        memoria_fisica[i].ultimo_acesso = 0;
        memoria_fisica[i].frequencia = 0;
        memoria_fisica[i].pagina_virtual_mapeada = 0;
    }
}

int encontrar_quadro_livre(void)
{
    for (unsigned int i = 0; i < num_quadros; i++) {
        if (memoria_fisica[i].valido == 0) {
            return i;
        }
    }
    return -1;
}

int substitui_random(void)
{
    int quadro_substituido = random() % num_quadros;
    return quadro_substituido;
}

int substitui_lru(void)
{
    unsigned long long menor_tempo = ULLONG_MAX;
    int quadro_substituido = 0;
    for (unsigned int i = 0; i < num_quadros; i++) {
        if (memoria_fisica[i].ultimo_acesso < menor_tempo) {
            menor_tempo = memoria_fisica[i].ultimo_acesso;
            quadro_substituido = i;
        }
    }
    return quadro_substituido;
}

int substitui_lfu(void)
{
    unsigned int menor_freq = UINT_MAX;
    int quadro_substituido = 0;
    for (unsigned int i = 0; i < num_quadros; i++) {
        if (memoria_fisica[i].frequencia < menor_freq) {
            menor_freq = memoria_fisica[i].frequencia;
            quadro_substituido = i;
        }
    }
    return quadro_substituido;
}

int substitui_mru(void)
{
    unsigned long long maior_tempo = 0; // Começa com 0 (ou ULLONG_MIN)
    int quadro_substituido = 0;
    for (unsigned int i = 0; i < num_quadros; i++) {
        // Encontra o quadro com o MAIOR tempo de acesso (o mais recente)
        if (memoria_fisica[i].ultimo_acesso > maior_tempo) {
            maior_tempo = memoria_fisica[i].ultimo_acesso;
            quadro_substituido = i;
        }
    }
    return quadro_substituido;
}

void* alocar_tabela_densa(unsigned long num_paginas, unsigned long* custo_memoria)
{
    *custo_memoria = num_paginas * sizeof(int);
    int* tabela = (int*) malloc(*custo_memoria);
    if (tabela == NULL) {
        perror("Falha ao alocar tabela densa");
        return NULL;
    }
    memset(tabela, -1, *custo_memoria);
    return (void*)tabela;
}

int lookup_tabela_densa(void* tabela, unsigned int num_pagina)
{
    int* t = (int*)tabela;
    return t[num_pagina];
}

void atualizar_tabela_densa(void* tabela, unsigned int num_pagina, int quadro)
{
    int* t = (int*)tabela;
    t[num_pagina] = quadro;
}

void liberar_tabela_densa(void* tabela)
{
    free(tabela);
}

void* alocar_tabela_hierarquica_2n(unsigned int bits_l1, unsigned int bits_l2, unsigned long* custo_memoria)
{
    unsigned long num_entradas_l1 = 1L << bits_l1;
    //unsigned long num_entradas_l2 = 1L << bits_l2; (é usado na função de atualizar a tabela 2n)
    *custo_memoria = num_entradas_l1 * sizeof(int*);
    
    int** tabela_l1 = (int**) malloc(*custo_memoria);
    if (tabela_l1 == NULL) return NULL;
    
    for (unsigned long i = 0; i < num_entradas_l1; i++) {
        tabela_l1[i] = NULL;
    }
    return (void*)tabela_l1;
}

int lookup_tabela_hierarquica_2n(void* tabela, unsigned int num_pagina, unsigned int bits_l1, unsigned int bits_l2)
{
    int** tabela_l1 = (int**)tabela;
    unsigned int mask_l1 = ((1L << bits_l1) - 1);
    unsigned int mask_l2 = ((1L << bits_l2) - 1);

    unsigned int idx_l1 = (num_pagina >> bits_l2) & mask_l1;
    unsigned int idx_l2 = num_pagina & mask_l2;

    if (tabela_l1[idx_l1] == NULL) {
        return -1;
    }
    
    return tabela_l1[idx_l1][idx_l2];
}

void atualizar_tabela_hierarquica_2n(void* tabela, unsigned int num_pagina, int quadro, unsigned int bits_l1, unsigned int bits_l2)
{
    int** tabela_l1 = (int**)tabela;
    unsigned long num_entradas_l2 = 1L << bits_l2;
    unsigned int mask_l1 = ((1L << bits_l1) - 1);
    unsigned int mask_l2 = ((1L << bits_l2) - 1);

    unsigned int idx_l1 = (num_pagina >> bits_l2) & mask_l1;
    unsigned int idx_l2 = num_pagina & mask_l2;

    if (tabela_l1[idx_l1] == NULL) {
        tabela_l1[idx_l1] = (int*) malloc(num_entradas_l2 * sizeof(int));
        memset(tabela_l1[idx_l1], -1, num_entradas_l2 * sizeof(int));
    }
    
    tabela_l1[idx_l1][idx_l2] = quadro;
}

void liberar_tabela_hierarquica_2n(void* tabela, unsigned int bits_l1)
{
    int** tabela_l1 = (int**)tabela;
    unsigned long num_entradas_l1 = 1L << bits_l1;

    for (unsigned long i = 0; i < num_entradas_l1; i++) {
        if (tabela_l1[i] != NULL) {
            free(tabela_l1[i]);
        }
    }
    free(tabela_l1);
}

void* alocar_tabela_hierarquica_3n(unsigned int bits_l1, unsigned int bits_l2, unsigned int bits_l3, unsigned long* custo_memoria)
{
    unsigned long num_entradas_l1 = 1L << bits_l1;
    *custo_memoria = num_entradas_l1 * sizeof(int**);

    int*** tabela_l1 = (int***) malloc(*custo_memoria);
    if (tabela_l1 == NULL) return NULL;
    
    for (unsigned long i = 0; i < num_entradas_l1; i++) {
        tabela_l1[i] = NULL;
    }
    return (void*)tabela_l1;
}

int lookup_tabela_hierarquica_3n(void* tabela, unsigned int num_pagina, unsigned int bits_l1, unsigned int bits_l2, unsigned int bits_l3)
{
    int*** tabela_l1 = (int***)tabela;
    unsigned int mask_l1 = ((1L << bits_l1) - 1);
    unsigned int mask_l2 = ((1L << bits_l2) - 1);
    unsigned int mask_l3 = ((1L << bits_l3) - 1);

    unsigned int idx_l1 = (num_pagina >> (bits_l2 + bits_l3)) & mask_l1;
    unsigned int idx_l2 = (num_pagina >> bits_l3) & mask_l2;
    unsigned int idx_l3 = num_pagina & mask_l3;

    if (tabela_l1[idx_l1] == NULL || tabela_l1[idx_l1][idx_l2] == NULL) {
        return -1;
    }

    return tabela_l1[idx_l1][idx_l2][idx_l3];
}

void atualizar_tabela_hierarquica_3n(void* tabela, unsigned int num_pagina, int quadro, unsigned int bits_l1, unsigned int bits_l2, unsigned int bits_l3)
{
    int*** tabela_l1 = (int***)tabela;
    unsigned long num_entradas_l2 = 1L << bits_l2;
    unsigned long num_entradas_l3 = 1L << bits_l3;

    unsigned int mask_l1 = ((1L << bits_l1) - 1);
    unsigned int mask_l2 = ((1L << bits_l2) - 1);
    unsigned int mask_l3 = ((1L << bits_l3) - 1);

    unsigned int idx_l1 = (num_pagina >> (bits_l2 + bits_l3)) & mask_l1;
    unsigned int idx_l2 = (num_pagina >> bits_l3) & mask_l2;
    unsigned int idx_l3 = num_pagina & mask_l3;

    if (tabela_l1[idx_l1] == NULL) {
        tabela_l1[idx_l1] = (int**) malloc(num_entradas_l2 * sizeof(int*));
        for (unsigned long i = 0; i < num_entradas_l2; i++) {
            tabela_l1[idx_l1][i] = NULL;
        }
    }
    
    if (tabela_l1[idx_l1][idx_l2] == NULL) {
        tabela_l1[idx_l1][idx_l2] = (int*) malloc(num_entradas_l3 * sizeof(int));
        memset(tabela_l1[idx_l1][idx_l2], -1, num_entradas_l3 * sizeof(int));
    }
    
    tabela_l1[idx_l1][idx_l2][idx_l3] = quadro;
}

void liberar_tabela_hierarquica_3n(void* tabela, unsigned int bits_l1, unsigned int bits_l2)
{
    int*** tabela_l1 = (int***)tabela;
    unsigned long num_entradas_l1 = 1L << bits_l1;
    unsigned long num_entradas_l2 = 1L << bits_l2;

    for (unsigned long i = 0; i < num_entradas_l1; i++) {
        if (tabela_l1[i] != NULL) {
            for (unsigned long j = 0; j < num_entradas_l2; j++) {
                if (tabela_l1[i][j] != NULL) {
                    free(tabela_l1[i][j]);
                }
            }
            free(tabela_l1[i]);
        }
    }
    free(tabela_l1);
}

void* alocar_tabela_invertida(unsigned long* custo_memoria)
{
    *custo_memoria = num_quadros * sizeof(unsigned int);
    unsigned int* tabela = (unsigned int*) malloc(*custo_memoria);
    if (tabela == NULL) return NULL;
    
    memset(tabela, 0, *custo_memoria);
    return (void*)tabela;
}

int lookup_tabela_invertida(void* tabela, unsigned int num_pagina)
{
    unsigned int* t = (unsigned int*)tabela;
    for (unsigned int i = 0; i < num_quadros; i++) {
        if (memoria_fisica[i].valido && t[i] == num_pagina) {
            return i;
        }
    }
    return -1;
}

void atualizar_tabela_invertida(void* tabela, unsigned int num_pagina, int quadro)
{
    unsigned int* t = (unsigned int*)tabela;
    t[quadro] = num_pagina;
}

void liberar_tabela_invertida(void* tabela)
{
    free(tabela);
}

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
)
/*
Seleção entre todos os tipos de algoritmos e tabelas
Opções de algoritmos: lru, lfu, random (default) e mru
*/
{
    contador_tempo++;
    unsigned int num_pagina = addr >> s;
    int quadro_fisico = -1;
    int foi_page_fault = 0;
    int quadro_substituido = -1;
    int escreveu_disco = 0;

    if (strcmp(tipo_tabela, "densa") == 0) {
        quadro_fisico = lookup_tabela_densa(tabela_paginas, num_pagina);
    } else if (strcmp(tipo_tabela, "hierarquica_2n") == 0) {
        quadro_fisico = lookup_tabela_hierarquica_2n(tabela_paginas, num_pagina, bits_l1, bits_l2);
    } else if (strcmp(tipo_tabela, "hierarquica_3n") == 0) {
        quadro_fisico = lookup_tabela_hierarquica_3n(tabela_paginas, num_pagina, bits_l1, bits_l2, bits_l3);
    } else if (strcmp(tipo_tabela, "invertida") == 0) {
        quadro_fisico = lookup_tabela_invertida(tabela_paginas, num_pagina);
    }

    if (quadro_fisico == -1 || memoria_fisica[quadro_fisico].valido == 0)
    {
        foi_page_fault = 1;
        (*num_pags_fault)++;
        
        int quadro_alvo = encontrar_quadro_livre();

        if (quadro_alvo == -1)
        {
            if (strcmp(algoritmo_usado, "random") == 0) quadro_alvo = substitui_random();
            else if (strcmp(algoritmo_usado, "lru") == 0) quadro_alvo = substitui_lru();
            else if (strcmp(algoritmo_usado, "lfu") == 0) quadro_alvo = substitui_lfu();
            else if(strcmp(algoritmo_usado, "mru")==0) quadro_alvo = substitui_mru();
            else 
            {
                printf("Nenhum algotimo válido selecionado; algoritmo randômico usado como default");
                quadro_alvo = substitui_random();
            }
            
            quadro_substituido = quadro_alvo;
            unsigned int pag_antiga = memoria_fisica[quadro_alvo].pagina_virtual_mapeada;

            if (memoria_fisica[quadro_alvo].suja) {
                escreveu_disco = 1;
                num_pags_sujas_global++;
            }

            if (strcmp(tipo_tabela, "densa") == 0) {
                atualizar_tabela_densa(tabela_paginas, pag_antiga, -1);
            } else if (strcmp(tipo_tabela, "hierarquica_2n") == 0) {
                atualizar_tabela_hierarquica_2n(tabela_paginas, pag_antiga, -1, bits_l1, bits_l2);
            } else if (strcmp(tipo_tabela, "hierarquica_3n") == 0) {
                atualizar_tabela_hierarquica_3n(tabela_paginas, pag_antiga, -1, bits_l1, bits_l2, bits_l3);
            }
        }
        
        memoria_fisica[quadro_alvo].valido = 1;
        memoria_fisica[quadro_alvo].pagina_virtual_mapeada = num_pagina;
        memoria_fisica[quadro_alvo].suja = 0;
        memoria_fisica[quadro_alvo].frequencia = 1;
        memoria_fisica[quadro_alvo].ultimo_acesso = contador_tempo;
        
        if (strcmp(tipo_tabela, "densa") == 0) {
            atualizar_tabela_densa(tabela_paginas, num_pagina, quadro_alvo);
        } else if (strcmp(tipo_tabela, "hierarquica_2n") == 0) {
            atualizar_tabela_hierarquica_2n(tabela_paginas, num_pagina, quadro_alvo, bits_l1, bits_l2);
        } else if (strcmp(tipo_tabela, "hierarquica_3n") == 0) {
            atualizar_tabela_hierarquica_3n(tabela_paginas, num_pagina, quadro_alvo, bits_l1, bits_l2, bits_l3);
        } else if (strcmp(tipo_tabela, "invertida") == 0) {
            atualizar_tabela_invertida(tabela_paginas, num_pagina, quadro_alvo);
        }
        
        quadro_fisico = quadro_alvo;
    }

    memoria_fisica[quadro_fisico].ultimo_acesso = contador_tempo;
    memoria_fisica[quadro_fisico].frequencia++;
    if (rw == 'W') {
        memoria_fisica[quadro_fisico].suja = 1;
    }

    print_info(addr, rw, num_pagina, quadro_fisico, foi_page_fault, quadro_substituido, escreveu_disco);
}


int main(int argc, char **argv)
{
    printf("Executando o simulador...\n\n");

    if (argc < 6)
    {
        fprintf(stderr, "Argumentos faltando. Exemplo de entrada: \nsimulador lru arquivo.log 4 128 0\n");
        return 1;
    }

    char *algoritmo_usado = argv[1];
    /*TODO: verificação de algoritmo usado
    Opções: random, lru, lfu, mru
    */ 
    char *arquivo_entrada = argv[2];
    unsigned int tam_pags = atoi(argv[3]);
    unsigned int tam_memo = atoi(argv[4]);
    unsigned int modo_depuracao = atoi(argv[5]);

    modo_depuracao_global = modo_depuracao;
    srandom(time(NULL));

    s = calcular_s(tam_pags);
    if (tam_pags == 0 || tam_memo == 0) {
        fprintf(stderr, "Tamanho da página ou da memória não podem ser nulos\n");
        return 1;
    }
    num_quadros = tam_memo / tam_pags;
    if (num_quadros == 0) {
        fprintf(stderr, "Memória insuficiente para o tamanho de página especificado\n");
        return 1;
    }

    FILE *fp;
    fp = fopen(arquivo_entrada, "r");
    if (fp == 0)
    {
        perror("Erro ao abrir o aquivo");
        return 1;
    }

    unsigned int bits_pagina = 32 - s;
    unsigned long num_total_paginas_virtuais = 1L << bits_pagina;
    unsigned int bits_l1_2n = 10;
    unsigned int bits_l2_2n = bits_pagina - bits_l1_2n;
    unsigned int bits_l1_3n = 10;
    unsigned int bits_l2_3n = 10;
    unsigned int bits_l3_3n = bits_pagina - bits_l1_3n - bits_l2_3n;

    char* tipos_tabela[] = {"densa", "hierarquica_2n", "hierarquica_3n", "invertida"};
    /*TODO: implementar rodagem dos dados para cada um dos quatro tipos de tabela de páginas
    Obs: caso bool_modo_depuracao == 1, imprimir relatório de desempenho (tempo de execução e uso de memo)
    */ 
    for (int i = 0; i < 4; i++)
    {
        char* tipo_tabela = tipos_tabela[i];
        void* tabela_paginas = NULL;
        unsigned long custo_memoria_tabela = 0;

        if (modo_depuracao_global) print_start(tipo_tabela);

        if (strcmp(tipo_tabela, "densa") == 0) {
            tabela_paginas = alocar_tabela_densa(num_total_paginas_virtuais, &custo_memoria_tabela);
        } else if (strcmp(tipo_tabela, "hierarquica_2n") == 0) {
            tabela_paginas = alocar_tabela_hierarquica_2n(bits_l1_2n, bits_l2_2n, &custo_memoria_tabela);
        } else if (strcmp(tipo_tabela, "hierarquica_3n") == 0) {
            tabela_paginas = alocar_tabela_hierarquica_3n(bits_l1_3n, bits_l2_3n, bits_l3_3n, &custo_memoria_tabela);
        } else if (strcmp(tipo_tabela, "invertida") == 0) {
            tabela_paginas = alocar_tabela_invertida(&custo_memoria_tabela);
        }

        if (tabela_paginas == NULL && !(strcmp(tipo_tabela, "invertida") == 0 && num_quadros > 0)) {
            fprintf(stderr, "Falha ao alocar tabela de páginas: %s\n", tipo_tabela);
            continue;
        }

        inicializar_memoria_fisica();
        
        unsigned int num_acessos_memo = 0, num_pags_fault = 0;
        num_pags_sujas_global = 0;
        contador_tempo = 0;
        
        fseek(fp, 0, SEEK_SET);
        
        unsigned int addr;
        char rw;
        clock_t inicio_sim = clock();
        while (fscanf(fp, "%x %c", &addr, &rw) == 2)
        {
            num_acessos_memo++;
            processar_acesso(addr, rw, algoritmo_usado, tabela_paginas, tipo_tabela, &num_pags_fault, bits_l1_2n, bits_l2_2n, bits_l3_3n);
        }
        
        clock_t fim_sim = clock();
        double tempo_gasto = (double)(fim_sim - inicio_sim) / CLOCKS_PER_SEC;

        print_report(tipo_tabela, arquivo_entrada, tam_memo, tam_pags, algoritmo_usado, num_acessos_memo, num_pags_fault, num_pags_sujas_global, tempo_gasto, custo_memoria_tabela);

        /*TODO: liberar memo de cada uma das tabelas*/
        if (strcmp(tipo_tabela, "densa") == 0) {
            liberar_tabela_densa(tabela_paginas);
        } else if (strcmp(tipo_tabela, "hierarquica_2n") == 0) {
            liberar_tabela_hierarquica_2n(tabela_paginas, bits_l1_2n);
        } else if (strcmp(tipo_tabela, "hierarquica_3n") == 0) {
            liberar_tabela_hierarquica_3n(tabela_paginas, bits_l1_3n, bits_l2_3n);
        } else if (strcmp(tipo_tabela, "invertida") == 0) {
            liberar_tabela_invertida(tabela_paginas);
        }
        
        free(memoria_fisica);
        memoria_fisica = NULL;
    }

    fclose(fp);
    return 0;
}