#include <windows.h>
#include <winuser.h>
#include "wtypes.h"
#include <math.h>
#include "commdlg.h"
#include <CommCtrl.h>
#include <scrnsave.h>
//#include <Wingdi.h>
#include <gdiplus.h>
//
//#include <iostream>

#include "resource.h"

#pragma comment (lib, "Comdlg32.lib")
#pragma comment (lib, "comctl32.lib")
//#pragma comment (lib, "scrnsavw.lib")
#pragma comment (lib, "Scrnsave.lib")
//#pragma comment (lib, "Gdi32.lib")
#pragma comment(lib, "gdiplus.lib")


using namespace Gdiplus;

#define PI       3.14159265358979323846   // pi
#define TIMER   1
extern HINSTANCE    hMainInstance;      // screen saver instance handle  
extern BOOL         fChildPreview;   

static bool g_start_flag = TRUE;
static bool g_clear_flag = TRUE;

static COLORREF defcolor;
static COLORREF bg_color;
static COLORREF hr_color;
static COLORREF d1_color;
static COLORREF mn_color;
static COLORREF d2_color;
static COLORREF sc_color;
static COLORREF wd_color;
static COLORREF md_color;
static COLORREF cs_color;
static COLORREF cf_color;

static bool antialias = TRUE;
static int dot_blink = 1;
static int digit_second = 1;
static int circle_second = 1;
static int show_date = 1;

static int monnum = 1;

void LoadSaveSettings(BOOL do_save);

void set_grey_theme()
{
    defcolor = 0x00535353;
    bg_color = 0x00000000;
    hr_color = 0x00353535;
    d1_color = 0x00232323;
    mn_color = 0x00353535;
    d2_color = 0x00232323;
    sc_color = 0x00232323;
    wd_color = 0x00323232;
    md_color = 0x00282828;
    cs_color = 0x00232323;
    cf_color = 0x002e2e2e;
}

void set_color_theme()
{
    defcolor = 0x00535353;
    bg_color = 0x00000000;
    hr_color = 0x00334514;
    d1_color = 0x00313131;
    mn_color = 0x00334514;
    d2_color = 0x00313131;
    sc_color = 0x002c5434;
    wd_color = 0x000c3026;
    md_color = 0x00313131;
    cs_color = 0x00144a24;
    cf_color = 0x002c5434;
}

static SYSTEMTIME st;

struct DMONPRM {
    int w_w;            // Display width (px)
    int w_h;            // Display height (px)
    int d_size;         // Dot size (px)
    int b_step;         // Step dots(px)
    int d_radius;       //
    int wr;             // Second circle radius (px)
    int xh;             // Center screen X (px)
    int yh;             // Center Screen Y (px)
    int y_pos;          // Y coordinates Clock line
    int ywd_pos;        // Y coordinates Date line
    int d_ch_height;    // Char height (px)
    int d_ch_width;     // Char width (px)
};

struct CHARCRD {
    int x_h_d1;         // Hour digit X coordinates (px)
    int x_h_d2;         // Hour digit X coordinates (px)
    int x_m_d1;         // Minute digit1 X coordinates (px)
    int x_m_d2;         // Minute digit2 X coordinates (px)
    int x_s_d1;         // Second digit1 X coordinates (px)
    int x_s_d2;         // Second digit2 X coordinates (px)
    int x_dot1;         // Dots X coordinates (px)
    int x_dot2;         // Dots X coordinates (px)
    int x_wday;         // Day of the week X coordinates (px)
    int x_mday1;        // Day of month digit1 X coordinates (px)
    int x_mday2;        // Day of month digit2 X coordinates (px)
};

static char digit_matrix[451] = "011101000110001100011000110001100011000101110000110000100001000010000100001000010000100001011101000100001000010111010000100001000011111011101000100001000010111000001000011000101110100011000110001100011111100001000010000100001111111000010000100001111000001000010000111110011101000110000100001111010001100011000101110111111000100001000010001000100010000100001000011101000110001100010111010001100011000101110011101000110001100010111100001000010000101110";
static char w_deys_matrix[246] = "11101111010100111010010101001110111111010110101011010111101010110101011110111101001011100101010010111001011101111000101100011110001001110100101011110100101110010001001000100101110111101001010100101010010101001011101111000100100011110001011110111";
static char m_deys_matrix[281] = "1111100110011001100110011111001100010001000100010001000111110001000111111000100011111111000100010111000100011111100110011001111100010001000111111000100011110001000111111111100010001111100110011111111110010001001001000100010011111001100111111001100111111111100110011111000100011111";
static char dots_matrix[91] = "000000000000100000000000000000001000000000000000000000000000001000000000100000000000000000";

