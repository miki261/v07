#include "main.h"
#include "rc.h"
#include <filesystem>
#include <commdlg.h>
#include <algorithm>

void main_window::on_paint(HDC hdc)
{
	RECT rect;
	GetClientRect(*this, &rect);
	using namespace Gdiplus;

	Graphics graphics(hdc);
	graphics.SetSmoothingMode(SmoothingModeHighQuality);
	graphics.SetInterpolationMode(InterpolationModeHighQualityBicubic);
	graphics.Clear(Color::White);

	if (m_pImage && !m_fileName.empty()) {

		int imgWidth = m_pImage->GetWidth();
		int imgHeight = m_pImage->GetHeight();

		float scaleX = (float)rect.right / imgWidth;
		float scaleY = (float)rect.bottom / imgHeight;
		float scale = std::min(scaleX, scaleY);

		int newWidth = (int)(imgWidth * scale);
		int newHeight = (int)(imgHeight * scale);

		int x = (rect.right - newWidth) / 2;
		int y = (rect.bottom - newHeight) / 2;

		graphics.DrawImage(m_pImage.get(), x, y, newWidth, newHeight);

		std::filesystem::path f_path(m_fileName);
		std::wstring f_name = f_path.filename();

		Font font(L"Arial", 12, FontStyleBold);
		SolidBrush b(Color(128, 0, 0, 0));
		SolidBrush w(Color::White);

		RectF layoutRect((float)0, (float)(rect.bottom - 30), (float)rect.right, 30);
		RectF shadowRect = layoutRect;
		shadowRect.X += 1;
		shadowRect.Y += 1;

		StringFormat format;
		format.SetAlignment(StringAlignmentCenter);
		format.SetLineAlignment(StringAlignmentFar);

		graphics.DrawString(f_name.c_str(), -1, &font, shadowRect, &format, &b);
		graphics.DrawString(f_name.c_str(), -1, &font, layoutRect, &format, &w);
	}
}

void main_window::on_command(int id)
{
	switch (id)
	{
	case ID_OPEN:
	{
		wchar_t path[MAX_PATH];
		*path = 0;
		wchar_t filter[] = L"JPEG\0*.jpeg\0PNG\0*.png\0BMP\0*.bmp\0GIF\0*.gif\0TIFF\0*.tiff\0EMF\0*.emf\0";

		OPENFILENAMEW ofn;
		ZeroMemory(&ofn, sizeof ofn);

		ofn.lStructSize = sizeof ofn;
		ofn.hwndOwner = *this;
		ofn.lpstrFile = path;
		ofn.nMaxFile = MAX_PATH;
		ofn.lpstrFilter = filter;
		ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;

		if (GetOpenFileNameW(&ofn))
		{
			std::filesystem::path f_path(path);
			m_fileName = f_path.filename();
			m_pImage = std::make_unique<Gdiplus::Image>(path);

			if (!m_pImage || m_pImage->GetLastStatus() != Gdiplus::Ok)
			{
				m_pImage.reset();
				MessageBox(*this, L"Failed to load image!", L"Error", MB_OK | MB_ICONERROR);
			}
			else
			{
				InvalidateRect(*this, nullptr, TRUE);
			}
		}
	}
	break;
	case ID_EXIT:
		DestroyWindow(*this);
		break;
	}
}

void main_window::on_destroy()
{
	::PostQuitMessage(0);
}

int WINAPI _tWinMain(HINSTANCE instance, HINSTANCE, LPTSTR, int)
{
	gdiplus_application app;
	main_window wnd;
	wnd.create(0, WS_OVERLAPPEDWINDOW | WS_VISIBLE, _T("NWP 7"), (int64_t)LoadMenu(instance, MAKEINTRESOURCE(IDM_MAIN)));
	return app.run();
}
