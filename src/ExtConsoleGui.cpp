//////////////////////////////////////////////
//          Ext Console Gui
//	Win32 Window for external console
//////////////////////////////////////////////
#include "pch.h"
#include <windows.h>
#include "resource.h"
#include <stdio.h>
#include <string>
#include <locale>
#include <thread>
#include <chrono>
#include "Console.hpp"
#include "ConfigManager.h"
//#include <iostream> //for debugging
WNDPROC g_wpOrigEditProc;
const char g_szClassName[] = "myWindowClass";
static HWND txtbox_output;
static HWND chkbox_darkmode;
bool isReady = false;
bool startInDarkMode = false;
std::string filename = "s2mp-mod.cfg";
std::string key = "extConDark";

bool ExternalConsoleGui::isExtConGuiReady() {
    return isReady;
}

//helper function to convert std::string to std::wstring
std::wstring stringToWString(const std::string& str) {
    if (str.empty()) {
        return std::wstring();
    }

    //calc number of wide chars to be alloc
    int wchars_num = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, NULL, 0);
    std::wstring wstr(wchars_num, 0);
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, &wstr[0], wchars_num);

    //remove null term
    wstr.pop_back();

    return wstr;
}

void ExternalConsoleGui::print(const std::string& s) {
    if (!txtbox_output) {
        return;
    }

    std::string modifiedString = s + "\r\n";
    std::wstring wstrNewText = stringToWString(modifiedString);
    int textLength = GetWindowTextLength(txtbox_output);
    SendMessage(txtbox_output, EM_SETSEL, textLength, textLength);
    SendMessage(txtbox_output, EM_REPLACESEL, FALSE, (LPARAM)wstrNewText.c_str());
    SendMessage(txtbox_output, EM_SCROLLCARET, 0, 0);
}


void sendToConsole(HWND hwndEdit) {
    wchar_t buffer[1024]; //buffer for text
    GetWindowText(hwndEdit, buffer, sizeof(buffer) / sizeof(wchar_t)); //get text from edit control
    std::wstring wstr(buffer);


    int size_needed = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), (int)wstr.size(), NULL, 0, NULL, NULL);
    std::string str(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), (int)wstr.size(), &str[0], size_needed, NULL, NULL);

    Console::execCmd(str);
    ExternalConsoleGui::print(str);
    SetWindowText(hwndEdit, L"");
}



LRESULT CALLBACK EditSubclassProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_CHAR:
        if (wParam == VK_RETURN) {
            sendToConsole(hwnd);
            return 0;
        }
        break;
    }
    return CallWindowProc(g_wpOrigEditProc, hwnd, msg, wParam, lParam);
}

