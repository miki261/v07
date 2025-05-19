#include "main.h"
#include "rc.h"
#include <filesystem>
#include <commdlg.h>

void main_window::on_paint(HDC hdc)
{
	Gdiplus::Graphics graphics(hdc);
	graphics.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
	graphics.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);

	RECT rect;
	GetClientRect(*this, &rect);
	graphics.Clear(Gdiplus::Color::White);

	if (m_pImage)
	{
		int imgWidth = m_pImage->GetWidth();
		int imgHeight = m_pImage->GetHeight();

		float scaleX = (float)rect.right / imgWidth;
		float scaleY = (float)rect.bottom / imgHeight;
		float scale = min(scaleX, scaleY);

		int newWidth = (int)(imgWidth * scale);
		int newHeight = (int)(imgHeight * scale);

		int x = (rect.right - newWidth) / 2;
		int y = (rect.bottom - newHeight) / 2;

		graphics.DrawImage(m_pImage, x, y, newWidth, newHeight);

		if (!m_fileName.empty())
		{
			Gdiplus::FontFamily fontFamily(L"Arial");
			Gdiplus::Font font(&fontFamily, 12, Gdiplus::FontStyleBold, Gdiplus::UnitPoint);

			Gdiplus::StringFormat stringFormat;
			stringFormat.SetAlignment(Gdiplus::StringAlignmentCenter);

			Gdiplus::RectF layoutRect((float)0, (float)(rect.bottom - 30), (float)rect.right, 30);

			Gdiplus::SolidBrush shadowBrush(Gdiplus::Color(128, 0, 0, 0));
			Gdiplus::PointF shadowPoint((float)1, (float)1);
			Gdiplus::RectF shadowRect = layoutRect;
			shadowRect.X += shadowPoint.X;
			shadowRect.Y += shadowPoint.Y;
			graphics.DrawString(m_fileName.c_str(), -1, &font, shadowRect, &stringFormat, &shadowBrush);

			Gdiplus::SolidBrush textBrush(Gdiplus::Color(255, 255, 255, 255));
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
		TCHAR szFileName[MAX_PATH] = _T("");
		OPENFILENAME ofn;
		ZeroMemory(&ofn, sizeof(ofn));
		ofn.lStructSize = sizeof(ofn);
		ofn.hwndOwner = *this;
		ofn.lpstrFilter = _T("Image Files (*.bmp;*.jpg;*.jpeg;*.png;*.gif;*.tif;*.tiff;*.emf)\0*.bmp;*.jpg;*.jpeg;*.png;*.gif;*.tif;*.tiff;*.emf\0All Files (*.*)\0*.*\0");
		ofn.lpstrFile = szFileName;
		ofn.nMaxFile = MAX_PATH;
		ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
		ofn.lpstrDefExt = _T("jpg");

		if (GetOpenFileName(&ofn))
		{
			if (m_pImage)
			{
				delete m_pImage;
				m_pImage = nullptr;
			}

			m_pImage = Gdiplus::Image::FromFile(szFileName);

			if (!m_pImage || m_pImage->GetLastStatus() != Gdiplus::Ok)
			{
				if (m_pImage)
				{
					delete m_pImage;
					m_pImage = nullptr;
				}
				MessageBox(*this, _T("Failed to load image!"), _T("Error"), MB_OK | MB_ICONERROR);
			}
			else
			{
				std::filesystem::path path(szFileName);
#ifdef _UNICODE
				m_fileName = path.filename().wstring();
#else
				std::string str = path.filename().string();
				int size_needed = MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, NULL, 0);
				m_fileName.resize(size_needed);
				MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, &m_fileName[0], size_needed);
#endif

				InvalidateRect(*this, NULL, TRUE);
			}
		}
		break;
	}
	case ID_EXIT:
		DestroyWindow(*this);
		break;
	}
}

void main_window::on_destroy()
{
	if (m_pImage)
	{
		delete m_pImage;
		m_pImage = nullptr;
	}

	::PostQuitMessage(0);
}

int WINAPI _tWinMain(HINSTANCE instance, HINSTANCE, LPTSTR, int)
{
	gdiplus_application app;
	main_window wnd;
	wnd.create(0, WS_OVERLAPPEDWINDOW | WS_VISIBLE, _T("NWP 7"), (int64_t)LoadMenu(instance, MAKEINTRESOURCE(IDM_MAIN)));
	return app.run();
}
