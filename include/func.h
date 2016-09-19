#pragma once

#include "io.h"

using Shift = std::pair  < std::pair<int, int>, std::pair<int, int> >;

#define colorSet(pixel, color, value) std::get<color>(pixel) = value
#define colorGet(pixel, color)        uint(std::get<color>(pixel))
#define levelGet(pixel)               colorGet(pixel, 0) 
#define lvlGet levelGet

enum Metrics {MSE, CCORR};
enum Colors {RED, GREEN, BLUE};

static const int    MAX_SHIFT = 15;
static const uint   INF  = uint(1e9);
static const double INFD = 1e14;
static const double EPS  = 1e-8;

Shift bestShift(const Image& A, const Image& B, const Image& C, const Metrics mtype);
Image unit     (const Image& R, const Image& G, const Image& B, const Shift& shift);
