#include <iostream>
#include <fstream>
#include <windows.h>
#include <vector>
#include <string>
#include <limits>
#include <chrono>


int main() {
    // Видаляємо попередній вихідний файл, якщо він існує
    std::string outputFile = "C:\\Users\\User\\CLionProjects\\main\\cmake-build-debug"
                           "\\output.txt";
    remove(outputFile.c_str());

    int num_processes = 2; // Кількість паралельних процесів
    int max = std::numeric_limits<int>::min(); // int - -2147483648
    std::vector<HANDLE> processHandles; // Вектор для збереження дескрипторів дочірніх процесів
    std::vector<int> start_end = {0, 499, 999}; // Діапазони обробки для
    // кожного дочірнього процесу
    std::string filename = "C:\\Users\\User\\CLionProjects\\file\\cmake-build-debug"
                           "\\data.txt";
    std::string applicationName =
            "C:\\Users\\User\\CLionProjects\\child_process\\cmake-build-debug\\child_process.exe";
    std::vector<std::chrono::time_point<std::chrono::system_clock>> start_time;

    for (int i = 0; i < num_processes; i++) {
        STARTUPINFO si;
        PROCESS_INFORMATION pi;
        ZeroMemory(&si, sizeof(si)); // Ініціалізуємо структуру STARTUPINFO
        si.cb = sizeof(si);
        ZeroMemory(&pi, sizeof(pi)); // Ініціалізуємо структуру PROCESS_INFORMATION
        std::string commandLine = applicationName + " " + filename + " " +
                std::to_string(start_end[i]) + " " + std::to_string(start_end[i+1]);
        // Створюємо дочірний процес
        auto child_process_start_time = std::chrono::high_resolution_clock::now();
        start_time.push_back(child_process_start_time);
        if (CreateProcess(NULL, const_cast<char*>(commandLine.c_str()), NULL, NULL, FALSE,
                0, NULL, NULL, &si, &pi)) {
            processHandles.push_back(pi.hProcess); // Додаємо дескриптор дочірнього процесу в вектор
        } else {
            std::cerr << "Failed to create child process." << std::endl;
            return 1;
        }
    }

    std::vector<int> time;
    for (size_t i = 0; i < processHandles.size(); i++) {
        WaitForSingleObject(processHandles[i], INFINITE); // Очікуємо завершення
        auto child_process_end_time = std::chrono::high_resolution_clock::now();
        auto child_process_duration =
                std::chrono::duration_cast<std::chrono::milliseconds>
                        (child_process_end_time - start_time[i]);
        time.push_back(child_process_duration.count());
    }

    int finalMax = std::numeric_limits<int>::min();
    std::string inputFilename = "output.txt";
    std::ifstream inputFile(inputFilename);
    int processMax;
    // Визначаємо максимум з максимумів
    while (inputFile >> processMax) {
            if (static_cast<int>(processMax) > finalMax) {
                finalMax = processMax;
            }
    }
    inputFile.close();
    // Записуємо час кожного процесу
    for (int i = 0; i < num_processes; i++) {
    std::ofstream outputFile("output.txt", std::ios::app);
    outputFile << "Child process " << i << " execution time: " << time[i] << " ms" <<
    std::endl;
    outputFile.close();
    }

    std::ofstream outputFileStream("output.txt", std::ios::app);
    outputFileStream << "Max number in the file: " << finalMax << std::endl;
    outputFileStream.close();

    return 0;
}