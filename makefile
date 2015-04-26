# compiler
CC = gcc

#release options for CC
CFLAGS = -std=c99 -Wall

# MYSQL program compile option
MYSQL_COMPILE = $(shell mysql_config --cflags)

# MYSQL program link option
MYSQL_LINK = $(shell mysql_config --libs)

a.out: main.o manage_information.o manage_score.o helper.o statistic.o system_setting.o
	$(CC) -o $@ $^ $(MYSQL_LINK)
main.o: main.c project.h
	$(CC) -o $@ $(CFLAGS) -c $(MYSQL_COMPILE) main.c
manage_information.o: manage_information.c project.h
	$(CC) -o $@  $(CFLAGS) -c $(MYSQL_COMPILE) manage_information.c
manage_score.o: manage_score.c project.h
	$(CC) -o $@  $(CFLAGS) -c $(MYSQL_COMPILE) manage_score.c
helper.o: helper.c project.h
	$(CC) -o $@  $(CFLAGS) -c $(MYSQL_COMPILE) helper.c
statistic.o: statistic.c project.h
	$(CC) -o $@  $(CFLAGS) -c $(MYSQL_COMPILE) statistic.c
system_setting.o: system_setting.c project.h
	$(CC) -o $@  $(CFLAGS) -c $(MYSQL_COMPILE) system_setting.c
.PHONY: clean
clean:
	rm -f *.o
