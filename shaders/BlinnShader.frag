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
    float noiseLevel;
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


void main() {
    vec3 norm = normalize(fragNorm);
    vec3 eyeDir = normalize(gubo.eyePos - fragPos);
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
