#include <windows.h>
#include <stdio.h>
#include <gl/gl.h>
#include <math.h>

#pragma comment(lib, "opengl32.lib")

LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);
void EnableOpenGL(HWND hwnd, HDC*, HGLRC*);
void DisableOpenGL(HWND, HDC, HGLRC);

//технические переменные
float scale = 1;
float width = 1024, height = 1024;
float k = 1;
float posx = 0, posy = 0;
float px = 0, py = 0;
float pdx = 0, pdy = 0;
int bt = 0;
RECT pf;
float f(float x) {

	return 1/x;

}
//рисование осей
void DrawOs(float A, float T)
{
	
	glPushMatrix();
		static float d = 0.02;
		glRotatef(A, 0, 0, 1);
		glLineWidth(2.2);
		glBegin(GL_LINES);
		if (-1 <= T <= 1)
		{
			glVertex2f(-1, T);
			glVertex2f(1, T);
			glEnd();
		}
		else
		{
			if (T < 0)
			{
				glVertex2f(-1, 0.99999);
				glVertex2f(1, 0.99999);
				glEnd();
			}
			else
			{
			   glVertex2f(-1, -0.99999);
			   glVertex2f(1, -0.99999);
			   glEnd();

			}
		}

/*
glBegin(GL_TRIANGLES);
glColor4f(0, 0, 0, 1);
glVertex2f(0.01, 0.9);
glVertex2f(+T, 1);
glVertex2f(-0.01 + T, 0.9);
glEnd();
*/


		glPopMatrix();
		


}

//рисование график y(x)=x 
void DrawGraph(float l)
{
	
	glColor4f(1.0, 0.0, 0.0, 0.0);
	glBegin(GL_LINE_STRIP);
	for (float i = -1.01 * scale - posx; i <= 1.01 * scale - posx; i += l*scale)
	{
		glBegin(GL_LINE_STRIP);
		float x = i;
		if (abs(f(x + l * scale) - f(x))>(5 + abs(posy)) * scale) {
			glEnd();
		}
		float y = f(x);
	
		glVertex2f((x + posx) / scale, (y * k - posy) / scale);
	}

	glEnd();
}

/*void DrawCircle(float x1, float y1, float r)
{
	glColor4f(0.0, 1.0, 0.0, 0.0);
	glBegin(GL_LINE_STRIP);
	for (float i = 0-posx; i <= 6.3*r  - posx; i += 0.001)
	{
		glVertex2f((r * cos(i) + x1 + posx) / scale, ((r * sin(i) + y1)* k - posy) / scale);
	}
	glEnd();

}*/









//окно
int WINAPI WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow)
{
	WNDCLASSEX wcex;
	HWND hwnd;
	HDC hDC;
	HGLRC hRC;
	MSG msg;
	BOOL bQuit = FALSE;

	
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_OWNDC;
	wcex.lpfnWndProc = WindowProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = "GLSample";
	wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION);;


	if (!RegisterClassEx(&wcex))
		return 0;

	hwnd = CreateWindowEx(0,
		"GLSample",
		"Graph",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		width,
		height,
		NULL,
		NULL,
		hInstance,
		NULL);

	ShowWindow(hwnd, nCmdShow);


	EnableOpenGL(hwnd, &hDC, &hRC);

	while (!bQuit)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
			{
				bQuit = TRUE;
			}
			else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
		{
			
			GetClientRect(hwnd, &pf);
			glClearColor(0.97f, 0.97f, 0.95f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT);

			glLoadIdentity();

			glColor3f(0.2, 0.2, 0.2);
			DrawOs(0,-posy/scale);
			glColor3f(0.2, 0.2, 0.2);
			DrawOs(270,posx/scale);
			DrawGraph(0.00005);
			//DrawCircle(0, 0, 1);
			SwapBuffers(hDC);
			Sleep(1);
		}
	}


	DisableOpenGL(hwnd, hDC, hRC);

	DestroyWindow(hwnd);

	return msg.wParam;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CLOSE:
		PostQuitMessage(0);
		break;

//получает масштаб
	case WM_MOUSEWHEEL:
		px = 2 * (0.5 - (float)LOWORD(lParam) / (float)pf.right) * scale;
		py = 2 * (0.5 - (float)HIWORD(lParam) / (float)pf.bottom) * scale;
		if ((int)wParam > 0)
		{
			scale *= 0.9;
			if (scale < 0.025) scale = 0.025;
			else
			{
				posx += 0.25 * px;
				posy += 0.25 * py * k;
			}
		}
		else  
		{
			scale /= 0.9;
			if (scale > 75) scale = 75;
			else 
			{
				posx -= 0.0625 * px;
				posy -= 0.0625 * py * k;
			}
		}
		
		
		break;

//для корректного изменения 
	case WM_SIZE:
		glViewport(0, 0, LOWORD(lParam), HIWORD(lParam));
		glLoadIdentity();
		k = (float)LOWORD(lParam) / (float)HIWORD(lParam);
		glOrtho(-k, k, -1, 1, -1, 1);
		width = (float)LOWORD(lParam);
		height = (float)HIWORD(lParam);
		break;

//перемещение 
	case WM_MOUSEMOVE:
		if (bt == 1) {
			px = pdx-(2*((0.5 - (float)LOWORD(lParam) / (float)pf.right)) * scale);
			py = pdy-(2*((0.5 - (float)HIWORD(lParam) / (float)pf.bottom)) * scale);
			posx = px;  
			posy = py;

		}
		break;

    case WM_LBUTTONUP:
		bt = 0;
		break;

	 case WM_LBUTTONDOWN:
		bt = 1;
		pdx= posx+2 * ((0.5 - (float)LOWORD(lParam) / (float)pf.right)) * scale;
		pdy= posy+2 * ((0.5 - (float)HIWORD(lParam) / (float)pf.bottom)) * scale;
		break;
	 case WM_RBUTTONDOWN:
		 px = 2 * (0.5 - (float)LOWORD(lParam) / (float)pf.right) * scale;
		 py = 2 * (0.5 - (float)HIWORD(lParam) / (float)pf.bottom) * scale;
		 posx += px;
		 posy += py;
		 break;



	case WM_DESTROY:
		return 0;
	case WM_KEYDOWN:
	{
		switch (wParam)
		{
		case VK_ESCAPE:
			PostQuitMessage(0);
			break;
		}
	}
	break;

	default:
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}

	return 0;
}

void EnableOpenGL(HWND hwnd, HDC* hDC, HGLRC* hRC)
{
	PIXELFORMATDESCRIPTOR pfd;

	int iFormat;

	*hDC = GetDC(hwnd);

	ZeroMemory(&pfd, sizeof(pfd));

	pfd.nSize = sizeof(pfd);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW |
		PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 24;
	pfd.cDepthBits = 16;
	pfd.iLayerType = PFD_MAIN_PLANE;

	iFormat = ChoosePixelFormat(*hDC, &pfd);

	SetPixelFormat(*hDC, iFormat, &pfd);

	*hRC = wglCreateContext(*hDC);

	wglMakeCurrent(*hDC, *hRC);
	
}

void DisableOpenGL(HWND hwnd, HDC hDC, HGLRC hRC)
{
	wglMakeCurrent(NULL, NULL);
	wglDeleteContext(hRC);
	ReleaseDC(hwnd, hDC);
}
