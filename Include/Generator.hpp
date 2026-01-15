#pragma once

#include <memory>
#include <functional>
#include <optional>
#include <stdexcept>

#include "Sequence.hpp"
#include "ArraySequence.hpp"


template <typename T>
class LazySequence;

template <typename T>
class Generator
{
public:
    virtual T get_next() = 0;
    virtual bool has_next() = 0;
    virtual ~Generator() = default;
};

template <typename T>
class Function_Generator : public Generator<T>
{
private:
    std::weak_ptr<LazySequence<T>> owner;
    std::function<T(const Sequence<T>&)> rule;
    size_t arity;

public:
    Function_Generator(
        std::shared_ptr<LazySequence<T>> gen_owner,
        size_t arity,
        std::function<T(const Sequence<T>&)> rule)
        : owner(gen_owner), rule(rule), arity(arity)
    {}

    T get_next() override
    {
        auto locked_owner = owner.lock();
        if (!locked_owner)
            throw std::runtime_error("Owner sequence expired");

        if (locked_owner->get_materialized_count() < arity)
            throw std::runtime_error("Not enough elements to generate next");

        ArraySequence<T> args_buffer(arity);
        size_t size = locked_owner->get_materialized_count();

        for (size_t i = 0; i < arity; i++)
            args_buffer.set(i, locked_owner->get(size - arity + i));

        return rule(args_buffer);
    }

    bool has_next() override
    {
        return true;
    }
};

template <typename T>
class Sequence_Generator : public Generator<T>
{
private:
    ArraySequence<T> sequence;
    size_t current_index;

public:
    explicit Sequence_Generator(const ArraySequence<T>& seq)
        : sequence(seq), current_index(0)
    {}

    Sequence_Generator(const ArraySequence<T>& seq, size_t index)
        : sequence(seq), current_index(index)
    {}

    T get_next() override
    {
        if(!has_next())
            throw std::runtime_error("End of Sequence");
        return sequence.get(current_index++);
    }

    bool has_next() override
    {
        return current_index < sequence.get_size();
    }
};

template <typename T>
class Concat_Generator : public Generator<T>
{
private:
    std::shared_ptr<LazySequence<T>> first;
    std::shared_ptr<LazySequence<T>> second;

    size_t first_index;
    size_t second_index;

public:
    Concat_Generator(
        std::shared_ptr<LazySequence<T>> first_seq,
        std::shared_ptr<LazySequence<T>> second_seq)
        : first(first_seq),
          second(second_seq),
          first_index(0),
          second_index(0)
    {}

    T get_next() override
    {
        bool can_use_first =
            first->has_next() || first_index < first->get_materialized_count();

        bool can_use_second =
            second->has_next() || second_index < second->get_materialized_count();

        if (can_use_first)
            return first->get(first_index++);

        if (can_use_second)
            return second->get(second_index++);

        throw std::runtime_error("Generation limit reached");
    }

    bool has_next() override
    {
        bool can_use_first =
            first->has_next() || first_index < first->get_materialized_count();

        bool can_use_second =
            second->has_next() || second_index < second->get_materialized_count();

        return can_use_first || can_use_second;
    }
};

template <typename T>
class Insert_Generator : public Generator<T>
{
private:
    std::shared_ptr<LazySequence<T>> primary;
    std::shared_ptr<LazySequence<T>> secondary;

    size_t primary_index;
    size_t secondary_index;
    size_t current_index;
    size_t insert_index;

public:
    Insert_Generator(
        std::shared_ptr<LazySequence<T>> primary_seq,
        std::shared_ptr<LazySequence<T>> secondary_seq,
        size_t insert_index)
        : primary(primary_seq),
          secondary(secondary_seq),
          primary_index(0),
          secondary_index(0),
          current_index(0),
          insert_index(insert_index)
    {}

    T get_next() override
    {
        bool can_use_primary =
            primary->has_next() || primary_index < primary->get_materialized_count();

        bool can_use_secondary =
            secondary->has_next() || secondary_index < secondary->get_materialized_count();

        if (current_index == insert_index && can_use_secondary)
        {
            ++current_index;
            return secondary->get(secondary_index++);
        }

        ++current_index;

        if (can_use_primary)
            return primary->get(primary_index++);

        throw std::runtime_error("Generation limit reached");
    }

    bool has_next() override
    {
        bool can_use_primary =
            primary->has_next() || primary_index < primary->get_materialized_count();

        bool can_use_secondary =
            secondary->has_next() || secondary_index < secondary->get_materialized_count();

        return can_use_primary || can_use_secondary;
    }
};

template <typename T>
class Subsequence_Generator : public Generator<T>
{
private:
    std::shared_ptr<LazySequence<T>> sequence;
    size_t current_index;
    size_t from_index;
    size_t to_index;

public:
    Subsequence_Generator(
        std::shared_ptr<LazySequence<T>> seq,
        size_t from,
        size_t to)
        : sequence(seq),
          current_index(from),
          from_index(from),
          to_index(to)
    {}

    T get_next() override
    {
        if (has_next())
            return sequence->get(current_index++);

        throw std::runtime_error("Generation limit reached");
    }

    bool has_next() override
    {
        bool can_use_seq =
            sequence->has_next() || current_index < sequence->get_materialized_count();

        return current_index >= from_index &&
               current_index <= to_index &&
               can_use_seq;
    }
};

template <typename TOut, typename TIn>
class Map_Generator : public Generator<TOut>
{
private:
    std::shared_ptr<LazySequence<TIn>> sequence;
    size_t current_index;
    std::function<TOut(TIn)> func;

public:
    Map_Generator(
        std::shared_ptr<LazySequence<TIn>> seq,
        std::function<TOut(TIn)> func)
        : sequence(seq),
          current_index(0),
          func(func)
    {}

    TOut get_next() override
    {
        if (has_next())
            return func(sequence->get(current_index++));

        throw std::runtime_error("Generation limit reached");
    }

    bool has_next() override
    {
        return sequence->has_next() ||
               current_index < sequence->get_materialized_count();
    }
};

template <typename T>
class Where_Generator : public Generator<T>
{
private:
    std::shared_ptr<LazySequence<T>> sequence;
    size_t current_index;
    std::optional<T> cached_item;
    std::function<bool(T)> func;

public:
    Where_Generator(
        std::shared_ptr<LazySequence<T>> seq,
        std::function<bool(T)> func)
        : sequence(seq),
          current_index(0),
          cached_item(std::nullopt),
          func(func)
    {}

    T get_next() override
    {
        if (has_next())
        {
            T result = *cached_item;
            cached_item.reset();
            return result;
        }

        throw std::runtime_error("Generation limit reached");
    }

    bool has_next() override
    {
        if (cached_item.has_value())
            return true;

        while (sequence->has_next() ||
               current_index < sequence->get_materialized_count())
        {
            T item = sequence->get(current_index++);
            if (func(item))
            {
                cached_item = item;
                return true;
            }
        }

        return false;
    }
};

template <typename T>
class Stream_Generator : public Generator<T>
{
private:
    std::istream* in;
    bool finished;

public:
    explicit Stream_Generator(std::istream& input)
        : in(&input), finished(false)
    {}

    bool has_next() override
    {
        if (finished)
            return false;

        int c = in->peek();
        if (c == EOF)
        {
            finished = true;
            return false;
        }
        return true;
    }

    T get_next() override
    {
        if (!has_next())
            throw std::runtime_error("End of stream");

        T value;
        in->get(value);

        if (!(*in))
            finished = true;

        return value;
    }
};

#include "LazySequence.hpp"