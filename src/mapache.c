/* Por:
 *   Albert De La Fuente <albert@ime.usp.br>
 *   Renan De Melo <> 
 *
 * Originalmente:
 * Por Prof. Daniel Batista <batista@ime.usp.br>
 * Em 12/08/2013
 * 
 * Um c�digo simples (n�o � o c�digo ideal, mas � o suficiente para o
 * EP) de um servidor de eco a ser usado como base para o EP1. Ele
 * recebe uma linha de um cliente e devolve a mesma linha. Teste ele
 * assim depois de compilar:
 * 
 * ./servidor 8000
 * 
 * Com este comando o servidor ficar� escutando por conex�es na porta
 * 8000 TCP (Se voc� quiser fazer o servidor escutar em uma porta
 * menor que 1024 voc� precisa ser root).
 *
 * Depois conecte no servidor via telnet. Rode em outro terminal:
 * 
 * telnet 127.0.0.1 8000
 * 
 * Escreva sequ�ncias de caracteres seguidas de ENTER. Voc� ver� que
 * o telnet exige a mesma linha em seguida. Esta repeti��o da linha �
 * enviada pelo servidor. O servidor tamb�m exibe no terminal onde ele
 * estiver rodando as linhas enviadas pelos clientes.
 * 
 * Obs.: Voc� pode conectar no servidor remotamente tamb�m. Basta saber o
 * endere�o IP remoto da m�quina onde o servidor est� rodando e n�o
 * pode haver nenhum firewall no meio do caminho bloqueando conex�es na
 * porta escolhida.
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <time.h>
#include <unistd.h>

#define LISTENQ 1
#define MAXDATASIZE 100
#define MAXLINE 4096

enum Req_Method { GET, POST, OPTIONS, UNSUPPORTED };

struct ReqInfo {
	enum Req_Method method;
	char	       *httpVersion;
	char           *referer;
	char           *useragent;
	char           *resource;
	char	       *content;
};

void Error_Quit(char const * msg) {
	fprintf(stderr, "WEBSERV: %s\n", msg);
	exit(EXIT_FAILURE);
}

ssize_t Writeline(int sockd, const void *vptr, size_t n) {
	size_t      nleft;
	ssize_t     nwritten;
	const char *buffer;

	buffer = vptr;
	nleft  = n;

	while ( nleft > 0 ) {
		if ( (nwritten = write(sockd, buffer, nleft)) <= 0 ) {
			if ( errno == EINTR )
				nwritten = 0;
			else
				Error_Quit("Error Writeline()");
		}
		nleft  -= nwritten;
		buffer += nwritten;
	}

	return n;
}

void mynonprint(const char *buf, int max) {
	int i;
	for (i = 0; i < max; i++) {
		if ((buf[i] >= 0 && buf[i] <=31) || (buf[i] == 127)) {
			printf("\\%d", buf[i]);
		}
		else {
			printf("%c", buf[i]);
		}
	}
	printf("\n");
}

int comando_options(int connfd, char * recvline, struct ReqInfo * reqinfo) {
/* HTTP/1.1 200 OK
 * < Date: Wed, 21 Aug 2013 01:17:31 GMT
 * < Server: Apache/2.2.22 (Linux/SUSE)
 * < Allow: POST,OPTIONS,GET,HEAD,TRACE
 * < Content-Length: 0
 * < Content-Type: text/html */
	
	char buffer[1000];
	time_t now = time(0);
	struct tm tm = *gmtime(&now);

	sprintf(buffer, "HTTP/1.1 200 OK\r\n");
	Writeline(connfd, buffer, strlen(buffer));

	strftime(buffer, sizeof buffer, "Date: %a, %d %b %Y %H:%M:%S %Z\r\n", &tm);
	Writeline(connfd, buffer, strlen(buffer));

	sprintf(buffer, "Server: Mapache/0.1\r\n");
	Writeline(connfd, buffer, strlen(buffer));

	sprintf(buffer, "Allow: POST,OPTIONS,GET\r\n");
	Writeline(connfd, buffer, strlen(buffer));

	sprintf(buffer, "Content-Length: 0\r\n");
	Writeline(connfd, buffer, strlen(buffer));

	sprintf(buffer, "Content-Type: text/html\r\n");
	Writeline(connfd, buffer, strlen(buffer));
	sprintf(buffer, "\r\n");
	Writeline(connfd, buffer, strlen(buffer));

	return 0;
}

