#include "tuple"
#include "split.h"
#include "format.h"
#include "interpol.h"

using real = double;

//params
real a = -5, b = 5;
std::size_t N = 1000, k = 100;

//functions
auto f1 = [](real x) -> real
{
    return x*x*x + x*x + x + 1;
};
auto f2 = [](real x) -> real
{
    return std::sin(x);
};
auto f3 = [](real x) -> real
{
    return 1 / (1 + std::pow(x, 2));
};

//second order derivatives
auto d2f1 = [](real x) -> real
{
    return 6 * x + 2;
};
auto d2f2 = [](real x) -> real
{
    return -std::sin(x);
};
auto d2f3 = [](real x) -> real
{
    return 2 / std::pow(1 + x * x, 2) * (4 * x * x / (1 + x * x) - 1);
};

//function string views
std::string v1 = "x^3 + x^2 + x + 1",
            v2 = "sin(x)",
            v3 = "1 / (1 + x^2)";

//split names
std::string su = "uniform split",
            sc = "Chebyshev split";

template<typename S, typename F, typename D2F>
auto generate(std::size_t n, const S& split, const F& f, const D2F& d2f)
{
    auto x = split(a, b, n);
    auto y = num::apply(x, f);
    auto p = num::cubic_spline(x, y, d2f(x[0]), d2f(x[n]));
    return std::make_tuple(x, y, p);
}

template<typename F, typename P>
auto subdivide(const std::valarray<real>& x, const F& f, const P& p)
{
    auto x_sub = num::subsplit(x, k);
    auto y_sub = num::apply(x_sub, f);
    auto p_sub = num::apply(x_sub, p);
    return std::make_tuple(x_sub, y_sub, p_sub);
}

template<typename F, typename D2F, typename H, typename R>
void table(const F& f, const D2F& d2f,
           const H& head, const R& row, const std::string& view)
{
    //head
    std::cout << std::endl
              << " Function: " << view << std::endl
              << " Range: [" << std::to_string(a) << "; " << std::to_string(b) << "]" << std::endl
              << " Subsegments per interpolation segment: " << std::to_string(k) << std::endl
              << std::endl;
    head();

    for (std::size_t n = 2; n <= N; /*n++*/ n += (n / 10 + 1))
    {
        //original data
        auto [xu, yu, pu] = generate(n, num::uniform<real>, f, d2f);
        auto [xc, yc, pc] = generate(n, num::chebyshev<real>, f, d2f);

        //subdivided data
        auto [xu_sub, yu_sub, pu_sub] = subdivide(xu, f, pu);
        auto [xc_sub, yc_sub, pc_sub] = subdivide(xc, f, pc);

        //error data
        row(n, num::error(yu_sub, pu_sub).max(), num::error(yc_sub, pc_sub).max());
    }

    std::cout << std::endl;
}

void table_mode()
{
    //table format
    std::string head_uni = "UNIFORM SPLIT ERROR",
                head_chb = "CHEBYSHEV SPLIT ERROR",
                sep = " | ";
    std::size_t real_places = num::format<real>(std::cout, false),
                width_size  = std::log10(std::numeric_limits<std::size_t>::max()) + 1,
                width_uni   = std::max(real_places, head_uni.size()),
                width_chb   = std::max(real_places, head_chb.size());
    auto head = [&width_size, &width_uni, &width_chb, &sep, &head_uni, &head_chb]()
    {
        std::cout << std::setw(width_size) << "N" << sep
                  << std::setw(width_uni)  << head_uni << sep
                  << std::setw(width_chb)  << head_chb
                  << std::endl;
        std::cout << std::setfill('_') << ' '
                  << std::setw(width_size + 1) << "|"
                  << std::setw(width_uni + 3)  << "|"
                  << std::setw(width_chb + 1)  << "_"
                  << std::endl;
        std::cout << std::setfill(' ')
                  << std::setw(width_size + 3) << sep
                  << std::setw(width_uni + 3)  << sep
                  << std::setw(width_chb + 3)
                  << std::endl;
    };
    auto row = [&width_size, &width_uni, &width_chb, &sep](std::size_t size, real uni, real chb)
    {
        std::cout << std::setw(width_size) << std::to_string(size) << sep
                  << std::setw(width_uni) << uni << sep
                  << std::setw(width_chb) << chb
                  << std::endl;
    };

    //tables
    table(f1, d2f1, head, row, v1);
    table(f2, d2f2, head, row, v2);
    table(f3, d2f3, head, row, v3);
}

std::vector<real> vec(const std::valarray<real>& val)
{
    return { begin(val), end(val) };
}

template<typename F, typename D2F, typename S>
void plot(const F& f, const D2F& d2f, const S& split, std::size_t n,
          const std::string& split_name, std::size_t i)
{
    //data
    auto [x, y, p] = generate(n, split, f, d2f);
    auto [x_sub, y_sub, p_sub] = subdivide(x, f, p);
    auto diff = num::error(y_sub, p_sub);

    auto vec_x_sub = vec(x_sub);

    matplot::subplot(2, 2, i);
    matplot::title(split_name);

    matplot::plot(vec_x_sub, vec(y_sub))
            ->line_width(3)
            .color(matplot::color::red);

    matplot::hold(true);

    matplot::plot(vec_x_sub, vec(p_sub))
            ->line_width(3)
            .color(matplot::color::green);

    matplot::scatter(vec(x), vec(y))
            ->marker_face(true)
            .marker_color(matplot::color::none)
            .marker_face_color(matplot::color::blue)
            .marker_size(8);

    matplot::subplot(2, 2, i + 2);

    matplot::plot(vec_x_sub, vec(diff))
            ->line_width(3)
            .color(matplot::color::magenta);
}

void plot_mode()
{
    int choice;

    //window size
    std::size_t width = 1600, height = 1200;

    auto show = [&width, &height](const auto& f, const auto& d2f, const std::string& v)
    {
        std::size_t n;
        std::cout << "Enter n: ";
        std::cin >> n;
        std::cout << std::endl;

        auto fig = matplot::figure();
        fig->size(width, height);
        fig->name(v);

        plot(f, d2f, num::uniform<real>, n, su, 0);
        plot(f, d2f, num::chebyshev<real>, n, sc, 1);
        matplot::show();
    };

    while (true)
    {
        std::cout << std::endl;
        std::cout << "Choose function:" << std::endl
                  << "1.\t" << v1 << std::endl
                  << "2.\t" << v2 << std::endl
                  << "3.\t" << v3 << std::endl;
        std::cin >> choice;
        std::cout << std::endl;

        switch (choice)
        {
            case 1:
                show(f1, d2f1, v1);
                break;
            case 2:
                show(f2, d2f2, v2);
                break;
            case 3:
                show(f3, d2f3, v3);
                break;
            default:
                return;
        }
    }
}

int main()
{
    int choice;
    while (true)
    {
        std::cout << "Choose mode:" << std::endl
                  << "1.\tError table mode" << std::endl
                  << "2.\tPlot mode" << std::endl;
        std::cin >> choice;
        std::cout << std::endl;

        switch (choice)
        {
            case 1:
                table_mode();
                break;
            case 2:
                plot_mode();
                break;
            default:
                return 0;
        }
    }
}
