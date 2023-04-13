CFLAGS= -g -Wall -std=c99

all: backup

backup: backup.c
	gcc backup.c -o backup $(CFLAGS)

run: backup
	./backup

clean: 
	rm -f backup coralina.odt coralina.txt planilha.ods  spirit.jpg
