#include "clipboard.h"
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/un.h> 
#include <unistd.h>
#include <string.h>


int main(){
		
		
		char dados[100];
		void *buf;
		char *msg;
		int sock_fd = clipboard_connect("./");
		printf("APP - Saí do connect\n");
		int copy_flag=0, flag=0, paste_flag=0, wait_flag = 0, reg;
		
		int nbytes=0;
	
		///tamanho do buff que podemos copiar do clipboard 
		///Acho que podemos escolher o tamanho que quisermos
		int size_buf=100;
		size_t size;
		size_t tamanho = 10;
		//Recebe o que o cliente pretende fazer - copy ou paste 
		do{
			/*****************************************************************************
								RETIRAR SHOW CLIPBOARD PA ENTREGAR
			*****************************************************************************/
			printf("What do you want to do?\n --Copy to clipboard press 0 \n -- Paste from clipboard press 1\n -- Wait for changes of clipboard on a region press 2\n ");
			fgets(dados,10, stdin);
			flag = atoi(dados);
            
			if(flag==0){
				
				copy_flag=1;
			}
			else if(flag==1){
				
				paste_flag=1;				//MUDAR ISTO PARA CASE + bonito e em baixo tb
			}else if(flag==2){
				wait_flag = 1;
			}

		}while(copy_flag==0  && paste_flag==0 && wait_flag==0);
		
		if(copy_flag == 1){
			
			printf("Region:");
			fgets(dados,10, stdin);
			reg = atoi(dados);
			/*
			///não deixar ultrapassar o limite máximo
			printf("Tamanho máximo do conteúdo:\n");
			fgets(dados,10, stdin);
			size = atoi(dados);
			
			buf= (void *) malloc((size+1));
			*/
			printf("Data:\n");
			//fgets(buf, size+1, stdin);
			msg = (char*) malloc(sizeof(char)*tamanho);
			size=getline(&msg, &tamanho , stdin);	
	
			buf = (void*) malloc(size);
	
			memcpy(buf, msg, size);
			printf("%zu ---> %s\n", size, buf);
			//memcpy(buf, msg, size);
		
			printf("ANTES COPY\n");
		
			//IMPOTANTE: size é do tipo int e acho que tem de ser size_t !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
			nbytes=clipboard_copy(sock_fd, reg, buf, size);
			
			if(nbytes == 0){
				printf("Erro clipboard_copy\n");
				exit(-1);

			}
			free(msg);
			free(buf);
			
			//clipboard_copy();
		}
		else if (paste_flag == 1){
		
			
			printf("Region:");
			fgets(dados,10, stdin);
			reg = atoi(dados);
			//REGIAO SO PODE SER ate 9 ->tratar erros
			
			
			//no maximo 100
			buf=malloc(size_buf);
			
			
			nbytes = clipboard_paste(sock_fd, reg, buf, size_buf);
			
			
			
			printf("Dados pedidos: %s\n", buf);
			free(buf);
		}	
		else if (wait_flag == 1){
			printf("Region:");
			fgets(dados,10, stdin);
			reg = atoi(dados);
			//REGIAO SO PODE SER ate 9 ->tratar erros
			//no maximo 100

			buf=malloc(size_buf);

			nbytes = clipboard_wait(sock_fd, reg, buf, size_buf);

			printf("Dados pedidos: %s\n", (char *)buf);
			free(buf);	
		}
		
		
		exit(0);
	}