int arquivoExiste(const char *fname)
{
    //TODO verificar se eh diretorio
    FILE *file;
    if (file = fopen(fname, "r"))
    {
        fclose(file);
        return 1;
    }
    return 0;
}


char* getAbsolutePath(char* recurso){
		char* prefixo= "../web";
		char* path = malloc(strlen(prefixo) + strlen(recurso) + 1);
		strcpy(path, prefixo);
		strcat(path, recurso);
		char actualpath [1000];
		realpath(path,actualpath);
		return actualpath;
}

void comando_get(int connfd, char * recvline, struct ReqInfo * reqinfo) {
	char *arquivoAbsolute = getAbsolutePath(reqinfo->resource);
	if(arquivoExiste(arquivoAbsolute)){
		le_escreve_arquivo(connfd,arquivoAbsolute);
	}
	else{
		arquivo_nao_encontrado(connfd,reqinfo->resource);
	}
}

void arquivo_nao_encontrado(int connfd,char* arquivo){
	int SIZE = 1000;
	char buffer[SIZE];
	char content_buffer[SIZE];
	time_t now = time(0);
	struct tm tm = *gmtime(&now);
	//Escrevendo o conteudo para saber o tamanho total
	sprintf(content_buffer, "<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML 2.0//EN\">\r\n<html><head>\r\n<title>404 Not Found</title>\r\n</head><body>\r\n<h1>Not Found</h1>\r\n<p>The requested URL %s was not found on this server.</p>\r\n</body></html>",arquivo);
	sprintf(buffer, "HTTP/1.1 404 Not Found\r\n");
	Writeline(connfd, buffer, strlen(buffer));
	strftime(buffer, sizeof buffer, "Date: %a, %d %b %Y %H:%M:%S %Z\r\n", &tm);
	Writeline(connfd, buffer, strlen(buffer));
	sprintf(buffer, "Server: Mapache/0.1\r\n");
	Writeline(connfd, buffer, strlen(buffer));
	sprintf(buffer, "Content-Type: text/html; charset=iso-8859-1\r\n");
	Writeline(connfd, buffer, strlen(buffer));
	sprintf(buffer, "Content-Length: %d\r\n",strlen(content_buffer));
	Writeline(connfd, buffer, strlen(buffer));
	sprintf(buffer, "Connection: Keep-Alive\r\n");
	Writeline(connfd, buffer, strlen(buffer));
	sprintf(buffer, "\r\n");
	Writeline(connfd, buffer, strlen(buffer));
	Writeline(connfd, content_buffer, strlen(content_buffer));
	free(buffer);
	free(content_buffer);
}

char* getContentType(char* arquivo){
    char* ext = strchr(arquivo,'.');
    if(!strcmp(ext+1,"html")){
	return "text/html";
    }
    if(!strcmp(ext+1,"png")){
	return "image/png";
    }
    return "application/octet-stream";
}


void le_escreve_arquivo(int connfd,char* arquivo){
	int SIZE = 1000;
	char buffer[SIZE];
	time_t now = time(0);
	struct tm tm = *gmtime(&now);

	FILE *fp;
	long lSize;
	char *fbuffer;

	sprintf(buffer, "HTTP/1.1 200 OK\r\n");
	Writeline(connfd, buffer, strlen(buffer));

	strftime(buffer, sizeof buffer, "Date: %a, %d %b %Y %H:%M:%S %Z\r\n", &tm);
	Writeline(connfd, buffer, strlen(buffer));

	fp = fopen ( arquivo , "rb" );
	if( !fp ) perror(arquivo),exit(1);

	fseek( fp , 0L , SEEK_END);
	lSize = ftell(fp);
	rewind(fp);

	sprintf(buffer, "Server: Mapache/0.1\r\n");
	Writeline(connfd, buffer, strlen(buffer));

	sprintf(buffer, "Content-Type: %s\r\n",getContentType(arquivo));
	Writeline(connfd, buffer, strlen(buffer));

	sprintf(buffer, "Content-Length: %ld\r\n",lSize);
	Writeline(connfd, buffer, strlen(buffer));

	sprintf(buffer, "\r\n");
	Writeline(connfd, buffer, strlen(buffer));

	fbuffer = calloc( 1, lSize+1 );
	if( !fbuffer ) fclose(fp),fputs("memory alloc fails",stderr),exit(1);

	if( 1!=fread( fbuffer , lSize, 1 , fp) )
		fclose(fp),free(fbuffer),fputs("entire read fails",stderr),exit(1);

	Writeline(connfd, fbuffer, lSize);

	fclose(fp);
	free(buffer);
}


