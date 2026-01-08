#include <iostream>
#include <fstream>
#include <string>

#include "ReadOnlyStream.hpp"
#include "SubstringFrequencyCounter.hpp"

std::string read_from_console()
{
    std::cout << "Введите текст (пустая строка — конец ввода):\n";

    std::string result;
    std::string line;

    while (true)
    {
        std::getline(std::cin, line);
        if (line.empty())
            break;

        result += line;
        result += '\n';
    }

    return result;
}

std::string read_from_file(const std::string& filename)
{
    std::ifstream file(filename);
    if (!file.is_open())
        throw std::runtime_error("Не удалось открыть файл");

    std::string result;
    std::string line;

    while (std::getline(file, line))
    {
        result += line;
        result += '\n';
    }

    return result;
}

int main()
{
    while (true)
    {
        std::cout << "\nМеню:\n";
        std::cout << "1 — ввод с консоли\n";
        std::cout << "2 — ввод из файла\n";
        std::cout << "0 — выход\n";
        std::cout << "> ";

        int choice;
        std::cin >> choice;
        std::cin.ignore();

        if (choice == 0)
        {
            std::cout << "Завершение программы.\n";
            return 0;
        }

        try
        {
            std::string text;

            if (choice == 1)
            {
                text = read_from_console();
            }
            else if (choice == 2)
            {
                std::cout << "Введите имя файла:\n> ";
                std::string filename;
                std::getline(std::cin, filename);
                text = read_from_file(filename);
            }
            else
            {
                std::cout << "Неверный пункт меню.\n";
                continue; 
            }

            std::cout << "Введите шаблон для поиска:\n> ";
            std::string pattern;
            std::getline(std::cin, pattern);

            if (pattern.empty())
            {
                std::cout << "Шаблон не может быть пустым.\n";
                continue;
            }

            ReadOnlyStream<char> stream(text);
            SubstringFrequencyCounter counter(pattern);

            size_t occurrences = counter.count(stream);

            std::cout << "Результат: " << occurrences << "\n";
        }
        catch (const std::exception& e)
        {
            std::cerr << "Ошибка: " << e.what() << "\n";
        }
    }
}
