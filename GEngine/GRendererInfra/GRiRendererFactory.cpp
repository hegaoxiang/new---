#include "GRiRendererFactory.h"

GRiGeometryGenerator* GRiRendererFactory::CreateGeometryGenerator()
{
	GRiGeometryGenerator* geoGenerator = new GRiGeometryGenerator();
	return geoGenerator;
}

GRiSceneObject* GRiRendererFactory::CreateSceneObjet()
{
	GRiSceneObject* obj = new GRiSceneObject();
	return obj;
}
