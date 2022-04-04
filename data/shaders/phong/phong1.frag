#version 450
layout (set = 1 ,binding = 0) uniform light
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
	outFragColor = inColor;
}