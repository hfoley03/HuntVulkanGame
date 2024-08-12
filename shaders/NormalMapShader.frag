#version 450#extension GL_ARB_separate_shader_objects : enable// this defines the variable received from the Vertex Shader// the locations must match the one of its out variableslayout(location = 0) in vec3 fragPos;layout(location = 1) in vec3 fragNorm;layout(location = 2) in vec2 fragUV;layout(location = 3) in vec4 fragTan;// This defines the color computed by this shader. Generally is always location 0.layout(location = 0) out vec4 outColor;// Here the Uniform buffers are defined. In this case, the Global Uniforms of Set 0// The texture of Set 1 (binding 1), and the Material parameters (Set 1, binding 2)// are used. Note that each definition must match the one used in the CPP codelayout(set = 0, binding = 0) uniform NormalMapUniformBufferObject {	vec3 lightDir;	vec4 lightColor;	vec3 eyePos;} gubo;layout(set = 1, binding = 6) uniform NormalMapParUniformBufferObject {	float Pow;	float Ang;	float ShowCloud;	float ShowTexture;} mubo;layout(set = 1, binding = 1) uniform sampler2D texDiff;layout(set = 1, binding = 2) uniform sampler2D texSpec;layout(set = 1, binding = 3) uniform sampler2D texNM;layout(set = 1, binding = 4) uniform sampler2D texEmit;layout(set = 1, binding = 5) uniform sampler2D texClouds;// The main shader, implementing a simple Blinn + Lambert + constant Ambient BRDF model// The scene is lit by a single Spot Lightvoid main() {	vec3 Norm = normalize(fragNorm);	vec3 Tan = normalize(fragTan.xyz - Norm * dot(fragTan.xyz, Norm));	vec3 Bitan = cross(Norm, Tan) * fragTan.w;	mat3 tbn = mat3(Tan, Bitan, Norm);	vec4 nMap = texture(texNM, fragUV);	vec3 N = normalize(tbn * (vec3(-1, 1, -1) + nMap.rgb * vec3(2, -2, 2)));		vec3 EyeDir = normalize(gubo.eyePos - fragPos);		vec3 lightDir = normalize(gubo.lightDir);	vec3 lightColor = gubo.lightColor.rgb;		float DiffInt = max(dot(N, lightDir),0.0);	float CloudInt = max(dot(Norm, lightDir),0.0);	float EmitInt = pow(1 - DiffInt,20);	float SpecInt = pow(max(dot(N, normalize(lightDir + EyeDir)),0.0), mubo.Pow);	vec3 DiffColor = texture(texDiff, fragUV).rgb; 	vec3 SpecColor = texture(texSpec, fragUV).rgb; 	vec3 CloudCol = texture(texClouds, fragUV + vec2(mubo.Ang,0)).rgb * mubo.ShowCloud;	vec3 Diffuse = (DiffColor * DiffInt + CloudCol * CloudInt ) * (1-EmitInt);	vec3 Specular = (SpecColor + CloudCol.rgb * 0.05) * SpecInt;	vec3 Emit = texture(texEmit, fragUV).rgb * EmitInt * (1 - CloudCol.g) + 0.01 * CloudCol.g;		vec3 col  = ((Diffuse + Specular) * lightColor + Emit) * mubo.ShowTexture +				(DiffInt + SpecInt) * vec3(1) * (1 - mubo.ShowTexture);	//vec3 col  = Diffuse * lightColor;		outColor = vec4(col, 1.0f);//	outColor = vec4(vec3(mubo.Ang), 1.0f);//	outColor = vec4(gubo.eyePos/5.0+vec3(0.5),1.0);//	outColor = vec4(0.5*N+vec3(0.5),1.0);	//outColor = vec4(fragPos/5.0+vec3(0.5),1.0);}