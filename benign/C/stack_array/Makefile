CC = ../../../mingw64/mingw64/bin/gcc.exe
CFLAGS = -O2 -Wall -Wextra -s
TARGET = binary.exe
SOURCE = source.c

$(TARGET): $(SOURCE)
	$(CC) $(CFLAGS) -o $(TARGET) $(SOURCE)

clean:
	del $(TARGET) 2>nul || true

.PHONY: clean