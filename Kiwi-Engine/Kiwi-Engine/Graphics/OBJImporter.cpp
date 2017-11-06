#include "OBJImporter.h"

#include "../Core/Utilities.h"
#include "../Core/Math.h"
#include "../Core/Exception.h"

#include <map>
#include <sstream>
#include <iostream>
#include <fstream>

using namespace std;

namespace Kiwi
{

	struct Vertex
	{
		float v, t, n;
	};

	OBJMeshData* OBJImporter::Import(std::wstring objFile)
	{

		OBJMeshData* mesh = 0;
		std::wstring objPath, matlibFile;
		std::vector<OBJRawData> rawData;
		std::vector<std::wstring> materials;
		//is this a right-hand coordinate system obj file?
		bool rhCoord = false;

		//store the file path of the obj file
		size_t pos = objFile.find_last_of( L"/" );
		if( pos == std::wstring::npos )
		{
			//no '/' found, check for '\' instead
			pos = objFile.find_last_of( L"\\" );
		}
		objPath = objFile.substr(0, pos + 1);		

		std::wifstream inputFile(objFile.c_str());
		if(inputFile)
		{

			OBJRawData* data = new OBJRawData();
			bool loadingFaces = false;

			//read file
			std::wstring currentLine;
			while(getline(inputFile, currentLine))
			{

				if(currentLine.size() <= 1) continue; //skip empty lines

				//trim spaces before lines
				while(currentLine[0] == ' ')
				{
					currentLine = currentLine.substr(1, std::wstring::npos);
				}

				if(currentLine.size() <= 1) continue; //skip empty lines

				//get first word in the line, store in lineType
				std::wstringstream ss(currentLine);
				std::wstring lineType;
				ss >> lineType;

				//check what type of data this line contains, and extract it
				switch(lineType[0])
				{
				case '#': //comment line, skip
					{
						break;
					}
				case 'v': //vertex
					{
						
						if(loadingFaces && data != 0)
						{
							//loading a group ends once the faces are loaded, so store that data and
							//create a new data structure for the next group of vertices/faces
							rawData.push_back(*data);
							SAFE_DELETE(data);
							data = new OBJRawData();
							loadingFaces = false;
						}

						//check what kind of vertex value this is

						if(lineType.compare(L"v") == 0)
						{

							//read the vertex values
							float x, y, z;
							ss >> x >> y >> z;

							if(rhCoord)
							{//flip z for right hand coordinate system
								z *= -1.0f;
							}

							data->vertices.push_back(Kiwi::Vector3(x,y,z));

						}else if(lineType.compare(L"vn") == 0)
						{

							float x, y, z;
							ss >> x >> y >> z;

							if(rhCoord)
							{//flip z for right hand coordinate system
								z *= -1.0f;
							}

							data->normals.push_back(Kiwi::Vector3(x, y, z));

						}else if(lineType.compare(L"vt") == 0)
						{

							float u, v, w;
							ss >> u >> v >> w;

							if(!rhCoord)
							{//flip v for right hand coordinate system
								v = 1.0f - v;
							}

							data->texCoords.push_back(Kiwi::Vector3(u, v, w));
						}

						break;
					}
				case 'f': //face
					{

						loadingFaces = true;

						std::vector<Vertex*> verts; //vertices
						
						//read in each vertex in the face one by one, and create vertices for each
						std::wstring vertStr;
						while(ss >> vertStr)
						{

							wchar_t temp;
							std::wstringstream vertStream(vertStr);


							if(vertStr.find_first_of(L"/") == std::string::npos)
							{//no '/' found in this vertex, face is in the form: f x y z

								Vertex* v = new Vertex;
								vertStream >> v->v;
								v->n = 0.0f;
								v->t = 0.0f;
									
								verts.push_back(v);

							}else if(vertStr.find_first_of(L"/") == vertStr.find_last_of(L"/"))
							{//only 1 '/' exists, face is in the form: f x/t y/t z/t

								Vertex* v = new Vertex;
								vertStream >> v->v >> temp >> v->t;
								v->n = 0.0f;

								verts.push_back(v);

							}else
							{

								int first = (int)vertStr.find_first_of(L"/");
								std::wstring sub = vertStr.substr(first, vertStr.find_last_of(L"/")-first);

								if(sub == L"//")
								{//face in the form: f x//n y//n z//n (no texture coords)

									Vertex* v = new Vertex;
									vertStream >> v->v >> temp >> temp >> v->n;
									v->t = 0.0f;
									
									verts.push_back(v);

								}else
								{//face in the form: f x/t/n y/t/n z/t/n

									Vertex* v = new Vertex;
									vertStream >> v->v >> temp >> v->t >> temp >> v->n;
									
									verts.push_back(v);

								}
							}

						}

						if(verts.size() < 3) break; //cant make a face out of less than 2 vertices

						Vertex* lastVert = 0;

						for(unsigned int i = 0; i < verts.size();)
						{

							OBJFace face;

							if(lastVert == 0)
							{//havent created any faces yet, create the first one

								face.v1 = verts[0]->v;
								face.t1 = verts[0]->t;
								face.n1 = verts[0]->n;
								face.v2 = verts[1]->v;
								face.t2 = verts[1]->t;
								face.n2 = verts[1]->n;
								face.v3 = verts[2]->v;
								face.t3 = verts[2]->t;
								face.n3 = verts[2]->n;

								data->faces.push_back(face);
								lastVert = verts[2];
								i += 3;
								
							}else
							{
								/*created a face already. all new faces consist of the first vertex in the list,
								the last vertex of the last-created face, and the next new vertex in the list*/ 
								face.v1 = verts[0]->v;
								face.t1 = verts[0]->t;
								face.n1 = verts[0]->n;
								face.v2 = lastVert->v;
								face.t2 = lastVert->t;
								face.n2 = lastVert->n;
								face.v3 = verts[i]->v;
								face.t3 = verts[i]->t;
								face.n3 = verts[i]->n;

								data->faces.push_back(face);
								lastVert = verts[i];
								i++;
							}
						}

						for(unsigned int i = 0; i < verts.size(); i++)
						{
							SAFE_DELETE(verts[i]);
						}

						break;
					}
				case 'm': //mtllib filename
					{
						if(lineType.compare(L"mtllib") == 0)
						{
							int pos = (int)currentLine.find_first_of(' ')+1;
							std::wstring file = currentLine.substr(pos, currentLine.find_first_of('\n')-pos);

							matlibFile = objPath + file;
							materials.push_back( matlibFile );
						}
						break;
					}
				case 'u': //usemtl (which material to use)
					{
						if(lineType.compare(L"usemtl") == 0)
						{
							//create the new material for these vertices
							//this material will be filled from the MTL file later;
							data->mData.hasTexture = false;
							data->mData.reflectivity = 0.0f;
							data->mData.opticalDensity = 0.0f;
							data->mData.illum = 0;
							data->mData.diffuseColor = Kiwi::Color( 1.0f, 1.0f, 1.0f, 1.0f );

							int pos = (int)currentLine.find_first_of(' ')+1;
							data->mData.name = currentLine.substr(pos, currentLine.find_first_of('\n')-pos);

							if( data->mData.name.compare(L" ") == 0)
							{
								data->mData.name = L"";
							}
						}
						break;
					}
				default:break;
				}
			}

			inputFile.close();

			//ended on loading faces, so we need to store the last face into the rawData vector
			if(loadingFaces && data != 0)
			{
				rawData.push_back(*data);
				SAFE_DELETE(data);
				loadingFaces = false;
			}

			//create the mesh object
			mesh = new OBJMeshData();
			mesh->vGroups.resize(rawData.size()); //create a group for each raw data group
			mesh->materialFiles = materials;


			//convert each raw data group into an array of vertices for the mesh
			this->_ConvertFaces(rawData, mesh->vGroups);

			//store the material data
			for(unsigned int i = 0; i < rawData.size(); i++)
			{
				mesh->vGroups[i].mData = rawData[i].mData;
			}

			//mesh is empty
			if(mesh->vGroups.size() == 0) return 0;

			//mesh is now created, and all vertices are loaded
			//now go through the mtllib file and load the data for the materials
			//if(matlibFile != L"")
			//{
			//	inputFile.open(matlibFile.c_str());

			//	if(inputFile)
			//	{

			//		bool diffuseSet = false; //if the diffuse color is not set, use the ambient color
			//		int matIndex = 0; //index of the current material (in obj->materials) to load data for

			//		//pointer to the material that we are currently loading data for
			//		//null until 'newmtl' is encountered
			//		OBJMaterialData* mat = 0;

			//		//read file
			//		std::wstring currentLine;
			//		while(getline(inputFile, currentLine))
			//		{

			//			if(currentLine.size() <= 1) continue; //skip empty lines

			//			while(currentLine[0] == ' ')
			//			{
			//				currentLine = currentLine.substr(1, std::wstring::npos);
			//			}

			//			if(currentLine.size() <= 1) continue; //skip empty lines

			//			//get first word in the line, store in lineType
			//			std::wstringstream ss(currentLine);
			//			std::wstring lineType;
			//			ss >> lineType;

			//			//check what type of data this line contains, and extract it
			//			switch(lineType[0])
			//			{
			//			case '#': //comment, skip
			//				{
			//					break;
			//				}
			//			case 'K':
			//			case 'k': //Ka, Kd, Ks, Ke
			//				{

			//					if(mat == 0) break; //break if there is no material to load data for

			//					if(lineType.compare(L"Ka") == 0)
			//					{ 
			//						//ambient color
			//						ss >> mat->ambientColor.red;
			//						ss >> mat->ambientColor.green;
			//						ss >> mat->ambientColor.blue;

			//						if(!diffuseSet)
			//						{
			//							//also store it as the diffuse color, if there is not yet a diffuse color set
			//							mat->diffuseColor = mat->ambientColor;

			//						}

			//					}else if(lineType.compare(L"Kd") == 0)
			//					{ 
			//						//diffuse color
			//						ss >> mat->diffuseColor.red;
			//						ss >> mat->diffuseColor.green;
			//						ss >> mat->diffuseColor.blue;
			//						mat->diffuseColor.alpha = 1.0f;

			//						diffuseSet = true;

			//					}else if(lineType.compare(L"Ks") == 0)
			//					{ 
			//						//specular color
			//						ss >> mat->specularColor.red;
			//						ss >> mat->specularColor.green;
			//						ss >> mat->specularColor.blue;

			//					}else if(lineType.compare(L"Ke") == 0)
			//					{ 
			//						//emissive color
			//						ss >> mat->emissiveColor.red;
			//						ss >> mat->emissiveColor.green;
			//						ss >> mat->emissiveColor.blue;
			//					}

			//					break;
			//				}
			//			case 'T':
			//			case 't': //Tr, Tf
			//				{

			//					if(mat == 0) break; //break if there is no material to load data for

			//					if(lineType.compare(L"Tr") == 0)
			//					{ 
			//						//transparency (1=transparent, 0=opaque)

			//						float transparency;
			//						ss >> transparency;

			//						if(transparency != 0.0f)
			//						{
			//							//flip the transparency so that 1.0f is opaque
			//							mat->diffuseColor.alpha = 1.0f - transparency;
			//						}

			//					}else if(lineType.compare(L"Tf") == 0)
			//					{ 
			//						//transmission filter (light passing through is filtered by this amount)
			//						//Tf r g b
			//						ss >> mat->transmissionFilter.red;
			//						ss >> mat->transmissionFilter.green;
			//						ss >> mat->transmissionFilter.blue;
			//					}

			//					break;
			//				}
			//			case 'D':
			//			case 'd': //d
			//				{ 
			//					//transparency (0=transparent, 1=opaque)(opposite of Tr)

			//					if(mat == 0) break; //break if there is no material to load data for

			//					if(lineType.size() > 1) break;

			//					float transparency;
			//					ss >> transparency;

			//					if(transparency != 1.0f)
			//					{
			//						mat->diffuseColor.alpha = transparency;
			//					}

			//					break;
			//				}
			//			case 'B':
			//			case 'b':
			//			case 'M': //texture maps
			//			case 'm': //map_Ka, map_Kd, map_Ks map_bump, bump
			//				{

			//					if(mat == 0) break; //break if there is no material to load data for

			//					if(lineType.compare(L"map_Ka") == 0) //ambient map texture
			//					{

			//						int pos = (int)currentLine.find_first_of(' ')+1;
			//						std::wstring map = currentLine.substr(pos, currentLine.find_first_of('\n')-pos);
			//						
			//						//add the filename to the objPath to get the full file path
			//						mat->ambientMap = objPath + map;

			//					}else if(lineType.compare(L"map_Kd") == 0) //diffuse map texture
			//					{

			//						int pos = (int)currentLine.find_first_of(' ')+1;
			//						std::wstring map = currentLine.substr(pos, currentLine.find_first_of('\n')-pos);

			//						//add the filename to the objPath to get the full file path
			//						mat->diffuseMap = objPath + map;
			//						mat->hasTexture = true;

			//					}else if(lineType.compare(L"map_Ks") == 0) //specular map texture
			//					{

			//						int pos = (int)currentLine.find_first_of(' ')+1;
			//						std::wstring map = currentLine.substr(pos, currentLine.find_first_of('\n')-pos);

			//						//add the filename to the objPath to get the full file path
			//						mat->specularMap = objPath + map;

			//					}else if(lineType.compare(L"map_bump") == 0 || lineType.compare(L"bump") == 0) //bump map texture
			//					{

			//						int pos = (int)currentLine.find_first_of(' ')+1;
			//						std::wstring map = currentLine.substr(pos, currentLine.find_first_of('\n')-pos);

			//						//add the filename to the objPath to get the full file path
			//						mat->bumpMap = objPath + map;

			//					}

			//					break;
			//				}
			//			case 'N':
			//			case 'n': //Ns, Ni, newmtl
			//				{
			//					if(lineType.compare(L"newmtl") == 0)
			//					{

			//						//get the name of the material
			//						std::wstring mtlName;
			//						int pos = (int)currentLine.find_first_of(' ')+1;
			//						if(pos < currentLine.size())
			//						{
			//							mtlName = currentLine.substr(pos, currentLine.find_first_of('\n')-pos);
			//						}else
			//						{
			//							mtlName = L"Unnamed";
			//						}
			//						//try to find the material in the mesh data
			//						for(unsigned int i = 0; i < mesh->vGroups.size(); i++)
			//						{
			//							if(mesh->vGroups[i].mData.name.compare(mtlName) == 0)
			//							{
			//								//found the material, now set the pointer to this material
			//								mat = &mesh->vGroups[i].mData;
			//							}
			//						}

			//					}else if(mat != 0)
			//					{
			//						
			//						if(lineType.compare(L"Ns") == 0 && matIndex != -1)
			//						{
			//							//specular power, 0-1000 (reflectivity)
			//							ss >> mat->reflectivity;

			//						}else if(lineType.compare(L"Ni") == 0 && matIndex != -1)
			//						{
			//							//optical density, 0-10 (used to bend light)
			//							ss >> mat->opticalDensity;
			//						}
			//					}

			//					break;
			//				}
			//			case 'I':
			//			case 'i': //illumination
			//				{

			//					if(mat == 0) break; //break if there is no material to load data for

			//					if(lineType.compare(L"illum") == 0)
			//					{
			//						//illumination model, 0-10
			//						ss >> mat->illum;
			//					}

			//					break;
			//				}
			//			default:break;
			//			}
			//		}

			//	}else
			//	{
			//		inputFile.close();

			//		throw Kiwi::Exception( L"OBJImporter::Import", L"Failed to open MTL file: " + matlibFile );
			//	}

			//	inputFile.close();
			//}else
			//{
			//	//Logger.Log(L"No MTLLIB for: "+objFile);
			//}

		}else
		{
			throw Kiwi::Exception( L"OBJImporter::Import", L"Failed to open OBJ file: " + objFile );
		}

		return mesh;

	}

