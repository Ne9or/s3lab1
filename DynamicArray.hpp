#pragma once
#include <stdexcept>

template <typename T>
class DynamicArray {
private:
    T* data;
    int size;
    int capacity;

private:
    void ensure_capacity(int min_capacity);

public:
    DynamicArray();
    DynamicArray(int initial_size);
    DynamicArray(const T* arr, int count);

    DynamicArray(const DynamicArray<T>& other);
    DynamicArray(DynamicArray<T>&& other) noexcept;

    ~DynamicArray();

    void push_back(const T& value);
    void push_front(const T& value);
    void set(int index, const T& value);
    T& get(int index) const;
    int get_size() const;
    void resize(int index);
    void reset();

    DynamicArray<T>& operator=(const DynamicArray<T>& other);
    DynamicArray<T>& operator=(DynamicArray<T>&& other) noexcept;
};

template <typename T>
void DynamicArray<T>::ensure_capacity(int min_capacity) {
    if (min_capacity <= capacity)
        return;

    int new_capacity = (capacity == 0) ? 1 : capacity * 2;

    T* new_data = new T[new_capacity];

    for (int i = 0; i < size; ++i) {
        new_data[i] = data[i];
    }

    delete[] data;
    data = new_data;
    capacity = new_capacity;
}

template <typename T>
DynamicArray<T>::DynamicArray() : data(nullptr), size(0), capacity(0) {}

template <typename T>
DynamicArray<T>::DynamicArray(int initial_size) : size(initial_size), capacity(initial_size) {
    data = new T[capacity];
}

template <typename T>
DynamicArray<T>::DynamicArray(const T* arr, int count) : size(count), capacity(count) {
    data = new T[capacity];
    for (int i = 0; i < count; ++i) {
        data[i] = arr[i];
    }
}

template <typename T>
DynamicArray<T>::DynamicArray(const DynamicArray<T>& other) : size(other.size), capacity(other.capacity) {
    data = new T[capacity];
    for (int i = 0; i < size; ++i) {
        data[i] = other.data[i];
    }
}

template <typename T>
DynamicArray<T>::DynamicArray(DynamicArray<T>&& other) noexcept
    : data(other.data), 
      size(other.size), 
      capacity(other.capacity) {
    other.data = nullptr;
    other.size = 0;
    other.capacity = 0;
}

template <typename T>
DynamicArray<T>::~DynamicArray() {
    delete[] data;
}

template <typename T>
void DynamicArray<T>::push_back(const T& value) {
    ensure_capacity(size + 1);
    data[size++] = value;
}

template <typename T>
void DynamicArray<T>::push_front(const T& value) {
    ensure_capacity(size + 1);
    for (int i = size; i > 0; --i) {
        data[i] = data[i - 1];
    }

    data[0] = value;
    ++size;
}

template <typename T>
void DynamicArray<T>::set(int index, const T& value) {
    if (index < 0 || index >= size)
        throw std::out_of_range("DynamicArray::set index out of range");
    data[index] = value;
}

template <typename T>
T& DynamicArray<T>::get(int index) const {
    if (index < 0 || index >= size)
        throw std::out_of_range("DynamicArray::get index out of range");
    return data[index];
}

template <typename T>
int DynamicArray<T>::get_size() const {
    return size; 
}

template <typename T>
void DynamicArray<T>::resize(int index) {
    size -= index;
}

template <typename T>
void DynamicArray<T>::reset() {
    if (!data) return;
    delete[] data;
    data = nullptr;
    size = 0;
    capacity = 0;
}

template <typename T>
DynamicArray<T>& DynamicArray<T>::operator=(const DynamicArray<T>& other) {
    if (this != &other) {
        delete[] data;
        
        size = other.size;
        capacity = other.capacity;
        
        if (capacity > 0) {
            data = new T[capacity];
            for (int i = 0; i < size; ++i) {
                data[i] = other.data[i];
            }
        } else {
            data = nullptr;
        }
    }
    return *this;
}

template <typename T>
DynamicArray<T>& DynamicArray<T>::operator=(DynamicArray<T>&& other) noexcept {
    if (this != &other) {
        delete[] data;
        
        data = other.data;
        size = other.size;
        capacity = other.capacity;
        
        other.data = nullptr;
        other.size = 0;
        other.capacity = 0;
    }
    return *this;
}