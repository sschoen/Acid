#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout(set = 0, binding = 0) uniform UboScene
{
	mat4 projection;
	mat4 view;
} scene;

struct Instance
{
	mat4 transform;
	vec4 colourOffset;
	vec4 offsets;
	vec3 blend;
	float _padding;
};

layout(set = 0, binding = 1) buffer Instances
{
	Instance data[MAX_INSTANCES];
} instances;

layout(set = 0, location = 0) in vec3 inPosition;

layout(location = 0) out vec2 outCoords1;
layout(location = 1) out vec2 outCoords2;
layout(location = 2) out vec4 outColourOffset;
layout(location = 3) out float outBlendFactor;
layout(location = 4) out float outTransparency;

out gl_PerVertex 
{
	vec4 gl_Position;
};

void main() 
{
	Instance instance = instances.data[gl_InstanceIndex];

	mat4 modelMatrix = instance.transform;
	modelMatrix[0][0] = scene.view[0][0];
	modelMatrix[0][1] = scene.view[1][0];
	modelMatrix[0][2] = scene.view[2][0];
	modelMatrix[1][0] = scene.view[0][1];
	modelMatrix[1][1] = scene.view[1][1];
	modelMatrix[1][2] = scene.view[2][1];
	modelMatrix[2][0] = scene.view[0][2];
	modelMatrix[2][1] = scene.view[1][2];
	modelMatrix[2][2] = scene.view[2][2];

	vec4 worldPosition = modelMatrix * vec4(inPosition, 1.0f);

	gl_Position = scene.projection * scene.view * worldPosition;

	vec2 uv = inPosition.xy + vec2(0.5f, 0.5f);
	uv.y = 1.0f - uv.y;
	uv /= instance.blend.z;

	outColourOffset = instance.colourOffset;
	outCoords1 = uv + instance.offsets.xy;
	outCoords2 = uv + instance.offsets.zw;
	outBlendFactor = instance.blend.x;
	outTransparency = instance.blend.y;
}
