#include "func.h"

#include <cassert>
#include <algorithm>

#define mp make_pair
#define x first
#define y second
#define fst first
#define sec second

#define MAX(a, b, c) std::max(std::max(a, b), c)

using std::make_pair;
using std::min;
using std::max;
using std::abs;

using std::cout;
using std::endl;

static int sgn(int x) {
    return (x == 0 ? 0 : (x > 0 ? 1 : -1));
}

static Shift makeShift(int x0, int y0, int x1, int y1) {
    return make_pair(mp(x0, y0), mp(x1, y1));
}

void weakSetGradType(Matrix <GradType> gradMap, Matrix <bool> used, const Size size, const Position pos) {
    static int kx[8] = {-1, -1, -1, 0, 1, 1,  1,  0};
    static int ky[8] = {-1,  0,  1, 1, 1, 0, -1, -1};

    assert(gradMap(pos.x, pos.y) == WEAK);
    used(pos.x, pos.y) = true;

    for (int i = 0; i < 8; ++i) {
        int x = pos.x + kx[i];
        int y = pos.y + ky[i];
        if (x < 0 || uint(x) >= size.x || y < 0 || uint(y) >= size.y) {
            continue;
        }

        if (gradMap(x, y) == WEAK && !used(x, y)) {
            weakSetGradType(gradMap, used, size, make_pair(x, y));
        }
        if (gradMap(x, y) == STRONG) {
            gradMap(pos.x, pos.y) = STRONG;
            return;
        }
    }

    gradMap(pos.x, pos.y) = EMPTY;
}

void pixelMul(Pixel& pixel, double valR, double valG, double valB) {
    colorMul(pixel, RED,   valR);
    colorMul(pixel, GREEN, valG);
    colorMul(pixel, BLUE,  valB);
}

void pixelMul(Pixel& pixel, double value) {
    pixelMul(pixel, value, value, value);
}

uint getMedianLvl(const uint (&count)[MAX_LVL], const uint median) {
    uint sum = 0;
    for (uint lvl = 0; lvl < MAX_LVL; ++lvl) {
        sum += count[lvl];
        if (sum > median) {
            return lvl;
        } 
    }

    return 255;
}

Pixel getMedianPixel(const uint (&count)[3][MAX_LVL], const uint median) {
    uint res[3];
    for (uint color = RED; color <= BLUE; ++color) {
        res[color] = getMedianLvl(count[color], median);
    }
    return Pixel(res[RED], res[GREEN], res[BLUE]);
}

Pixel pixelSum(const Image image) {
    uint res[3] = {0};

    for (uint x = 0; x < image.n_rows; ++x) {
        for (uint y = 0; y < image.n_cols; ++y) {
            res[RED  ] += colorGet(image(x, y), RED);
            res[GREEN] += colorGet(image(x, y), GREEN);
            res[BLUE ] += colorGet(image(x, y), BLUE);
        }
    }
    return Pixel(res[RED], res[GREEN], res[BLUE]);
}

static void updateWH(uint& w, uint& h, const Shift shift) {
    h -= (sgn(shift.fst.x) == sgn(shift.sec.x) ? 
        max(abs(shift.fst.x), abs(shift.sec.x)) :
        abs(shift.fst.x) + abs(shift.sec.x));
    w -= (sgn(shift.fst.y) == sgn(shift.sec.y) ?
        max(abs(shift.fst.y), abs(shift.sec.y)) :
        abs(shift.fst.y) + abs(shift.sec.y));                       
}

static uint pixelDiff(Pixel a, Pixel b, Metrics mtype) {
    return (mtype == MSE ? 
        sqr(levelGet(a) - levelGet(b)) :
        levelGet(a) * levelGet(b)
    );
}

