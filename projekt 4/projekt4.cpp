#include <windows.h>
#include <gdiplus.h>
#include <wingdi.h>
#include <list>
#include <ctime>

using namespace Gdiplus;
using namespace std;

#define MAX_LOADSTRING 100

#define IDD_ABOUTBOX			103
#define IDM_ABOUT				104
#define IDM_EXIT				105

#define ID_BUTTON1				200
#define ID_BUTTON2				201
#define ID_BUTTON3				202
#define ID_BUTTON4				203
#define ID_BUTTON5				204
#define ID_BUTTON6				205

struct block
{
	int corners = 4; // 0 - circle, 4 - square, 3 - triangle
	int length = 60;
	int radius = 30;
	int x = 412; // the coordinates of the lower left corner
	int y = 700; // for a circle, the point of intersection of the upper horizontal and left vertical tangents
	int weight = 15 * corners;
	bool isPicked = false;
	bool isPlaced = false;
};

struct rope
{
	int y = 300; //coordinate of the lower point of the rope, the upper one is constant and equals 200
	int x = 442;
	int position = 0;
	int sliderX1 = x - 30 + position * 60;
	int sliderX2 = x + 30 + position * 60;
	int sliderY1 = 191;
	int sliderY2 = 200;
};

int previousAction = 0;
int action = 0;

rope r;
list<block> blocks;
block places[6][3];
block current;
int p = 100;

LPCWSTR ClassName = L"Class Name";
MSG statement;

// Global Variables:
HWND hwndButton;
HINSTANCE hInstance;							// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

RECT drawArea{ 410, 700, 950, 191 };

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	Buttons(HWND, UINT, WPARAM, LPARAM);

void createCrane(HDC& hdc, HWND hwnd, PAINTSTRUCT& ps)
{
	hdc = BeginPaint(hwnd, &ps);
	Graphics graphics(hdc);
	Pen pen(Color(255, 0, 0, 0), 3); //black
	Pen pen2(Color(255, 255, 188, 0), 2); //yellow
	Pen pen3(Color(255, 92, 92, 92), 30); //dark gray
	Pen pen4(Color(255, 150, 150, 150), 380); //light gray

	graphics.DrawLine(&pen4, 0, 890, 1920, 890);
	graphics.DrawLine(&pen2, 320, 700, 320, 100);
	graphics.DrawLine(&pen2, 350, 700, 350, 100);
	graphics.DrawLine(&pen3, 260, 685, 410, 685);
	graphics.DrawLine(&pen2, 170, 190, 950, 190);
	graphics.DrawLine(&pen2, 170, 160, 920, 160);

	for (int i = 0; i < 25; i++)
	{
		graphics.DrawLine(&pen2, 200 + 30 * i, 160, 200 + 30 * i, 190);
		graphics.DrawLine(&pen2, 200 + 30 * i, 160, 200 + 30 + 30 * i, 190);
	}

	for (int i = 670; i > 100; i -= 30)
	{
		graphics.DrawLine(&pen2, 320, i - 30, 350, i);
		graphics.DrawLine(&pen2, 320, i, 350, i - 30);
	}

	graphics.DrawLine(&pen3, 170, 145, 230, 145);
	graphics.DrawLine(&pen3, 170, 175, 230, 175);
	graphics.DrawLine(&pen3, 170, 205, 230, 205);
	graphics.DrawLine(&pen, 170, 160, 230, 160);
	graphics.DrawLine(&pen, 170, 190, 230, 190);
	graphics.DrawLine(&pen, 200, 130, 200, 220);
	graphics.DrawLine(&pen, 260, 160, 320, 100);
	graphics.DrawLine(&pen, 320, 100, 350, 100);
	graphics.DrawLine(&pen, 350, 100, 650, 160);

	graphics.DrawLine(&pen, 260, 670, 410, 670);
	graphics.DrawLine(&pen, 290, 700, 290, 640);
	graphics.DrawLine(&pen, 290, 640, 380, 640);
	graphics.DrawLine(&pen, 380, 640, 380, 700);

	SolidBrush brush = SolidBrush(Color(255, 0, 0, 0));
	PointF p1(412, 191);
	PointF p2(472, 191);
	PointF p3(472, 200);
	PointF p4(412, 200);
	PointF points[4] = { p1, p2, p3, p4 };
	PointF* pPoints = points;

	graphics.FillPolygon(&brush, pPoints, 4);
	graphics.DrawLine(&pen, 442, 200, 442, 300);

	EndPaint(hwnd, &ps);
}

