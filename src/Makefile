CC = gcc
CFLAGS = -Wall -Wextra -I../ -I -Imongoose -Iinput
LDFLAGS = -lmongoose -lrt
SRC = main.c mongoose/mongoose.c input/input.c
TARGET = server

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f $(TARGET)
