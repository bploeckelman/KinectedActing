#pragma once

#include "Mesh.h"


class CylinderMesh : public Mesh
{
public:
	CylinderMesh(const std::string& name);
	~CylinderMesh();

	void render() const;

private:
	// Non-copyable
	CylinderMesh(const CylinderMesh& cylinderMesh);
	CylinderMesh& operator=(const CylinderMesh& clyinderMesh);

};