void randomNumbers(int numbers[150]) //randomizes 150 different numbers to later obtain randomly arranged block shapes in a list
{
	srand(time(NULL));
	for (int i = 0; i < 150; i++) {
		numbers[i] = i;
	}

	int j = 0;
	int temp = 0;

	for (int i = 0; i < 150; i++) {
		j = (rand() % 149);
		temp = numbers[i];
		numbers[i] = numbers[j];
		numbers[j] = temp;
	}
}

void createBlocks() //arranges blocks in the shape of a circle, triangle and square in a list
{
	int numbers[150];
	randomNumbers(numbers);

	for (int i = 0; i < 150; i++)
	{
		block temporary;
		temporary.corners = numbers[i] % 5;

		if (numbers[i] % 5 != 1 && numbers[i] % 5 != 2)
		{
			blocks.push_back(temporary);
		}
	}
}

void drawSquare(HDC hdc)
{
	Graphics graphics(hdc);
	SolidBrush brush = SolidBrush(Color(255, 209, 75, 35));
	Pen pen(Color(255, 0, 0, 0), 3);

	PointF p1(blocks.front().x, blocks.front().y);
	PointF p2(blocks.front().x + blocks.front().length, blocks.front().y);
	PointF p3(blocks.front().x + blocks.front().length, blocks.front().y - blocks.front().length);
	PointF p4(blocks.front().x, blocks.front().y - blocks.front().length);
	PointF points[4] = { p1, p2, p3, p4 };
	PointF* pPoints = points;

	graphics.FillPolygon(&brush, pPoints, 4);

	SolidBrush xbrush = SolidBrush(Color(255, 0, 0, 0));
	PointF x1(412, 190);
	PointF x2(472, 190);
	PointF x3(472, 200);
	PointF x4(412, 200);
	PointF xpoints[4] = { x1, x2, x3, x4 };
	PointF* xPoints = xpoints;

	graphics.FillPolygon(&xbrush, xPoints, 4);
	graphics.DrawLine(&pen, 442, 200, 442, 300);
}

void drawCircle(HDC hdc)
{
	Graphics graphics(hdc);
	SolidBrush brush = SolidBrush(Color(255, 209, 75, 35));
	Pen pen(Color(255, 0, 0, 0), 3);

	graphics.FillEllipse(&brush, blocks.front().x, blocks.front().y - blocks.front().length, blocks.front().length, blocks.front().length);

	SolidBrush xbrush = SolidBrush(Color(255, 0, 0, 0));
	PointF x1(412, 190);
	PointF x2(472, 190);
	PointF x3(472, 200);
	PointF x4(412, 200);
	PointF xpoints[4] = { x1, x2, x3, x4 };
	PointF* xPoints = xpoints;

	graphics.FillPolygon(&xbrush, xPoints, 4);
	graphics.DrawLine(&pen, 442, 200, 442, 300);
}

void drawTriangle(HDC hdc)
{
	Graphics graphics(hdc);
	SolidBrush brush = SolidBrush(Color(255, 209, 75, 35));
	Pen pen(Color(255, 0, 0, 0), 3);

	PointF p1(blocks.front().x, blocks.front().y);
	PointF p2(blocks.front().x + blocks.front().radius, blocks.front().y - blocks.front().length);
	PointF p3(blocks.front().x + blocks.front().length, blocks.front().y);
	PointF points[3] = { p1, p2, p3 };
	PointF* pPoints = points;
	graphics.FillPolygon(&brush, pPoints, 3);

	SolidBrush xbrush = SolidBrush(Color(255, 0, 0, 0));
	PointF x1(412, 190);
	PointF x2(472, 190);
	PointF x3(472, 200);
	PointF x4(412, 200);
	PointF xpoints[4] = { x1, x2, x3, x4 };
	PointF* xPoints = xpoints;

	graphics.FillPolygon(&xbrush, xPoints, 4);
	graphics.DrawLine(&pen, 442, 200, 442, 300);
}

