#include <iostream>
#include <fstream>
#include <string>
#include <limits>

int main(int argc, char* argv[]) {

    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] << " filename start end" << std::endl;
        return 1;
    }

    std::string filename = argv[1];
    int start = std::stoi(argv[2]);
    int end = std::stoi(argv[3]);

    std::ifstream file(filename); // Відкриваємо файл для читання

    if (!file.is_open()) {
        std::cerr << "Failed to open the file." << std::endl;
        return 1;
    }

    int max = std::numeric_limits<int>::min(); // Ініціалізуємо максимальне значення
    int num;

    // Обробляємо лише частину файлу, пропускаючи певну кількість значень
    for (int i = 0; i < start; i++) {
        file >> num;
    }

    for (int i = start; i <= end; i++) {
        if (file >> num && num > max) {
            max = num;
        }
    }

    file.close();

    std::ofstream outputFile("output.txt", std::ios::app);
    outputFile << max << std::endl; // Записуємо максимальне значення до файлу
    outputFile.close();

    return 0;
}



