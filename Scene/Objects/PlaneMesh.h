#pragma once

#include "Mesh.h"


class PlaneMesh : public Mesh
{
public:
	PlaneMesh(const std::string& name
		    , float width=10.f
		    , float height=10.f
		    , float spacing=1.f);
	~PlaneMesh();

	void render() const;

private:
	// Non-copyable
	PlaneMesh(const PlaneMesh& planeMesh);
	PlaneMesh& operator=(const PlaneMesh& planeMesh);

};
