#include "operators.h"
#include "func.h"
#include <cassert>

ConvolutionOperator::ConvolutionOperator(Matrix<double>& _kernel) :
    radius(_kernel.n_rows / 2), kernel(_kernel.deep_copy()), sum(0)
{
    assert(kernel.n_rows == kernel.n_cols);
    assert(kernel.n_rows & 1);

    for (uint i = 0; i < kernel.n_rows; ++i) {
        for (uint j = 0; j < kernel.n_cols; ++j) {
            sum += kernel(i, j);
        }
    }
}

ConvolutionOperator::~ConvolutionOperator() {};

Pixel ConvolutionOperator::operator() (Image subImage) const {
    double val[3] = {0, 0, 0};

    for (uint x = 0; x < subImage.n_rows; ++x) {
        for (uint y = 0; y < subImage.n_cols; ++y) {
            val[RED  ] += icolorGet(subImage(x, y), RED)   * kernel(x, y);
            val[GREEN] += icolorGet(subImage(x, y), GREEN) * kernel(x, y);
            val[BLUE ] += icolorGet(subImage(x, y), BLUE)  * kernel(x, y);
        }
    }

    return Pixel(val[RED] / sum, val[GREEN] / sum, val[BLUE] / sum);
}

MedianOperator::MedianOperator(uint _radius) : radius(_radius) {};

MedianOperator::~MedianOperator() {};

Pixel MedianOperator::operator() (Image subImage) const {
    uint count[3][MAX_LVL] = {{0}};

    for (uint x = 0; x < subImage.n_rows; ++x) {
        for (uint y = 0; y < subImage.n_cols; ++y) {
            ++count[RED  ][colorGet(subImage(x, y), RED  )];
            ++count[GREEN][colorGet(subImage(x, y), GREEN)];
            ++count[BLUE ][colorGet(subImage(x, y), BLUE )];
        }
    }

    uint result[3];

    uint med = sqr(2 * radius + 1) / 2;
    for (uint color = RED; color <= BLUE; ++color) {
        uint sum = 0;
        for (uint lvl = 0; lvl < MAX_LVL; ++lvl) {
            sum += count[color][lvl];
            if (sum > med) {
                result[color] = lvl;
                break;
            }
        }
    }
    
    return Pixel(result[RED], result[GREEN], result[BLUE]);
}






