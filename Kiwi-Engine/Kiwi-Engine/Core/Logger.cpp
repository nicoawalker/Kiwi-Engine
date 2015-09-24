#include "Logger.h"
#include "Utilities.h"

#include <Windows.h>
#include <iostream>
#include <fstream>

namespace Kiwi
{

	Logger::Logger()
	{
		m_shutdownThread = false;
		m_initialized = false;
	}

	Logger::~Logger()
	{

	}

	void Logger::Initialize(std::wstring logFile)
	{

		//start the logging thread
		m_outputThread = std::thread(&Logger::_OutputThread, this, logFile);

		m_filename = logFile;

		m_initialized = true;

	}

	void Logger::Shutdown()
	{

		//tell the thread to shutdown once it's finished writing
		m_outputStrings_mutex.lock();
			m_shutdownThread = true;
		m_outputStrings_mutex.unlock();

		//wait for the thread to finish, and close it
		m_outputThread.join();

	}

	void Logger::_OutputThread(std::wstring outputFileName)
	{

		std::wofstream outputFile(outputFileName.c_str(), std::ios_base::app | std::ios_base::out);
		if(outputFile.good())
		{
			outputFile << "=============================================\n";
			outputFile << "               Logging Started\n";
			outputFile << "=============================================\n";

			outputFile.close();
		}

		while(true)
		{

			bool shutdown = false;
			unsigned int size = 0;
			std::wstring output = L"";

			m_outputStrings_mutex.lock();

				size = m_outputStrings.size();
				if(size > 0)
				{
					output = m_outputStrings.front();
					m_outputStrings.pop_front();
				}
				shutdown = m_shutdownThread;

			m_outputStrings_mutex.unlock();

			if(size == 0)
			{
				Sleep(1);
				if(shutdown)
				{
					break;
				}else
				{
					continue;
				}
			}else if(output.compare(L"") != 0)
			{
				outputFile.open(outputFileName.c_str(), std::ios_base::app | std::ios_base::out);
				if(outputFile.good())
				{

					outputFile << output;

					outputFile.close();
				}
			}

		}

		outputFile.open(outputFileName.c_str(), std::ios_base::app | std::ios_base::out);
		if(outputFile.good())
		{

			outputFile << "=============================================\n";
			outputFile << "                Logging Ended\n";
			outputFile << "=============================================\n";

			outputFile.close();
		}

	}

};