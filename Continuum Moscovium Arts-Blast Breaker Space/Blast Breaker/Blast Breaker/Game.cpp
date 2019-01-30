#include <windows.h>
#include <windowsx.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <time.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <mmsystem.h>

#define SCREEN_WIDTH  800
#define SCREEN_HEIGHT 600
#define KEY_DOWN(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0)

#pragma comment (lib, "d3d9.lib")
#pragma comment (lib, "d3dx9.lib")
#pragma comment (lib, "winmm.lib")
LPDIRECT3D9 d3d;
LPDIRECT3DDEVICE9 d3ddev;
//plane and rintangan
LPD3DXMESH Rintangan1, Rintangan2,Rintangan3, Rintangan4, Rintangan5, Rintangan6, Rintangan7, Rintangan8, Rintangan9, Rintangan10,plane;
D3DMATERIAL9 *mat, *matPlane;
DWORD numMat, numMatPlane;

LPD3DXSPRITE d3dspt;

//LPD3DXSPRITE gameBG;
LPDIRECT3DTEXTURE9 spriteGameBG;

// menu sprite
//LPD3DXSPRITE d3dsptBG, d3dsptLG, d3dsptStartBtnIdle, d3dsptStartBtnHover, d3dsptStartBtnOnClick, d3dsptAboutBtnIdle, d3dsptAboutBtnHover, d3dsptSettingBtnIdle, d3dsptSettingBtnHover;
LPDIRECT3DTEXTURE9 spriteBG, spriteLG, spriteStartBtnIdle, spriteStartBtnHover, spriteStartBtnOnClick, spriteAboutBtnIdle, spriteAboutBtnHover, spriteSettingBtnIdle, spriteSettingBtnHover;

// about setting sprite
//LPD3DXSPRITE  d3dsptAbout, d3dsptSetting, d3dsptCheck1Off, d3dsptCheck1On, d3dsptCheck2Off, d3dsptCheck2On, d3dsptBackBtnOff, d3dsptBackBtnHover, d3dsptBackBtnClick;
LPDIRECT3DTEXTURE9 spriteAbout, spriteSetting, spriteCheckOff, spriteCheckOn, BackBtnOff, BackBtnHover, BackBtnClick;
LPDIRECT3DTEXTURE9 spritePause, spriteExit, spriteContinue;

LPD3DXFONT dxfont;
// effect declarations
LPD3DXEFFECT effect;	// define the effect pointer
D3DXHANDLE technique;	// define the handle for the best technique
//menu bool
bool menu = true;
bool game = false;
bool setting = false;
bool about = false;
bool win = false;
bool lose = false;
bool pause = false;

// col detection
POINT MousePos;

//rintangan
int a, ctr = 270;
int score = 0;

void initD3D(HWND hWnd);
void render_frame(void);
void cleanD3D(void);
void init_light(void);
void init_graphics(void);
void render_menu(void);
void load_display(void);
void render_game(void);
void render_setting(void);
void render_about(void);
void render_pause(void);

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow)
{
	HWND hWnd;
	WNDCLASSEX wc;

	ZeroMemory(&wc, sizeof(WNDCLASSEX));

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = (WNDPROC)WindowProc;
	wc.hInstance = hInstance;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.lpszClassName = L"WindowClass1";

	RegisterClassEx(&wc);

	hWnd = CreateWindowEx(NULL,
		L"WindowClass1",
		L"Blast Breaker Space Edition",
		WS_OVERLAPPEDWINDOW,
		0, 0,
		SCREEN_WIDTH, SCREEN_HEIGHT,
		NULL,
		NULL,
		hInstance,
		NULL);
	ShowWindow(hWnd, nCmdShow);
	initD3D(hWnd);
	MSG msg;
	while (TRUE)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				break;

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		render_frame();
	}
	cleanD3D();

	return msg.wParam;
}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		return 0;
	} break;
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}

void initD3D(HWND hWnd)
{
	d3d = Direct3DCreate9(D3D_SDK_VERSION);
	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.Windowed = TRUE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.hDeviceWindow = hWnd;
	d3dpp.BackBufferFormat = D3DFMT_X8R8G8B8;
	d3dpp.BackBufferWidth = SCREEN_WIDTH;
	d3dpp.BackBufferHeight = SCREEN_HEIGHT;
	d3dpp.BackBufferHeight = SCREEN_HEIGHT;
	
	d3dpp.EnableAutoDepthStencil = TRUE;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D16;

	d3d->CreateDevice(D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL,
		hWnd,
		D3DCREATE_SOFTWARE_VERTEXPROCESSING,
		&d3dpp,
		&d3ddev);
	d3ddev->SetRenderState(D3DRS_LIGHTING, TRUE);
	d3ddev->SetRenderState(D3DRS_ZENABLE, TRUE);
	d3ddev->SetRenderState(D3DRS_AMBIENT, D3DCOLOR_XRGB(50, 50, 50));
	//d3ddev->SetRenderState(D3DRS_ALPHABLENDENABLE, 1);
	//d3ddev->SetRenderState(D3DRS_ALPHATESTENABLE, 1);
	load_display();
	init_graphics();
	LPD3DXBUFFER errorlog;
	D3DXCreateEffectFromFile(d3ddev, L"dxtshaderFog.fx", 0, 0,
	D3DXSHADER_ENABLE_BACKWARDS_COMPATIBILITY, 0, &effect, &errorlog);
	effect->FindNextValidTechnique(NULL, &technique);
	init_light();
	return;
}

