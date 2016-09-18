#include "align.h"
#include <string>

using std::string;
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
    return unit(R, G, B, bestShift(R, G, B, MSE));
  
    /* 
    #define x first
    #define y second 
    Shift sh = bestShift(R, G, B, MSE);
    cout << sh.x.x << " " << sh.x.y << " : " << sh.y.x << " " << sh.y.y << endl;
    #define mp std::make_pair
    return unit(R, G, B, mp(mp(-5, -1), mp(-9, -3)));
    */
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
    return src_image;
}

Image gray_world(Image src_image) {
    return src_image;
}

Image resize(Image src_image, double scale) {
    return src_image;
}

Image custom(Image src_image, Matrix<double> kernel) {
    // Function custom is useful for making concrete linear filtrations
    // like gaussian or sobel. So, we assume that you implement custom
    // and then implement other filtrations using this function.
    // sobel_x and sobel_y are given as an example.
    return src_image;
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

Image median(Image src_image, int radius) {
    return src_image;
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
