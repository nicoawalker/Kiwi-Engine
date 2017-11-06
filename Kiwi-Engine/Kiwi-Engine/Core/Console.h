#ifndef _KIWI_CONSOLE_H_
#define _KIWI_CONSOLE_H_

#include "Exception.h"
#include "Utilities.h"

#include "../Graphics/Color.h"

#include <string>
#include <vector>
#include <mutex>

namespace Kiwi
{

	class EngineRoot;

	enum ConsoleMessageType { Debug, Error, Warning, Message };
	struct ConsoleMessage
	{
		std::wstring message;
		ConsoleMessageType msgType;

		ConsoleMessage( ConsoleMessageType msgType, const std::wstring& message )
		{
			this->msgType = msgType;
			this->message = message;
		}

		ConsoleMessage( ConsoleMessageType msgType, const Kiwi::Exception& exception )
		{
			this->msgType = msgType;
			this->message = Kiwi::ToWString( L"[" + exception.GetSource() + L"] " + exception.GetError() + L"\n" );
		}
	};

	class Console
	{
	public:

		std::vector<ConsoleMessage> m_consoleMessages;

		std::mutex m_consoleMessagesMutex;

	protected:

		virtual void _Print( const ConsoleMessage& msg );

	public:

		Console();
		virtual ~Console();

		virtual void Shutdown();

		/*prints a message to the console*/
		virtual void Print( const ConsoleMessage& msg );

		unsigned int GetMessageCount()const { return (unsigned int)m_consoleMessages.size(); }
		ConsoleMessage GetMessageByIndex( unsigned int messageIndex );

	};

}

#endif