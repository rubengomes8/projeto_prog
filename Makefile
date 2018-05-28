make:
	gcc -o server_clip.o -c server_clip.c && gcc -pthread clipboard.c server_clip.o -o clip && ./clip
