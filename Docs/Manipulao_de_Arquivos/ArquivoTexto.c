#include <stdio.h>
#include <stdlib.h>

void leitura(){

    FILE* arq;
    char texto;
    arq = fopen("teste.txt","r");
    if(arq == NULL){
		printf("Nao e possivel abrir o arquivo\n");
		exit(1);
	}
	printf("Imprimindo conteúdo do arquivo \n");
    //lê conteúdo do arquivo até que encontrar EOF (End of File)
	while((fscanf(arq,"%c", &texto)) != EOF)
        printf("%c", texto);

    fclose(arq);
}

main()
{
  char c;
  FILE *arquivo;

  // Tenta abrir o arquivo no modo a. Indica modo para escrita ao final do arquivo
  if((arquivo=fopen("teste.txt","a")) == NULL)
  {
   printf("Não é possível criar arquivo...");
   fclose(arquivo);
   exit(1);
  }

  printf("Digite o texto que será inserido. Para encerrar digite .\n");
  c = ' ';
  while (c != '.')
  {
   scanf("%c", &c);
    fputc(c,arquivo);
  }

  fclose(arquivo);
  leitura();

}

