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

    float oppositeLuma;
    float oppositeGradient;

    if(positiveGradient < negativeGradient)
    {
        stepSize *= -1;
        oppositeLuma = negativeLuma;
        oppositeGradient = negativeGradient;
    }
    else
    {
        oppositeLuma = positiveLuma;
        oppositeGradient = positiveGradient;
    }

    // Blending -- refactor
    vec2 uv = oUv;

    if(isHorizontalEdge) uv.y += stepSize.y * blendFactor;
    else                 uv.x += stepSize.x * blendFactor;

    //oAccum = textureLod(_Accum, uv, 0.);

    float originalEdgeLuminance = (lumaM + oppositeLuma) * 0.5;
    float gradientThreshold = oppositeGradient * 0.25;

    vec2 edgeUv = oUv;
    vec2 edgeStep;

    if(isHorizontalEdge)
    {
        edgeUv.y += stepSize.y * 0.5;
        edgeStep = vec2(0., stepSize.y);
    }
    else
    {
        edgeUv.x += stepSize.x * 0.5;
        edgeStep = vec2(stepSize.x, 0.);
    }

    vec2 pUv = edgeUv + edgeStep;
    vec2 nUv = edgeUv - edgeStep;

    float pLuminanceDelta = FxaaLuma(texture(_Accum, pUv).rgb) - originalEdgeLuminance;
    float nLuminanceDelta = FxaaLuma(texture(_Accum, nUv).rgb) - originalEdgeLuminance;

    bool pAtEnd = abs(pLuminanceDelta) >= gradientThreshold;
    bool nAtEnd = abs(nLuminanceDelta) >= gradientThreshold;

    for(int i = 0; i < 9 && (!pAtEnd || !nAtEnd); ++i)
    {
        if(!pAtEnd)
        {
            pUv += edgeStep;
            pLuminanceDelta = FxaaLuma(texture(_Accum, pUv).rgb) - originalEdgeLuminance;
            pAtEnd = abs(pLuminanceDelta) >= gradientThreshold;
        }
        if(!nAtEnd)
        {
            nUv += edgeStep;
            nLuminanceDelta = FxaaLuma(texture(_Accum, nUv).rgb) - originalEdgeLuminance;
            pAtEnd = abs(nLuminanceDelta) >= gradientThreshold;
        }
    }

    float pDistance;
    float nDistance;

    if(isHorizontalEdge)
    {
        pDistance = pUv.x - uv.x;
        nDistance = uv.y - nUv.y;
    }
    else
    {
        pDistance = pUv.y - uv.y;
        nDistance = uv.y - nUv.y;
    }

    float shortestDistance = pDistance <= nDistance ? pDistance : nDistance;

    oAccum = vec4(shortestDistance * 10.);
}