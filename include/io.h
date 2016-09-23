#pragma once

#include "matrix.h"
#include "EasyBMP.h"

#include <tuple>

using Pixel = std::tuple <int, int, int>;
using Image = Matrix <Pixel>;

Image load_image(const char*);
void save_image(const Image&, const char*);