	void OBJImporter::_ConvertFaces(std::vector<OBJRawData>& faceData, std::vector<OBJVertexGroup>& vertexGroups)
	{

		std::vector<Kiwi::Vector3> vertices;
		std::vector<Kiwi::Vector3> normals;
		std::vector<Kiwi::Vector3> texCoords;

		for(unsigned int i = 0; i < faceData.size(); i++)
		{
			vertices.insert(vertices.end(), faceData[i].vertices.begin(), faceData[i].vertices.end());
			normals.insert(normals.end(), faceData[i].normals.begin(), faceData[i].normals.end());
			texCoords.insert(texCoords.end(), faceData[i].texCoords.begin(), faceData[i].texCoords.end());
		}

		if(vertexGroups.size() != faceData.size())
		{
			vertexGroups.resize(faceData.size());
		}

		//create the vertices from the faces
		for(unsigned int a = 0; a < faceData.size(); a++)
		{
			for(unsigned int i = 0; i < faceData[a].faces.size(); i++)
			{

				OBJVertex v1, v2, v3;

				int vIndex = (int)faceData[a].faces[i].v1 - 1;
				int tIndex = (int)faceData[a].faces[i].t1 - 1;
				int nIndex = (int)faceData[a].faces[i].n1 - 1;

				if(vIndex >= 0)
				{
					//set the vertices
					v1.position = vertices[vIndex];
				}

				if(tIndex >= 0)
				{
					//set the texture UV
					v1.textureUV.x = texCoords[tIndex].x;
					v1.textureUV.y = texCoords[tIndex].y;
				}

				if(nIndex >= 0)
				{
					//set the normals
					v1.normals = normals[nIndex];
				}

				vIndex = (int)faceData[a].faces[i].v2 - 1;
				tIndex = (int)faceData[a].faces[i].t2 - 1;
				nIndex = (int)faceData[a].faces[i].n2 - 1;

				if(vIndex >= 0)
				{
					v2.position = vertices[vIndex];
				}

				if(tIndex >= 0)
				{
					v2.textureUV.x = texCoords[tIndex].x;
					v2.textureUV.y = texCoords[tIndex].y;
				}

				if(nIndex >= 0)
				{
					v2.normals = normals[nIndex];
				}

				vIndex = (int)faceData[a].faces[i].v3 - 1;
				tIndex = (int)faceData[a].faces[i].t3 - 1;
				nIndex = (int)faceData[a].faces[i].n3 - 1;

				if(vIndex >= 0)
				{
					v3.position = vertices[vIndex];
				}

				if(tIndex >= 0)
				{
					v3.textureUV.x = texCoords[tIndex].x;
					v3.textureUV.y = texCoords[tIndex].y;
				}

				if(nIndex >= 0)
				{
					v3.normals = normals[nIndex];
				}

				vertexGroups[a].vertices.push_back(v1);
				vertexGroups[a].vertices.push_back(v2);
				vertexGroups[a].vertices.push_back(v3);
			}
		}

	}

};