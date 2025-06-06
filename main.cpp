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

	if (m_pImage)
	{
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

		if (!m_fileName.empty())
		{
			FontFamily fontFamily(L"Arial");
			Font font(&fontFamily, 12, FontStyleBold, UnitPoint);

			StringFormat stringFormat;
			stringFormat.SetAlignment(StringAlignmentCenter);
			stringFormat.SetLineAlignment(StringAlignmentFar);

			RectF layoutRect((float)0, (float)(rect.bottom - 30), (float)rect.right, 30);

			SolidBrush shadowBrush(Color(128, 0, 0, 0));
			RectF shadowRect = layoutRect;
			shadowRect.X += 1;
			shadowRect.Y += 1;
			graphics.DrawString(m_fileName.c_str(), -1, &font, shadowRect, &stringFormat, &shadowBrush);

			SolidBrush textBrush(Color(255, 255, 255, 255));
			graphics.DrawString(m_fileName.c_str(), -1, &font, layoutRect, &stringFormat, &textBrush);
		}
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
		wchar_t filter[] = L"Sve slike\0*.jpeg;*.jpg;*.png;*.bmp;*.gif;*.tiff;*.tif;*.emf\0"
			L"JPEG slike\0*.jpeg;*.jpg\0"
			L"PNG slike\0*.png\0"
			L"BMP slike\0*.bmp\0"
			L"GIF slike\0*.gif\0"
			L"TIFF slike\0*.tiff;*.tif\0"
			L"EMF slike\0*.emf\0"
			L"Sve datoteke\0*.*\0\0";

		OPENFILENAMEW ofn;
		ZeroMemory(&ofn, sizeof ofn);

		ofn.lStructSize = sizeof ofn;
		ofn.hwndOwner = *this;
		ofn.lpstrFile = path;
		ofn.nMaxFile = MAX_PATH;
		ofn.lpstrFilter = filter;
		ofn.lpstrTitle = L"Choose an image";
		ofn.nFilterIndex = 1;
		ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_PATHMUSTEXIST;

		if (GetOpenFileNameW(&ofn))
		{
			std::filesystem::path file_path(path);
			m_fileName = file_path.filename().wstring();
			m_pImage = std::make_unique<Gdiplus::Image>(path);

			if (!m_pImage || m_pImage->GetLastStatus() != Gdiplus::Ok)
			{
				m_pImage.reset();
				MessageBox(*this, L"Error!", L"Warning", MB_OK | MB_ICONWARNING);
			}
			else
			{
				InvalidateRect(*this, NULL, TRUE);
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
