#include "pipeline.h"
#include "camera.h"

const static float StepScale = 0.3f;
const static int MARGIN = 10;

Camera::Camera(int WindowWidth,int WindowHeight, SDL_Window* Window)
{
	for (int i = 0; i < 256; i++)keys[i] = false;
	m_windowWidth = WindowWidth;
	m_windowHeight = WindowHeight;

	
	m_pos = Vector3f(0.0f, 0.0f, 0.0f);
	m_target = Vector3f(0.0f, 0.0f, 1.0f);
	m_target.Normalize();
	m_up = Vector3f(0.0f, 1.0f, 0.0f);

	m_window = Window;

	Init();
}

Camera::Camera(int WindowWidth, int WindowHeight, const Vector3f& Pos, const Vector3f& Target, const Vector3f& Up, SDL_Window *Window)
{
	for (int i = 0; i < 256; i++)keys[i] = false;
	m_windowWidth = WindowWidth;
	m_windowHeight = WindowHeight;

	m_pos = Pos;

	m_target = Target;
	m_target.Normalize();

	m_up = Up;
	m_up.Normalize();

	m_window = Window;

	Init();
}

void Camera::Init()
{
	Vector3f HTarget(m_target.x, 0.0, m_target.z);
	HTarget.Normalize();

	if (HTarget.z >= 0.0f)
	{
		if (HTarget.x >= 0.0f)
		{
			m_AngleH = 360.0f - ToDegree(asin(HTarget.z));
		}
		else
		{
			m_AngleH = 180.0f + ToDegree(asin(HTarget.z));
		}
	}
	else
	{
		if (HTarget.x >= 0.0f)
		{
			m_AngleH = ToDegree(asin(-HTarget.z));
		}
		else
		{
			m_AngleH = 90.0f + ToDegree(asin(-HTarget.z));
		}
	}

	m_AngleV = -ToDegree(asin(m_target.y));

	m_OnUpperEdge = false;
	m_OnLowerEdge = false;
	m_OnLeftEdge = false;
	m_OnRightEdge = false;
	m_mousePos.x = m_windowWidth / 2;
	m_mousePos.y = m_windowHeight / 2;
	
	SDL_WarpMouseInWindow(m_window, m_mousePos.x, m_mousePos.y);
}

bool Camera::OnKeyboard()
{
	bool Ret = false;

	if (keys[119])//Key 'W'
	{
		//Scale -= 1.1f;
		m_pos += (m_target*StepScale);
		Ret = true;
	}

	if (keys[115])//Key 'S'
	{
		//Scale -= 1.1f;;
		m_pos -= (m_target*StepScale);
		Ret = true;
	}
	
	if (keys[100])//Key 'D'
	{
		Vector3f Right = m_up.Cross(m_target);
		Right.Normalize();
		Right *= StepScale;
		m_pos += Right;
		Ret = true;
	}
	if (keys[97])//Key 'A'
	{
		Vector3f Left = m_target.Cross(m_up);
		Left.Normalize();
		Left *= StepScale;
		m_pos += Left;
		Ret = true;
	}
	return Ret;
}

void Camera::OnMouse(int x, int y,SDL_Window* gWindow)
{
	//printf("x=%i y=%i\n", x, y);
	const int DeltaX = 2*(x - m_windowWidth / 2);
	const int DeltaY = 2*(y - m_windowHeight / 2);
	
	if (DeltaX == 0 && DeltaY == 0)return;
	SDL_WarpMouseInWindow(gWindow, m_windowWidth / 2, m_windowHeight / 2);

	m_mousePos.x = x;
	m_mousePos.y = y;

	m_AngleH += (float)DeltaX / 20.0f;
	m_AngleV += (float)DeltaY / 20.0f;

	if (DeltaX == 0)
	{
		if (x <= MARGIN)
		{
			m_OnLeftEdge = true;
		}
		else if (x >= m_windowWidth - MARGIN)
		{
			m_OnRightEdge = true;
		}
	}
	else
	{
		m_OnLeftEdge = false;
		m_OnRightEdge = false;
	}

	if (DeltaY == 0)
	{
		if (y <= MARGIN)
		{
			m_OnUpperEdge = true;
		}
		else if (y >= m_windowHeight - MARGIN)
		{
			m_OnLowerEdge = true;
		}
	}
	else
	{
		m_OnUpperEdge = false;
		m_OnLowerEdge = false;
	}

	Update();
}

void Camera::OnRender()
{
	bool ShouldUpdate = false;

	/*if (m_OnLeftEdge)
	{
		m_AngleH -= 2.0f;
		ShouldUpdate = true;
	}
	else if (m_OnRightEdge)
	{
		m_AngleH += 2.0f;
		ShouldUpdate = true;
	}

	if (m_OnUpperEdge)
	{
		if (m_AngleV > -90.0f)
		{
			m_AngleV -= 2.0f;
			ShouldUpdate = true;
		}
	}
	else if (m_OnLowerEdge)
	{
		if (m_AngleV < 90.0f)
		{
			m_AngleV += 2.0f;
			ShouldUpdate = true;
		}
	}*/

	if (ShouldUpdate)
	{
		Update();
	}
}

void Camera::Update()
{
	const Vector3f Vaxis(0.0f, 1.0f, 0.0f);

	Vector3f View(1.0f, 0.0f, 0.0f);
	View.Rotate(m_AngleH, Vaxis);
	View.Normalize();

	Vector3f Haxis = Vaxis.Cross(View);
	Haxis.Normalize();
	View.Rotate(m_AngleV, Haxis);

	m_target = View;
	m_target.Normalize();

	m_up = m_target.Cross(Haxis);
	m_up.Normalize();
}