#include "saveimage.h"

#include <cstdint>
#include <fstream>
#include <string>
#include <vector>

namespace
{
  uint32_t crc32(const std::vector<uint8_t>& data)
  {
    uint32_t crc = 0xFFFFFFFFu;

    for (uint8_t byte : data)
    {
      crc ^= byte;

      for (int i = 0; i < 8; ++i)
      {
        if (crc & 1u)
          crc = (crc >> 1) ^ 0xEDB88320u;
        else
          crc >>= 1;
      }
    }

    return crc ^ 0xFFFFFFFFu;
  }

  uint32_t adler32(const std::vector<uint8_t>& data)
  {
    const uint32_t MOD_ADLER = 65521u;

    uint32_t a = 1;
    uint32_t b = 0;

    for (uint8_t byte : data)
    {
      a = (a + byte) % MOD_ADLER;
      b = (b + a) % MOD_ADLER;
    }

    return (b << 16) | a;
  }

  void writeUint32(std::ofstream& file, uint32_t value)
  {
    uint8_t bytes[4];

    bytes[0] = static_cast<uint8_t>((value >> 24) & 0xFF);
    bytes[1] = static_cast<uint8_t>((value >> 16) & 0xFF);
    bytes[2] = static_cast<uint8_t>((value >> 8) & 0xFF);
    bytes[3] = static_cast<uint8_t>(value & 0xFF);

    file.write(
      reinterpret_cast<const char*>(bytes),
      4
    );
  }

  void writeChunk(
    std::ofstream& file,
    const char type[4],
    const std::vector<uint8_t>& data)
  {
    writeUint32(
      file,
      static_cast<uint32_t>(data.size())
    );

    file.write(type, 4);

    if (!data.empty())
    {
      file.write(
        reinterpret_cast<const char*>(data.data()),
        static_cast<std::streamsize>(data.size())
      );
    }

    std::vector<uint8_t> crcData;

    crcData.reserve(4 + data.size());

    for (int i = 0; i < 4; ++i)
      crcData.push_back(
        static_cast<uint8_t>(type[i])
      );

    for (uint8_t byte : data)
      crcData.push_back(byte);

    writeUint32(file, crc32(crcData));
  }
}

bool saveImage(
  const std::string& filename,
  const std::vector<uint32_t>& pixels,
  int inputSize,
  int outputSize)
{
  if (inputSize <= 0)
    return false;

  if (outputSize <= 0)
    return false;

  if (outputSize > inputSize)
    return false;

  const size_t expectedPixels =
    static_cast<size_t>(inputSize) *
    static_cast<size_t>(inputSize);

  if (pixels.size() < expectedPixels)
    return false;

  std::ofstream file(filename, std::ios::binary);

  if (!file)
    return false;

  // PNG signature
  const uint8_t pngSignature[8] =
  {
    137, 80, 78, 71,
    13, 10, 26, 10
  };

  file.write(
    reinterpret_cast<const char*>(pngSignature),
    8
  );

  // IHDR
  std::vector<uint8_t> ihdr(13);

  // Width
  ihdr[0] = static_cast<uint8_t>(
    (outputSize >> 24) & 0xFF
  );

  ihdr[1] = static_cast<uint8_t>(
    (outputSize >> 16) & 0xFF
  );

  ihdr[2] = static_cast<uint8_t>(
    (outputSize >> 8) & 0xFF
  );

  ihdr[3] = static_cast<uint8_t>(
    outputSize & 0xFF
  );

  // Height
  ihdr[4] = static_cast<uint8_t>(
    (outputSize >> 24) & 0xFF
  );

  ihdr[5] = static_cast<uint8_t>(
    (outputSize >> 16) & 0xFF
  );

  ihdr[6] = static_cast<uint8_t>(
    (outputSize >> 8) & 0xFF
  );

  ihdr[7] = static_cast<uint8_t>(
    outputSize & 0xFF
  );

  // Bit depth
  ihdr[8] = 8;

  // Color type: RGBA
  ihdr[9] = 6;

  // Compression
  ihdr[10] = 0;

  // Filter
  ihdr[11] = 0;

  // Interlace
  ihdr[12] = 0;

  writeChunk(file, "IHDR", ihdr);

  // raw image data

  std::vector<uint8_t> rawData;

  const size_t rowSize = 1 + static_cast<size_t>(outputSize) * 4;

  rawData.reserve(
    static_cast<size_t>(outputSize) * rowSize
  );

  for (int y = 0; y < outputSize; ++y)
  {
    // PNG filter: none
    rawData.push_back(0);

    int sourceY = static_cast<int>(static_cast<long long>(y) * inputSize / outputSize);

    for (int x = 0; x < outputSize; ++x)
    {
      int sourceX = static_cast<int>(static_cast<long long>(x) * inputSize / outputSize);

      size_t index = static_cast<size_t>(sourceY) * static_cast<size_t>(inputSize) + static_cast<size_t>(sourceX);

      uint32_t pixel = pixels[index];

      // Pixels are stored as AARRGGBB.
      uint8_t a = static_cast<uint8_t>((pixel >> 24) & 0xFF);

      uint8_t r = static_cast<uint8_t>((pixel >> 16) & 0xFF);

      uint8_t g = static_cast<uint8_t>((pixel >> 8) & 0xFF);

      uint8_t b = static_cast<uint8_t>(pixel & 0xFF);

      rawData.push_back(r);
      rawData.push_back(g);
      rawData.push_back(b);
      rawData.push_back(a);
    }
  }

  // ZLIB stream
  std::vector<uint8_t> compressed;

  // header
  compressed.push_back(0x78);
  compressed.push_back(0x01);

  size_t position = 0;

  while (position < rawData.size())
  {
    size_t remaining = rawData.size() - position;

    size_t currentSize = remaining > 65535 ? 65535 : remaining;

    uint16_t blockSize = static_cast<uint16_t>(currentSize);

    bool finalBlock = (position + currentSize == rawData.size());

    // BFINAL + BTYPE=00
    compressed.push_back(finalBlock ? 0x01 : 0x00);

    uint16_t length = blockSize;

    uint16_t invertedLength = static_cast<uint16_t>(~length);

    compressed.push_back(static_cast<uint8_t>(length & 0xFF));

    compressed.push_back(static_cast<uint8_t>((length >> 8) & 0xFF));

    compressed.push_back(static_cast<uint8_t>(invertedLength & 0xFF));

    compressed.push_back(static_cast<uint8_t>((invertedLength >> 8) & 0xFF));



    for (size_t i = 0; i < currentSize; ++i)
    {
      compressed.push_back(rawData[position + i]);
    }

    position += currentSize;
  }

  // Adler-32
  uint32_t checksum = adler32(rawData);

  compressed.push_back(static_cast<uint8_t>((checksum >> 24) & 0xFF));

  compressed.push_back(static_cast<uint8_t>((checksum >> 16) & 0xFF));

  compressed.push_back(static_cast<uint8_t>((checksum >> 8) & 0xFF));

  compressed.push_back(static_cast<uint8_t>(checksum & 0xFF));

  writeChunk(file, "IDAT", compressed);

  
  // IEND
  std::vector<uint8_t> empty;

  writeChunk(file, "IEND", empty);

  return file.good();
}