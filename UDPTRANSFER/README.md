# Transferência Confiável de Arquivos Ponto a Ponto com UDP

## :mag: Requisitos
Para executar o código precisa de três máquinas virtuais com o sistema operacional Linux instalado(o utilizado por nós foi o Linux Mint 19) com repositório atualizado e gcc instalado(comando: sudo apt get install gcc). A rede de cada máquina virtual deve ser configurada para em “modo bridge” e cada um deve ter um ipv4 diferente e caso não tenha deve ser configurado manualmente(tirando a opção do DHCP gerar o ip automaticamente).

## :bell: Informações
O arquivo server.c deve ser colocado na primeira máquina virtual, além disso o arquivo banco.txt também deve ser colocado na mesma máquina virtual.
O arquivo cliente.c deve ser colocado em uma segunda máquina virtual.
O arquivo cliente2.c deve ser colocado em uma terceira máquina virtual junto com algum arquivo de texto ou vídeo ou imagem para a transferência.

No banco.txt deve ser colocado em cada linha as seguintes informações:
nome_do_arquivo.extensão ip_onde_está_o_arquivo porta

O banco é responsável por saber os arquivos de cada nó.

Os arquivos gif.gif, video.mp4, planeta.jpg, mickey.png e aa.txt podem ser usados como teste de transferência pelo cliente2.

## :pencil2: Alterar
Deve ser alterado no banco.txt os arquivos e o ip caso seja necessário.
Deve ser alterado o IP_SERVER no #define do cliente2.c para o ip da máquina virtual que estiver com o server.c
Deve ser alterado o IP_LOCAL do #define do cliente.c para o ip da máquina virtual que estiver com o cliente.c

## :microscope: Compilar
Para compilar deve abrir um terminal na pasta dos arquivos e utilizar o comando:

> gcc nome_arquivo.c -o nome_executavel

## :bicyclist: Executar
Para executar o server.c e o cliente2.c deve abrir um terminal na pasta dos arquivos(já compilados) e em suas respectivas máquinas virtuais e utilizar o comando:

> ./nome_executavel

Para executar o cliente.c deve abrir um terminal na pasta dos arquivos(já compilados) e em sua respectiva máquina virtual e utilizar o comando:

> ./nome_executavel ip_do_server nome_do_arquivo.extensão

## :eyeglasses: Explicação dos .c
**server.c:** é um arquivo utilizado pelo servidor para enviar respostas aos clientes, ele que administra quem vai enviar o arquivo para quem está requisitando e salva informações no banco.txt dos arquivos que os nós possuem.

**cliente.c:** é um arquivo utilizado para o cliente que irá requisitar algum arquivo ao server, ele requisita e espera uma resposta e um provável envio do arquivo por parte de um outro nó.

**cliente2.c:** é um arquivo utilizado para esperar a solicitação do server para o envio de algum arquivo para algum nó, ou seja, ele espera a comunicação com o servidor e depois se conecta a um nó que requisitou um arquivo e o envia.

**Obs.:** o programa apresenta número de pacote, reconhecimento, checksum e temporizador de dados para garantir a entrega dos pacotes e a formação do arquivo em UDP(sock_dgram).
Se você tiver alguma dúvida ou sugestão ou encontrar algum erro, contate-me porfavor. Obrigado.
