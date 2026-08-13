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
#include "saveimage.h"



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

    if (!saveImage(
            "mandelbrot_output.png",
            pixels,
            size,
            outputSize))
    {
        std::cerr << "Cant save mandelbrot_output.png\n";
        return 1;
    }

    std::cout << "Saved mandelbrot_output.png\n";
  }

  return 0;
}
