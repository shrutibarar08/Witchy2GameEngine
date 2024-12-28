#pragma once

#define RENDER_API_EXCEPT_NOINFO(hr) W2RenderAPI::HRException(__LINE__, __FILE__, (hr))
#define RENDER_API_THROW_NOINFO(hrCall) if (FAILED(hr=(hrCall))) throw RENDER_API_EXCEPT_NOINFO(hr)

#ifdef _DEBUG
#define ON_DEBUG TRUE

#define RENDER_API_EXCEPT(hr) W2RenderAPI::HRException(__LINE__, __FILE__, (hr), m_dxgiInfoManager.GetMessage())
#define RENDER_API_THROW(hrCall) m_dxgiInfoManager.Set(); if (FAILED(hr=(hrCall))) throw RENDER_API_EXCEPT(hr)
#define RENDER_API_REMOVED_EXCEPT(hr) W2RenderAPI::DeviceRemovedException(__LINE__, __FILE__, (hr), m_dxgiInfoManager.GetMessage())
#define RENDER_API_INFO_ONLY(hrCall) m_dxgiInfoManager.Set(); (hrCall); { auto v = m_dxgiInfoManager.GetMessage(); if (!v.empty()) throw W2RenderAPI::InfoException(__LINE__, __FILE__, std::move(v)); }
#else
#define ON_DEBUG FALSE

#define RENDER_API_EXCEPT(hr) W2RenderAPI::HRException(__LINE__, __FILE__, (hr))
#define RENDER_API_THROW(hrCall) RENDER_API_THROW_NOINFO(hrCall)
#define RENDER_API_REMOVED_EXCEPT(hr) W2RenderAPI::DeviceRemovedException(__LINE__, __FILE__, (hr))
#define RENDER_API_INFO_ONLY(hrCall) (hrCall)
#endif


#ifdef _DEBUG
#define INIT_INFO() HRESULT hr; DxgiInfoManager& m_dxgiInfoManager = W2RenderAPI::Get()->GetInfoManager()
#else
#define INIT_INFO() HRESULT hr
#endif
