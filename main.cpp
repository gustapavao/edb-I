#include <iostream>
#include <vector>
#include <algorithm>
#include <chrono>
#include <random>
#include <iomanip>
#include <fstream>

struct Metrics {
    long long comparisons;
    long long swaps;
    double time_ms;

    Metrics() : comparisons(0), swaps(0), time_ms(0.0) {}
};

void incrementComparisons(Metrics& metrics) {
    metrics.comparisons++;
}

void incrementSwaps(Metrics& metrics) {
    metrics.swaps++;
}


// Gera um vetor com dados aleatórios
std::vector<int> generateRandomData(int size) {
    std::vector<int> data(size);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(0, 2 * size);

    for (int i = 0; i < size; ++i) {
        data[i] = distrib(gen);
    }
    return data;
}

std::vector<int> generateNearlySortedData(int size, int disorder_percentage) {
    std::vector<int> data(size);
    for (int i = 0; i < size; ++i) {
        data[i] = i;
    }

    std::random_device rd;
    std::mt19937 gen(rd());
    int num_swaps = (size * disorder_percentage) / 100;
    std::uniform_int_distribution<> distrib(0, size - 1);

    for (int i = 0; i < num_swaps; ++i) {
        int idx1 = distrib(gen);
        int idx2 = distrib(gen);
        std::swap(data[idx1], data[idx2]);
    }
    return data;
}

std::vector<int> generateReverseSortedData(int size) {
    std::vector<int> data(size);
    // Preenche o vetor em ordem decrescente
    for (int i = 0; i < size; ++i) {
        data[i] = size - 1 - i;
    }
    return data;
}


// Bubble Sort
void bubbleSort(std::vector<int>& arr, Metrics& metrics) {
    int n = arr.size();
    bool swapped;
    for (int i = 0; i < n - 1; ++i) {
        swapped = false;
        for (int j = 0; j < n - i - 1; ++j) {
            incrementComparisons(metrics); // Contagem de comparações
            if (arr[j] > arr[j + 1]) {
                std::swap(arr[j], arr[j + 1]);
                incrementSwaps(metrics); // Contagem de trocas
                swapped = true;
            }
        }
        if (!swapped) break;
    }
}

// Insertion Sort
void insertionSort(std::vector<int>& arr, Metrics& metrics) {
    int n = arr.size();
    for (int i = 1; i < n; ++i) {
        int key = arr[i];
        int j = i - 1;
        while (j >= 0) {
            incrementComparisons(metrics);
            if (arr[j] > key) {
                arr[j + 1] = arr[j];
                incrementSwaps(metrics);
                j = j - 1;
            } else {
                break;
            }
        }
        if (arr[j+1] != key) {
             arr[j + 1] = key;
        }
    }
}


// Selection Sort
void selectionSort(std::vector<int>& arr, Metrics& metrics) {
    int n = arr.size();
    for (int i = 0; i < n - 1; ++i) {
        int min_idx = i;
        for (int j = i + 1; j < n; ++j) {
            incrementComparisons(metrics);
            if (arr[j] < arr[min_idx]) {
                min_idx = j;
            }
        }
        if (min_idx != i) {
            std::swap(arr[i], arr[min_idx]);
            incrementSwaps(metrics);
        }
    }
}

void merge(std::vector<int>& arr, int left, int mid, int right, Metrics& metrics) {
    int n1 = mid - left + 1;
    int n2 = right - mid;

    std::vector<int> L(n1);
    std::vector<int> R(n2);

    for (int i = 0; i < n1; i++)
        L[i] = arr[left + i];
    for (int j = 0; j < n2; j++)
        R[j] = arr[mid + 1 + j];

    int i = 0;
    int j = 0;
    int k = left;

    while (i < n1 && j < n2) {
        incrementComparisons(metrics);
        if (L[i] <= R[j]) {
            arr[k] = L[i];
            i++;
        } else {
            arr[k] = R[j];
            j++;
            incrementSwaps(metrics);
        }
        k++;
    }

    while (i < n1) {
        arr[k] = L[i];
        i++;
        k++;
    }

    while (j < n2) {
        arr[k] = R[j];
        j++;
        k++;
    }
}

void mergeSort(std::vector<int>& arr, int left, int right, Metrics& metrics) {
    if (left >= right) {
        return;
    }
    int mid = left + (right - left) / 2;
    mergeSort(arr, left, mid, metrics);
    mergeSort(arr, mid + 1, right, metrics);
    merge(arr, left, mid, right, metrics);
}

// Quick Sort - Função de partição
int partition(std::vector<int>& arr, int low, int high, Metrics& metrics) {
    int pivot = arr[high];
    int i = (low - 1);

    for (int j = low; j <= high - 1; ++j) {
        incrementComparisons(metrics);
        if (arr[j] <= pivot) {
            i++;
            if (i != j) {
                std::swap(arr[i], arr[j]);
                incrementSwaps(metrics);
            }
        }
    }
    if (arr[i + 1] != arr[high]) {
        std::swap(arr[i + 1], arr[high]);
        incrementSwaps(metrics);
    }
    return (i + 1);
}

void quickSort(std::vector<int>& arr, int low, int high, Metrics& metrics) {
    if (low < high) {
        int pi = partition(arr, low, high, metrics);

        quickSort(arr, low, pi - 1, metrics);
        quickSort(arr, pi + 1, high, metrics);
    }
}

