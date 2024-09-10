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
    vec3 pointLightColor;
    vec3 userDir;
    float gFactor;
    float beta;
    float cIn;
    float cOut;
} gubo;

layout(set = 1, binding = 2) uniform BlinnParUniformBufferObject {
    float Pow;
    float scaleUV;
} mubo;

layout(set = 1, binding = 1) uniform sampler2D tex;

// blinn phong specular component
vec3 calcSpecular(vec3 norm, vec3 lightDir, vec3 eyeDir, float shininess) {
    vec3 halfDir = normalize(lightDir + eyeDir);
    float specFactor = pow(max(dot(norm, halfDir), 0.0), shininess);
    return vec3(specFactor);
}

// calculate diffuse lighting
vec3 calcDiffuse(vec3 norm, vec3 lightDir, vec3 textureColor, float ambientIntensity) {
    float diffuseFactor = max(dot(norm, lightDir), 0.0);
    return textureColor * (1.0 - ambientIntensity) * diffuseFactor;
}


void main() {
    vec3 norm = normalize(fragNorm);
    vec3 eyeDir = normalize(gubo.eyePos - fragPos);
    vec3 textureColor = texture(tex, fragUV * mubo.scaleUV).rgb;

    // ambient 
    float ambientIntensity = mix(0.005f, 0.09f, (gubo.ambient + 1.0f) * 0.5f);
    vec3 ambient = textureColor * ambientIntensity;

    // sun
    vec3 lightDir = normalize(gubo.lightDir);
    vec3 sunlightDiffuse = calcDiffuse(norm, lightDir, textureColor, ambientIntensity);
    vec3 sunlightSpecular = calcSpecular(norm, lightDir, eyeDir, mubo.Pow);
    vec3 sunlight = (sunlightDiffuse + sunlightSpecular) * gubo.dayLightColor.rgb;

    // moon
    vec3 moonlightDiffuse = calcDiffuse(norm, -lightDir, textureColor, ambientIntensity);
    vec3 moonlightSpecular = calcSpecular(norm, -lightDir, eyeDir, mubo.Pow);
    vec3 moonlight = (moonlightDiffuse + moonlightSpecular) * gubo.nightLightColor.rgb;

    // torch 
    vec3 pointLightDir = eyeDir;
    float distanceFactor = pow(gubo.gFactor / length(gubo.eyePos - fragPos), gubo.beta);
    vec3 pointLightColor = distanceFactor * gubo.pointLightColor;
    float spotlightEffect = clamp((dot(pointLightDir, -gubo.userDir) - gubo.cOut) / (gubo.cIn - gubo.cOut), 0.0, 1.0);
    
    vec3 pointDiffuse = calcDiffuse(norm, pointLightDir, textureColor, ambientIntensity);
    vec3 pointSpecular = calcSpecular(norm, pointLightDir, eyeDir, mubo.Pow);
    vec3 pointLight = (pointDiffuse + pointSpecular) * spotlightEffect * pointLightColor;
    
    vec3 finalColor = sunlight + moonlight + pointLight + ambient ;

    outColor = vec4(finalColor, 1.0f);
}
