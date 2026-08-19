CC = gcc
CFLAGS = -Wall -Wextra -Iinclude
LDFLAGS = -lreadline

TARGET = shellforge

OBJ = src/main.o \
      src/token.o \
      src/lexer.o \
      src/history.o \
      src/parser.o \
      src/expand.o

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET) $(LDFLAGS)

src/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f src/*.o $(TARGET)
