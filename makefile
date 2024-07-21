.PHONY: build clean

build:
	clang++ src/main.cpp -o build/main -Wall -Wextra -pedantic $(ARGS) -std=c++20

run:
	build/main.exe $(ARGS)

clean:
	del /f build\*