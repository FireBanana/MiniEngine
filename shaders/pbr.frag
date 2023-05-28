#version 450 core

#define PI 3.142

in vec2 oUv;

layout (std140, binding = 0) uniform MatrixBlock
{
	mat4 view;
	mat4 projection;
};

uniform sampler2D _Diffuse;
uniform sampler2D _Position;
uniform sampler2D _Normal;
uniform sampler2D _Roughness;

layout (location = 0) out vec4 oAccum;

// n = surface normal
// m = microfacet normal
float specular_normal_distribution(float r, vec3 n, vec3 m)
{
    r = r * r;
    float a = (dot(n, m)) * (dot(n, m));
    float b = r - 1.;

    return r / pow( PI * ( a * a ) * b + 1, 2. );
}

// l = light vector
// v = view vector
// n = normal
float specular_geometric_attenuation(float r, vec3 n, vec3 v, vec3 l)
{
    float k = ((r + 1) * (r + 1)) / 8;
    float g1 = dot(n, v) / ( dot(n, v) * ( 1 - k ) + k );
    float g2 = dot(n, l) / ( dot(n, l) * ( 1 - k ) + k );
    
    return g1 * g2;
}

// f = reflectance at 0 degrees
float specular_fresnel(float f, vec3 v, vec3 m)
{
    return f + ( 1 - f ) * pow( 2, (-5.55473 * dot( v, m ) - 6.98316) * dot(v, m));
}

float specular_brdf(vec3 n, vec3 m, vec3 l, vec3 v, float r)
{
    float d = specular_normal_distribution(r, n, m);
    float f = specular_fresnel(1.0, v, m);
    float g = specular_geometric_attenuation(r, n, v, l);

    return ( d * f * g ) / ( 4 * dot(n, l) * dot(n, v) );
}

void main()
{
    // Egzample light position
    const vec3 lPos = vec3(0.0,0.0,-2.0);
    
    vec3 position = texture(_Position, oUv).xyz;
    vec3 normal = texture(_Normal, oUv).xyz;

    float a = 0.1 * length(normal);
    a += max(dot(normalize(normal), normalize(position - lPos)), 0.);
    vec4 res = vec4(a,a,a,1.);

    vec4 viewDirClipSpace = vec4(0.,0.,0.,1.) - vec4(position, 1.0);
    vec4 viewDir = inverse(view) * inverse(projection) * viewDirClipSpace;

    float t = specular_brdf( normal, normal, normalize( lPos - position ), normalize( viewDir.xyz ), 0.5);

    oAccum = vec4(t,t,t,1.0); // texture(_Diffuse, oUv) * a;
}