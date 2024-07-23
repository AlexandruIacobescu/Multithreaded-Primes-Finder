#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstring>
#include <fstream>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

#include "../include/argparse.hpp"

std::vector<int> primes;
std::mutex primes_mutex;
std::vector<std::pair<std::thread::id, double>>
    thread_times;  // To store thread id and the time it took
std::mutex times_mutex;
bool sort_ascending = true;  // Default sort order
bool hush = false;           // Suppress thread finishing status
int columns = 1;             // Number of columns for output

bool is_prime(int n) {
    if (n <= 1) return false;
    if (n <= 3) return true;
    if (n % 2 == 0 || n % 3 == 0) return false;
    for (int i = 5; i * i <= n; i += 6) {
        if (n % i == 0 || n % (i + 2) == 0) return false;
    }
    return true;
}

void find_primes(int start, int end) {
    auto start_time = std::chrono::high_resolution_clock::now();
    std::vector<int> local_primes;

    for (int i = start; i <= end; ++i) {
        if (is_prime(i)) {
            local_primes.push_back(i);
        }
    }

    {
        std::lock_guard<std::mutex> lock(primes_mutex);
        primes.insert(primes.end(), local_primes.begin(), local_primes.end());
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed = end_time - start_time;

    {
        std::lock_guard<std::mutex> lock(times_mutex);
        thread_times.emplace_back(std::this_thread::get_id(), elapsed.count());
    }
}

void parse_arguments(int argc, char *argv[], int &a, int &b, std::string &filename, int &threads,
                     bool &output_to_file, bool &sort_ascending, bool &hush, int &columns) {
    argparse::ArgumentParser program("prime_finder");

    program.add_argument("a")
        .help("Start of the range (must be a positive integer)")
        .scan<'i', int>();
    program.add_argument("b")
        .help("End of the range (must be a positive integer greater than a)")
        .scan<'i', int>();

    program.add_argument("-file")
        .help("Output primes to FILE instead of the console")
        .default_value(std::string(""))
        .implicit_value(std::string(""));

    program.add_argument("-threads")
        .help("Number of threads to use (default: 4)")
        .default_value(4)
        .scan<'i', int>();

    program.add_argument("-sort")
        .help("Sort order of the primes: 'asc' for ascending (default), 'desc' for descending")
        .default_value(std::string("asc"))
        .action([](const std::string &value) {
            static const std::vector<std::string> choices = {"asc", "desc"};
            if (std::find(choices.begin(), choices.end(), value) != choices.end()) {
                return value;
            }
            return std::string("asc");
        });

    program.add_argument("--hush")
        .help("Suppress the output of thread finishing status")
        .default_value(false)
        .implicit_value(true);

    program.add_argument("-columns")
        .help("Number of columns for output format (default: 1)")
        .default_value(1)
        .scan<'i', int>();

    try {
        program.parse_args(argc, argv);
    } catch (const std::runtime_error &err) {
        std::cerr << err.what() << std::endl;
        std::cerr << program;
        exit(1);
    }

    a = program.get<int>("a");
    b = program.get<int>("b");
    filename = program.get<std::string>("-file");
    threads = program.get<int>("-threads");
    sort_ascending = program.get<std::string>("-sort") == "asc";
    hush = program.get<bool>("--hush");
    columns = program.get<int>("-columns");

    output_to_file = !filename.empty();
}

void print_primes(const std::vector<int> &primes, int columns) {
    int count = 0;
    for (const auto &prime : primes) {
        std::cout << prime << "\t";
        if (++count % columns == 0) {
            std::cout << std::endl;
        }
    }
    if (count % columns != 0) {
        std::cout << std::endl;
    }
}

int main(int argc, char *argv[]) {
    // if (argc < 3) {
    //     std::cerr
    //         << "Usage: prime_finder a b [options]\n"
    //         << "Finds all prime numbers in the range [a, b].\n\n"
    //         << "Positional arguments:\n"
    //         << "  a               Start of the range (must be a positive integer)\n"
    //         << "  b               End of the range (must be a positive integer greater than a)\n\n"
    //         << "Optional arguments:\n"
    //         << "  -file FILE      Output primes to FILE instead of the console\n"
    //         << "  -threads T      Number of threads to use (default: 4)\n"
    //         << "  -sort ORDER     Sort order of the primes: 'asc' for ascending (default), 'desc' "
    //            "for descending\n"
    //         << "  --hush          Suppress the output of thread finishing status\n"
    //         << "  -columns NUMBER Number of columns for output format (default: 1)\n";
    //     return 1;
    // }

    int a, b, threads;
    std::string filename;
    bool output_to_file, sort_ascending;
    parse_arguments(argc, argv, a, b, filename, threads, output_to_file, sort_ascending, hush,
                    columns);

    if (a >= b || a < 1 || b < 1) {
        std::cerr << "Invalid range. Ensure that a < b and both are positive integers.\n";
        return 1;
    }

    std::vector<std::thread> thread_pool;
    int range = (b - a + 1);
    int chunk_size = range / threads;
    int start = a;

    for (int i = 0; i < threads; ++i) {
        int end = (i == threads - 1) ? b : start + chunk_size - 1;
        thread_pool.emplace_back(find_primes, start, end);
        start += chunk_size;
    }

    for (auto &t : thread_pool) {
        t.join();
    }

    if (!sort_ascending) {
        std::sort(primes.begin(), primes.end(), std::greater<int>());
    } else {
        std::sort(primes.begin(), primes.end());
    }

    if (output_to_file) {
        std::ofstream outfile(filename);
        int count = 0;
        for (const auto &prime : primes) {
            outfile << prime << "\t";
            if (++count % columns == 0) {
                outfile << "\n";
            }
        }
        if (count % columns != 0) {
            outfile << "\n";
        }
        outfile.close();
    } else {
        print_primes(primes, columns);
    }

    if (!hush) {
        for (const auto &time_record : thread_times) {
            std::cout << "Thread " << time_record.first << " finished in " << time_record.second
                      << " ms\n";
        }
    }

    return 0;
}