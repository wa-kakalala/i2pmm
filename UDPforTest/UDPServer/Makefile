TARGET = UDPServer
CC = gcc
CFLAGS += -g -wall --std=c89
LFLGAS += -lm
SRC = $(wildcard *.c)
OBJ = $(SRC: %.c=%.o)

$(TARGET):$(OBJ)
	$(CC) $^ $(LFLAGS) -o $@
%.o:%.c
	$(CC) $(CFLAGS) -c $< -o $@
clean:
	$(RM) -r *.o $(TARGET)