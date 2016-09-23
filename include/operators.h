#pragma once

#include "matrix.h"
#include "io.h"
#include <cmath>

#define value first
#define angle second

using Gradient = std::pair<double, double>;

class ConvolutionOperator {
public:
    const uint radius;

    ConvolutionOperator(Matrix<double>& kernel, bool normalize, bool bounds);
    ~ConvolutionOperator();

    Pixel operator() (Image subImage) const;

private:
    Matrix<double> kernel;
    const bool bounds;
};

class RemoveNoMaxOperator {
public:
    static const uint radius = 1;

    RemoveNoMaxOperator();
    ~RemoveNoMaxOperator();

    Gradient operator() (Matrix<Gradient> grad);
};
