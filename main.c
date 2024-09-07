#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct
{
    int especie;
    char nome[41];
    char nome_cientifico[61];
    int populacao;
    char status[9];
    float localizacao[2];
    int impacto_humano;
} Registro;

int tamanho_bytes(FILE *arquivo)
{
    fseek(arquivo, 0, SEEK_END);
    return ftell(arquivo);
}

void readline(char *string)
{
    char c = 0;

    do
    {
        c = (char)getchar();
    } while (c == '\n' || c == '\r');

    int i = 0;

    do
    {
        string[i] = c;
        i++;
        c = getchar();
    } while (c != '\n' && c != '\r');

    string[i] = '\0';
}

int busca_por_id(int id, FILE *arquivo)
{
    int id_cmp = 0, tamanho = tamanho_bytes(arquivo);

    for (int i = 0; i < tamanho; i += 131)
    {
        fseek(arquivo, i, SEEK_SET);
        fread(&id_cmp, sizeof(int), 1, arquivo);

        if (id_cmp == id)
        {
            return i;
        }
    }

    return -1;
}

void le_registro(Registro *registro, FILE *arquivo, int offset)
{
    // arruma o ponteiro para o inicio do registro e le os valores
    fseek(arquivo, offset, SEEK_SET);
    fread(&registro->especie, 4, 1, arquivo);
    fread(registro->nome, 1, 41, arquivo);
    fread(registro->nome_cientifico, 1, 61, arquivo);
    fread(&registro->populacao, 4, 1, arquivo);
    fread(registro->status, 1, 9, arquivo);
    fread(registro->localizacao, 4, 2, arquivo);
    fread(&registro->impacto_humano, 4, 1, arquivo);
}

void printa_formatado(Registro *registro)
{
    printf("ID: %d\n", registro->especie);
    printf("Nome: %s\n", registro->nome);
    printf("Nome Científico: %s\n", registro->nome_cientifico);
    printf("População: %d\n", registro->populacao);
    printf("Status: %s\n", registro->status);
    printf("Localização: (%.2f, %.2f)\n", registro->localizacao[0], registro->localizacao[1]);
    printf("Impacto Humano: %d\n", registro->impacto_humano);
}

void relatorio(FILE *arquivo)

{
    // Primeiro abrimos o arquivo e lemos primeiro registro
    Registro *registro_temporario = (Registro *)malloc(sizeof(Registro));

    // primeiro abrimos o arquivo e contamos o numero de registros
    int tamanho = tamanho_bytes(arquivo);

    // por fim, pegamos as informacoes e printamos
    for (int offset = 0; offset < tamanho; offset += 131)
    {
        le_registro(registro_temporario, arquivo, offset);
        printa_formatado(registro_temporario);
    }

    // por fim libera o registro temporario
    free(registro_temporario);
}

void busca(FILE *arquivo)

{
    // Primeiro criamos um registro e abrimos o arquivo
    Registro *registro = (Registro *)malloc(sizeof(Registro));

    // entao pedimos o RRN e calculamos o byteoffset
    unsigned offset, rrn;
    scanf("%u", &rrn);
    offset = rrn * sizeof(Registro);

    // precisamos checar se o rrn existe, caso nao retornar erro
    unsigned ultimo_offset = tamanho_bytes(arquivo) - sizeof(Registro);
    if (offset > ultimo_offset)
    {
        printf("Espécie não encontrada\n");
        return;
    }

    // por fim lemos o registro e printamos ele na tela
    le_registro(registro, arquivo, offset);
    printa_formatado(registro);

    // libera o registro
    free(registro);
}

void preenche_string(char *string, size_t tamanho)
{
    size_t len = strlen(string);

    if (len < tamanho - 1)
    {
        string[len] = '\0';
    }

    for (size_t i = len + 1; i < tamanho; i++)
    {
        string[i] = '$';
    }
}

void limpa_campo(char *campo, size_t tamanho)
{
    for (size_t i = 0; i < tamanho; i++)
    {
        if (campo[i] == '$')
        {
            campo[i] = '\0';
            break;
        }
    }
}

