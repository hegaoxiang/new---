#pragma once
#include "GRiPreInclude.h"
#include "GRiSubmesh.h"
#include "GRiMeshData.h"

class GRiMesh
{
public:
	virtual ~GRiMesh() {};
	// Unique name for hash table lookup.
	std::string UniqueName;

	std::unordered_map<std::string, GRiSubmesh> Submeshes;
};

