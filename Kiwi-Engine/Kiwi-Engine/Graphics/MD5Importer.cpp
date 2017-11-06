#include "MD5Importer.h"

#include <sstream>
#include <iostream>
#include <fstream>

namespace Kiwi
{

	bool MD5Importer::_CheckStream( std::wifstream& stream, std::wstring& error )
	{
		if( stream.fail() )
		{
			stream.clear();
			stream >> error;
			return false;
		}

		return true;
	}

	std::wstring MD5Importer::Import( MD5Data& _md5data, const std::wstring& md5File )
	{
		//https://www.braynzarsoft.net/viewtutorial/q16390-27-loading-an-md5-model

		std::wstring errorString;

		std::wifstream inputFile( md5File.c_str() );
		if( inputFile )
		{
			std::wstring curString;
			int numJoints = -1, numMeshes = -1, md5Version = -1;
			std::wstring commandline;

			//attempt to read the header info
			errorString = this->_ReadMD5Header( inputFile, commandline, md5Version, numJoints, numMeshes );
			if( errorString.compare( L"" ) != 0 ) return errorString;

			//attempt to read in the model's joints
			errorString = this->_ReadMD5Joints( inputFile, _md5data.joints, numJoints );
			if( errorString.compare( L"" ) != 0 ) return errorString;

			//attempt to read in the model's subsets
			errorString = this->_ReadMD5Subsets( inputFile, _md5data.weights, _md5data.subsets, _md5data.vertices, _md5data.indices );
			if( errorString.compare( L"" ) != 0 ) return errorString;

			inputFile.close();

		} else
		{
			errorString = L"Failed to open file \"" + md5File + L"\"";
		}

		return errorString;
	}

	std::wstring MD5Importer::_ReadMD5Header( std::wifstream& fileStream, std::wstring& commandline, int& md5Version, int& numJoints, int& numMeshes )
	{
		std::wstring errorString, curString;
		unsigned int finished = 0;

		//reset the file stream
		fileStream.clear();
		fileStream.seekg( 0, std::ios::beg );

		while( fileStream && finished < 4 )
		{
			fileStream >> curString;

			if( curString.compare( L"MD5Version" ) == 0 ) //MD5Version (e.g. MD5Version 10)
			{
				fileStream >> md5Version;
				if( !_CheckStream( fileStream, errorString ) ) break;
				if( md5Version != 10 )
				{
					errorString = L"MD5 version is not 10";
					break;
				}

				finished++;

			} else if( curString.compare( L"commandline" ) == 0 ) //commandline e.g. commandline ""
			{
				std::getline( fileStream, commandline );
				finished++;

			} else if( curString.compare( L"numJoints" ) == 0 ) //numJoints e.g. numJoints 27
			{
				fileStream >> numJoints;
				if( !_CheckStream( fileStream, errorString ) ) break;
				finished++;

			} else if( curString.compare( L"numMeshes" ) == 0 ) //number of submeshes e.g. numMeshes 5
			{
				fileStream >> numMeshes;
				if( !_CheckStream( fileStream, errorString ) ) break;
				finished++;

			} else
			{
				std::getline( fileStream, curString ); //skip
			}
		}

		//reset the file stream
		fileStream.clear();
		fileStream.seekg( 0, std::ios::beg );

		return errorString;
	}

