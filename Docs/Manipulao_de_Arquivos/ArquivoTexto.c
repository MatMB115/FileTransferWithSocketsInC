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
	printf("Imprimindo conte�do do arquivo \n");
    //l� conte�do do arquivo at� que encontrar EOF (End of File)
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
   printf("N�o � poss�vel criar arquivo...");
   fclose(arquivo);
   exit(1);
  }

  printf("Digite o texto que ser� inserido. Para encerrar digite .\n");
  c = ' ';
  while (c != '.')
  {
   scanf("%c", &c);
    fputc(c,arquivo);
  }

  fclose(arquivo);
  leitura();

}

