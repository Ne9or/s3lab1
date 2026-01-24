#pragma once

#include <memory>
#include <functional>
#include <stdexcept>

template <typename T>
class Generator; 

#include "Sequence.hpp"
#include "ArraySequence.hpp"

template <typename T>
class LazySequence : public std::enable_shared_from_this<LazySequence<T>>
{
private:
    std::unique_ptr<Generator<T>> generator;
    std::unique_ptr<ArraySequence<T>> materialized_data;

    void init_function_generator(
        size_t arity,
        std::function<T(const Sequence<T>&)> rule
    );

public:
    LazySequence();

    LazySequence(
        const Sequence<T>& start_sequence,
        size_t arity,
        std::function<T(const Sequence<T>&)> rule
    );

    explicit LazySequence(const Sequence<T>& sequence);
    explicit LazySequence(std::unique_ptr<Generator<T>>&& gen);

    static std::shared_ptr<LazySequence<T>> create();

    static std::shared_ptr<LazySequence<T>> create(
        const Sequence<T>& start_sequence,
        size_t arity,
        std::function<T(const ArraySequence<T>&)> rule
    );

    static std::shared_ptr<LazySequence<T>> create(
        std::unique_ptr<Generator<T>>&& gen
    );

    static std::shared_ptr<LazySequence<T>> create(
        const Sequence<T>& sequence
    );

    T get(size_t index);
    T get_next();

    T get_first_materialized() const;
    T get_last_materialized() const;
    size_t get_materialized_count() const;

    bool has_next() const;

    std::shared_ptr<LazySequence<T>> append(
        std::shared_ptr<LazySequence<T>> items
    );

    std::shared_ptr<LazySequence<T>> prepend(
        std::shared_ptr<LazySequence<T>> items
    );

    std::shared_ptr<LazySequence<T>> insert_at(
        size_t insert_index,
        std::shared_ptr<LazySequence<T>> items
    );

    std::shared_ptr<LazySequence<T>> get_subsequence(
        size_t from_index,
        size_t to_index
    );

    template <typename T2>
    std::shared_ptr<LazySequence<T2>> map(
        std::function<T2(const T&)> func
    );

    std::shared_ptr<LazySequence<T>> where(
        std::function<bool(T)> func
    );

    std::shared_ptr<LazySequence<T>> set_generator(
        std::unique_ptr<Generator<T>> generator
    );
};

#include "../src/LazySequence.inl"
