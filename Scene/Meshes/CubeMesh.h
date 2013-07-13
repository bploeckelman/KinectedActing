#pragma once

#include "Mesh.h"


class CubeMesh : public Mesh
{
public:
	CubeMesh(const std::string& name);
	~CubeMesh();

	void render() const;

private:
	// Non-copyable
	CubeMesh(const CubeMesh& planeMesh);
	CubeMesh& operator=(const CubeMesh& planeMesh);

};
