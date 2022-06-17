#pragma once
#include <valarray>
#include <random>

namespace num
{
    template<typename T>
    class vector
    {
        std::valarray<T> _values;

    public:
        int indexing = 1;

        vector(std::size_t size = 1, const T &value = 0, int indexing = 1);
        vector(std::size_t size, const T &min, const T &max, int indexing = 1);

        vector(const vector &other);
        vector &operator=(const vector &other);

        vector(const std::valarray<T>& values);
        operator std::valarray<T>() const;

        T &operator[](std::size_t pos);
        const T &operator[](std::size_t pos) const;

        std::size_t size() const;
    };
}

//func def
namespace num
{
    template<typename T>
    vector<T>::vector(std::size_t size, const T &value, int indexing)
        : _values(size), indexing(indexing)
    {
        std::fill(begin(_values), end(_values), value);
    }

    template<typename T>
    vector<T>::vector(std::size_t size, const T &min, const T &max, int indexing)
        : _values(size), indexing(indexing)
    {
        std::mt19937 rng((std::random_device()()));
        std::uniform_real_distribution<T> distrib(min, max);
        std::generate(begin(_values), end(_values),
                              [&rng, &distrib]() { return distrib(rng); });
    }

    template<typename T>
    vector<T>::vector(const vector<T> &other)
            : _values(other._values), indexing(other.indexing) {}

    template<typename T>
    vector<T> &vector<T>::operator=(const vector<T> &other)
    {
        _values = other._values;
        return *this;
    }

    template<typename T>
    vector<T>::vector(const std::valarray<T>& values)
            : _values(values), indexing(0) {}

    template<typename T>
    vector<T>::operator std::valarray<T>() const
    {
        return _values;
    }

    template<typename T>
    T &vector<T>::operator[](std::size_t pos) 
    {
        return _values[pos - indexing];
    }

    template<typename T>
    const T &vector<T>::operator[](std::size_t pos) const
    {
        return _values[pos - indexing];
    }

    template<typename T>
    std::size_t vector<T>::size() const
    {
        return _values.size();
    }
}
