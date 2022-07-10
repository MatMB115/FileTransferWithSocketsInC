#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h> //Manipular posições de memória
#include <unistd.h> 
#include <string.h> //Manipular strings
#include <stdbool.h>
#include <time.h> //Temporizar o envio

#define SERVER_PORT 1500 //porta padrão do servidor
#define TAM_MAX_MSG 100      //tamanho max do buffer
#define T_PADRAO 1   //tempo padrão do temporizador

//Varíaveis Globais
int socket_server, flag=1, flag2=1;
char mensagem[TAM_MAX_MSG]="";

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

int main(int argc, char *argv[]) {

  int bind_server;
  struct sockaddr_in cliente, server;
  
  //Cria socket do servidor
  socket_server = socket(AF_INET, SOCK_DGRAM, 0);
  if(socket_server<0){
    printf("%s: Problema no socket\n", argv[0]);
    exit(1);
  }
  
  //Inicializa estrutura sockaddr do servidor
  inicializar_sockaddr_in(&server, SERVER_PORT);

  //Bind do servidor
  bind_server = bind(socket_server, (struct sockaddr *)&server, sizeof(server));
  if(bind_server<0){
    printf("%s: Problema no bind\n", argv[0]);
    exit(1);
  }

  printf("Servidor Online\n");

  struct sockaddr_in cliente2;
  char nome[30]="", ip[15]="", porta[5]="", dados_enviar[TAM_MAX_MSG]="";
  int recebido = -1;
  char ip_cliente[15]="", ip_cliente2[15]="", porta_cliente2[5]="", arquivo_cliente2[30]="";
  int porta_cliente2_int;

  //Aguarda requisições
  while(1) {
    //Zera o buffer da mensagem e reseta flags
    memset(mensagem,0x0,TAM_MAX_MSG);
    flag2=1; flag=1;

    int tam_server = sizeof(server);

    //Espera receber dados
    while((recebido = recvfrom(socket_server, mensagem, TAM_MAX_MSG, 0, (struct sockaddr *) &server, &tam_server))<0){
      temporizador_de_dados(T_PADRAO);
    }

    printf("Buffer da mensagem: %s\n", mensagem);
    FILE *banco;

    //Se a mensagem for uma requisição de arquivo
    if(strcmp(mensagem, "AC2")!=0){

      int tam_arquivo = retorne_quantidade(mensagem,1);
      int tam_ip = retorne_quantidade(mensagem,2);

      char nome_mensagem[30]="";
      memset(nome_mensagem,0x0,30);
      memcpy(nome_mensagem,mensagem,tam_arquivo);
      
      memcpy(ip_cliente, &mensagem[tam_arquivo+1], tam_ip);
        
      //configura o host do cliente(quem requisitou o arquivo)  
      printf("%s: Configurando cliente requisitor\n",argv[0]);
      inicializar_host(&cliente, ip_cliente, SERVER_PORT);

      printf("IP DO CLIENTE: %s\n",ip_cliente);

      banco = fopen("banco.txt", "r+");

      //Verifica no banco se o arquivo existe
      while(!feof(banco)){
          fscanf(banco, "%s %s %s", nome,ip,porta);

          //verifica se o arquivo existe em algum nó
          if(strcmp(nome_mensagem, nome)==0 && flag==1){
              strcpy(arquivo_cliente2, nome);
              strcpy(ip_cliente2, ip);
              strcpy(porta_cliente2, porta);
              flag = 0;
          }
          //Verifica se o arquivo já foi passado para o cliente
          if(strcmp(nome_mensagem, nome)==0 && strcmp(ip_cliente,ip)==0){
              flag2 = 0;
          }
      }
      fclose(banco);

      //Se o cliente já possui o arquivo
      if(flag2==0){
        //Envia resposta apenas para o cliente
        sprintf(dados_enviar, "NGC");
        bind_server = sendto(socket_server, dados_enviar, sizeof(dados_enviar)+1, 0, 
        (struct sockaddr *) &cliente, sizeof(cliente));
        printf("Para Cliente: %s\n", dados_enviar);
      }//Se existe um nó com o arquivo
      else if(flag==0){
        //Envia resposta para o cliente1 e depois para cliente2
        sprintf(dados_enviar,"ACK %s %s",ip_cliente2,porta_cliente2);

        //Envia dados pro cliente
        bind_server = sendto(socket_server, dados_enviar, sizeof(dados_enviar)+1, 0, 
          (struct sockaddr *) &cliente, sizeof(cliente));
        printf("Para Cliente: %s\n", dados_enviar);
          //Configura o host do cliente2
          porta_cliente2_int = atoi(porta_cliente2);
          printf("%s: Configurando hospedeiro do arquivo...\n", argv[0]);
          inicializar_host(&cliente2, ip_cliente2, porta_cliente2_int);
          
            temporizador_de_dados(T_PADRAO);

          //Envia dados pro cliente2
          sprintf(dados_enviar,"REQ %s %s %s",ip_cliente, porta_cliente2,arquivo_cliente2);

          while((bind_server = sendto(socket_server, dados_enviar, sizeof(dados_enviar)+1, 0, 
          (struct sockaddr *) &cliente2, sizeof(cliente2)))<0){
            temporizador_de_dados(T_PADRAO);
          }
        printf("Para Cliente2: %s\n", dados_enviar);
      }//Se o arquivo não existe
        else{
        //Envia resposta apenas para o cliente
        bind_server = sendto(socket_server, "NGC", 4, 0, 
          (struct sockaddr *) &cliente, sizeof(cliente));
        printf("%s: O arquivo especificado nao existe\n", argv[0]);
      }
    }//Se o que recebeu for AC2(confirmação do cliente2)
    else{        
        //Escreve no banco que o cliente possui o arquivo
        banco = fopen("banco.txt", "a");
        fprintf(banco, "%s %s %s\n", arquivo_cliente2, ip_cliente, porta_cliente2);
        fclose(banco);
    }
  }
  return 0;
}
