#pragma once

#include "Mesh.h"


class CapsuleMesh : public Mesh
{
public:
	CapsuleMesh(const std::string& name);
	~CapsuleMesh();

	void render() const;

private:
	// Non-copyable
	CapsuleMesh(const CapsuleMesh& planeMesh);
	CapsuleMesh& operator=(const CapsuleMesh& planeMesh);

};
