#version 450

layout (location = 0) in vec3 inPos;
layout (location = 1) in vec2 inUV;

layout (set = 0,binding = 0) uniform UBO 
{
	mat4 mvp;
	mat4 model;
} uboScene;

layout (location = 0) out vec2 outUV;

void main() 
{
    gl_Position = uboScene.mvp  * uboScene.model * vec4(inPos.xyz, 1.0);
    outUV = inUV;
}
