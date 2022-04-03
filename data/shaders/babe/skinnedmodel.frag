#version 450
layout (set = 1, binding = 0) uniform sampler2D samplerColorMap;

layout (location = 0) in vec2 inNormal;
layout (location = 1) in vec2 inUV;
layout (location = 2) in vec3 inColor;

layout (location = 0) out vec4 outFragColor;

void main() 
{
	vec4 color = outFragColor = texture(samplerColorMap, inUV);
}