CC = gcc
CFLAGS = -I. -lpthread
SRC = input/input.c main.c mongoose/mongoose.c
TARGET = temp_converter
TEMPLATES_DIR = templates

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) -o $@ $^ $(CFLAGS)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)

.PHONY: all run clean
