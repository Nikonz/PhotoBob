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
using std::max;
using std::abs;

using std::cout;
using std::endl;

static int sqr(int x) {
    return x * x;
}

static int sgn(int x) {
    return (x == 0 ? 0 : (x > 0 ? 1 : -1));
}

static Shift makeShift(int x0, int y0, int x1, int y1) {
    return make_pair(mp(x0, y0), mp(x1, y1));
}

static void updateWH(uint& w, uint& h, const Shift shift) {
    h -= (sgn(shift.fst.x) == sgn(shift.sec.x) ? 
        max(abs(shift.fst.x), abs(shift.sec.x)) :
        abs(shift.fst.x) + abs(shift.sec.x));
    w -= (sgn(shift.fst.y) == sgn(shift.sec.y) ?
        max(abs(shift.fst.y), abs(shift.sec.y)) :
        abs(shift.fst.y) + abs(shift.sec.y));                       
}

static double pixelDiff(Pixel a, Pixel b, Metrics mtype) {
    return (mtype == MSE ?                                                                  
        sqr(ilevelGet(a) - ilevelGet(b)) :
        levelGet(a) * levelGet(b)
    );
}

// must remember about static buf, if you want to reuse bestShift()
static double imagesDiff(const Image& A, const Image& B, const pair<uint, uint> size, const Shift shift, const Metrics mtype, const int numBuf = -1) {
    static double buf[3][4 * MAX_SHIFT + 1][4 * MAX_SHIFT + 1]; 

    uint dx = shift.fst.x - shift.sec.x;
    uint dy = shift.fst.y - shift.sec.y;

    if (numBuf != -1 && buf[numBuf][2 * MAX_SHIFT + dx][2 * MAX_SHIFT + dy] > EPS) {
        return buf[numBuf][2 * MAX_SHIFT + dx][2 * MAX_SHIFT + dy] - INFD;
    }
    
    uint h = size.fst;
    uint w = size.sec;

    double res = 0;
    for (uint x = 0; x < h; ++x) {
        for (uint y = 0; y < w; ++y) {
             res += pixelDiff(A(x + shift.fst.x, y + shift.fst.y), B(x + shift.sec.x, y + shift.sec.y), mtype);
        }
    }
    res = (mtype == MSE ? res / (w * h) : res * (w * h));

    if (numBuf != -1) {
        buf[numBuf][2 * MAX_SHIFT + dx][2 * MAX_SHIFT + dy] = res + INFD;
    } 
    return res;
}

static double imagesDiff(const Image& A, const Image& B, const Image& C, const Shift& shift, const Metrics mtype) {
    uint w = A.n_cols;
    uint h = A.n_rows;
    updateWH(w, h, shift);

    int shiftAx = MAX(0, shift.fst.x, shift.sec.x);
    int shiftAy = MAX(0, shift.fst.y, shift.sec.y);

    int shiftBx = shiftAx - shift.fst.x;
    int shiftBy = shiftAy - shift.fst.y;
    int shiftCx = shiftAx - shift.sec.x;
    int shiftCy = shiftAy - shift.sec.y;

    return imagesDiff(A, B, make_pair(h, w), makeShift(shiftAx, shiftAy, shiftBx, shiftBy), mtype, 0) +
           imagesDiff(A, C, make_pair(h, w), makeShift(shiftAx, shiftAy, shiftCx, shiftCy), mtype, 1) +
           imagesDiff(B, C, make_pair(h, w), makeShift(shiftBx, shiftBy, shiftCx, shiftCy), mtype, 2);
}

Shift bestShift(const Image& R, const Image& G, const Image& B, const Metrics mtype) {
    assert(R.n_rows == G.n_rows && R.n_rows == B.n_rows);
    assert(R.n_cols == G.n_cols && R.n_cols == B.n_cols);
    
    Shift  optShift = makeShift(INF, INF, INF, INF);
    double optVal = (mtype == MSE ? INF : -INF);    

    for (int x0 = -MAX_SHIFT; x0 <= MAX_SHIFT; ++x0)
    for (int y0 = -MAX_SHIFT; y0 <= MAX_SHIFT; ++y0) {
        for (int x1 = -MAX_SHIFT; x1 <= MAX_SHIFT; ++x1)       
        for (int y1 = -MAX_SHIFT; y1 <= MAX_SHIFT; ++y1) {
            
            Shift shift = makeShift(x0, y0, x1, y1);
            double diff = imagesDiff(R, G, B, shift, mtype);

            if ((optVal < diff) ^ (mtype == MSE)) {
                optVal = diff;
                optShift = makeShift(x0, y0, x1, y1); 
            }
        }
    }
    cout << optVal << endl;

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

#undef x
#undef y
#undef mp
#undef MAX
