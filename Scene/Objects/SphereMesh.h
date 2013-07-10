#pragma once

#include "Mesh.h"


class SphereMesh : public Mesh
{
public:
	SphereMesh(const std::string& name);
	~SphereMesh();

	void render() const;

private:
	// Non-copyable
	SphereMesh(const SphereMesh& planeMesh);
	SphereMesh& operator=(const SphereMesh& planeMesh);

};
