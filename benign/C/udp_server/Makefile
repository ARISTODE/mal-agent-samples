CC=../../../mingw64/mingw64/bin/gcc.exe
CFLAGS=-O2 -Wall -Wextra
TARGET=binary.exe
SOURCE=source.c

all: $(TARGET)

$(TARGET): $(SOURCE)
	$(CC) $(CFLAGS) -s -o $(TARGET) $(SOURCE) -lws2_32

clean:
	rm -f $(TARGET)

.PHONY: all clean