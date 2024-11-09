#ifndef MEAC___STACK_H
#define MEAC___STACK_H

#include <iostream>
#include <vector>
#include <stdexcept>

template <typename T>
class Stack {
protected:
    std::vector<T> items;
    int max_size;

public:
    // Constructor with optional max_size
    explicit Stack(int max_size = -1) : max_size(max_size) {}

    // Check if the stack is empty
    bool is_empty() const {
        return items.empty();
    }

    virtual // Push an item to the stack
    void push(const T& item) {
        if (max_size != -1 && items.size() >= max_size) {
            // If the max size is reached, remove the oldest item (front)
            items.erase(items.begin());
        }
        items.push_back(item);
    }

    // Pop an item from the stack
    T pop() {
        if (is_empty()) {
            throw std::out_of_range("pop from an empty stack");
        }
        T item = items.back();
        items.pop_back();
        return item;
    }

    // Peek the top item without removing it
    T peek() const {
        if (is_empty()) {
            throw std::out_of_range("peek from an empty stack");
        }
        return items.back();
    }

    // Get the current size of the stack
    int size() const {
        return items.size();
    }

    // Overload the [] operator to access elements by index
    T& operator[](int index) {
        if (index < 0 || index >= size()) {
            throw std::out_of_range("index out of range");
        }
        return items[index];
    }

    // Overload the [] operator for constant access
    const T& operator[](int index) const {
        if (index < 0 || index >= size()) {
            throw std::out_of_range("index out of range");
        }
        return items[index];
    }

    // Function to get the last n elements of the stack
    std::vector<T> get_last_n(int n) const {
        if (n <= 0) {
            throw std::invalid_argument("Number of elements must be positive");
        }
        int count = std::min(static_cast<int>(this->items.size()), n);

        return std::vector<T>(items.end() - count, items.end());
    }
};

#endif //MEAC___STACK_H
