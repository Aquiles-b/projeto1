#include <stdio.h>
#include <getopt.h>
#include <string.h>
#include <stdlib.h>

/* Struct com os principais dados de um registro.
 * Usa @char para conteudo porque ele eh 1 byte.*/
struct registro {
    char *nome;
    char *conteudo;
    int tamConteudo;
};

/* Retorna um ponteiro para uma struct registro.*/
struct registro *criaRegistro()
{
    struct registro *reg = malloc(sizeof(struct registro));
    reg->conteudo = NULL;
    reg->nome = NULL;
    reg->tamConteudo = 0;

    return reg;
}

/* Retorna o tamanho de um registro. */
int tamDoRegistro(FILE *arqBckp)
{
    int tamReg;
    fread(&tamReg, sizeof(int), 1, arqBckp);

    return tamReg;
}

/* Retorna o tamanho do nome de um registro.*/
int tamNomeRegistro(char *dados)
{
    int i = 0;
    while (dados[i] != '\n')
        i++;
    i++;

    return i;
}

/* Coloca o nome do registro na struct.*/
void separaNomeReg(struct registro *reg, char *dados, int n)
{
    reg->nome = malloc(sizeof(char) * n);
    strncpy(reg->nome, dados, n);
    reg->nome[strcspn(reg->nome, "\n")] = '\0';
}

/* Copia o conteudo dos dados do registro */
void separaConteudo(struct registro *reg, char *dados, int comeco, int tamReg)
{
    reg->tamConteudo = tamReg - comeco;
    reg->conteudo = malloc(sizeof(char) * reg->tamConteudo);
    for (int j = 0; j < reg->tamConteudo; j++) {
        reg->conteudo[j] = dados[comeco];
        comeco++;
    }
}

/* Retorna um ponteiro para um registro vindo de @arqBckp.*/
struct registro *leRegistro(FILE *arqBckp)
{
    unsigned int tamReg, indFimNome;
    char *dadosRegistro;
    struct registro *reg = criaRegistro();

    tamReg = tamDoRegistro(arqBckp);
    dadosRegistro = malloc(sizeof(char) * tamReg);
    fread(dadosRegistro, sizeof(char), tamReg, arqBckp);

    indFimNome = tamNomeRegistro(dadosRegistro);
    separaNomeReg(reg, dadosRegistro, indFimNome);
    separaConteudo(reg, dadosRegistro, indFimNome, tamReg);

    free(dadosRegistro);

    return reg;
}

/* Escreve a struct registro em disco.*/
void escreveRegDisco(struct registro *reg)
{
    FILE *arqFinal = fopen(reg->nome, "w");
    if (!arqFinal){
        perror("Nao foi possivel criar arquivo.");
        return;
    }

    fwrite(reg->conteudo, sizeof(char), reg->tamConteudo, arqFinal);
    fclose(arqFinal);
}

/* Desaloca a memoria da struct registro e retorna NULL.*/
struct registro *liberaReg(struct registro *reg)
{
    free(reg->conteudo);
    free(reg->nome);
    free(reg);

    return NULL;
}

/* Retorna 1 se for para extrair os arquivos e 0 caso contrario.*/
int ehPraExtrair(int argc, char **argv)
{
    int option;
    option = getopt (argc, argv, "t");

    return option == -1;
}

FILE *abreAquivoBckp()
{
    FILE *arqBckp = fopen("bkp.bin", "r");
    if (!arqBckp)
        perror("Nao foi possivel abir o arquivo.");

    return arqBckp;
}

/* Extrai todos os arquivos de @arqBckp.*/
void extraiArquivos(int qtdRegistros, FILE *arqBckp)
{
    struct registro *registro;
    for (int i = 0; i < qtdRegistros; i++) {
        registro = leRegistro(arqBckp);
        escreveRegDisco(registro);
        registro = liberaReg(registro);
    }
}

/* Imprime o nome dos arquivos e se respectivo tamanho.*/
void imprimeArquivos(int qtdRegistros, FILE *arqBckp)
{
    struct registro *registro;
    for (int i = 0; i < qtdRegistros; i++) {
        registro = leRegistro(arqBckp);
        printf ("%s - tamanho: %d\n", registro->nome, registro->tamConteudo);
        registro = liberaReg(registro);
    }
}

int main(int argc, char **argv)
{
    if (argc > 2){
        fprintf(stderr, "Parametros incorretos.\n");
        return 1;
    }
    unsigned int qtdRegistros;
    FILE *arqBckp = abreAquivoBckp();
    if (!arqBckp)
        return 1;

    fread(&qtdRegistros, sizeof(int), 1, arqBckp);

    if (ehPraExtrair(argc, argv))
        extraiArquivos(qtdRegistros, arqBckp);
    else
        imprimeArquivos(qtdRegistros, arqBckp);

    fclose(arqBckp);

    return 0;
}
