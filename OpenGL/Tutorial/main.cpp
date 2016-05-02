#include <fstream>
#include <SDL\SDL.h>
#include <gl\glew.h>
#include <time.h>
#include <SDL\SDL_opengl.h>
#include <gl\freeglut.h>

#include "pipeline.h"
#include "simple_color_technique.h"
#include "lighting_technique.h"
#include "camera.h"
#include "mesh.h"
#include "SDL_backend.h"
#include "engine_common.h" 
#include "picking_technique.h"
#include "picking_texture.h"
#undef main


using namespace std;

int WindowWidth = 1280, WindowHeight = 768;

class Main : public ICallbacks
{
public:

	Main()
	{
		m_pCamera = NULL;
		m_pLightingTechnique = NULL;
		/*m_pSimpleTechnique = NULL;
		m_pPickingTexture = NULL;
		m_pPickingTechnique = NULL;*/

		m_persProjInfo.FOV = 60.0f;
		m_persProjInfo.Height = WindowHeight;
		m_persProjInfo.Width = WindowWidth;
		m_persProjInfo.zNear = 1.0f;
		m_persProjInfo.zFar = 100.0f;

		m_worldPos[0] = Vector3f(-10.0f, 0.0f, 5.0f);
		m_worldPos[1] = Vector3f(10.0f, 0.0f, 5.0f);

		m_dirLight.AmbientIntensity = 0.2f;
		m_dirLight.DiffuseIntensity = 0.8f;
		m_dirLight.Color = Vector3f(1.0f, 1.0f, 1.0f);
		m_dirLight.Direction = Vector3f(1.0f, 0.0f, 0.0f);

		m_pDisplacementMap = NULL;
		m_dispFactor = 5.0f;
		m_isWireframe = false;
	}

	virtual ~Main()
	{
		SAFE_DELETE(m_pCamera);
		SAFE_DELETE(m_pMesh);
		//SAFE_DELETE(m_pSimpleTechnique);
		SAFE_DELETE(m_pLightingTechnique);
	}

	bool Init()
	{
		Vector3f Pos(0.0f, 5.0f, -22.0f);
		Vector3f Target(0.0f, -0.2f, 1.0f);
		Vector3f Up(0.0, 1.0f, 0.0f);

		GLint MaxPatchVertices = 0;
		glGetIntegerv(GL_MAX_PATCH_VERTICES, &MaxPatchVertices);
		printf("Max supported patch vertices %d\n", MaxPatchVertices);

		glActiveTexture(GL_TEXTURE4);
		m_pDisplacementMap = new Texture(GL_TEXTURE_2D, "Content/heightmap.jpg");

		if (!m_pDisplacementMap->Load()) {
			return false;
		}

		m_pDisplacementMap->Bind(DISPLACEMENT_TEXTURE_UNIT);

		glActiveTexture(GL_TEXTURE0);
		m_pColorMap = new Texture(GL_TEXTURE_2D, "Content/diffuse.jpg");

		if (!m_pColorMap->Load()) {
			return false;
		}

		m_pColorMap->Bind(COLOR_TEXTURE_UNIT);

		/*
		m_pSimpleTechnique = new SimpleColorTechnique();

		if (!m_pSimpleTechnique->Init())
		{
			printf("error init simple color technique");
			return false;
		}

		m_pPickingTechnique = new PickingTechnique();

		if (!m_pPickingTechnique->Init())
		{
			printf("error init picking technique");
			return false;
		}

		m_pPickingTexture = new PickingTexture();
		if (!m_pPickingTexture->Init(WindowWidth, WindowHeight))
		{
			printf("error init picking terchnique");
			return false;
		}
		*/
		m_pLightingTechnique = new LightingTechnique();

		if (!m_pLightingTechnique->Init())
		{
			printf("Error initializing the lighting technique\n");
			return false;
		}

		m_pLightingTechnique->Enable();
		m_pLightingTechnique->SetDirectionalLight(m_dirLight);
		m_pLightingTechnique->SetColorTextureUnit(COLOR_TEXTURE_UNIT_INDEX);
		//m_pLightingTechnique->SetDisplacementTextureUnit(DISPLACEMENT_TEXTURE_UNIT_INDEX);
		m_pLightingTechnique->SetTesselationLevel(m_dispFactor);
		GLExitIfError();
		m_pCamera = new Camera(WindowWidth, WindowHeight, Pos, Target, Up, gWindow);

		m_pMesh = new Mesh();

		return m_pMesh->LoadMesh("Content/monkey.obj");
	}

	void Run()
	{
		SDLBackendRun(this, gWindow);
	}

