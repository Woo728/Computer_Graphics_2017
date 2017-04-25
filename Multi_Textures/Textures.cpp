#include <Windows.h>
#include <Mmsystem.h>
#include <d3dx9.h>

LPDIRECT3D9 g_pD3D = nullptr;  // D3D 
LPDIRECT3DDEVICE9 g_pd3dDevice = nullptr;  // �������ϴ� D3D ����̽�
LPDIRECT3DVERTEXBUFFER9 g_pVB = nullptr;    // ���� ����
LPDIRECT3DTEXTURE9 g_pTexture0 = nullptr;    // �ؽ�ó0(��)
LPDIRECT3DTEXTURE9 g_pTexture1 = nullptr;    // �ؽ�ó1(Light Map)
LPDIRECT3DTEXTURE9 g_pTexture2 = nullptr;
LPDIRECT3DTEXTURE9 g_pTexture3 = nullptr;
LPDIRECT3DTEXTURE9 g_pTexture4 = nullptr;
LPDIRECT3DTEXTURE9 g_pTexture5 = nullptr;
LPDIRECT3DTEXTURE9 g_pTexture6 = nullptr;
LPDIRECT3DTEXTURE9 g_pTexture7 = nullptr;
LPDIRECT3DTEXTURE9 g_pTexture8 = nullptr;
LPDIRECT3DTEXTURE9 g_pTexture9 = nullptr;
LPDIRECT3DTEXTURE9 g_pTexture10 = nullptr;
LPDIRECT3DTEXTURE9 g_pTexture11 = nullptr;
LPDIRECT3DTEXTURE9 g_pTexture12 = nullptr;

static float counter1 = 0;
static float counter2 = 0;
											 // ����� ���� ���� ����ü
struct CUSTOMVERTEX {
	D3DXVECTOR3 position;   // ���� ��ǥ
	D3DCOLOR color;         // ���� ����
	FLOAT tu, tv;           // �ؽ�ó ��ǥ
};

// ����ڰ� ������ ������ ���� ������ ��Ÿ���� FVF ��
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1 | D3DFVF_TEX2)

HRESULT InitD3D(HWND hWnd);
HRESULT InitGeometry();
HRESULT InitTexture();
VOID Cleanup();
VOID Render();
VOID SetupMatrices();
LRESULT WINAPI MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, INT)
{
	// ������ Ŭ���� ���� �� �ʱ�ȭ, ���
	WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, MsgProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, L"D3D Tutorial", NULL };
	RegisterClassEx(&wc);

	// ������ ����
	HWND hWnd = CreateWindow(L"D3D Tutorial", L"D3D Tutorial 01: CreateDevice", WS_OVERLAPPEDWINDOW, 100, 100, 800, 800, GetDesktopWindow(), NULL, wc.hInstance, NULL);

	// Direct3D �ʱ�ȭ
	if (SUCCEEDED(InitD3D(hWnd))) {
		// ���� ���� �ʱ�ȭ
		if (SUCCEEDED(InitGeometry())) {
			// �ؽ�ó �ʱ�ȭ
			if (SUCCEEDED(InitTexture())) {
				// ������ ���
				ShowWindow(hWnd, SW_SHOWDEFAULT);
				UpdateWindow(hWnd);

				// �޼��� ����
				MSG msg;
				ZeroMemory(&msg, sizeof(msg));

				while (msg.message != WM_QUIT) {
					// GetMessage() �Լ��� �޸� ������� �ʴ´�.
					if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE)) {
						TranslateMessage(&msg);
						DispatchMessage(&msg);
					}
					else {
						// ó���� �޼����� ������ �������Ѵ�.
						Render();
					}
				}
			}
		}
	}

	// ����� Ŭ������ �޸𸮸� ��ȯ�ϰ�, ��Ͽ��� �����Ѵ�.
	UnregisterClass(L"D3D Tutorial", wc.hInstance);

	return 0;
}

