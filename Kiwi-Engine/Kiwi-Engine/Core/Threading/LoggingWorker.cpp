#include "LoggingWorker.h"

#include "../Utilities.h"

#include "..\..\Types.h"

#include <ctime>

namespace Kiwi
{

	LoggingWorker::LoggingWorker( const std::wstring logFile ):
		Kiwi::IAsyncWorker( L"KiwiLoggingThread" )
	{
		m_logFile = logFile;
	}

	void LoggingWorker::Start()
	{
		m_logStream.open( m_logFile.c_str(), std::ios::app | std::ios::out );
		if( m_logStream.good() )
		{
			time_t curTime = time( 0 );
			std::tm* now = std::localtime( &curTime );

			m_logStream << L"Logging Started @ " + Kiwi::ToWString( now->tm_hour ) + L":" + Kiwi::ToWString( now->tm_min ) + L":" + Kiwi::ToWString( now->tm_sec ) +
							L", " + Kiwi::ToWString( now->tm_mday ) + L"/" + Kiwi::ToWString( now->tm_mon + 1 ) + L"/" + Kiwi::ToWString( now->tm_year + 1900 ) + L"\n\n";

			m_logStream.flush();

		} else
		{
			throw Kiwi::Exception( L"LoggingWorker::Start", L"Failed to open log file \"" + m_logFile + L"\" for writing", KIWI_EXCEPTION::FILEOPENFAILURE );
		}
	}

	void LoggingWorker::Run()
	{
		if( m_logStream.good() )
		{
			std::lock_guard<std::mutex> lock( m_logger_M );
			if( m_outputQueue.size() > 0 )
			{
				m_logStream << m_outputQueue.front();
				m_logStream.flush();
				m_outputQueue.pop_front();

			} else
			{
				std::this_thread::sleep_for( std::chrono::milliseconds( 5 ) );
			}

		} else
		{
			m_logStream.open( m_logFile.c_str(), std::ios::app | std::ios::out );
			if( m_logStream.good() )
			{
				time_t curTime = time( 0 );
				std::tm* now = std::localtime( &curTime );

				m_logStream << L"Logging re-started @ " + Kiwi::ToWString( now->tm_hour ) + L":" + Kiwi::ToWString( now->tm_min ) + L":" + Kiwi::ToWString( now->tm_sec ) +
								L", " + Kiwi::ToWString( now->tm_mday ) + L"/" + Kiwi::ToWString( now->tm_mon + 1 ) + L"/" + Kiwi::ToWString( now->tm_year + 1900 ) + L"\n\n";
				m_logStream.flush();

			} else
			{/*couldn't re-open the file so just quit*/
				m_ready = true;
				throw Kiwi::Exception( L"LoggingWorker::Run", L"Logging worker lost the log file \"" + m_logFile + L"\" and couldn't re-open it", KIWI_EXCEPTION::FILEOPENFAILURE );
			}
		}
	}

	void LoggingWorker::Finish()
	{
		if( m_logStream.good() )
		{
			std::lock_guard<std::mutex> lock( m_logger_M );
			while( m_outputQueue.size() > 0 )
			{
				m_logStream << m_outputQueue.front();
				m_logStream.flush();
				m_outputQueue.pop_front();
			}

			time_t curTime = time( 0 );
			std::tm* now = std::localtime( &curTime );

			m_logStream << L"Logging finished @ " + Kiwi::ToWString( now->tm_hour ) + L":" + Kiwi::ToWString( now->tm_min ) + L":" + Kiwi::ToWString( now->tm_sec ) +
							L", " + Kiwi::ToWString( now->tm_mday ) + L"/" + Kiwi::ToWString( now->tm_mon + 1 ) + L"/" + Kiwi::ToWString( now->tm_year + 1900 ) + L"\n\n\n\n";

			m_logStream.flush();
		}

		m_logStream.close();
	}

	void LoggingWorker::Log( std::wstring output )
	{
		/*add new line character if needed*/
		if( output[output.size() - 1] != '\n' )
		{
			output.append( L"\n" );
		}

		std::lock_guard<std::mutex> lock( m_logger_M );
		if( !m_ready )
		{
			m_outputQueue.push_back( output );
		}
	}

}