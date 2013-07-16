#pragma once

#include "Mesh.h"


class AxisMesh : public Mesh
{
public:
	AxisMesh(const std::string& name);
	~AxisMesh();

	void render() const;

private:
	// Non-copyable
	AxisMesh(const AxisMesh& axisMesh);
	AxisMesh& operator=(const AxisMesh& axisMesh);

};
