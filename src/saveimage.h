#ifndef SAVEIMAGE_H
#define SAVEIMAGE_H

#include <cstdint>
#include <string>
#include <vector>

bool saveImage(
  const std::string& filename,
  const std::vector<uint32_t>& pixels,
  int inputSize,
  int outputSize
);

#endif