void runTest(const std::string& algorithm_name, std::vector<int> data,
             void (*sort_func)(std::vector<int>&, Metrics&),
             Metrics& metrics) {
    if (algorithm_name == "Merge Sort") {
        auto start_time = std::chrono::high_resolution_clock::now();
        mergeSort(data, 0, data.size() - 1, metrics);
        auto end_time = std::chrono::high_resolution_clock::now();
        metrics.time_ms = std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time).count() / 1e6;
    } else if (algorithm_name == "Quick Sort") {
        auto start_time = std::chrono::high_resolution_clock::now();
        quickSort(data, 0, data.size() - 1, metrics);
        auto end_time = std::chrono::high_resolution_clock::now();
        metrics.time_ms = std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time).count() / 1e6;
    } else {
        auto start_time = std::chrono::high_resolution_clock::now();
        sort_func(data, metrics);
        auto end_time = std::chrono::high_resolution_clock::now();
        metrics.time_ms = std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time).count() / 1e6;
    }
}


int main() {
    std::vector<int> sizes = {1000, 5000, 10000, 50000, 100000};
    int num_runs_per_test = 5;

    std::ofstream csv_file("sorting_results.csv");
    if (!csv_file.is_open()) {
        std::cerr << "Erro ao abrir o arquivo CSV!" << std::endl;
        return 1;
    }

    csv_file << "Algorithm,DataType,Size,Time_ms,Comparisons,Swaps\n";

    std::cout << std::fixed << std::setprecision(3); // Formata a saída de tempo

    std::cout << "--- Início dos Testes de Algoritmos de Ordenação ---" << std::endl;

    std::vector<std::pair<std::string, void (*)(std::vector<int>&, Metrics&)>> simple_algorithms = {
        {"Bubble Sort", bubbleSort},
        {"Insertion Sort", insertionSort},
        {"Selection Sort", selectionSort},
    };

    std::vector<std::string> data_types = {"Random", "Nearly Sorted", "Reverse Sorted"};

    for (int size : sizes) {
        std::cout << "\n--- Testando com Tamanho do Vetor: " << size << " ---" << std::endl;

        for (const std::string& data_type : data_types) {
            std::cout << "\nTipo de Dados: " << data_type << std::endl;

            for (const auto& algo_pair : simple_algorithms) {
                Metrics avg_metrics;
                bool skipped = false;
                if (size > 10000 && (algo_pair.first == "Bubble Sort" || algo_pair.first == "Selection Sort")) {
                     std::cout << algo_pair.first << ": N/A (Tempo excessivo para este tamanho)" << std::endl;
                     skipped = true;
                } else if (size > 50000 && (algo_pair.first == "Insertion Sort")) { // Insertion Sort ainda vai bem em quase ordenado
                     std::cout << algo_pair.first << ": N/A (Tempo excessivo para este tamanho)" << std::endl;
                     skipped = true;
                }


                if (!skipped) {
                    for (int i = 0; i < num_runs_per_test; ++i) {
                        Metrics current_metrics;
                        std::vector<int> data;
                        if (data_type == "Random") data = generateRandomData(size);
                        else if (data_type == "Nearly Sorted") data = generateNearlySortedData(size, 1);
                        else if (data_type == "Reverse Sorted") data = generateReverseSortedData(size);

                        runTest(algo_pair.first, data, algo_pair.second, current_metrics);
                        avg_metrics.time_ms += current_metrics.time_ms;
                        avg_metrics.comparisons += current_metrics.comparisons;
                        avg_metrics.swaps += current_metrics.swaps;
                    }
                    avg_metrics.time_ms /= num_runs_per_test;
                    avg_metrics.comparisons /= num_runs_per_test;
                    avg_metrics.swaps /= num_runs_per_test;
                    std::cout << algo_pair.first << ": Tempo = " << avg_metrics.time_ms << " ms, Comparações = " << avg_metrics.comparisons << ", Trocas = " << avg_metrics.swaps << std::endl;
                    csv_file << algo_pair.first << "," << data_type << "," << size << "," << avg_metrics.time_ms << "," << avg_metrics.comparisons << "," << avg_metrics.swaps << "\n";
                } else {
                     csv_file << algo_pair.first << "," << data_type << "," << size << "," << "N/A,N/A,N/A\n";
                }
            }

            std::vector<std::string> efficient_algorithms = {"Merge Sort", "Quick Sort"};
            for (const std::string& algo_name : efficient_algorithms) {
                Metrics avg_metrics;
                for (int i = 0; i < num_runs_per_test; ++i) {
                    Metrics current_metrics;
                    std::vector<int> data;
                    if (data_type == "Random") data = generateRandomData(size);
                    else if (data_type == "Nearly Sorted") data = generateNearlySortedData(size, 1);
                    else if (data_type == "Reverse Sorted") data = generateReverseSortedData(size);

                    runTest(algo_name, data, nullptr, current_metrics); // nullptr para chamar a função específica
                    avg_metrics.time_ms += current_metrics.time_ms;
                    avg_metrics.comparisons += current_metrics.comparisons;
                    avg_metrics.swaps += current_metrics.swaps;
                }
                avg_metrics.time_ms /= num_runs_per_test;
                avg_metrics.comparisons /= num_runs_per_test;
                avg_metrics.swaps /= num_runs_per_test;
                std::cout << algo_name << ": Tempo = " << avg_metrics.time_ms << " ms, Comparações = " << avg_metrics.comparisons << ", Trocas = " << avg_metrics.swaps << std::endl;
                csv_file << algo_name << "," << data_type << "," << size << "," << avg_metrics.time_ms << "," << avg_metrics.comparisons << "," << avg_metrics.swaps << "\n";
            }
        }
    }

    csv_file.close();
    std::cout << "\n--- Testes Concluídos. Resultados salvos em sorting_results.csv ---" << std::endl;

    return 0;
}