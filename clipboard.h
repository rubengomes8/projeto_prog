
#include <sys/types.h>
#include <pthread.h>

//------------------------------------------------------------------------------------------------------------//
// 		O que está em cima é para retirar, o cliente nao deve saber que tipo de includes precisamos acho eu	  //
//------------------------------------------------------------------------------------------------------------//

int clipboard_connect(char * clipboard_dir);
int clipboard_copy(int clipboard_id, int region, void *buf, size_t count);
int clipboard_paste(int clipboard_id, int region, void *buf, size_t count);
int clipboard_wait(int clipboard_id, int region, void *buf, size_t count);

