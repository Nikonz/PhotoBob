#include "func.h"

const bool ADD    = false;
const bool REMOVE = true;

class Counter {
public:
    Counter();
    Counter(Counter& counter);
    ~Counter();

    void update(const Pixel image, const bool action);
    void update(const Image image, const bool action);
    void update(const Counter& counter, const bool action);
    void reset();

    Pixel getPixel(double fraction);
    Pixel getMedianPixel();
private:
    uint count[3][MAX_LVL];
    uint sum;
};