HRESULT InitD3D(HWND hWnd)
{
	// D3D ��ü ����
	if (nullptr == (g_pD3D = Direct3DCreate9(D3D_SDK_VERSION))) {
		return E_FAIL;
	}

	// D3D ����̽��� �����ϱ� ���ؼ� �ʿ��� ����ü
	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory(&d3dpp, sizeof(D3DPRESENT_PARAMETERS));  // 0���� �ʱ�ȭ���� ������ ����� ������ ���� �� �ִ�.

	d3dpp.Windowed = TRUE;  // â���
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
	d3dpp.EnableAutoDepthStencil = TRUE;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D16;

	// D3DDEVTYPE_HAL�� �ϵ���� ������ �����ϵ��� �����Ѵ�. 
	// �������� g_pd3dDevice�� D3D ����̽� �����͸� �����Ѵ�. 
	if (FAILED(g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &g_pd3dDevice))) {
		return E_FAIL;
	}

	// �ﰢ���� �޸��� ǥ���ϱ� ���� �ڵ�, �ø� ����� ����.
	g_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	// ���� ����
	g_pd3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

	// z ���� ����� �Ҵ�.
	g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, TRUE);

	return S_OK;
}

// ���� ���� �ʱ�ȭ, �ؽ�ó �ʱ�ȭ
HRESULT InitGeometry()
{
	CUSTOMVERTEX vertecies[] = {
		{ D3DXVECTOR3(-1.0f, 1.0f, 0.f), D3DCOLOR(0xffffffff), 0.f, 0.f },
		{ D3DXVECTOR3(1.0f, 1.0f, 0.f), D3DCOLOR(0xffffffff), 1.f, 0.f },
		{ D3DXVECTOR3(-1.0f, -1.0f, 0.f), D3DCOLOR(0xffffffff), 0.f, 1.f },

		{ D3DXVECTOR3(1.0f, 1.0f, 0.f), D3DCOLOR(0xffffffff), 1.f, 0.f },
		{ D3DXVECTOR3(-1.0f, -1.0f, 0.f), D3DCOLOR(0xffffffff), 0.f, 1.f },
		{ D3DXVECTOR3(1.0f, -1.0f, 0.f), D3DCOLOR(0xffffffff), 1.f, 1.f },
	};

	// ���� ���� ����
	if (FAILED(g_pd3dDevice->CreateVertexBuffer(sizeof(vertecies), 0, D3DFVF_CUSTOMVERTEX, D3DPOOL_DEFAULT, &g_pVB, NULL))) {
		return E_FAIL;
	}

	CUSTOMVERTEX* pVertices;

	// Lock() �Լ��� ù ��°�� �� ��° ���ڸ� 0���� �����ϸ�
	// Vertex Buffer�� ��ü ���� ũ��� ����
	if (FAILED(g_pVB->Lock(0, 0, (void**)&pVertices, 0))) {
		return E_FAIL;
	}
	memcpy(pVertices, &vertecies, sizeof(vertecies));

	g_pVB->Unlock();

	return S_OK;
}

HRESULT InitTexture()
{
	// �ؽ�ó�� �����Ѵ�.
	D3DXCreateTextureFromFile(g_pd3dDevice, L"attack_1.png", &g_pTexture0);
	D3DXCreateTextureFromFile(g_pd3dDevice, L"attack_2.png", &g_pTexture1);
	D3DXCreateTextureFromFile(g_pd3dDevice, L"attack_3.png", &g_pTexture2);
	D3DXCreateTextureFromFile(g_pd3dDevice, L"attack_4.png", &g_pTexture3);
	D3DXCreateTextureFromFile(g_pd3dDevice, L"attack_5.png", &g_pTexture4);
	D3DXCreateTextureFromFile(g_pd3dDevice, L"effect_1.png", &g_pTexture5);
	D3DXCreateTextureFromFile(g_pd3dDevice, L"effect_2.png", &g_pTexture6);
	D3DXCreateTextureFromFile(g_pd3dDevice, L"effect_3.png", &g_pTexture7);
	D3DXCreateTextureFromFile(g_pd3dDevice, L"effect_4.png", &g_pTexture8);
	D3DXCreateTextureFromFile(g_pd3dDevice, L"effect_5.png", &g_pTexture9);
	D3DXCreateTextureFromFile(g_pd3dDevice, L"effect_6.png", &g_pTexture10);
	D3DXCreateTextureFromFile(g_pd3dDevice, L"effect_7.png", &g_pTexture11);
	D3DXCreateTextureFromFile(g_pd3dDevice, L"effect_8.png", &g_pTexture12);
	
	return S_OK;
}