	std::wstring MD5Importer::_ReadMD5Joints( std::wifstream& fileStream, std::vector<Kiwi::MD5Model::Joint>& loadedJoints, int numJoints )
	{
		std::wstring errorString, curString;

		//reset the file stream
		fileStream.clear();
		fileStream.seekg( 0, std::ios::beg );

		while( fileStream )
		{
			fileStream >> curString;

			if( curString.compare( L"joints" ) == 0 ) //joint list e.g. joints { "Bip01" - 1 (0.569962 0.0 - 6.39413) (0.0 0.0 0.707106) ... }
			{
				/*This is the start of the joint descriptions. The joint descriptions start on the next line (after "joints {")
				and go until a line containing a closing bracket ("}") is reached. Each line after "joints {" is a new joint. Each
				of these lines starts with a string inside two quotation marks, which is the name of the joint. Following the name
				of the joint is the ID number of that joint. The joint with the ID number "-1" is the root joint. After the ID number
				is a 3D vector (contained in two parentheses) describing the joints position. After that is another 3D vector (also
				inside parentheses) which describes the joints "bind-pose" orientation. Although the joint orientation is stored as a
				3D vector inside the file, it is actually used as a quaternion (or 4D vector).*/

				Kiwi::MD5Model::Joint newJoint;

				fileStream >> curString; //skip leading {

				for( unsigned int i = 0; i < (unsigned int)numJoints; i++ )
				{
					fileStream >> newJoint.name; //store joint name

												 /*handle joint names containing spaces*/
					if( newJoint.name[newJoint.name.length() - 1] != '"' )
					{
						wchar_t nextChar;
						bool nameEndFound = false;
						while( !nameEndFound )
						{
							nextChar = fileStream.get();
							if( nextChar == '"' ) nameEndFound = true;

							newJoint.name += nextChar;
						}
					}

					fileStream >> newJoint.parentID; //store parent ID
					if( !_CheckStream( fileStream, errorString ) ) break;

					fileStream >> curString; //skip next (
					if( !_CheckStream( fileStream, errorString ) ) break;

					//store position of the joint (assumes model was created using left-hand coordinate system, else swap y and z)
					fileStream >> newJoint.position.x >> newJoint.position.z >> newJoint.position.y;
					if( !_CheckStream( fileStream, errorString ) ) break;

					fileStream >> curString >> curString; //skip ) and (

					fileStream >> newJoint.orientation.x >> newJoint.orientation.z >> newJoint.orientation.y;
					if( !_CheckStream( fileStream, errorString ) ) break;

					//remove quotation marks from joints name
					newJoint.name.erase( 0, 1 );
					newJoint.name.erase( newJoint.name.length() - 1, 1 );

					//compute w axis for quaternion
					double temp = 1.0 - pow( newJoint.orientation.x, 2 ) - pow( newJoint.orientation.y, 2 ) - pow( newJoint.orientation.z, 2 );
					(temp < 0.0) ? newJoint.orientation.w = 0.0 : newJoint.orientation.w = -sqrt( temp );

					//skip rest of current line
					std::getline( fileStream, curString );

					loadedJoints.push_back( newJoint );
				}

				//stop reading if there was an error
				if( errorString.compare( L"" ) != 0 ) break;

			} else
			{
				//skip anything else
				std::getline( fileStream, curString );
			}
		}

		//reset the file stream
		fileStream.clear();
		fileStream.seekg( 0, std::ios::beg );

		return errorString;
	}

