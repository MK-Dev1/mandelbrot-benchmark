#ifndef RENDERLINE_H
#define RENDERLINE_H

#include "config.h"
#include "color.h"

void renderLine(std::vector<uint32_t>& pixels, int y, int size, Real centerX, Real centerY, Real scale);

#endif