void swapBlocks() //pushing blocks one place closer to the front of the list
{
	block temporary = blocks.front();
	blocks.pop_front();
	blocks.push_back(temporary);
}

void show(HDC hdc) //draws the block at the front of the list
{
	switch (blocks.front().corners)
	{
	case 0:
		drawCircle(hdc);
		break;
	case 3:
		drawTriangle(hdc);
		break;
	case 4:
		drawSquare(hdc);
		break;
	}
}

int lastFreePlace() //returns the first place for current position of the rope where a block can be dropped
{
	for (int i = 0; i < 3; i++)
	{
		if (places[r.position - 1][i].isPlaced == false)
		{
			return i;
		}
	}
	return 3;
}

int pressureForce() //calculates the pressure force of tower blocks in a given area
{
	const int g = 10;
	int force;
	switch (lastFreePlace())
	{
	case 3:
		force = g * places[r.position - 1][lastFreePlace() - 1].weight * (lastFreePlace()); //we do not have space in places[r.position - 1][3]
		break;
	default:
		force = g * places[r.position - 1][lastFreePlace()].weight * (lastFreePlace());
		break;
	}

	return force;
}

void drawSlider(HDC hdc) //ensures that the slider does not disappear when the window is repainted
{
	Graphics graphics(hdc);
	Pen pen(Color(255, 0, 0, 0), 3);
	SolidBrush xbrush = SolidBrush(Color(255, 0, 0, 0));
	SolidBrush brush = SolidBrush(Color(255, 209, 75, 35));
	PointF x1(r.sliderX1, r.sliderY1);
	PointF x2(r.sliderX2, r.sliderY1);
	PointF x3(r.sliderX2, r.sliderY2);
	PointF x4(r.sliderX1, r.sliderY2);
	PointF xpoints[4] = { x1, x2, x3, x4 };
	PointF* xPoints = xpoints;

	if (current.isPicked == true)
	{
		PointF p1(current.x, current.y);
		PointF p2(current.x + current.length, current.y);
		PointF p3(current.x + current.length, current.y - current.length);
		PointF p4(current.x, current.y - current.length);
		PointF points[4] = { p1, p2, p3, p4 };
		PointF* pPoints = points;
		graphics.FillPolygon(&brush, pPoints, 4);
	}

	graphics.FillPolygon(&xbrush, xPoints, 4);
	graphics.DrawLine(&pen, r.x, 200, r.x, r.y);
}

bool possible() //checks the possibility of current action
{
	switch (action)
	{
	case 1:
		if (r.position == 0 || previousAction == 5)
			return false;
		break;
	case 2:
		if (r.position == 6 || previousAction == 5)
			return false;
		break;
	case 3:
		if (current.corners != 4 || current.isPicked == true || r.position != 0)
			return false;
		break;
	case 4:
		if (r.position == 0 || pressureForce() == 1800 || current.isPicked == false)
			return false;
		break;
	case 5:
		if (current.isPicked == true || r.position != 0)
			return false;
		break;
	}
	return true;
}