int comando_post(int connfd, char * recvline, struct ReqInfo * reqinfo) {
/*
Request:
        POST /post-form.php HTTP/1.1
        Host: localhost
        User-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:23.0) Gecko/20100101 Firefox/23.0
        Accept: text/html,application/xhtml+xml,application/xml;q=0.9,* /*;q=0.8
        Accept-Language: en-US,en;q=0.5
        Accept-Encoding: gzip, deflate
        Referer: http://localhost/post-form.php
        Connection: keep-alive
        Content-Type: application/x-www-form-urlencoded
        Content-Length: 55
//FIXME: Aqui va um salto de linha???
        Fullname=Albert&UserAddress=My+address&BtnSubmit=Submit

Response:

        HTTP/1.1 201 Created
        Date: �
        Content-Length: 0
        Location: http://example.com/foo/bar
 */
	
	char buffer[1000];
	time_t now = time(0);
	struct tm tm = *gmtime(&now);

	sprintf(buffer, "HTTP/1.1 201 Created\r\n");
	Writeline(connfd, buffer, strlen(buffer));

	strftime(buffer, sizeof buffer, "Date: %a, %d %b %Y %H:%M:%S %Z\r\n", &tm);
	Writeline(connfd, buffer, strlen(buffer));

	sprintf(buffer, "Server: Mapache/0.1\r\n");
	Writeline(connfd, buffer, strlen(buffer));

	sprintf(buffer, "Content-Length: 0\r\n");
	Writeline(connfd, buffer, strlen(buffer));

	sprintf(buffer, "Location: ...\r\n");
	Writeline(connfd, buffer, strlen(buffer));

	sprintf(buffer, "\r\n");
	Writeline(connfd, buffer, strlen(buffer));

	return 0;
}


int parsear_comando(int connfd, char * recvline, struct ReqInfo * reqinfo) {
	char *metodo;
	char *recurso;
	char *versaoHTTP;
	metodo = strtok(recvline," ");
	recurso = strtok (NULL, " ");
	versaoHTTP = strtok (NULL, " ");

	int tamanho_conteudo = 0;
	if(reqinfo->content)
		tamanho_conteudo = strlen(reqinfo->content);
	printf("Tamanho do conteudo: %d\n", tamanho_conteudo);

	if (!strcmp(metodo,"GET")){
		reqinfo->method = GET;
		reqinfo->httpVersion = versaoHTTP;
		reqinfo->resource = recurso;
		comando_get(connfd, recvline, reqinfo);
	}

	if (!strcmp(metodo,"OPTIONS")){
		reqinfo->method = OPTIONS;
		reqinfo->httpVersion = versaoHTTP;
		comando_options(connfd, recvline, reqinfo);
	}

	if (!strcmp(metodo,"POST")){
		reqinfo->method = POST;
		reqinfo->httpVersion = versaoHTTP;
		comando_post(connfd, recvline, reqinfo);
	}
}

