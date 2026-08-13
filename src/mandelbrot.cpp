#include <iostream>
#include <vector>
#include <cmath>
#include <chrono>
#include <fstream>
#include <cstdint>
#include <thread>
#include <atomic>

#include "config.h"
#include "renderline.h"


int main(int argc, char* argv[])
{
  if (argc != 6)
  {
    std::cerr
      << "USAGE:\n"
      << argv[0]
      << " <cores> <size> <max iterations> <save render preview> <preview size>\n\n"
      << "Example usage:\n"
      << argv[0]
      << "./mandelbrot 8 600 150 1 100\n"
      << "600x600 image rendered on 8 cores with 150 max iterations"
      << "preview will be saved with 100x100 resolution.";
      return 1;
  }

  int threadCount;
  int size;
  int iterations;
  int save;
  int previewSize;

  try
  {
    threadCount = std::stoi(argv[1]);

    size = std::stoi(argv[2]);

    iterations = std::stoi(argv[3]);

    save = std::stoi(argv[4]);

    previewSize = std::stoi(argv[5]);
  }
  catch (...)
  {
    std::cerr << "Error: incorrect arguments.\n";
    return 1;
  }

  if (threadCount <= 0)
  {
    std::cerr << "Number of cores must be > 0.\n";
    return 1;
  }


  if (size <= 0)
  {
    std::cerr << "Size must be > 0.\n";
    return 1;
  }


  if (iterations <= 0)
  {
    std::cerr << "Iteration count must be > 0.\n";
    return 1;
  }


  if (save != 0 && save != 1)
  {
    std::cerr << "Save parameter must be 0 or 1.\n";
    return 1;
  }


  if (previewSize <= 0)
  {
    std::cerr << "Preview size must be > 0.\n";
    return 1;
  }

  if (threadCount > size)
    threadCount = size;


  MAX_ITER = iterations;

  std::vector<uint32_t> pixels(static_cast<size_t>(size) * static_cast<size_t>(size));

  Real centerX = Real("-0.5");
  Real centerY = Real("0");
  Real scale   = Real("3");

  std::atomic<int> nextLine(0);

  std::vector<std::thread> workers;

  workers.reserve(threadCount);

  auto start = std::chrono::high_resolution_clock::now();

  for (int t = 0; t < threadCount; t++)
  {
    workers.emplace_back([&]()
      {
        while (true)
          {
            int y = nextLine.fetch_add(1);

            if (y >= size)
              break;


            renderLine(pixels, y, size, centerX, centerY, scale);
          }
      }
    );
  }


  for (auto& worker : workers)
  {
    worker.join();
  }


  auto end = std::chrono::high_resolution_clock::now();

  double seconds = std::chrono::duration<double>(end - start).count();

  std::cout << "Render time: " << seconds << " seconds\n";

  std::cout << "Score: " << 7500/seconds << "\n";


  if (save == 1)
  {
    int outputSize = previewSize;

    if (outputSize > size)
      outputSize = size;

    std::ofstream file("mandelbrot_output.txt");

    if (!file)
    {
      std::cerr << "Cant open " << "mandelbrot_output.txt\n";
      return 1;
    }

    for (int py = 0; py < outputSize; py++)
    {
      int sourceY = py * size / outputSize;

      for (int px = 0; px < outputSize; px++)
      {
        int sourceX = px * size / outputSize;

        uint32_t pixel = pixels[static_cast<size_t>(sourceY) * size + sourceX];

        uint8_t r = (pixel >> 16) & 255;

        uint8_t g = (pixel >> 8) & 255;

        uint8_t b = pixel & 255;

        file << static_cast<int>(r) << " " << static_cast<int>(g) << " " << static_cast<int>(b);

        if (px != outputSize - 1)
          file << " ";
      }

      file << "\n";
    }


    file.close();
  }
  return 0;
}
