#version 450

layout (location = 0) in vec3 inPos;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec4 inColor;
layout (location = 3) in vec2 inUV;

layout (set = 0 ,binding = 0) uniform UBOScene
{
	mat4 projection;
	mat4 view;
	mat4 model;
} uboScene;

layout(push_constant) uniform PushConsts {
	mat4 model;
} primitive;

layout (location = 0) out vec3 outNormal;
layout (location = 1) out vec4 outColor;
layout (location = 2) out vec2 outUV;
layout (location = 3) out vec3 outPos;

void main() 
{
	outUV = inUV;

	vec4 pos = uboScene.model * primitive.model * vec4(inPos, 1.0);

	outPos = pos.xyz;

	outNormal = inNormal;

	outColor = inColor;

	gl_Position = uboScene.projection * uboScene.view * pos;
}