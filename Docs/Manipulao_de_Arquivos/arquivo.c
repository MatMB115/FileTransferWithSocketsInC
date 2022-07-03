#include<stdio.h>
#include<stdlib.h>

void leitura(char nome_arquivo[]){

    FILE* arq;
    char texto;
    arq = fopen(nome_arquivo,"rt");
    if(arq == NULL){
		printf("Nao e possivel abrir o arquivo\n");
		exit(1);
	}
    //lê conteúdo do arquivo até que encontrar EOF (End of File)
	while((fscanf(arq,"%c", &texto)) != EOF)
        printf("%c", texto);

    fclose(arq);
}

main(){
	FILE* arq;
	char nome_arquivo[20];

	printf("Digite o nome do arquivo: ");
	scanf("%s", nome_arquivo);
	/*Tenta abrir o arquivo informado pelo usuário para escrita no modo texto*/
	arq = fopen(nome_arquivo,"wt");
	//Testa se o arquivo foi aberto com sucesso
	if(arq == NULL){
		printf("Nao e possivel abrir o arquivo\n");
		exit(1);
	}
	//Escreve no arquivo
	fprintf(arq,"Disciplina de Algoritmos e Estruturas de Dados I\n");
    //fecha arquivo
    fclose(arq);

	//Chama função para ler conteúdo do arquivo
	leitura(nome_arquivo);

}