// ��ü ���ҽ��� ��ȯ�ϴ� �Լ�
VOID Cleanup()
{
	// ��ü�� ���ҽ��� ��ȯ�ϴ� ������ �߿��ϴ�.
	// �� ������ �߸��Ǹ� ������ �߻���Ų��.
	// ������ ����

	if (g_pTexture0 != nullptr) {
		g_pTexture0->Release();
	}

	if (g_pTexture1 != nullptr) {
		g_pTexture1->Release();
	}

	if (g_pVB != nullptr) {
		g_pVB->Release();
	}

	if (g_pd3dDevice != nullptr) {
		g_pd3dDevice->Release();
	}

	if (g_pD3D != nullptr) {
		g_pD3D->Release();
	}
}

// ȭ���� �׸��� �Լ�
VOID Render()
{
	if (nullptr == g_pD3D || nullptr == g_pd3dDevice || nullptr == g_pVB || nullptr == g_pTexture0 || nullptr == g_pTexture1) {
		return;
	}

	// �ĸ� ���۸� �Ķ������� ä���. z���۸� �����.
	g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);

	// ���̾� ���������� ����.
	//g_pd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

	// BeginScene() �Լ��� ����̽����� Rendering ������ �˸���.
	if (SUCCEEDED(g_pd3dDevice->BeginScene())) {

		SetupMatrices();

		// ������ �ؽ�ó�� ���������� �Ҵ��Ѵ�.

		switch ((int)counter1 % 7)
		{
		case 0:
			g_pd3dDevice->SetTexture(0, g_pTexture0);
			break;
		case 1:
			g_pd3dDevice->SetTexture(0, g_pTexture1);
			break;
		case 2:
			g_pd3dDevice->SetTexture(0, g_pTexture2);
			break;
		case 3:
			g_pd3dDevice->SetTexture(0, g_pTexture3);
			break;
		case 4:
			g_pd3dDevice->SetTexture(0, g_pTexture4);
			break;
		case 5:
			g_pd3dDevice->SetTexture(0, g_pTexture1);
			break;
		case 6:
			g_pd3dDevice->SetTexture(0, g_pTexture0);
			break;
		}

		switch ((int)counter2 % 8)
		{
		case 0:
			g_pd3dDevice->SetTexture(1, g_pTexture5);
			break;					
		case 1:						 
			g_pd3dDevice->SetTexture(1, g_pTexture6);
			break;					 
		case 2:						 
			g_pd3dDevice->SetTexture(1, g_pTexture7);
			break;					 
		case 3:						 
			g_pd3dDevice->SetTexture(1, g_pTexture8);
			break;					 
		case 4:						 
			g_pd3dDevice->SetTexture(1, g_pTexture9);
			break;					 
		case 5:						 
			g_pd3dDevice->SetTexture(1, g_pTexture10);
			break;
		case 6:
			g_pd3dDevice->SetTexture(1, g_pTexture11);
			break;
		case 7:
			g_pd3dDevice->SetTexture(1, g_pTexture12);
			break;
		}




		// 0�� �ؽ�ó�� 0�� �ؽ�ó �ε����� ����Ѵ�.(FVF�� �������� 8���� �ؽ�ó �ε����� �ִ�)
		g_pd3dDevice->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, 0);
		// 1�� �ؽ�ó�� 0�� �ؽ�ó �ε����� ����Ѵ�.
		g_pd3dDevice->SetTextureStageState(1, D3DTSS_TEXCOORDINDEX, 0);

		// �ؽ�ó ������ �Ҷ� Ȯ�� ���͸� ����ϰ�, Linear ������� �����Ѵ�.
		g_pd3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
		g_pd3dDevice->SetSamplerState(1, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);

		// D3DTSS_COLOROP, D3DTSS_ALPHAOP�� �Բ�
		// D3DTOP_SELECTARG1�� ����ϸ� �̹� stage�� input�� �ٷ� output���� �����Ѵ�.
		// 0�� ���������� �÷� ���� ���� ���� �״�� output���� ���޵ȴ�.
		g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);

		g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);

		g_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);

		g_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);

		// color operation�� modulate�� ����
		g_pd3dDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_ADD);

		g_pd3dDevice->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_MODULATE);

		// �÷��� ù ��° ���ڴ� ���� ���������� �ؽ�ó�� �÷� ��
		g_pd3dDevice->SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_TEXTURE);

		// �÷��� �� ��° ���ڴ� ���� ������������ ���޵� �÷� ��
		g_pd3dDevice->SetTextureStageState(1, D3DTSS_COLORARG2, D3DTA_CURRENT);

		// ������ ù ��° ���ڴ� ���� ���������� �ؽ�ó�� ���� ��
		g_pd3dDevice->SetTextureStageState(1, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);

		// ������ �� ��° ���ڴ� ���� ������������ ���޵� ���� ��
		g_pd3dDevice->SetTextureStageState(1, D3DTSS_ALPHAARG2, D3DTA_CURRENT);

		// �ᱹ SetTextureStageState() �Լ��� ����Ͽ� �ؽ�ó1�� �ؽ�ó2�� ���� �������Ͽ���.


		// 3��° ���������������ʹ� �÷��� ���İ��� ���� ������ ���� �ʴ´�.
		g_pd3dDevice->SetTextureStageState(2, D3DTSS_COLOROP, D3DTOP_DISABLE);
		g_pd3dDevice->SetTextureStageState(2, D3DTSS_ALPHAOP, D3DTOP_DISABLE);


		// ���� ���۸� ��� ��Ʈ������ �����Ѵ�.
		g_pd3dDevice->SetStreamSource(0, g_pVB, 0, sizeof(CUSTOMVERTEX));

		// D3D�� ���� ������ �����Ѵ�.
		g_pd3dDevice->SetFVF(D3DFVF_CUSTOMVERTEX);

		// ���� ���� ��ü�� �׸���.
		g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 2);

		// Rendering�� �������� �˸���.
		g_pd3dDevice->EndScene();
	}

	// �ĸ� ���۸� ���� �׸��� ���۷� �����Ѵ�.
	// Double Buffering(���� ����)�� ���õ� �����̴�.
	// ���� ���۸� ����ϸ鼭 �� �Լ��� �������� ������ ����� �׷����� �ʴ´�.
	g_pd3dDevice->Present(NULL, NULL, NULL, NULL);
}