static int dc = 0;

void LoadSaveSettings(BOOL do_save)
{
    HKEY key;
    DWORD disposition;
    DWORD type = REG_DWORD, size = sizeof(REG_DWORD);

    if (RegCreateKeyEx(HKEY_CURRENT_USER, "Software\\ScreenSavers\\stClock", 0, NULL, 0, KEY_WRITE | KEY_READ, NULL, &key, &disposition) == ERROR_SUCCESS)
    {
        if (do_save)
        {
            RegSetValueEx(key, "hour.color", 0, type, (PBYTE)&hr_color, size);
            RegSetValueEx(key, "minut.color", 0, type, (PBYTE)&mn_color, size);
            RegSetValueEx(key, "second.color", 0, type, (PBYTE)&sc_color, size);
            RegSetValueEx(key, "blink_dot_min.color", 0, type, (PBYTE)&d1_color, size);
            RegSetValueEx(key, "blink_dot_sec.color", 0, type, (PBYTE)&d2_color, size);
            RegSetValueEx(key, "background.color", 0, type, (PBYTE)&bg_color, size);
            RegSetValueEx(key, "week_day.color", 0, type, (PBYTE)&wd_color, size);
            RegSetValueEx(key, "month_day.color", 0, type, (PBYTE)&md_color, size);
            RegSetValueEx(key, "cirlce_second.color", 0, type, (PBYTE)&cs_color, size);
            RegSetValueEx(key, "cirlce_5second.color", 0, type, (PBYTE)&cf_color, size);

            RegSetValueEx(key, "dot_blink", 0, type, (PBYTE)&dot_blink, size);
            RegSetValueEx(key, "digit_second", 0, type, (PBYTE)&digit_second, size);
            RegSetValueEx(key, "circle_second", 0, type, (PBYTE)&circle_second, size);
            RegSetValueEx(key, "show_date", 0, type, (PBYTE)&show_date, size);
            RegSetValueEx(key, "antialias", 0, type, (PBYTE)&antialias, size);
        }
        else
        {
            RegQueryValueEx(key, "hour.color", 0, &type, (PBYTE)&hr_color, &size);
            RegQueryValueEx(key, "minut.color", 0, &type, (PBYTE)&mn_color, &size);
            RegQueryValueEx(key, "second.color", 0, &type, (PBYTE)&sc_color, &size);
            RegQueryValueEx(key, "blink_dot_min.color", 0, &type, (PBYTE)&d1_color, &size);
            RegQueryValueEx(key, "blink_dot_sec.color", 0, &type, (PBYTE)&d2_color, &size);
            RegQueryValueEx(key, "background.color", 0, &type, (PBYTE)&bg_color, &size);
            RegQueryValueEx(key, "week_day.color", 0, &type, (PBYTE)&wd_color, &size);
            RegQueryValueEx(key, "month_day.color", 0, &type, (PBYTE)&md_color, &size);
            RegQueryValueEx(key, "cirlce_second.color", 0, &type, (PBYTE)&cs_color, &size);
            RegQueryValueEx(key, "cirlce_5second.color", 0, &type, (PBYTE)&cf_color, &size);

            RegQueryValueEx(key, "dot_blink", 0, &type, (PBYTE)&dot_blink, &size);
            RegQueryValueEx(key, "digit_second", 0, &type, (PBYTE)&digit_second, &size);
            RegQueryValueEx(key, "circle_second", 0, &type, (PBYTE)&circle_second, &size);
            RegQueryValueEx(key, "show_date", 0, &type, (PBYTE)&show_date, &size);
            RegQueryValueEx(key, "antialias", 0, &type, (PBYTE)&antialias, &size);
        }

        RegCloseKey(key);
    }
}


