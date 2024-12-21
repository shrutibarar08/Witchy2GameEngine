#include "W2Exception.h"
#include <sstream>


W2Exception::W2Exception(int line, const char* file) noexcept
	: m_line(line), m_file(file)
{}

const char* W2Exception::what() const noexcept
{
	std::ostringstream oss;
	oss << GetType() << std::endl
		<< GetOriginString();

	m_whatBuffer = oss.str();
	return m_whatBuffer.c_str();
}

const char* W2Exception::GetType() const noexcept
{
	return "Witchy Exception";
}

int W2Exception::GetLine() const noexcept
{
	return m_line;
}

const std::string& W2Exception::GetFile() const noexcept
{
	return m_file;
}

std::string W2Exception::GetOriginString() const noexcept
{
	std::ostringstream oss;
	oss << "[File] " << m_file << std::endl
		<< "[Line] " << m_line;
	return oss.str();
}
