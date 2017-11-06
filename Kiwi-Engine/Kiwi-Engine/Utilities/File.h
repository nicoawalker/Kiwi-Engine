#ifndef _KIWI_FILE_H_
#define _KIWI_FILE_H_

#include <string>
#include <fstream>

namespace Kiwi
{

	class File
	{
	public:

		enum IO_TYPE { READ_WRITE, READ_ONLY };

	protected:

		std::wstring m_filename;

		//read only file stream
		std::wifstream m_fsReadOnly;

		//read/write file stream
		std::wofstream m_fsReadWrite;

		bool m_isOpen;

	public:

		File( std::wstring filename );
		~File();

		bool Open( IO_TYPE ioType );
		bool Close();

		void SetFilename( std::wstring filename );

		/*returns the current character*/
		char GetChar();

		/*returns the next 'numChars' characters, or until EOF is reached*/
		std::wstring GetNext( unsigned int numChars );

		/*returns the entire current line*/
		std::wstring Getline();

		/*returns the specified line, or the last line if the specified line exceeds the number of lines in the file*/
		std::wstring Getline( unsigned int line );

		/*returns the number of lines in the file*/
		unsigned int GetLineCount();

		/*returns the number of characters in the file*/
		unsigned int GetCharacterCount();

		/*sets the position in the file that will be read from*/
		void SeekTo( unsigned int position );

		/*sets the position to the first character in the specified line*/
		void SeekToLine( unsigned int line );

		std::wstring GetFilename()const { return m_filename; }
		bool IsOpen()const { return m_isOpen; }

	};
}

#endif