void load_display() {
	
	D3DXCreateFont(d3ddev,
		30,
		0,
		FW_NORMAL,
		1,
		false,
		DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS,
		DEFAULT_QUALITY,
		DEFAULT_PITCH | FF_DONTCARE,
		L"SFDistantGalaxy",
		&dxfont
	);

	D3DXCreateSprite(d3ddev, &d3dspt);
	D3DXCreateTextureFromFileEx(d3ddev,
		L"BackBtnOff.png",
		195,//w
		62,//h
		D3DX_DEFAULT,
		NULL,
		D3DFMT_A8R8G8B8,
		D3DPOOL_MANAGED,
		D3DX_DEFAULT,
		D3DX_DEFAULT,
		D3DCOLOR_XRGB(0, 0, 0),//Configure ini kalau mau menghilangkan warna yang tidak diinginkan 
		NULL,
		NULL,
		&BackBtnOff);
	D3DXCreateTextureFromFileEx(d3ddev,
		L"BackBtnHover.png",
		195,//w
		62,//h
		D3DX_DEFAULT,
		NULL,
		D3DFMT_A8R8G8B8,
		D3DPOOL_MANAGED,
		D3DX_DEFAULT,
		D3DX_DEFAULT,
		D3DCOLOR_XRGB(0, 0, 0),//Configure ini kalau mau menghilangkan warna yang tidak diinginkan 
		NULL,
		NULL,
		&BackBtnHover);
	D3DXCreateTextureFromFileEx(d3ddev,
		L"BackBtnClick.png",
		195,//w
		62,//h
		D3DX_DEFAULT,
		NULL,
		D3DFMT_A8R8G8B8,
		D3DPOOL_MANAGED,
		D3DX_DEFAULT,
		D3DX_DEFAULT,
		D3DCOLOR_XRGB(0, 0, 0),//Configure ini kalau mau menghilangkan warna yang tidak diinginkan 
		NULL,
		NULL,
		&BackBtnClick);

	D3DXCreateTextureFromFileEx(d3ddev,
		L"About.png",
		800,//w
		600,//h
		D3DX_DEFAULT,
		NULL,
		D3DFMT_A8R8G8B8,
		D3DPOOL_MANAGED,
		D3DX_DEFAULT,
		D3DX_DEFAULT,
		D3DCOLOR_XRGB(255, 0, 0),//Configure ini kalau mau menghilangkan warna yang tidak diinginkan 
		NULL,
		NULL,
		&spriteAbout);

	D3DXCreateTextureFromFileEx(d3ddev,
		L"Setting.png",
		800,//w
		600,//h
		D3DX_DEFAULT,
		NULL,
		D3DFMT_A8R8G8B8,
		D3DPOOL_MANAGED,
		D3DX_DEFAULT,
		D3DX_DEFAULT,
		D3DCOLOR_XRGB(255, 0, 0),//Configure ini kalau mau menghilangkan warna yang tidak diinginkan 
		NULL,
		NULL,
		&spriteSetting);

	D3DXCreateTextureFromFileEx(d3ddev,
		L"Checkbox_Off.png",
		100,//w
		100,//h
		D3DX_DEFAULT,
		NULL,
		D3DFMT_A8R8G8B8,
		D3DPOOL_MANAGED,
		D3DX_DEFAULT,
		D3DX_DEFAULT,
		D3DCOLOR_XRGB(255, 0, 0),//Configure ini kalau mau menghilangkan warna yang tidak diinginkan 
		NULL,
		NULL,
		&spriteCheckOff);

	D3DXCreateTextureFromFileEx(d3ddev,
		L"Checkbox_On.png",
		100,//w
		100,//h
		D3DX_DEFAULT,
		NULL,
		D3DFMT_A8R8G8B8,
		D3DPOOL_MANAGED,
		D3DX_DEFAULT,
		D3DX_DEFAULT,
		D3DCOLOR_XRGB(255, 0, 0),//Configure ini kalau mau menghilangkan warna yang tidak diinginkan 
		NULL,
		NULL,
		&spriteCheckOn);

	D3DXCreateTextureFromFileEx(d3ddev,
		L"gameBG.png",
		800,//w
		600,//h
		D3DX_DEFAULT,
		NULL,
		D3DFMT_A8R8G8B8,
		D3DPOOL_MANAGED,
		D3DX_DEFAULT,
		D3DX_DEFAULT,
		D3DCOLOR_XRGB(0, 0, 0),//Configure ini kalau mau menghilangkan warna yang tidak diinginkan 
		NULL,
		NULL,
		&spriteBG);
	
	D3DXCreateTextureFromFileEx(d3ddev,
		L"BlastBreakerLogo.png",
		600,//w
		290,//h
		D3DX_DEFAULT,
		NULL,
		D3DFMT_A8R8G8B8,
		D3DPOOL_MANAGED,
		D3DX_DEFAULT,
		D3DX_DEFAULT,
		D3DCOLOR_XRGB(255, 0, 0),
		NULL,
		NULL,
		&spriteLG);
	
	D3DXCreateTextureFromFileEx(d3ddev,
		L"StartBtnIdle.png",
		350,//w
		150,//h
		D3DX_DEFAULT,
		NULL,
		D3DFMT_A8R8G8B8,
		D3DPOOL_MANAGED,
		D3DX_DEFAULT,
		D3DX_DEFAULT,
		D3DCOLOR_XRGB(255, 0, 0),
		NULL,
		NULL,
		&spriteStartBtnIdle);
	
	D3DXCreateTextureFromFileEx(d3ddev,
		L"StartBtn.png",
		350,//w
		150,//h
		D3DX_DEFAULT,
		NULL,
		D3DFMT_A8R8G8B8,
		D3DPOOL_MANAGED,
		D3DX_DEFAULT,
		D3DX_DEFAULT,
		D3DCOLOR_XRGB(0, 0, 0),
		NULL,
		NULL,
		&spriteStartBtnHover);
		

	D3DXCreateTextureFromFileEx(d3ddev,
		L"AboutBtnIdle.png",
		128,//w
		128,//h
		D3DX_DEFAULT,
		NULL,
		D3DFMT_A8R8G8B8,
		D3DPOOL_MANAGED,
		D3DX_DEFAULT,
		D3DX_DEFAULT,
		D3DCOLOR_XRGB(0, 0, 0),
		NULL,
		NULL,
		&spriteAboutBtnIdle);

	D3DXCreateTextureFromFileEx(d3ddev,
		L"AboutBtn.png",
		128,//w
		128,//h
		D3DX_DEFAULT,
		NULL,
		D3DFMT_A8R8G8B8,
		D3DPOOL_MANAGED,
		D3DX_DEFAULT,
		D3DX_DEFAULT,
		D3DCOLOR_XRGB(0, 0, 0),
		NULL,
		NULL,
		&spriteAboutBtnHover);

	D3DXCreateTextureFromFileEx(d3ddev,
		L"SettingBtnIdle.png",
		128,//w
		128,//h
		D3DX_DEFAULT,
		NULL,
		D3DFMT_A8R8G8B8,
		D3DPOOL_MANAGED,
		D3DX_DEFAULT,
		D3DX_DEFAULT,
		D3DCOLOR_XRGB(0, 0, 0),
		NULL,
		NULL,
		&spriteSettingBtnIdle);

	D3DXCreateTextureFromFileEx(d3ddev,
		L"SettingBtn.png",
		128,//w
		128,//h
		D3DX_DEFAULT,
		NULL,
		D3DFMT_A8R8G8B8,
		D3DPOOL_MANAGED,
		D3DX_DEFAULT,
		D3DX_DEFAULT,
		D3DCOLOR_XRGB(0, 0, 0),
		NULL,
		NULL,
		&spriteSettingBtnHover);
	D3DXCreateTextureFromFileEx(d3ddev,
		L"gameBG.png",
		800,//w
		600,//h
		D3DX_DEFAULT,
		NULL,
		D3DFMT_A8R8G8B8,
		D3DPOOL_MANAGED,
		D3DX_DEFAULT,
		D3DX_DEFAULT,
		D3DCOLOR_XRGB(255, 255, 0),//Configure ini kalau mau menghilangkan warna yang tidak diinginkan 
		NULL,
		NULL,
		&spriteGameBG);

	D3DXCreateTextureFromFileEx(d3ddev,
		L"PauseNoBg.png",
		800,//w
		600,//h
		D3DX_DEFAULT,
		NULL,
		D3DFMT_A8R8G8B8,
		D3DPOOL_MANAGED,
		D3DX_DEFAULT,
		D3DX_DEFAULT,
		D3DCOLOR_XRGB(255, 0, 0),
		NULL,
		NULL,
		&spritePause);
	return;
}

