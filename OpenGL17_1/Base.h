#pragma once
#include <iostream>
using namespace std;
#define print(x) std::cout << x
#define print2D(v) std::cout << "(" << v[0] << ", " << v[1] << ")"
#define print3D(v) std::cout << "(" << v[0] << ", " << v[1] << ", " << v[2] << ")"
#define printColor3D(v) std::cout << "(" << v.r << ", " << v.g << ", " << v.b << ")"
#define convertVec3(v) glm::vec3(v[0], v[1], v[2])
#define convertVec2(v) glm::vec2(v[0], v[1])