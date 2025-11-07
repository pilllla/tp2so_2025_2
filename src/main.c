#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

/**
 * criar a tabela das paginas (pode ser multinivel)
 * ler um arquivo deve pegar o endereco e a opcao de escrita ou leitura
 *
 * entrada:
 *  arquivo
 *  tamanho da pagina: kb
 *  tamanho da mem física: kb
 */

void print_info()
{
}

int main(int argc, char **argv)
{
    unsigned int pg_s, mem_sz;
    char *sub_a;

    unsigned int addr;
    char rw;

    if (argc < 4)
    {
        perror("Argumentos faltando: ");
    }

    sub_a = argv[1];
    pg_s = atoi(argv[3]);
    mem_sz = atoi(argv[4]);

    FILE *fp;
    fp = fopen(argv[2], "r");

    if (fp == 0)
    {
        perror("erro ao abrir o aquivo");
    }

    while (fscanf(fp, "%x %c", addr, rw) == 2)
    {
    }

    return 0;
}