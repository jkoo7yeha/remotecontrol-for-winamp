#pragma once

extern int  init();
extern void config();
extern void quit();

extern LRESULT CALLBACK MainWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);