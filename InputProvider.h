#pragma once
#include "InputState.h"

class InputProvider
{
public:
    virtual ~InputProvider() = default;
    virtual const InputState& GetInputState() const = 0;
};