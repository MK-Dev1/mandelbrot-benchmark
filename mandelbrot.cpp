#include <iostream>
#include <vector>
#include <cmath>
#include <chrono>
#include <fstream>
#include <cstdint>
#include <thread>
#include <atomic>

#include <boost/multiprecision/cpp_dec_float.hpp>


// =====================================================
// TYP LICZB
// =====================================================

using Real = boost::multiprecision::cpp_dec_float_100;


// =====================================================
// PARAMETRY
// =====================================================

int MAX_ITER = 50;


// =====================================================
// KOLOR
// =====================================================

void getColor(
    int i,
    uint8_t& r,
    uint8_t& g,
    uint8_t& b
)
{
    if (i >= MAX_ITER)
    {
        r = 0;
        g = 0;
        b = 0;

        return;
    }

    Real t = (Real)i / MAX_ITER;

    r = static_cast<uint8_t>(
        127 + 128 *
        boost::multiprecision::sin(t * 10)
    );

    g = static_cast<uint8_t>(
        127 + 128 *
        boost::multiprecision::sin(t * 10 + 2)
    );

    b = static_cast<uint8_t>(
        127 + 128 *
        boost::multiprecision::sin(t * 10 + 4)
    );
}


// =====================================================
// RENDEROWANIE JEDNEJ LINII
// =====================================================

void renderLine(
    std::vector<uint32_t>& pixels,
    int y,
    int size,
    Real centerX,
    Real centerY,
    Real scale
)
{
    for (int x = 0; x < size; x++)
    {
        Real cx =
            centerX +
            (Real(x) - Real(size) / 2) *
            scale / size;

        Real cy =
            centerY +
            (Real(y) - Real(size) / 2) *
            scale / size;

        Real zx = 0;
        Real zy = 0;

        int i = 0;

        while (
            zx * zx +
            zy * zy <= 4 &&
            i < MAX_ITER
        )
        {
            Real xx =
                zx * zx -
                zy * zy +
                cx;

            zy =
                2 * zx * zy +
                cy;

            zx = xx;

            i++;
        }

        uint8_t r;
        uint8_t g;
        uint8_t b;

        getColor(
            i,
            r,
            g,
            b
        );

        pixels[
            static_cast<size_t>(y) *
            size +
            x
        ] =
            (255u << 24) |
            (uint32_t(r) << 16) |
            (uint32_t(g) << 8) |
            uint32_t(b);
    }
}


// =====================================================
// MAIN
// =====================================================

