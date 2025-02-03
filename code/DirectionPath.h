#ifndef DIRECTION_PATH_H
#define DIRECTION_PATH_H

// You can also use #pragma once as an alternative to include guards.
// #pragma once

#include <vector>
#include <tuple>

// Enum class to define directions
//TODO: auf 2 bits reduzieren 
enum class Direction : uint8_t{
    UP,
    RIGHT,
    DOWN,
    LEFT
};

// Define the tuple type for path information
// The tuple contains: startEdge, startDirection, and a directionVector
using PathInfo = std::tuple<int, Direction, std::vector<bool>>;

// Define the vector of tuples to hold path information
using PathInfoVector = std::vector<PathInfo>;

using RLEVector = std::vector<std::tuple<int, std::vector<bool>, std::vector<uint16_t>, bool>>;

#endif // DIRECTION_PATH_H
