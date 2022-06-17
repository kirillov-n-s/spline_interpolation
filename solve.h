#pragma once
#include "tridiag.h"

namespace num
{
    template <typename T>
    vector<T> thomas_solve(tridiag<T> mat, vector<T> vec);
}

namespace num
{
    template <typename T>
    vector<T> thomas_solve(tridiag<T> mat, vector<T> vec)
    {
        //aliases
        auto& a = mat.a;
        auto& b = mat.b;
        auto& c = mat.c;
        auto& d = vec;

        //variables & result
        std::size_t n = mat.size();
        vector<T> x(n), L(n - 1, 0, 2), M(n, 0, 2);

        //forward iteration
        L[2] = c[1] / b[1];
        M[2] = d[1] / b[1];
        for (std::size_t i = 2; i < n; i++)
        {
            L[i + 1] = c[i] / (b[i] - a[i] * L[i]);
            M[i + 1] = (d[i] - a[i] * M[i]) / (b[i] - a[i] * L[i]);
        }
        M[n + 1] = (d[n] - a[n] * M[n]) / (b[n] - a[n] * L[n]);

        //backward iteration
        x[n] = M[n + 1];
        for (std::size_t i = n - 1; i > 0; i--)
            x[i] = M[i + 1] - L[i + 1] * x[i + 1];

        return x;
    }
}
