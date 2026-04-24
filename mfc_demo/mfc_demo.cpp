// mfc_demo.cpp : 定义应用程序的入口点。
//

#include "framework.h"
#include "mfc_demo.h"
#include <opencv2/opencv.hpp>
#include <string>

#define MAX_LOADSTRING 100

HINSTANCE hInst;
WCHAR szTitle[MAX_LOADSTRING];
WCHAR szWindowClass[MAX_LOADSTRING];

ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

static cv::Mat g_originalImage;
static int g_thresholdRed = 127;
static int g_thresholdGreen = 127;
static int g_thresholdBlue = 127;
static int g_thresholdGray = 127;

static HWND g_hStaticRed = NULL;
static HWND g_hStaticGreen = NULL;
static HWND g_hStaticBlue = NULL;
static HWND g_hStaticGray = NULL;
static HWND g_hSliderRed = NULL;
static HWND g_hSliderGreen = NULL;
static HWND g_hSliderBlue = NULL;
static HWND g_hSliderGray = NULL;
static HWND g_hValueRed = NULL;
static HWND g_hValueGreen = NULL;
static HWND g_hValueBlue = NULL;
static HWND g_hValueGray = NULL;

void CreateChildWindows(HWND hWnd);
void LoadImageFile(HWND hWnd);
void UpdateAllDisplays(HWND hWnd);
void DisplayChannelImage(HWND hStatic, const cv::Mat& channel, int threshold);
cv::Mat ApplyThreshold(const cv::Mat& channel, int threshold);
HBITMAP MatToHBITMAP(const cv::Mat& mat);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_MFCDEMO, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MFCDEMO));

    MSG msg;

    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MFCDEMO));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_MFCDEMO);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance;

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, 1200, 800, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   CreateChildWindows(hWnd);

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