int main (int argc, char **argv) {
	/* Os sockets. Um que ser� o socket que vai escutar pelas conex�es
	 * e o outro que vai ser o socket espec�fico de cada conex�o */
	int listenfd, connfd;
	/* Informa��es sobre o socket (endere�o e porta) ficam nesta struct */
	struct sockaddr_in servaddr;
	/* Retorno da fun��o fork para saber quem � o processo filho e quem
	 * � o processo pai */
	pid_t childpid;
	/* Armazena linhas recebidas do cliente */
	char	recvline[MAXLINE + 1];
	/* Armazena o tamanho da string lida do cliente */
	ssize_t  n;
	struct ReqInfo reqinfo;
	int primeiro_crlf;
	char buf[MAXLINE] = { 0 };
	char *content_length;
	char *recurso;
	char *versaoHTTP;
	int con_length;
   
	if (argc != 2) {
		fprintf(stderr,"Uso: %s <Porta>\n",argv[0]);
		fprintf(stderr,"Vai rodar um servidor de echo na porta <Porta> TCP\n");
		exit(1);
	}

	/* Cria��o de um socket. Eh como se fosse um descritor de arquivo. Eh
	 * possivel fazer operacoes como read, write e close. Neste
	 * caso o socket criado eh um socket IPv4 (por causa do AF_INET),
	 * que vai usar TCP (por causa do SOCK_STREAM), j� que o HTTP
	 * funciona sobre TCP, e ser� usado para uma aplica��o convencional sobre
	 * a Internet (por causa do n�mero 0) */
	if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("socket :(\n");
		exit(2);
	}

	/* Agora � necess�rio informar os endere�os associados a este
	 * socket. � necess�rio informar o endere�o / interface e a porta,
	 * pois mais adiante o socket ficar� esperando conex�es nesta porta
	 * e neste(s) endere�os. Para isso � necess�rio preencher a struct
	 * servaddr. � necess�rio colocar l� o tipo de socket (No nosso
	 * caso AF_INET porque � IPv4), em qual endere�o / interface ser�o
	 * esperadas conex�es (Neste caso em qualquer uma -- INADDR_ANY) e
	 * qual a porta. Neste caso ser� a porta que foi passada como
	 * argumento no shell (atoi(argv[1]))
	 */
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family      = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port        = htons(atoi(argv[1]));
	if (bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) == -1) {
		perror("bind :(\n");
		exit(3);
	}

	/* Como este c�digo � o c�digo de um servidor, o socket ser� um
	 * socket passivo. Para isto � necess�rio chamar a fun��o listen
	 * que define que este � um socket de servidor que ficar� esperando
	 * por conex�es nos endere�os definidos na fun��o bind. */
	if (listen(listenfd, LISTENQ) == -1) {
		perror("listen :(\n");
		exit(4);
	}

	printf("[Servidor no ar. Aguardando conexoes na porta %s]\n",argv[1]);
	printf("[Para finalizar, pressione CTRL+c ou rode um kill ou killall]\n");
   
	/* O servidor no final das contas � um loop infinito de espera por
	 * conex�es e processamento de cada uma individualmente */
	for (;;) {
		/* O socket inicial que foi criado � o socket que vai aguardar
		 * pela conex�o na porta especificada. Mas pode ser que existam
		 * diversos clientes conectando no servidor. Por isso deve-se
		 * utilizar a fun��o accept. Esta fun��o vai retirar uma conex�o
		 * da fila de conex�es que foram aceitas no socket listenfd e
		 * vai criar um socket espec�fico para esta conex�o. O descritor
		 * deste novo socket � o retorno da fun��o accept. */
		if ((connfd = accept(listenfd, (struct sockaddr *) NULL, NULL)) == -1 ) {
			perror("accept :(\n");
			exit(5);
		}
      
		/* Agora o servidor precisa tratar este cliente de forma
		 * separada. Para isto � criado um processo filho usando a
		 * fun��o fork. O processo vai ser uma c�pia deste. Depois da
		 * fun��o fork, os dois processos (pai e filho) estar�o no mesmo
		 * ponto do c�digo, mas cada um ter� um PID diferente. Assim �
		 * poss�vel diferenciar o que cada processo ter� que fazer. O
		 * filho tem que processar a requisi��o do cliente. O pai tem
		 * que voltar no loop para continuar aceitando novas conex�es */
		/* Se o retorno da fun��o fork for zero, � porque est� no
		 * processo filho. */
		if ( (childpid = fork()) == 0) {
			/**** PROCESSO FILHO ****/
			printf("[Uma conexao aberta]\n");
			/* J� que est� no processo filho, n�o precisa mais do socket
			 * listenfd. S� o processo pai precisa deste socket. */
			close(listenfd);
		
			/* Agora pode ler do socket e escrever no socket. Isto tem
			 * que ser feito em sincronia com o cliente. N�o faz sentido
			 * ler sem ter o que ler. Ou seja, neste caso est� sendo
			 * considerado que o cliente vai enviar algo para o servidor.
			 * O servidor vai processar o que tiver sido enviado e vai
			 * enviar uma resposta para o cliente (Que precisar� estar
			 * esperando por esta resposta) 
			 */

			/* ========================================================= */
			/* ========================================================= */
			/*                         EP1 IN�CIO                        */
			/* ========================================================= */
			/* ========================================================= */
			/* TODO: � esta parte do c�digo que ter� que ser modificada
			 * para que este servidor consiga interpretar comandos HTTP */

			//Le cabecalho

			int content_length = 0;
			size_t idx_inicio_linha = 0;
			size_t buf_idx = 0;
			while(1){
				//le linha
				while(1){
					n = read(connfd, &buf[buf_idx], 1);
					if ((fputs(recvline, stdout)) == EOF) {
						perror("fputs :( \n");
						exit(6);
					}
					if (buf_idx >= 2 && '\n' == buf[buf_idx] &&
				    		'\r' == buf[buf_idx-1]) {
						buf_idx = buf_idx-1;
						buf[buf_idx] = '\0';
						break;
					}
					buf_idx++;
				}
				int tamanho_linha = strlen(&buf[idx_inicio_linha]);
				if(tamanho_linha == 0) //linha em branco (final dos headers)
					break;
				char linha[tamanho_linha];
				strcpy (linha,&buf[idx_inicio_linha]);
				//verificando se possui content-length...
				int content_length_str = "Content-Length:";
				if(tamanho_linha > strlen(content_length_str)){
					//colocando um final de string bem apos o ':' para usar o strcasecmp
					linha[strlen(content_length_str)] = '\0';  
					if (strcasecmp (content_length_str, linha) == 0) {
						content_length = atoi(&linha[strlen(content_length_str)+1]);
        					printf ("Content-length %d!!\n",content_length);
    					}
				}
				idx_inicio_linha = buf_idx;
			}


			//lendo conteudo (content) do request
			(&reqinfo)->content = NULL;
			if(content_length > 0){
				(&reqinfo)->content = calloc(content_length+1,sizeof(char));
				read(connfd, (&reqinfo)->content, content_length);
				(&reqinfo)->content[content_length] = '\0'; //final da string!
			}
			parsear_comando(connfd, buf, &reqinfo);

//    			free(buf);
			/*
			while ((buf_idx < MAXLINE) && (n = read(connfd, &buf[buf_idx], 1)) > 0) {
				if ((fputs(recvline, stdout)) == EOF) {
					perror("fputs :( \n");
					exit(6);
				}
				mynonprint(buf, buf_idx);
				if (buf_idx > 330) {
					sscanf (buf, "Content-Length: %d\r\n", &con_length);
					printf("\n\nCONTENT-LENGTH: '%d' '%s' '%s'\n\n", con_length, recurso, versaoHTTP);
					
					printf("\n\nCONTENT-LENGTH: '%s' '%s' '%s'\n\n", content_length, recurso, versaoHTTP);
				}
				buf_idx++;
				if (buf_idx > 4            && 
				    '\n' == buf[buf_idx-1] &&
				    '\r' == buf[buf_idx-2] && 
				    '\n' == buf[buf_idx-3] &&
				    '\r' == buf[buf_idx-4]) {
					break;
				}
			}
			
			parsear_comando(connfd, buf, &reqinfo);
			*/


			/* ========================================================= */
			/* ========================================================= */
			/*                         EP1 FIM                           */
			/* ========================================================= */
			/* ========================================================= */

			/* Ap�s ter feito toda a troca de informa��o com o cliente,
			 * pode finalizar o processo filho */
			printf("[Uma conexao fechada]\n");
			exit(0);
		}
		/**** PROCESSO PAI ****/
		/* Se for o pai, a �nica coisa a ser feita � fechar o socket
		 * connfd (ele � o socket do cliente espec�fico que ser� tratado
		 * pelo processo filho) */
		close(connfd);
	}
	exit(0);
}
