#include <sys/types.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <dirent.h>

// Porta uDP
#define LOCAL_SERVER_PORT 1500
// Tamanho maximo da mensagem
#define MAX_MSG 2000
// Tamanho maximo da parte da mensagem, que este pacote contem
#define MAX_MSG_PACOTE 50

//estrutura do pacote a se enviar
typedef struct pacote
{
    int numero_pact;
    unsigned char check_sum;
    int tamanho;
    char part_msg[MAX_MSG_PACOTE];
} pacote ;

//estrutura da resposta (ACK) a se receber
typedef struct resposta
{
    int resposta;
    int numero_pact;
} resposta;

unsigned char checksum (char* s)
{
	signed char sum = -1;
	while (*s != 0)
	{
		sum += *s;
		s++;
	}
	return sum;
}

/*
	Funcao responsavel por criar o pacote binario para ser enviado ao cliente;
	recebendo a parte do arquivo binario que se deseja enviar, justapoe-lhe o seu tamanho,
	o numero do pacote, e tambem o seu valor de chacksum para o cliente confirmar.

*/

pacote criaPacote(char *arq_em_Binario,int numero_pact,int *tamanho)
{
    pacote p;
    int i=0;
    for(i=0; i<(*tamanho); i++)
        p.part_msg[i]=arq_em_Binario[i];

    p.check_sum = checksum(p.part_msg);
    p.numero_pact = numero_pact;
    p.tamanho = *tamanho;
    return p;
}

