#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <stdbool.h>

/**
 * ANOTAÇÕES
 *
 * criar a tabela das paginas - o tamanho da p´agina ser´a sempre fornecido como
 * uma potˆencia de 2:
 *  densa
 *  hierarquica
 *      com 2 niveis
 *      com 3 niveis
 *  invertida
 *
 * ler um arquivo deve pegar o endereco e a opcao de escrita ou leitura
 *
 * entrada:
 *  arquivo
 *  tamanho da pagina: kb
 *  tamanho da mem física: kb
 */

// Calculate s dynamically based on the input page size
int dyn_s(int page_size)
{
    int s = 0;
    int temp = page_size;
    while (temp > 1)
    {
        temp = temp >> 1;
        s++;
    }

    return s;
}

void print_info()
{
}

int main(int argc, char **argv)
{
    int pg_s, mem_sz, s; // page size, memory size, smallest s bits of address
    char *sub_a;         // substitution algorithm
    bool debug = 0;

    unsigned int addr; // address
    char rw;           // read or write

    if (argc < 4)
    {
        perror("Argumentos faltando: ");
    }
    if (argv[5] == "debug")
    {
        debug = 1;
    }

    sub_a = argv[1];
    pg_s = atoi(argv[3]);
    mem_sz = atoi(argv[4]);
    s = dyn_s(pg_s);

    FILE *fp;
    fp = fopen(argv[2], "r");

    if (fp == 0)
    {
        perror("Erro ao abrir o aquivo ");
    }

    while (fscanf(fp, "%x %c", addr, rw) == 2)
    {
    }

    return 0;
}