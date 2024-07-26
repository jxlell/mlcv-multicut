#ifndef DIRECTION_PATH_H
#define DIRECTION_PATH_H

// You can also use #pragma once as an alternative to include guards.
// #pragma once

#include <vector>
#include <tuple>

// Enum class to define directions
enum class Direction {
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

#endif // DIRECTION_PATH_H