	std::wstring MD5Importer::_ReadMD5Subsets( std::wifstream& fileStream, std::vector<Kiwi::MD5Model::JointWeight>& loadedWeights, std::vector<Kiwi::MD5Model::MD5Subset>& loadedSubsets,
												std::vector<Kiwi::Vertex>& loadedVertices, std::vector<unsigned long>& loadedIndices )
	{
		std::wstring errorString, curString;

		//reset the file stream
		fileStream.clear();
		fileStream.seekg( 0, std::ios::beg );

		while( fileStream )
		{
			fileStream >> curString;

			if( curString.compare( L"mesh" ) == 0 ) //start of submesh e.g. mesh {  ...  }
			{
				/*This string is the start of a mesh or subset. Everything between the two opening and closing curly brackets defines
				this specific subset. The "md5mesh" file contains only one section for the joints, but can contain one or more sections
				for each subset or mesh.*/

				Kiwi::MD5Model::MD5Subset md5Subset;

				//temporary storage for the data read in for the subset
				std::vector<Kiwi::MD5Model::JointWeight> tempWeights;
				std::vector<Kiwi::Vertex> tempVertices;
				std::vector<unsigned long> tempIndices;

				//stores whether an error occured while reading the file
				bool readError = false;

				/*set offsets of vertices, indices, and weights to the current end of the respective lists*/
				md5Subset.indexOffset = (unsigned int)loadedIndices.size();
				md5Subset.vertexOffset = (unsigned int)loadedVertices.size();
				md5Subset.weightOffset = (unsigned int)loadedWeights.size();

				fileStream >> curString; //skip {

				fileStream >> curString;
				while( curString.compare( L"}" ) != 0 && readError == false ) //read until } or an error occurs
				{
					if( curString.compare( L"shader" ) == 0 ) //load texture/material
					{


					} else if( curString.compare( L"numverts" ) == 0 ) //number of vertices in the submesh
					{
						fileStream >> md5Subset.numVertices;
						if( !(readError = _CheckStream( fileStream, errorString )) )
						{//break if a read error occurred (next value wasn't a number)
							break;

						} else
						{
							std::getline( fileStream, curString ); //skip rest of line

																   /*read and create the number of vertices specified in 'numverts'*/
							for( unsigned int i = 0; i < md5Subset.numVertices; i++ )
							{
								Kiwi::Vertex newVertex;

								fileStream >> curString >> curString >> curString; //skip "vert # ("

								fileStream >> newVertex.tex.x >> newVertex.tex.y; //store texture coordinates
								if( !(readError = _CheckStream( fileStream, errorString )) ) break; //stop reading if a read error occurred

								fileStream >> curString; //skip )

								fileStream >> newVertex.startWeight; //index of first weight the vertex is weighted to
								if( !(readError = _CheckStream( fileStream, errorString )) ) break; //stop reading if a read error occurred

								fileStream >> newVertex.weightCount; //number of weights for this vertex
								if( !(readError = _CheckStream( fileStream, errorString )) ) break; //stop reading if a read error occurred

								std::getline( fileStream, curString ); //skip rest of line

								tempVertices.push_back( newVertex );
							}
						}

					} else if( curString.compare( L"numtris" ) == 0 ) //number of triangles in the submesh e.g. numtris 99
					{
						/*Here is the number of triangles in the subset. The lines following this line are the indices/triangles that make up this subset.*/

						fileStream >> md5Subset.numTriangles;
						if( !(readError = _CheckStream( fileStream, errorString )) ) break; //stop reading if a read error occurred

						std::getline( fileStream, curString ); //skip rest of the line

															   /*load the submesh's indices*/
						for( unsigned int i = 0; i < md5Subset.numTriangles; i++ )
						{
							unsigned int tempIndex = 0;
							fileStream >> curString >> curString; //skip 'tri' and tri counter

							for( unsigned int a = 0; a < 3; a++ )
							{
								fileStream >> tempIndex;
								if( !(readError = _CheckStream( fileStream, errorString )) ) break; //stop reading if a read error occurred

								tempIndices.push_back( tempIndex );
							}

							std::getline( fileStream, curString ); //skip rest of line
						}

					} else if( curString.compare( L"numweights" ) == 0 )
					{
						fileStream >> md5Subset.numWeights;
						if( !(readError = _CheckStream( fileStream, errorString )) ) break; //stop reading if a read error occurred

						std::getline( fileStream, curString ); //skip rest of line

						for( unsigned int i = 0; i < md5Subset.numWeights; i++ )
						{
							MD5Model::JointWeight weight;

							fileStream >> curString >> curString; //skip 'weight #'

							fileStream >> weight.jointID;
							if( !(readError = _CheckStream( fileStream, errorString )) ) break; //stop reading if a read error occurred

							fileStream >> weight.bias;
							if( !(readError = _CheckStream( fileStream, errorString )) ) break; //stop reading if a read error occurred

							fileStream >> curString; //skip '('

							//position of the weight in the joint's local space
							fileStream >> weight.position.x >> weight.position.y >> weight.position.z;
							if( !(readError = _CheckStream( fileStream, errorString )) ) break; //stop reading if a read error occurred

							std::getline( fileStream, curString ); //skip rest of line

							tempWeights.push_back( weight );
						}

					} else
					{
						std::getline( fileStream, curString ); //skip anything else
					}

					fileStream >> curString; //skip '}'
				}

				if( readError == false )
				{
					//add the new data to the list of loaded data if there were no read errors
					loadedSubsets.push_back( md5Subset );
					loadedIndices.insert( loadedIndices.end(), tempIndices.begin(), tempIndices.end() );
					loadedVertices.insert( loadedVertices.end(), tempVertices.begin(), tempVertices.end() );
					loadedWeights.insert( loadedWeights.end(), tempWeights.begin(), tempWeights.end() );
				}
			} else
			{
				//skip anything else
				std::getline( fileStream, curString );
			}
		}

		//reset the file stream
		fileStream.clear();
		fileStream.seekg( 0, std::ios::beg );

		return errorString;
	}

}