#pragma once
#include <string>
#include <functional>

template <typename T>
class Sequence
{
public:

    virtual ~Sequence() =  default;

    virtual Sequence<T>* append(const T& item) = 0;
    virtual Sequence<T>* prepend(const T& item) = 0;
    virtual Sequence<T>* set(int index, const T& item) = 0;
    virtual Sequence<T>* remove(int index) = 0;
    virtual Sequence<T>* insert_at(int index, const Sequence<T>* other_seq) = 0;
    virtual T& get(int index) const = 0;
    virtual T get_first() const = 0;
    virtual T get_last() const = 0;
    virtual int get_size() const = 0;
    virtual Sequence<T>* get_subsequence(int start_index, int end_index) const = 0;
    virtual Sequence<T>* map(std::function<T(T)> func) = 0;
    virtual Sequence<T>* reset() = 0;
};