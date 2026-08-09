#include <cstdint>
#include <vector>
#include "config.h"
#include "color.h"

void getColor(int i, uint8_t& r, uint8_t& g, uint8_t& b)
{
  if (i >= MAX_ITER)
  {
    r = 0;
    g = 0;
    b = 0;
    return;
  }

  Real t = (Real)i / MAX_ITER;

  r = static_cast<uint8_t>(127 + 128 * boost::multiprecision::sin(t * 10));
  g = static_cast<uint8_t>(127 + 128 * boost::multiprecision::sin(t * 10 + 2));
  b = static_cast<uint8_t>(127 + 128 * boost::multiprecision::sin(t * 10 + 4));
}