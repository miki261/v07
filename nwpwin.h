#pragma once

#include <windows.h>
#include <windowsx.h>
#include <string>
#include <sstream>
#include <tchar.h>

using tstring = std::basic_string<TCHAR>;
using tstringstream = std::basic_stringstream<TCHAR>;

namespace vsite::nwp {

	class application
	{
	public:
		int run();
	};

	class window
	{
		HWND hw{ 0 };

	protected:
		virtual tstring class_name();
		bool register_class(const tstring& name);
		tstring generate_class_name();

	public:
		bool create(HWND parent, DWORD style, LPCTSTR caption = 0, int64_t IdOrMenu = 0,
			int x = CW_USEDEFAULT, int y = CW_USEDEFAULT, int width = CW_USEDEFAULT, int height = CW_USEDEFAULT);

		operator HWND();
		static LRESULT CALLBACK proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

		//	messages
	protected:
		virtual int on_create(CREATESTRUCT*) { return 0; }
		virtual void on_command(int) {}
		virtual void on_destroy() {}

		virtual void on_key_down(int key) {}
		virtual void on_key_up(int key) {}
		virtual void on_char(TCHAR c) {}
		virtual void on_left_button_down(POINT p) {}
		virtual void on_right_button_down(POINT p) {}
		virtual void on_left_button_up(POINT p) {}
		virtual void on_left_button_double_click(POINT p) {}

		virtual void on_timer(int id) {}

		virtual void on_paint(HDC dc) {}
		virtual bool on_erase_bkgnd(HDC dc) { return false; }
	};

} // namespace