//window proc
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    static HWND txtbox_input;
    static HWND hwndImage;
    static HFONT hFont;

    switch (msg) {
    case WM_CREATE: {

        //checkbox for dark mode
        chkbox_darkmode = CreateWindowEx(
            0,
            L"BUTTON",
            L"Dark Mode",
            WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX,
            15, 535,
            100, 20,
            hwnd,
            (HMENU)3,
            (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE),
            NULL);

        if (startInDarkMode) {
            SendMessage(chkbox_darkmode, BM_SETCHECK, (WPARAM)BST_CHECKED, 0);
        }

        hwndImage = CreateWindowEx(0, L"STATIC", NULL,
            WS_CHILD | WS_VISIBLE | SS_BITMAP,
            0, 0, 731, 100,
            hwnd, NULL, (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), NULL);
        //HBITMAP hBitmap = (HBITMAP)::LoadImage(GetModuleHandle(L"t6sp-mod.dll"), MAKEINTRESOURCE(IDB_BITMAP1), IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION | LR_SHARED);
        HBITMAP hBitmap = LoadBitmap(GetModuleHandle(L"S2MP-Mod.dll"), MAKEINTRESOURCE(IDB_BITMAP1));
        if (hBitmap == NULL) {
            //std::cout << GetLastError() << std::endl;
            MessageBox(hwnd, L"Could not load external console head (Visual Only)", L"Error", MB_OK | MB_ICONEXCLAMATION);
        }
        SendMessage(hwndImage, STM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)hBitmap);

        txtbox_output = CreateWindowEx(
            NULL,
            L"EDIT",
            L"",
            WS_CHILD | WS_VISIBLE | ES_AUTOVSCROLL | ES_MULTILINE | WS_VSCROLL,
            15, 110,
            700, 200,
            hwnd,
            (HMENU)1,
            (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE),
            NULL);
        if (startInDarkMode) {
            SetBkColor(GetDC(txtbox_output), RGB(71, 71, 71)); //background color to grey for txtbox_output
            SetTextColor(GetDC(txtbox_output), RGB(255, 255, 255));
        }
        hFont = CreateFont(
            14, 0, 0, 0, FW_NORMAL,
            FALSE, FALSE, 0, ANSI_CHARSET,
            OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
            DEFAULT_PITCH | FF_MODERN, L"Consolas");

        SendMessage(txtbox_output, WM_SETFONT, (WPARAM)hFont, TRUE);
        SendMessage(chkbox_darkmode, WM_SETFONT, (WPARAM)hFont, TRUE);

        txtbox_input = CreateWindowEx(
            NULL,
            L"EDIT",
            L"",
            WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL | ES_LEFT,
            15, 515,
            700, 20,
            hwnd,
            (HMENU)2,
            (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE),
            NULL);
        SendMessage(txtbox_input, WM_SETFONT, (WPARAM)hFont, TRUE);
        if (startInDarkMode) {
            SetBkColor(GetDC(txtbox_input), RGB(71, 71, 71)); //background color to grey for txtbox_input
            SetTextColor(GetDC(txtbox_input), RGB(255, 255, 255));
        }

        g_wpOrigEditProc = (WNDPROC)SetWindowLongPtr(txtbox_input, GWLP_WNDPROC, (LONG_PTR)EditSubclassProc);

    }
    break;
    case WM_SIZE: {
        int width = LOWORD(lParam);
        MoveWindow(txtbox_output, 15, 110, width - 30, 400, TRUE);
    }
    break;
    case WM_CTLCOLORBTN: {

        HDC hdc = (HDC)wParam;
        HWND hButton = (HWND)lParam;
        if (hButton == chkbox_darkmode) {
            LRESULT checkState = SendMessage(chkbox_darkmode, BM_GETCHECK, 0, 0);
            if (checkState == BST_CHECKED) {
                SetTextColor(hdc, RGB(255, 255, 255));
                SetBkColor(hdc, RGB(41, 41, 41));
                static HBRUSH hbrBackground = CreateSolidBrush(RGB(41, 41, 41));
                return (INT_PTR)hbrBackground;
            }
            else {
                SetTextColor(hdc, RGB(0, 0, 0));
                SetBkColor(hdc, RGB(240, 240, 240));
                static HBRUSH hbrBackground = CreateSolidBrush(RGB(240, 240, 240));
                return (INT_PTR)hbrBackground;
            }
        }
        break;
    }
    case WM_CTLCOLORSTATIC: {
        HDC hdc = (HDC)wParam;
        HWND hButton = (HWND)lParam;
        LRESULT checkState = SendMessage(chkbox_darkmode, BM_GETCHECK, 0, 0);
        if (checkState == BST_CHECKED) {

            if (hButton == chkbox_darkmode) {
                SetTextColor(hdc, RGB(255, 255, 255));
                SetBkColor(hdc, RGB(41, 41, 41));
                static HBRUSH hbrBackground = CreateSolidBrush(RGB(41, 41, 41));
                return (INT_PTR)hbrBackground;
            }
        }
        else {
            if (hButton == chkbox_darkmode) {
                SetTextColor(hdc, RGB(0, 0, 0));
                SetBkColor(hdc, RGB(240, 240, 240));
                static HBRUSH hbrBackground = CreateSolidBrush(RGB(240, 240, 240));
                return (INT_PTR)hbrBackground;
            }
        }
    }
    case WM_CTLCOLOREDIT: {
        HDC hdcEdit = (HDC)wParam;
        HWND hwndEdit = (HWND)lParam;
        LRESULT checkState = SendMessage(chkbox_darkmode, BM_GETCHECK, 0, 0);

        
        static HBRUSH hbrBackgroundDark = CreateSolidBrush(RGB(71, 71, 71));
        static HBRUSH hbrBackgroundLight = CreateSolidBrush(RGB(255, 255, 255));

        if (checkState == BST_CHECKED) {
            SetTextColor(hdcEdit, RGB(255, 255, 255));
            //SetBkMode(hdcEdit, TRANSPARENT);

            if (hwndEdit == txtbox_output) {
                SetBkColor(hdcEdit, RGB(71, 71, 71));
                SetTextColor(hdcEdit, RGB(255, 255, 255));
                return (INT_PTR)hbrBackgroundDark;
            }
            else if (hwndEdit == txtbox_input) {
                SetBkColor(hdcEdit, RGB(71, 71, 71));
                return (INT_PTR)hbrBackgroundDark;
            }
        }
        else { //light mode
            SetTextColor(hdcEdit, RGB(0, 0, 0));
            //SetBkMode(hdcEdit, TRANSPARENT); 

            if (hwndEdit == txtbox_output) {
                SetBkColor(hdcEdit, RGB(255, 255, 255));
                return (INT_PTR)hbrBackgroundLight;
            }
            else if (hwndEdit == txtbox_input) {
                SetBkColor(hdcEdit, RGB(255, 255, 255));
                return (INT_PTR)hbrBackgroundLight;
            }
        }

        break;
    }

    case WM_COMMAND: {
        int wmId = LOWORD(wParam);
        int wmEvent = HIWORD(wParam);

        switch (wmId) {
        case 3: //dark mode checkbox ID
            if (wmEvent == BN_CLICKED) {
                LRESULT state = SendMessage(chkbox_darkmode, BM_GETCHECK, 0, 0);
                if (state == BST_CHECKED) {
                    SetClassLongPtr(hwnd, GCLP_HBRBACKGROUND, (LONG_PTR)CreateSolidBrush(RGB(41, 41, 41)));
                    ConfigManager::writeConfigValue(filename, key, true);
                }
                else {
                    SetClassLongPtr(hwnd, GCLP_HBRBACKGROUND, (LONG_PTR)CreateSolidBrush(RGB(240, 240, 240)));
                    ConfigManager::writeConfigValue(filename, key, false);
                }
                InvalidateRect(hwnd, NULL, TRUE);
                UpdateWindow(hwnd);
            }
            break;
        }
    }
    break;
    case WM_CLOSE:
        DestroyWindow(hwnd);
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}


