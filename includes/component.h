#pragma once

#include "core.h"

namespace SGL
{
    struct Transform
    {
        raylib::Vector3    position = { 0, 0, 0 };
        raylib::Quaternion rotation = raylib::QuaternionIdentity();
        raylib::Vector3    scale = { 1, 1, 1 };
    };

    struct Color
    {
        unsigned char r = 255;
        unsigned char g = 255;
        unsigned char b = 255;
        unsigned char a = 255;
    };

    struct Visible
    {
        int value = 1;
    };

}