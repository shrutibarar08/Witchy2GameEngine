#pragma once
#include <queue>
#include <Windows.h>
#include <optional>


class W2Mouse
{
	friend class W2WindowAPI;
public:
	class Event
	{
	public:
		enum class Type
		{
			LPress,
			LRelease,
			RPress,
			RRelease,
			WheelUp,
			WheelDown,
			Move,
			Enter,
			Leave,
			Invalid
		};
	private:
		Type m_type;
		bool m_leftIsPressed;
		bool m_rightIsPressed;
		int m_x;
		int m_y;
	public:

		Event() noexcept
			: m_type(Type::Invalid),
			m_leftIsPressed(false),
			m_rightIsPressed(false),
			m_x(0),
			m_y(0)
		{}

		Event(Type type, const W2Mouse& parent) noexcept
			: m_type(type),
			m_leftIsPressed(parent.m_leftIsPressed),
			m_rightIsPressed(parent.m_rightIsPressed),
			m_x(parent.m_x),
			m_y(parent.m_y)
		{}

		bool IsValid() const noexcept
		{
			return m_type != Type::Invalid;
		}

		Type GetType() const noexcept
		{
			return m_type;
		}

		std::pair<int, int> GetPos() const noexcept
		{
			return { m_x, m_y };
		}

		int GetPosX() const noexcept
		{
			return m_x;
		}

		int GetPosY() const noexcept
		{
			return m_y;
		}

		bool LeftIsPressed() const noexcept
		{
			return m_leftIsPressed;
		}

		bool RightIsPressed() const noexcept
		{
			return m_rightIsPressed;
		}
	};

	struct MOUSE_DELTA_DESC
	{
		int x;
		int y;
	};
public:
	W2Mouse() = default;
	W2Mouse(const W2Mouse&) = delete;
	W2Mouse& operator=(const W2Mouse&) = delete;

	//~ Mouse Events
	bool IsLeftPressed() const noexcept;
	bool IsRightPressed() const noexcept;

	std::pair<int, int> GetPos() const noexcept;
	int GetPosX() const noexcept;
	int GetPosY() const noexcept;
	std::optional<W2Mouse::MOUSE_DELTA_DESC> ReadMouseDelta() noexcept;

	bool IsInWindow() const noexcept;

	W2Mouse::Event Read() noexcept;
	bool IsEmpty() const noexcept;
	void Flush() noexcept;

private:
	void ShowCursor() noexcept;
	void HideCursor() noexcept;
	void ConfineCursor(HWND hWnd) noexcept;
	void FreeCursor() noexcept;

	void EnableMouseDelta() noexcept;
	void DisableMouseDelta() noexcept;
	bool IsMouseDelta() const noexcept;

	void SetActive(HWND hWnd) noexcept;
	void SetInactive() noexcept;

	void OnMouseDelta(int dx, int dy) noexcept;

	void OnMouseMove(int x, int y) noexcept;
	void OnMouseLeave() noexcept;
	void OnMouseEnter() noexcept;
	void OnLeftPressed(int x, int y) noexcept;
	void OnLeftReleased(int x, int y) noexcept;
	void OnRightPressed(int x, int y) noexcept;
	void OnRightReleased(int x, int y) noexcept;
	void OnWheelUp(int x, int y) noexcept;
	void OnWheelDown(int x, int y) noexcept;
	void OnWheelDelta(int x, int y, int delta) noexcept;
	void TrimBuffer() noexcept;
	void TrimMouseDeltaBuffer() noexcept;

private:
	static constexpr unsigned int m_bufferSize{ 16u };
	int m_x{ 0 };
	int m_y{ 0 };
	int m_wheelDeltaCarry{ 0 };
	bool m_leftIsPressed { false };
	bool m_rightIsPressed{ false };
	bool m_isInWindow	 { true };
	std::queue<Event> m_buffer;
	std::queue<MOUSE_DELTA_DESC> m_mouseDeltaQueue;
	bool m_bMouseDelta{ false };
};
