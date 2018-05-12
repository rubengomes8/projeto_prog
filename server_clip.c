#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include "clipboard.h"
#include <arpa/inet.h>
#include <pthread.h>
#include "clip_implem.h"

//Receives connection from apps
int socket_create_app(){
	struct sockaddr_un local_addr;
	struct sockaddr_un client_addr;
	socklen_t size_addr = sizeof(client_addr);
	int sock_fd_app = socket(AF_UNIX, SOCK_STREAM, 0);
	if (sock_fd_app == -1){
		perror("socket: ");
		exit(-1);
	}

	local_addr.sun_family = AF_UNIX;
	strcpy(local_addr.sun_path, SOCK_ADDRESS);

	int err = bind(sock_fd_app, (struct sockaddr *)&local_addr, sizeof(local_addr));
	if(err == -1) {
		perror("bind");
		exit(-1);
	}
	printf("Socket created and binded - apps_connect\n");

	if(listen(sock_fd_app, 2) == -1) {
		perror("listen");
		exit(-1);
	}
	return sock_fd_app;
	
	
}

int remote_connect(char ip[], int port){

	struct sockaddr_in server_addr;
	int nbytes;

	int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (sock_fd == -1){
		perror("socket: ");
		exit(-1);
	}
	

	printf("Socket created - Clipboard.c\n");

	server_addr.sin_family = AF_INET;
	server_addr.sin_port= htons(port);

	inet_aton(ip, &server_addr.sin_addr);

	if( connect(sock_fd, (const struct sockaddr *) &server_addr,	sizeof(server_addr)) == -1)
	{
		printf("Error connecting\n");
		exit(-1);
	}
		printf("consegui ligar ao clipboard remote");

	return sock_fd;
}


/*void receive_remote_data(int sock_fd, _clip_element _clipboard){

	int i;
	for(i=0;i<10; i++){
		recv(sock_fd, _clipboard[i].dados, 100, 0);
	}

}*/

void *data_processing(void *client_fd){  
	// IMPORTANTE: TA A LER MAIS UMA VEZ QD FAZEMOS PASTE!!!!!

	int *client_fd_ptr = client_fd;
	int fd = *client_fd_ptr;
	
	void *buf;
	char* str;
	char flag[10];
	char message[100];
	
	int nbytes;
	int region;
	int size;
	int a = 0;

	//Lê a informação recebida da app sobre o tipo de ação a fazer
	read(fd, flag, 10);
	printf("\nDATA_PROCESSING: Através do socket da app - Recebi uma flag ----> %s\n", flag);
	
	sscanf(flag, "%*[^0-9]%d-%d", &region, &size);
	
	
	printf("DATA_PROCESSING: pid: %d - threadID: %lu\n", getpid(), pthread_self());
	printf("DATA_PROCESSING: Regiao recebida pela app--->%d\n",region);
	printf("DATA_PROCESSING: Tamanho dos dados recebido pela app--->%d\n",size);

	printf("Aqui vai o que eu penso que tenho de fazer: %c\n", flag[0]);

	if(flag[0]=='c') //copy to clipboard
	{
		printf("Data Processing - Entrei no copy\n");
		if (_clipboard[region].dados != NULL)
			free(_clipboard[region].dados);
		
		_clipboard[region].length=size;
		
		//printf("tamanho do dados %d\n", _clipboard[msg.region].lenght);
		
		_clipboard[region].dados= (void *) malloc(size);
		printf("DATA_PROCESSING: Malloc do element_clip\n");

		nbytes=read(fd, _clipboard[region].dados, size);
		printf("DATA_PROCESSING: A seguir ao read - nbytes = %d\n", nbytes);
		if(nbytes < 0)
		{
			perror("read");
			exit(-1);
		}
		else if(nbytes != size)
			printf("DATA_PROCESSING: -> copy - Erro a ler a infromação do socket (nbytes read != size(dados))\n");

		str = (char*)malloc(size);
		strcpy(str, (char*)_clipboard[region].dados);
		printf("Copiei para o clip, para a região %d, os seguintes dados: %s\n", region, _clipboard[region].dados);
		
		
	}

	if(flag[0]=='p') //paste from clipboard
	{
		
		printf("DATA PROCESSING: Entrei no paste\n");
	
		
		printf("tamanho dos dados ---->%d\n", _clipboard[region].length);
		sprintf(flag, "%d",  _clipboard[region].length );
		
		write(fd, flag, 10);
				
		write(fd, _clipboard[region].dados, _clipboard[region].length );
				
	}
	/*
	if(flag[0]=='w') //wait for changes on clipboard
	{
		printf("DATA PROCESSING: Entrei no wait\n");
		strcpy(message, (char*) _clipboard[region].dados);
		printf("DATA PROCESSING: mensagem antiga -> %s \n", message);

	
		
		printf("tamanho dos dados ---->%d\n", _clipboard[region].length);
		sprintf(flag, "%d",  _clipboard[region].length );
		
		write(fd, flag, 10);
		while(a == 0){ 
			if((strcmp(message, _clipboard[region].length)) != 0) //se forem diferentes
				a = 1;


		}
				
		write(fd, _clipboard[region].dados, _clipboard[region].length );
				
	}*/
}

 void initialize_clip(_clip_element clip[]){
	int i= 0;
	for(i=0; i<10; i++){
		clip[i].length = 0;
		clip[i].dados = NULL;
	}
}
 

