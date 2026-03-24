#pragma once

#include "Core/Defines.h"
#include <sstream>

namespace Cosmos
{
	class COSMOS_API Logger
	{
	public:

		typedef enum Severity
		{
			Trace = 0,
			Todo,
			Info,
			Warn,
			Error,
			Assert
		} Severity;

	public:

		/// @brief constructor
		Logger() = default;

		/// @brief destructor
		~Logger();

		/// @brief outputs to ostringstream object
		template<class T>
		inline Logger& operator<<(const T& other)
		{
			mOutput << other << std::endl;
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

		std::ostringstream mOutput;
	};

	/// @brief outputs the log message to a file
	#define LOG_TO_FILE(severity, filepath, ...) Logger::ToFile(severity, filepath, __FILE__, __LINE__, __VA_ARGS__);

	/// @brief outputs the log message as an assertion
	#define LOG_ASSERT(x, ...) { if(!(x)) { Logger::ToTerminal(Logger::Severity::Assert, __FILE__, __LINE__, __VA_ARGS__); std::abort();}}

	/// @brief outputs the log message to the terminal
	#define LOG_TO_TERMINAL(severity, ...) Logger::ToTerminal(severity, __FILE__, __LINE__, __VA_ARGS__);
}