int ExternalConsoleGui::init(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, int nCmdShow) {
    if (ConfigManager::configExists(filename)) {
        startInDarkMode = ConfigManager::readConfigValue(filename, key);
    }
    else { //cfg file doesnt exist so store default value
        ConfigManager::writeConfigValue(filename, key, startInDarkMode);
    }

    WNDCLASSEX wc;
    HWND hwnd;
    MSG Msg;
    HICON hIcon = LoadIcon(GetModuleHandle(L"S2MP-Mod.dll"), MAKEINTRESOURCE(IDB_BITMAP1));
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = 0;
    wc.lpfnWndProc = WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = hIcon;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    //wc.hbrBackground;
    if (startInDarkMode) {
        wc.hbrBackground = CreateSolidBrush(RGB(41, 41, 41));
    }
    else {
        wc.hbrBackground = CreateSolidBrush(RGB(240, 240, 240));
    }
    wc.lpszMenuName = NULL;
    wc.lpszClassName = (LPCWSTR)g_szClassName;
    wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

    if (!RegisterClassEx(&wc)) {
        MessageBox(NULL, L"Window Registration Failed!", L"Error!", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    RECT rect = { 0, 0, 725, 235 };
    AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);

    hwnd = CreateWindowEx(
        WS_EX_CLIENTEDGE,
        (LPCWSTR)g_szClassName,
        L"S2MP-Mod External Console",
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        rect.right - rect.left,
        rect.bottom - rect.top + 320,
        NULL, NULL, hInstance, NULL);

    if (hwnd == NULL) {
        MessageBox(NULL, L"Window Creation Failed!", L"Error!", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);
    SendMessage(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
    SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
    isReady = true;
    while (GetMessage(&Msg, NULL, 0, 0) > 0) {
        TranslateMessage(&Msg);
        DispatchMessage(&Msg);
    }

    return Msg.wParam;
}