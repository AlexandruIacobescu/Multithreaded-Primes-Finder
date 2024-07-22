.PHONY: build clean

build:
	g++ src/main.cpp -o build/main -Wall -Wextra -pedantic $(ARGS) -std=c++17

run:
	build/main.exe $(ARGS)

clean:
	del /f build\*