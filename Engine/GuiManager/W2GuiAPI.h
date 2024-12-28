#pragma once
#include <Windows.h>


class W2GuiAPI
{
public:

	W2GuiAPI(const W2GuiAPI&) = delete;
	W2GuiAPI& operator=(const W2GuiAPI&) = delete;
	W2GuiAPI(W2GuiAPI&&) = delete;
	W2GuiAPI operator=(W2GuiAPI&&) = delete;

	static void Init() noexcept;
	static W2GuiAPI* Get();
	void RecordStart();
	void RecordEnd();

	static void Release();

private:
	W2GuiAPI();
	~W2GuiAPI() = default;

	static W2GuiAPI* m_instance;
};
