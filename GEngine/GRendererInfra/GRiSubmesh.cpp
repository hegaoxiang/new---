#include "GRiSubmesh.h"

void GRiSubmesh::SetMaterial(GRiMaterial* material)
{
	pMaterial = material;
}

GRiMaterial* GRiSubmesh::GetMaterial()
{
	return pMaterial;
}
