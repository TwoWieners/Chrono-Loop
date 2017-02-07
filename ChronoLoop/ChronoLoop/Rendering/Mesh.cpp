#include "Mesh.h"
#include <ppltasks.h>
#include <fstream>
#include <string>
#include "DDSTextureLoader.h"
#include "stdafx.h"
#include "renderer.h"

using namespace Concurrency;

Mesh::Mesh()
{
	this->Image = nullptr;
	this->m_node = nullptr;
}

Mesh::Mesh(char * path)
{
	this->Load(path);
	this->Image = nullptr;
}

Mesh::Mesh(char * path, wchar_t * path2)
{
	this->Load(path);
	Image = path2;
}

Mesh::~Mesh()
{
	this->Clear();
}

bool Mesh::Load(char * path)
{
	this->Clear();
	std::vector<AnimatedVert> Verts;
	std::vector<unsigned short> Ind;
	int index = 1;
	std::ifstream file;
	file.open(path, std::ios::in);
	if (!file.is_open())
		return false;
	std::string line;
	std::vector<DirectX::XMFLOAT4> verts;
	std::vector<DirectX::XMFLOAT4> norms;
	std::vector<DirectX::XMFLOAT4> uvs;
	while (std::getline(file, line))
	{
		if (line[0] == 'v') {
			if (line[1] == 't') {
				DirectX::XMFLOAT4 uv;
				sscanf_s(line.c_str(), "vt %f %f\n", &uv.x, &uv.y);
				uv.y = 1 - uv.y;
				uvs.push_back(uv);
			}
			else if (line[1] == 'n') {
				DirectX::XMFLOAT4 normal;
				sscanf_s(line.c_str(), "vn %f %f %f\n", &normal.x, &normal.y, &normal.z);
				norms.push_back(normal);
			}
			else if(line[1] == ' ') {
				DirectX::XMFLOAT4 vertex;
				sscanf_s(line.c_str(), "v %f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
				verts.push_back(vertex);
			}
		}
		else if (line[0] == 'f') {
			std::string vertex1, vertex2, vertex3;
			unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
			int matches = sscanf_s(line.c_str(), "f %d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2]);
			if (matches != 9) {
				return false;
			}
			for (int i = 0; i < 3; i++)
			{
				AnimatedVert temp;
				temp.position = verts[vertexIndex[i] - 1];
				temp.position.w = 1;
				//temp.color = DirectX::XMFLOAT4(i == 0, i == 1, i == 3);
				temp.normal = norms[normalIndex[i] - 1];
				temp.uv = uvs[uvIndex[i] - 1];
				temp.uv.z = 0;
				temp.blendWeights = XMFLOAT4(1.0f, 0, 0, 0);
				temp.boneIndices = XMUINT4(0, 0, 0, 0);
				Ind.push_back((unsigned short)Verts.size());
				Verts.push_back(temp);
			}
		}
	}
	UniqueVerts = Verts;
	Indicies = Ind;
	totalUnique = verts.size();
	totalVerts = Ind.size();
	file.close();
	return true;
}

//bool Mesh::Load(std::vector<Vertex>* vecArray, std::vector<Bone>* boneArray, Animation * anim)
//{
//	this->Clear();
//	m_anim = *anim;
//	for (int i = 0; i < vecArray->size(); i++)
//	{
//		Indicies.push_back(i);
//		DirectX::XMFLOAT4 pos(vecArray->at(i).pos);
//		DirectX::XMFLOAT4 norm(vecArray->at(i).norms);
//		//DirectX::XMFLOAT4 norm(1, 1, 1, 1);
//		DirectX::XMFLOAT4 uv(vecArray->at(i).uvs);
//		AnimatedVert temp;
//		temp.position = pos;
//		temp.normal = norm;
//		temp.uv = uv;
//		float weights[4];
//		u_int indices[4];
//		for (int j = 0; j < 4; j++)
//		{
//			if (j < vecArray->at(i).blendWeights.size())
//			{
//				weights[j] = vecArray->at(i).blendWeights[j];
//				indices[j] = vecArray->at(i).boneIndices[j];
//			}
//			else
//			{
//				weights[j] = 0;
//				indices[j] = 0;
//			}
//		}
//		temp.blendWeights = DirectX::XMFLOAT4(weights);
//		temp.boneIndices = DirectX::XMUINT4(indices);
//		UniqueVerts.push_back(temp);
//	}
//	totalVerts = totalUnique = vecArray->size();
//
//	for (int i = 0; i < boneArray->size(); ++i)
//	{
//		Bones.push_back(&boneArray->at(i));
//	}
//	return true;
//}


bool Mesh::LoadBin(char* path)
{
	LoadFBX(path, false);
	std::ifstream file;
	std::string fileThing(path); 
	size_t lastindex = fileThing.find_last_of(".");
	std::string rawname = fileThing.substr(0, lastindex);
	rawname.append(".bin");
	file.open(rawname.c_str(), std::ios_base::binary);
	if (file.is_open())
	{
		Skeleton skeleton;

		//Number of Verts
		unsigned int numVerts;
		file.read((char*)&numVerts, sizeof(unsigned int));

		//For every Vert
		for (unsigned int i = 0; i < numVerts; ++i)
		{
			AnimatedVert vert;
			//Pos
			float tempP[4];
			file.read((char*)&tempP, sizeof(float[4]));
			vert.position = DirectX::XMFLOAT4(tempP);

			//UVs
			float tempUV[4];
			file.read((char*)&tempUV, sizeof(float[2]));
			vert.uv = DirectX::XMFLOAT4(tempUV);
			vert.uv.z = 0;
			vert.uv.w = 0;

			//Norms
			float tempN[4];
			file.read((char*)&tempN, sizeof(float[4]));
			DirectX::XMFLOAT4 norms(tempN);
			vert.normal = norms;

			//Number of Blend Weights
			unsigned int numWeights;
			file.read((char*)&numWeights, sizeof(unsigned int));

			//For every Blend Weight
			float weights[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
			for (unsigned int b = 0; b < numWeights && b < 4; ++b)
			{
				//Weights
				file.read((char*)&weights[b], sizeof(float));
			}
			vert.blendWeights = XMFLOAT4(weights);
			//Number of Bone Indices
			unsigned int numIndices;
			file.read((char*)&numIndices, sizeof(unsigned int));

			//For every Bone Index
			unsigned int indices[4] = { 0, 0, 0, 0 };
			for (unsigned int b = 0; b < numIndices && b < 4; ++b)
			{
				//Weights
				file.read((char*)&indices[b], sizeof(unsigned int));
			}
			vert.boneIndices = XMUINT4(indices);

			if (FirstRun)
			{
				UniqueVerts.push_back(vert);
				Indicies.push_back(i);
			}
		}
		
		//Number of Bones
		unsigned int numBones;
		file.read((char*)&numBones, sizeof(unsigned int));
		
		//For every Bone
		for (unsigned int i = 0; i < numBones; ++i)
		{
			Bone* bone = new Bone();

			//Bones index
			file.read((char*)&bone->myIndex, sizeof(int));

			//Bone's parent index
			file.read((char*)&bone->parentIndex, sizeof(int));

			//Bind Pos
			file.read((char*)&bone->transform, sizeof(DirectX::XMFLOAT4X4));

			//Number of Keyframes
			unsigned int numKeys;
			file.read((char*)&numKeys, sizeof(unsigned int));
			bone->keyframes = new std::vector<KeyFrame*>();

			//For each Keyframe
			for(unsigned int k = 0; k < numKeys; ++k)
			{
				//Keyframes
				KeyFrame* temp = new KeyFrame();
				file.read((char*)temp, sizeof(KeyFrame));
				bone->keyframes->push_back(temp);
			}
			skeleton.Bones.push_back(bone);
			skeleton.m_anim.bones.push_back(bone);
		}

		//Animation Time
		file.read((char*)&skeleton.m_anim.loopTime, sizeof(float));

		for (unsigned int i = 0; i < numBones; ++i)
		{
			if (skeleton.m_anim.bones.at(i)->parentIndex > -1)
			{
				skeleton.m_anim.bones.at(i)->Parent = skeleton.m_anim.bones.at(skeleton.m_anim.bones.at(i)->parentIndex);
				skeleton.m_anim.bones.at(skeleton.m_anim.bones.at(i)->parentIndex)->children.push_back(skeleton.m_anim.bones.at(i));
			}
			else if (skeleton.m_anim.bones.at(i)->parentIndex == -1)
				skeleton.m_anim.bones.at(i)->Parent = nullptr;
		}

		NumSkeletons++;
		skeletons.push_back(skeleton);
		file.close();
		FirstRun = false;
		return true;
	}

	return false;
}

void Mesh::Clear()
{
	UniqueVerts.clear();
	Indicies.clear();
}

void Mesh::Invert()
{
	for (int i = 0; i < totalVerts / 3; i++)
	{
		int temp = Indicies[(i * 3)];
		Indicies[(i * 3)] = Indicies[(i * 3) + 2];
		Indicies[(i * 3) + 2] = temp;
	}
}

void Mesh::MakePlane()
{
	this->Clear();

	static const AnimatedVert cubeVertices[] =
	{
		{ XMFLOAT4(-50.0f,  0.0f, -50.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT4(0, 0, 0, 0), XMUINT4(0, 0, 0, 0) },
		{ XMFLOAT4(-50.0f,  0.0f,  50.0f, 1.0f), XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT4(0, 0, 0, 0), XMUINT4(0, 0, 0, 0) },
		{ XMFLOAT4(50.0f,  0.0f, -50.0f, 1.0f), XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f),  XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT4(0, 0, 0, 0), XMUINT4(0, 0, 0, 0) },
		{ XMFLOAT4(50.0f,  0.0f,  50.0f, 1.0f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f),  XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT4(0, 0, 0, 0), XMUINT4(0, 0, 0, 0) },
	};
	for (int i = 0; i < 6; i++)
		UniqueVerts.push_back(cubeVertices[i]);
	totalUnique = UniqueVerts.size();

	static const unsigned short cubeIndices[] =
	{
		0,3,2, // +y
		0,1,3
	};
	for (int i = 0; i < 6; i++)
		Indicies.push_back(cubeIndices[i]);
	totalVerts = Indicies.size();
}

void Mesh::MakeViewPlane()
{
	this->Clear();

	static const AnimatedVert cubeVertices[] =
	{
		{ XMFLOAT4(-1.0f, -1.0f, 0.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT4(0, 0, 0, 0), XMUINT4(0, 0, 0, 0) },
		{ XMFLOAT4(-1.0f,  1.0f, 0.0f, 1.0f), XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT4(0, 0, 0, 0), XMUINT4(0, 0, 0, 0) },
		{ XMFLOAT4(1.0f,  -1.0f, 0.0f, 1.0f), XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f),  XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT4(0, 0, 0, 0), XMUINT4(0, 0, 0, 0) },
		{ XMFLOAT4(1.0f,   1.0f, 0.0f, 1.0f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f),  XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT4(0, 0, 0, 0), XMUINT4(0, 0, 0, 0) },
	};
	for (int i = 0; i < 6; i++)
		UniqueVerts.push_back(cubeVertices[i]);
	totalUnique = UniqueVerts.size();

	static const unsigned short cubeIndices[] =
	{
		0,3,2, // +y
		0,1,3
	};
	for (int i = 0; i < 6; i++)
		Indicies.push_back(cubeIndices[i]);
	totalVerts = Indicies.size();
}

AnimatedVert * Mesh::GetVerts()
{
	return UniqueVerts.data();
}

size_t Mesh::VertSize()
{
	return UniqueVerts.size();
}

unsigned short * Mesh::GetIndicies()
{
	return Indicies.data();
}

size_t Mesh::IndicieSize()
{
	return Indicies.size();
}

wchar_t *Mesh::ImagePath()
{
	return Image;
}

void Mesh::CreateNode()
{
	m_node = new Renderer::Node(this);
}
