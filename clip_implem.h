#define SOCK_ADDRESS "./CLIPBOARD_SOCKET"

typedef struct clip_element{
	int length;
	void *dados; 
}_clip_element;

typedef struct update_clipboard{ 
	int *sons_clips_fd;
	int clips_connected;
	int mailling_clip_fd; //o filho que me enviou a atualização
	int father_clip_fd;
	int my_fd_in_father; //para o meu pai saber a quem nao enviar (fui eu que lhe enviei)
}_update_clipboard;

typedef struct sons_list{
	int sons_fd;
	struct sons_list *next;
}_sons_list;

//Variáveis globais (a todas as threads)

//char clipboard[10][100]; //Temos de inicializar as 10 strings a '\0'
int aux_fd;
int num_apps;
int client_fd;
int flag_root;
int cond_var;
int region_wait_change;

_update_clipboard handler; 
_clip_element _clipboard[10];

_sons_list *head;
_sons_list *tail;


pthread_t thread_id_my_clip;


pthread_mutex_t lock;
pthread_rwlock_t lock_rw;
pthread_cond_t cond;
pthread_t thread_ids_inform_root[50];
pthread_t thread_ids_update_sons[50];
pthread_mutex_t lock_wait[10];



//Receives connection from apps
int socket_create_app();

int remote_connect(char ip[], int port);

void receive_remote_data(int sock_fd, _clip_element _clipboard[]);

void *data_processing(void *client_fd);

void initialize_clip(_clip_element clip[]);

void *wait_app(void *sock_fd_app);

int socket_create_clip();

void *wait_clipboard(void * sock_fd_clip);



void *inform_root(void *son_fd);

void * update_sons_clip(void * son_fd);

void update_clipboard(int region, int size, void *buf);

void *update_my_clip(void *);