void draw_dot(HDC *hdc, int x, int y, DMONPRM dp, COLORREF *c)
{
    
    if (fChildPreview) {
        SetPixelV(*hdc, x, y, *c);
    }
    else {
        Graphics g(*hdc);
        ::Color p, b;
        p.SetFromCOLORREF(bg_color);
        b.SetFromCOLORREF(*c);
        if (antialias)
        {
            g.SetSmoothingMode(SmoothingModeAntiAlias);
        }
        
        ::Pen pen(p, 1);
        ::SolidBrush brush(b);

        g.FillEllipse(&brush, x, y, dp.d_size, dp.d_size);
        g.DrawEllipse(&pen, x, y, dp.d_size, dp.d_size);
    }
}

void draw_symbol(HDC *hdc, RECT &l_rc, unsigned int m, int *x, int *y, COLORREF *d_color, DMONPRM *dp, char *p_matrix, int cel = 5, int row = 9)
{

    HBRUSH  bgbrush;
    HGDIOBJ hbrushOld;

    bgbrush = CreateSolidBrush(bg_color);

    if (g_clear_flag)       // Draw Background
    {
        hbrushOld = SelectObject(*hdc, bgbrush);
        FillRect(*hdc, &l_rc, bgbrush);
        SelectObject(*hdc, hbrushOld);
        g_clear_flag = FALSE;
    }


    HDC hDCMem = CreateCompatibleDC(*hdc);

    RECT rect;

    rect.left = 0;
    rect.top = 0;
    rect.right = dp->b_step * cel;
    rect.bottom = dp->b_step * row;

    HBITMAP hBmp = CreateCompatibleBitmap(*hdc, rect.right, rect.bottom);
    HGDIOBJ hTmp = SelectObject(hDCMem, hBmp);

    hbrushOld = SelectObject(hDCMem, bgbrush);

    //FillRect(hDCMem, &rect, bgbrush);

    SelectObject(hDCMem, hbrushOld);
    DeleteObject(bgbrush);

    int bm_x = 0;
    int bm_y = 0;
    int counter;
    char *p_2char = &p_matrix[cel * row * m];
    char *p_bm = p_2char;

    while (1)
    {
        counter = 0;
        do {
            if (p_bm[counter] != 48)
            {
              draw_dot(&hDCMem, bm_x, bm_y, *dp, d_color);
            }
            bm_x += dp->b_step;
            ++counter;
        } while (counter < cel);
        if (!--row)
            break;
        bm_y += dp->b_step;
        p_bm += cel;
        bm_x = 0;
        counter = 0;
    }
    BitBlt(*hdc, (*x) + l_rc.left, (*y) + l_rc.top, rect.right, rect.bottom, hDCMem, 0, 0, SRCCOPY);

    SelectObject(*hdc, hTmp);
    DeleteObject(hBmp);
    DeleteDC(hDCMem);
}

void draw_second_circle(HDC *hdc, int m, DMONPRM *dp, RECT &l_rc)
{
    double xdr, ydr;

    dp->wr = (digit_second) ? dp->b_step * 28 : dp->b_step * 20;

    int count = m;
    if (g_start_flag || fChildPreview) // preview fix
        count = 0;

    do {
        int a = count + 45;
        double r = a * 6 * PI / 180;
        xdr = dp->xh + (dp->wr * cos(r)) - dp->d_radius;
        ydr = dp->yh + (dp->wr * sin(r)) - dp->d_radius;

        COLORREF dcolor = cf_color;
        if (count % 5) 
        {
            dcolor = cs_color;
        }

        draw_dot(hdc, (int)xdr + l_rc.left, (int)ydr + l_rc.top, *dp, &dcolor);

        if (count % 5 == 0)
        {
            xdr = dp->xh + ((dp->wr - dp->b_step) * cos(r)) - dp->d_radius;
            ydr = dp->yh + ((dp->wr - dp->b_step) * sin(r)) - dp->d_radius;
            draw_dot(hdc, (int)xdr, (int)ydr, *dp, &dcolor);

            xdr = dp->xh + ((dp->wr - dp->b_step * 2) * cos(r)) - dp->d_radius;
            ydr = dp->yh + ((dp->wr - dp->b_step * 2) * sin(r)) - dp->d_radius;
            draw_dot(hdc, (int)xdr, (int)ydr, *dp, &dcolor);
        }
        ++count;
    } while (count <= m);
}

