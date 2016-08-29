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
#include "FontRender.h"
#include "ds_geom_pass_technique.h"
#include "gbuffer.h"
#undef main
enum GBUFFER_TEXTURE_TYPE {
	GBUFFER_TEXTURE_TYPE_POSITION,
	GBUFFER_TEXTURE_TYPE_DIFFUSE,
	GBUFFER_TEXTURE_TYPE_NORMAL,
	GBUFFER_TEXTURE_TYPE_TEXCOORD,
	GBUFFER_NUM_TEXTURES
};

using namespace std;

int WindowWidth = 1280, WindowHeight = 768;

#define NUM_ROWS 50
#define NUM_COLS 20
#define NUM_INSTANCES NUM_ROWS * NUM_COLS

GLuint VBO;

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
		m_scale = 0;
		m_pFont = 0;
	}

	virtual ~Main()
	{
		SAFE_DELETE(m_pCamera);
		SAFE_DELETE(m_pMesh);
		//SAFE_DELETE(m_pSimpleTechnique);
		SAFE_DELETE(m_pLightingTechnique);
		SAFE_DELETE(m_pColorMap);
		SAFE_DELETE(m_pDisplacementMap);
		SAFE_DELETE(m_pDSGeomPassTech);
		SAFE_DELETE(m_pFont);
	}

	bool Init() 
	{
		Vector3f Pos(7.0f, 3.0f, 0.0f);
		Vector3f Target(0.0f, -0.2f, 1.0f);
		Vector3f Up(0.0, 1.0f, 0.0f);

		//GLint MaxPatchVertices = 0;
		//glGetIntegerv(GL_MAX_PATCH_VERTICES, &MaxPatchVertices);
		//printf("Max supported patch vertices %d\n", MaxPatchVertices);

		//glActiveTexture(GL_TEXTURE4);
		//m_pDisplacementMap = new Texture(GL_TEXTURE_2D, "Content/heightmap.jpg");

		//if (!m_pDisplacementMap->Load()) {
		//	return false;
		//}

		//m_pDisplacementMap->Bind(DISPLACEMENT_TEXTURE_UNIT);

		//glActiveTexture(GL_TEXTURE0);
		//m_pColorMap = new Texture(GL_TEXTURE_2D, "Content/diffuse.jpg");

		//if (!m_pColorMap->Load()) {
		//	return false;
		//}

		//m_pColorMap->Bind(COLOR_TEXTURE_UNIT);
		
		m_pMesh = new Mesh();

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
		m_pLightingTechnique->SetMatSpecularPower(0);
		m_pLightingTechnique->SetColorTextureUnit(COLOR_TEXTURE_UNIT_INDEX);
		m_pLightingTechnique->SetMatSpecularIntensity(0.0f);
		m_pLightingTechnique->SetColor(0, Vector4f(1.0f, 0.5f, 0.5f, 0.0f));
		m_pLightingTechnique->SetColor(1, Vector4f(0.5f, 1.0f, 1.0f, 0.0f));
		m_pLightingTechnique->SetColor(2, Vector4f(1.0f, 0.5f, 1.0f, 0.0f));
		m_pLightingTechnique->SetColor(3, Vector4f(1.0f, 1.0f, 1.0f, 0.0f));
		//m_pLightingTechnique->SetDisplacementTextureUnit(DISPLACEMENT_TEXTURE_UNIT_INDEX);
		//m_pLightingTechnique->SetTesselationLevel(m_dispFactor);
		GLExitIfError();
		m_pCamera = new Camera(WindowWidth, WindowHeight, Pos, Target, Up, gWindow);

		CalcPositions();

		m_pFont = new FontRenderer();
		if (!m_pFont->Init("arial.ttf")) {
			cout << "Couldn't init FontRenderer" << endl;
			return false;
		}
		m_pGBuffer = new GBuffer();
		m_pGBuffer->Init(WindowWidth, WindowHeight);
		m_pDSGeomPassTech = new DSGeomPassTech();
		if (!m_pDSGeomPassTech->Init()) {
			cout << "Couldn't init DSgeometryPassTech" << endl;
			return false;
		}
		m_pDSGeomPassTech->Enable();
		m_pDSGeomPassTech->SetColorTextureUnit(COLOR_TEXTURE_UNIT_INDEX);

		m_simple.Init();
		ReadyFBO();
		GLExitIfError();
		return m_pMesh->LoadMesh("Content/phoenix_ugv.md2");
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
	void DSGeometryPass() 
	{
		m_pDSGeomPassTech->Enable();
		glUseProgram(0);
		m_pGBuffer->BindForWriting();

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		Pipeline p;
		p.Scale(0.1f, 0.1f, 0.1f);
		p.Rotate(0.0f, m_scale, 0.0f);
		p.WorldPos(-0.8f, -1.0f, 12.0f);
		p.SetCamera(m_pCamera->GetPos(), m_pCamera->GetTarget(), m_pCamera->GetUp());
		p.SetPerspectiveProj(m_persProjInfo);
		m_pDSGeomPassTech->SetWVP(p.GetWVPTrans());
		m_pDSGeomPassTech->SetWorldMatrix(p.GetWorldTrans());

		//m_pMesh->Render(NUM_INSTANCES, WVPMatricx, WorldMatrics);
		//m_simple.Enable();
		m_simple.SetWVP(p.GetWVPTrans());


		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

		glDrawArrays(GL_TRIANGLES, 0, 3);

		glDisableVertexAttribArray(0);


		//m_pMesh->Render();
		GLint Status;
		glGetIntegerv(GL_READ_BUFFER, &Status);
		printf("%d \n", (Status));
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glGetIntegerv(GL_READ_BUFFER, &Status);
		printf("%d \n", (Status));
	}
	void DSLightingPass() 
	{
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		//glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);

		//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		m_pGBuffer->BindForReading();

		GLint Status;
		glGetIntegerv(GL_READ_BUFFER, &Status);
		printf("%d \n", (Status));

		GLsizei HalfWidth = (GLsizei)(WindowWidth / 2.0f);
		GLsizei HalfHeight = (GLsizei)(WindowHeight / 2.0f);
		//glReadBuffer(GL_COLOR_ATTACHMENT0);
		m_pGBuffer->SetReadBuffer(GBuffer::GBUFFER_TEXTURE_TYPE_POSITION);
		glBlitFramebuffer(0, 0, HalfWidth,WindowHeight, HalfWidth, 0, WindowWidth, WindowHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);

		//m_gBuffer.SetReadBuffer(GBuffer::GBUFFER_TEXTURE_TYPE_DIFUSE);
		//glBlitFramebuffer(0, 0, WindowWidth, WindowHeight, 0, HalfHeight, HalfWidth, WindowHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);

		//m_gBuffer.SetReadBuffer(GBuffer::GBUFFER_TEXTURE_TYPE_NORMAL);
		//glBlitFramebuffer(0, 0, WindowWidth, WindowHeight, HalfWidth, HalfHeight, WindowWidth, WindowHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);

		//m_gBuffer.SetReadBuffer(GBuffer::GBUFFER_TEXTURE_TYPE_TEXCOORD);
		//glBlitFramebuffer(0, 0, WindowWidth, WindowHeight, HalfWidth, 0, WindowWidth, HalfHeight, GL_COLOR_BUFFER_BIT,GL_LINEAR);

	}

	static void CreateVertexBuffer()
	{
		Vector3f Vertices[3];
		Vertices[0] = Vector3f(0.0f, 1.0f, 0.0f);
		Vertices[1] = Vector3f(1.0f, 0.0f, 1.0f);
		Vertices[2] = Vector3f(-1.0f, 0.0f, 0.0f);

		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);
	}

	virtual void RenderSceneCB()
	{
		CalcFPS();
		//m_pLightingTechnique->Enable();
		m_scale += 0.05f;
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		m_pCamera->OnKeyboard();
		m_pCamera->OnRender();
		
		//Pipeline p;
		//p.Rotate(0.0f, 90.0f, 0.0f);
		//p.Scale(0.001f, 0.001f, 0.001f);
		//
		//p.SetCamera(m_pCamera->GetPos(), m_pCamera->GetTarget(), m_pCamera->GetUp());
		//p.SetPerspectiveProj(m_persProjInfo);

		//Matrix4f WVPMatricx[NUM_INSTANCES];
		//Matrix4f WorldMatrics[NUM_INSTANCES];

		//for (unsigned int i = 0; i < NUM_INSTANCES; i++) {
		//	Vector3f Pos(m_positions[i]);
		//	Pos.y += sinf(m_scale)*m_velocity[i];
		//	p.WorldPos(Pos);
		//	WVPMatricx[i] = p.GetWVPTrans().Transpose();
		//	WorldMatrics[i] = p.GetWorldTrans().Transpose();
		//}

		RenderFBO();

		//DSGeometryPass();
		//DSLightingPass();
		
		//m_pLightingTechnique->SetEyeWorldPos(m_pCamera->GetPos());

		//m_pLightingTechnique->SetVP(p.GetVPTrans());
		//m_pLightingTechnique->SetWorldMatrix(p.GetWorldTrans());

		//m_pLightingTechnique->SetTesselationLevel(m_dispFactor);
		
		//m_pMesh->Render(NUM_INSTANCES,WVPMatricx,WorldMatrics);
		
		
		//p.WorldPos(3.0f, 0.0f, 0.0f);
		//p.Rotate(-90.0f, -15.0f, 0.0f);
		//m_pLightingTechnique->SetVP(p.GetVPTrans());
		//m_pLightingTechnique->SetWorldMatrix(p.GetWorldTrans());
		//m_pLightingTechnique->SetTesselationLevel(1.0f);
		//m_pMesh->Render();
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
		ShowFPS();
	}
	void CalcPositions()
	{
		for (unsigned int i = 0; i < NUM_ROWS; i++) {
			for (unsigned int j = 0; j < NUM_COLS; j++) {
				unsigned int Index = i * NUM_COLS + j;
				m_positions[Index].x = (float)j;
				m_positions[Index].y = RandomFloat() * 5.0f;
				m_positions[Index].z = (float)i;
				m_velocity[Index] = RandomFloat();
				if (i & 1) {
					m_velocity[Index] *= (-1.0f);
				}
			}
		}
	}
	SDL_Window* gWindow;
	SDL_GLContext gContext;
