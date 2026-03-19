#include "Time.h"
#include <algorithm>

using SteadyClock = std::chrono::steady_clock;
SteadyClock::time_point Time::lastFrameTime;

float Time::time = 0.0f;
float Time::unscaledTime = 0.0f;

float Time::deltaTime = 0.0f;
float Time::unscaledDeltaTime = 0.0f;

float Time::timeScale = 1.0f;
bool Time::initialized = false;

void Time::Init()
{
    if (!initialized)
    {
        lastFrameTime = SteadyClock::now();
        time = 0.0f;
        unscaledTime = 0.0f;
        deltaTime = 0.0f;
        unscaledDeltaTime = 0.0f;

        initialized = true;
    }
}

void Time::Update()
{
    auto currentTime = SteadyClock::now();
    std::chrono::duration<float> duration = currentTime - lastFrameTime;

    constexpr float MAX_DELTA = 0.1f;

    unscaledDeltaTime = std::min(duration.count(), MAX_DELTA);
    deltaTime = unscaledDeltaTime * timeScale;     

    unscaledTime += unscaledDeltaTime;
    time += deltaTime;

    lastFrameTime = currentTime;
}

float Time::GetDeltaTime()
{
    return deltaTime;
}

float Time::GetTime()
{
    return time;
}

float Time::GetTimeScale() 
{ 
    return timeScale; 
}

void Time::SetTimeScale(float scale) 
{ 
    timeScale = std::clamp(scale, 0.0f, 10.0f);
}

void Time::Reset()
{
    lastFrameTime = SteadyClock::now();
    deltaTime = 0.0f;
    unscaledDeltaTime = 0.0f;
    time = 0.0f;
    unscaledTime = 0.0f;
}