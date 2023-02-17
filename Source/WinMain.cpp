#include <Windows.h>
#include "Argent/Core/ArApp.h"
#include <d3d12.h>
#include "Argent/Other/Misc.h"


#ifdef _DEBUG
void EnableDebugLayer()
{
	HRESULT hr{ S_OK };
	ID3D12Debug* debugLayer = nullptr;
	hr = D3D12GetDebugInterface(IID_PPV_ARGS(&debugLayer));
	_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));

	debugLayer->EnableDebugLayer();
	debugLayer->Release();
}
#endif


int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpszCmdLine, _In_ int nCmdShow){
	constexpr LONG windowWidth = 1280LL;
	constexpr LONG windowHeight = 720LL;

#ifdef _DEBUG 
	EnableDebugLayer();
#endif
	Argent::App::ArgentApp app(hInstance, windowWidth, windowHeight, "Dx12", false);
	return app.Execute();
}