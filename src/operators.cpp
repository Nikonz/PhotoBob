#include "operators.h"
#include "func.h"
#include "other_types.h"
#include <cassert>
#include <cmath>

using std::min;
using std::max;

ConvolutionOperator::ConvolutionOperator(Matrix<double>& _kernel, bool normalize, bool _bounds) :
    radius(_kernel.n_rows / 2), kernel(_kernel.deep_copy()), bounds(_bounds)
{
    assert(kernel.n_rows == kernel.n_cols);
    assert(kernel.n_rows & 1);

    double sum = 0;
    for (uint i = 0; i < kernel.n_rows; ++i) {
        for (uint j = 0; j < kernel.n_cols; ++j) {
            sum += kernel(i, j);
        }
    }
    for (uint i = 0; i < kernel.n_rows; ++i) {
        for (uint j = 0; j < kernel.n_cols; ++j) {
            if (normalize) {
                kernel(i, j) = (kernel(i, j) / sum);
            }
        }
    }
}

ConvolutionOperator::~ConvolutionOperator() {};

Pixel ConvolutionOperator::operator() (Image subImage) const {
    double val[3] = {0, 0, 0};

    for (uint x = 0; x < subImage.n_rows; ++x) {
        for (uint y = 0; y < subImage.n_cols; ++y) {
            val[RED  ] += colorGet(subImage(x, y), RED)   * kernel(x, y);
            val[GREEN] += colorGet(subImage(x, y), GREEN) * kernel(x, y);
            val[BLUE ] += colorGet(subImage(x, y), BLUE)  * kernel(x, y);
        }
    }

    if (!bounds) {
        return Pixel(val[RED], val[GREEN], val[BLUE]);
    } else {
        return Pixel(max(0, min(int(round(val[RED  ])), 255)), 
                    max(0, min(int(round(val[GREEN])), 255)), 
                    max(0, min(int(round(val[BLUE ])), 255)));
    }
}

RemoveNoMaxOperator::RemoveNoMaxOperator() {};

RemoveNoMaxOperator::~RemoveNoMaxOperator() {};

Gradient RemoveNoMaxOperator::operator() (Matrix<Gradient> grad) {
    double ang = round(grad(1, 1).angle / (M_PI / 4)) * (M_PI / 4) - M_PI / 2;
    
    int dx = (cos(ang) > 0 ? 1 : -1);
    int dy = (sin(ang) > 0 ? 1 : -1);

    if (grad(1, 1).value > grad(1 - dx, 1 - dy).value &&
        grad(1, 1).value > grad(1 + dx, 1 + dy).value) {
        return grad(1, 1);
    } else {
        return std::make_pair(0, 0);
    }
}