void registra_especie(FILE *arquivo)
{
    int qtd_registros;
    scanf("%d", &qtd_registros);

    for (int i = 0; i < qtd_registros; i++)
    {
        Registro *registro = calloc(1, sizeof(Registro));
        registro->impacto_humano = 0;
        registro->populacao = 0;
        int id_cmp = 0;
        int id_existente = 0; // Flag para verificar se o ID já existe

        // Leitura do ID da espécie
        scanf("%d", &registro->especie);

        // Verifica se o ID já existe no arquivo
        for (int j = 0; j < tamanho_bytes(arquivo); j += 131)
        {
            fseek(arquivo, j, SEEK_SET);
            fread(&id_cmp, sizeof(int), 1, arquivo);
            if (registro->especie == id_cmp)
            {
                id_existente = 1; // Marca que o ID já existe
                break;            // Sai do loop interno
            }
        }

        // Realiza as leituras dos dados mesmo se o ID já existir
        getchar(); // Limpa o '\n' deixado pelo scanf

        readline(registro->nome);
        preenche_string(registro->nome, sizeof(registro->nome));

        readline(registro->nome_cientifico);
        preenche_string(registro->nome_cientifico, sizeof(registro->nome_cientifico));

        scanf("%d", &registro->populacao);
        getchar(); // Limpa o '\n'

        readline(registro->status);
        preenche_string(registro->status, sizeof(registro->status));

        scanf("%f %f", &registro->localizacao[0], &registro->localizacao[1]);
        registro->localizacao[0] = ((int)(registro->localizacao[0] * 100 + 0.5)) / 100.0;
        registro->localizacao[1] = ((int)(registro->localizacao[1] * 100 + 0.5)) / 100.0;

        scanf("%d", &registro->impacto_humano);

        // Se o ID já existir, pula a gravação no arquivo
        if (id_existente)
        {
            free(registro);
            continue; // Volta para o início do loop principal
        }

        // Gravação do novo registro no arquivo (somente se o ID não existir)
        fseek(arquivo, 0, SEEK_END);
        fwrite(&registro->especie, sizeof(int), 1, arquivo);
        fwrite(registro->nome, sizeof(registro->nome), 1, arquivo);
        fwrite(registro->nome_cientifico, sizeof(registro->nome_cientifico), 1, arquivo);
        fwrite(&registro->populacao, sizeof(int), 1, arquivo);
        fwrite(registro->status, sizeof(registro->status), 1, arquivo);
        fwrite(&registro->localizacao, sizeof(float), 2, arquivo);
        fwrite(&registro->impacto_humano, sizeof(int), 1, arquivo);

        free(registro);
    }
}

void registra_informacao(FILE *arquivo)
{
    Registro *registro = calloc(1, sizeof(Registro));

    int id, qtd_informacoes;
    char dado_adicionado[20];

    scanf("%d", &id);

    int offset = busca_por_id(id, arquivo);

    if (offset == -1)
    {
        printf("Espécie não encontrada\n");
        free(registro);
        return;
    }

    le_registro(registro, arquivo, offset);

    limpa_campo(registro->status, sizeof(registro->status));

    scanf("%d", &qtd_informacoes);
    getchar();

    for (int i = 0; i < qtd_informacoes; i++)
    {

        readline(dado_adicionado);

        if (strcmp(dado_adicionado, "HUMAN IMPACT") == 0)
        {
            if (registro->impacto_humano != 0)
            {
                printf("Informação já inserida no arquivo\n");
                free(registro);
                return;
            }

            scanf("%d", &registro->impacto_humano);
            getchar();

            if (registro->impacto_humano == 0)
            {
                registro->impacto_humano = 0;
            }

            fseek(arquivo, offset + 127, SEEK_SET);
            fwrite(&registro->impacto_humano, sizeof(registro->impacto_humano), 1, arquivo);
        }

        if (strcmp(dado_adicionado, "POPULATION") == 0)
        {
            if (registro->populacao != 0)
            {
                printf("Informação já inserida no arquivo\n");
                free(registro);
                return;
            }

            scanf("%d", &registro->populacao);
            getchar();

            if (registro->populacao == 0)
            {
                registro->populacao = 0;
            }

            fseek(arquivo, offset + 106, SEEK_SET);
            fwrite(&registro->populacao, sizeof(registro->populacao), 1, arquivo);
        }

        if (strcmp(dado_adicionado, "STATUS") == 0)
        {

            limpa_campo(registro->status, sizeof(registro->status));

            if (strcmp(registro->status, "NULO") != 0)
            {
                printf("Informação já inserida no arquivo\n");
            }
            readline(registro->status);

            preenche_string(registro->status, sizeof(registro->status));

            fseek(arquivo, offset + 110, SEEK_SET);
            fwrite(registro->status, sizeof(registro->status), 1, arquivo);
        }
    }

    free(registro);
}

void binarioNaTela(char *nomeArquivoBinario)
{ /* Você não precisa entender o código dessa função. */
    unsigned long i, cs;
    unsigned char *mb;
    size_t fl;
    FILE *fs;
    if (nomeArquivoBinario == NULL || !(fs = fopen(nomeArquivoBinario, "rb")))
    {
        fprintf(stderr, "ERRO AO ESCREVER O BINARIO NA TELA (função binarioNaTela): não foi possível abrir o arquivo que me passou para leitura. Ele existe e você tá passando o nome certo? Você lembrou de fechar ele com fclose depois de usar?\n");
        return;
    }
    fseek(fs, 0, SEEK_END);
    fl = ftell(fs);
    fseek(fs, 0, SEEK_SET);
    mb = (unsigned char *)malloc(fl);
    fread(mb, 1, fl, fs);

    cs = 0;
    for (i = 0; i < fl; i++)
    {
        cs += (unsigned long)mb[i];
    }
    printf("%lf\n", (cs / (double)100));
    free(mb);
    fclose(fs);
}

int main()
{
    // primeiro recebemos as informacoes que precisamos
    int n;
    char nome[20];
    scanf("%d %s", &n, nome);

    // busca o arquivo
    FILE *arquivo = fopen(nome, "r+");
    if (arquivo == NULL)
    {
        printf("Falha no processamento do arquivo\n");
        return 1;
    }

    // executa funcionalidade desejada
    switch (n)
    {
    case 1:
        registra_especie(arquivo);
        break;
    case 2:
        relatorio(arquivo);
        break;
    case 3:
        busca(arquivo);
        break;
    case 4:
        registra_informacao(arquivo);
        break;
    }

    // fecha o arquivo
    fclose(arquivo);

    binarioNaTela(nome);

    return 0;
}
