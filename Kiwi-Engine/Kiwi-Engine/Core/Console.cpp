#include "Console.h"
#include "EngineRoot.h"

namespace Kiwi
{

	Console::Console()
	{
	}

	Console::~Console()
	{

	}

	void Console::_Print( const ConsoleMessage& msg )
	{
		switch( msg.msgType )
		{
			case ConsoleMessageType::Error:
				{
					/*output as debug message*/
					KIWIDEBUGSTRING( msg.message.c_str() );
					m_consoleMessages.push_back( msg );
					break;
				}
			case ConsoleMessageType::Warning:
				{
					/*output as debug message*/
					KIWIDEBUGSTRING( msg.message.c_str() );
					m_consoleMessages.push_back( msg );
					break;
				}
			case ConsoleMessageType::Message:
				{
					m_consoleMessages.push_back( msg );
					break;
				}
			case ConsoleMessageType::Debug:
				{
					/*output as debug message*/
					KIWIDEBUGSTRING( msg.message.c_str() );
					m_consoleMessages.push_back( msg );
					break;
				}
			default: return;
		}
	}

	void Console::Shutdown()
	{

	}

	void Console::Print( const ConsoleMessage& msg )
	{

		std::lock_guard<std::mutex> guard( m_consoleMessagesMutex );

		this->_Print( msg );

	}

	ConsoleMessage Console::GetMessageByIndex( unsigned int messageIndex )
	{
		return (messageIndex < m_consoleMessages.size()) ? m_consoleMessages[messageIndex] : throw Kiwi::Exception( L"Console::GetMessage", L"Invalid message index", KIWI_EXCEPTION::INVALIDINDEX );
	}

}