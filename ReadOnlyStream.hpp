#pragma once

#include <memory>
#include <stdexcept>
#include <string>

#include "LazySequence.hpp"

template <typename T>
class ReadOnlyStream {
private:
    std::shared_ptr<LazySequence<T>> source;
    size_t position;
    bool opened;

public:
    explicit ReadOnlyStream(std::shared_ptr<LazySequence<T>> seq)
        : source(seq), position(0), opened(false)
    {}

    explicit ReadOnlyStream(const std::basic_string<T>& str)
        : position(0), opened(false)
    {
        ArraySequence<T> seq;
        for (size_t i = 0; i < str.size(); ++i)
            seq.append(str[i]);

        source = LazySequence<T>::create(seq);
    }

    void open() {
        if (!source)
            throw std::runtime_error("Stream has no source");
        opened = true;
    }

    void close() {
        opened = false;
    }

    bool is_end_of_stream() const {
        if (!opened)
            throw std::runtime_error("Stream is not opened");

        return !(source->has_next() ||
                 position < source->get_materialized_count());
    }

    T read() {
        if (!opened)
            throw std::runtime_error("Stream is not opened");

        if (is_end_of_stream())
            throw std::runtime_error("End of stream");

        return source->get(position++);
    }

    size_t get_position() const {
        return position;
    }
};
