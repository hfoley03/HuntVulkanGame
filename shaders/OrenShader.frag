#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 fragPos;
layout(location = 1) in vec3 fragNorm;
layout(location = 2) in vec2 fragUV;

layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 0) uniform GlobalUniformBufferObject {
    vec3 lightDir;
    vec4 dayLightColor;
    vec3 eyePos;
    vec4 nightLightColor;
    float ambient;
	vec3 spotLightColor;
	vec3 userDir;
    float gFactor;
	float beta;
	float cIn;
	float cOut;
} gubo;

layout(set = 1, binding = 2) uniform BlinnParUniformBufferObject {
    float roughness; 
    float scaleUV;
    float noiseLevel;
} mubo;

layout(set = 1, binding = 1) uniform sampler2D tex;

float random (vec2 coord) {
    return fract(sin(dot(coord.xy, vec2(12.9898,78.233))) * 43758.5453123);
}

float noise(vec2 coord){
    vec2 i = floor(coord);
    vec2 f = fract(coord);
    // four corners
    float a = random(i);
    float b = random(i + vec2(1.0, 0.0));
    float c = random(i + vec2(0.0, 1.0));
    float d = random(i + vec2(1.0, 1.0));

    vec2 u = smoothstep(0., 1., f);
    float mixValue = mix(a, b, u.x) + (c - a)* u.y * (1.0 - u.x) + (d - b) * u.x * u.y;
    return clamp(mixValue, 0.0, 1.0);
}

void calculateLightingAngles(vec3 norm, vec3 lightDir, out float cosTheta, out float sinTheta) {
    cosTheta = max(dot(norm, lightDir), 0.0);
    sinTheta = sqrt(1.0 - cosTheta * cosTheta);
}

//diffuse component using Oren-Nayar model
vec3 calcDiffuse(vec3 textureColor, float ambientIntensity, float cosThetaI, float cosThetaR, float sinThetaI, float sinThetaR) {
    float roughnessSquared = pow(mubo.roughness, 2.0);
    float A = 1.0 - 0.5 * (roughnessSquared / (roughnessSquared + 0.33));
    float B = 0.45 * (roughnessSquared / (roughnessSquared + 0.09));

    return textureColor * (1.0 - ambientIntensity) * 
           (A + B * max(0.0, cosThetaI * cosThetaR - sinThetaI * sinThetaR)) * max(cosThetaI, 0.0);
}

float calculateSpotlightEffect(vec3 spotLightDir, vec3 norm, vec3 userDir) {
    float dim = clamp((dot(spotLightDir, -userDir) - gubo.cOut) / (gubo.cIn - gubo.cOut), 0.0, 1.0);
    return dim;
}

void main() {
    vec3 Norm = normalize(fragNorm);
    vec3 EyeDir = normalize(gubo.eyePos - fragPos);
    
    float ambientIntensity = mix(0.005f, 0.09f, (gubo.ambient + 1.0f) * 0.5f);
    vec3 textureColor = texture(tex, fragUV * mubo.scaleUV).rgb;

    if( mubo.noiseLevel > 0.0){
        vec2 resolution = vec2(0.1, 0.1); 
        float smallNoise = noise( mubo.noiseLevel * (fragPos.xz / resolution));
        vec3 smallNoiseVec = 0.25*  vec3(smallNoise,smallNoise,smallNoise);
        resolution = vec2(10.0, 10.0);
        float largeNoise = noise( mubo.noiseLevel * (fragPos.xz / resolution));
        vec3 largeNoiseVec = 0.5*  vec3(largeNoise,largeNoise,largeNoise);
        textureColor = textureColor - mix(smallNoiseVec, largeNoiseVec, 0.25);
    }  

    vec3 Ambient = textureColor * ambientIntensity;

    float cosThetaI, sinThetaI, cosThetaR, sinThetaR;
    calculateLightingAngles(Norm, gubo.lightDir, cosThetaI, sinThetaI);
    calculateLightingAngles(Norm, -gubo.lightDir, cosThetaR, sinThetaR);

    vec3 Diffuse = calcDiffuse(textureColor, ambientIntensity, cosThetaI, cosThetaR, sinThetaI, sinThetaR);
    vec3 sunLight = Diffuse * gubo.dayLightColor.rgb;

    calculateLightingAngles(Norm, -gubo.lightDir, cosThetaI, sinThetaI);
    calculateLightingAngles(Norm, gubo.lightDir, cosThetaR, sinThetaR);

    vec3 DiffuseNight = calcDiffuse(textureColor, ambientIntensity, cosThetaI, cosThetaR, sinThetaI, sinThetaR);

    vec3 moonLight = DiffuseNight * gubo.nightLightColor.rgb;

    vec3 spotLightDir = EyeDir;
    float dim = calculateSpotlightEffect(spotLightDir, Norm, gubo.userDir);
    vec3 spotLightColor = pow(gubo.gFactor / length(gubo.eyePos - fragPos), gubo.beta) * gubo.spotLightColor;
    
    calculateLightingAngles(Norm, spotLightDir, cosThetaI, sinThetaI);
    calculateLightingAngles(Norm, -spotLightDir, cosThetaR, sinThetaR);

    vec3 DiffuseSpotLight = calcDiffuse(textureColor, ambientIntensity, cosThetaI, cosThetaR, sinThetaI, sinThetaR);

    vec3 spotLight = DiffuseSpotLight * dim * spotLightColor;

    vec3 finalColor = sunLight + moonLight + spotLight + Ambient;
    outColor = vec4(finalColor, 1.0f);

}
