#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>

#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

// Porta de servidor
#define ReMoTe_SeRVeR_PoRT 1500
// Tamanho maximo da mensagem
#define MaX_MSG 2000
// Tamanho maximo da parte da mensagem, que este pacote contem
#define MaX_MSG_PaCoTe 50

//estrutura de tipo do pacote a ser enviado
typedef struct pacote
{
    int numero_pact;
    unsigned char check_sum;
    int Tamanho;
    char part_msg[MaX_MSG_PaCoTe];
} pacote ;

//estrutura da resposta
typedef struct resposta
{
    int resposta;
    int numero_pact;
} resposta;

//Funcao checksum
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

int main(int argc, char *argv[])
{
    pacote p;
    resposta rsp;
    unsigned char valor_checksum;
    FiLe *arquivo;
    char msg[MaX_MSG];
    int sd, rc, i, n, servLen, j=0, escolha = 0, opt;
    struct sockaddr_in cliaddr, remoteServaddr;
    struct hostent *h;

    //Segue agora o codigo fornecido pelo professor, para a conexao uDP
    if(argc<3)
    {
        printf("usage : %s <server> <data1> ... <dataN> \n", argv[0]);
        exit(1);
    }

    h = gethostbyname(argv[1]);
    if(h==NuLL)
    {
        printf("%s: unknown host '%s' \n", argv[0], argv[1]);
        exit(1);
    }

    printf("%s: sending data to '%s' (iP : %s) \n", argv[0], h->h_name,
           inet_ntoa(*(struct in_addr *)h->h_addr_list[0]));

    remoteServaddr.sin_family = h->h_addrtype;
    memcpy((char *) &remoteServaddr.sin_addr.s_addr,
           h->h_addr_list[0], h->h_length);
    remoteServaddr.sin_port = htons(ReMoTe_SeRVeR_PoRT);


    sd = socket(aF_iNeT,SoCK_DGRaM,0);
    if(sd<0)
    {
        printf("%s: cannot open socket \n",argv[0]);
        exit(1);
    }

    cliaddr.sin_family = aF_iNeT;
    cliaddr.sin_addr.s_addr = htonl(iNaDDR_aNY);
    cliaddr.sin_port = htons(0);

    rc = bind(sd, (struct sockaddr *) &cliaddr, sizeof(cliaddr));
    if(rc<0)
    {
        printf("%s: cannot bind port\n", argv[0]);
        exit(1);
    }


    for(i=2; i<argc; i++)
    {
        rc = sendto(sd, argv[i], strlen(argv[i])+1, 0,
                    (struct sockaddr *) &remoteServaddr,
                    sizeof(remoteServaddr));

        if(rc<0)
        {
            printf("%s: cannot send data %d \n",argv[0],i-1);
            close(sd);
            exit(1);
        }

    }

    msg[0]='1';

    rc = sendto(sd, msg, strlen(msg)+1, 0, (struct sockaddr *) &remoteServaddr, sizeof(remoteServaddr));

    if(rc<0)
    {
        printf("%s: cannot send data!\n",argv[0]);
        close(sd);
        exit(1);
    }

    memset(msg,0x0,MaX_MSG);

    servLen = sizeof(remoteServaddr);
    n = recvfrom(sd, msg, MaX_MSG, 0, (struct sockaddr *) &remoteServaddr, &servLen);

    if(n<0)
    {
        printf("%s: cannot receive data \n",argv[0]);
    }

    printf("%s: from %s:uDP%u : %s \n", argv[0],inet_ntoa(remoteServaddr.sin_addr), ntohs(remoteServaddr.sin_port),msg);

    /*
    	o servidor mostrara os arquivos disponiveis para serem transferidos e o usuario
    	tera de digitar exatamente um deles; se nao, a aplicacao e encerrada
    */

    printf("insira o nome do arquivo ou pressione Ctrl+C para encerrar: ");
    scanf(" %[^\n]s ", msg);
    setbuf(stdin,NuLL);

    arquivo = fopen(msg,"wt");
    if(!arquivo)
    {
        printf("\nerro fatal! Voce nao informou um nome de arquivo,\n ou informou um que nao esta disponivel!\n\n");
        return 1;
    }
    rc = sendto(sd, msg, strlen(msg)+1, 0, (struct sockaddr *) &remoteServaddr, sizeof(remoteServaddr));

    if(rc<0)
    {
        printf("%s: cannot send data!\n",argv[0]);
        close(sd);
        exit(1);
    }


    while(1)
    {

        /*
        	enquanto o servidor nao envia uma mensagem avisando que a transferencia foi finalizada,
        	ele fica aguardando o proximo pacote. Caso ele recebe ele confere com o checksum e se o valor bater
        	ele responde com 1 ae ele recebe o proximo pacote, caso ao contrario ele responde 0 e recebe do servidor o mesmo pacote
        */

        memset(&p,0x0,MaX_MSG);
        servLen = sizeof(remoteServaddr);
        n = recvfrom(sd,&p, sizeof(pacote)+1, 0,(struct sockaddr *) &remoteServaddr, &servLen);
        if (!(strcmp(p.part_msg,"TRaNFeReNCia FiNaLiZaDa")))
            break;
        if(n<0)
        {
            printf("aviso: Pacote recebido com erro, ou nao recebido; solitando reenvio...\n");
            rsp.numero_pact = p.numero_pact;
            rsp.resposta = 0;
            sendto(sd,&rsp, sizeof(resposta), 0,(struct sockaddr *) &remoteServaddr, sizeof(remoteServaddr));
        }
        else
        {
            printf("** Pacote numero %d recebido do servidor com sucesso.\n",p.numero_pact);
            valor_checksum = checksum(p.part_msg);
            if( valor_checksum == p.check_sum )
            {
                printf("*** Checksum do pacote %d confere com o esperado!\n",p.numero_pact);
                fwrite(p.part_msg,sizeof(char),p.Tamanho,arquivo);
                rsp.numero_pact = p.numero_pact;
                rsp.resposta = 1;
                sendto(sd,&rsp, sizeof(resposta), 0,(struct sockaddr *) &remoteServaddr, sizeof(remoteServaddr));
            }
            else
            {
                printf("*** Checksum do pacote %d Nao confere com o esperado. Solicitando reenvio...\n",p.numero_pact);
                rsp.numero_pact = p.numero_pact;
                rsp.resposta = 0;
                rc = sendto(sd,&rsp, sizeof(resposta), 0,(struct sockaddr *) &remoteServaddr, sizeof(remoteServaddr));
            }
        }
    }
    if (!strcmp(p.part_msg,"TRaNFeReNCia FiNaLiZaDa"))
    {
        printf("\t\n =======================================================================\n");
        printf("\n \t arquivo recebido completamente e salvo na raiz! \n");
        printf("\t\n =======================================================================\n\n");

    }
    fclose(arquivo);
    return 0;
}
