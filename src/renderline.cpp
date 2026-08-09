#include "config.h"
#include "color.h"
#include "renderline.h"

void renderLine(std::vector<uint32_t>& pixels, int y, int size, Real centerX, Real centerY, Real scale)
{
  for (int x = 0; x < size; x++)
  {
    Real cx = centerX + (Real(x) - Real(size) / 2) * scale / size;
    Real cy = centerY + (Real(y) - Real(size) / 2) * scale / size;
    
    Real zx = 0;
    Real zy = 0;

    int i = 0;

    while (zx * zx + zy * zy <= 4 && i < MAX_ITER)
    {
      Real xx = zx * zx - zy * zy + cx;
      zy = 2 * zx * zy + cy;
      zx = xx;
      i++;
    }

    uint8_t r;
    uint8_t g;
    uint8_t b;

    getColor(i, r, g, b);
    pixels[static_cast<size_t>(y) * size + x] =
      (255u << 24) |
      (uint32_t(r) << 16) |
      (uint32_t(g) << 8) |
      uint32_t(b);
  }
}