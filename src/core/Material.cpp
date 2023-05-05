#include "Material.h"
#include "Texture.h"

Material::Material(Texture texture, Shader* shader) :
	mShader(shader)
{
	mTextureReference.push(texture);
}
