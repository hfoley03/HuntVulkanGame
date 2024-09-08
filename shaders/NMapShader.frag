#version 450
#extension GL_ARB_separate_shader_objects : enable

// Variables received from the Vertex Shader
layout(location = 0) in vec3 fragPos;
layout(location = 1) in vec3 fragNorm;
layout(location = 2) in vec2 fragUV;

// Output color
layout(location = 0) out vec4 outColor;

// Uniforms
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



// Oren-Nayar diffuse model implementation
void main() {
    float ambientIntensity = (gubo.ambient - (-1.0f)) / (1.0f - (-1.0f)) * (0.09f - 0.005f) + 0.005f;
    vec3 Norm = normalize(fragNorm);


    vec3 EyeDir = normalize(gubo.eyePos - fragPos);
    vec3 lightDir = normalize(gubo.lightDir);
    vec3 lightColor = gubo.dayLightColor.rgb;
    vec3 nightLightColor = gubo.nightLightColor.rgb;

    // Compute angles for Oren-Nayar
    float cosThetaI = max(dot(Norm, lightDir), 0.0);
    float cosThetaR = max(dot(Norm, -lightDir), 0.0);
    float sinThetaI = sqrt(1.0 - cosThetaI * cosThetaI);
    float sinThetaR = sqrt(1.0 - cosThetaR * cosThetaR);
    
    // Compute roughness factor
    float alpha = max(sinThetaI, sinThetaR);
    float beta = max(cosThetaI, cosThetaR);
    float A = 1.0 - 0.5 * (pow(mubo.roughness, 2.0) / (pow(mubo.roughness, 2.0) + 0.33));
    float B = 0.45 * (pow(mubo.roughness, 2.0) / (pow(mubo.roughness, 2.0) + 0.09));

    // Oren-Nayar diffuse term
    vec3 Diffuse = texture(tex, fragUV * mubo.scaleUV).rgb * (1.0 - ambientIntensity) *
                   ((A + B * max(0.0, cosThetaI * cosThetaR - sinThetaI * sinThetaR)) * max(cosThetaI, 0.0));

    //vec3 Specular = vec3(pow(max(dot(Norm, normalize(lightDir + EyeDir)), 0.0), mubo.Pow));
    vec3 Ambient = texture(tex, fragUV * mubo.scaleUV).rgb * ambientIntensity;

    vec3 nightDiffuse = texture(tex, fragUV * mubo.scaleUV).rgb * (1.0 - ambientIntensity) *
                        ((A + B * max(0.0, cosThetaI * cosThetaR - sinThetaI * sinThetaR)) * max(cosThetaR, 0.0));

    //vec3 nightSpecular = vec3(pow(max(dot(Norm, normalize(-lightDir + EyeDir)), 0.0), mubo.Pow));

    vec3 sunLight = Diffuse * lightColor;
    vec3 moonLight = nightDiffuse * nightLightColor;

    vec3 spotLightDir = EyeDir;
	vec3 spotLightColor = pow(gubo.gFactor / length(gubo.eyePos - fragPos), gubo.beta) * gubo.spotLightColor;
	float dim = clamp((dot(spotLightDir, -gubo.userDir) - gubo.cOut) / (gubo.cIn - gubo.cOut),0.0,1.0);

    float spotCosThetaI = max(dot(Norm, spotLightDir), 0.0);
    float spotCosThetaR = max(dot(Norm, -spotLightDir), 0.0);
    float spotSinThetaI = sqrt(1.0 - cosThetaI * cosThetaI);
    float spotSinThetaR = sqrt(1.0 - cosThetaR * cosThetaR);

    vec3 spotDiffuse = texture(tex, fragUV * mubo.scaleUV).rgb * (1.0 - ambientIntensity) *
                   ((A + B * max(0.0, spotCosThetaI * spotCosThetaR - spotSinThetaI * spotSinThetaR)) * max(spotCosThetaI, 0.0));

	vec3 spotLight = spotDiffuse * dim * spotLightColor;

    vec3 col = sunLight + moonLight + spotLight + Ambient;
    
    outColor = vec4(col, 1.0f);
   // outColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);
}