int main(int argc, char* argv[])
{
    // -------------------------------------------------
    // Sprawdzenie liczby argumentów
    // -------------------------------------------------

    if (argc != 6)
    {
        std::cerr
            << "Uzycie:\n"
            << argv[0]
            << " <rdzenie> <rozmiar> <iteracje> "
               "<zapis> <podglad>\n\n"

            << "Przyklad:\n"
            << argv[0]
            << " 8 600 150 1 100\n";

        return 1;
    }


    // -------------------------------------------------
    // Parametry
    // -------------------------------------------------

    int threadCount;
    int size;
    int iterations;
    int save;
    int previewSize;


    try
    {
        threadCount =
            std::stoi(argv[1]);

        size =
            std::stoi(argv[2]);

        iterations =
            std::stoi(argv[3]);

        save =
            std::stoi(argv[4]);

        previewSize =
            std::stoi(argv[5]);
    }
    catch (...)
    {
        std::cerr
            << "Blad: nieprawidlowe argumenty.\n";

        return 1;
    }


    // -------------------------------------------------
    // Walidacja
    // -------------------------------------------------

    if (threadCount <= 0)
    {
        std::cerr
            << "Liczba rdzeni musi byc > 0.\n";

        return 1;
    }


    if (size <= 0)
    {
        std::cerr
            << "Rozmiar musi byc > 0.\n";

        return 1;
    }


    if (iterations <= 0)
    {
        std::cerr
            << "Liczba iteracji musi byc > 0.\n";

        return 1;
    }


    if (save != 0 && save != 1)
    {
        std::cerr
            << "Parametr zapisu musi byc 0 albo 1.\n";

        return 1;
    }


    if (previewSize <= 0)
    {
        std::cerr
            << "Rozmiar podgladu musi byc > 0.\n";

        return 1;
    }


    // Jeżeli użytkownik poda więcej wątków
    // niż jest linii, nie ma sensu ich tworzyć.

    if (threadCount > size)
        threadCount = size;


    MAX_ITER = iterations;


    // -------------------------------------------------
    // Bufor obrazu
    // -------------------------------------------------

    std::vector<uint32_t> pixels(
        static_cast<size_t>(size) *
        static_cast<size_t>(size)
    );


    // -------------------------------------------------
    // Parametry Mandelbrota
    // -------------------------------------------------

    Real centerX = Real("-0.5");
    Real centerY = Real("0");
    Real scale   = Real("3");


    // =================================================
    // RENDEROWANIE WIELU WĄTKACH
    // =================================================

    std::atomic<int> nextLine(0);

    std::vector<std::thread> workers;

    workers.reserve(threadCount);


    // -------------------------------------------------
    // START TIMERA
    // -------------------------------------------------

    auto start =
        std::chrono::high_resolution_clock::now();


    // -------------------------------------------------
    // Tworzenie wątków
    // -------------------------------------------------

    for (int t = 0; t < threadCount; t++)
    {
        workers.emplace_back(
            [&]()
            {
                while (true)
                {
                    int y =
                        nextLine.fetch_add(1);


                    if (y >= size)
                        break;


                    renderLine(
                        pixels,
                        y,
                        size,
                        centerX,
                        centerY,
                        scale
                    );
                }
            }
        );
    }


    // -------------------------------------------------
    // Czekamy na wszystkie wątki
    // -------------------------------------------------

    for (auto& worker : workers)
    {
        worker.join();
    }


    // -------------------------------------------------
    // KONIEC TIMERA
    // -------------------------------------------------

    auto end =
        std::chrono::high_resolution_clock::now();


    double seconds =
        std::chrono::duration<double>(
            end - start
        ).count();


    // =================================================
    // INFORMACJE
    // =================================================

    std::cout
        << "Rdzenie/watki: "
        << threadCount
        << "\n";


    std::cout
        << "Rozmiar: "
        << size
        << " x "
        << size
        << "\n";


    std::cout
        << "Punktow w jednej linii: "
        << size
        << "\n";


    std::cout
        << "Liczba punktow: "
        << static_cast<long long>(size) *
           static_cast<long long>(size)
        << "\n";


    std::cout
        << "Iteracje: "
        << iterations
        << "\n";


    std::cout
        << "Czas renderowania: "
        << seconds
        << " s\n";


    // =================================================
    // ZAPIS PODGLADU
    // =================================================

    if (save == 1)
    {
        int outputSize =
            previewSize;


        if (outputSize > size)
            outputSize = size;


        std::ofstream file(
            "mandelbrot_output.txt"
        );


        if (!file)
        {
            std::cerr
                << "Nie mozna otworzyc "
                << "mandelbrot_output.txt\n";

            return 1;
        }


        // -------------------------------------------------
        // Skalowanie po zatrzymaniu timera
        // -------------------------------------------------

        for (int py = 0;
             py < outputSize;
             py++)
        {
            int sourceY =
                py * size / outputSize;


            for (int px = 0;
                 px < outputSize;
                 px++)
            {
                int sourceX =
                    px * size / outputSize;


                uint32_t pixel =
                    pixels[
                        static_cast<size_t>(sourceY) *
                        size +
                        sourceX
                    ];


                uint8_t r =
                    (pixel >> 16) & 255;


                uint8_t g =
                    (pixel >> 8) & 255;


                uint8_t b =
                    pixel & 255;


                file
                    << static_cast<int>(r)
                    << " "
                    << static_cast<int>(g)
                    << " "
                    << static_cast<int>(b);


                if (px != outputSize - 1)
                    file << " ";
            }


            file << "\n";
        }


        file.close();


        std::cout
            << "Zapisano obraz: "
            << "mandelbrot_output.txt\n";


        std::cout
            << "Rozmiar podgladu: "
            << outputSize
            << " x "
            << outputSize
            << "\n";
    }


    return 0;
}
