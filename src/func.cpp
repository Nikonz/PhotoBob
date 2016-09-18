#include "func.h"

#include <cassert>
#include <algorithm>

#define mp std::make_pair
#define x  first
#define y  second

#define colorSet(pixel, color, value) std::get<color>(pixel) = value
#define colorGet(pixel, color)        uint(std::get<color>(pixel))
#define lvlGet(pixel)                 colorGet(pixel, 0)

#define sgn(x) (x == 0 ? 0 : (x > 0 ? 1 : -1))
#define MAX(a, b, c) std::max(std::max(a, b), c)

using std::max;
using std::abs;

using std::cout;
using std::endl;

static int sqr(int x) {
    return x * x;
}

// must remember about static buf, if you want to reuse bestShift()
static double calcDiff(const Image& A, const Image& B, int sx, int sy, Metrics mtype, int numBuf = -1) {
    static double buf[2][4 * MAX_SHIFT + 1][4 * MAX_SHIFT + 1]; 

    if (numBuf != -1 && buf[numBuf][2 * MAX_SHIFT + sx][2 * MAX_SHIFT + sy] > EPS) {
        return buf[numBuf][2 * MAX_SHIFT + sx][2 * MAX_SHIFT + sy] - INFD;
    }

    assert(A.n_cols == B.n_cols && A.n_rows == B.n_rows);

    uint h = A.n_rows - abs(sx);
    uint w = A.n_cols - abs(sy);

    uint dx = max(0, sx);
    uint dy = max(0, sy);

    double res = 0;
    for (uint x = 0; x < h; ++x) {
        for (uint y = 0; y < w; ++y) {
             res += (mtype == MSE ? 
                sqr(int(lvlGet(A(x + dx, y + dy))) - int(lvlGet(B(x + dx - sx, y + dy - sy)))) :
                lvlGet(A(x + dx, y + dy)) * lvlGet(B(x + dx - sx, y + dy - sy))
                );
        }
    }

    if (numBuf != -1) {
        buf[numBuf][2 * MAX_SHIFT + sx][2 * MAX_SHIFT + sy] = (mtype == MSE ? double(res) / (w * h) : res) + INFD;
    } 
    return (mtype == MSE ? double(res) / (w * h) : res);
}

Shift bestShift(const Image& R, const Image& G, const Image& B, Metrics mtype) {
    Shift  optShift = std::make_pair(mp(INF, INF), mp(INF, INF));
    double optVal = (mtype == MSE ? INF : -INF);    

    for (int x0 = -MAX_SHIFT; x0 <= MAX_SHIFT; ++x0)
    for (int y0 = -MAX_SHIFT; y0 <= MAX_SHIFT; ++y0) {
        double resRG = calcDiff(R, G, x0, y0, mtype);
 
        for (int x1 = -MAX_SHIFT; x1 <= MAX_SHIFT; ++x1)       
        for (int y1 = -MAX_SHIFT; y1 <= MAX_SHIFT; ++y1) {
            double resRB = calcDiff(R, B, x1, y1, mtype, 0);
            double resGB = calcDiff(G, B, x0 - x1, y0 - y1, mtype, 1);
            //double resGB = calcDiff(G, B, x1 - x0, y1 - y0, mtype, 1);

            double sum = resRG + resRB + resGB;
            //double sum = resRB;
            if ((optVal < sum) ^ (mtype == MSE)) {
                optVal = sum;
                /*
                if (sum < 5400) {
                    cout << resAB << " " << resAC << " " << resBC << endl;
                    cout << x0 << " " << y0 << " : " << x1 << " " << y1 << endl;
                }
                */
                optShift = std::make_pair(mp(x0, y0), mp(x1, y1)); 
            }
        }
    }
    cout << optVal << endl;

    return optShift;
}

Image unit(const Image& R, const Image& G, const Image& B, Shift shift) {
    assert(R.n_rows == G.n_rows && R.n_rows == B.n_rows);
    assert(R.n_cols == G.n_cols && R.n_cols == B.n_cols);

    uint h = R.n_rows - (sgn(shift.first.x) == sgn(shift.second.x) ? 
        max(abs(shift.first.x), abs(shift.second.x)) :
        abs(shift.first.x) + abs(shift.second.x));
    uint w = R.n_cols - (sgn(shift.first.y) == sgn(shift.second.y) ?
        max(abs(shift.first.y), abs(shift.second.y)) :
        abs(shift.first.y) + abs(shift.second.y));

    Image result(h, w);
    
    uint dx = MAX(0, shift.first.x, shift.second.x);
    uint dy = MAX(0, shift.first.y, shift.second.y);

    for (uint x = 0; x < h; ++x) {
        for (uint y = 0; y < w; ++y) {
            colorSet(result(x, y), RED,   lvlGet(R(x + dx, y + dy)));
            colorSet(result(x, y), GREEN, lvlGet(G(x + dx - shift.first.x,  y + dy - shift.first.y)));
            colorSet(result(x, y), BLUE,  lvlGet(B(x + dx - shift.second.x, y + dy - shift.second.y)));
        }
    }

    return result;
}

#undef x
#undef y
#undef mp
#undef sgn
#undef MAX
