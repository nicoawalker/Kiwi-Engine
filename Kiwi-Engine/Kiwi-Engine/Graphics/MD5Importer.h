#ifndef _KIWI_MD5IMPORTER_H_
#define _KIWI_MD5IMPORTER_H_

#include "../Graphics/MD5Model.h"

#include <string>
#include <memory>
#include <vector>

namespace Kiwi
{

	struct MD5Data
	{
		std::vector<Kiwi::MD5Model::Joint> joints;
		std::vector<Kiwi::MD5Model::JointWeight> weights;
		std::vector<Kiwi::MD5Model::MD5Subset> subsets;
		std::vector<Kiwi::Vertex> vertices;
		std::vector<unsigned long> indices;
	};

	class MD5Importer
	{
	protected:

		bool _CheckStream( std::wifstream& stream, std::wstring& error );

		//load the header information from the md5 model
		std::wstring _ReadMD5Header( std::wifstream& fileStream, std::wstring& commandline, int& md5Version, int& numJoints, int& numMeshes );

		std::wstring _ReadMD5Joints( std::wifstream& fileStream, std::vector<Kiwi::MD5Model::Joint>& loadedJoints, int numJoints );
		std::wstring _ReadMD5Subsets( std::wifstream& fileStream, std::vector<Kiwi::MD5Model::JointWeight>& loadedWeights, std::vector<Kiwi::MD5Model::MD5Subset>& loadedSubsets,
									  std::vector<Kiwi::Vertex>& loadedVertices, std::vector<unsigned long>& loadedIndices );

	public:

		MD5Importer() {}
		virtual ~MD5Importer() {}

		std::wstring Import( MD5Data& _md5data, const std::wstring& md5File );

	};

}

#endif