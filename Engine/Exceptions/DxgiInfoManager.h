#pragma once

#include <Windows.h>
#include <vector>
#include <string>
#include <memory>
#include <wrl.h>

/*
 * @brief Redirect DirectX Exception towards W2Exception
 */
class DxgiInfoManager
{
public:
	DxgiInfoManager();
	~DxgiInfoManager();

	DxgiInfoManager(const DxgiInfoManager&) = delete;
	DxgiInfoManager& operator=(const DxgiInfoManager&) = delete;
	DxgiInfoManager(DxgiInfoManager&&) = delete;
	DxgiInfoManager& operator=(DxgiInfoManager&&) = delete;

	void Set() noexcept;
	std::vector<std::string> GetMessage() const;

private:
	unsigned long long m_next = 0u;
	struct IDXGIInfoQueue* m_dxgiInfoQueue{ nullptr };
};

#define RENDER_API_NOINFO(hrCall) if (FAILED(hr = (hrCall))) throw W2RenderAPI::HRException(__LINE__, __FILE__, hr)