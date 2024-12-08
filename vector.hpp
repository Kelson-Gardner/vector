#pragma once

#include <algorithm>
#include <cstdint>
#include <exception>
#include <functional>
#include <iterator>
#include <memory>
#include <stdexcept>
#include <string>

namespace usu
{
template <typename T, std::size_t InitialCapacity = 10> class vector
{
  public:
    using size_type = std::size_t;
    using reference = T&;
    using pointer = std::unique_ptr<T[]>;
    using value_type = T;
    using resize_type = std::function<size_type(size_type)>;

    class iterator
    {
      public:
        using iterator_category = std::bidirectional_iterator_tag;

        iterator() : pos(0), data(nullptr), size(0) {}
        iterator(size_type pos, T* data, size_type size)
            : pos(pos), data(data), size(size)
        {
        }

        iterator(const iterator& obj) = default;

        iterator(iterator&& obj) noexcept = default;

        iterator& operator=(const iterator& rhs) = default;

        iterator& operator=(iterator&& rhs) = default;

        iterator& operator++()
        {
            if (this->pos >= this->size)
                throw std::out_of_range("Iterator is outside of range");
            this->pos++;
            return *this;
        }

        iterator operator++(int)
        {
            iterator temp = *this;
            ++(*this);
            return temp;
        }

        iterator& operator--()
        {
            if (this->pos == 0)
                throw std::out_of_range("Iterator is outside of range");
            this->pos--;
            return *this;
        }

        iterator operator--(int)
        {
            iterator temp = *this;
            --(*this);
            return temp;
        }

        T* operator->() { return &this->data[this->pos]; }

        reference operator*() { return this->data[this->pos]; }

        bool operator==(const iterator& rhs) const { return this->pos == rhs.pos; }
        bool operator!=(const iterator& rhs) const { return !(*this == rhs); }

      private:
        size_type pos;
        T* data;
        size_type size;
    };

    vector()
        : m_size(0), m_capacity(InitialCapacity),
          m_resize_type([](size_type currentCapacity)
                        { return currentCapacity * 2; }),
          m_data(std::make_unique<T[]>(InitialCapacity))
    {
    }

    vector(size_type size) : vector()
    {
        resize_capacity(size > InitialCapacity ? size * 2 : InitialCapacity);
        m_size = size;
    }

    vector(resize_type resize) : vector() { m_resize_type = resize; }

    vector(size_type size, resize_type resize) : vector(size)
    {
        m_resize_type = resize;
    }

    vector(std::initializer_list<T> list) : vector()
    {
        for (const auto& item : list)
            add(item);
    }

    vector(std::initializer_list<T> list, resize_type resize) : vector(list)
    {
        m_resize_type = resize;
    }

    reference operator[](size_type index)
    {
        if (index >= m_size)
            throw std::range_error("Out of Range");
        return m_data[index];
    }

    void add(T value)
    {
        if (m_size == m_capacity)
            resize_capacity(m_resize_type(m_capacity));
        m_data[m_size++] = value;
    }

    void insert(size_type index, T value)
    {
        if (index > m_size)
            throw std::range_error("Out of Range");

        if (m_size == m_capacity)
            resize_capacity(m_resize_type(m_capacity));

        for (size_type i = m_size; i > index; i--)
            m_data[i] = std::move(m_data[i - 1]);

        m_data[index] = value;
        m_size += 1;
    }

    void remove(size_type index)
    {
        if (index >= m_size)
            throw std::range_error("Index out of bounds");

        for (size_type i = index; i < m_size - 1; i++)
            m_data[i] = std::move(m_data[i + 1]);

        m_size -= 1;
    }

    void clear()
    {
        m_data = std::make_unique<T[]>(m_capacity);
        m_size = 0;
    }

    size_type size() const { return m_size; }

    size_type capacity() const { return m_capacity; }

    iterator begin() { return iterator(0, m_data.get(), m_size); }

    iterator end() { return iterator(m_size, m_data.get(), m_size); }

    void map(std::function<void(reference)> func)
    {
        for (size_type i = 0; i < m_size; i++)
            func(m_data[i]);
    }

  private:
    size_type m_size;
    size_type m_capacity;
    resize_type m_resize_type;
    pointer m_data;

    void resize_capacity(size_type new_capacity)
    {
        pointer new_data = std::make_unique<T[]>(new_capacity);
        std::move(m_data.get(), m_data.get() + m_size, new_data.get());
        m_data = std::move(new_data);
        m_capacity = new_capacity;
    }
};

} // namespace usu
