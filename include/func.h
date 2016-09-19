#pragma once

#include "io.h"
#include <algorithm>
#include <cmath>

//*** MACRO block ***

#define sqr(x) ((x) * (x))

#define _colorSet(pixel, color, value) std::get<color>(pixel) = uint(value)
#define colorSet(pixel, color, value)  _colorSet(pixel, color, std::min(255, std::max(0, int(value))))

#define colorGet(pixel, color) uint(std::get<color>(pixel))
#define levelGet(pixel)        colorGet(pixel, 0) 

#define icolorGet(pixel, color) int(colorGet(pixel, color))
#define ilevelGet(pixel)        int(levelGet(pixel))

#define colorMul(pixel, color, value) colorSet(pixel, color, round(colorGet(pixel, color) * (value)))

//*** End ***

using Position = std::pair< uint, uint >;
using Shift    = std::pair< std::pair<int, int>, std::pair<int, int> >;

enum Metrics {MSE, CCORR};
enum Colors {RED, GREEN, BLUE};

const uint MAX_LVL = 256;

static const int    MAX_SHIFT = 15;
static const uint   INF  = uint(1e9);
static const double INFD = 1e14;
static const double EPS  = 1e-8;

void pixelMul(Pixel& pixel, double valR, double valG, double valB);
void pixelMul(Pixel& pixel, double value);

Shift bestShift(const Image& R, const Image& G, const Image& B, const Metrics mtype);
Image unit     (const Image& R, const Image& G, const Image& B, const Shift& shift);
