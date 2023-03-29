#include "RenderPass.h"

RenderPass::RenderPass()
{
}

void RenderPass::setGeometry(const Mesh& mesh)
{
	mMesh = &mesh;
}

void RenderPass::setMaterial(const Material& material)
{
	mMaterial = &material;
}
