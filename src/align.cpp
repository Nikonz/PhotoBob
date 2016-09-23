#include "align.h"
#include "operators.h"
#include "other_types.h"
#include "func.h"

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
        } else if (postprocessingType == "--autocontrast") {
            result = autocontrast(result, fraction);
        }
    }

    return result;
}

Image sobel_x(Image src_image) {
    Matrix<double> kernel = {{-1, 0, 1},
                             {-2, 0, 2},
                             {-1, 0, 1}};

    return custom(src_image, kernel, false, false);
}

Image sobel_y(Image src_image) {
    Matrix<double> kernel = {{ 1,  2,  1},
                             { 0,  0,  0},
                             {-1, -2, -1}};
    return custom(src_image, kernel, false, false);
}

Image discolor(Image srcImage) {
    Image result(srcImage.n_rows, srcImage.n_cols);
    
    for (uint x = 0; x < srcImage.n_rows; ++x) {
        for (uint y = 0; y < srcImage.n_cols; ++y) {
            levelSet(result(x, y), round(
                0.2125 * colorGet(srcImage(x, y), RED) +
                0.7154 * colorGet(srcImage(x, y), GREEN) +
                0.0721 * colorGet(srcImage(x, y), BLUE)
                                        ));
        }
    }
    return result;
}

