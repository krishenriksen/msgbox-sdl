CC = gcc
CCFLAGS = -W -Wall

BINARY = msgbox
LIBRARIES = -lSDL2 -lSDL2_ttf -lGLESv1_CM
SOURCES = "main.c"

all:
	$(CC) $(CCFLAGS) $(INCLUDES) $(SOURCES) -o $(BINARY) $(LIBRARIES)

clean:
	rm -f $(BINARY)