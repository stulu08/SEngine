#include "st_pch.h"
#include "Random.h"

namespace Stulu {
    int Random::getInt(int min, int max) {
        return min + rand() % (max + 1 - min);
    }
    float Random::getFloat(float min, float max) {
        float random = ((float)(rand()) / (float)(RAND_MAX));
        float distance = max - min;
        return min + distance * random;
    }
}

