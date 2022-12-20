#pragma once

#include "Core/Core.h"

#include <iostream>
#include <sstream>
#include <chrono>
#include <cstdarg>

#define LOG_MAX_SIZE 1024

namespace Cosmos
{
	class API Logger
	{
	public:

		typedef enum Severity
		{
			Trace = 0,
			Info,
			Warn,
			Error,
			Assert
		} Severity;

		// constructor
		Logger();

		// destructor
		~Logger();

		// logs basically anything
		template<class T>
		inline Logger& operator<<(const T& other)
		{
			m_Output << other << std::endl;
			return *this;
		}

	public:

		// outputs a message to os's terminal
		static void ToTerminal(Severity severity, const char* file, int line, const char* msg, ...);

		// outputs a message to a file
		static void ToFile(Severity severity, const char* path, const char* file, int line, const char* msg, ...);

		// translates severity level to readable text
		static const char* SeverityToConstChar(Severity severity);

	private:

		std::ostringstream m_Output;
	};
}

// macros to facilitate using logging
#define LOG_WARN(...)	Cosmos::Logger::ToTerminal(Cosmos::Logger::Severity::Warn, __FILE__, __LINE__, __VA_ARGS__);
#define LOG_ERROR(...)	Cosmos::Logger::ToTerminal(Cosmos::Logger::Severity::Error, __FILE__, __LINE__, __VA_ARGS__);
#define LOG_TO_FILE(x, y, ...) Cosmos::Logger::ToFile(x, y, __FILE__, __LINE__, __VA_ARGS__);
#define ASSERT(x, ...) { if(!(x)) { Cosmos::Logger::ToTerminal(Cosmos::Logger::Severity::Assert, __FILE__, __LINE__, __VA_ARGS__); std::abort();}}

#if defined ENGINE_DEBUG || EDITOR_DEBUG
	#define LOG_TRACE(...)	Cosmos::Logger::ToTerminal(Cosmos::Logger::Severity::Trace, __FILE__, __LINE__, __VA_ARGS__);
	#define LOG_INFO(...)	Cosmos::Logger::ToTerminal(Cosmos::Logger::Severity::Info, __FILE__, __LINE__, __VA_ARGS__);
#else
	#define LOG_TRACE(...)
	#define LOG_INFO(...)
#endif