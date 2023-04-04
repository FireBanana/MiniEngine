#pragma once

class Mesh;
class Material;

class RenderPass
{
public:

	RenderPass();

	void setGeometry(const Mesh* mesh);
	void setMaterial(const Material* material);

private:

	const Mesh* mMesh;
	const Material* mMaterial;
};