#include "Direct3DInitializing.h"

LPCTSTR WndClassName = L"DirectX11Window";                  //Define our window class name




LRESULT CALLBACK WndProc(HWND g_hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_KEYDOWN:
    {
        if (wParam == VK_ESCAPE)                            //if escape key was pressed, display popup box
        {
            if (MessageBox(0, L"Are you sure you want to exit?", L"Really?", MB_YESNO | MB_ICONQUESTION) == IDYES)
            {
                DestroyWindow(g_hwnd);                        //Release the windows allocated memory
            }
        }

        return 0;
    }

    case WM_DESTROY:                                        //if x button in top right was pressed
    {
        PostQuitMessage(0);
        return 0;
    }

    default:
        break;
    }

    return DefWindowProc(g_hwnd, msg, wParam, lParam);        //return the message for windows to handle it
}

bool InitializeWindow(HINSTANCE hInstance, int ShowWnd, int width, int height, bool windowed)
{
    WNDCLASSEX wc;                                          //Create a new extended windows class

    wc.cbSize = sizeof(WNDCLASSEX);                         //Size of our windows class
    wc.style = CS_HREDRAW | CS_VREDRAW;                     //class styles
    wc.lpfnWndProc = WndProc;                               //Default windows procedure function
    wc.cbClsExtra = NULL;                                   //Extra bytes after our wc structure
    wc.cbWndExtra = NULL;                                   //Extra bytes after our windows instance
    wc.hInstance = hInstance;                               //Instance to current application
    wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);                 //Title bar Icon
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);               //Default mouse Icon
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 2);          //Window bg color
    wc.lpszMenuName = NULL;                                 //Name of the menu attached to our window
    wc.lpszClassName = WndClassName;                        //Name of our windows class
    wc.hIconSm = LoadIcon(NULL, IDI_WINLOGO);               //Icon in your taskbar

    if (!RegisterClassEx(&wc))                              //Register our windows class
    {
        MessageBox(NULL, L"Error registering class", L"Error", MB_OK | MB_ICONERROR);
        return 1;
    }

    g_hwnd = CreateWindowEx(                                  //Create our Extended Window
        NULL,                                               //Extended style
        WndClassName,                                       //Name of our windows class
        L"Direct X 11",                                     //Name in the title bar of our window
        WS_OVERLAPPEDWINDOW,                                //style of our window
        CW_USEDEFAULT, CW_USEDEFAULT,                       //Top left corner of window
        width,                                              //g_Width of our window
        height,                                             //g_Height of our window
        NULL,                                               //Handle to parent window
        NULL,                                               //Handle to a Menu
        hInstance,                                          //Specifies instance of current program
        NULL                                                //used for an MDI client window
    );

    if (!g_hwnd)                                              //Make sure our window has been created
    {
        MessageBox(NULL, L"Error creating window", L"Error", MB_OK | MB_ICONERROR);
        return 1;
    }

    ShowWindow(g_hwnd, ShowWnd);                              //Shows our window
    UpdateWindow(g_hwnd);                                     //Its good to update our window

    return true;
}

int MessageLoop()
{
    MSG msg;                                                //Create a new message structure
    ZeroMemory(&msg, sizeof(MSG));                          //clear message structure to NULL

    while (true)                                            //while there is a message
    {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))       //if there was a windows message
        {
            if (msg.message == WM_QUIT)                     //if the message was WM_QUIT
                break;                                      //Exit the message loop

            TranslateMessage(&msg);                         //Translate the message
            DispatchMessage(&msg);                          //Send the message to default windows procedure
        }
        else                                                //Otherewise, keep the flow going
        {
            UpdateScene();
            DrawScene();
        }
    }

    return (int)msg.wParam;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
    if (!InitializeWindow(hInstance, nShowCmd, g_Width, g_Height, true))
    {
        MessageBox(0, L"Window Initialization - Failed", L"Error", MB_OK);
        return 0;
    }

    if (!InitializeDirect3d11App(hInstance))    //Initialize Direct3D
    {
        MessageBox(0, L"Direct3D Initialization - Failed", L"Error", MB_OK);
        return 0;
    }

    if (!InitScene())    //Initialize our scene
    {
        MessageBox(0, L"Scene Initialization - Failed", L"Error", MB_OK);
        return 0;
    }

    MessageLoop();

    ReleaseObjects();

    return 0;
}