#pragma once

#include "Generator.hpp"

// ===== constructors =====

template <typename T>
LazySequence<T>::LazySequence()
    : materialized_data(std::make_unique<ArraySequence<T>>())
{}

template <typename T>
LazySequence<T>::LazySequence(
    const Sequence<T>& start_sequence,
    size_t,
    std::function<T(const Sequence<T>&)>
)
    : materialized_data(std::make_unique<ArraySequence<T>>(start_sequence))
{}

template <typename T>
LazySequence<T>::LazySequence(const Sequence<T>& sequence)
{
    materialized_data = std::make_unique<ArraySequence<T>>();
    generator = std::make_unique<Sequence_Generator<T>>(sequence);
}

template <typename T>
LazySequence<T>::LazySequence(std::unique_ptr<Generator<T>>&& gen)
{
    materialized_data = std::make_unique<ArraySequence<T>>();
    generator = std::move(gen);
}

// ===== init =====

template <typename T>
void LazySequence<T>::init_function_generator(
    size_t arity,
    std::function<T(const Sequence<T>&)> rule
)
{
    generator = std::make_unique<Function_Generator<T>>(
        this->shared_from_this(), arity, rule
    );
}

// ===== factory =====

template <typename T>
std::shared_ptr<LazySequence<T>> LazySequence<T>::create()
{
    return std::shared_ptr<LazySequence<T>>(new LazySequence<T>());
}

template <typename T>
std::shared_ptr<LazySequence<T>> LazySequence<T>::create(
    const Sequence<T>& start_sequence,
    size_t arity,
    std::function<T(const ArraySequence<T>&)> rule
)
{
    auto l = std::shared_ptr<LazySequence<T>>(
        new LazySequence<T>(start_sequence, arity, rule)
    );
    l->init_function_generator(arity, rule);
    return l;
}

template <typename T>
std::shared_ptr<LazySequence<T>> LazySequence<T>::create(
    std::unique_ptr<Generator<T>>&& gen
)
{
    return std::shared_ptr<LazySequence<T>>(
        new LazySequence<T>(std::move(gen))
    );
}

template <typename T>
std::shared_ptr<LazySequence<T>> LazySequence<T>::create(
    const Sequence<T>& sequence
)
{
    return std::shared_ptr<LazySequence<T>>(
        new LazySequence<T>(sequence)
    );
}

// ===== core =====

template <typename T>
T LazySequence<T>::get(size_t index)
{
    while (materialized_data->get_size() <= index &&
           generator && generator->has_next())
    {
        materialized_data->append(generator->get_next());
    }

    if (materialized_data->get_size() <= index)
        throw std::runtime_error("Index beyond possible generation");

    return materialized_data->get(index);
}

template <typename T>
T LazySequence<T>::get_next()
{
    return get(materialized_data->get_size());
}

template <typename T>
T LazySequence<T>::get_first_materialized() const
{
    return materialized_data->get_first();
}

template <typename T>
T LazySequence<T>::get_last_materialized() const
{
    return materialized_data->get_last();
}

template <typename T>
size_t LazySequence<T>::get_materialized_count() const
{
    return materialized_data->get_size();
}

template <typename T>
bool LazySequence<T>::has_next() const
{
    return generator && generator->has_next();
}

// ===== combinators =====

template <typename T>
std::shared_ptr<LazySequence<T>> LazySequence<T>::append(
    std::shared_ptr<LazySequence<T>> items)
{
    auto gen = std::make_unique<Concat_Generator<T>>(
        this->shared_from_this(), items
    );
    return std::make_shared<LazySequence<T>>(std::move(gen));
}

template <typename T>
std::shared_ptr<LazySequence<T>> LazySequence<T>::prepend(
    std::shared_ptr<LazySequence<T>> items)
{
    auto gen = std::make_unique<Concat_Generator<T>>(
        items, this->shared_from_this()
    );
    return std::make_shared<LazySequence<T>>(std::move(gen));
}

template <typename T>
std::shared_ptr<LazySequence<T>> LazySequence<T>::insert_at(
    size_t insert_index,
    std::shared_ptr<LazySequence<T>> items)
{
    auto gen = std::make_unique<Insert_Generator<T>>(
        this->shared_from_this(), items, insert_index
    );
    return std::make_shared<LazySequence<T>>(std::move(gen));
}

template <typename T>
std::shared_ptr<LazySequence<T>> LazySequence<T>::get_subsequence(
    size_t from_index,
    size_t to_index)
{
    auto gen = std::make_unique<Subsequence_Generator<T>>(
        this->shared_from_this(), from_index, to_index
    );
    return std::make_shared<LazySequence<T>>(std::move(gen));
}

template <typename T>
template <typename T2>
std::shared_ptr<LazySequence<T2>> LazySequence<T>::map(
    std::function<T2(const T&)> func)
{
    auto gen = std::make_unique<Map_Generator<T2, T>>(
        this->shared_from_this(), func
    );
    return std::make_shared<LazySequence<T2>>(std::move(gen));
}

template <typename T>
std::shared_ptr<LazySequence<T>> LazySequence<T>::where(
    std::function<bool(T)> func)
{
    auto gen = std::make_unique<Where_Generator<T>>(
        this->shared_from_this(), func
    );
    return std::make_shared<LazySequence<T>>(std::move(gen));
}

template <typename T>
std::shared_ptr<LazySequence<T>> LazySequence<T>::set_generator(
    std::unique_ptr<Generator<T>> generator)
{
    return std::make_shared<LazySequence<T>>(std::move(generator));
}


#include "LazySequence.hpp"