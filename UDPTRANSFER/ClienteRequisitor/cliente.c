#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h> //Manipular posições de memória
#include <unistd.h>
#include <string.h> //Manipular strings
#include <sys/time.h>
#include <stdbool.h>
#include <time.h> //Temporizar o envio

#define SERVER_PORT 1500        //porta padrão do servidor
#define TAM_MAX_MSG 1024            //tamanho max do buffer
#define T_PADRAO 1          //tempo padrão do temporizador
#define IP_LOCAL "192.168.1.11" //IPv4 do cliente requisitor
#define LIMITE_DADOS 50         //índice que possui o início dos dados

//Varíaveis Globais
int cliente_socket, recebido, tam_server;
char mensagem[TAM_MAX_MSG+1]="";

//Configura a estrutura do socket com protocolos e parâmetros estabelecidos
void inicializar_sockaddr_in(struct sockaddr_in *estrutura, int port){
  estrutura->sin_family = AF_INET;
  estrutura->sin_addr.s_addr = htonl(INADDR_ANY);
  estrutura->sin_port = htons(port);
}


//Configurar a struct da biblioteca de sockets com o IP fornecido
void inicializar_host(struct sockaddr_in *estrutura, char* ip, int port){
  struct hostent *host;
  host = gethostbyname(ip);

  //caso o host não seja encontrado: finaliza o programa
  if(host==NULL) {
    printf("-> Host desconhecido '%s' \n", ip);
    exit(1);
  }

  estrutura->sin_family = host->h_addrtype;
  memcpy((char *) &estrutura->sin_addr.s_addr, 
	 host->h_addr_list[0], host->h_length);
  estrutura->sin_port = htons(port);
  
  printf("-> Configurando host '%s' (IP : %s) \n", host->h_name,
	 inet_ntoa(*(struct in_addr *)host->h_addr_list[0]));
}

//Retorna a quantidade de digitos de um inteiro para alocar espaço no número de sequência do cabeçalho
int digitos_numero(int num){
  int contador = 0;
  do{
    contador++;
    num/=10;
  }while(num!=0);
  return contador;
}

//Aguarda para reenviar uma mensagem
void temporizador_de_dados(float t_fornecido){
  float t1=0, t2=0;
  t1 = (float)clock()/(float)CLOCKS_PER_SEC;

  //Realiza loop até a diferença for menor que tempo fornecido (escopo limitado pelo float)
  while ( (t2-t1) < (t_fornecido) ) {
      t2 = (float)clock()/(float)CLOCKS_PER_SEC;
  }
   return;
}

//Retorna o tamanho de algum parâmetro no buffer da mensagem 
int retorne_quantidade(char* mensagem, int num){
    //contador
    int quantidade=0;

    //enquanto o tamanho da mensagem > i
    for(int i=0; i<strlen(mensagem);i++){
        //se encontrar algum espaço
        if(mensagem[i]==' '){
            num--;
            //se num==0, então sai do loop
            if(num==0){
                break;
            }
            //caso não seja o parametro certo, zera o contador
            quantidade=0;
        }//senão for espaço, então soma 1 no contador
        else if(mensagem[i]!=' ')
            quantidade++;
    }
    return quantidade;
}

//Confere o checksum recebido por complemento de 1
bool checksum_correto(char* checksum, char* mensagem){
    int k=0, j=0;

    //cria um checksum_confere
    char checksum_confere[11]="";

    //zera checksum_confere
    for(k=0;k<10; k++){
       checksum_confere[k]='0';
    }

    //gera um checksum condizente com os dados recebidos
    for(k=LIMITE_DADOS;k<TAM_MAX_MSG;k++){
        if(j>=10){
            j=0;
        }
        //soma alguns bits às posições do checksum
        checksum_confere[j]+=mensagem[k];
        j++;
    }

    //confere se o checksum encontrado é igual ao recebido
    if(strcmp(checksum,checksum_confere)==0){
        printf("--- Checksum correto\n");
        return true;
    }
    printf("--- Checksum ERROR\n");
    return false;
}

