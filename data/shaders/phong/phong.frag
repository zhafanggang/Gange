#version 450
layout (set = 1 ,binding = 0) uniform sampler2D samplerColorMap;

layout (set = 2 ,binding = 0) uniform light
{
	vec3 LightPos;
	vec3 ViewPos;
    bool blinn;
} uboLight;

layout (location = 0) in vec3 inNormal;
layout (location = 1) in vec4 inColor;
layout (location = 2) in vec2 inUV;
layout (location = 3) in vec3 inPos;

layout (location = 0) out vec4 outFragColor;

void main() 
{
	vec4 color1 = texture(samplerColorMap, inUV);

	if (color1.a < 0.5) {
		discard;
	}

    vec3 color = color1.rgb;

    vec3 ambient = 0.05 * color;

    vec3 lightDir = normalize(uboLight.LightPos - inPos);
    vec3 normal = normalize(inNormal);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * color;
    
    vec3 viewDir = normalize(uboLight.ViewPos - inPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = 0.0;
    if(uboLight.blinn)
    {
        vec3 halfwayDir = normalize(lightDir + viewDir);  
        spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);
    }
    else
    {
        vec3 reflectDir = reflect(-lightDir, normal);
        spec = pow(max(dot(viewDir, reflectDir), 0.0), 8.0);
    }
    vec3 specular = vec3(0.3) * spec; 
    outFragColor = vec4(ambient + diffuse + specular, 1.0);
}