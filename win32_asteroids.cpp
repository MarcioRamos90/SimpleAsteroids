
#include "program.h"
#include "space.h"
#include "space.c"
#include "win32_asteroids.h"

win32_screen_buffer GlobalBuffer = {};
bool32 GlobalRunning = false;
RECT TextDrawble = {10, 10, 500, 500};
key_board_input KeyboardInput;
HFONT hFont;
i64 GlobalPerfCountFrequency;


internal void
Win32CreateScreenMemoryBuffer(win32_screen_buffer* Buffer, u32 Width, u32 Height)
{
    Buffer->Width = Width;
    Buffer->Height = Height;
    Buffer->BytesPerPixel = 4;
    Buffer->Pitch = Buffer->BytesPerPixel * Buffer->Width;

    Buffer->Info.bmiHeader.biWidth = Width;
    Buffer->Info.bmiHeader.biHeight = Height;
    Buffer->Info.bmiHeader.biPlanes = 1;
    Buffer->Info.bmiHeader.biBitCount = 32;
    Buffer->Info.bmiHeader.biCompression = BI_RGB;
    Buffer->Info.bmiHeader.biSize = sizeof(Buffer->Info.bmiHeader);

    u32 BitmapMemorySize = Width * Height * Buffer->BytesPerPixel;
    Buffer->Memory = VirtualAlloc(0, BitmapMemorySize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
}

internal void
Win32DisplayWindowBuffer(win32_screen_buffer* Buffer, HWND Window)
{
    HDC dc = GetDC(Window);

    RECT Rect;
    GetClientRect(Window, &Rect);

    u32 Width = Rect.right - Rect.left;
    u32 Height = Rect.bottom - Rect.top;

    PatBlt(
        dc,
        Buffer->Width, 0,
        Width, Height,
        WHITENESS);
    PatBlt(
        dc,
        0, Buffer->Height,
        Width, Height,
        WHITENESS);

    StretchDIBits(
        GetDC(Window), 
        0, 0,  Buffer->Width, Buffer->Height,
        0, 0,  Buffer->Width, Buffer->Height,
        Buffer->Memory,
        &Buffer->Info,
        DIB_RGB_COLORS, SRCCOPY);
    ReleaseDC(Window, dc);
}

LRESULT CALLBACK
Win32MainWindowCallback(HWND Window,
                        UINT Message,
                        WPARAM WParam,
                        LPARAM LParam)
{       
    LRESULT Result = 0;

    switch(Message)
    {
        case WM_CLOSE:
        {
            GlobalRunning = false;
        } break;

        case WM_ACTIVATEAPP:
        {

        } break;

        case WM_DESTROY:
        {
            GlobalRunning = false;
        } break;

        case WM_LBUTTONDOWN:
        {
            RECT ClientRect;
            GetClientRect(Window, &ClientRect);
            // Width = ClientRect.right - ClientRect.left;
            // Height = ClientRect.bottom - ClientRect.top;

            // xPos = LOWORD(LParam) - 5; // Get the x-coordinate
            // yPos = HIWORD(LParam) - 10; // Get the y-coordinate
        } break;

        case WM_LBUTTONUP:
        {

        } break;

        case WM_MOUSEMOVE:
        {} break;

        case WM_SYSKEYDOWN:
        case WM_SYSKEYUP:
        case WM_KEYDOWN:
        case WM_KEYUP:
        {
            assert(!"Keyboard input came in through a non-dispatch message!");
        } break;
        
        case WM_PAINT:
        {
            PAINTSTRUCT Paint;
            HDC DeviceContext = BeginPaint(Window, &Paint);
            Win32DisplayWindowBuffer(&GlobalBuffer, Window);
            EndPaint(Window, &Paint);
        } break;
        case WM_LBUTTONDBLCLK:
        {
            OutputDebugStringA("clicado\n");
        } break;
        default:
        {
            Result = DefWindowProcA(Window, Message, WParam, LParam);
        } break;
    }
    
    return(Result);
}

internal void
Win32ProcessPendingMessages(void)
{
    MSG Message;
    while(PeekMessage(&Message, 0, 0, 0, PM_REMOVE))
    {
        switch(Message.message)
        {
            case WM_QUIT:
            {
                GlobalRunning = false;
            } break;
            case WM_SYSKEYDOWN:
            case WM_SYSKEYUP:
            case WM_KEYDOWN:
            case WM_KEYUP:
            {
                u32 VKCode = (u32)Message.wParam;
                u32 lparam = LOWORD(Message.lParam);

                bool32 WasDown = ((Message.lParam & (1 << 30)) != 0);
                bool32 IsDown = ((Message.lParam & (1 << 31)) == 0);

                bool32 AltKeyWasDown = (Message.lParam & (1 << 29));

                if((VKCode == VK_F4) && AltKeyWasDown)
                {
                    GlobalRunning = false;
                }
                
                if(VKCode == 'W' )
                {
                    if (IsDown)
                    {
                        KeyboardInput.KeyW.IsDown = true;
                    }
                    else
                    {
                        KeyboardInput.KeyW.IsDown = false;
                    }
                }
                if(VKCode == 'S' )
                {
                    if (IsDown)
                    {
                        KeyboardInput.KeyS.IsDown = true;
                    }
                    else
                    {
                        KeyboardInput.KeyS.IsDown = false;
                    }
                }
                if(VKCode == 'D' )
                {
                    if (IsDown)
                    {
                        KeyboardInput.KeyD.IsDown = true;
                    }
                    else
                    {
                        KeyboardInput.KeyD.IsDown = false;
                    }
                }
                if(VKCode == 'A' )
                {
                    if (IsDown)
                    {
                        KeyboardInput.KeyA.IsDown = true;
                    }
                    else
                    {
                        KeyboardInput.KeyA.IsDown = false;
                    }
                }

                if(VKCode == VK_UP)
                {
                    if (IsDown)
                    {
                        KeyboardInput.KeyUP.IsDown = true;
                    }
                    else
                    {
                        KeyboardInput.KeyUP.IsDown = false;
                    }
                }
                if(VKCode == VK_LEFT)
                {
                    if (IsDown)
                    {
                        KeyboardInput.KeyLeft.IsDown = true;
                    }
                    else
                    {
                        KeyboardInput.KeyLeft.IsDown = false;
                    }
                }
                if(VKCode == VK_DOWN)
                {
                    if (IsDown)
                    {
                        KeyboardInput.KeyDown.IsDown = true;
                    }
                    else
                    {
                        KeyboardInput.KeyDown.IsDown = false;
                    }
                }
                if(VKCode == VK_RIGHT)
                {
                    if (IsDown)
                    {
                        KeyboardInput.KeyRight.IsDown = true;
                    }
                    else
                    {
                        KeyboardInput.KeyRight.IsDown = false;
                    }
                }
            } break;

            case WM_LBUTTONDBLCLK:
            {
                OutputDebugStringA("clicado\n");
            } break;

            default:
            {
                TranslateMessage(&Message);
                DispatchMessageA(&Message);
            } break;
        }
    }
}


inline LARGE_INTEGER
Win32GetWallClock(void)
{    
    LARGE_INTEGER Result;
    QueryPerformanceCounter(&Result);
    return(Result);
}

inline f32
Win32GetSecondsElapsed(LARGE_INTEGER Start, LARGE_INTEGER End)
{
    f32 Result = ((f32)(End.QuadPart - Start.QuadPart) /
                     (f32)GlobalPerfCountFrequency);
    return(Result);
}

int CALLBACK
WinMain(HINSTANCE Instance,
        HINSTANCE PrevInstance,
        LPSTR CommandLine,
        int ShowCode)
{
    // HandleFonts(L"");

    GlobalRunning = true;

    LARGE_INTEGER PerfCountFrequencyResult;
    QueryPerformanceFrequency(&PerfCountFrequencyResult);
    GlobalPerfCountFrequency = PerfCountFrequencyResult.QuadPart;

    
    UINT DesiredSchedulerMS = 1;
    bool32 SleepIsGranular = (timeBeginPeriod(DesiredSchedulerMS) == TIMERR_NOERROR);


    Win32CreateScreenMemoryBuffer(&GlobalBuffer, 960, 540);

    WNDCLASSW WindowClass = {};
    {
        WindowClass.style = CS_HREDRAW|CS_VREDRAW;
        WindowClass.lpfnWndProc = Win32MainWindowCallback;
        WindowClass.hInstance = Instance;
        WindowClass.lpszClassName = L"HandmadeHeroWindowClass";
    }

    if(RegisterClassW(&WindowClass))
    {
        HWND Window =
            CreateWindowExW(
                NULL,
                WindowClass.lpszClassName,
                L"Handmade Hero",
                WS_OVERLAPPEDWINDOW|WS_VISIBLE,
                CW_USEDEFAULT,
                CW_USEDEFAULT,
                CW_USEDEFAULT,
                CW_USEDEFAULT,
                0,
                0,
                Instance,
                0);

        if(Window)
        {
            int MonitorRefreshHz = 60;
            HDC RefreshDC = GetDC(Window);
            int Win32RefreshRate = GetDeviceCaps(RefreshDC, VREFRESH);
            ReleaseDC(Window, RefreshDC);
            if(Win32RefreshRate > 1)
            {
                MonitorRefreshHz = Win32RefreshRate;
            }
            f32 GameUpdateHz = (MonitorRefreshHz / 2.0f);
            f32 TargetSecondsPerFrame = 1.0f / (f32)GameUpdateHz;

            LARGE_INTEGER LastCounter = Win32GetWallClock();
            while(GlobalRunning)
            {
                Win32ProcessPendingMessages();

                game_struct GameState = {};
                GameState.DisplayBuffer.Width = GlobalBuffer.Width;
                GameState.DisplayBuffer.Height = GlobalBuffer.Height;
                GameState.DisplayBuffer.Pitch = GlobalBuffer.Pitch;
                GameState.DisplayBuffer.BytesPerPixel = GlobalBuffer.BytesPerPixel;
                GameState.DisplayBuffer.Memory = GlobalBuffer.Memory;

                MainGame(&GameState, &KeyboardInput);

                Win32DisplayWindowBuffer(&GlobalBuffer, Window);

                LARGE_INTEGER WorkCounter = Win32GetWallClock();

                f32 WorkSecondsElapsed = Win32GetSecondsElapsed(LastCounter, WorkCounter);

                f32 SecondsElapsedForFrame = WorkSecondsElapsed;
                if(SecondsElapsedForFrame < TargetSecondsPerFrame)
                {
                    if(SleepIsGranular)
                    {
                        DWORD SleepMS = (DWORD)(1000.0f * (TargetSecondsPerFrame -
                                                                   SecondsElapsedForFrame));
                        if(SleepMS > 0)
                        {
                            Sleep(SleepMS);
                        }
                    }
                }

                LARGE_INTEGER EndCounter = Win32GetWallClock();
                LastCounter = EndCounter;
            }
        }
    }
    return 0;
}