void init_graphics(void)
{
	
	LPD3DXBUFFER R1;
	D3DXLoadMeshFromX(
		L"R1byZ.x",
		D3DXMESH_SYSTEMMEM,
		d3ddev,
		NULL,
		&R1,
		NULL,
		&numMat,
		&Rintangan1
	);

	D3DXLoadMeshFromX(
		L"R2byZ.x",
		D3DXMESH_SYSTEMMEM,
		d3ddev,
		NULL,
		&R1,
		NULL,
		&numMat,
		&Rintangan2
	);

	D3DXLoadMeshFromX(
		L"R3byZ.x",
		D3DXMESH_SYSTEMMEM,
		d3ddev,
		NULL,
		&R1,
		NULL,
		&numMat,
		&Rintangan3
	);

	D3DXLoadMeshFromX(
		L"R4byZ.x",
		D3DXMESH_SYSTEMMEM,
		d3ddev,
		NULL,
		&R1,
		NULL,
		&numMat,
		&Rintangan4
	);

	D3DXLoadMeshFromX(
		L"R5byZ.x",
		D3DXMESH_SYSTEMMEM,
		d3ddev,
		NULL,
		&R1,
		NULL,
		&numMat,
		&Rintangan5
	);

	D3DXLoadMeshFromX(
		L"R6byZ.x",
		D3DXMESH_SYSTEMMEM,
		d3ddev,
		NULL,
		&R1,
		NULL,
		&numMat,
		&Rintangan6
	);

	D3DXLoadMeshFromX(
		L"R7byZ.x",
		D3DXMESH_SYSTEMMEM,
		d3ddev,
		NULL,
		&R1,
		NULL,
		&numMat,
		&Rintangan7
	);

	D3DXLoadMeshFromX(
		L"R8byZ.x",
		D3DXMESH_SYSTEMMEM,
		d3ddev,
		NULL,
		&R1,
		NULL,
		&numMat,
		&Rintangan8
	);

	D3DXLoadMeshFromX(
		L"R9byZ.x",
		D3DXMESH_SYSTEMMEM,
		d3ddev,
		NULL,
		&R1,
		NULL,
		&numMat,
		&Rintangan9
	);

	D3DXLoadMeshFromX(
		L"R10byZ.x",
		D3DXMESH_SYSTEMMEM,
		d3ddev,
		NULL,
		&R1,
		NULL,
		&numMat,
		&Rintangan10
	);

	D3DXMATERIAL *tempMaterialsRin = (D3DXMATERIAL*)R1->GetBufferPointer();

	mat = new D3DMATERIAL9[numMat];

	for (DWORD i = 0; i < numMat; i++) {
		mat[i] = tempMaterialsRin[i].MatD3D;
		mat[i].Ambient = mat[i].Diffuse;
	};

	LPD3DXBUFFER bufPlane;
	D3DXLoadMeshFromX(
		L"xwing.x",
		D3DXMESH_SYSTEMMEM,
		d3ddev,
		NULL,
		&bufPlane,
		NULL,
		&numMatPlane,
		&plane
	);

	D3DXMATERIAL *tempMaterialsPlane = (D3DXMATERIAL*)bufPlane->GetBufferPointer();
	matPlane = new D3DMATERIAL9[numMatPlane];

	for (DWORD i = 0; i < numMatPlane; i++) {
		matPlane[i] = tempMaterialsPlane[i].MatD3D;
		matPlane[i].Ambient = matPlane[i].Diffuse;
	};
}

