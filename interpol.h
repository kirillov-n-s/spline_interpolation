#pragma once
#include "solve.h"

namespace num
{
    template<typename T>
    std::valarray<T> divided_differences(const std::valarray<T>& x, std::valarray<T> f, std::size_t N);

    template<typename T>
    auto cubic_spline(const std::valarray<T>& x, const std::valarray<T>& F, T A, T B);

    template<typename T>
    std::valarray<T> error(const std::valarray<T>& f, const std::valarray<T>& Pn);

    template<typename T, typename F>
    std::valarray<T> apply(const std::valarray<T>& x, const F& f);
}

namespace num
{
    template<typename T>
    std::valarray<T> divided_differences(const std::valarray<T>& x, std::valarray<T> f, std::size_t N)
    {
        auto n = x.size();
        for (std::size_t i = 1; i <= N; ++i)
            for (std::size_t k = 0; k < n - i; ++k)
                f[k] = (f[k + 1] - f[k]) / (x[i + k] - x[k]);
        return f[std::slice(0, n - N, 1)];
    }

    template<typename T>
    auto cubic_spline(const std::valarray<T>& x, const std::valarray<T>& f, T A, T B)
    {
        //system size
        auto n = x.size() - 1;

        //h
        vector<T> h(n);
        for (std::size_t i = 1; i <= n; ++i)
            h[i] = x[i] - x[i - 1];

        //div diff
        auto F = divided_differences(x, f, 1);

        //vectors a, b, c, d for system
        //not using letters not to confuse with spline coefs
        vector<T> low(n - 1, 0, 2),
                  mid(n),
                  high(n - 1),
                  vec(n);

        T tmp;

        //i = 2..n-1
        for (std::size_t i = 2; i < n; ++i)
        {
            tmp = 1 / (h[i] + h[i + 1]);
            low[i]  = h[i + 1] * tmp;
            mid[i]  = 2;
            high[i] = h[i] * tmp;
            vec[i]  = 3 * (F[i - 1] * low[i] + F[i] * high[i]);
        }

        //i = 1
        tmp = 4 * h[1] + 3 * h[2];
        mid[1]  = 1;
        high[1] = 2 * h[1] / tmp;
        vec[1]  = A * h[1] * h[2] / 2 / tmp + 3 / tmp * (2 * h[1] * F[1] + h[2] * F[0]);

        //i = n
        low[n] = 1;
        mid[n] = 2;
        vec[n] = B * h[n] / 2 + 3 * F[n - 1];

        //solve system, define coef vectors
        auto b = thomas_solve({low, mid, high}, vec);
        vector<T> a = f,
                  c(n),
                  d(n);

        //i = 2..n
        for (std::size_t i = 2; i <= n; ++i)
        {
            c[i] = (2 * b[i] + b[i - 1] - 3 * F[i - 1]) / h[i];
            d[i] = (b[i] + b[i - 1] - 2 * F[i - 1]) / h[i] / h[i];
        }

        //i = 1
        c[1] = -1 / h[2] * (2 * b[1] + b[2] - 3 * F[1]);
        d[1] = 1 / h[1] / h[1] * (F[0] - b[1] + c[1] * h[1]);

        return [a, b, c, d, x](T X) -> T
        {
            //find interval index
            std::size_t n = b.size(), i;
            for (i = 1; i <= n; i++)
                if (X <= x[i] && X >= x[i - 1])
                    break;

            auto H = X - x[i];
            return a[i] + b[i] * H + c[i] * H * H + d[i] * H * H * H;
        };
    }

    template<typename T>
    std::valarray<T> error(const std::valarray<T>& f, const std::valarray<T>& Pn)
    {
        return std::abs(f - Pn);
    }

    template<typename T, typename F>
    std::valarray<T> apply(const std::valarray<T>& x, const F& f)
    {
        auto n = x.size();
        std::valarray<T> y(n);
        for (std::size_t i = 0; i < n; ++i)
            y[i] = f(x[i]);
        return y;
    }
}
