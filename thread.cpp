#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <fstream>
#include <algorithm>
#include <chrono>

std::mutex mtx; // Глобальна змінна для синхронізації доступу до спільних даних між потоками.
using TimePoint = std::chrono::high_resolution_clock::time_point;

// Функція findMaxInThread виконує пошук максимального значення в частині вектора arr, вказаній індексами start і end.
// Результат зберігається в maxResult. Крім того, ця функція записує інформацію про час виконання в файл outputFile.
void findMaxInThread(const std::vector<int>& arr, int& maxResult,
        int threadId, std::ofstream& outputFile, TimePoint start_time) {
    auto start_time2 = std::chrono::high_resolution_clock::now();
    int max = *std::max_element(arr.begin(), arr.end());
    TimePoint end_time = std::chrono::high_resolution_clock::now();
    mtx.lock(); // Блокування м'ютекса для безпечного доступу до спільних даних.
    if (max > maxResult) {
        maxResult = max;
    }
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time -
            start_time);
    outputFile << "Maximum " << threadId << ": " << maxResult << "\n";
    outputFile << "Thread " << threadId << ": Execution time: " << duration.count() << " ms\n";
    mtx.unlock(); // Розблокування м'ютекса після завершення доступу до спільних даних.
}

int main() {
    std::vector<int> data; // Вектор для зберігання чисел

    // Відкриття текстового файлу для зчитування
    std::ifstream inputFile("C:\\Users\\User\\CLionProjects\\main\\cmake-build-debug"
                            "\\output.txt");

    if (!inputFile.is_open()) {
        std::cerr << "Can't open this file." << std::endl;
        return 1;
    }

    // Зчитування чисел з файлу
    int number;
    while (inputFile >> number) {
        data.push_back(number);
    }

    // Закриття файлу
    inputFile.close();

    const int numThreads = 2;
    int chunkSize = data.size() / numThreads;

    std::vector<std::thread> threads(numThreads);  // Вектор для потоків.
    std::vector<int> maxResults(numThreads, std::numeric_limits<int>::min());

    // Відкриття файлу для запису результатів
    std::ofstream outputFile("output.txt");

    for (int i = 0; i < numThreads; i++) {
        int start = i * chunkSize;
        int end = (i == numThreads - 1) ? data.size() : (i + 1) * chunkSize; //
        // Обчислює кінцевий індекс для поточного потоку.
        // Якщо поточний потік останній (i == numThreads - 1), то end буде рівний розміру вектора data, інакше він визначає кінець "куска" даних
        std::vector<int> data_part(data.begin() + start, data.begin() + end);
        TimePoint start_time = std::chrono::high_resolution_clock::now();
        threads[i] = std::thread(findMaxInThread, data_part, std::ref(maxResults[i]), i, std::ref(outputFile), start_time);
        // Створює новий потік, який викликає функцію
    }

    for (int i = 0; i < numThreads; i++) {
        threads[i].join();
    }
    // чекає, поки всі потоки завершать свою роботу і об'єднує їхнє виконання.
    // Це необхідно, щоб головний потік зачекав із продовженням виконання, доки всі потоки завершать обчислення.

    int maxElement = *std::max_element(maxResults.begin(), maxResults.end());

    // Закриття файлу результатів
    outputFile << "Maximum number: " << maxElement << std::endl;
    outputFile.close();

    return 0;
}