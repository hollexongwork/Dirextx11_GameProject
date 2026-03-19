#pragma once
#include <chrono>

class Time
{
private:
    static std::chrono::high_resolution_clock::time_point lastFrameTime;
    static float time;
    static float unscaledTime;

    static float deltaTime;
    static float unscaledDeltaTime;

    static float timeScale;
    static bool initialized;

public:
    static void Init();
    static void Update();
    static float GetTime();
    static float GetDeltaTime();
    static float GetTimeScale();      
    static void SetTimeScale(float scale);
    static void Reset();
};
