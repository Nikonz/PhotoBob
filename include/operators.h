#pragma once

#include "matrix.h"
#include "io.h"

class ConvolutionOperator {
public:
    const uint radius;

    ConvolutionOperator(Matrix<double>& _kernel);
    ~ConvolutionOperator();

    Pixel operator() (Image subImage) const;

private:
    const Matrix<double> kernel;
    double sum = 0;
};

