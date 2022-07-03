#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct
{
  int codigo;
  char nome[30];
  float salario;
} info;

main()
{
  FILE *arquivo, *arquivoi;
  info dados[2];
  info recupera;
  int NumeroRegistroArquivoIndice;
  int CodigoArquivoIndice;

  // Abastecimento de dados em uma estrutura
  dados[0].codigo = 1050;
  strcpy(dados[0].nome, "jose");
  dados[0].salario = 200.56;

  dados[1].codigo = 2050;
  strcpy(dados[1].nome, "joaquim");
  dados[1].salario = 176.22;


  // GRAVAÇÃO DE DADOS EM ARQUIVO BINÁRIO
  if((arquivo=fopen("dados.dat","wb"))==NULL)
  {
   printf("\nNão é possível criar arquivo de dados...");
   fclose(arquivo);
   exit(1);
  }

  fwrite(&dados[0], sizeof(info), 1, arquivo);
  fwrite(&dados[1], sizeof(info), 1, arquivo);
  printf("\nDados gravados com sucesso no ARQUIVO DE DADOS...");
  fclose(arquivo);

  // GRAVAÇÃO DE DADOS DE ÍNDICE EM ARQUIVO
  if((arquivoi=fopen("indices.ind","wb"))==NULL)
  {
   printf("\nNão é possível criar arquivo de índices...");
   fclose(arquivoi);
   exit(1);
  }

  NumeroRegistroArquivoIndice = 0;
  fwrite(&dados[0].codigo, sizeof(int), 1, arquivoi);
  fwrite(&NumeroRegistroArquivoIndice, sizeof(int), 1, arquivoi);
  NumeroRegistroArquivoIndice++;
  fwrite(&dados[1].codigo, sizeof(int), 1, arquivoi);
  fwrite(&NumeroRegistroArquivoIndice, sizeof(int), 1, arquivoi);
  printf("\nÍndices gravados com sucesso no ARQUIVO DE INDICES...");
  fclose(arquivoi);


  // RECUPERAÇÃO DE DADOS EM ARQUIVO BINÁRIO USANDO ÍNDICES
  printf("\n\nRecuperando os dados dos arquivo de dados e de índices...\n\n");
  if((arquivo=fopen("dados.dat","rb"))==NULL)
  {
   printf("\nNão é possível abrir arquivo de dados...");
   fclose(arquivo);
   exit(1);
  }

  if((arquivoi=fopen("indices.ind","rb"))==NULL)
  {
   printf("Não é possível abrir arquivo de índices...");
   fclose(arquivoi);
   exit(1);
  }

  /*+------------------------------------------------------+
    | fseek(fp, offset, base)                              |
    | Define a nova posição corrente do arquivo fp (FILE). |
    | A nova posição é calculada adicionando o valor de    |
    | offset (int) (contado em bytes) à posição indicada   |
    | por base (int).                                      |
    | - base = 0 – início do arquivo                       |
    | - base = 1 – valor corrente                          |
    | - base = 2 – fim do arquivo                          |
    +------------------------------------------------------+*/

  fread(&CodigoArquivoIndice, sizeof(int), 1, arquivoi);
  fread(&NumeroRegistroArquivoIndice, sizeof(int), 1, arquivoi);

  fseek(arquivo, NumeroRegistroArquivoIndice*sizeof(info), 0);
  fread(&recupera, sizeof(info), 1, arquivo);

  if (CodigoArquivoIndice == recupera.codigo)
  {
   printf("\nCodigo recuperado...= %d \n", recupera.codigo);
   printf("\nNome recuperado.....= %s \n", recupera.nome);
   printf("\nSalario recuperado..= %5.2f \n", recupera.salario);
  }

  fclose(arquivo);
  fclose(arquivoi);

}