void CreateChildWindows(HWND hWnd)
{
    int clientWidth = 1180;
    int clientHeight = 720;
    
    int margin = 20;
    int spacing = 15;
    int labelHeight = 25;
    int sliderHeight = 40;
    int valueWidth = 50;
    
    int displayWidth = (clientWidth - 2 * margin - 3 * spacing) / 2;
    int displayHeight = (clientHeight - 2 * margin - labelHeight - sliderHeight - 2 * spacing) / 2;
    
    int topRowY = margin;
    int bottomRowY = margin + displayHeight + labelHeight + sliderHeight + 2 * spacing;
    
    int leftColX = margin;
    int rightColX = margin + displayWidth + spacing;
    
    g_hStaticRed = CreateWindowExW(
        WS_EX_CLIENTEDGE, L"STATIC", NULL,
        WS_CHILD | WS_VISIBLE | SS_BITMAP | SS_CENTERIMAGE,
        leftColX, topRowY, displayWidth, displayHeight,
        hWnd, (HMENU)IDC_STATIC_RED, hInst, NULL);
    
    CreateWindowW(
        L"STATIC", L"红通道 (R):",
        WS_CHILD | WS_VISIBLE | SS_LEFT,
        leftColX, topRowY + displayHeight + 5, displayWidth - valueWidth - 10, labelHeight,
        hWnd, (HMENU)IDC_LABEL_RED, hInst, NULL);
    
    g_hValueRed = CreateWindowW(
        L"STATIC", L"127",
        WS_CHILD | WS_VISIBLE | SS_RIGHT | SS_CENTERIMAGE,
        leftColX + displayWidth - valueWidth, topRowY + displayHeight + 5, valueWidth, labelHeight,
        hWnd, (HMENU)IDC_VALUE_RED, hInst, NULL);
    
    g_hSliderRed = CreateWindowExW(
        0, TRACKBAR_CLASSW, NULL,
        WS_CHILD | WS_VISIBLE | TBS_AUTOTICKS | TBS_ENABLESELRANGE,
        leftColX, topRowY + displayHeight + labelHeight + 5, displayWidth, sliderHeight,
        hWnd, (HMENU)IDC_SLIDER_RED, hInst, NULL);
    SendMessageW(g_hSliderRed, TBM_SETRANGE, TRUE, MAKELONG(0, 255));
    SendMessageW(g_hSliderRed, TBM_SETPOS, TRUE, 127);
    SendMessageW(g_hSliderRed, TBM_SETTICFREQ, 32, 0);
    
    g_hStaticGreen = CreateWindowExW(
        WS_EX_CLIENTEDGE, L"STATIC", NULL,
        WS_CHILD | WS_VISIBLE | SS_BITMAP | SS_CENTERIMAGE,
        rightColX, topRowY, displayWidth, displayHeight,
        hWnd, (HMENU)IDC_STATIC_GREEN, hInst, NULL);
    
    CreateWindowW(
        L"STATIC", L"绿通道 (G):",
        WS_CHILD | WS_VISIBLE | SS_LEFT,
        rightColX, topRowY + displayHeight + 5, displayWidth - valueWidth - 10, labelHeight,
        hWnd, (HMENU)IDC_LABEL_GREEN, hInst, NULL);
    
    g_hValueGreen = CreateWindowW(
        L"STATIC", L"127",
        WS_CHILD | WS_VISIBLE | SS_RIGHT | SS_CENTERIMAGE,
        rightColX + displayWidth - valueWidth, topRowY + displayHeight + 5, valueWidth, labelHeight,
        hWnd, (HMENU)IDC_VALUE_GREEN, hInst, NULL);
    
    g_hSliderGreen = CreateWindowExW(
        0, TRACKBAR_CLASSW, NULL,
        WS_CHILD | WS_VISIBLE | TBS_AUTOTICKS | TBS_ENABLESELRANGE,
        rightColX, topRowY + displayHeight + labelHeight + 5, displayWidth, sliderHeight,
        hWnd, (HMENU)IDC_SLIDER_GREEN, hInst, NULL);
    SendMessageW(g_hSliderGreen, TBM_SETRANGE, TRUE, MAKELONG(0, 255));
    SendMessageW(g_hSliderGreen, TBM_SETPOS, TRUE, 127);
    SendMessageW(g_hSliderGreen, TBM_SETTICFREQ, 32, 0);
    
    g_hStaticBlue = CreateWindowExW(
        WS_EX_CLIENTEDGE, L"STATIC", NULL,
        WS_CHILD | WS_VISIBLE | SS_BITMAP | SS_CENTERIMAGE,
        leftColX, bottomRowY, displayWidth, displayHeight,
        hWnd, (HMENU)IDC_STATIC_BLUE, hInst, NULL);
    
    CreateWindowW(
        L"STATIC", L"蓝通道 (B):",
        WS_CHILD | WS_VISIBLE | SS_LEFT,
        leftColX, bottomRowY + displayHeight + 5, displayWidth - valueWidth - 10, labelHeight,
        hWnd, (HMENU)IDC_LABEL_BLUE, hInst, NULL);
    
    g_hValueBlue = CreateWindowW(
        L"STATIC", L"127",
        WS_CHILD | WS_VISIBLE | SS_RIGHT | SS_CENTERIMAGE,
        leftColX + displayWidth - valueWidth, bottomRowY + displayHeight + 5, valueWidth, labelHeight,
        hWnd, (HMENU)IDC_VALUE_BLUE, hInst, NULL);
    
    g_hSliderBlue = CreateWindowExW(
        0, TRACKBAR_CLASSW, NULL,
        WS_CHILD | WS_VISIBLE | TBS_AUTOTICKS | TBS_ENABLESELRANGE,
        leftColX, bottomRowY + displayHeight + labelHeight + 5, displayWidth, sliderHeight,
        hWnd, (HMENU)IDC_SLIDER_BLUE, hInst, NULL);
    SendMessageW(g_hSliderBlue, TBM_SETRANGE, TRUE, MAKELONG(0, 255));
    SendMessageW(g_hSliderBlue, TBM_SETPOS, TRUE, 127);
    SendMessageW(g_hSliderBlue, TBM_SETTICFREQ, 32, 0);
    
    g_hStaticGray = CreateWindowExW(
        WS_EX_CLIENTEDGE, L"STATIC", NULL,
        WS_CHILD | WS_VISIBLE | SS_BITMAP | SS_CENTERIMAGE,
        rightColX, bottomRowY, displayWidth, displayHeight,
        hWnd, (HMENU)IDC_STATIC_GRAY, hInst, NULL);
    
    CreateWindowW(
        L"STATIC", L"灰度通道:",
        WS_CHILD | WS_VISIBLE | SS_LEFT,
        rightColX, bottomRowY + displayHeight + 5, displayWidth - valueWidth - 10, labelHeight,
        hWnd, (HMENU)IDC_LABEL_GRAY, hInst, NULL);
    
    g_hValueGray = CreateWindowW(
        L"STATIC", L"127",
        WS_CHILD | WS_VISIBLE | SS_RIGHT | SS_CENTERIMAGE,
        rightColX + displayWidth - valueWidth, bottomRowY + displayHeight + 5, valueWidth, labelHeight,
        hWnd, (HMENU)IDC_VALUE_GRAY, hInst, NULL);
    
    g_hSliderGray = CreateWindowExW(
        0, TRACKBAR_CLASSW, NULL,
        WS_CHILD | WS_VISIBLE | TBS_AUTOTICKS | TBS_ENABLESELRANGE,
        rightColX, bottomRowY + displayHeight + labelHeight + 5, displayWidth, sliderHeight,
        hWnd, (HMENU)IDC_SLIDER_GRAY, hInst, NULL);
    SendMessageW(g_hSliderGray, TBM_SETRANGE, TRUE, MAKELONG(0, 255));
    SendMessageW(g_hSliderGray, TBM_SETPOS, TRUE, 127);
    SendMessageW(g_hSliderGray, TBM_SETTICFREQ, 32, 0);
}

