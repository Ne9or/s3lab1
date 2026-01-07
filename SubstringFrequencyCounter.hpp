#pragma once

#include <stdexcept>

#include "DynamicArray.hpp"
#include "ReadOnlyStream.hpp"

class SubstringFrequencyCounter {
private:
    DynamicArray<char> pattern;
    DynamicArray<char> window;

    bool window_equals_pattern() const {
        if (window.get_size() != pattern.get_size())
            return false;

        for (int i = 0; i < pattern.get_size(); ++i) {
            if (window.get(i) != pattern.get(i))
                return false;
        }

        return true;
    }

    void slide_window(char next_char) {
        if (window.get_size() < pattern.get_size()) {
            window.push_back(next_char);
            return;
        }

        for (int i = 1; i < window.get_size(); ++i) {
            window.set(i - 1, window.get(i));
        }

        window.set(window.get_size() - 1, next_char);
    }

public:
    explicit SubstringFrequencyCounter(const std::string& pat)
        : pattern(pat.size()), window()
    {
        if (pat.empty())
            throw std::invalid_argument("Pattern must not be empty");

        for (int i = 0; i < pat.size(); ++i)
            pattern.set(i, pat[i]);
    }

    size_t count(ReadOnlyStream<char>& stream) {
        stream.open();

        size_t occurrences = 0;
        window.reset();

        while (!stream.is_end_of_stream()) {
            char c = stream.read();
            slide_window(c);

            if (window_equals_pattern())
                ++occurrences;
        }

        stream.close();
        return occurrences;
    }
};
