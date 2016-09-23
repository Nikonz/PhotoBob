#include "other_types.h"
#include <cassert>

Counter::Counter() : sum(0) {
    reset();
};

Counter::Counter(Counter& counter) : sum(counter.sum) {
    for (uint color = RED; color <= BLUE; ++color) {
        for (uint lvl = 0; lvl < MAX_LVL; ++lvl) {
            count[color][lvl] = counter.count[color][lvl];
        }
    }
}

Counter::~Counter() {};

void Counter::update(const Pixel pixel, const bool action) {
    assert(action != REMOVE || sum > 0);

    int value = (action == REMOVE ? -1 : 1);
    count[RED  ][colorGet(pixel, RED)]   += value;
    count[GREEN][colorGet(pixel, GREEN)] += value;
    count[BLUE ][colorGet(pixel, BLUE)]  += value;
    sum += value;
}

void Counter::update(const Image image, const bool action) {
    for (uint x = 0; x < image.n_rows; ++x) {
        for (uint y = 0; y < image.n_cols; ++y) {
            update(image(x, y), action);
        }
    }
}

void Counter::update(const Counter& counter, const bool action) {
    uint value = (action ? -1 : 1);
    for (uint color = RED; color <= BLUE; ++color) {
        for (uint lvl = 0; lvl < MAX_LVL; ++lvl) {
            assert(action != REMOVE || count[color][lvl] >= counter.count[color][lvl]);
            count[color][lvl] += counter.count[color][lvl] * value;
        }
    }
    sum += counter.sum * value;
}

void Counter::reset() {
    memset(count, 0, sizeof(count));
    sum = 0;
}

Pixel Counter::getPixel(double fraction) {
    uint res[3];
    for (uint color = RED; color <= BLUE; ++color) {
        res[color] = getKthLvl(count[color], round(sum * fraction));
    }
    return Pixel(res[RED], res[GREEN], res[BLUE]);
}

Pixel Counter::getMedianPixel() {
    return getPixel(0.5);
}

