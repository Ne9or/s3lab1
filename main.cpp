#include <iostream>
#include <string>
#include <limits>

#include "SubstringFrequencyCounter.hpp"
#include "ReadOnlyStream.hpp"
#include "LazySequence.hpp"
#include "ArraySequence.hpp"

void print_menu() {
    std::cout << "\n===== Substring Frequency Counter =====\n";
    std::cout << "1. Count substring frequency in a string\n";
    std::cout << "0. Exit\n";
    std::cout << "Choose option: ";
}

std::string read_line(const std::string& prompt) {
    std::cout << prompt;
    std::string s;
    std::getline(std::cin, s);
    return s;
}

void handle_string_mode() {
    std::string text = read_line("Enter text: ");
    std::string pattern = read_line("Enter pattern: ");

    if (pattern.empty()) {
        std::cout << "Error: pattern must not be empty\n";
        return;
    }

    try {
        ReadOnlyStream<char> stream(text);
        SubstringFrequencyCounter counter(pattern);

        size_t result = counter.count(stream);

        std::cout << "Occurrences: " << result << '\n';
    }
    catch (const std::exception& e) {
        std::cout << "Error: " << e.what() << '\n';
    }
}

int main() {
    bool running = true;

    while (running) {
        print_menu();

        int choice;
        if (!(std::cin >> choice)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Invalid input\n";
            continue;
        }

        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        switch (choice) {
        case 1:
            handle_string_mode();
            break;

        case 0:
            running = false;
            break;

        default:
            std::cout << "Unknown option\n";
        }
    }

    std::cout << "Program finished\n";
    return 0;
}