int main(int argc, char *argv[])
{
    int sd, rc, numero_sequencia, n, cliLen, opt, tamanho, i=0, aux;
    struct sockaddr_in cliAddr, servAddr;
    char msg[MAX_MSG], caminho_arquivo[20], *msg_pacote, dados[MAX_MSG_PACOTE], arq_diretorio[MAX_MSG] = {}, parte_msg[MAX_MSG_PACOTE];
    DIR *dir;
    struct dirent *lsdir;
    fd_set select_fds; /* fd's usado por select */
	struct timeval timeout; /* Valor de tempo para timeout */
    FILE *arq;
    pacote p;
    resposta rsp;

    /* Criacao do socket */
    sd=socket(AF_INET, SOCK_DGRAM, 0);
    if(sd<0)
    {
        printf("%s: cannot open socket \n",argv[0]);
        exit(1);
    }

    /* bind local server port */
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servAddr.sin_port = htons( LOCAL_SERVER_PORT );
    rc = bind (sd, (struct sockaddr *) &servAddr,sizeof(servAddr));
    if(rc<0)
    {
        printf("%s: cannot bind port number %d \n",
               argv[0], LOCAL_SERVER_PORT);
        exit(1);
    }

    printf("%s: waiting for data on port uDP %u\n",
           argv[0],LOCAL_SERVER_PORT);


    while(1)
    {
        memset(msg,0x0,MAX_MSG);

        cliLen = sizeof(cliAddr);
        n = recvfrom(sd, msg, MAX_MSG, 0,
                     (struct sockaddr *) &cliAddr, &cliLen);

        if(n<0)
        {
            printf("%s: cannot receive data \n",argv[0]);
        }

        if(msg[0] == '1')
        {

            /*
            	utilizando o readdir das bibliotecas <sys/types.h> e <dirent.h> conseguimos fazer uma listagem do diretorio,
            	que e mandada para o cliente para que ele escolha qual arquivo deseja receber. os arquivos sao colocados em um vetor de
            	caracteres e enviado para o cliente.
            */

            dir = opendir("/home/maysu/Redes");

            strcpy(arq_diretorio,"\n \t Arquivos disponiveis no servidor: \n * ");

            while ( ( lsdir = readdir(dir) ) != NULL )
            {

                strcpy(msg, lsdir->d_name);
                if(!((!strcmp(msg,"."))||(!strcmp(msg,".."))))
                {
                    strcat(arq_diretorio, msg);
                    strcat(arq_diretorio, "\n * ");
                }
            }

            closedir(dir);

            rc = sendto(sd, arq_diretorio, strlen(arq_diretorio)+1, 0, (struct sockaddr *) &cliAddr, sizeof(cliAddr));

            if(rc<0)
            {
                printf("%s: cannot send data\n",argv[0]);
                close(sd);
                exit(1);
            }

            while(1)
            {
                memset(msg,0x0,MAX_MSG);

                cliLen = sizeof(cliAddr);
                n = recvfrom(sd, msg, MAX_MSG, 0, (struct sockaddr *) &cliAddr, &cliLen);

                /*
                	Apos o cliente digitar o arquivo que deseja, o servidor tenta-o abrir; caso o nome inserido esteja errado,
                	a aplicacao e encerrada.
                */


                if(n<0)
                {
                    printf("%s: cannot receive data \n",argv[0]);
                    continue;
                }

                strcpy(caminho_arquivo, "/home/maysu/Redes");
                strcat(caminho_arquivo, msg);

                arq = fopen(caminho_arquivo,"rb");
                if(!arq)
                {
                    printf("\n erro Fatal! Nao foi possivel abrir o arquivo.\n\n");
                    exit(1);
                }

                numero_sequencia = 1;

                /*
                	Apos a abertura do arquivo, com a funcao fread podemos ler o arquivo binario, em partes
                	cujo tamanho maximo e definido pela constante MAX_MSG_PACOTE.
                */

                tamanho = fread(&dados,sizeof(char),MAX_MSG_PACOTE,arq);
                if (tamanho == 0)
                    strcpy(dados,"Transferencia Realizada");
                else
                {
                    msg_pacote=(char*)calloc(tamanho,sizeof(char));
                }
                for (i=0; i<tamanho; i++)
                {
                    msg_pacote[i]=dados[i];
                }
                while(strcmp(msg_pacote,"Transferencia Realizada"))
                {

                    /*
                    	enquanto nao atingirmos o termino do arquivo, vamos lendo o arquivo binario em blocos e enviando para o cliente
                    */

                    p = criaPacote(msg_pacote,numero_sequencia, &tamanho);

                    Envio:
                    sendto(sd,&p, sizeof(pacote)+1, 0,(struct sockaddr *) &cliAddr,sizeof(cliAddr));
                    memset(&rsp,0x0,sizeof(rsp));
                    aux = sizeof(cliAddr);

		            /* -----------------------------------------
		             Ajustando a descricao para o select()
		             ----------------------------------------- */
		            FD_ZERO(&select_fds); /* Limpando o fd's */
		            FD_SET(sd, &select_fds); /* Ajustando o bit que corresponde ao soquete */

		            /* -----------------------------------------
		             Ajustando o valor de expiracao
		             ----------------------------------------- */
		            timeout.tv_sec = 2;
		            timeout.tv_usec = 0;

				    /**
				     * [TeMPoRiZADoR]
				         * Perda de conexao.
				         * o tempo maximo permitido e de 5 segundos;
				    */

			        if (select(32, &select_fds, NULL, NULL, &timeout) == 0){
			            printf("\nAviso: o temporizador estourou! Reenviando Pacote...\n");
			            goto Envio;
			        } else {
			            printf("--- Resposta do cliente recebida! \n");
			        }

                    recvfrom(sd,&rsp,sizeof(resposta), 0,(struct sockaddr *) &cliAddr, &aux);
                    printf("-- Pacote numero %d enviado ao cliente!  \n",numero_sequencia);
                    tamanho=fread(&parte_msg,sizeof(char),MAX_MSG_PACOTE,arq);
                    if (tamanho==0)
                    {
                        strcpy(msg_pacote,"Transferencia Realizada");
                    }
                    else
                    {
                        free(msg_pacote);
                        msg_pacote=(char*)calloc(tamanho,sizeof(char));
                        for (i=0; i<tamanho; i++)
                        {
                            msg_pacote[i]=parte_msg[i];
                        }
                    }

                    /*
                    	Se a resposta ao pacote pelo cliente for 1, quer dizer que o recebeu perfeitamente; entao incrementamos o numero de sequencia e enviamos o
                    	proximo pacote. Caso retorne zero, nao reenviamos o mesmo pacote para o cliente, pois algo aconteceu com o pacote.
                    */

                    if(rsp.resposta)
                        numero_sequencia+=1;
                    else
                        printf("Aviso: Reenviando pacote %d ",numero_sequencia);
                }

                /*
                	Quando terminamos de ler o arquivo e enviamos o ultimo pacote, e a funcao fread retorna 0, definimos a transferencia como finalizada e avisamos
                	o cliente que a transferencia foi finalizada e encerramos a conexao
                */

                strcpy(p.part_msg,"Transferencia Realizada");
                rc = sendto(sd, &p, sizeof(pacote)+1, 0,(struct sockaddr *) &cliAddr,sizeof(cliAddr));
                printf("\t\n =======================================================================\n");
                printf("\n \t Arquivo enviado! \n");
                printf("\t\n =======================================================================\n\n");
                fclose(arq);
                exit(0);
            }
        }
    }
}
