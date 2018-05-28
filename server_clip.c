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



 void initialize_clip(_clip_element clip[]){
	int i= 0;
	for(i=0; i<10; i++){
		clip[i].length = 0;
		clip[i].dados = NULL;
	}
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
		printf("consegui ligar ao clipboard remote\n");

	return sock_fd;
}


void receive_remote_data(int sock_fd, _clip_element _clipboard[]){

	int i;
	char dados[10];
	int size;
	int nbytes;

	for(i=0;i<10; i++){
		nbytes = recv(sock_fd, dados, 10, 0);
		if(nbytes == -1){
			perror("recv");
			exit(-1);
		}
		size=atoi(dados);
		printf("posição--> %d tamanho %d\n", i, size);
		
		if(size==0)
			continue;
			
		_clipboard[i].length=size;
		_clipboard[i].dados=malloc(size);
		nbytes = recv(sock_fd, _clipboard[i].dados, _clipboard[i].length, 0);
		if(nbytes == -1){
			perror("recv");
			exit(-1);
		}
		printf("posição--> %d\n", i);
	}

}

int socket_create_clip(){
	
		srand(time(NULL));
		int port = 3005;
		port=port+rand()%20;
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
		//inet_aton("127.0.0.1", &local_addr.sin_addr); //ISTO E PARA MUDAR 127.0.0.1
		local_addr.sin_addr.s_addr = INADDR_ANY;
		//Assignment of a address to a communication point (slides)
		int err = bind(sock_fd, (struct sockaddr *)&local_addr, sizeof(local_addr));
		if(err == -1) {
			perror("bind\n");
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
	char aux[10];
	int nbytes;
	int i;
	int *fd_clip=sock_fd_clip;
	int sock_fd= *fd_clip;
	struct sockaddr_in client_addr;
	socklen_t size_addr;
	_sons_list *new_element;

	

	printf("WAIT_CLIPBOARDS: Estou à espera de clips\n\n");	
	while(1){
		int client_fd = accept(sock_fd, (struct sockaddr *) & client_addr, &size_addr);
		printf("cliente_fd ----> %d\n", client_fd);
		if(handler.father_clip_fd!=-1)
			pthread_cancel(thread_id_my_clip);

		printf("WAIT_CLIPBOARDS: Connected to a remote clipboard\n");
		new_element = (_sons_list *) malloc(sizeof(_sons_list));
		if(new_element == NULL)
		{
			perror("malloc");
			exit(-1);
		}
		new_element->next = NULL;
		new_element->sons_fd = client_fd;

		//DÚVIDA: É PRECISO FAZER MALLOC D HEAD E TAIL??????
		if(handler.clips_connected == 0){
			head = new_element;
			tail = new_element;
		}else{
			tail->next = new_element;
			tail = new_element;
		}

		handler.clips_connected++;
		for(i=0;i<10;i++){
			sprintf(aux, "%d",  _clipboard[i].length );
			nbytes = send(client_fd, aux, 10,0);
			if(nbytes == -1){
				printf("send length\n");
				perror("send");
				exit(-1);
			}
			if(_clipboard[i].length==0){				
				continue;
			}
			nbytes = send(client_fd, _clipboard[i].dados , _clipboard[i].length, 0);
			if(nbytes == -1){
				perror("send");
				exit(-1);
			}
		}
		
		pthread_create(&thread_ids_inform_root[handler.clips_connected-1], NULL, inform_root, &client_fd);
		printf("Criei a thread inform root\n");
		pthread_create(&thread_ids_update_sons[handler.clips_connected-1], NULL, update_sons_clip, &client_fd);
	}
	
	//enviar agora a informação do clipboard
}



void *inform_root(void *son_fd){
	int *son_fd_ptr = son_fd;
	int fd = *son_fd_ptr;
	void *buf;
	char buffer[10];
	int reg, size;
	int nbytes;
	while(1){ 
		//Lê tamanho e região do filho
		if(handler.father_clip_fd > 0){
			nbytes = read(fd, buffer, 10);
			if(nbytes<0){
				perror("read--inform_root");
				exit(-1);
			}
			printf("inform root, recebi do meu filho --> buffer :%s \n",buffer );
			sscanf(buffer, "%d-%d", &reg, &size);
			//Lê a mensagem
			printf("Regiao %d, tamanho %d\n",reg, size);
			buf = (void *) malloc (size);
			nbytes = read(fd, buf, size);
			printf("inform_root, recebi do meu filho---> mensagem %s\n",(char *)buf );
			if(nbytes<0){
				perror("read--inform_root");
				exit(-1);
			}
		}

		if(handler.father_clip_fd > 0){//Não é root (!= -1)
			//Escreve tamanho e região 
			nbytes = write(handler.father_clip_fd, buffer, 10);
			if(nbytes<0){
				perror("read--inform_root");
				exit(-1);
			}
			nbytes = write(handler.father_clip_fd, buf, size);
			if(nbytes<0){
				perror("read--inform_root");
				exit(-1);
			}
			free(buf);
		}
		else{
			flag_root = 1; 
		}
	}
}



void * update_sons_clip(void * son_fd){

	int *son_fd_ptr = son_fd;
	int fd = *son_fd_ptr;
	char buffer[10];
	int nbytes;
	int reg, size;
	void *buf;
	_sons_list *current= head;
	
	while(1){

	//retirei flag_root do if
	if(handler.father_clip_fd == -1 ){ //root
		//Lê tamanho e região
		printf("Sou o pai\n");
		nbytes = read(fd, buffer, 10);
		if(nbytes<0){
			perror("read--update_sons_clip");
			exit(-1);
		}
		sscanf(buffer, "%d-%d", &reg, &size);
		printf("update-sons-clip --->Regiao %d, tamanho %d\n",reg, size);
		//Lê a mensagem

		buf = (void *) malloc (size);
		nbytes = read(fd, buf, size);
		if(nbytes<0){
			perror("read--update_sons_clip");
			exit(-1);
		}
		printf("update-sons-clip, recebi do meu filho---> mensagem %s\n",(char *)buf );
		//Mandar para os filhos!!!!!!!!!!!!!! write para o sonfd
		
		/*
		nbytes = write(fd, buffer, 10);
		if(nbytes<0){
			perror("write--update_sons_clip");
			exit(-1);
		}
		nbytes = write(fd, buf, size);
		if(nbytes<0){
			perror("write--update_sons_clip");
			exit(-1);
		}*/
		
		//Faça upgrade dele!!!!! mexer c clipboard
		update_clipboard(reg, size, buf);
		
		while(current != NULL){
			printf("aquii\n\n");
			nbytes=write(current->sons_fd, buffer, 10);
			
			if(nbytes < 0)
			{
				perror("read");
				exit(-1);
			}
			nbytes=write(current->sons_fd, buf, size);
			printf("update sons clip mandei informação para um filho \n");
			if(nbytes < 0)
			{
				perror("read");
				exit(-1);
			}
			if(current->next != NULL)
				current = current->next;
			else
				break;
		}

		//Altera flag
		flag_root = 0;
		free(buf);
	}
	else{
		printf("estou a espera de ler do pai \n");
		nbytes = read(handler.father_clip_fd, buffer, 10);
		if(nbytes<0){
			perror("read--update_sons_clip");
			exit(-1);
		}
		sscanf(buffer, "%d-%d", &reg, &size);
		//Lê a mensagem
		buf = (void *) malloc(size);

		nbytes = read(handler.father_clip_fd, buf, size);
		if(nbytes<0){
			perror("read--update_sons_clip");
			exit(-1);
		}
		printf("update-sons-clip, li do meu pai e vou ,mandar para filho--->Regiao %d, tamanho %d\n",reg, size);
		/*
		//Mandar para os filhos!!!!!!!!!!!!!! write para o sonfd				
		nbytes = write(fd, buffer, 10);
		if(nbytes<0){
			perror("write--update_sons_clip");
			exit(-1);
		}

		nbytes = write(fd, buf, size);
		if(nbytes<0){
			perror("write--update_sons_clip");
			exit(-1);
		}*/

		//Faça upgrade dele!!!!! mexer c clipboard
		update_clipboard(reg, size, buf);
		
			while(current != NULL){
					printf("aquii\n\n");
					nbytes=write(current->sons_fd, buffer, 10);
					if(nbytes < 0)
					{
						perror("read");
						exit(-1);
					}
					nbytes=write(current->sons_fd, buf, size);
					printf("update sons clip mandei informação para um filho \n");
					if(nbytes < 0)
					{
						perror("read");
						exit(-1);
					}
					if(current->next != NULL)
						current = current->next;
					else
						break;
				}
		
		free(buf);
			
	}
	}
}



void *update_my_clip(void *a){

	char buffer[10];
	int nbytes;
	int size, reg;
	void *buf;
	while(1){//cuidado com a espera ativa -> queremos tb verificar se flag_sons varia -> wait_cond
		
		printf("Update my clipboard--> Regiao %d , tamanho %d \n", reg, size);
		
		if(handler.clips_connected >0)//recebeu 1 filho
		{	
			printf("saí da thread, recebi um clipboard\n");
			pthread_exit(NULL);
			
		}
		//cuidado porque ele verifica que nao recebeu e fica preso no read, nao esta sempre a verificar se recebeu filho e poe dar problema
		printf("father_clip_fd--> %d\n",handler.father_clip_fd );

		nbytes = read(handler.father_clip_fd, buffer, 10);
		if(nbytes<0){
			perror("read--update_my_clip");
			exit(-1);
		}
		sscanf(buffer, "%d-%d", &reg, &size);
		printf("Update clipboard--> Regiao %d , tamanho %d \n", reg, size);


		//Lê a mensagem
		buf = (void *) malloc (size);
		nbytes = read(handler.father_clip_fd, buf, size);
		if(nbytes<0){
			perror("read--update_my_clip");
			exit(-1);
		}

		update_clipboard(reg, size, buf);

		free(buf);
	}
}



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
		
		client_fd = accept(fd, (struct sockaddr *) & client_addr, &size_addr);
			printf("aceitei uma nova appp\n");
		if(client_fd == -1) {
			perror("accept");
			exit(-1);
		}
		
		num_apps++;	
		
	}
	
	
}


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
	size_t size;
	int a = 0;
	int i=0;
	_sons_list *current= head;

	//Lê a informação recebida da app sobre o tipo de ação a fazer
	read(fd, flag, 10);
	printf("\nDATA_PROCESSING: Através do socket da app - Recebi uma flag ----> %s\n", flag);
	
	sscanf(flag, "%*[^0-9]%d-%zu", &region, &size);
	
	
	printf("DATA_PROCESSING: pid: %d - threadID: %lu\n", getpid(), pthread_self());
	printf("DATA_PROCESSING: Regiao recebida pela app--->%d\n",region);
	printf("DATA_PROCESSING: Tamanho dos dados recebido pela app--->%zu\n",size);

	printf("Aqui vai o que eu penso que tenho de fazer: %c\n", flag[0]);

	if(flag[0]=='c') //copy to clipboard
	{
		printf("Data Processing - Entrei no copy\n");
	/*	if (_clipboard[region].dados != NULL)
			free(_clipboard[region].dados);
		
		_clipboard[region].length=size;*/
		
		//printf("tamanho do dados %d\n", _clipboard[msg.region].lenght);
		
		/*_clipboard[region].dados= (void *) malloc(size);
		printf("DATA_PROCESSING: Malloc do element_clip\n");*/
		buf = (void*) malloc(size);
		nbytes=read(fd, buf, size);
		printf("DATA_PROCESSING: A seguir ao read - nbytes = %d\n", nbytes);
		if(nbytes < 0)
		{
			perror("read");
			exit(-1);
		}
		else if(nbytes != size)
			printf("DATA_PROCESSING: -> copy - Erro a ler a infromação do socket (nbytes read != size(dados))\n");

		sprintf(flag, "%d-%zu", region, size );
		if(handler.father_clip_fd >0){	//tem pai
			
			nbytes=write(handler.father_clip_fd, flag, 10);
			if(nbytes < 0)
			{
				perror("read");
				exit(-1);
			}
			nbytes=write(handler.father_clip_fd, buf, size);
			printf("Data processing mandei informação para o pai\n");
			if(nbytes < 0)
			{
				perror("read");
				exit(-1);
			}
		} 
		else if(handler.father_clip_fd == -1)
		{		//atualizaçao acontecer no root -> aviso que foi a root que recebeu a atualizacao na flag
				flag_root =1;
				update_clipboard(region, size, buf);
				while(current != NULL){
					printf("aquii\n\n");
					nbytes=write(current->sons_fd, flag, 10);
					if(nbytes < 0)
					{
						perror("read");
						exit(-1);
					}
					nbytes=write(current->sons_fd, buf, size);
					printf("Data processing mandei informação para os meus filhos \n");
					if(nbytes < 0)
					{
						perror("read");
						exit(-1);
					}
					if(current->next != NULL)
						current = current->next;
					else
						break;
				}


		}
			
		//apagar no fim
		str = (char*)malloc(size);
		strcpy(str, (char*)buf);
		printf("Copiei para o clip, para a região %d, os seguintes dados: %s\n", region, str);
		
		free(buf);

		
	}

	else if(flag[0]=='p') //paste from clipboard
	{
		
		printf("DATA PROCESSING: Entrei no paste\n");
	
		
		printf("tamanho dos dados ---->%d\n", _clipboard[region].length);
		
		
		
		//Synchronization with read_write_locks.
		//pthread_rwlock_rdlock(&lock_rw);
		
		size=_clipboard[region].length;		
		buf = (void*)malloc(size);
		memcpy(buf, _clipboard[region].dados, size);
		
		//pthread_rwlock_unlock(&lock_rw);


		
		memset(flag, 0, 10);
		
		
		sprintf(flag, "%zu",  size);
		
		write(fd, flag, 10);
				
		write(fd, buf, size );
		
		free(buf);
				
	}
	
	else if(flag[0]=='w') //wait for changes on clipboard
	{	
		cond_var = 1;
		region_wait_change=region;
		
		pthread_mutex_lock(&lock_wait[region]);

		pthread_cond_wait(&cond, &lock_wait[region]);
		

		size = _clipboard[region].length;
		buf = (void*)malloc(size);
		memcpy(buf, _clipboard[region].dados, size);

		sprintf(flag, "%zu",  size);
		write(fd, flag, 10);					
		write(fd, buf, size );
		free(buf);


		pthread_mutex_unlock(&lock_wait[region]);

		cond_var = 0;
		
	}
}

 

 void update_clipboard(int region, int size, void *buf){
		

 		printf("Update clipboard--> Regiao %d , tamanho %d , mensagem %s\n", region, size, (char *)buf );
/*
		if(cond_var == 1)
			pthread_cond_signal(&cond);
			
*/		
 		//pthread_rwlock_wrlock(&lock_rw);		


 		if (_clipboard[region].length!=0)
 			free(_clipboard[region].dados);		
 		_clipboard[region].length=size;
		printf("TAMANHO!!! %d\n", _clipboard[region].length);
 		_clipboard[region].dados=(void *)malloc(size);
 		
 		if(_clipboard[region].dados==NULL){
 			printf("Error malloc-update_clipboard\n");
 			exit(-1);
 		}
 		memcpy(_clipboard[region].dados, buf, size);
 		
 		//pthread_rwlock_unlock(&lock_rw);

 		if(region==region_wait_change)
 			pthread_cond_signal(&cond);

 }






