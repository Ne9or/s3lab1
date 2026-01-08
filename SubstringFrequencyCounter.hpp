#pragma once

#include <stdexcept>
#include <string>

#include "DynamicArray.hpp"
#include "ReadOnlyStream.hpp"

class SubstringFrequencyCounter {
private:
    DynamicArray<char> pattern;

    bool is_delimiter(char c) const
    {
        return c == ' '  ||
               c == '\n' ||
               c == '\t' ||
               c == ','  ||
               c == ';';
    }

public:
    explicit SubstringFrequencyCounter(const std::string& pat)
        : pattern(pat.size())
    {
        if (pat.empty())
            throw std::invalid_argument("Pattern must not be empty");

        for (int i = 0; i < pat.size(); ++i)
            pattern.set(i, pat[i]);
    }

    size_t count(ReadOnlyStream<char>& stream)
    {
        stream.open();

        size_t occurrences = 0;
        int matched = 0;

        while (!stream.is_end_of_stream())
        {
            char c = stream.read();

            // игнорируем разделители (включая CSV)
            if (is_delimiter(c))
                continue;

            if (c == pattern.get(matched))
            {
                ++matched;

                if (matched == pattern.get_size())
                {
                    ++occurrences;
                    matched = 0;
                }
            }
            else
            {
                if (c == pattern.get(0))
                    matched = 1;
                else
                    matched = 0;
            }
        }

        stream.close();
        return occurrences;
    }
};
