CC = gcc
OBJ = Mobile.o LoadShader.o Matrix.o
CFLAGS = -g -Wall -Wextra

LDLIBS=-lm -lglut -lGLEW -lGL

Mobile: $(OBJ)
	 $(CC) -o $@ $^ $(CFLAGS) $(LDLIBS)

clean:
	rm -f *.o Mobile 
.PHONY: all clean
