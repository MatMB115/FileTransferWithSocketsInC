

PASSOS PARA EXECUTAR OS ARQUIVOS:


** SISTEMA OPERACIONAL UTILIZADO: Linux


									
************* SERVIDOR ***************


1º Você deverá se atentar para o codigo, nas linhas 125 e 170 pois elas se referem ao diretório que os clientes teram acesso aos arquivos para baixar, configurar estes linhas com o caminho da sua pasta desejada. O servidor enviara uma mensagem para o cliente com os arquivos disponiveis dentro da pasta. O servidor consegue transmitir para o cliente todo tipo de arquivo.

	
Ex: Se quiser transferir os arquivos que estão na pasta Downloads, subistitua por 

		
linha 125: opendir("/home/jonas/Downloads")
		
linha 170: strcpy(caminho_arquivo, "/home/jonas/Downloads/") -- Nesta linha no final do caminho da pasta deve contar '/'


2º Compilar o codigo usando o gcc

3º Executar o servidor 
	


									

************* CLIENTE ****************


1º Compilar o arquivo usando o gcc


2º Executar o arquivo com o seguinte comando:

	-- Exemplo de comando caso voce compilou e nomeou o arquivo como cliente, caso contrário substitua cliente pelo nome so arquivo escolhido.
 
     ./cliente <ip_do_servidor> connect


3º Digitar o nome completo de uns dos arquivos informados pelo servidor que estão disponiveis para você baixar. 
Ex.: November_rain.mp3.
	 
O arquivo será salvo na raiz aonde esta sendo executado o cliente



