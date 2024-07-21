#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstring>
#include <fstream>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

std::vector<int> primes;
std::mutex primes_mutex;
std::vector<std::pair<std::thread::id, double>>
    thread_times;  // To store thread id and the time it took
std::mutex times_mutex;
bool sort_ascending = true;  // Default sort order

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

void parse_arguments(int argc, char *argv[], int &a, int &b,
                     std::string &filename, int &threads, bool &output_to_file,
                     bool &sort_ascending) {
    a = std::stoi(argv[1]);
    b = std::stoi(argv[2]);

    threads = 4;  // default value
    output_to_file = false;
    sort_ascending = true;  // default sort order

    for (int i = 3; i < argc; ++i) {
        if (strcmp(argv[i], "-file") == 0) {
            output_to_file = true;
            filename = argv[++i];
        } else if (strcmp(argv[i], "-threads") == 0) {
            threads = std::stoi(argv[++i]);
        } else if (strcmp(argv[i], "-sort") == 0) {
            if (strcmp(argv[++i], "asc") == 0) {
                sort_ascending = true;
            } else if (strcmp(argv[i], "desc") == 0) {
                sort_ascending = false;
            }
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0]
                  << " a b -file file.txt -threads t -sort asc|desc\n";
        return 1;
    }

    int a, b, threads;
    std::string filename;
    bool output_to_file, sort_ascending;
    parse_arguments(argc, argv, a, b, filename, threads, output_to_file,
                    sort_ascending);

    if (a >= b || a < 1 || b < 1) {
        std::cerr << "Invalid range. Ensure that a < b and both are positive "
                     "integers.\n";
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
        for (const auto &prime : primes) {
            outfile << prime << "\n";
        }
        outfile.close();
    } else {
        for (const auto &prime : primes) {
            std::cout << prime << " ";
        }
        std::cout << std::endl;
    }

    for (const auto &time_record : thread_times) {
        std::cout << "Thread " << time_record.first << " finished in "
                  << time_record.second << " ms\n";
    }

    return 0;
}