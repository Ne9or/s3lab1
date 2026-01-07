#pragma once

#include "Generator.hpp"
#include "Sequence.hpp"
#include "ArraySequence.hpp"
#include "Cardinal.hpp"
#include <functional>
#include <memory>
#include <stdexcept>

template <typename T>
class LazySequence : public std::enable_shared_from_this<LazySequence<T>>
{
private:
    std::unique_ptr<Generator<T>> generator;
    std::unique_ptr<ArraySequence<T>> materialized_data;

private:
    void init_function_generator(size_t arity, std::function<T(const Sequence<T>&)> rule) {
        generator = std::make_unique<Function_Generator<T>>(
            this->shared_from_this(), arity, rule
        );
    }

public:
    LazySequence()
        : materialized_data(std::make_unique<ArraySequence<T>>()) {}

    LazySequence(const Sequence<T>& start_sequence, size_t arity, std::function<T(const Sequence<T>&)> rule)
        : materialized_data(std::make_unique<ArraySequence<T>>(start_sequence)) {}

    LazySequence(const Sequence<T>& sequence)
    {
        this->materialized_data = std::make_unique<ArraySequence<T>>();
        this->generator = std::make_unique<Sequence_Generator<T>>(sequence);
    }

    LazySequence(std::unique_ptr<Generator<T>>&& gen)
    {
        this->materialized_data = std::make_unique<ArraySequence<T>>();
        this->generator = std::move(gen);
    }

    static std::shared_ptr<LazySequence<T>> create()
    {
        return std::shared_ptr<LazySequence<T>>(
            new LazySequence<T>()
        );
    }

    static std::shared_ptr<LazySequence<T>> create(
        const Sequence<T>& start_sequence,
        size_t arity,
        std::function<T(const ArraySequence<T>&)> rule
    ) {
        auto l = std::shared_ptr<LazySequence<T>>(
            new LazySequence<T>(start_sequence, arity, rule)
        );
        l->init_function_generator(arity, rule);
        return l;
    }

    static std::shared_ptr<LazySequence<T>> create(std::unique_ptr<Generator<T>>&& gen)
    {
        return std::shared_ptr<LazySequence<T>>(
            new LazySequence<T>(std::move(gen))
        );
    }

    static std::shared_ptr<LazySequence<T>> create(const Sequence<T>& sequence)
    {
        return std::shared_ptr<LazySequence<T>>(
            new LazySequence<T>(sequence)
        );
    }

    T get(size_t index) {
        while (materialized_data->get_size() <= index && generator->has_next()) {
            materialized_data->append(generator->get_next());
        }

        if (materialized_data->get_size() < index)
            throw std::runtime_error("Index beyond possible generation");

        return materialized_data->get(index);
    }

    T get_next() {
        return this->get(materialized_data->get_size());
    }

    T get_first_materialized() const {
        return materialized_data->get_first();
    }

    T get_last_materialized() const {
        return materialized_data->get_last();
    }

    size_t get_materialized_count() const {
        return materialized_data->get_size();
    }

    bool has_next() const {
        return generator->has_next();
    }

    std::shared_ptr<LazySequence<T>> append(std::shared_ptr<LazySequence<T>> items) {
        auto append_generator = std::make_unique<Concat_Generator<T>>(
            this->shared_from_this(), items
        );
        return std::make_shared<LazySequence<T>>(std::move(append_generator));
    }

    std::shared_ptr<LazySequence<T>> prepend(std::shared_ptr<LazySequence<T>> items) {
        auto prepend_generator = std::make_unique<Concat_Generator<T>>(
            items, this->shared_from_this()
        );
        return std::make_shared<LazySequence<T>>(std::move(prepend_generator));
    }

    std::shared_ptr<LazySequence<T>> insert_at(
        size_t insert_index,
        std::shared_ptr<LazySequence<T>> items
    ) {
        auto insert_generator = std::make_unique<Insert_Generator<T>>(
            this->shared_from_this(), items, insert_index
        );

        return std::make_shared<LazySequence<T>>(std::move(insert_generator));
    }

    std::shared_ptr<LazySequence<T>> get_subsequence(size_t from_index, size_t to_index) {
        auto subsequence_generator = std::make_unique<Subsequence_Generator<T>>(
            this->shared_from_this(), from_index, to_index
        );

        return std::make_shared<LazySequence<T>>(std::move(subsequence_generator));
    }

    template <typename T2>
    std::shared_ptr<LazySequence<T2>> map(std::function<T2(const T&)> func) {
        auto map_generator = std::make_unique<Map_Generator<T2, T>>(
            this->shared_from_this(), func
        );

        return std::make_shared<LazySequence<T2>>(std::move(map_generator));
    }

    std::shared_ptr<LazySequence<T>> where(std::function<bool(T)> func) {
        auto where_generator = std::make_unique<Where_Generator<T>>(
            this->shared_from_this(), func
        );

        return std::make_shared<LazySequence<T>>(std::move(where_generator));
    }

    std::shared_ptr<LazySequence<T>> set_generator(std::unique_ptr<Generator<T>> generator) {
        return std::make_shared<LazySequence<T>>(std::move(generator));
    }
};
