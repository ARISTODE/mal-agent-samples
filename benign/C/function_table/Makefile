CC = ../../../mingw64/mingw64/bin/gcc.exe
CFLAGS = -O2 -Wall -Wextra -s
LIBS = -lm
TARGET = binary.exe
SOURCE = source.c

$(TARGET): $(SOURCE)
	$(CC) $(CFLAGS) -o $(TARGET) $(SOURCE) $(LIBS)

clean:
	del $(TARGET) 2>nul || true

.PHONY: clean