	virtual void KeyboardUpCB(int Key)
	{
		if (Key < 256)m_pCamera->SetKeyState(Key, false);
	}
	virtual void KeyboardDownCB(int Key)
	{
		switch (Key)
		{
		case 'z':
			m_isWireframe = !m_isWireframe;

			if (m_isWireframe) {
				glPolygonMode(GL_FRONT, GL_LINE);
			}
			else {
				glPolygonMode(GL_FRONT, GL_FILL);
			}
			break;
		case '[':
			m_dispFactor += 1.0f;
			break;

		case ']':
			m_dispFactor -= 1.0f;
			break;
		default: if (Key < 256)m_pCamera->SetKeyState(Key, true); break;
			
		}
		
		//printf("The %c key was pressed!\n", Key);
		
	}
	virtual void PassiveMouseCB(int x, int y)
	{
		m_pCamera->OnMouse(x, y, gWindow);
	}
	virtual void MouseCB(int Button, int State, int x, int y)
	{
		if (Button == SDL_BUTTON_LEFT) {
			m_leftMouseButton.IsPressed = !((State == SDL_PRESSED)&&m_leftMouseButton.IsPressed);
			m_leftMouseButton.x = x;
			m_leftMouseButton.y = y;
		}
	}
	/*
	void PickingPhase()
	{
		Pipeline p;
		p.Scale(0.1f, 0.1f, 0.1f);
		p.Rotate(0.0f, 90.0f, 0.0f);
		p.SetCamera(m_pCamera->GetPos(), m_pCamera->GetTarget(), m_pCamera->GetUp());
		p.SetPerspectiveProj(m_persProjInfo);

		m_pPickingTexture->EnableWriting();

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		m_pPickingTechnique->Enable();

		for (unsigned int i = 0; i < ARRAY_SIZE_IN_ELEMENTS(m_worldPos); i++) {
			p.WorldPos(m_worldPos[i]);
			m_pPickingTechnique->SetObjectIndex(i);
			m_pPickingTechnique->SetWVP(p.GetWVPTrans());
			m_pMesh->Render(m_pPickingTechnique);
		}

		m_pPickingTexture->DisableWriting();
	}

	void RenderPhase()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		Pipeline p;
		p.Scale(0.1f, 0.1f, 0.1f);
		p.Rotate(0.0f, 90.0f, 0.0f);
		p.SetCamera(m_pCamera->GetPos(), m_pCamera->GetTarget(), m_pCamera->GetUp());
		p.SetPerspectiveProj(m_persProjInfo);


		// If the left mouse button is clicked check if it hit a triangle
		// and color it red
		if (m_leftMouseButton.IsPressed) {
			PickingTexture::PixelInfo Pixel = m_pPickingTexture->ReadPixel(m_leftMouseButton.x, WindowHeight - m_leftMouseButton.y - 1);
			
			if (Pixel.PrimID != 0) {
				m_pSimpleTechnique->Enable();
				p.WorldPos(m_worldPos[Pixel.ObjectID]);
				m_pSimpleTechnique->SetWVP(p.GetWVPTrans());
				// Must compensate for the decrement in the FS!
				m_pMesh->Render(Pixel.DrawID, Pixel.PrimID - 1);
			}
		}

		// render the objects as usual

		m_pLightingTechnique->Enable();
		m_pLightingTechnique->SetEyeWorldPos(m_pCamera->GetPos());

		for (unsigned int i = 0; i < ARRAY_SIZE_IN_ELEMENTS(m_worldPos); i++) {
			p.WorldPos(m_worldPos[i]);
			m_pLightingTechnique->SetWVP(p.GetWVPTrans());
			m_pLightingTechnique->SetWorldMatrix(p.GetWorldTrans());
			m_pMesh->Render(NULL);
		}
	}*/
	virtual void RenderSceneCB()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		m_pCamera->OnKeyboard();
		m_pCamera->OnRender();

		Pipeline p;
		p.WorldPos(-3.0f, 0.0f, 0.0f);
		p.Scale(2.0f, 2.0f, 2.0f);
		p.Rotate(-90.0f, 15.0f, 0.0f);
		p.SetCamera(m_pCamera->GetPos(), m_pCamera->GetTarget(), m_pCamera->GetUp());
		p.SetPerspectiveProj(m_persProjInfo);

		//m_pLightingTechnique->SetEyeWorldPos(m_pCamera->GetPos());

		m_pLightingTechnique->SetVP(p.GetVPTrans());
		m_pLightingTechnique->SetWorldMatrix(p.GetWorldTrans());

		m_pLightingTechnique->SetTesselationLevel(m_dispFactor);
		m_pMesh->Render();

		p.WorldPos(3.0f, 0.0f, 0.0f);
		p.Rotate(-90.0f, -15.0f, 0.0f);
		m_pLightingTechnique->SetVP(p.GetVPTrans());
		m_pLightingTechnique->SetWorldMatrix(p.GetWorldTrans());
		m_pLightingTechnique->SetTesselationLevel(1.0f);
		m_pMesh->Render();
		//PickingPhase();
		//RenderPhase();

		/*
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		Pipeline p;
		p.Scale(0.5f, 0.5f, 0.5f);
		p.SetCamera(m_pCamera->GetPos(), m_pCamera->GetTarget(), m_pCamera->GetUp());
		p.SetPerspectiveProj(m_persProjInfo);

		m_pLightingTechnique->SetWVP(p.GetWVPTrans());
		m_pLightingTechnique->SetWorldMatrix(p.GetWorldTrans());

		//m_pSimpleTechnique->SetWVP(p.GetWVPTrans());

		m_pMesh->Render(NULL);*/
	}

	SDL_Window* gWindow;
	SDL_GLContext gContext;
private:
	DirectionalLight m_dirLight;
	LightingTechnique *m_pLightingTechnique;
	Texture * m_pColorMap;
	Texture * m_pDisplacementMap;
	/*SimpleColorTechnique *m_pSimpleTechnique;
	PickingTechnique *m_pPickingTechnique;
	PickingTexture *m_pPickingTexture;*/
	Camera *m_pCamera;
	
	struct {
		bool IsPressed;
		int x;
		int y;
	} m_leftMouseButton;
	Vector3f m_worldPos[2];
	PersProjInfo m_persProjInfo;
	Mesh *m_pMesh;
	float m_dispFactor;
	bool m_isWireframe;
};

int main(int argc, char** argv)
{
	SDLBackendInit(argc,argv);

	Main *pApp = new Main();

	if(!SDLBackendCreateWindow(WindowWidth, WindowHeight, 32,false, "OpenGL tutors", &pApp->gWindow, pApp->gContext))
	{
		return 1;
	}


	if (!pApp->Init())

	{
		return 1;
	}

 	pApp->Run();

	delete pApp;

	return 0;
}