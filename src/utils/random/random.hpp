#pragma once

#include <random>

std::random_device rd;
std::mt19937 gen(rd());

namespace random
{
  float rangeFloat(float min, float max)
  {
    std::uniform_real_distribution<float> dis(min, max);

    return dis(gen);
  }
}