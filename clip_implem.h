#define SOCK_ADDRESS "./CLIPBOARD_SOCKET"

typedef struct clip_element{
	int length;
	void *dados; 
}_clip_element;


//Variáveis globais (a todas as threads)
int sock_fd_remote;
//char clipboard[10][100]; //Temos de inicializar as 10 strings a '\0'
int aux_fd;
int num_apps;
int client_fd;

_clip_element _clipboard[10];
pthread_mutex_t lock;

//Receives connection from apps
int socket_create_app();

int remote_connect(char ip[], int port);

//void receive_remote_data(int sock_fd, char clipboard [][]);

void *data_processing(void *client_fd);

void initialize_clip(_clip_element clip[]);

void *wait_app(void *sock_fd_app);

int socket_create_clip();

void *wait_clipboard(void * sock_fd_clip);