void drop(HDC hdc) //animates the dropping of the block
{
	Graphics graphics(hdc);
	Pen pen(Color(255, 0, 0, 0), 3);
	Pen pen3(Color(255, 255, 255, 255), 1);
	SolidBrush xbrush = SolidBrush(Color(255, 0, 0, 0));
	SolidBrush brush = SolidBrush(Color(255, 209, 75, 35));
	SolidBrush abrush = SolidBrush(Color(255, 255, 255, 255));
	PointF x1(r.sliderX1, r.sliderY1);
	PointF x2(r.sliderX2, r.sliderY1);
	PointF x3(r.sliderX2, r.sliderY2);
	PointF x4(r.sliderX1, r.sliderY2);
	PointF xpoints[4] = { x1, x2, x3, x4 };
	PointF* xPoints = xpoints;

	graphics.FillPolygon(&xbrush, xPoints, 4);

	int i = 340;
	if (lastFreePlace() == 1)
	{
		i = 280;
	}
	if (lastFreePlace() == 2)
	{
		i = 220;
	}

	for (i; i > 0; i--)
	{
		r.y++;
		current.y++;
		PointF p1(current.x, current.y);
		PointF p2(current.x + current.length, current.y);
		PointF p3(current.x + current.length, current.y - current.length);
		PointF p4(current.x, current.y - current.length);
		PointF points[4] = { p1, p2, p3, p4 };
		PointF* pPoints = points;

		PointF a1(current.x, current.y - 60);
		PointF a2(current.x + current.length, current.y - 60);
		PointF a3(current.x + current.length, current.y - current.length - 60);
		PointF a4(current.x, current.y - current.length - 60);
		PointF apoints[4] = { a1, a2, a3, a4 };
		PointF* aPoints = apoints;

		graphics.FillPolygon(&abrush, aPoints, 4);
		graphics.FillPolygon(&brush, pPoints, 4);
		graphics.DrawLine(&pen, r.x, 200, r.x, r.y);

		graphics.DrawLine(&pen3, 0, 0, 5, 5);
		Sleep(2);
	}
}

void rollRopeUp(HDC hdc) //rolling the rope after the block is dropped
{
	Graphics graphics(hdc);
	Pen pen(Color(255, 0, 0, 0), 3);
	Pen pen3(Color(255, 255, 255, 255), 1);
	SolidBrush xbrush = SolidBrush(Color(255, 0, 0, 0));
	SolidBrush brush = SolidBrush(Color(255, 209, 75, 35));
	PointF x1(r.sliderX1, r.sliderY1);
	PointF x2(r.sliderX2, r.sliderY1);
	PointF x3(r.sliderX2, r.sliderY2);
	PointF x4(r.sliderX1, r.sliderY2);
	PointF xpoints[4] = { x1, x2, x3, x4 };
	PointF* xPoints = xpoints;

	graphics.FillPolygon(&xbrush, xPoints, 4);
	int i = 340;
	if (lastFreePlace() == 2)
	{
		i = 280;
	}
	if (lastFreePlace() == 3)
	{
		i = 220;
	}
	for (i; i > 0; i--)
	{
		Sleep(2);
		r.y--;
		graphics.DrawLine(&pen, r.x, 200, r.x, r.y);
		graphics.DrawLine(&pen3, r.x - 3, r.y, r.x + 3, r.y);
		graphics.DrawLine(&pen3, 0, 0, 5, 5);
	}
}

