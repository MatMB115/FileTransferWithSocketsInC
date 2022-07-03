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
  FILE *arquivo;
  info dados[2];
  info recupera;

  // Abastecimento de dados em uma estrutura
  dados[0].codigo = 1;
  strcpy(dados[0].nome, "jose");
  dados[0].salario = 200.56;

  dados[1].codigo = 2;
  strcpy(dados[1].nome, "joaquim");
  dados[1].salario = 176.22;


  // GRAVA��O DE DADOS EM ARQUIVO BIN�RIO
  if((arquivo=fopen("dados.dat","wb"))==NULL)
  {
   printf("N�o � poss�vel criar arquivo...");
   fclose(arquivo);
   exit(1);
  }

  fwrite(&dados[0], sizeof(info), 1, arquivo);
  fwrite(&dados[1], sizeof(info), 1, arquivo);
  printf("Dados gravados com sucesso...");
  fclose(arquivo);


  // RECUPERA��O DE DADOS EM ARQUIVO BIN�RIO
  printf("\n\nRecuperando os dados do arquivo...\n\n");
  if((arquivo=fopen("dados.dat","rb"))==NULL)
  {
   printf("N�o � poss�vel abrir arquivo...");
   fclose(arquivo);
   exit(1);
  }

  //Enquanto n�o for o final do arquivo, imprime.
  while (!feof(arquivo))
  {
    //H� um erro aqui. Implemente a corre��o
     fread(&recupera, sizeof(info), 1, arquivo);
     printf("Codigo recuperado...= %d \n", recupera.codigo);
     printf("Nome recuperado.....= %s \n", recupera.nome);
     printf("Salario recuperado..= %5.2f \n", recupera.salario);
     printf("\n------\n");
  }

  fclose(arquivo);

}

