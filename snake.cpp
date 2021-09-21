/*
                          _/\)
                 _.---. .'   `\
               .'      `     ^ )
              /     \       .--'
             |      /       )'-.
             ; ,   <__..-(   '-.)
              \ \-.__)    ``--._)
           mew '.'-.__.-.
                 '-...-'
*/

// <--- For Window ---> //

#include <windows.h> // For crating the WinAPI Console
#include <winuser.h> // For customizing the Window
#include <tchar.h>   // To write generic code, used to name the Window

// <--- Random Generator ---> //

#include <chrono> // Time library, to generate a random seed
#include <random> // Functions for generating pseudorandom numbers

#include <vector> // To store the snake body

using namespace std;

#define SN_TIMER 1000
#define SN_CLASS _T("SNAKE")
#define SN_SIZE 16
#define SN_NONE 0
#define SN_BODY 1
#define SN_FOOD 2
#define SN_HEAD 3

//Functions Declarations
int create_window(HINSTANCE hinstance, LPCTSTR cap);
void display(HDC mdc, int dir, HBRUSH back, HPEN pen);
void display_text(HDC mdc);
void start_game(int &dir);
void generate_apple();

//Variables
bool game_over;

const int ROWS = 30;
const int COLS = 40;
const int WIDTH = 640;  // Multiply COLS * 16
const int HEIGHT = 480; // Multiply ROWS * 16
int SCORE = 0;
int TIMER = 1;
int DEAD = 0;
int fruit_x, fruit_y;

BYTE game_table[ROWS][COLS];

struct point
{
    int x{}, y{};
    point() = default;
    point(int _x, int _y) : x(_x), y(_y) {}
};

vector<point> snake_body;

//Creating The Window
int WINAPI _tWinMain(HINSTANCE pHinstance, HINSTANCE, LPTSTR, int)
{
    return create_window(pHinstance, "Snake Game");
}

//Setup Game
void start_game(int &dir)
{
    dir = VK_UP;
    snake_body.clear();
    for (auto &i : game_table)
    {
        for (unsigned char &j : i)
            j = SN_NONE;
    }

    for (int b = 0; b < 3; ++b)
    {
        snake_body.emplace_back(COLS / 2, ROWS / 2 + b);
        const point &p = snake_body.back();
        game_table[p.y][p.x] = SN_BODY;
    }
    generate_apple();
    SCORE = 0;
    TIMER = 1;
    DEAD = 0;
    game_over = false;
}

//Draw Function
void display(HDC mdc, int dir, HBRUSH back, HPEN pen, HBRUSH food)
{
    if (game_over)
    {
        display_text(mdc);
        return;
    }

    point prev = snake_body.front();
    switch (dir)
    {
    case VK_LEFT:
        snake_body.front().x--;
        break;
    case VK_RIGHT:
        snake_body.front().x++;
        break;
    case VK_UP:
        snake_body.front().y--;
        break;
    case VK_DOWN:
        snake_body.front().y++;
        break;
    default:
        break;
    }

    //Checking for Impact
    point q = snake_body.front();
    if ((q.x < 0) || (q.y < 0) || (q.y >= ROWS) || (q.x >= COLS) || (game_table[q.y][q.x] == SN_BODY))
    {
        game_over = true;
        return;
    }

    point end = snake_body.back();
    vector<point>::size_type s;
    for (s = 1; s < snake_body.size(); ++s)
    {
        swap(snake_body[s], prev);
    }

    TIMER = TIMER + 1;

    //Checks if the head coordinates are equal with the food coordinates
    if (game_table[q.y][q.x] == SN_FOOD)
    {
        snake_body.push_back(end);
        game_table[q.y][q.x] = SN_NONE;
        SCORE = SCORE + 5; //Updating the Score
        generate_apple();
        TIMER = 1;
        DEAD = 0;
    }

    if (TIMER == 100)
    {

        game_table[fruit_y][fruit_x] = SN_NONE;
        generate_apple();
        TIMER = 1;
        DEAD++;
    }

    if (DEAD == 10)
    {

        game_over = true;
        return;
    }

    game_table[end.y][end.x] = SN_NONE;
    game_table[snake_body.front().y][snake_body.front().x] = SN_HEAD;

    for (s = 1; s < snake_body.size(); ++s)
        game_table[snake_body[s].y][snake_body[s].x] = SN_BODY;

    //Drawing the Snake
    int x, y;
    RECT rc;
    HGDIOBJ a, b;
    a = SelectObject(mdc, back);
    b = SelectObject(mdc, pen);
    for (int i = 0; i < ROWS; ++i)
    {
        for (int j = 0; j < COLS; ++j)
        {
            switch (game_table[i][j])
            {
            case SN_HEAD: // <--- HEAD --> //
                x = j * SN_SIZE;
                y = i * SN_SIZE;
                Rectangle(mdc, x, y, x + SN_SIZE, y + SN_SIZE);
                //SetROP2(mdc, m);
                break;
            case SN_BODY: // <--- BODY --> //
                x = j * SN_SIZE;
                y = i * SN_SIZE;
                Rectangle(mdc, x, y, x + SN_SIZE, y + SN_SIZE);
                break;
            case SN_FOOD: // <--- FRUIT --> //
                x = j * SN_SIZE;
                y = i * SN_SIZE;
                SetRect(&rc, x, y, x + SN_SIZE, y + SN_SIZE);
                FillRect(mdc, &rc, food);
                break;
            }
        }
    }
    SelectObject(mdc, a);
    SelectObject(mdc, b);
}