void move(HDC hdc) //moves the rope, slider and possibly a hooked block along the crane arm
{
	Graphics graphics(hdc);
	SolidBrush brush = SolidBrush(Color(255, 0, 0, 0));
	Pen pen(Color(255, 0, 0, 0), 3);
	Pen pen2(Color(255, 255, 255, 255), 1);
	SolidBrush xbrush = SolidBrush(Color(255, 209, 75, 35));
	SolidBrush abrush = SolidBrush(Color(255, 255, 255, 255));

	for (int i = 0; i < 60; i++)
	{
		if (action == 1)
		{
			r.sliderX1--;
			r.sliderX2--;
			r.x--;
			if (current.isPicked == true)
				current.x--;
		}
		else
		{
			r.sliderX1++;
			r.sliderX2++;
			r.x++;
			if (current.isPicked == true)
				current.x++;
		}
		PointF p1(r.sliderX1, r.sliderY1);
		PointF p2(r.sliderX2, r.sliderY1);
		PointF p3(r.sliderX2, r.sliderY2);
		PointF p4(r.sliderX1, r.sliderY2);
		PointF points[4] = { p1, p2, p3, p4 };
		PointF* pPoints = points;

		PointF x1(current.x, current.y);
		PointF x2(current.x + current.length, current.y);
		PointF x3(current.x + current.length, current.y - current.length);
		PointF x4(current.x, current.y - current.length);
		PointF xpoints[4] = { x1, x2, x3, x4 };
		PointF* xPoints = xpoints;

		graphics.FillPolygon(&brush, pPoints, 4);
		graphics.DrawLine(&pen, r.x, 200, r.x, r.y);
		if (action == 1)
		{
			graphics.DrawLine(&pen2, r.x + 2, 200, r.x + 2, r.y);
			graphics.DrawLine(&pen2, r.sliderX2 + 1, r.sliderY1, r.sliderX2 + 1, r.sliderY2);
			if (current.isPicked == true)
			{
				graphics.FillPolygon(&xbrush, xPoints, 4);
				PointF a1(current.x + 60, current.y);
				PointF a2(current.x + 60 + current.length, current.y);
				PointF a3(current.x + 60 + current.length, current.y - current.length);
				PointF a4(current.x + 60, current.y - current.length);
				PointF apoints[4] = { a1, a2, a3, a4 };
				PointF* aPoints = apoints;
				graphics.FillPolygon(&abrush, aPoints, 4);
			}
		}
		else
		{
			graphics.DrawLine(&pen2, r.sliderX1 - 1, r.sliderY1, r.sliderX1 - 1, r.sliderY2);
			graphics.DrawLine(&pen2, r.x - 2, 200, r.x - 2, r.y);
			if (current.isPicked == true)
			{
				graphics.FillPolygon(&xbrush, xPoints, 4);
				PointF a1(current.x - 60, current.y);
				PointF a2(current.x - 60 + current.length, current.y);
				PointF a3(current.x - 60 + current.length, current.y - current.length);
				PointF a4(current.x - 60, current.y - current.length);
				PointF apoints[4] = { a1, a2, a3, a4 };
				PointF* aPoints = apoints;
				graphics.FillPolygon(&abrush, aPoints, 4);
			}
		}
		graphics.DrawLine(&pen2, 0, 10, 5, 300);
		Sleep(2);
	}
}

void rollRopeDown(HDC hdc) //rolling the rope before the block is picked
{
	Graphics graphics(hdc);
	Pen pen(Color(255, 0, 0, 0), 3);
	Pen pen3(Color(255, 255, 255, 255), 1);
	SolidBrush xbrush = SolidBrush(Color(255, 0, 0, 0));
	SolidBrush brush = SolidBrush(Color(255, 209, 75, 35));
	PointF x1(r.sliderX1, r.sliderY1);
	PointF x2(r.sliderX2, r.sliderY1);
	PointF x3(r.sliderX2, r.sliderY2);
	PointF x4(r.sliderX1, r.sliderY2);
	PointF xpoints[4] = { x1, x2, x3, x4 };
	PointF* xPoints = xpoints;

	PointF p1(current.x, current.y);
	PointF p2(current.x + current.length, current.y);
	PointF p3(current.x + current.length, current.y - current.length);
	PointF p4(current.x, current.y - current.length);
	PointF points[4] = { p1, p2, p3, p4 };
	PointF* pPoints = points;

	graphics.FillPolygon(&xbrush, xPoints, 4);
	graphics.FillPolygon(&brush, pPoints, 4);
	for (int i = 0; i < 340; i++)
	{
		Sleep(2);
		r.y++;
		graphics.DrawLine(&pen, r.x, 200, r.x, r.y);
		graphics.DrawLine(&pen3, 0, 0, 5, 5);
	}
}

