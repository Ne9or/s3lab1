#include <iostream>
#include <fstream>
#include <memory>
#include <string>
#include <limits>

#include "Generator.hpp"
#include "LazySequence.hpp"
#include "ReadOnlyStream.hpp"
#include "SubstringFrequencyCounter.hpp"

int main()
{
    while (true)
    {
        std::cout << "\nМеню:\n"
                  << "1 — ввод с консоли\n"
                  << "2 — ввод из файла\n"
                  << "0 — выход\n> ";

        int choice;
        if (!(std::cin >> choice))
            return 0;

        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        if (choice == 0)
            break;

        try
        {
            std::cout << "Введите шаблон:\n> ";
            std::string pattern;
            std::getline(std::cin, pattern);

            if (pattern.empty())
                throw std::runtime_error("Шаблон пуст");

            SubstringFrequencyCounter counter(pattern);

            if (choice == 1)
            {
                std::cout << "Введите текст (пустая строка — конец ввода):\n";

                std::stringstream buffer;
                std::string line;

                while (true)
                {
                    std::getline(std::cin, line);
                    if (line.empty())
                        break;
                    buffer << line << '\n';
                }

                auto gen = std::make_unique<Stream_Generator<char>>(buffer);
                auto lazy = std::make_shared<LazySequence<char>>(std::move(gen));
                ReadOnlyStream<char> stream(lazy);

                std::cout << "Результат: " << counter.count(stream) << "\n";
            }
            else if (choice == 2)
            {
                std::cout << "Имя файла:\n> ";
                std::string filename;
                std::getline(std::cin, filename);

                std::ifstream file(filename);
                if (!file)
                    throw std::runtime_error("Не удалось открыть файл");

                auto gen = std::make_unique<Stream_Generator<char>>(file);
                auto lazy = std::make_shared<LazySequence<char>>(std::move(gen));
                ReadOnlyStream<char> stream(lazy);

                std::cout << "Результат: " << counter.count(stream) << "\n";
            }
        }
        catch (const std::exception& e)
        {
            std::cerr << "Ошибка: " << e.what() << "\n";
            std::cin.clear();
        }
    }

    return 0;
}