private:
	void CalcFPS() {
		m_frameCount++;
		int time = glutGet(GLUT_ELAPSED_TIME);
		if (time - m_time > 1000) {
			m_FPS = (float)m_frameCount*1000.0f / (time - m_time);
			m_time = time;
			m_frameCount = 0;
		}
	}

	void ShowFPS() {
		char text[30];
		snprintf(text, sizeof(text), "FPS: %.2f", m_FPS);
		m_pFont->RenderText(text, -1.0, 0.9, 0.1, Vector3f(1.0, 1.0, 1.0));
	}

	void ReadyFBO() {
		glGenFramebuffers(1, &gBuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);

		glGenTextures(4, m_textures);

		for (unsigned int i = 0; i < 4; i++) {
			glBindTexture(GL_TEXTURE_2D, m_textures[i]);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, WindowWidth, WindowHeight, 0, GL_RGB, GL_FLOAT, NULL);
			glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, m_textures[i], 0);
		}

		//glGenTextures(1, &gPosition);
		//glBindTexture(GL_TEXTURE_2D, gPosition);
		//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, WindowWidth, WindowWidth, 0, GL_RGB, GL_FLOAT, NULL);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);
		//// - Normal color buffer
		//glGenTextures(1, &gNormal);
		//glBindTexture(GL_TEXTURE_2D, gNormal);
		//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, WindowWidth, WindowWidth, 0, GL_RGB, GL_FLOAT, NULL);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);
		//// - Color + Specular color buffer
		//glGenTextures(1, &gAlbedoSpec);
		//glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
		//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, WindowWidth, WindowWidth, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedoSpec, 0);
		// - Tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 
		GLuint attachments[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 , GL_COLOR_ATTACHMENT3 };
		glDrawBuffers(4, attachments);
		// - Create and attach depth buffer (renderbuffer)
		GLuint rboDepth;
		glGenRenderbuffers(1, &rboDepth);
		glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, WindowWidth, WindowWidth);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
		// - Finally check if framebuffer is complete
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "Framebuffer not complete!" << std::endl;
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

		CreateVertexBuffer();
	}
	void RenderFBO() {
		glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

		glDrawArrays(GL_TRIANGLES, 0, 3);

		glDisableVertexAttribArray(0);

		GLint Status;
		glGetIntegerv(GL_READ_BUFFER, &Status);
		printf("%d\n", Status);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glGetIntegerv(GL_READ_BUFFER, &Status);
		printf("%d\n", Status);
		glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer);
		glReadBuffer(GL_COLOR_ATTACHMENT0);
		glGetIntegerv(GL_READ_BUFFER, &Status);
		printf("%d\n", Status);
		glBlitFramebuffer(0, WindowHeight / 2.0f, WindowWidth, WindowHeight, 0, 0, WindowWidth, WindowHeight / 2.0f, GL_COLOR_BUFFER_BIT, GL_LINEAR);

	}

	GLuint m_textures[GBUFFER_NUM_TEXTURES];
	GLuint gBuffer;

	GBuffer* m_pGBuffer;
	DirectionalLight m_dirLight;
	LightingTechnique *m_pLightingTechnique;
	DSGeomPassTech *m_pDSGeomPassTech;
	Texture * m_pColorMap;
	SimpleColorTechnique m_simple;
	Texture * m_pDisplacementMap;
	/*SimpleColorTechnique *m_pSimpleTechnique;
	PickingTechnique *m_pPickingTechnique;
	PickingTexture *m_pPickingTexture;*/
	Camera *m_pCamera;
	Vector3f m_positions[NUM_INSTANCES];
	float m_scale;
	float m_velocity[NUM_INSTANCES];
	FontRenderer *m_pFont;
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
	float m_FPS;
	int m_frameCount;
	int m_time;
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