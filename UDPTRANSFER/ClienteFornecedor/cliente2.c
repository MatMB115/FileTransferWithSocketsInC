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

#define SERVER_PORT 1500            //porta padrão do servidor
#define IP_SERVER "192.168.1.10"    //IPV4 do Servidor
#define TAM_MAX_MSG 1024                //tamanho max do buffer
#define T_PADRAO 1              //tempo padrão do temporizador
#define LIMITE_DADOS 50             //índice que possui o início dos dados

//Varíaveis Globais
int socket_cliente2, flag=0;
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

int main(int argc, char *argv[]) {

  int socket_cliente2, bind_cliente2;
  struct sockaddr_in cliente2, cliente, server;
  
  //Cria socket para cliente2
  socket_cliente2 = socket(AF_INET, SOCK_DGRAM, 0);
  if(socket_cliente2<0){
    printf("%s: Problema no socket\n", argv[0]);
    exit(1);
  }

  //Inicializar a struct do cliente2
  inicializar_sockaddr_in(&cliente2, SERVER_PORT);

  //Bind do cliente2
  bind_cliente2 = bind(socket_cliente2, (struct sockaddr *)&cliente2, sizeof(cliente2));
  if(bind_cliente2<0){
    printf("%s: Problema no bind\n", argv[0]);
    exit(1);
  }

  printf("Esperando requisicao...\n");

  int recebido = -1;
  char ip_cliente[15]="", porta_cliente[5]="", arquivo_cliente[30]="";
  int porta_cliente_int;

  //Inicializa o host do servidor
  printf("%s: Configurando servidor chefe...\n", argv[0]);
  inicializar_host(&server, IP_SERVER, SERVER_PORT);

  int tam_server = sizeof(server);

  //Espera requisições
  while(1){
      memset(mensagem,0x0,TAM_MAX_MSG);
      memset(ip_cliente,0x0,15);
      memset(porta_cliente,0x0,5);
      memset(arquivo_cliente,0x0,30);
      //espera receber alguma mensagem do servidor
      while((recebido = recvfrom(socket_cliente2, mensagem, TAM_MAX_MSG, 0, (struct sockaddr *) &server, &tam_server))<0){
        temporizador_de_dados(T_PADRAO);
      }

      printf("Server: %s\n",mensagem);

      //Se a mensagem for uma requisição
      if(strncmp(mensagem, "REQ",3)==0){
            //Armazena ip, porta, nome do arquivo
            int tam_ip = retorne_quantidade(mensagem,2);
            int tam_porta = retorne_quantidade(mensagem,3);
            int tam_arquivo = retorne_quantidade(mensagem,4);
            memcpy(ip_cliente,&mensagem[4],tam_ip);
            memcpy(porta_cliente,&mensagem[4+tam_ip+1],tam_porta);
            memcpy(arquivo_cliente,&mensagem[4+tam_ip+1+tam_porta+1],tam_arquivo);
            porta_cliente_int = atoi(porta_cliente);

            //Inicializa/configura host do cliente
            printf("%s: Configurando cliente para o envio...\n", argv[0]);
            inicializar_host(&cliente,ip_cliente,porta_cliente_int);
            
            //Envia resposta de confirmação pro server
            while((bind_cliente2 = sendto(socket_cliente2, "AC2", 4, 0, 
		        (struct sockaddr *) &server, sizeof(server)))<0){
                temporizador_de_dados(T_PADRAO);        
            }

            printf("Para Server: AC2\n");
        }

      //Inicializa o checksum e abre arquivo 
      char mensagem_envio[TAM_MAX_MSG]="", checksum[11]="0000000000";
      FILE* arquivo = fopen(arquivo_cliente, "rb");
      int num_pacote=0, termino=0, espaco_cabecalho = 0;

      //Envio de dados enquanto não for o final do arquivo
      while(!feof(arquivo)){
        memset(checksum,0x0,11);

        //tamanho do cabeçalho
        espaco_cabecalho = 4+digitos_numero(num_pacote)+1+digitos_numero(termino)+1+sizeof(checksum)-1+1;
        
        strcpy(checksum, "0000000000");
        
        printf("Pacote num: %d | cabecalho: %d\n", num_pacote, espaco_cabecalho);

        //Gera correção de espaços para manter dados na posição LIMITE_DADOS
        char correcao[LIMITE_DADOS]="";
        while(espaco_cabecalho<LIMITE_DADOS){
            strcat(correcao, " ");
            espaco_cabecalho++;
        }
        
        //Cria cabeçalho
        sprintf(mensagem_envio, "REQ %d %d %s %s", num_pacote, termino, checksum, correcao);

        //Se não deu problema no envio, então leia mais dados do arquivo
        if(flag==0){
            fread(&mensagem_envio[LIMITE_DADOS], 1, TAM_MAX_MSG-(LIMITE_DADOS-1), arquivo);
        }
        flag=0;
        
        //Gera checksum
        int j=0, i=0;
        for(i=LIMITE_DADOS;i<TAM_MAX_MSG; i++){            
            if(j>=10){
                j=0;            
            }
            checksum[j]+=mensagem_envio[i];
            j++;
        }    

        //Passa o checksum pro espaço no cabeçalho
        memcpy(&mensagem_envio[4+digitos_numero(num_pacote)+1+1+1],checksum,10);

        memset(correcao,0x0,LIMITE_DADOS);

        //Envia pacote para o cliente
        while((bind_cliente2 = sendto(socket_cliente2, mensagem_envio, sizeof(mensagem_envio)+1, 0,(struct sockaddr *)&cliente, sizeof(cliente)))<0){
            temporizador_de_dados(T_PADRAO);
        }

        //Reseta mensagem
        memset(mensagem, 0x0, TAM_MAX_MSG);

        int tam_cliente = sizeof(cliente);

        //Espera receber resposta do cliente
        while((recebido = recvfrom(socket_cliente2, mensagem, TAM_MAX_MSG, 0, (struct sockaddr *) &cliente, &tam_cliente))<0){
            temporizador_de_dados(T_PADRAO);
        }

        //Se a resposta for uma confirmação, vai pro próximo pacote
        if(strncmp("ACK",mensagem,3)==0){
            num_pacote++;
            memset(mensagem_envio,0x0,TAM_MAX_MSG);
        }//Senão se de erro, ele mantém os dados desse pacote para reenviar
        else if(strncmp("NGC",mensagem, 3)==0){
            //Pega num pacote com problema  
            char numQualquer[TAM_MAX_MSG];
            memcpy(numQualquer, &mensagem[4], sizeof(mensagem)-4);
            num_pacote = atoi(numQualquer); 
            memset(numQualquer,0x0,TAM_MAX_MSG);
            flag=1; 
        }

        printf("Cliente: %s\n",mensagem);

      }
      fclose(arquivo); //Fecha arquivo(já enviado)
    printf("%s: FINALIZADO ENVIO DO ARQUIVO\n", argv[0]);
    memset(mensagem_envio, 0x0, TAM_MAX_MSG);

      //Envia pacote de término(sem dados)
      sprintf(mensagem_envio, "REQ %d %d", num_pacote, termino+1);

      while((bind_cliente2 = sendto(socket_cliente2, mensagem_envio, strlen(mensagem_envio)+1, 0,
      (struct sockaddr *)&cliente, sizeof(cliente)))<0){
          temporizador_de_dados(T_PADRAO);
      }

    //Espera nova requisição
     printf("\nEsperando requisicao...\n");
  }
  return 0;
}
