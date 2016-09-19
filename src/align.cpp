#include "align.h"
#include "operators.h"

#include <string>
#include <cmath>
#include <algorithm>
#include <cassert>

using std::string;
using std::min;
using std::cout;
using std::endl;

Image align(Image srcImage, bool isPostprocessing, std::string postprocessingType, double fraction, bool isMirror, 
            bool isInterp, bool isSubpixel, double subScale)
{
    uint w = srcImage.n_cols;
    uint h = srcImage.n_rows / 3;

    Image B = srcImage.submatrix(0, 0, h, w);
    Image G = srcImage.submatrix(h, 0, h, w);
    Image R = srcImage.submatrix(h * 2, 0, h, w);

    // metrics: MSE or CCORR
    Image result = unit(R, G, B, bestShift(R, G, B, MSE));

    if (isPostprocessing) {
        if (postprocessingType == "--gray-world") {
            result = gray_world(result);
        } else if (postprocessingType == "--unsharp") {
            result = unsharp(result);
        }
    }

    return result;
}

Image sobel_x(Image src_image) {
    Matrix<double> kernel = {{-1, 0, 1},
                             {-2, 0, 2},
                             {-1, 0, 1}};
    return custom(src_image, kernel);
}

Image sobel_y(Image src_image) {
    Matrix<double> kernel = {{ 1,  2,  1},
                             { 0,  0,  0},
                             {-1, -2, -1}};
    return custom(src_image, kernel);
}

Image unsharp(Image src_image) {
    Matrix<double> kernel = {{-1/6., -2/3., -1/6.},
                             {-2/3., 13/3., -2/3.},
                             {-1/6., -2/3., -1/6.}};
    return custom(src_image, kernel);
}

Image gray_world(Image srcImage) {
    double Rsum = 0, Gsum = 0, Bsum = 0;

    for (uint i = 0; i < srcImage.n_rows; ++i) {
        for (uint j = 0; j < srcImage.n_cols; ++j) {
            Rsum += colorGet(srcImage(i, j), RED);
            Gsum += colorGet(srcImage(i, j), GREEN);
            Bsum += colorGet(srcImage(i, j), BLUE);
        }
    }

    double sum = (Rsum + Gsum + Bsum) / 3;
    for (uint i = 0; i < srcImage.n_rows; ++i) {
        for (uint j = 0; j < srcImage.n_cols; ++j) {
            pixelMul(srcImage(i, j), sum / Rsum, sum / Gsum, sum / Bsum);
        }
    }

    return srcImage;
}

Image resize(Image src_image, double scale) {
    return src_image;
}

Image custom(Image srcImage, Matrix<double> kernel) {
    ConvolutionOperator op(kernel);
    return srcImage.unary_map(op);
}

Image autocontrast(Image src_image, double fraction) {
    return src_image;
}

Image gaussian(Image src_image, double sigma, int radius)  {
    return src_image;
}

Image gaussian_separable(Image src_image, double sigma, int radius) {
    return src_image;
}

Image median(Image srcImage, int radius) {
    MedianOperator op(radius);
    return srcImage.unary_map(op);
}

Image median_linear(Image src_image, int radius) { 
    
    return src_image;
}

Image median_const(Image src_image, int radius) {
    return src_image;
}

Image canny(Image src_image, int threshold1, int threshold2) {
    return src_image;
}
