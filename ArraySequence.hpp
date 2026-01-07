#pragma once
#include <sstream>
#include <stdexcept>
#include <string>
#include "Sequence.hpp"
#include "DynamicArray.hpp"

template <typename T>
class ArraySequence : public Sequence<T>
{
private:
    DynamicArray<T> array;

public:
    ArraySequence() = default;
    ArraySequence(int initial_size);
    ArraySequence(const T* arr, int count);
    ArraySequence(const ArraySequence<T>& other);
    ArraySequence(ArraySequence<T>&& other) noexcept;
    ArraySequence(const Sequence<T>& seq);
    ~ArraySequence() override = default;

    ArraySequence<T>* append(const T& item) override;
    ArraySequence<T>* prepend(const T& item) override;
    ArraySequence<T>* set(int index, const T& item) override;
    ArraySequence<T>* remove(int index) override;
    ArraySequence<T>* insert_at(int index, const Sequence<T>* other_seq) override;

    T& get(int index) const override;
    T get_first() const override;
    T get_last() const override;

    int get_size() const override;

    ArraySequence<T>* get_subsequence(int start_index, int end_index) const override;
    ArraySequence<T>* map(std::function<T(T)> func ) override;
    ArraySequence<T>* reset() override;

    ArraySequence<T>& operator=(const ArraySequence<T>& other);
    ArraySequence<T>& operator=(ArraySequence<T>&& other) noexcept;
};

    
    template <typename T>
    ArraySequence<T>::ArraySequence(int initial_size) : array(initial_size) {} 

    template <typename T>
    ArraySequence<T>::ArraySequence(const T* arr, int count) : array(arr, count) {}

    template <typename T>
    ArraySequence<T>::ArraySequence(const ArraySequence<T>& other) : array(other.array) {}

    template <typename T>
    ArraySequence<T>::ArraySequence(ArraySequence<T>&& other) noexcept
    : array(std::move(other.array)) {} 

    template <typename T>
    ArraySequence<T>::ArraySequence(const Sequence<T>& seq) : array(seq.get_size()) {
        for (int i = 0; i < seq.get_size(); i++) {
            array.set(i, seq.get(i));
        }
    }

    template <typename T>
    ArraySequence<T>* ArraySequence<T>::append(const T& item) {
        array.push_back(item);
        return this;
    }

    
    template <typename T>
    ArraySequence<T>* ArraySequence<T>::prepend(const T& item) {
        array.push_front(item);
        return this;
    }

    template <typename T>
    ArraySequence<T>* ArraySequence<T>::set(int index, const T& item) {
        array.set(index, item);
        return this;
    }

    template <typename T>
    ArraySequence<T>* ArraySequence<T>::remove(int index) {
        for (int i = index; i < array.get_size() - 1; i++) {
            array.set(i, array.get(i + 1));
        }

        array.resize(array.get_size() - 1);
        return this;
    }

    template <typename T>
    ArraySequence<T>* ArraySequence<T>::insert_at(int index, const Sequence<T>* other_seq) {
        if (index < 0 || index > array.get_size()) {
            throw std::out_of_range("Index out of range");
        }
        
        if (other_seq == nullptr) {
            throw std::invalid_argument("Other sequence cannot be null");
        }

        int other_size = other_seq->get_size();

        if (index == array.get_size()) {
            for (int i = 0; i < other_size; i++) {
                array.push_back(other_seq->get(i));
            }
            return this;
        }

        if (index == 0) {

            for (int i = other_size - 1; i >= 0; --i) {
                array.push_front(other_seq->get(i));
            }
            return this;
        }

        DynamicArray<T> temp;
        
        for (int i = 0; i < index; i++) {
            temp.push_back(array.get(i));
        }
        
        for (int i = 0; i < other_size; i++) {
            temp.push_back(other_seq->get(i));
        }
        
        for (int i = index; i < array.get_size(); i++) {
            temp.push_back(array.get(i));
        }
        
        array = std::move(temp);
        return this;
    }

    template <typename T>
    T& ArraySequence<T>::get(int index) const {
        return array.get(index);
    }

    template <typename T>
    T ArraySequence<T>::get_first() const {
        if (array.get_size() == 0)
            throw std::runtime_error("Sequence is empty");
        return array.get(0);
    }

    template <typename T>
    T ArraySequence<T>::get_last() const {
        int vector_size = array.get_size();
        if (vector_size == 0)
            throw std::runtime_error("Sequence is empty");
        return array.get(vector_size - 1);
    }

    template <typename T>
    int ArraySequence<T>:: get_size() const {
        return array.get_size();
    }

    template <typename T>
    ArraySequence<T>* ArraySequence<T>::get_subsequence(int start_index, int end_index) const {
        if (array.get_size() == 0)
            throw std::runtime_error("Sequence is empty");

        if (start_index < 0 || end_index >= array.get_size() || start_index > end_index)
            throw std::out_of_range("Invalid subsequence range");

        int sub_size = end_index - start_index + 1;
        ArraySequence<T>* sub = new ArraySequence<T>;

        for (int i = start_index; i < end_index + 1; i++) {
            T item = array.get(i);
            sub->append(item);
        }

        return sub;
    }

    template <typename T>
    ArraySequence<T>* ArraySequence<T>::map(std::function<T(T)> func ) {
        ArraySequence<T>* mapped_array = new ArraySequence<T>(array.get_size());

        for (int i = 0; i < array.get_size(); i++){
            T mapped_item = func(array.get(i));
            mapped_array->set(i, mapped_item);
        }

        return mapped_array;
    }

    template <typename T>
    ArraySequence<T>* ArraySequence<T>::reset() {
        array.reset();

        return this;
    }

    template <typename T>
    ArraySequence<T>& ArraySequence<T>::operator=(const ArraySequence<T>& other) {
        if (this != &other) {
            array = other.array;  
        }
        return *this;
    }
    
    template <typename T>
    ArraySequence<T>& ArraySequence<T>::operator=(ArraySequence<T>&& other) noexcept {
        if (this != &other) {
            array = std::move(other.array);
        }
        return *this;
    }