BOOL CALLBACK MyPaintEnumProc(
    HMONITOR hMonitor,    // handle to display monitor
    HDC hdc,              // handle to monitor DC
    LPRECT lprcMonitor,   // monitor intersection rectangle
    LPARAM data           // data
    )
{
    RECT rc = *lprcMonitor;

    DMONPRM dp;
    CHARCRD crd;

/////////////////////////////////////////////////////////////////////

    if (fChildPreview)
        {
            if (digit_second)
            {
                dp.w_w = 160;
                dp.w_h = 160;
            }
            else
            {
                dp.w_w = 152;
                dp.w_h = 120;
            }
        }
    else
    {
        dp.w_w = GetDeviceCaps(hdc, HORZRES);
        dp.w_h = GetDeviceCaps(hdc, VERTRES);
    }

    dp.xh = dp.w_w / 2;
    dp.yh = dp.w_h / 2;

    dp.w_w = (dp.w_w < dp.w_h) ? dp.w_w : dp.w_h;

    dp.d_size = (digit_second) ? dp.w_w / 80 : dp.w_w / 60;  // dot size

    dp.d_radius = dp.d_size / 2;

    dp.b_step = dp.d_size + dp.d_size / 5;

    dp.d_ch_width = dp.b_step * 5;
    dp.d_ch_height = dp.b_step * 9;

    if (show_date)
    {
        dp.y_pos = dp.yh - dp.d_ch_height / 2 - dp.b_step * 3;
        dp.ywd_pos = dp.b_step * 11 + dp.y_pos;
    } else {
        dp.y_pos = dp.yh - dp.d_ch_height / 2;
    }

    if (digit_second)
    {
        crd.x_h_d1 = dp.xh - dp.d_ch_width * 4 - dp.d_size * 3;
        crd.x_h_d2 = crd.x_h_d1 + dp.d_ch_width + dp.d_size * 2;
        crd.x_dot1 = crd.x_h_d2 + dp.d_ch_width;
        crd.x_m_d1 = crd.x_dot1 + dp.d_ch_width;
        crd.x_m_d2 = dp.xh + dp.d_size;
        crd.x_dot2 = crd.x_m_d2 + dp.d_ch_width;
        crd.x_s_d1 = crd.x_dot2 + dp.d_ch_width;
        crd.x_s_d2 = crd.x_s_d1 + dp.d_ch_width + dp.d_size * 2;
        crd.x_wday = crd.x_h_d2;
    }
    else
    {
        crd.x_h_d1 = dp.xh - dp.d_ch_width * 3;
        crd.x_h_d2 = crd.x_h_d1 + dp.d_ch_width * 1.4;
        crd.x_dot1 = dp.xh - dp.d_ch_width / 2;
        crd.x_m_d2 = dp.xh + dp.d_ch_width * 2;
        crd.x_m_d1 = crd.x_m_d2 - dp.d_ch_width * 1.4;
        crd.x_wday = crd.x_h_d2 - dp.d_size * 2;
    }

    crd.x_mday1 = crd.x_wday + dp.b_step * 9;
    crd.x_mday2 = crd.x_mday1 + dp.b_step * 5;

/////////////////////////////////////////////////////////////////////


    // you have the rect which has coordinates of the monitor
    if (!st.wSecond || g_start_flag || fChildPreview)
    {
        g_clear_flag = TRUE;

        draw_symbol(&hdc, rc, st.wHour / 10, &crd.x_h_d1, &dp.y_pos, &hr_color, &dp, digit_matrix);
        draw_symbol(&hdc, rc, st.wHour % 10, &crd.x_h_d2, &dp.y_pos, &hr_color, &dp, digit_matrix);

        draw_symbol(&hdc, rc, st.wMinute / 10, &crd.x_m_d1, &dp.y_pos, &mn_color, &dp, digit_matrix);
        draw_symbol(&hdc, rc, st.wMinute % 10, &crd.x_m_d2, &dp.y_pos, &mn_color, &dp, digit_matrix);

        if (show_date)
        {
            draw_symbol(&hdc, rc, st.wDayOfWeek, &crd.x_wday, &dp.ywd_pos, &wd_color, &dp, w_deys_matrix, 7, 5);
            draw_symbol(&hdc, rc, st.wDay / 10, &crd.x_mday1, &dp.ywd_pos, &md_color, &dp, m_deys_matrix, 4, 7);
            draw_symbol(&hdc, rc, st.wDay % 10, &crd.x_mday2, &dp.ywd_pos, &md_color, &dp, m_deys_matrix, 4, 7);
        }
    }

    if (digit_second)
    {
        draw_symbol(&hdc, rc, st.wSecond / 10, &crd.x_s_d1, &dp.y_pos, &sc_color, &dp, digit_matrix);
        draw_symbol(&hdc, rc, st.wSecond % 10, &crd.x_s_d2, &dp.y_pos, &sc_color, &dp, digit_matrix);
    }

    if (dot_blink)
    {
        dc = st.wSecond % 2;
    }

    draw_symbol(&hdc, rc, dc, &crd.x_dot1, &dp.y_pos, &d1_color, &dp, dots_matrix);

    if (digit_second)
    {
        draw_symbol(&hdc, rc, dc, &crd.x_dot2, &dp.y_pos, &d2_color, &dp, dots_matrix);
    }

    if (circle_second)
    {
        draw_second_circle(&hdc, st.wSecond, &dp, rc);
    }
    // Draw here now
    return 1;
}
//Required Function
LRESULT WINAPI ScreenSaverProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    static HDC          hdc;      // device-context handle  
    static RECT         rc;       // RECT structure  
    static UINT         uTimer;   // timer identifier

    static PAINTSTRUCT ps;

    switch (message)
    {
        case WM_CREATE:

            set_grey_theme();
            g_start_flag = TRUE;
            LoadSaveSettings(FALSE);
            GetClientRect(hWnd, &rc);

            //int monnum = GetSystemMetrics (SM_CMONITORS);

            uTimer = SetTimer(hWnd, TIMER, 1000, NULL);

            break;

        case WM_TIMER:
            switch (wParam)
            {
            case TIMER:
                RedrawWindow(hWnd, &rc, NULL, RDW_INVALIDATE);
            //  InvalidateRect(hWnd, &rc, FALSE);
                break;
            }
            break;

        case WM_PAINT:

            hdc = BeginPaint(hWnd, &ps);
            GetLocalTime(&st);
            EnumDisplayMonitors(hdc, NULL, MyPaintEnumProc, 0);
            g_start_flag = FALSE;
            EndPaint(hWnd, &ps);
            break;

        case WM_DESTROY:
            if (uTimer)
                KillTimer(hWnd, uTimer);
            PostQuitMessage(0);
        break;

        default:
            return DefScreenSaverProc(hWnd, message, wParam, lParam);
    }
    
    GdiplusShutdown(gdiplusToken);
    return 0;
}

