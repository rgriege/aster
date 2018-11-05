CC = gcc
CCFLAGS_D = -Darray_size_t=u32
CCFLAGS_W = -Wall -Werror -Wshadow -Wno-missing-braces -Wno-unused-local-typedefs
CCFLAGS_I = -I. -I/usr/include/ -I/usr/local/include/SDL2/
CCFLAGS_DEBUG = -g -g3 -DDEBUG
CCFLAGS_RELEASE = -O2 -DNDEBUG
CCFLAGS = -std=gnu99 $(CCFLAGS_DEBUG) $(CCFLAGS_D) $(CCFLAGS_W) $(CCFLAGS_I)
LFLAGS = -lGL -lGLEW -lm -lSDL2 -ldl

aster: aster.c
	$(CC) $(CCFLAGS) -o aster aster.c $(LFLAGS)

index.html: aster.c template.html
	emcc -std=gnu99 -Wno-typedef-redefinition aster.c -O2 -I. -I$(INC)/SDL2/ -DFMATH_NO_SSE -DDMATH_NO_SSE -s WASM=1 --shell-file template.html -o aster.html -s USE_SDL=2
	mv aster.html index.html

.PHONY: clean
clean:
	rm -f aster log.txt
	rm -f index.html aster.html aster.wasm aster.js