// must remember about static buf, if you want to reuse bestShift()
static dblong imagesDiff(const Image& A, const Image& B, const SingleShift shift, const Metrics mtype, const int numBuf = -1) {
    static dblong buf[3][2 * MAX_SHIFT + 1][2 * MAX_SHIFT + 1]; 

    int dx = shift.x;
    int dy = shift.y;

    if (numBuf != -1 && buf[numBuf][MAX_SHIFT + dx][MAX_SHIFT + dy] > EPS) {
        return buf[numBuf][MAX_SHIFT + dx][MAX_SHIFT + dy] - 1;
    } 

    uint h  = A.n_rows;
    uint w  = A.n_cols;
    uint dh = h * FAKE_BRD;
    uint dw = w * FAKE_BRD;

    Image subA = A.submatrix(dh, dw, h - 2 * dh, w - 2 * dw);
    Image subB = B.submatrix(dh, dw, h - 2 * dh, w - 2 * dw);

    h = subA.n_rows;
    w = subA.n_cols;

    uintl res = 0;
    for (uint x = max(0, dx) ; x < h - max(0, -dx); ++x) {
        for (uint y = max(0, dy); y < w - max(0, -dy); ++y) {
            res += pixelDiff(subA(x, y), subB(x - dx, y - dy), mtype);
        }
    }
    h -= abs(dx);
    w -= abs(dy);

    dblong result = (mtype == MSE ? dblong(res) / (w * h) : res);

    if (numBuf != -1) {
        buf[numBuf][MAX_SHIFT + dx][MAX_SHIFT + dy] = result + 1;
    } 
    return result;
}

static dblong imagesDiff(const Image& A, const Image& B, const Shift& shift, const Metrics mtype, const int numBuf = -1) {
    return imagesDiff(A, B, make_pair(shift.sec.x - shift.fst.x, shift.sec.y - shift.fst.y), mtype, numBuf);
}

static dblong imagesDiff(const Image& A, const Image& B, const Image& C, const Shift& shift, const Metrics mtype) {
    return imagesDiff(A, B, shift.fst, mtype, 0) +
           imagesDiff(A, C, shift.sec, mtype, 1) +
           imagesDiff(B, C, shift, mtype, 2);
}

Shift bestShift(const Image& R, const Image& G, const Image& B, const Metrics mtype) {
    assert(R.n_rows == G.n_rows && R.n_rows == B.n_rows);
    assert(R.n_cols == G.n_cols && R.n_cols == B.n_cols);
    
    Shift  optShift = makeShift(INF, INF, INF, INF);
    dblong optVal = (mtype == MSE ? INFL : -1);    

    for (int x0 = -MAX_SHIFT; x0 <= MAX_SHIFT; ++x0)
    for (int y0 = -MAX_SHIFT; y0 <= MAX_SHIFT; ++y0) {
        for (int x1 = max(0, x0) - MAX_SHIFT; x1 <= MAX_SHIFT + min(0, x0); ++x1)       
        for (int y1 = max(0, y0) - MAX_SHIFT; y1 <= MAX_SHIFT + min(0, y0); ++y1) {
            
            Shift shift = makeShift(x0, y0, x1, y1);
            dblong diff = imagesDiff(R, G, B, shift, mtype);

            if ((optVal < diff) ^ (mtype == MSE)) {
                optVal = diff;
                optShift = makeShift(x0, y0, x1, y1); 
            }
        }
    }
    cout << optVal << endl;
    cout << optShift.fst.x << " " << optShift.fst.y <<
         " : " << optShift.sec.x << " " << optShift.sec.y << endl;

    return optShift;
}

Image unit(const Image& R, const Image& G, const Image& B, const Shift& shift) {
    assert(R.n_rows == G.n_rows && R.n_rows == B.n_rows);
    assert(R.n_cols == G.n_cols && R.n_cols == B.n_cols);

    uint w = R.n_cols; 
    uint h = R.n_rows;
    updateWH(w, h, shift);
    Image result(h, w);
    
    uint dx = MAX(0, shift.fst.x, shift.sec.x);
    uint dy = MAX(0, shift.fst.y, shift.sec.y);

    for (uint x = 0; x < h; ++x) {
        for (uint y = 0; y < w; ++y) {
            colorSet(result(x, y), RED,   levelGet(R(x + dx, y + dy)));
            colorSet(result(x, y), GREEN, levelGet(G(x + dx - shift.first.x,  y + dy - shift.first.y)));
            colorSet(result(x, y), BLUE,  levelGet(B(x + dx - shift.second.x, y + dy - shift.second.y)));
        }
    }

    return result;
}

