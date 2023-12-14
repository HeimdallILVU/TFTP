CC := gcc
CFLAGS := -Wall -Wextra -I./include 
LDFLAGS := 

SRC := main.c $(wildcard src/*.c)   # Add your source files here
OBJ := $(SRC:.c=.o)

TARGET := tftp

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)
