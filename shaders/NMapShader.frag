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

layout(set = 1, binding = 3) uniform BlinnParUniformBufferObject {
    float roughness; 
    float scaleUV;
} mubo;

layout(set = 1, binding = 1) uniform sampler2D tex;
layout(set = 1, binding = 2) uniform sampler2D texNM;

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
    vec3 N = normalize(fragNorm);
    vec3 nMap = texture(texNM, fragUV).rgb;
    vec3 perturbedNormal = normalize(nMap * 2.0 - 1.0);
    vec3 Norm = normalize(perturbedNormal);
    vec3 EyeDir = normalize(gubo.eyePos - fragPos);
    
    float ambientIntensity = mix(0.005f, 0.09f, (gubo.ambient + 1.0f) * 0.5f);
    vec3 textureColor = texture(tex, fragUV * mubo.scaleUV).rgb;
    vec3 Ambient = textureColor * ambientIntensity;

    float cosThetaI, sinThetaI, cosThetaR, sinThetaR;
    calculateLightingAngles(Norm, gubo.lightDir, cosThetaI, sinThetaI);
    calculateLightingAngles(Norm, -gubo.lightDir, cosThetaR, sinThetaR);

    vec3 Diffuse = calcDiffuse(textureColor, ambientIntensity, cosThetaI, cosThetaR, sinThetaI, sinThetaR);
    vec3 sunLight = Diffuse * gubo.dayLightColor.rgb;
    vec3 moonLight = Diffuse * gubo.nightLightColor.rgb;

    vec3 spotLightDir = EyeDir;
    float dim = calculateSpotlightEffect(spotLightDir, Norm, gubo.userDir);
    vec3 spotLightColor = pow(gubo.gFactor / length(gubo.eyePos - fragPos), gubo.beta) * gubo.spotLightColor;
    vec3 spotLight = Diffuse * dim * spotLightColor;

    vec3 finalColor = sunLight + moonLight + spotLight + Ambient;
    outColor = vec4(finalColor, 1.0f);
}
