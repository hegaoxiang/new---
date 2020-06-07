#pragma once
#include "GRiPreInclude.h"
#include "GRiVertex.h"

class GRiMeshData
{
public:
	GRiMeshData() {};
	~GRiMeshData() {};

	std::string MeshDataName;

	std::vector<GRiVertex> Vertices;
	std::vector<uint32_t> Indices;

};


