#include "func.h"

class Counter {
public:
    Counter();
    Counter(Counter& counter);
    ~Counter();

    void update(const Pixel image, const bool remove = false);
    void update(const Image image, const bool remove = false);
    void update(const Counter& counter, const bool remove = false);
    void reset();

    Pixel getMedianPixel();
private:
    uint count[3][MAX_LVL];
    uint sum;
};
