#include "DxgiInfoManager.h"

#include "WindowsManager/W2WindowAPI.h"
#include "RenderManager/W2RenderAPI.h"

#include <dxgidebug.h>


DxgiInfoManager::DxgiInfoManager()
{
	typedef HRESULT(WINAPI* DXGIGetDebugInterface)(REFIID, void**);

	const auto hModDxgiDebug = LoadLibraryEx("dxgidebug.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
	if (hModDxgiDebug == nullptr)
	{
		throw W2WND_LAST_EXCEPT();
	}

	const auto DxgiGetDebugInterface = reinterpret_cast<DXGIGetDebugInterface>(GetProcAddress(hModDxgiDebug, "DXGIGetDebugInterface"));
	if (DxgiGetDebugInterface == nullptr)
	{
		throw W2WND_LAST_EXCEPT();
	}

	HRESULT hr;
	RENDER_API_NOINFO(DxgiGetDebugInterface(__uuidof(IDXGIInfoQueue), reinterpret_cast<void**>(&m_dxgiInfoQueue)));
}

DxgiInfoManager::~DxgiInfoManager()
{
	if (m_dxgiInfoQueue != nullptr)
	{
		m_dxgiInfoQueue->Release();
		m_dxgiInfoQueue = nullptr;
	}
}

void DxgiInfoManager::Set() noexcept
{
	m_next = m_dxgiInfoQueue->GetNumStoredMessages(DXGI_DEBUG_ALL);
}

std::vector<std::string> DxgiInfoManager::GetMessage() const
{
	std::vector<std::string> messages;
	
	for (auto i = m_next; i < m_dxgiInfoQueue->GetNumStoredMessages(DXGI_DEBUG_ALL); i++)
	{
		HRESULT hr;
		SIZE_T messageLength;
		RENDER_API_NOINFO(m_dxgiInfoQueue->GetMessage(DXGI_DEBUG_ALL, i, nullptr, &messageLength));

		auto bytes = std::make_unique<BYTE[]>(messageLength);
		const auto message = reinterpret_cast<DXGI_INFO_QUEUE_MESSAGE*>(bytes.get());

		RENDER_API_NOINFO(m_dxgiInfoQueue->GetMessage(DXGI_DEBUG_ALL, i, message, &messageLength));

		messages.emplace_back(message->pDescription);
	}
	return messages;
}