COLORREF SelectColor(HWND parent, COLORREF clr) {

    static COLORREF CustomColors[16] = {
        RGB(0x12, 0x12, 0x12), RGB(0x31, 0x31, 0x31), RGB(0x78, 0x78, 0x78), RGB(0xa1, 0xa1, 0xa1),
        RGB(0xa1, 0x80, 0x4c), RGB(0x51, 0x4a, 0x1c), RGB(0x65, 0x54, 0x04), RGB(0x24, 0x4a, 0x14),
        RGB(0x34, 0x54, 0x2c), RGB(0x26, 0x30, 0x0c), RGB(0x28, 0x12, 0x0a), RGB(0x34, 0x12, 0x06),
        RGB(0x94, 0x80, 0x4c), RGB(0x74, 0x60, 0x2c), RGB(0x84, 0x70, 0x4c), RGB(0x14, 0x45, 0x33)
    };

    CHOOSECOLOR cc;
    ZeroMemory(&cc, sizeof(CHOOSECOLOR));
    cc.lStructSize = sizeof(CHOOSECOLOR);
    cc.hwndOwner = parent;
    cc.Flags = CC_FULLOPEN | CC_ANYCOLOR | CC_RGBINIT;
    cc.rgbResult = clr;
    cc.lpCustColors = CustomColors;
    if (ChooseColor(&cc)) {
        return cc.rgbResult;
    }
    return clr;
}

