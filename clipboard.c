#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
//#include "clipboard.h"
#include <arpa/inet.h>
#include <pthread.h>
#include "clip_implem.h"

// gcc -o server_clip.o -c server_clip.c && gcc -pthread clipboard.c server_clip.o -o clip && ./clip

int main(int argc, char* argv[]){

	int nbytes;
	struct sockaddr_un local_addr;
	struct sockaddr_un client_addr;
	socklen_t size_addr = sizeof(client_addr);
	char buff[100];
	int len_buff;
	
	pthread_t thread_ids[100];

	int sock_fd;
	int err;
	client_fd = 0;
	num_apps = 0;


	pthread_t thread_wait_apps=0;
	pthread_t thread_id_clip=0;
	int sock_fd_clipboard;

	unlink(SOCK_ADDRESS);// app 
	
	initialize_clip(_clipboard);

	if (pthread_mutex_init(&lock, NULL) != 0)
	{
		printf("\n mutex init failed\n");
		exit(-1);
	}
	//Cria um socket para comunicar com outros clipboards
	sock_fd_clipboard = socket_create_clip();
	printf("Clipboard.c - Socket criado para comunicar com outros clipboards\n\n");
	//espera conexao de um clipboard
	pthread_create(&thread_id_clip, NULL, wait_clipboard, &sock_fd_clipboard);

	if(argc > 3){

		int port=atoi(argv[3]);	
		char ip[100];
		strcpy(ip, argv[2]);
		sock_fd_remote = remote_connect(ip, port);
		//recebe todas a informação do remote clipboard
		//receive_remote_data(sock_fd_remote, clipboard);
	
		//receive connect from apps -> temos de criar 1 thread que faça isto (ciclo while(1))
	
	}
	
	//Cria um socket para comunicar com apps
	int sock_fd_app = socket_create_app();
	//thread que espera de apps
	pthread_create(&thread_wait_apps, NULL, wait_app, &sock_fd_app);
	
	//create thhreads para as apps
	while(1) {
		//alterar para read_locks
		//pthread_mutex_lock(&lock);
		if(client_fd!=aux_fd)		//aux_fd <-> wait_apps -> client_fd alterado quando recebe uma conexão por isso aux_fd != client_fd e processa o que a app precisar.
			pthread_create(&(thread_ids[num_apps-1]), NULL, data_processing, &client_fd);
		// pthread_mutex_unlock(&lock);
	}
	
	close(sock_fd_remote);
	close(sock_fd_clipboard);
	exit(0);

}
