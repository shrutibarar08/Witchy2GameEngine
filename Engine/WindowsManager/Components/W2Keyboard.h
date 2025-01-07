#pragma once
#include <queue>
#include <optional>
#include <bitset>


class W2Keyboard
{
	friend class W2WindowAPI;
public:
	class Event
	{
	public:
		enum class Type
		{
			Press,
			Release,
			Invalid
		};
	public:
		Event(): m_type(Type::Invalid), m_code(0u)
		{}

		Event(Type type, unsigned char code) noexcept
			: m_type(type), m_code(code)
		{}
		
		bool IsPress() const noexcept
		{
			return m_type == Type::Press;
		}

		bool IsRelease() const noexcept
		{
			return m_type == Type::Release;
		}

		bool IsValid() const noexcept
		{
			return m_type != Type::Invalid;
		}

		Type GetType() const noexcept
		{
			return m_type;
		}
		
		unsigned char GetCode() const noexcept
		{
			return m_code;
		}

	private:
		Type m_type;
		unsigned char m_code;
	};

public:
	W2Keyboard() = default;
	W2Keyboard(const W2Keyboard&) = delete;
	W2Keyboard& operator=(const W2Keyboard&) = delete;

	bool KeyIsPressed(unsigned char keycode) const noexcept;
	bool KeyBufferIsEmpty() const noexcept;
	bool CharBufferIsEmpty() const noexcept;
	
	std::optional<W2Keyboard::Event> ReadKey() noexcept;
	std::optional<unsigned char> ReadChar() noexcept;
	
	void FlushKey() noexcept;
	void FlushChar() noexcept;
	void Flush() noexcept;

	void EnableAutoRepeat() noexcept;
	void DisableAutoRepeat() noexcept;
	bool AutoRepeatIsEnabled() const noexcept;

private:
	void OnKeyPressed(unsigned char keycode) noexcept;
	void OnKeyReleased(unsigned char keycode) noexcept;
	void OnChar(char character) noexcept;
	void ClearState() noexcept;
	
	template<typename T>
	static void TrimBuffer(std::queue<T>& buffer) noexcept;

private:
	static constexpr unsigned int m_nKeys = 256u;
	static constexpr unsigned int m_bufferSize = 16u;
	bool m_autoRepeatEnabled = false;
	std::bitset<m_nKeys> m_keyStates;
	std::queue<Event> m_keyBuffer;
	std::queue<char> m_charBuffer;
};

template<typename T>
inline void W2Keyboard::TrimBuffer(std::queue<T>& buffer) noexcept
{
	while (buffer.size() > m_bufferSize)
	{
		buffer.pop();
	}
}
