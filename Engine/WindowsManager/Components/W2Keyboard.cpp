#include "W2Keyboard.h"


bool W2Keyboard::KeyIsPressed(unsigned char keycode) const noexcept
{
	return m_keyStates[keycode];
}

bool W2Keyboard::KeyBufferIsEmpty() const noexcept
{
	return m_keyBuffer.empty();
}

bool W2Keyboard::CharBufferIsEmpty() const noexcept
{
	return m_charBuffer.empty();
}

W2Keyboard::Event W2Keyboard::ReadKey() noexcept
{
	if (!m_keyBuffer.empty())
	{
		W2Keyboard::Event e = m_keyBuffer.front();
		m_keyBuffer.pop();
		return e;
	}
	return W2Keyboard::Event();
}

unsigned char W2Keyboard::ReadChar() noexcept
{
	if (!m_charBuffer.empty())
	{
		unsigned char e = m_charBuffer.front();
		m_charBuffer.pop();
		return e;
	}
	return 0;
}

void W2Keyboard::FlushKey() noexcept
{
	m_keyBuffer = std::queue<Event>();
}

void W2Keyboard::FlushChar() noexcept
{
	m_charBuffer = std::queue<char>();
}

void W2Keyboard::Flush() noexcept
{
	FlushKey();
	FlushChar();
}

void W2Keyboard::EnableAutoRepeat() noexcept
{
	m_autoRepeatEnabled = true;
}

void W2Keyboard::DisableAutoRepeat() noexcept
{
	m_autoRepeatEnabled = false;
}

bool W2Keyboard::AutoRepeatIsEnabled() const noexcept
{
	return m_autoRepeatEnabled;
}

void W2Keyboard::OnKeyPressed(unsigned char keycode) noexcept
{
	m_keyStates[keycode] = true;
	m_keyBuffer.push(W2Keyboard::Event(W2Keyboard::Event::Type::Press, keycode));
	TrimBuffer(m_keyBuffer);
}

void W2Keyboard::OnKeyReleased(unsigned char keycode) noexcept
{
	m_keyStates[keycode] = false;
	m_keyBuffer.push(W2Keyboard::Event(W2Keyboard::Event::Type::Release, keycode));
	TrimBuffer(m_keyBuffer);
}

void W2Keyboard::OnChar(char character) noexcept
{
	m_charBuffer.push(character);
	TrimBuffer(m_charBuffer);
}

void W2Keyboard::ClearState() noexcept
{
	m_keyStates.reset();
}