void LoadImageFile(HWND hWnd)
{
    OPENFILENAMEW ofn;
    WCHAR szFileName[MAX_PATH] = {0};
    
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hWnd;
    ofn.lpstrFile = szFileName;
    ofn.nMaxFile = sizeof(szFileName) / sizeof(WCHAR);
    ofn.lpstrFilter = L"图片文件\0*.jpg;*.jpeg;*.png\0JPEG文件\0*.jpg;*.jpeg\0PNG文件\0*.png\0所有文件\0*.*\0";
    ofn.nFilterIndex = 1;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
    
    if (GetOpenFileNameW(&ofn) == TRUE)
    {
        std::wstring wPath(szFileName);
        std::string path(wPath.begin(), wPath.end());
        
        g_originalImage = cv::imread(path, cv::IMREAD_COLOR);
        
        if (g_originalImage.empty())
        {
            MessageBoxW(hWnd, L"无法加载图片文件，请确认文件格式正确。", L"错误", MB_ICONERROR | MB_OK);
            return;
        }
        
        g_thresholdRed = 127;
        g_thresholdGreen = 127;
        g_thresholdBlue = 127;
        g_thresholdGray = 127;
        
        SendMessageW(g_hSliderRed, TBM_SETPOS, TRUE, 127);
        SendMessageW(g_hSliderGreen, TBM_SETPOS, TRUE, 127);
        SendMessageW(g_hSliderBlue, TBM_SETPOS, TRUE, 127);
        SendMessageW(g_hSliderGray, TBM_SETPOS, TRUE, 127);
        
        SetWindowTextW(g_hValueRed, L"127");
        SetWindowTextW(g_hValueGreen, L"127");
        SetWindowTextW(g_hValueBlue, L"127");
        SetWindowTextW(g_hValueGray, L"127");
        
        UpdateAllDisplays(hWnd);
    }
}

void UpdateAllDisplays(HWND hWnd)
{
    if (g_originalImage.empty())
        return;
    
    std::vector<cv::Mat> channels;
    cv::split(g_originalImage, channels);
    
    cv::Mat blueChannel = channels[0];
    cv::Mat greenChannel = channels[1];
    cv::Mat redChannel = channels[2];
    
    cv::Mat grayChannel;
    cv::cvtColor(g_originalImage, grayChannel, cv::COLOR_BGR2GRAY);
    
    DisplayChannelImage(g_hStaticRed, redChannel, g_thresholdRed);
    DisplayChannelImage(g_hStaticGreen, greenChannel, g_thresholdGreen);
    DisplayChannelImage(g_hStaticBlue, blueChannel, g_thresholdBlue);
    DisplayChannelImage(g_hStaticGray, grayChannel, g_thresholdGray);
}

cv::Mat ApplyThreshold(const cv::Mat& channel, int threshold)
{
    cv::Mat result;
    cv::threshold(channel, result, threshold, 255, cv::THRESH_BINARY);
    return result;
}

void DisplayChannelImage(HWND hStatic, const cv::Mat& channel, int threshold)
{
    if (hStatic == NULL || channel.empty())
        return;
    
    cv::Mat thresholded = ApplyThreshold(channel, threshold);
    
    cv::Mat displayMat;
    if (thresholded.channels() == 1)
    {
        cv::cvtColor(thresholded, displayMat, cv::COLOR_GRAY2BGR);
    }
    else
    {
        displayMat = thresholded.clone();
    }
    
    RECT rect;
    GetClientRect(hStatic, &rect);
    int displayWidth = rect.right - rect.left;
    int displayHeight = rect.bottom - rect.top;
    
    if (displayWidth <= 0 || displayHeight <= 0)
    {
        displayWidth = 400;
        displayHeight = 300;
    }
    
    cv::Mat resized;
    double scaleX = static_cast<double>(displayWidth) / displayMat.cols;
    double scaleY = static_cast<double>(displayHeight) / displayMat.rows;
    double scale = std::min(scaleX, scaleY);
    
    if (scale > 0)
    {
        cv::resize(displayMat, resized, cv::Size(), scale, scale, cv::INTER_AREA);
    }
    else
    {
        resized = displayMat;
    }
    
    HBITMAP hBitmap = MatToHBITMAP(resized);
    if (hBitmap != NULL)
    {
        HBITMAP hOldBitmap = (HBITMAP)SendMessageW(hStatic, STM_GETIMAGE, IMAGE_BITMAP, 0);
        SendMessageW(hStatic, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hBitmap);
        
        if (hOldBitmap != NULL)
        {
            DeleteObject(hOldBitmap);
        }
    }
}

