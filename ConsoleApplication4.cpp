#include <iostream>
#include <Windows.h>
#include <chrono>
#include <thread>
#include <vector>
#include <time.h>

using namespace std::this_thread;
using namespace std::chrono;
using namespace std;

COORD mh;
bool mh_dir_r;
const int WINDOW_W = 50;
const int WINDOW_H = 45;

HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);

LPCWSTR init_cl() {
    string cl;
    for(int i = 0; i < WINDOW_W; i++)
        cl += ' ';
    int len;
    int slength = (int)cl.length() + 1;
    len = MultiByteToWideChar(CP_ACP, 0, cl.c_str(), slength, 0, 0);
    wchar_t* buf = new wchar_t[len];
    MultiByteToWideChar(CP_ACP, 0, cl.c_str(), slength, buf, len);
    std::wstring r(buf);
    delete[] buf;
    return r.c_str();
}
LPCWSTR clear_line = init_cl();
void cls() {
    DWORD dw;
    for(int i = 0; i < WINDOW_H; i++) {
        COORD dcrd;
        dcrd.X = 0;
        dcrd.Y = i;
        WriteConsoleOutputCharacter(hStdOut, clear_line, WINDOW_W, dcrd, &dw);
    }
}
void draw_mh() {
    DWORD dw;

    COORD dcrd;

    dcrd.Y = WINDOW_H - mh.Y - 1;
    dcrd.X = mh.X;
    if(mh_dir_r) {
        WriteConsoleOutputCharacter(hStdOut, L"* * ", 4, dcrd, &dw); dcrd.Y--;
        WriteConsoleOutputCharacter(hStdOut, L"*** ", 4, dcrd, &dw); dcrd.Y--;
        WriteConsoleOutputCharacter(hStdOut, L"****", 4, dcrd, &dw);
    }
    else {
        WriteConsoleOutputCharacter(hStdOut, L" * *", 4, dcrd, &dw); dcrd.Y--;
        WriteConsoleOutputCharacter(hStdOut, L" ***", 4, dcrd, &dw); dcrd.Y--;
        WriteConsoleOutputCharacter(hStdOut, L"****", 4, dcrd, &dw);
    }
}

void move_mh(int px, int py) {
    if(abs(px) + abs(py) > 1)
        throw std::exception("Invalid move_mh arg");

    mh.X += px;
    mh.X -= WINDOW_W * (mh.X > WINDOW_W);
    mh.X += WINDOW_W * (mh.X < 0);

    mh.Y += py;

    //for(int i = 0; i < platforms.size(); i++) {
    //    platforms[i].Y -= py;
    //}
}

bool l = false;
void lose() {
    l = true;
}

const int JUMP_H = 10;
bool first_jump = true;
int jump_progress = 0;
bool up = true;
void jump() {
    int jh = JUMP_H + (first_jump * 5);
    int jdir = 1 - (2 * !up);
    if(jump_progress >= jh)
        up = false;
    jump_progress += jdir;
    move_mh(0, jdir);
}

vector<COORD> platforms;
const int PLATFORM_OFFSET = 6;
void ground() {
    if(up == true) 
        return;
    if(mh.Y == 0) {
        jump_progress = 0;
        up = true;
    }
    for(COORD platform : platforms) {
        COORD check = mh;
        check.X += 3 + !mh_dir_r;
        if(check.Y == platform.Y)
        if(check.X >= platform.X) {
            check = mh;
            check.X += !mh_dir_r;
            if(check.X <= platform.X + 6) {
                first_jump = false;
                jump_progress = 0;
                up = true;
            }
        }
    }
}

void add_platforms_line() {
    COORD n;
    platforms;
    int last;
    int psz = platforms.size();
    if(psz > 0)
        last = platforms[psz - 1].Y;
    else
        last = 0;
    n.Y = last + PLATFORM_OFFSET;
    for(int i = 0; i < 4; i++) {
        int r = rand() % WINDOW_W - 6 + 3;
        while(psz > 0 && abs(platforms[psz - 1].X - r) < 10) {
            r = rand() % WINDOW_W - 6 + 3;
        }
        n.X = r;
        platforms.push_back(n);
    }
}
void draw_platforms() {
    DWORD dw;
    for(COORD platform : platforms) {
        COORD dcrd = platform;
        dcrd.Y = WINDOW_H - platform.Y - 1;
        WriteConsoleOutputCharacter(hStdOut, L"@@@@@@@", 7, dcrd, &dw);
    }
}
void spawn_platforms() {
    platforms.clear();
    for(int i = 0; i < 7; i++) {
        add_platforms_line();
    }
}

void main() {
    srand(time(0));
    spawn_platforms();
    system("color a");
    for(; ;) {
        if(l) return;
        system("cls");
        draw_platforms();
        draw_mh();
        jump();
        ground();
        if(GetKeyState('A') < 0) {
            move_mh(-1, 0);
            mh_dir_r = false;
        }
        if(GetKeyState('D') < 0) {
            move_mh(1, 0);
            mh_dir_r = true;
        }
        sleep_for(69ms);
    }
}