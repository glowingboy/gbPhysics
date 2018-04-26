#pragma once

#include "type.h"

GB_PHYSICS_NS_BEGIN

template <typename T>
struct plane
{
    vec3<T> normal;
    vec3<T> point;
};

GB_PHYSICS_NS_END
