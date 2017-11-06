#ifndef _KIWI_LOGGINGWORKER_H_
#define _KIWI_LOGGINGWORKER_H_

#include "IAsyncWorker.h"

#include "..\..\Types.h"

#include <string>
#include <deque>
#include <mutex>
#include <fstream>

namespace Kiwi
{

	class LoggingWorker :
		public Kiwi::IAsyncWorker
	{
	protected:

		std::deque<std::wstring> m_outputQueue;

		std::wstring m_logFile;

		std::wofstream m_logStream;

		std::mutex m_logger_M;

	public:

		LoggingWorker( const std::wstring logFile );

		void Start();

		void Run();

		void Finish();

		void Log( std::wstring output );

	};
}

#endif