int main(int argc, char *argv[]) {

  int bind_cliente, i=2;
  struct sockaddr_in cliente, server, cliente_servidor;

  char ip_de_envio[15]="", porta_de_envio[5]="", nome_arquivo[30]="", extensao[5]="";
  int porta_de_envio_int;

  // Criando o socket do cliente
  cliente_socket = socket(AF_INET,SOCK_DGRAM,0);
  if(cliente_socket < 0) {
    printf("%s: Problema no socket \n",argv[0]);
    exit(1);
  }

  if(argc!=3) {
    // parâmetros errados
    // ./nomeprograma ip_server nome_arquivo
    printf("Parametros: %s <ip_server> <arquivo.extensao>\n", argv[0]);
    exit(1);
  }

  // Inicializando o host do servidor e do cliente.
  printf("%s: Configurando servidor chefe...\n", argv[0]);
  inicializar_host(&server, argv[1], SERVER_PORT);
  inicializar_sockaddr_in(&cliente, SERVER_PORT);

  // Criando o bind do cliente 
  bind_cliente = bind(cliente_socket, (struct sockaddr *) &cliente, sizeof(cliente));
  if(bind_cliente < 0) {
    printf("%s: Problema no bind\n", argv[0]);
    exit(1);
  }

  strcpy(nome_arquivo, argv[2]);
  printf("NOME ARQUIVO: %s\n", nome_arquivo);
  int passou = 0, j = 0;

  // Encontrando a extensão do arquivo
  for(int i = 0; i < strlen(nome_arquivo); i++){
    if(passou == 1){
        extensao[j] = nome_arquivo[i];
        j++;
    }
    if(nome_arquivo[i] == '.'){
        passou = 1;
    }
  }
  printf("EXTENSAO: %s\n",extensao);

  // Requisição para o download
  for(i = 2; i < argc; i++) {
    // Enviando requisição para o servidor
    sprintf(mensagem, "%s %s", argv[i], IP_LOCAL);
    while((bind_cliente = sendto(cliente_socket, mensagem, sizeof(mensagem) + 1, 0, 
		(struct sockaddr *) &server, sizeof(server))) < 0) {
            temporizador_de_dados(T_PADRAO);
    }

    printf("Para Server: %s\n", mensagem);

    int tam_cliente = sizeof(cliente);
    tam_server = sizeof(server);

    // Recebendo resposta do cliente
    while((recebido = recvfrom(cliente_socket, mensagem, TAM_MAX_MSG, 0, (struct sockaddr *) &cliente, &tam_cliente)) < 0){
        temporizador_de_dados(T_PADRAO);
    }
    printf("Server: %s\n", mensagem);
  }

  // Caso a resposta seja negatica, ou seja, o arquivo não existe ou já foi copiado.
  if(strncmp(mensagem, "NGC",3) == 0) {
    printf("Arquivo nao existe ou ja copiado!\n");
    
    exit(1);
  }

  int tam_ip=retorne_quantidade(mensagem,2);
  int tam_porta=retorne_quantidade(mensagem,3);

  // Armazenando o ip e a porta de envio que estão no buffer "mensagem"
  memcpy(ip_de_envio,&mensagem[4],tam_ip);
  memcpy(porta_de_envio,&mensagem[4+tam_ip+1],tam_porta);
  porta_de_envio_int = atoi(porta_de_envio);

  // Configurando o host do cliente2 para pedir o arquivo
  printf("%s: Configurando servidor do arquivo...\n", argv[0]);
  inicializar_host(&cliente_servidor, ip_de_envio, porta_de_envio_int);
  int tam_cliente = sizeof(cliente_servidor);

  // Abrindo arquivo para escrita binária
  FILE* arquivo = fopen(argv[2], "wb");

  char cabecalho_recebido[TAM_MAX_MSG]="", cabecalho_envio[TAM_MAX_MSG]="", dados[TAM_MAX_MSG-(LIMITE_DADOS-1)]="";
  char checksum[11]="";
  int num_pacote = 0, num_pacote_recebido = 0, termino_recebido = 0;
  

  // Criando o arquivo enquanto termino é diferente de 1
  while(termino_recebido != 1){
    // Resetando valores
    memset(mensagem, 0x0, TAM_MAX_MSG);
    memset(cabecalho_recebido, 0x0, TAM_MAX_MSG);
    memset(dados, 0x0, TAM_MAX_MSG - (LIMITE_DADOS - 1));
    memset(checksum, 0x0, sizeof(checksum));

    // Recebendo dados do cliente2
    while((recebido = recvfrom(cliente_socket, mensagem, TAM_MAX_MSG + 1, 0, (struct sockaddr *) &cliente_servidor, &tam_cliente)) < 0){
        temporizador_de_dados(T_PADRAO);
    }

    // Armazenando em um pacote
    memcpy(cabecalho_recebido, &mensagem[4], sizeof(mensagem) - 4);
    num_pacote_recebido = atoi(cabecalho_recebido);

    // Armazenando num do término
    memcpy(cabecalho_recebido, &cabecalho_recebido[digitos_numero(num_pacote_recebido) + 1], sizeof(cabecalho_recebido) - (digitos_numero(num_pacote_recebido) + 1));
    termino_recebido = atoi(cabecalho_recebido);

    // Salvando o checksum
    memcpy(checksum, &mensagem[4 + retorne_quantidade(mensagem, 2) + 1 + 1 + 1], sizeof(checksum) - 1);

    // Caso o termino for 1 sai do loop
    if(termino_recebido == 1){
        break;
    }

    // Armazena os dados arquivo que estão no buffer
    memcpy(dados, &mensagem[LIMITE_DADOS], sizeof(mensagem)-(LIMITE_DADOS-1));

    // Verificando se o num_pacote e o checksum estão corretos
    if(num_pacote_recebido == num_pacote && checksum_correto(checksum,mensagem) == 1) {
        // Enviando ACK para o cliente2
        memset(cabecalho_envio, 0x0, TAM_MAX_MSG);
        sprintf(cabecalho_envio,"ACK %d", num_pacote_recebido);
        while((bind_cliente = sendto(cliente_socket, cabecalho_envio, sizeof(cabecalho_envio), 0, 
    (struct sockaddr *) &cliente_servidor, sizeof(cliente_servidor))) < 0){
            temporizador_de_dados(T_PADRAO);
        }
        
        printf("Para Cliente2: %s\n", cabecalho_envio);

        // Reconstitui os dados do pacote
        if(strcmp(extensao,"txt") != 0){        
            fwrite(&dados, 1, sizeof(dados), arquivo);
        }else{
            fwrite(&dados, 1, strlen(dados), arquivo);
        }
        num_pacote++;
    }// Num_pacote diferente ou checksum incorreto
    else{
        // Enviando o erro no pacote para o cliente2
        sprintf(cabecalho_envio,"NGC %d", num_pacote);
        while((bind_cliente = sendto(cliente_socket, cabecalho_envio, sizeof(cabecalho_envio), 0, 
        (struct sockaddr *) &cliente_servidor, sizeof(cliente_servidor))) < 0){
            temporizador_de_dados(T_PADRAO);        
        }

    }

  }
  fclose(arquivo); // Fechando arquivo
  printf("\n%s: Arquivo recebido com sucesso!\n", argv[0]);

  return 0; // Fim :)
}
