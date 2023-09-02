#version 450 core

layout (location = 0) out vec4 oAccum;

uniform sampler2D _Accum;
uniform float _width; //inverse width
uniform float _height; //inverse height

in vec2 oUv;

#define FXAA_EDGE_THRESHOLD_MIN = 0.0625
#define FXAA_EDGE_THRESHOLD = 0.125


float FxaaLuma(vec3 rgb)
{
    return rgb.y * (0.587/0.299) + rgb.x;
}

void main()
{
    // Local contrast check to early exit when contrast not over certain threshold
    vec3 colorN = textureOffset( _Accum, oUv, ivec2(0, -1) ).rgb;
    vec3 colorW = textureOffset( _Accum, oUv, ivec2(-1, 0) ).rgb;
    vec3 colorM = textureOffset( _Accum, oUv, ivec2(0, 0) ).rgb;
    vec3 colorE = textureOffset( _Accum, oUv, ivec2(1, 0) ).rgb;
    vec3 colorS = textureOffset( _Accum, oUv, ivec2(0, 1) ).rgb;

    float lumaN = FxaaLuma(colorN);
    float lumaW = FxaaLuma(colorW);
    float lumaM = FxaaLuma(colorM);
    float lumaE = FxaaLuma(colorE);
    float lumaS = FxaaLuma(colorS);

    float rangeMin = min( min(lumaN, lumaW), min(lumaS, lumaE) );
    float rangeMax = max( max(lumaN, lumaW), max(lumaS, lumaE) );
    float contrast = rangeMax - rangeMin;

    if(contrast < max(0.0625, rangeMax * 0.125)) // threshold, local constranst threshold
    {
        oAccum = vec4(colorM, 1.);
        return;
    }

    // Blending factor
    vec3 colorNE = textureOffset( _Accum, oUv, ivec2(1, 1) ).rgb;
    vec3 colorNW = textureOffset( _Accum, oUv, ivec2(-1, 1) ).rgb;
    vec3 colorSE = textureOffset( _Accum, oUv, ivec2(1, -1) ).rgb;
    vec3 colorSW = textureOffset( _Accum, oUv, ivec2(-1, -1) ).rgb;

    float lumaNE = FxaaLuma(colorNE);
    float lumaNW = FxaaLuma(colorNW);
    float lumaSE = FxaaLuma(colorSE);
    float lumaSW = FxaaLuma(colorSW);

    float averageLuminance = 2 * ( lumaN + lumaS + lumaE + lumaW ) + lumaNE + lumaNW + lumaSE + lumaSW;
    averageLuminance *= 1.0 / 12.0;
    averageLuminance = abs(averageLuminance - lumaM);
    averageLuminance = clamp(averageLuminance / contrast, 0., 1.);
    averageLuminance = smoothstep(0., 1., averageLuminance);
    float blendFactor = averageLuminance * averageLuminance; // subpixel blending value here

    // Blend direction - Finding edges
    float verticalContrast   = 2 * abs( lumaN + lumaS - 2*lumaM ) + abs( lumaNE + lumaSE - 2*lumaE ) + abs( lumaNW + lumaSW - 2*lumaW );
    float horizontalContrast = 2 * abs( lumaE + lumaW - 2*lumaM ) + abs( lumaNE + lumaNW - 2*lumaN ) + abs( lumaSE + lumaSW - 2*lumaS );
    bool isHorizontalEdge = verticalContrast >= horizontalContrast; 

    vec2 stepSize = vec2(_width, _height);

    float positiveLuma = isHorizontalEdge ? lumaN : lumaE;
    float negativeLuma = isHorizontalEdge ? lumaS : lumaW;

    float positiveGradient = abs(positiveLuma - lumaM);
    float negativeGradient = abs(negativeLuma - lumaM);

    if(positiveGradient < negativeGradient) stepSize *= -1;

    // Blending
    vec2 uv = oUv;

    if(isHorizontalEdge) uv.y += stepSize.y * blendFactor;
    else                 uv.x += stepSize.x * blendFactor;

    oAccum = textureLod(_Accum, uv, 0.);
}