HBITMAP MatToHBITMAP(const cv::Mat& mat)
{
    if (mat.empty())
        return NULL;
    
    cv::Mat matCopy;
    if (mat.channels() == 3)
    {
        cv::cvtColor(mat, matCopy, cv::COLOR_BGR2BGRA);
    }
    else if (mat.channels() == 1)
    {
        cv::cvtColor(mat, matCopy, cv::COLOR_GRAY2BGRA);
    }
    else
    {
        matCopy = mat.clone();
    }
    
    BITMAPINFOHEADER bi = {0};
    bi.biSize = sizeof(BITMAPINFOHEADER);
    bi.biWidth = matCopy.cols;
    bi.biHeight = -matCopy.rows;
    bi.biPlanes = 1;
    bi.biBitCount = 32;
    bi.biCompression = BI_RGB;
    
    HDC hdc = GetDC(NULL);
    void* pBits = NULL;
    HBITMAP hBitmap = CreateDIBSection(hdc, (BITMAPINFO*)&bi, DIB_RGB_COLORS, &pBits, NULL, 0);
    ReleaseDC(NULL, hdc);
    
    if (hBitmap == NULL)
        return NULL;
    
    int pitch = matCopy.cols * 4;
    for (int y = 0; y < matCopy.rows; y++)
    {
        memcpy((BYTE*)pBits + y * pitch, matCopy.ptr(y), pitch);
    }
    
    return hBitmap;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            case IDM_OPEN:
                LoadImageFile(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_HSCROLL:
        {
            HWND hSlider = (HWND)lParam;
            int pos = SendMessageW(hSlider, TBM_GETPOS, 0, 0);
            WCHAR szValue[10];
            
            if (hSlider == g_hSliderRed)
            {
                g_thresholdRed = pos;
                swprintf_s(szValue, L"%d", pos);
                SetWindowTextW(g_hValueRed, szValue);
                if (!g_originalImage.empty())
                {
                    std::vector<cv::Mat> channels;
                    cv::split(g_originalImage, channels);
                    DisplayChannelImage(g_hStaticRed, channels[2], g_thresholdRed);
                }
            }
            else if (hSlider == g_hSliderGreen)
            {
                g_thresholdGreen = pos;
                swprintf_s(szValue, L"%d", pos);
                SetWindowTextW(g_hValueGreen, szValue);
                if (!g_originalImage.empty())
                {
                    std::vector<cv::Mat> channels;
                    cv::split(g_originalImage, channels);
                    DisplayChannelImage(g_hStaticGreen, channels[1], g_thresholdGreen);
                }
            }
            else if (hSlider == g_hSliderBlue)
            {
                g_thresholdBlue = pos;
                swprintf_s(szValue, L"%d", pos);
                SetWindowTextW(g_hValueBlue, szValue);
                if (!g_originalImage.empty())
                {
                    std::vector<cv::Mat> channels;
                    cv::split(g_originalImage, channels);
                    DisplayChannelImage(g_hStaticBlue, channels[0], g_thresholdBlue);
                }
            }
            else if (hSlider == g_hSliderGray)
            {
                g_thresholdGray = pos;
                swprintf_s(szValue, L"%d", pos);
                SetWindowTextW(g_hValueGray, szValue);
                if (!g_originalImage.empty())
                {
                    cv::Mat grayChannel;
                    cv::cvtColor(g_originalImage, grayChannel, cv::COLOR_BGR2GRAY);
                    DisplayChannelImage(g_hStaticGray, grayChannel, g_thresholdGray);
                }
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        {
            HBITMAP hBitmap;
            hBitmap = (HBITMAP)SendMessageW(g_hStaticRed, STM_GETIMAGE, IMAGE_BITMAP, 0);
            if (hBitmap) DeleteObject(hBitmap);
            hBitmap = (HBITMAP)SendMessageW(g_hStaticGreen, STM_GETIMAGE, IMAGE_BITMAP, 0);
            if (hBitmap) DeleteObject(hBitmap);
            hBitmap = (HBITMAP)SendMessageW(g_hStaticBlue, STM_GETIMAGE, IMAGE_BITMAP, 0);
            if (hBitmap) DeleteObject(hBitmap);
            hBitmap = (HBITMAP)SendMessageW(g_hStaticGray, STM_GETIMAGE, IMAGE_BITMAP, 0);
            if (hBitmap) DeleteObject(hBitmap);
            
            g_originalImage.release();
            PostQuitMessage(0);
        }
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
