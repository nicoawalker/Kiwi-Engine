#ifndef _KIWI_LOGGER_H_
#define _KIWI_LOGGER_H_

#include <string>
#include <Windows.h>
#include <sstream>
#include <mutex>
#include <thread>
#include <deque>

namespace Kiwi
{

	class Logger
	{
	private:

		std::wstring m_filename;

		//array of strings waiting to be written
		//this is shared with the output thread and locked using m_outputStrings_mutex
		std::deque<std::wstring> m_outputStrings;
		std::mutex m_outputStrings_mutex;

		//thread used to write the output
		std::thread m_outputThread;

		//when true the thread will end after it has finished writing its output
		bool m_shutdownThread;

	private:

		void _OutputThread(std::wstring outputFile);

	public:

		Logger();
		~Logger();

		void Initialize(std::wstring logFile);

		void Shutdown();

		template<typename T> void Log(const T& output);

	};

	template<typename T> 
	void Logger::Log(const T& output)
	{

		std::lock_guard<std::mutex> guard(m_outputStrings_mutex);

		std::wstring msg;
		std::wstringstream wstr;
		wstr << output;
		msg = wstr.str();

		msg += L"\n";

		if(msg.size() > 0)
		{
			m_outputStrings.push_back(msg);
		}

	}

	extern Kiwi::Logger _Logger;

};

#endif