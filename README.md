# File_Transfer_With_Sockets/UDP_In_C
  Implementar em linguagem C a transferência de arquivos (vídeo, música, pdf, doc, etc) entre sistemas finais remotos. O ambiente deverá ser composto por no mínimo dois usuários e um servidor (Rastreador), o qual possui informações sobre os nós que possuem determinado arquivo. Uma vez que o protocolo UDP não fornece a transferência confiável de dados, as transferências devem considerar uma transmissão confiável a nível da Camada de Aplicação por meio da implementação dos tópicos vistos em sala de aula: soma de verificação (checksum), número de sequência, temporizador e pacote de reconhecimento. Não é permitida a utilização do protocolo TCP.

## Ambiente de testes
  Dois cenários possíveis: 

**Três máquinas virtuais:** com o sistema operacional Linux instalado(nos testes foi utilizado o Linux Mint 19) com repositório atualizado e gcc/gcc multilib instalados(comando: sudo apt-get install gcc gcc-multilib). A rede de cada máquina virtual deve ser configurada para “modo bridge” e cada um deve ter um IPv4 diferente e caso não tenha deve ser configurado manualmente.

**Três máquinas físicas:** com o sistema operacional Linux instalado (nos testes foi utilizado o Linux Mint 19) com repositório atualizado e gcc/gcc multilib instalados (comando: sudo apt-get install gcc gcc-multilib).

## Arquivos .c
**server.c:** código utilizado pelo servidor para enviar respostas aos clientes, administra quem vai enviar o arquivo para quem está requisitando e salva informações no banco.txt dos arquivos que os nós possuem.

**cliente.c:** código utilizado para o cliente que irá requisitar algum arquivo ao server, requisita e espera uma resposta e um provável envio do arquivo por parte de um outro nó.

**cliente2.c:** código que espera a solicitação do servidor para enviar um arquivo especificado para algum nó, ou seja, ele espera a comunicação do servidor e depois se conecta ao nó que está requisitando um arquivo e o envia.



## Requisito
- Os aquivos server.c e banco.txt deve ser colocados juntos na mesma pasta da máquina que será o servidor.

- O cliente2.c deve ser colocado em uma pasta da máquina junto aos arquivos que serão transferidos.

- É recomendado colocar o cliente.c em uma pasta de uma das três máquinas disponíveis.

- No banco.txt deve ser colocado em cada linha as seguintes informações:
nome_do_arquivo.extensão ip_origem_arquivo porta

- Há arquivos previamente fornecidos para o teste.

## Alterar
- Deve ser alterado os ips e os nomes dos arquivos no banco.txt de acordo com o ip do cliente2 e os seus arquivos disponíveis;

- O IP_SERVER no #define do cliente2.c deve ser alterado para o ip do servidor da máquina atual;

- O IP_LOCAL do #define do cliente.c deve ser alterado para o ip da maquina que está rodando o cliente.c.

## Compilar
Para compilar é necessário abrir um terminal na pasta em que os arquivos estão e utilizar o comando:

> gcc nome_arquivo.c -o nome_executavel

## Executar
Antes é necessário navegar pelos diretórios no terminal até a pasta onde foram colocados os arquivos em cada máquina.

Para os server.c e cliente2.c será necessário apenas executar o programa no terminal com seguinte comando:

> ./nome_executavel

Já para executar o cliente.c é necessário, em sua respectiva máquina virtual, utilizar o comando:

> ./nome_executavel ip_do_server nome_do_arquivo.extensão


## Etapas concluídas
- [x] Cliente solicitador comunica com servidor;
- [x] Servidor cria um banco de dados;
- [x] Servidor consulta banco de dados;
- [x] Servidor retorna mensagem (dados do struct de quem possui o arquivo) para Cliente solicitador;
- [x] Cliente A solicita arquivo ao Cliente B;
- [x] Cliente B lê arquivo solicitado, escreve no buffer de memória e envia ao Cliente A;
- [x] Cliente A recebe os blocos do arquivo solicitado e escreve em um novo arquivo com o mesmo nome;
- [x] Servidor registra recebimento do arquivo;
- [x] Servidor atualiza o banco de dados;
- [x] Número do Pacote;
- [x] Checksum;
- [x] Temporizador;
- [x] Número de Reconhecimento.


*Grupo 4:<br>
Adriano Lucas Ferreira - 2020006711<br>
Kaique de Souza Leal Silva - 2020003596 <br>
Matheus Martins Batista - 2019005687<br>*