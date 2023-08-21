// ref http://blog.simonrodriguez.fr/articles/2016/07/implementing_fxaa.html
#version 450 core

#define EDGE_THRESHOLD_MIN = 0.0312
#define EDGE_THRESHOLD_MAX = 0.125

layout (location = 0) out vec4 oAccum;

uniform sampler2D _Accum;
in vec3 fPos;

float rgb2luma(vec3 rgb)
{
    return sqrt(dot(rgb, vec3(0.299, 0.587, 0.114)));
}

void main()
{
    vec3 centerColor = texture(_Accum, fPos).rgb;

    float lumaCenter = rgb2luma(centerColor);

    float lumaDown  = rgb2luma( textureOffset(_Accum, fPos, ivec2(0,-1)).rgb );
    float lumaUp    = rgb2luma( textureOffset(_Accum, fPos, ivec2(0,1)).rgb );
    float lumaLeft  = rgb2luma( textureOffset(_Accum, fPos, ivec2(-1,0)).rgb );
    float lumaRight = rgb2luma( textureOffset(_Accum, fPos, ivec2(1,0)).rgb );

    float lumaMin = min( lumaCenter, min( min(lumaDown, lumaUp), min(lumaLeft, lumaRight) ) );
    float lumaMax = max( lumaCenter, max( max(lumaDown, lumaUp), max(lumaLeft, lumaRight) ) );

    float lumaRange = lumaMax - lumaMin;

    if(lumaRange < max(EDGE_THRESHOLD_MIN, lumaMax * EDGE_THRESHOLD_MAX))
    {
        oAccum = colorCenter;
        return;
    }

    float lumaDownUp = lumaDown + lumaUp;
    float lumaLeftRight = lumaLeft + lumaRight;

    float lumaDownLeft  = rgb2luma(textureOffset(_Accum, fPos, ivec2(-1,-1)).rgb);
    float lumaUpRight   = rgb2luma(textureOffset(_Accum, fPos, ivec2(1,1)).rgb);
    float lumaUpLeft    = rgb2luma(textureOffset(_Accum, fPos, ivec2(-1,1)).rgb);
    float lumaDownRight = rgb2luma(textureOffset(_Accum, fPos, ivec2(1,-1)).rgb);

    float edgeHorizontal = abs(-2.0 * lumaLeft + lumaLeftCorners) + abs(-2. * lumaCenter + lumaDownUp) * 2.0 + abs(-2.0 * lumaRight + lumaRightCorners);
    float edgeVertical =   abs(-2.0 * lumaUp + lumaUpCorners) + abs(-2.0 * lumaCenter + lumaLeftRight) * 2.0  + abs(-2.0 * lumaDown + lumaDownCorners);

    bool isHorizontal = edgeHorizontal >= edgeVertical;
}