// ����, ��, �������� ��� ����
VOID SetupMatrices()
{
	// ���� ��� ����
	D3DXMATRIXA16 matWorld;

	D3DXMatrixIdentity(&matWorld);
	// �Ʒ����� �ּ��� �����ϸ� ������ x���� �������� ȸ���Ѵ�.
	//D3DXMatrixRotationX( &matWorld, timeGetTime() / 500.0f );
	g_pd3dDevice->SetTransform(D3DTS_WORLD, &matWorld);

	// �� ��� ����
	D3DXVECTOR3 vEyePt(0.0f, 0.0f, -3.0f);  // ī�޶��� ��ġ
	D3DXVECTOR3 vLookatPt(0.0f, 0.0f, 0.0f);    // ī�޶� �ٶ󺸴� ����
	D3DXVECTOR3 vUpVec(0.0f, 1.0f, 0.0f);   // ī�޶��� ���⺤��

	D3DXMATRIXA16 matView;
	// ī�޶� ��ȯ ��� ���
	D3DXMatrixLookAtLH(&matView, &vEyePt, &vLookatPt, &vUpVec);
	// ���� ī�޶� ��ȯ ����� ����
	g_pd3dDevice->SetTransform(D3DTS_VIEW, &matView);

	// �������� ��� ����
	D3DXMATRIXA16 matProj;
	// ���� ��ȯ ��� ���
	D3DXMatrixPerspectiveFovLH(&matProj, D3DX_PI / 2, 1.0f, 1.0f, 100.f);
	// ���� ���� ��ȯ ����� ����
	g_pd3dDevice->SetTransform(D3DTS_PROJECTION, &matProj);
}

// ������ ���ν��� �Լ�
LRESULT WINAPI MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg) {
	case WM_KEYDOWN:
		counter1 = counter1 + 0.2f, counter2 = counter2 + 0.4f;
		break;
	case WM_DESTROY:
		Cleanup();
		PostQuitMessage(0);
		return 0;

		// �޼��� ���� ����
		/*case WM_PAINT:
		Render();
		ValidateRect(hWnd, NULL);
		return 0;*/
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}