void render_menu(void)
{
	d3ddev->BeginScene();// Mulai gambar scene	
	D3DXVECTOR3 center(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 position(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 positionLG(100.0f, 0.0f, 0.0f);
	D3DXVECTOR3 positionStartBtn(230.0f, 250.0f, 0.0f);
	D3DXVECTOR3 positionAboutBtn(580.0f, 460.0f, 0.0f);
	D3DXVECTOR3 positionSettingBtn(680.0f, 467.0f, 0.0f);
	d3dspt->Begin(D3DXSPRITE_ALPHABLEND);

	//Background
	d3dspt->Draw(spriteBG,
		NULL,
		&center,
		&position,
		D3DCOLOR_ARGB(255, 255, 255, 255));
	//Logo
	d3dspt->Draw(spriteLG,
		NULL,
		&center,
		&positionLG,
		D3DCOLOR_ARGB(255, 255, 255, 255));
		
	PlaySound(L"MainMenu.wav", NULL, SND_ASYNC | SND_NOSTOP);

	// jika click start
	GetCursorPos(&MousePos);
	if ((MousePos.x >= positionStartBtn.x) && MousePos.x <= (positionStartBtn.x + 350) && MousePos.y >= positionStartBtn.y && MousePos.y <= (positionStartBtn.y + 150))
	{
		d3dspt->Draw(spriteStartBtnHover,
			NULL,
			&center,
			&positionStartBtn,
			D3DCOLOR_ARGB(255, 255, 255, 255));
		if (KEY_DOWN(VK_LBUTTON))
		{
			menu = false;
			PlaySound(NULL, 0, 0);
			game = true;
			PlaySound(L"InGame.wav", NULL, SND_ASYNC | SND_NOSTOP);
			setting = false;
			about = false;
		}
	}
	else
	{
		d3dspt->Draw(spriteStartBtnIdle,
			NULL,
			&center,
			&positionStartBtn,
			D3DCOLOR_ARGB(255, 255, 255, 255));
	}
	//jika klik about
	if ((MousePos.x >= positionAboutBtn.x) && MousePos.x <= (positionAboutBtn.x + 100) && MousePos.y >= positionAboutBtn.y && MousePos.y <= (positionAboutBtn.y + 128))
	{
		d3dspt->Draw(spriteAboutBtnHover,
			NULL,
			&center,
			&positionAboutBtn,
			D3DCOLOR_ARGB(255, 255, 255, 255));
		if (KEY_DOWN(VK_LBUTTON))
		{
			menu = false;
			game = false;
			setting = false;
			about = true;
		}
	}
	else
	{
		d3dspt->Draw(spriteAboutBtnIdle,
			NULL,
			&center,
			&positionAboutBtn,
			D3DCOLOR_ARGB(255, 255, 255, 255));
	}
	//jika klik setting
	if ((MousePos.x >= positionSettingBtn.x) && MousePos.x <= (positionSettingBtn.x + 100) && MousePos.y >= positionSettingBtn.y && MousePos.y <= (positionSettingBtn.y + 128))
	{
		d3dspt->Draw(spriteSettingBtnHover,
			NULL,
			&center,
			&positionSettingBtn,
			D3DCOLOR_ARGB(255, 255, 255, 255));
		if (KEY_DOWN(VK_LBUTTON))
		{
			menu = false;
			game = false;
			setting = true;
			about = false;
		}
	}
	else
	{
		d3dspt->Draw(spriteSettingBtnIdle,
			NULL,
			&center,
			&positionSettingBtn,
			D3DCOLOR_ARGB(255, 255, 255, 255));
	}
	d3dspt->End();
	d3ddev->EndScene();
	return;
}

void render_frame(void)
{
	d3ddev->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(5, 5, 5), 1.0f, 0);
	d3ddev->Clear(0, NULL, D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);
	if (menu == true)
	{
		render_menu();
	}
	else if (game == true)
	{
		render_game();
	}
	else if (setting == true)
	{
		render_setting();
	}
	else if (about == true)
	{
		render_about();
	}
	else if (pause == true)
	{
		render_pause();
	}
	d3ddev->Present(NULL, NULL, NULL, NULL);
	return;
}

void render_game(void)
{
	d3ddev->BeginScene();
	d3dspt->Begin(NULL);
	D3DXVECTOR3 center(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 position(0.0f, 0.0f, 0.0f);
	d3dspt->Draw(spriteGameBG,
		NULL,
		&center,
		&position,
		D3DCOLOR_ARGB(255, 255, 255, 255));
	static RECT scorebox;
	char sc[255] = { "Score : " };
	char tempscore[100];
	sprintf_s(tempscore, "%d", score);
	strcat_s(sc, tempscore);
	SetRect(&scorebox, 0, 0, 100, 100);
	dxfont->DrawTextA(
		NULL,
		sc,
		-1,
		&scorebox,
		DT_LEFT | DT_WORDBREAK,
		D3DCOLOR_ARGB(255, 255, 255, 255)
		);
	static RECT ex;
	char inst[255] = { "Press CTRL to exit" };
	SetRect(&ex, 0, 500, 100, 800);
	dxfont->DrawTextA(
		NULL,
		inst,
		-1,
		&ex,
		DT_LEFT | DT_WORDBREAK,
		D3DCOLOR_ARGB(255, 255, 255, 255)
	);
	static RECT pa;
	char pas[255] = { "Press SPACE to pause" };
	SetRect(&pa, 700, 500, 800, 600);
	dxfont->DrawTextA(
		NULL,
		pas,
		-1,
		&pa,
		DT_LEFT | DT_WORDBREAK,
		D3DCOLOR_ARGB(255, 255, 255, 255)
	);


	//start plane
	static float z = 6.0f, xaxis = 0.0f, yaxis = 0.0f, zaxis = 0.0f, x = 0.0f, y = 0.0f, z1 = 0.0f;
	static float xcam = 0.0f, ycam = 0.0f;
	static float index = 0.0f, index2;
	index += 0.03f;

	D3DXMATRIX matRotate, matTrans;

	if (KEY_DOWN(VK_RIGHT)) {
		zaxis += 0.025f;
	}
	if (KEY_DOWN(VK_LEFT)) {
		zaxis -= 0.025f;
	}
	if (KEY_DOWN(VK_UP)) {
		//xaxis += 0.025f;
		ycam += 0.025f;
	}
	if (KEY_DOWN(VK_DOWN)) {
		//xaxis -= 0.025f;
		ycam -= 0.025f;
	}
	if (KEY_DOWN(0x57) && y <=1.7f ) { //W
		y += 0.04f;
	}
	if (KEY_DOWN(0x53) && y>= - 1.7f) { //S
		y -= 0.04f;
	}
	if (KEY_DOWN(0x41) && x <= 2.2f) { //A
		x += 0.04f;
	}
	if (KEY_DOWN(0x44) && x >=- 2.2f) { //D
		x -= 0.04f;
	}
	if (KEY_DOWN(0x52)) {//tombol r
		xaxis = yaxis = 0.0f;
		x = y = z1 = 0.0f;
		xcam = ycam = 0.0f;
	}
	
	D3DXMatrixTranslation(
		&matTrans,
		x,
		y,
		z1
	);
	D3DXMATRIX matRotation1, matRotation2;
	D3DXMatrixRotationX(
		&matRotation1,
		xaxis);
	D3DXMatrixRotationZ(
		&matRotation2,
		zaxis);
	D3DXMATRIX matRotation = matRotation1 * matRotation2;

	//d3ddev->SetTransform(D3DTS_WORLD, &(matRotation * matTrans));
	effect->SetMatrix("World", &(matRotation * matTrans));
	
	//endplane

	D3DXMATRIX matView;
	D3DXMatrixLookAtLH(&matView,
		&D3DXVECTOR3(0.0f, 0.0f, 5.0f),
		&D3DXVECTOR3(0.0f, ycam, 0.0f),
		&D3DXVECTOR3(0.0f, 1.0f, 0.0f));
	//d3ddev->SetTransform(D3DTS_VIEW, &matView);
	effect->SetMatrix("View", &matView);

	D3DXMATRIX matProjection;
	D3DXMatrixPerspectiveFovLH(&matProjection,
		D3DXToRadian(45),
		(FLOAT)SCREEN_WIDTH / (FLOAT)SCREEN_HEIGHT,
		1.0f,
		50.0f);
	//d3ddev->SetTransform(D3DTS_PROJECTION, &matProjection);
	effect->SetMatrix("Projection", &matProjection);
	/*
	for (DWORD i = 0; i < numMatPlane; i++) {
		d3ddev->SetMaterial(&matPlane[i]);
		plane->DrawSubset(0);
	};
	*/
	effect->Begin(NULL, NULL);
	effect->BeginPass(0);
	plane->DrawSubset(0);
	effect->EndPass();
	effect->End();

	// rintangan start here
	static float idx = -50.0f;
	if (ctr == 270)
	{
		srand(time(0));
		a = rand() % 10 + 1;
	}
	if (a == 1)
	{
		D3DXMATRIX rint1;
		D3DXMatrixTranslation(
			&rint1,
			0.0f,
			0.0f,
			idx
		);
		//d3ddev->SetTransform(D3DTS_WORLD, &rint1);
		effect->SetMatrix("World", &rint1);
		for (DWORD i = 0; i < numMat; i++) {
			//d3ddev->SetMaterial(&mat[i]);
			effect->Begin(NULL, NULL);
			effect->BeginPass(0);
			Rintangan1->DrawSubset(0);
			effect->EndPass();
			effect->End();
		};
	}
	else if (a == 2)
	{
		D3DXMATRIX rint2;
		D3DXMatrixTranslation(
			&rint2,
			0.0f,
			0.0f,
			idx
		);
		//d3ddev->SetTransform(D3DTS_WORLD, &rint2);
		effect->SetMatrix("World", &rint2);
		for (DWORD i = 0; i < numMat; i++) {
			effect->Begin(NULL, NULL);
			effect->BeginPass(0);
			//d3ddev->SetMaterial(&mat[i]);
			Rintangan2->DrawSubset(0);
			effect->EndPass();
			effect->End();
		};
	}
	else if (a == 3)
	{
		D3DXMATRIX rint3;
		D3DXMatrixTranslation(
			&rint3,
			0.0f,
			0.0f,
			idx
		);
		//d3ddev->SetTransform(D3DTS_WORLD, &rint3);
		effect->SetMatrix("World", &rint3);
		for (DWORD i = 0; i < numMat; i++) {
			effect->Begin(NULL, NULL);
			effect->BeginPass(0);
			//d3ddev->SetMaterial(&mat[i]);
			Rintangan3->DrawSubset(0);
			effect->EndPass();
			effect->End();
		};
	}
	else if (a == 4)
	{
		D3DXMATRIX rint4;
		D3DXMatrixTranslation(
			&rint4,
			0.0f,
			0.0f,
			idx
		);
		//d3ddev->SetTransform(D3DTS_WORLD, &rint4);
		effect->SetMatrix("World", &rint4);
		for (DWORD i = 0; i < numMat; i++) {
			effect->Begin(NULL, NULL);
			effect->BeginPass(0);
			//d3ddev->SetMaterial(&mat[i]);
			Rintangan4->DrawSubset(0);
			effect->EndPass();
			effect->End();
		};
	}
	else if (a == 5)
	{
		D3DXMATRIX rint5;
		D3DXMatrixTranslation(
			&rint5,
			0.0f,
			0.0f,
			idx
		);
		//d3ddev->SetTransform(D3DTS_WORLD, &rint5);
		effect->SetMatrix("World", &rint5);
		for (DWORD i = 0; i < numMat; i++) {
			effect->Begin(NULL, NULL);
			effect->BeginPass(0);
			//d3ddev->SetMaterial(&mat[i]);
			Rintangan5->DrawSubset(0);
			effect->EndPass();
			effect->End();
		};
	}
	else if (a == 6)
	{
		D3DXMATRIX rint6;
		D3DXMatrixTranslation(
			&rint6,
			0.0f,
			0.0f,
			idx
		);
		//d3ddev->SetTransform(D3DTS_WORLD, &rint6);
		effect->SetMatrix("World", &rint6);
		for (DWORD i = 0; i < numMat; i++) {
			effect->Begin(NULL, NULL);
			effect->BeginPass(0);
			//d3ddev->SetMaterial(&mat[i]);
			Rintangan6->DrawSubset(0);
			effect->EndPass();
			effect->End();
		};
	}
	else if (a == 7)
	{
		D3DXMATRIX rint7;
		D3DXMatrixTranslation(
			&rint7,
			0.0f,
			0.0f,
			idx
		);
		//d3ddev->SetTransform(D3DTS_WORLD, &rint7);
		effect->SetMatrix("World", &rint7);
		for (DWORD i = 0; i < numMat; i++) {
			effect->Begin(NULL, NULL);
			effect->BeginPass(0);
			//d3ddev->SetMaterial(&mat[i]);
			Rintangan7->DrawSubset(0);
			effect->EndPass();
			effect->End();
		};
	}
	else if (a == 8)
	{
		D3DXMATRIX rint8;
		D3DXMatrixTranslation(
			&rint8,
			0.0f,
			0.0f,
			idx
		);
		//d3ddev->SetTransform(D3DTS_WORLD, &rint8);
		effect->SetMatrix("World", &rint8);
		for (DWORD i = 0; i < numMat; i++) {
			effect->Begin(NULL, NULL);
			effect->BeginPass(0);
			//d3ddev->SetMaterial(&mat[i]);
			Rintangan8->DrawSubset(0);
			effect->EndPass();
			effect->End();
		};
	}
	else if (a == 9)
	{
		D3DXMATRIX rint9;
		D3DXMatrixTranslation(
			&rint9,
			0.0f,
			0.0f,
			idx
		);
		//d3ddev->SetTransform(D3DTS_WORLD, &rint9);
		effect->SetMatrix("World", &rint9);
		for (DWORD i = 0; i < numMat; i++) {
			effect->Begin(NULL, NULL);
			effect->BeginPass(0);
			//d3ddev->SetMaterial(&mat[i]);
			Rintangan9->DrawSubset(0);
			effect->EndPass();
			effect->End();
		};
	}
	else if (a == 10)
	{
		D3DXMATRIX rint10;
		D3DXMatrixTranslation(
			&rint10,
			0.0f,
			0.0f,
			idx
		);
		//d3ddev->SetTransform(D3DTS_WORLD, &rint10);
		effect->SetMatrix("World", &rint10);
		for (DWORD i = 0; i < numMat; i++) {
			effect->Begin(NULL, NULL);
			effect->BeginPass(0);
			//d3ddev->SetMaterial(&mat[i]);
			Rintangan10->DrawSubset(0);
			effect->EndPass();
			effect->End();
		}
	}

	idx += 0.2f;
	ctr--;
	if (ctr == 0)
	{
		idx = -50.0f;
		ctr = 270;
		score++;
	}
	if (KEY_DOWN(0x11))// ctrl for reset
	{
		menu = true;
		game = false;
		ctr = 270;
		idx = -50.0f;
		xaxis = yaxis = 0.0f;
		x = y = z1 = 0.0f;
		xcam = ycam = 0.0f;
		score = 0;
		PlaySound(NULL, 0, 0);
	}
	if (KEY_DOWN(0x20))// Space for pause
	{
		pause = true;
		game = false;
	}
	

	/*
	static float idx1 = -50.0f, idx2 = -100.0f, idx3 = -150.0f, idx4 = -200.0f, idx5 = -250.0f;
	static float idx6 = -300.0f, idx7 = -350.0f, idx8 = -400.0f, idx9 = -450.0f, idx10 = -500.0f;
	idx1 += 0.2f; idx2 += 0.2f; idx3 += 0.2f; idx4 += 0.2f;
	idx5 += 0.2f; idx6 += 0.2f; idx7 += 0.2f; idx8 += 0.2f;
	idx9 += 0.2f; idx10 += 0.2f;
	
	if (idx10 == -500.0f)
	{
		system("Pause");
		game = false;
		menu = true;
	}

	//1
	D3DXMATRIX rint1;
	D3DXMatrixTranslation(
		&rint1,
		0.0f,
		0.0f,
		idx1
	);
	//d3ddev->SetTransform(D3DTS_WORLD, &rint1);
	effect->SetMatrix("World", &rint1);
	for (DWORD i = 0; i < numMat; i++) {
		//d3ddev->SetMaterial(&mat[i]);
		effect->Begin(NULL, NULL);
		effect->BeginPass(0);
		Rintangan1->DrawSubset(0);
		effect->EndPass();
		effect->End();
	};
	//2
	D3DXMATRIX rint2;
	D3DXMatrixTranslation(
		&rint2,
		0.0f,
		0.0f,
		idx2
	);
	//d3ddev->SetTransform(D3DTS_WORLD, &rint2);
	effect->SetMatrix("World", &rint2);
	for (DWORD i = 0; i < numMat; i++) {
		effect->Begin(NULL, NULL);
		effect->BeginPass(0);
		//d3ddev->SetMaterial(&mat[i]);
		Rintangan2->DrawSubset(0);
		effect->EndPass();
		effect->End();
	};
	//3
	D3DXMATRIX rint3;
	D3DXMatrixTranslation(
		&rint3,
		0.0f,
		0.0f,
		idx3
	);
	//d3ddev->SetTransform(D3DTS_WORLD, &rint3);
	effect->SetMatrix("World", &rint3);
	for (DWORD i = 0; i < numMat; i++) {
		effect->Begin(NULL, NULL);
		effect->BeginPass(0);
		//d3ddev->SetMaterial(&mat[i]);
		Rintangan3->DrawSubset(0);
		effect->EndPass();
		effect->End();
	};
	//4
	D3DXMATRIX rint4;
	D3DXMatrixTranslation(
		&rint4,
		0.0f,
		0.0f,
		idx4
	);
	//d3ddev->SetTransform(D3DTS_WORLD, &rint4);
	effect->SetMatrix("World", &rint4);
	for (DWORD i = 0; i < numMat; i++) {
		effect->Begin(NULL, NULL);
		effect->BeginPass(0);
		//d3ddev->SetMaterial(&mat[i]);
		Rintangan4->DrawSubset(0);
		effect->EndPass();
		effect->End();
	};
	//5
	D3DXMATRIX rint5;
	D3DXMatrixTranslation(
		&rint5,
		0.0f,
		0.0f,
		idx5
	);
	//d3ddev->SetTransform(D3DTS_WORLD, &rint5);
	effect->SetMatrix("World", &rint5);
	for (DWORD i = 0; i < numMat; i++) {
		effect->Begin(NULL, NULL);
		effect->BeginPass(0);
		//d3ddev->SetMaterial(&mat[i]);
		Rintangan5->DrawSubset(0);
		effect->EndPass();
		effect->End();
	};
	//6
	D3DXMATRIX rint6;
	D3DXMatrixTranslation(
		&rint6,
		0.0f,
		0.0f,
		idx6
	);
	//d3ddev->SetTransform(D3DTS_WORLD, &rint6);
	effect->SetMatrix("World", &rint6);
	for (DWORD i = 0; i < numMat; i++) {
		effect->Begin(NULL, NULL);
		effect->BeginPass(0);
		//d3ddev->SetMaterial(&mat[i]);
		Rintangan6->DrawSubset(0);
		effect->EndPass();
		effect->End();
	};
	//7
	D3DXMATRIX rint7;
	D3DXMatrixTranslation(
		&rint7,
		0.0f,
		0.0f,
		idx7
	);
	//d3ddev->SetTransform(D3DTS_WORLD, &rint7);
	effect->SetMatrix("World", &rint7);
	for (DWORD i = 0; i < numMat; i++) {
		effect->Begin(NULL, NULL);
		effect->BeginPass(0);
		//d3ddev->SetMaterial(&mat[i]);
		Rintangan7->DrawSubset(0);
		effect->EndPass();
		effect->End();
	};
	//8
	D3DXMATRIX rint8;
	D3DXMatrixTranslation(
		&rint8,
		0.0f,
		0.0f,
		idx8
	);
	//d3ddev->SetTransform(D3DTS_WORLD, &rint8);
	effect->SetMatrix("World", &rint8);
	for (DWORD i = 0; i < numMat; i++) {
		effect->Begin(NULL, NULL);
		effect->BeginPass(0);
		//d3ddev->SetMaterial(&mat[i]);
		Rintangan8->DrawSubset(0);
		effect->EndPass();
		effect->End();
	};
	//9
	D3DXMATRIX rint9;
	D3DXMatrixTranslation(
		&rint9,
		0.0f,
		0.0f,
		idx9
	);
	//d3ddev->SetTransform(D3DTS_WORLD, &rint9);
	effect->SetMatrix("World", &rint9);
	for (DWORD i = 0; i < numMat; i++) {
		effect->Begin(NULL, NULL);
		effect->BeginPass(0);
		//d3ddev->SetMaterial(&mat[i]);
		Rintangan9->DrawSubset(0);
		effect->EndPass();
		effect->End();
	};
	//10
	D3DXMATRIX rint10;
	D3DXMatrixTranslation(
		&rint10,
		0.0f,
		0.0f,
		idx10
	);
	//d3ddev->SetTransform(D3DTS_WORLD, &rint10);
	effect->SetMatrix("World", &rint10);
	for (DWORD i = 0; i < numMat; i++) {
		effect->Begin(NULL, NULL);
		effect->BeginPass(0);
		//d3ddev->SetMaterial(&mat[i]);
		Rintangan10->DrawSubset(0);
		effect->EndPass();
		effect->End();
	}
/
	
	*/
	d3ddev->EndScene();
}

void render_pause(void)
{
	d3ddev->BeginScene();// Mulai gambar scene	
	D3DXVECTOR3 center(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 position(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 positionLG(100.0f, 0.0f, 0.0f);
	d3dspt->Begin(D3DXSPRITE_ALPHABLEND);
	d3dspt->Draw(spritePause,
		NULL,
		&center,
		&position,
		D3DCOLOR_ARGB(255, 255, 255, 255));
	static RECT con;
	char co[255] = { "Press Enter to continue" };
	SetRect(&con, 250, 500, 800, 800);
	dxfont->DrawTextA(
		NULL,
		co,
		-1,
		&con,
		DT_LEFT | DT_WORDBREAK,
		D3DCOLOR_ARGB(255, 255, 255, 255)
	);

	if (KEY_DOWN(0x0D))// enter for continue
	{
		pause = false;
		game = true;
	}
	d3dspt->End();
	d3ddev->EndScene();
}

void render_setting(void)
{
	bool check1 = false, check2 = false;
	d3ddev->BeginScene();
	d3dspt->Begin(D3DXSPRITE_ALPHABLEND);
	D3DXVECTOR3 positionback(50.0f, 50.0f, 0.0f);
	D3DXVECTOR3 center(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 position(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 poscheck1(250.0f, 210.0f, 0.0f);
	D3DXVECTOR3 poscheck2(250.0f, 360.0f, 0.0f);

	d3dspt->Draw(spriteSetting,
		NULL,
		&center,
		&position,
		D3DCOLOR_ARGB(255, 255, 255, 255));
	

	d3dspt->Draw(spriteCheckOff,
		NULL,
		&center,
		&poscheck2,
		D3DCOLOR_ARGB(255, 255, 255, 255));
	GetCursorPos(&MousePos);
	if ((MousePos.x >= positionback.x) && MousePos.x <= (positionback.x + 195) && MousePos.y >= positionback.y && MousePos.y <= (positionback.y + 62))
	{
		d3dspt->Draw(BackBtnHover,
			NULL,
			&center,
			&positionback,
			D3DCOLOR_ARGB(255, 255, 255, 255));
		if (KEY_DOWN(VK_LBUTTON))
		{
			menu = true;
			game = false;
			setting = false;
			about = false;
		}
	}
	else
	{
		d3dspt->Draw(BackBtnOff,
			NULL,
			&center,
			&positionback,
			D3DCOLOR_ARGB(255, 255, 255, 255));
	}
	// checkbox1
	if ((MousePos.x >= poscheck1.x) && MousePos.x <= (poscheck1.x + 100) && MousePos.y >= poscheck1.y && MousePos.y <= (poscheck1.y + 100))
	{
		if (check1 == true)
		{
			d3dspt->Draw(spriteCheckOn,
				NULL,
				&center,
				&poscheck1,
				D3DCOLOR_ARGB(255, 255, 255, 255));
		}
		else if (check1 == false)
		{
			d3dspt->Draw(spriteCheckOff,
				NULL,
				&center,
				&poscheck1,
				D3DCOLOR_ARGB(255, 255, 255, 255));
		}
		if (KEY_DOWN(VK_LBUTTON) && check1 == false)
		{
			check1 = true;
		}
		if (KEY_DOWN(VK_LBUTTON) && check1 == true)
		{
			check1 = false;
		}
	}
	else
	{
		if (check1 == true)
		{
			d3dspt->Draw(spriteCheckOn,
				NULL,
				&center,
				&poscheck1,
				D3DCOLOR_ARGB(255, 255, 255, 255));
		}
		if (check1 == false)
		{
			d3dspt->Draw(spriteCheckOff,
				NULL,
				&center,
				&poscheck1,
				D3DCOLOR_ARGB(255, 255, 255, 255));
		}
		
	}
	d3dspt->End();
	d3ddev->EndScene();
}

void render_about(void)
{
	d3ddev->BeginScene();
	d3dspt->Begin(D3DXSPRITE_ALPHABLEND);
	/*
	d3dsptAbout->Begin(D3DXSPRITE_ALPHABLEND);
	d3dsptBackBtnOff->Begin(D3DXSPRITE_ALPHABLEND);
	d3dsptBackBtnHover->Begin(D3DXSPRITE_ALPHABLEND);
	*/
	D3DXVECTOR3 center(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 position(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 positionback(50.0f, 50.0f, 0.0f);

	d3dspt->Draw(spriteAbout,
		NULL,
		&center,
		&position,
		D3DCOLOR_ARGB(255, 255, 255, 255));
	
	GetCursorPos(&MousePos);
	if ((MousePos.x >= positionback.x) && MousePos.x <= (positionback.x + 195) && MousePos.y >= positionback.y && MousePos.y <= (positionback.y + 62))
	{
		
		d3dspt->Draw(BackBtnHover,
			NULL,
			&center,
			&positionback,
			D3DCOLOR_ARGB(255, 255, 255, 255));
		if (KEY_DOWN(VK_LBUTTON))
		{
			menu = true;
			game = false;
			setting = false;
			about = false;
		}
	}
	else
	{
		d3dspt->Draw(BackBtnOff,
			NULL,
			&center,
			&positionback,
			D3DCOLOR_ARGB(255, 255, 255, 255));
	}


	/*
	d3dsptAbout->End();
	d3dsptBackBtnOff->End();
	d3dsptBackBtnHover->End();
	*/
	
	d3dspt->End();
	d3ddev->EndScene();
}

void cleanD3D(void)
{
	Rintangan1->Release();
	Rintangan2->Release();
	Rintangan3->Release();
	Rintangan4->Release();
	Rintangan5->Release();
	Rintangan6->Release();
	Rintangan7->Release();
	Rintangan8->Release();
	Rintangan9->Release();
	Rintangan10->Release();
	spriteBG->Release();
	spriteLG->Release();
	spriteStartBtnIdle->Release();
	spriteAboutBtnIdle->Release();
	spriteSettingBtnIdle->Release();
	spriteGameBG->Release();
	spriteAboutBtnHover->Release();
	spriteStartBtnHover->Release();
	spriteSettingBtnHover->Release();
	spriteAbout->Release();
	spriteSetting->Release();
	spriteCheckOff->Release();
	spriteCheckOn->Release();
	BackBtnOff->Release();
	BackBtnHover->Release();
	plane->Release();
	effect->Release();
	d3ddev->Release();
	d3d->Release();
	return;
}

void init_light(void)
{
	D3DLIGHT9 light;
	D3DMATERIAL9 material;
	ZeroMemory(&light, sizeof(light));
	light.Type = D3DLIGHT_DIRECTIONAL;
	light.Diffuse = D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.0f);
	light.Direction = D3DXVECTOR3(-1.0f, -3.0f, -1.0f);
	d3ddev->SetLight(0, &light);
	d3ddev->LightEnable(0, TRUE);

	ZeroMemory(&material, sizeof(D3DMATERIAL9));
	material.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	material.Ambient = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	d3ddev->SetMaterial(&material);
	/*
	light.Diffuse.r = light.Ambient.r = 0.5f;
	light.Diffuse.g = light.Ambient.g = 0.5f;
	light.Diffuse.b = light.Ambient.b = 0.5f;

	D3DVECTOR vecPosition = { 2.0f,2.0f,2.0f };
	light.Position = vecPosition;
	effect->SetFloatArray("LightPos", &vecPosition.x, 3);
	effect->SetFloatArray("AmbientColor", &light.Ambient.r, 3);
	effect->SetFloatArray("DiffuseColor", &light.Diffuse.r, 3);
	*/
}

