# Prime Finder

Prime Finder is a multi-threaded C++ application that finds all prime numbers within a given range. It supports various options for outputting results, sorting, and customizing the number of threads used. The application utilizes the argparse library for command-line argument parsing.

## Features

- Multi-threaded execution: Utilizes multiple threads to speed up the prime finding process.
- Customizable output: Outputs primes to a file or console in a specified column format.
- Sorting: Sorts the primes in ascending or descending order.
- Silent mode: Optionally suppresses thread completion messages.

## Usage

```
Usage: prime_finder [--help] [--version] [-file] [-threads VAR] [-sort VAR] [--hush] [-columns VAR] a b

Positional arguments:
  a              Start of the range (must be a positive integer)
  b              End of the range (must be a positive integer greater than a)

Optional arguments:
  -h, --help     shows help message and exits
  -v, --version  prints version information and exits
  -file          Output primes to FILE instead of the console
  -threads       Number of threads to use (default: 4) [nargs=0..1] [default: 4]
  -sort          Sort order of the primes: 'asc' for ascending (default), 'desc' for descending [nargs=0..1] [default: "asc"]
  --hush         Suppress the output of thread finishing status
  -columns       Number of columns for output format (default: 1) [nargs=0..1] [default: 1]
```

## Installation
### Prerequisitres
- `C++17`, or later
- `GNU/gcc` or `LLVM/clang` compilers installed.
- `GNU/make` (recommended)

1. Clone the repository:
```sh
git clone https://github.com/AlexandruIacobescu/Multithreaded-Primes-Finder.git
```
2. Change directory to the project root directory
```sh
cd Multithreaded-Primes-Finder
```
3. Compile the application:
- using make:
```
make build
```
- manual:
```
g++ -std=c++17 src/main.cpp -o build/main
```

## Example
```bash
build/main.exe 1 100 -file primes.txt -threads 8 -sort desc --hush -columns 5
```

This command finds all prime numbers between 1 and 100, outputs them to `primes.txt` in descending order using 8 threads, suppresses thread completion messages, and formats the output in 5 columns.

### Output
```
97      89      83      79      73
71      67      61      59      53
47      43      41      37      31
29      23      19      17      13
11      7       5       3       2
```

## Code Structure

- `main()`: The entry point of the application.
- `parse_arguments()`: Uses argparse to parse command-line arguments.
- `find_primes()`: Finds primes in a given range and records execution time.
- `is_prime()`: Checks if a number is prime.
- `print_primes()`: Prints the prime numbers in the specified column format.

## License
This project is licensed under the MIT License