//Required Function
BOOL WINAPI ScreenSaverConfigureDialog(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    static HWND hCheckbox, previewer;
    static COLORREF selectcolor;
    static RECT         rc;

    GetClientRect(hDlg, &rc);

    switch (message)
    {
    case WM_INITDIALOG:

        set_grey_theme();

        // Load Settings  from Registry 
        LoadSaveSettings(FALSE);
        
        hCheckbox = GetDlgItem(hDlg, IDC_SHOWSECONDS);
        SendMessage(hCheckbox, BM_SETCHECK, digit_second, 0);
        hCheckbox = GetDlgItem(hDlg, IDC_SHOWSECONDCICRCLE);
        SendMessage(hCheckbox, BM_SETCHECK, circle_second, 0);
        hCheckbox = GetDlgItem(hDlg, IDC_SHOWDATE);
        SendMessage(hCheckbox, BM_SETCHECK, show_date, 0);
        hCheckbox = GetDlgItem(hDlg, IDC_BLINKDOTS);
        SendMessage(hCheckbox, BM_SETCHECK, dot_blink, 0);
        hCheckbox = GetDlgItem(hDlg, IDC_ANTIALIAS);
        SendMessage(hCheckbox, BM_SETCHECK, antialias, 0);
        break;

    case WM_CTLCOLORBTN:

        if ((HWND)lParam == GetDlgItem(hDlg, IDC_BGCOLORBTN))
            selectcolor = bg_color;
        if ((HWND)lParam == GetDlgItem(hDlg, IDC_HOURCOLORBTN))
            selectcolor = hr_color;
        if ((HWND)lParam == GetDlgItem(hDlg, IDC_MINUTECOLORBTN))
            selectcolor = mn_color;
        if ((HWND)lParam == GetDlgItem(hDlg, IDC_SECONDCOLORBTN))
            selectcolor = sc_color;
        if ((HWND)lParam == GetDlgItem(hDlg, IDC_DOTMINCOLORBTN))
            selectcolor = d1_color;
        if ((HWND)lParam == GetDlgItem(hDlg, IDC_DOTSECCOLORBTN))
            selectcolor = d2_color;
        if ((HWND)lParam == GetDlgItem(hDlg, IDC_WEEKCOLORBTN))
            selectcolor = wd_color;
        if ((HWND)lParam == GetDlgItem(hDlg, IDC_MDAYCOLORBTN))
            selectcolor = md_color;
        if ((HWND)lParam == GetDlgItem(hDlg, IDC_CIRCLECOLORBNT))
            selectcolor = cs_color;
        if ((HWND)lParam == GetDlgItem(hDlg, IDC_CIRCLE5COLORBNT))
            selectcolor = cf_color;

        SetTextColor((HDC)wParam, selectcolor);
        return (LRESULT)CreateSolidBrush(selectcolor);

    case WM_COMMAND:

        switch (LOWORD(wParam))
        {
        case IDC_BGCOLORBTN:
            bg_color = SelectColor(hDlg, bg_color);
            LoadSaveSettings(TRUE);
            //SendMessage(hDlg, WM_INITDIALOG, 0, 0);
            SendDlgItemMessage(hDlg, 1026, WM_SETFONT, (WPARAM)NULL, (LPARAM)TRUE);
            break;
        case IDC_HOURCOLORBTN:
            hr_color = SelectColor(hDlg, hr_color);
            LoadSaveSettings(TRUE);
            SendDlgItemMessage(hDlg, 1021, WM_SETFONT, (WPARAM)NULL, (LPARAM)TRUE);
            break;
        case IDC_MINUTECOLORBTN:
            mn_color = SelectColor(hDlg, mn_color);
            LoadSaveSettings(TRUE);
            SendDlgItemMessage(hDlg, 1022, WM_SETFONT, (WPARAM)NULL, (LPARAM)TRUE);
            break;
        case IDC_SECONDCOLORBTN:
            sc_color = SelectColor(hDlg, sc_color);
            LoadSaveSettings(TRUE);
            SendDlgItemMessage(hDlg, 1023, WM_SETFONT, (WPARAM)NULL, (LPARAM)TRUE);
            break;
        case IDC_DOTMINCOLORBTN:
            d1_color = SelectColor(hDlg, d1_color);
            LoadSaveSettings(TRUE);
            SendDlgItemMessage(hDlg, 1024, WM_SETFONT, (WPARAM)NULL, (LPARAM)TRUE);
            break;
        case IDC_DOTSECCOLORBTN:
            d2_color = SelectColor(hDlg, d2_color);
            LoadSaveSettings(TRUE);
            SendDlgItemMessage(hDlg, 1025, WM_SETFONT, (WPARAM)NULL, (LPARAM)TRUE);
            break;
        case IDC_WEEKCOLORBTN:
            wd_color = SelectColor(hDlg, wd_color);
            LoadSaveSettings(TRUE);
            SendDlgItemMessage(hDlg, 1008, WM_SETFONT, (WPARAM)NULL, (LPARAM)TRUE);
            break;
        case IDC_MDAYCOLORBTN:
            md_color = SelectColor(hDlg, md_color);
            LoadSaveSettings(TRUE);
            SendDlgItemMessage(hDlg, 1010, WM_SETFONT, (WPARAM)NULL, (LPARAM)TRUE);
            break;
        case IDC_CIRCLECOLORBNT:
            cs_color = SelectColor(hDlg, cs_color);
            LoadSaveSettings(TRUE);
            SendDlgItemMessage(hDlg, 1018, WM_SETFONT, (WPARAM)NULL, (LPARAM)TRUE);
            break;
        case IDC_CIRCLE5COLORBNT:
            cf_color = SelectColor(hDlg, cf_color);
            LoadSaveSettings(TRUE);
            SendDlgItemMessage(hDlg, 1020, WM_SETFONT, (WPARAM)NULL, (LPARAM)TRUE);
            break;
        case IDC_SHOWSECONDS:
            digit_second = SendMessage(GetDlgItem(hDlg, IDC_SHOWSECONDS), BM_GETCHECK, 0, 0);
            LoadSaveSettings(TRUE);
        case IDC_SHOWSECONDCICRCLE:
            circle_second = SendMessage(GetDlgItem(hDlg, IDC_SHOWSECONDCICRCLE), BM_GETCHECK, 0, 0);
            LoadSaveSettings(TRUE);
            break;
        case IDC_SHOWDATE:
            show_date = SendMessage(GetDlgItem(hDlg, IDC_SHOWDATE), BM_GETCHECK, 0, 0);
            LoadSaveSettings(TRUE);
            break;
        case IDC_BLINKDOTS:
            dot_blink = SendMessage(GetDlgItem(hDlg, IDC_BLINKDOTS), BM_GETCHECK, 0, 0);
            LoadSaveSettings(TRUE);
            break;
        case IDC_ANTIALIAS:
            antialias = SendMessage(GetDlgItem(hDlg, IDC_ANTIALIAS), BM_GETCHECK, 0, 0);
            LoadSaveSettings(TRUE);
            break;
        case IDC_SETTHEME01BTN:
            set_color_theme();
            LoadSaveSettings(TRUE);
            //SendMessage(hDlg, WM_INITDIALOG, 0, 0);
            RedrawWindow(hDlg, &rc, NULL, RDW_INVALIDATE);
            break;
        case IDC_SETGREYTHEMEBTN:
            set_grey_theme();
            LoadSaveSettings(TRUE);
            RedrawWindow(hDlg, &rc, NULL, RDW_INVALIDATE);
            //InvalidateRect(hDlg, &rc, FALSE);
            //SendMessage(hDlg, WM_INITDIALOG, 0, 0);
            break;
        case IDC_GOGITBTN:
            ShellExecute(hDlg, "Open", "https://github.com/rty65tt/stClock.scr", (LPCTSTR)NULL, (LPCTSTR)NULL, SW_SHOW);
            break;
        case IDOK:
            EndDialog(hDlg, LOWORD(wParam));
            break;

        case IDCANCEL:
            EndDialog(hDlg, LOWORD(wParam));
            break;
        }
        return TRUE;
    }
    return(FALSE);
}

BOOL WINAPI RegisterDialogClasses(HANDLE hInst)
{
    return(TRUE);
}
