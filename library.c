/*
Falar sobre este .c
-> API do programa
*/
#include "clipboard.h"
#include "clip_implem.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

/*
Function: clipboard_connect
Description:
*/
int clipboard_connect(char * clipboard_dir){

		struct sockaddr_un server_addr;
		struct sockaddr_un client_addr;
		//Cria socket
		int sock_fd = socket(AF_UNIX, SOCK_STREAM, 0);

		if (sock_fd == -1){
			perror("socket: ");
			exit(-1);
		}


		printf("CLIPBOARD_CONNECT: Socket created \n");

		client_addr.sun_family = AF_UNIX;
		sprintf(client_addr.sun_path, "%ssocket_%d",clipboard_dir, getpid());

		server_addr.sun_family = AF_UNIX;
		strcpy(server_addr.sun_path, SOCK_ADDRESS);


		int err_c = connect(sock_fd, (const struct sockaddr *) &server_addr, sizeof(server_addr));
		if(err_c==-1){
					printf("Error connecting\n");
					exit(-1);
		}
		printf("CLIPBOARD_CONNECT: Connected com valor err:%d\n\n", err_c);

		return sock_fd;
}


/*
Function: clipboard_copy
Description:
*/
int clipboard_copy(int clipboard_id, int region, void *buf, size_t count){

	int nbytes;
	
	
	/*
	msg.action = 0;
	msg.region=region;
	printf("tamanho----%d\n",count);
	msg.clip.lenght=count;
	msg.clip.dados=malloc(count);
	memcpy(msg.clip.dados, buf, count);
	*/
	
	//strcpy(msg.data,buf);
	
	char flag[10];

	///formato da flag: "actionregiao-tamanho"
	sprintf(flag,"c%d-%d", region, count);
	printf("a flag tem a seguinte forma ----> %s\n", flag);
	nbytes = write(clipboard_id, flag, 10);	
	if(nbytes == 0)
	{
		perror("write");
		exit(-1);
	}
	nbytes = write(clipboard_id, buf, count);
	
	return nbytes;
}


/*
Function: clipboard_paste
Description:
*/
int clipboard_paste(int clipboard_id, int region, void *buf, size_t count){

	char flag[10];
	char dados[10];
	int size_message;
	int nbytes;
	///formato da flag: "actionregiao-tamanho"
	sprintf(flag,"p%d-%d", region, count);
	printf("a flag tem a seguinte forma ----> %s\n", flag);
	
	
	write(clipboard_id, flag, 10);
	
	
	read(clipboard_id, dados, 10);
	
	size_message=atoi(dados);
	printf("tamanho da mensagem--->%d\n", size_message);
	
	if(count < size_message)
		printf("mensagem vai ser cortada\n");
		
	nbytes = read(clipboard_id, buf, size_message);
	if(nbytes < 0)
	{
		perror("read");
		exit(-1);
	}
	printf("nbytes lidos -->%d\n", nbytes);
	
	printf("CLIPBOARD_PASTE: mensagem recebida -> %s", buf);
	return nbytes;
}


/*
Function: clipboard_wait
Description:
*/
int clipboard_wait(int clipboard_id, int region, void *buf, size_t count){

	int nbytes;
	char dados[10];
	int size_message;
	
	sprintf(dados,"w%d-%d", region, count);
	printf("a flag tem a seguinte forma ----> %s\n", dados);

	write(clipboard_id, dados, 10);

	read(clipboard_id, dados, 10);

	size_message=atoi(dados);
	printf("tamanho da mensagem--->%d\n", size_message);
	
	if(count < size_message)
		printf("mensagem vai ser cortada\n");
		
	nbytes = read(clipboard_id, buf, size_message);						//ESTRANHO, TA A SER IGUAL A FUNCAO PASTE
	if(nbytes < 0)
	{
		perror("read");
		exit(-1);
	}
	printf("nbytes lidos -->%d\n", nbytes);
	
	printf("CLIPBOARD_WAIT: mensagem recebida -> %s", buf);

	return nbytes;
}

/*
Function: clipboard_close
Description:
*/
void clipboard_close(int clipboard_id){
	//É preciso fazer unlink?
	close(clipboard_id); 
}
