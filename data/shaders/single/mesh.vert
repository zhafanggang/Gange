#version 450

layout (location = 0) in vec3 inPos;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec2 inUV;

layout (set = 0 ,binding = 0) uniform UBOScene
{
	mat4 projection;
	mat4 view;
} uboScene;

layout (location = 0) out vec2 outUV;

void main() 
{
	outUV = inUV;
	gl_Position = uboScene.projection * uboScene.view * vec4(inPos.xyz, 1.0);
}