//Generate Fruit
void generate_apple()
{
    unsigned seed;

    //Generates a random seed based on current time
    seed = chrono::steady_clock::now().time_since_epoch().count();
    //Creates a random engine
    default_random_engine generator(seed);
    //Makes sure our random number is between [0, 8]
    uniform_int_distribution<int> distribution(0, 128);

    do
    {
        fruit_x = distribution(generator) % COLS;
        fruit_y = distribution(generator) % ROWS;
        if (game_table[fruit_y][fruit_x] == SN_NONE)
        {
            game_table[fruit_y][fruit_x] = SN_FOOD;
            break;
        }
    } while (true);
}

//Display Function
void display_text(HDC mdc)
{
    SIZE sz;
    const COLORREF rgbWhite = 0x00FFFFFF;
    SetTextColor(mdc, rgbWhite);
    SetBkMode(mdc, TRANSPARENT);
    const TCHAR s1[] = _T("GAME OVER");
    const int n1 = sizeof(s1) / sizeof(s1[0]) - 1;

    GetTextExtentPoint32(mdc, s1, n1, &sz);
    TextOut(mdc, (WIDTH - sz.cx) / 2, (HEIGHT - sz.cy) / 2 - sz.cy * 2, s1, n1);
}

// Function to Display the Score Dynamically
void update_graph(HWND hwnd, HDC dc)
{
    const COLORREF rgbWhite = 0x00FFFFFF;
    RECT rc;
    GetClientRect(hwnd, &rc);
    string score_str = "Score " + to_string(SCORE);
    SetTextColor(dc, rgbWhite);
    SetBkMode(dc, TRANSPARENT);
    DrawText(dc, score_str.c_str(), -1, &rc, DT_TOP | DT_CENTER | DT_SINGLELINE);
}

//Window Handler
LRESULT CALLBACK wnd_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    static HBITMAP hbm = nullptr;
    static HBRUSH back = nullptr;
    static HPEN pen = nullptr;
    static HBRUSH food = nullptr;
    static HDC mdc = nullptr;
    static int dir = VK_UP;
    HDC hdc;

    switch (msg)
    {
    case WM_PAINT:
        RECT rToPaint;
        if (!GetUpdateRect(hwnd, &rToPaint, FALSE))
            break; //No regions to update, leave procedure
        PAINTSTRUCT ps;
        BeginPaint(hwnd, &ps);
        //Do graphical paint
        update_graph(hwnd, ps.hdc);

        EndPaint(hwnd, &ps);
        break; //End of procedure
    case WM_CREATE:
        hdc = GetDC(hwnd);
        mdc = CreateCompatibleDC(hdc);
        hbm = CreateCompatibleBitmap(hdc, WIDTH, HEIGHT);
        SelectObject(mdc, hbm);
        ReleaseDC(hwnd, hdc);
        back = CreateSolidBrush(RGB(34, 214, 43));
        pen = CreatePen(PS_SOLID, 1, RGB(34, 214, 43));
        food = CreateSolidBrush(RGB(214, 34, 34));
        start_game(dir);
        SetTimer(hwnd, SN_TIMER, 95, nullptr);
        break;
    case WM_ERASEBKGND:
        PatBlt(mdc, 0, 0, WIDTH, HEIGHT, BLACKNESS);
        display(mdc, dir, back, pen, food);
        BitBlt((HDC)wParam, 0, 0, WIDTH, HEIGHT, mdc, 0, 0, SRCCOPY);
        return 1;
    case WM_KEYDOWN:
        switch (LOWORD(wParam))
        {
        case 0x41:
            dir = VK_LEFT;
            break;
        case 0x44:
            dir = VK_RIGHT;
            break;
        case 0x57:
            dir = VK_UP;
            break;
        case 0x53:
            dir = VK_DOWN;
            break;
        case VK_RETURN:
            if (game_over)
                start_game(dir);
            break;
        }
        break;
    case WM_TIMER:
        InvalidateRect(hwnd, nullptr, TRUE);
        break;
    case WM_DESTROY:
        DeleteDC(mdc);
        DeleteObject(hbm);
        DeleteObject(back);
        DeleteObject(pen);
        DeleteObject(food);
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

//Creating the Window
int create_window(HINSTANCE hinstance, LPCTSTR cap)
{
    WNDCLASSEX cls = {0};
    cls.hbrBackground = (HBRUSH)CreateSolidBrush(RGB(5, 5, 0));
    cls.cbSize = sizeof(cls);
    cls.lpfnWndProc = (WNDPROC)wnd_proc;
    cls.hInstance = hinstance;
    cls.hCursor = LoadCursor(nullptr, IDC_ARROW);
    cls.lpszClassName = SN_CLASS;
    if (!RegisterClassEx(&cls))
        return 1;

    DWORD sty = WS_OVERLAPPEDWINDOW & ~(WS_MAXIMIZEBOX | WS_SIZEBOX);
    RECT rc = {0, 0, WIDTH, HEIGHT};
    AdjustWindowRectEx(&rc, sty, FALSE, 0);
    int width = rc.right - rc.left;
    int height = rc.bottom - rc.top;
    HWND hwnd = CreateWindowEx(0, SN_CLASS, cap, sty, (GetSystemMetrics(SM_CXSCREEN) - width) / 2,
                               (GetSystemMetrics(SM_CYSCREEN) - height) / 2, width, height, nullptr, nullptr, hinstance, nullptr);
    if (hwnd == nullptr)
    {
        UnregisterClass(SN_CLASS, hinstance);
        return 1;
    }
    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);

    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    UnregisterClass(SN_CLASS, hinstance);
    return 0;
}
