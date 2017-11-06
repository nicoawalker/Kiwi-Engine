#include "File.h"

namespace Kiwi
{

	File::File( std::wstring filename )
	{

		m_filename = filename;
		m_isOpen = false;

	}

	File::~File()
	{

	}

	bool File::Open( IO_TYPE ioType )
	{

		if( m_isOpen )
		{
			return true;
		}

		switch( ioType )
		{
			case READ_WRITE:
				{

					break;
				}
			case READ_ONLY:
				{
					m_fsReadOnly.open( m_filename );
					if( m_fsReadOnly.fail() )
					{
						m_isOpen = false;
						return false;
					}
					break;
				}
			default: return false;
		}

		return true;

	}

	bool File::Close()
	{

		if( !m_isOpen )
		{
			return true;
		}

		if( m_fsReadOnly.is_open() )
		{
			m_fsReadOnly.close();

		} else if( m_fsReadWrite.is_open() )
		{
			m_fsReadWrite.close();
		}

		m_isOpen = false;

		return true;

	}

	void File::SetFilename( std::wstring filename ) 
	{
	}

	/*returns the current character*/
	char File::GetChar()
	{

		return ' ';

	}

	/*returns the next 'numChars' characters, or until EOF is reached*/
	std::wstring File::GetNext( unsigned int numChars )
	{

		return L"";

	}

	/*returns the entire current line*/
	std::wstring File::Getline()
	{

		return L"";

	}

	/*returns the specified line, or the last line if the specified line exceeds the number of lines in the file*/
	std::wstring File::Getline( unsigned int line )
	{

		return L"";

	}

	/*returns the number of lines in the file*/
	unsigned int File::GetLineCount()
	{

		return 0;

	}

	/*returns the number of characters in the file*/
	unsigned int File::GetCharacterCount()
	{

		return 0;

	}

	/*sets the position in the file that will be read from*/
	void File::SeekTo( unsigned int position )
	{



	}

	/*sets the position to the first character in the specified line*/
	void File::SeekToLine( unsigned int line )
	{



	}

}