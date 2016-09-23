#pragma once

#include "io.h"
#include <algorithm>
#include <cmath>

//*** MACRO block ***

#define sqr(x) ((x) * (x))

#define _colorSet(pixel, color, value) std::get<color>(pixel) = uint(value)
#define colorSet(pixel, color, value)  _colorSet(pixel, color, std::min(255, std::max(0, int(value))))
#define levelSet(pixel, value) \
    colorSet(pixel, RED, value), \
    colorSet(pixel, GREEN, value), \
    colorSet(pixel, BLUE, value)
    

#define colorGet(pixel, color) int(std::get<color>(pixel))
#define levelGet(pixel)        colorGet(pixel, 0) 

#define colorMul(pixel, color, value) colorSet(pixel, color, round(colorGet(pixel, color) * (value)))

//*** End ***

using uintl    = long long;
using dblong   = long double;

using Size        = std::pair< uint, uint >;
using Position    = std::pair< uint, uint >;
using SingleShift = std::pair< int, int >;
using Shift       = std::pair< SingleShift, SingleShift >;

enum Metrics  {MSE, CCORR};
enum Colors   {RED, GREEN, BLUE};
enum GradType {EMPTY, WEAK, STRONG};

const uint MAX_LVL = 256;
const uint DEF_CLR = 0;

static const int    MAX_SHIFT = 15;
static const uint   INF  = uint(1e9);
static const uintl  INFL = uintl(1e14);
static const double EPS  = 1e-8;
static const double FAKE_BRD = 0.025; // * 100%

void weakSetGradType(Matrix <GradType> gradMap, Matrix <bool> used, const Size size, const Position pos);

void  pixelMul(Pixel& pixel, double valR, double valG, double valB);
void  pixelMul(Pixel& pixel, double value);
Pixel pixelSum(const Image image);

uint  getMedianLvl  (const uint (&count)[MAX_LVL], const uint median);
Pixel getMedianPixel(const uint (&count)[3][MAX_LVL], const uint median);

Shift bestShift(const Image& R, const Image& G, const Image& B, const Metrics mtype);
Image unit     (const Image& R, const Image& G, const Image& B, const Shift& shift);