//Thread que aceita a conexão de uma app e altera a variável client_fd , pelo que, no main será detetada a conexão de uma nova app
void *wait_app(void *sock_fd_app){
	
	int *aux=sock_fd_app;
	int fd=*aux;
	printf(".\n");
	
	struct sockaddr_un client_addr;
	socklen_t size_addr = sizeof(client_addr);
	while(1){
		aux_fd=client_fd;
		//alterar para read_locks
		pthread_mutex_lock(&lock);
		client_fd = accept(fd, (struct sockaddr *) & client_addr, &size_addr);
		pthread_mutex_unlock(&lock);
		if(client_fd == -1) {
			perror("accept");
			exit(-1);
		}
		
		num_apps++;	
		
	}
	
	
}

int socket_create_clip(){
	
		int port = 3002;
		struct sockaddr_in local_addr;
		struct sockaddr_in client_addr;
		socklen_t size_addr;

		//Cria socket
		int sock_fd = socket(AF_INET, SOCK_STREAM, 0);

		if (sock_fd == -1){
			perror("socket: ");
			exit(-1);
		}

		printf("SOCKET_CREATE_CLIP: Socket created with port %d \n", port);
		local_addr.sin_family = AF_INET;
		local_addr.sin_port= htons(port);
		inet_aton("127.0.0.1", &local_addr.sin_addr);

		//Assignment of a address to a communication point (slides)
		int err = bind(sock_fd, (struct sockaddr *)&local_addr, sizeof(local_addr));
		if(err == -1) {
			perror("bind");
			exit(-1);
	    }
		printf("SOCKET_CREATE_CLIP: Socket binded - backup\n");

		//Listen to incoming connections
		listen(sock_fd, 10);

		printf("SOCKET_CREATE_CLIP: sock_fd :%d\n", sock_fd);
	return sock_fd;
	
	
}

//MUDAR IMPLEMENTACAO DO CLIPBOARD
void *wait_clipboard(void * sock_fd_clip){
	
	int i;
	int *fd_clip=sock_fd_clip;
	int sock_fd= *fd_clip;
	struct sockaddr_in client_addr;
	socklen_t size_addr;
	printf("WAIT_CLIPBOARDS: Estou à espera de clips\n\n");	
	while(1){
		int client_fd = accept(sock_fd, (struct sockaddr *) & client_addr, &size_addr);
		printf("WAIT_CLIPBOARDS: Connected to a remote clipboard\n");
		
		for(i=0;i<10;i++){
			send(client_fd, _clipboard[i].dados , 100, 0);
		}
	};
	
	//enviar agora a informação do clipboard
}
