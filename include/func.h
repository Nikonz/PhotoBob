#pragma once

#include "io.h"

using Shift = std::pair< std::pair<int, int>, std::pair<int, int> >;

enum Metrics {MSE, CCORR};
enum Colors {RED, GREEN, BLUE};

static const int    MAX_SHIFT = 15;
static const uint   INF  = uint(1e9);
static const double INFD = 1e14;
static const double EPS  = 1e-8;

Shift bestShift(const Image& A, const Image& B, const Image& C, Metrics   mtype);
Image unit     (const Image& R, const Image& G, const Image& B, Shift shift);