void pickBlock(HDC hdc) ////animates the process of picking the block
{
	Graphics graphics(hdc);
	Pen pen(Color(255, 0, 0, 0), 3);
	Pen pen3(Color(255, 255, 255, 255), 1);
	SolidBrush xbrush = SolidBrush(Color(255, 0, 0, 0));
	SolidBrush brush = SolidBrush(Color(255, 209, 75, 35));
	PointF x1(r.sliderX1, r.sliderY1);
	PointF x2(r.sliderX2, r.sliderY1);
	PointF x3(r.sliderX2, r.sliderY2);
	PointF x4(r.sliderX1, r.sliderY2);
	PointF xpoints[4] = { x1, x2, x3, x4 };
	PointF* xPoints = xpoints;

	graphics.FillPolygon(&xbrush, xPoints, 4);

	for (int i = 340; i > 0; i--)
	{
		r.y--;
		current.y--;
		PointF p1(current.x, current.y);
		PointF p2(current.x + current.length, current.y);
		PointF p3(current.x + current.length, current.y - current.length);
		PointF p4(current.x, current.y - current.length);
		PointF points[4] = { p1, p2, p3, p4 };
		PointF* pPoints = points;
		graphics.DrawLine(&pen, r.x, 200, r.x, r.y);
		graphics.FillPolygon(&brush, pPoints, 4);
		graphics.DrawLine(&pen3, current.x, current.y, current.x + current.length, current.y);
		graphics.DrawLine(&pen3, 0, 0, 5, 5);
		Sleep(2);
	}
}

void drawPlacedBlocks(HDC hdc) //ensures that placed blocks will not disappear when the window is repainted
{
	Graphics graphics(hdc);
	SolidBrush brush = SolidBrush(Color(255, 209, 75, 35));
	for (int i = 0; i < 6; i++)
	{
		for (int j = 2; j >= 0; j--)
		{
			if (places[i][j].isPlaced == true)
			{
				PointF p1(places[i][j].x, places[i][j].y);
				PointF p2(places[i][j].x + places[i][j].length, places[i][j].y);
				PointF p3(places[i][j].x + places[i][j].length, places[i][j].y - places[i][j].length);
				PointF p4(places[i][j].x, places[i][j].y - places[i][j].length);
				PointF points[4] = { p1, p2, p3, p4 };
				PointF* pPoints = points;

				graphics.FillPolygon(&brush, pPoints, 4);
			}
		}
	}

}

void clerBlocks() //removes all placed blocks
{
	for (int i = 0; i < 6; i++)
	{
		for (int j = 2; j >= 0; j--)
		{
			places[i][j].isPlaced = false;
		}
	}
}

void repaintWindow(HWND hWnd, HDC& hdc, PAINTSTRUCT& ps, RECT* drawArea)
{
	if (drawArea == NULL)
		InvalidateRect(hWnd, NULL, TRUE); // repaint all
	else
		InvalidateRect(hWnd, drawArea, TRUE); //repaint drawArea
	hdc = BeginPaint(hWnd, &ps);
	drawPlacedBlocks(hdc);
	switch (action)
	{
	case 1:
		move(hdc);
		break;
	case 2:
		move(hdc);
		break;
	case 3:
		rollRopeDown(hdc);
		break;
	case 4:
		drop(hdc);
		break;
	case 5:
		show(hdc);
		break;
	case 6:
		rollRopeUp(hdc);
		break;
	case 7:
		pickBlock(hdc);
		break;
	case 8:
		drawSlider(hdc);
		break;
	}
	EndPaint(hWnd, &ps);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{

	// GDI initialization 
	GdiplusStartupInput gdiplusstartupinput;
	ULONG_PTR gdiplustoken;
	GdiplusStartup(&gdiplustoken, &gdiplusstartupinput, nullptr);

	// filling structure
	WNDCLASSEX wc;

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = 0;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = ClassName;
	wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	// registration of the window class
	if (!RegisterClassEx(&wc))
	{
		MessageBox(NULL, nullptr, nullptr,
			MB_ICONEXCLAMATION | MB_OK);
		return 1;
	}

	createBlocks();

	// creating window
	HWND hwnd;

	hwnd = CreateWindowEx(WS_EX_CLIENTEDGE, ClassName, nullptr, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, 1920, 1080, NULL, NULL, hInstance, NULL);

	hwndButton = CreateWindow(TEXT("button"), TEXT("<"),
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		1200, 310, 40, 40, hwnd, (HMENU)ID_BUTTON1, GetModuleHandle(NULL), NULL);

	hwndButton = CreateWindow(TEXT("button"), TEXT(">"),
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		1240, 310, 40, 40, hwnd, (HMENU)ID_BUTTON2, GetModuleHandle(NULL), NULL);

	hwndButton = CreateWindow(TEXT("button"), TEXT("pick"),
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		1280, 350, 80, 40, hwnd, (HMENU)ID_BUTTON3, GetModuleHandle(NULL), NULL);

	hwndButton = CreateWindow(TEXT("button"), TEXT("drop"),
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		1280, 310, 80, 40, hwnd, (HMENU)ID_BUTTON4, GetModuleHandle(NULL), NULL);

	hwndButton = CreateWindow(TEXT("button"), TEXT("show"),
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		1200, 350, 80, 40, hwnd, (HMENU)ID_BUTTON5, GetModuleHandle(NULL), NULL);

	hwndButton = CreateWindow(TEXT("button"), TEXT("clear the ground"),
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		1200, 390, 160, 40, hwnd, (HMENU)ID_BUTTON6, GetModuleHandle(NULL), NULL);

	if (hwnd == NULL)
	{
		MessageBox(NULL, nullptr, nullptr, MB_ICONEXCLAMATION);
		return 1;
	}

	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);

	// messages loop
	while (GetMessage(&statement, NULL, 0, 0))
	{
		TranslateMessage(&statement);
		DispatchMessage(&statement);
	}
	GdiplusShutdown(gdiplustoken);

	return statement.wParam;
}

