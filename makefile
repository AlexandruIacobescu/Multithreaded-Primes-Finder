.PHONY: build clean

ifeq ($(OS), Windows_NT)
CLEAR_CMD := cls
else
CLEAR_CMD := clear
endif

all:
	$(CLEAR_CMD)
	g++ src/main.cpp -o build/main -Wall -Wextra -pedantic-errors
	build/main.exe

build:
	g++ src/main.cpp -o build/main $(ARGS)

run:
	build/main.exe

clean:
	del /f build\*