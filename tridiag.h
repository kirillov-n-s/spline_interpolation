#pragma once
#include "vector.h"

namespace num
{
    template<typename T>
    class tridiag;

    template<typename T>
    vector<T> operator*(const tridiag<T> &mat, const vector<T> &vec);
}

namespace num
{
    template<typename T>
    class tridiag
    {
    public:
        vector<T> a, b, c;

        tridiag(std::size_t size = 1, const T &value = 0);
        tridiag(const vector<T> &a,
                const vector<T> &b,
                const vector<T> &c);
        tridiag(std::size_t size, const T &min, const T &max);
        tridiag(std::size_t size,
                const T &min_a, const T &max_a,
                const T &min_b, const T &max_b,
                const T &min_c, const T &max_c);

        tridiag(const tridiag &other);
        tridiag &operator=(const tridiag &other);

        std::size_t size() const;

        friend vector<T> operator*<T>(const tridiag<T> &mat, const vector<T> &vec);
    };
}

namespace num
{
    template<typename T>
    tridiag<T>::tridiag(std::size_t size, const T &value)
        : a(size - 1, value, 2), b(size, value), c(size - 1, value) {}

    template<typename T>
    tridiag<T>::tridiag(const vector<T> &a,
                        const vector<T> &b,
                        const vector<T> &c)
        : a(a), b(b), c(c) {}

    template<typename T>
    tridiag<T>::tridiag(std::size_t size, const T &min, const T &max)
        : a(size - 1, min, max, 2),
          b(size, min, max),
          c(size - 1, min, max)
          {}

    template<typename T>
    tridiag<T>::tridiag(std::size_t size,
                        const T &min_a, const T &max_a,
                        const T &min_b, const T &max_b,
                        const T &min_c, const T &max_c)
        : a(size - 1, min_a, max_a, 2),
          b(size, min_b, max_b),
          c(size - 1, min_c, max_c)
          {}

    template<typename T>
    tridiag<T>::tridiag(const tridiag &other)
            : a(other.a), b(other.b), c(other.c) {}

    template<typename T>
    tridiag<T> &tridiag<T>::operator=(const tridiag<T> &other)
    {
        a = other.a;
        b = other.b;
        c = other.c;
        return *this;
    }

    template<typename T>
    std::size_t tridiag<T>::size() const
    {
        return b.size();
    }

    template<typename T>
    vector<T> operator*(const tridiag<T> &mat, const vector<T> &vec)
    {
        int n = mat.size();
        vector<T> res(n);
        res[1] = mat.b[1] * vec[1] + mat.c[1] * vec[2];
        for (int i = 2; i < n; i++)
            res[i] = mat.a[i] * vec[i - 1] + mat.b[i] * vec[i] + mat.c[i] * vec[i + 1];
        res[n] = mat.a[n] * vec[n - 1] + mat.b[n] * vec[n];
        return res;
    }
}