// action handling
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (msg)
	{
	case WM_COMMAND:
		wmId = LOWORD(wParam);
		wmEvent = HIWORD(wParam);

		// MENU & BUTTON messages
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInstance, MAKEINTRESOURCE(IDD_ABOUTBOX), hwnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hwnd);
			break;
		case ID_BUTTON1:
			action = 1;
			if (possible() == true)
			{
				SetTimer(hwnd, 4, 2, NULL);
				repaintWindow(hwnd, hdc, ps, &drawArea);
				KillTimer(hwnd, 4);
				r.position = r.position - 1;
			}
			else
				break;
			previousAction = 1;
			break;
		case ID_BUTTON2:
			action = 2;
			if (possible() == true)
			{
				SetTimer(hwnd, 4, 2, NULL);
				repaintWindow(hwnd, hdc, ps, &drawArea);
				KillTimer(hwnd, 4);
				r.position = r.position + 1;
			}
			else
				break;
			previousAction = 2;
			break;
		case ID_BUTTON3:
			action = 3;
			current.corners = blocks.front().corners;
			if (possible() == true)
			{
				current = blocks.front();
				swapBlocks();
				current.isPicked = true;
				SetTimer(hwnd, 4, 2, NULL);
				repaintWindow(hwnd, hdc, ps, &drawArea);
				action = 7;
				repaintWindow(hwnd, hdc, ps, &drawArea);
				KillTimer(hwnd, 4);

			}
			else
			{
				current.corners = 4;
				break;
			}
			previousAction = 3;
			break;
		case ID_BUTTON4:
			action = 4;
			if (possible() == true)
			{
				SetTimer(hwnd, 4, 2, NULL);
				repaintWindow(hwnd, hdc, ps, &drawArea);
				current.isPicked = false;
				current.isPlaced = true;
				places[r.position - 1][lastFreePlace()] = current;
				action = 6;
				repaintWindow(hwnd, hdc, ps, &drawArea);
				KillTimer(hwnd, 4);
			}
			else
				break;
			previousAction = 4;
			break;
		case ID_BUTTON5:
			action = 5;
			if (possible() == true)
			{
				swapBlocks();
				repaintWindow(hwnd, hdc, ps, &drawArea);
			}
			else
				break;
			previousAction = 5;
			break;
		case ID_BUTTON6:
			action = 8;
			clerBlocks();
			repaintWindow(hwnd, hdc, ps, &drawArea);
			break;
		default:
			return DefWindowProc(hwnd, msg, wParam, lParam);
		}
		break;
	case WM_PAINT:
		//drawing code not depending on timer, buttons
		createCrane(hdc, hwnd, ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_TIMER:
		switch (wParam)
		{
			break;
		}

	default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	return 0;

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