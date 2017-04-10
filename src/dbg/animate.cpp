#include "animate.h"
#include "x64dbg.h"
#include "command.h"

static char animate_command[deflen];
static unsigned int animate_interval = 50;
HANDLE hAnimateThread = nullptr;

static DWORD WINAPI animateThread(void* arg1)
{
    while(animate_command[0] != 0)
    {
        auto beforeTime = GetTickCount();
        if(!cmddirectexec(animate_command))
        {
            // An error occurs
            duint value = 0;
            BridgeSettingGetUint("Misc", "AnimateStopWhenError", &value);
            if(value != 0)
                break;
        }
        auto currentTime = GetTickCount();
        if(currentTime < (beforeTime + animate_interval))
        {
            Sleep(beforeTime + animate_interval - currentTime);
        }
    }
    // Close the handle itself
    HANDLE hAnimateThread2 = hAnimateThread;
    hAnimateThread = nullptr;
    CloseHandle(hAnimateThread2);
    return 0;
}

bool _dbg_animatecommand(const char* command)
{
    if(command) // Animate command
    {
        strcpy_s(animate_command, command);
        if(hAnimateThread == nullptr)
        {
            hAnimateThread = CreateThread(NULL, 0, animateThread, nullptr, 0, nullptr);
        }
    }
    else // command = null : stop animating
    {
        animate_command[0] = 0;
    }
    return true;
}

void _dbg_setanimateinterval(unsigned int milliseconds)
{
    if(milliseconds <= 20)
        animate_interval = 20;
    else
        animate_interval = milliseconds;
}

bool _dbg_isanimating()
{
    return hAnimateThread != nullptr;
}