Image unsharp(Image src_image) {
    Matrix<double> kernel = {{-1, -4, -1},
                             {-4, 26, -4},
                             {-1., -4, -1}};
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

Image custom(Image srcImage, Matrix<double> kernel, bool normalize, bool bounds) {
    ConvolutionOperator op(kernel, normalize, bounds);
    return srcImage.unary_map(op);
}

Image autocontrast(Image srcImage, double fraction) {
    Counter counter;
    counter.update(srcImage, ADD);

    Pixel pixMax = counter.getPixel(1 - fraction);
    Pixel pixMin = counter.getPixel(fraction);
    Pixel radius = pixelLinearDiff(pixMax, pixMin);

    Image result(srcImage.n_rows, srcImage.n_cols);
    for (uint x = 0; x < srcImage.n_rows; ++x) {
        for (uint y = 0; y < srcImage.n_cols; ++y) {
            Pixel diff = pixelLinearDiff(srcImage(x, y), pixMin);
            colorMul(diff, RED  , 255. / colorGet(radius, RED  ));
            colorMul(diff, GREEN, 255. / colorGet(radius, GREEN));
            colorMul(diff, BLUE , 255. / colorGet(radius, BLUE ));

            result(x, y) = diff;
            assert(colorGet(diff, RED) < 256);
        }
    }

    return result;
}

Image gaussian(Image srcImage, double sigma, int radius)  {
    Matrix<double> kernel(2 * radius + 1, 2 * radius + 1);

    for (int x = 0; x < 2 * radius + 1; ++x) {
        for (int y = 0; y < 2 * radius + 1; ++y) {
            kernel(x, y) = gaussElem(sigma, radius - x, radius - y);
        }
    }

    return custom(srcImage, kernel);
}

Image gaussian_separable(Image srcImage, double sigma, int radius) {
    Matrix<double> kerX(2 * radius + 1, 1);
    Matrix<double> kerY(1, 2 * radius + 1);

    for (int i = 0; i < 2 * radius + 1; ++i) {
        kerX(i, 0) = gaussElem(sigma, radius - i);
        kerY(0, i) = gaussElem(sigma, radius - i);
    }

    return custom(custom(srcImage, kerX), kerY);
}

Image median(Image srcImage, int radius) {
    Counter counter;
    Image result(srcImage.n_rows, srcImage.n_cols);

    for (uint x = 0; x < srcImage.n_rows; ++x) {
        for (uint y = 0; y < srcImage.n_cols; ++y) {
            uint xLen = min(uint(radius), x) + 1 + min(uint(radius), (srcImage.n_rows - 1) - x);
            uint yLen = min(uint(radius), y) + 1 + min(uint(radius), (srcImage.n_cols - 1) - y);

            counter.update(srcImage.submatrix(max(0, int(x) - radius), max(0, int(y) - radius), xLen, yLen), ADD);
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
            counter.update(srcImage.submatrix(max(0, int(x) - radius), y, xLen, 1), ADD);
        }

        for (uint y = 0; y < srcImage.n_cols; ++y) {
            if (y + radius < srcImage.n_cols) {
                counter.update(srcImage.submatrix(max(0, int(x) - radius), y + radius, xLen, 1), ADD);
            }
            result(x, y) = counter.getMedianPixel();

            if (int(y) - radius >= 0) {
                counter.update(srcImage.submatrix(max(0, int(x) - radius), y - radius, xLen, 1), REMOVE);
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
        hist[y].update(srcImage.submatrix(0, y, radius, 1), ADD);
    }

    for (uint x = 0; x < srcImage.n_rows; ++x) {
        for (uint y = 0; y < uint(radius); ++y) {
            if (x + radius < srcImage.n_rows) {
                hist[y].update(srcImage(x + radius, y), ADD);
            }
            counter.update(hist[y], ADD);
        }

        for (uint y = 0; y < srcImage.n_cols; ++y) {
            if (y + radius < srcImage.n_cols) {
                if (x + radius < srcImage.n_rows) {
                    hist[y + radius].update(srcImage(x + radius, y + radius), ADD);
                }
                counter.update(hist[y + radius], ADD);
            }

            result(x, y) = counter.getMedianPixel();

            if (int(y) - radius >= 0) {
                counter.update(hist[y - radius], REMOVE);
                if (int(x) - radius >= 0) {
                    hist[y - radius].update(srcImage(x - radius, y - radius), REMOVE);
                }
            }
        }

        for (uint dy = 0; dy < uint(radius); ++dy) {
            uint y = srcImage.n_cols - dy - 1;
            if (int(x) - radius >= 0) {
                hist[y].update(srcImage(x - radius, y), REMOVE);
            }
        }

        counter.reset();
    }

    return result;
}

Image canny(Image srcImage, int threshold1, int threshold2) {
    Image blured = gaussian(discolor(srcImage), 1.4, 2);

    Image derX = sobel_x(blured);
    Image derY = sobel_y(blured);

    Matrix<Gradient> grad(srcImage.n_rows, srcImage.n_cols);

    for (uint x = 0; x < grad.n_rows; ++x) {
        for (uint y = 0; y < grad.n_cols; ++y) {
            grad(x, y).value = sqrt(sqr(levelGet(derX(x, y))) + sqr(levelGet(derY(x, y))));
            grad(x, y).angle = atan2(levelGet(derY(x, y)), levelGet(derX(x, y)));
        }
    }

    RemoveNoMaxOperator op;
    grad = grad.unary_map(op);

    Matrix<GradType> gradMap(grad.n_rows, grad.n_cols);

    for (uint x = 0; x < gradMap.n_rows; ++x) {
        for (uint y = 0; y < gradMap.n_cols; ++y) {
            if (grad(x, y).value < threshold1) {
                gradMap(x, y) = EMPTY;
            } else if (grad(x, y).value > threshold2) {
                gradMap(x, y) = STRONG;
            } else {
                gradMap(x, y) = WEAK;
            }
        }
    }

    Matrix <bool> usedBuf(gradMap.n_rows, gradMap.n_cols);

    for (uint x = 0; x < gradMap.n_rows; ++x) {
        for (uint y = 0; y < gradMap.n_cols; ++y) {
            if (gradMap(x, y) == WEAK) {
                weakSetGradType(gradMap, 
                                usedBuf, 
                                std::make_pair(gradMap.n_rows, gradMap.n_cols),
                                std::make_pair(x, y));
            }
        }
    }

    Image result(srcImage.n_rows, srcImage.n_cols);
    for (uint x = 0; x < result.n_rows; ++x) {
        for (uint y = 0; y  < result.n_cols; ++y) {
            levelSet(result(x, y), (gradMap(x, y) == STRONG) ? min(255., grad(x, y).value) : 0);
        }
    }

    return result;
}
