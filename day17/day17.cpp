#include <fstream>
#include <Windows.h>

#include <fmt/core.h>

using u8 = std::uint8_t;
using i32 = std::int32_t;
using u32 = std::uint32_t;

static constexpr u32 K_INPUT_RECORD_BUFFER_SIZE{ 256 };

struct SimulationContext
{
    i32 HitTrajectoryCount;
    i32 x;
    i32 y;

    i32 TargetXMin;
    i32 TargetXMax;
    i32 TargetYMin;
    i32 TargetYMax;

    mutable bool UpdateDisplay{ true };
    bool KeepRunning{ true };
};

struct SimulationResult
{
    i32 dx{ 0 };
    i32 dy{ 0 };

    i32 MinX{ 0 };
    i32 MaxX{ 0 };
    i32 MinY{ 0 };
    i32 MaxY{ 0 };
    bool HitTarget{ false };
};

void HandleInputs(SimulationContext& context)
{
    unsigned long readEvents;
    INPUT_RECORD irInBuf[K_INPUT_RECORD_BUFFER_SIZE];
    HANDLE inHandle{ GetStdHandle(STD_INPUT_HANDLE) };
    PeekConsoleInput(inHandle, irInBuf, K_INPUT_RECORD_BUFFER_SIZE, &readEvents);
    FlushConsoleInputBuffer(inHandle);
    for (unsigned long i = 0; i < readEvents; i++)
    {
        switch (irInBuf[i].EventType)
        {
            case KEY_EVENT:
            {
                const KEY_EVENT_RECORD& keyEvent{ irInBuf[i].Event.KeyEvent };

                context.UpdateDisplay = true;
                switch (keyEvent.wVirtualKeyCode)
                {
                    case 37: /*Left*/ { --context.x; break; }
                    case 38: /*Up*/ { --context.y; break; }
                    case 39: /*Right*/ { ++context.x; break; }
                    case 40: /*Down*/ { ++context.y; break; }
                    default:
                    {
                        context.KeepRunning = false;
                        break;
                    }
                }
                break;
            }
        }
    }
}

void ClearConsole()
{
    COORD topLeft = { 0, 0 };
    HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO screen;
    DWORD written;

    GetConsoleScreenBufferInfo(console, &screen);
    FillConsoleOutputCharacterA(
        console, ' ', screen.dwSize.X * screen.dwSize.Y, topLeft, &written
    );
    FillConsoleOutputAttribute(
        console, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE,
        screen.dwSize.X * screen.dwSize.Y, topLeft, &written
    );
    SetConsoleCursorPosition(console, topLeft);
}

bool IsInTargetArea(i32 x, i32 y, const SimulationContext& context)
{
    return x >= context.TargetXMin && x <= context.TargetXMax &&
           y >= context.TargetYMin && y <= context.TargetYMax;
}

void UpdateMinMax(i32 x, i32 y, SimulationResult& result)
{
    if (x < result.MinX) { result.MinX = x; }
    if (x > result.MaxX) { result.MaxX = x; }
    if (y < result.MinY) { result.MinY = y; }
    if (y > result.MaxY) { result.MaxY = y; }
}

void PerformSimulationStep(i32& x, i32& y, const SimulationContext& context, SimulationResult& result)
{
    x += result.dx;
    y += result.dy;

    if (result.dx > 0) { --result.dx; }
    else if (result.dx < 0) { ++result.dx; }
    --result.dy;

    UpdateMinMax(x, y, result);
    result.HitTarget = IsInTargetArea(x, y, context);
}

void SimulateTrajectory(const SimulationContext& context, SimulationResult& result)
{
    i32 curX{};
    i32 curY{};

    result.dx = context.x;
    result.dy = context.y;

    while (curY > context.TargetYMin && !result.HitTarget)
    {
        PerformSimulationStep(curX, curY, context, result);
    }
}

void UpdateDisplay(const SimulationContext& context)
{
    ClearConsole();

    SimulationResult result{};
    SimulateTrajectory(context, result);

    fmt::print("Possible Hit Trajectories:{}\n", context.HitTrajectoryCount);
    fmt::print("x:{} y:{}\n", context.x, context.y);
    fmt::print("target: x[{},{}] y[{},{}]\n", context.TargetXMin, context.TargetXMax, context.TargetYMin, context.TargetYMax);

    HANDLE coutHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(coutHandle, result.HitTarget ? FOREGROUND_GREEN : FOREGROUND_RED);
    fmt::print("\nHitsArea: {}\n", result.HitTarget);
    SetConsoleTextAttribute(coutHandle, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
    fmt::print("result: x[{},{}] y[{},{}]\n", result.MinX, result.MaxX, result.MinY, result.MaxY);

    context.UpdateDisplay = false;
}

bool DoesHorVelocityHitTarget(i32 dx, const SimulationContext& context)
{
    i32 x{};
    while (dx != 0 && x <= context.TargetXMax)
    {
        x += dx;
        if (dx > 0) { --dx; }

        if (x >= context.TargetXMin && x <= context.TargetXMax)
        {
            return true;
        }
    }
    return false;
}

i32 ComputeHitTrajectoryCount(const SimulationContext& context)
{
    SimulationContext dummyContext{};
    dummyContext.TargetXMin = context.TargetXMin;
    dummyContext.TargetXMax = context.TargetXMax;
    dummyContext.TargetYMin = context.TargetYMin;
    dummyContext.TargetYMax = context.TargetYMax;

    i32 possibleConfigs{};
    for (i32 dx = 1; dx <= context.TargetXMax; ++dx)
    {
        if (DoesHorVelocityHitTarget(dx, context))
        {
            for (i32 dy = context.TargetYMin; dy < 500; ++dy)
            {
                dummyContext.x = dx;
                dummyContext.y = dy;
                SimulationResult result{};
                SimulateTrajectory(dummyContext, result);
                if (result.HitTarget)
                {
                    ++possibleConfigs;
                }
            }
        }
    }

    return possibleConfigs;
}

void RunSimulationLoop()
{
    SimulationContext context{};
    //target area : x = 20..30, y = -10.. -5
    //context.TargetXMin = 20;
    //context.TargetXMax = 30;
    //context.TargetYMin = -10;
    //context.TargetYMax = -5;

    //target area: x=25..67, y=-260..-200
    context.TargetXMin = 25;
    context.TargetXMax = 67;
    context.TargetYMin = -260;
    context.TargetYMax = -200;

    context.HitTrajectoryCount = ComputeHitTrajectoryCount(context);

    while (context.KeepRunning)
    {
        HandleInputs(context);
        if (context.UpdateDisplay)
        {
            UpdateDisplay(context);
        }
        Sleep(33);
    }
}

int main()
{
    RunSimulationLoop();
    return 0;
}