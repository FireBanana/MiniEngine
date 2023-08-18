#version 450 core

layout (std140, binding = 0) uniform SceneBlock
{
	mat4 view;
	mat4 projection;
    vec3 cameraPos;

	vec3 lightPos1;
    float lightIntensity1;

    float roughness;
    float metallic;
};

in vec2 fUv;
in vec3 fNormal;
in vec3 fPosition;

uniform sampler2D _Diffuse;
uniform sampler2D _Normal;
uniform sampler2D _Roughness;

uniform float _baseRoughness;
uniform float _baseMetallic;

layout (location = 1) out vec4 oDiffuse; 
layout (location = 2) out vec4 oPosition; 
layout (location = 3) out vec4 oNormal; 
layout (location = 4) out vec4 oRoughness;

// http://www.thetenthplanet.de/archives/1180
mat3 cotangent_frame( vec3 N, vec3 p, vec2 uv ) 
{
	 // get edge vectors of the pixel triangle 
	vec3 dp1 = dFdx( p );
	vec3 dp2 = dFdy( p ); 
	vec2 duv1 = dFdx( uv ); 
	vec2 duv2 = dFdy( uv );
	// solve the linear system 
	vec3 dp2perp = cross( dp2, N );
	vec3 dp1perp = cross( N, dp1 ); 
	vec3 T = dp2perp * duv1.x + dp1perp * duv2.x; 
	vec3 B = dp2perp * duv1.y + dp1perp * duv2.y;   
	// construct a scale-invariant frame 
	float invmax = inversesqrt( max( dot(T,T), dot(B,B) ) ); 
	return mat3( T * invmax, B * invmax, N ); 
}

void main()
{
	vec3 v = normalize(fNormal);

	vec4 normalTexture = (texture(_Normal, fUv)) * 2.0 - 1.0;
	vec4 roughnessTex = texture(_Roughness, fUv);
	roughnessTex.x *= _baseRoughness;
	roughnessTex.y *= _baseMetallic;

	mat3 m = cotangent_frame(v, fPosition, fUv);

	oDiffuse = texture(_Diffuse, fUv);
	oPosition = vec4(fPosition, 0.0);
	oNormal = vec4(m * normalTexture.xyz,0);
	oRoughness = roughnessTex;
}