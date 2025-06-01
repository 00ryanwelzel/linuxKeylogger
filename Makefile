CC = gcc
CFLAGS = -Wall -Wextra -g -O2
LIBS = -ludev

TARGET = keylogger
SRC = keylogger.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC) $(LIBS)

clean:
	rm -f $(TARGET) keylogger_output.txt
