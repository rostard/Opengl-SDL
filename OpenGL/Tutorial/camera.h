#ifndef CAMERA_H
#define CAMERA_H

#include "pipeline.h"
#include <SDL\SDL.h>

class Camera
{
public:
	Camera(int WindowWidth, int WidnowHeight,SDL_Window *Window);
	Camera(int WindowWidth, int WidnowHeight, const Vector3f& Pos, const Vector3f& Target, const Vector3f& Up, SDL_Window *Window);

	bool OnKeyboard();

	void OnMouse(int x, int y, SDL_Window* gWindow);

	void OnRender();

	void SetKeyState(long int num, bool state)
	{
		keys[num] = state;
	}

	const Vector3f& GetPos() const
	{
		return m_pos;
	}
	const Vector3f& GetTarget() const
	{
		return m_target;
	}
	const Vector3f& GetUp() const
	{
		return m_up;
	}
	
private:
	

	void Init();
	void Update();

	bool keys[256];
	Vector3f m_pos;
	Vector3f m_target;
	Vector3f m_up;

	int m_windowWidth;
	int m_windowHeight;

	float m_AngleH;
	float m_AngleV;

	bool m_OnUpperEdge;
	bool m_OnLowerEdge;
	bool m_OnLeftEdge;
	bool m_OnRightEdge;
	
	SDL_Window* m_window;

	Vector2i m_mousePos;
};

#endif 