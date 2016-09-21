#include "align.h"
#include "operators.h"
#include "other_types.h"

#include <string>
#include <vector>
#include <cmath>
#include <algorithm>
#include <cassert>

using std::string;
using std::max;
using std::min;
using std::tie;
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
    Counter counter;
    Image result(srcImage.n_rows, srcImage.n_cols);

    for (uint x = 0; x < srcImage.n_rows; ++x) {
        for (uint y = 0; y < srcImage.n_cols; ++y) {
            uint xLen = min(uint(radius), x) + 1 + min(uint(radius), (srcImage.n_rows - 1) - x);
            uint yLen = min(uint(radius), y) + 1 + min(uint(radius), (srcImage.n_cols - 1) - y);

            counter.update(srcImage.submatrix(max(0, int(x) - radius), max(0, int(y) - radius), xLen, yLen));
            result(x, y) = counter.getMedianPixel();
            counter.reset();
        }
    }
    
    return result;
}

Image median_linear(Image srcImage, int radius) { 
    static Counter counter;
    Image result(srcImage.n_rows, srcImage.n_cols);

    for (uint x = 0; x < srcImage.n_rows; ++x) {
        uint xLen = min(uint(radius), x) + 1 + min(uint(radius), (srcImage.n_rows - 1) - x);

        for (uint y = 0; y < uint(radius); ++y) {
            counter.update(srcImage.submatrix(max(0, int(x) - radius), y, xLen, 1));
        }

        for (uint y = 0; y < srcImage.n_cols; ++y) {
            if (y + radius < srcImage.n_cols) {
                counter.update(srcImage.submatrix(max(0, int(x) - radius), y + radius, xLen, 1));
            }
            result(x, y) = counter.getMedianPixel();

            if (int(y) - radius >= 0) {
                counter.update(srcImage.submatrix(max(0, int(x) - radius), y - radius, xLen, 1), true);
            }
        }

        counter.reset();
    }

    return result;
}

Image median_const(Image srcImage, int radius) {
    static Counter counter;

    std::vector <Counter> hist(srcImage.n_cols);
    Image result(srcImage.n_rows, srcImage.n_cols);

    for (uint y = 0; y < srcImage.n_cols; ++y) {
        hist[y].update(srcImage.submatrix(0, y, radius, 1));
    }

    for (uint x = 0; x < srcImage.n_rows; ++x) {
        for (uint y = 0; y < uint(radius); ++y) {
            if (x + radius < srcImage.n_rows) {
                hist[y].update(srcImage(x + radius, y));
            }
            counter.update(hist[y]);
        }

        for (uint y = 0; y < srcImage.n_cols; ++y) {
            if (y + radius < srcImage.n_cols) {
                if (x + radius < srcImage.n_rows) {
                    hist[y + radius].update(srcImage(x + radius, y + radius));
                }
                counter.update(hist[y + radius]);
            }

            result(x, y) = counter.getMedianPixel();

            if (int(y) - radius >= 0) {
                counter.update(hist[y - radius], true);
                if (int(x) - radius >= 0) {
                    hist[y - radius].update(srcImage(x - radius, y - radius), true);
                }
            }
        }

        for (uint dy = 0; dy < uint(radius); ++dy) {
            uint y = srcImage.n_cols - dy - 1;
            if (int(x) - radius >= 0) {
                hist[y].update(srcImage(x - radius, y), true);
            }
        }

        counter.reset();
    }

    return result;
}

Image canny(Image src_image, int threshold1, int threshold2) {
    return src_image;
}
