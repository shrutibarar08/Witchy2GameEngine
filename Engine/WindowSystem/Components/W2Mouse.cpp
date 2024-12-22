#include "W2Mouse.h"
#include <Windows.h>

bool W2Mouse::IsLeftPressed() const noexcept
{
	return m_leftIsPressed;
}

bool W2Mouse::IsRightPressed() const noexcept
{
	return m_rightIsPressed;
}

std::pair<int, int> W2Mouse::GetPos() const noexcept
{
	return { m_x, m_y };
}

int W2Mouse::GetPosX() const noexcept
{
	return m_x;
}

int W2Mouse::GetPosY() const noexcept
{
	return m_y;
}

bool W2Mouse::IsInWindow() const noexcept
{
	return m_isInWindow;
}

W2Mouse::Event W2Mouse::Read() noexcept
{
	if (!m_buffer.empty())
	{
		W2Mouse::Event e = m_buffer.front();
		m_buffer.pop();
		return e;
	}
	return W2Mouse::Event();
}

bool W2Mouse::IsEmpty() const noexcept
{
	return m_buffer.empty();
}

void W2Mouse::Flush() noexcept
{
	m_buffer = std::queue<Event>();
}

void W2Mouse::OnMouseMove(int x, int y) noexcept
{
	m_x = x;
	m_y = y;
	m_buffer.push(W2Mouse::Event(W2Mouse::Event::Type::Move, *this));
	TrimBuffer();
}

void W2Mouse::OnMouseLeave() noexcept
{
	m_isInWindow = false;
	m_buffer.push(W2Mouse::Event(W2Mouse::Event::Type::Leave, *this));
	TrimBuffer();
}

void W2Mouse::OnMouseEnter() noexcept
{
	m_isInWindow = true;
	m_buffer.push(W2Mouse::Event(W2Mouse::Event::Type::Enter, *this));
	TrimBuffer();
}

void W2Mouse::OnLeftPressed(int x, int y) noexcept
{
	m_leftIsPressed = true;
	m_buffer.push(W2Mouse::Event(W2Mouse::Event::Type::LPress, *this));
	TrimBuffer();
}

void W2Mouse::OnLeftReleased(int x, int y) noexcept
{
	m_leftIsPressed = false;
	m_buffer.push(W2Mouse::Event(W2Mouse::Event::Type::LRelease, *this));
	TrimBuffer();
}

void W2Mouse::OnRightPressed(int x, int y) noexcept
{
	m_rightIsPressed = true;
	m_buffer.push(W2Mouse::Event(W2Mouse::Event::Type::RPress, *this));
	TrimBuffer();
}

void W2Mouse::OnRightReleased(int x, int y) noexcept
{
	m_rightIsPressed = false;
	m_buffer.push(W2Mouse::Event(W2Mouse::Event::Type::RRelease, *this));
	TrimBuffer();
}

void W2Mouse::OnWheelUp(int x, int y) noexcept
{
	m_buffer.push(W2Mouse::Event(W2Mouse::Event::Type::WheelUp, *this));
	TrimBuffer();
}

void W2Mouse::OnWheelDown(int x, int y) noexcept
{
	m_buffer.push(W2Mouse::Event(W2Mouse::Event::Type::WheelDown, *this));
	TrimBuffer();
}

void W2Mouse::OnWheelDelta(int x, int y, int delta) noexcept
{
	m_wheelDeltaCarry += delta;
	while (m_wheelDeltaCarry >= WHEEL_DELTA)
	{
		m_wheelDeltaCarry -= WHEEL_DELTA;
		OnWheelUp(x, y);
	}
	while (m_wheelDeltaCarry <= -WHEEL_DELTA)
	{
		m_wheelDeltaCarry += WHEEL_DELTA;
		OnWheelDown(x, y);
	}
}

void W2Mouse::TrimBuffer() noexcept
{
	while (m_buffer.size() > m_bufferSize)
	{
		m_buffer.pop();
	}
}