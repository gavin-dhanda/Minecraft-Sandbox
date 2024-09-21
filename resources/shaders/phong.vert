#version 330 core

layout (location = 0) in vec3 pos;
layout(location = 1) in vec3 objectSpaceNormal;
layout(location = 2) in vec2 UV;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;


out vec3 world_pos;
out vec3 world_normal;
out vec4 camera_pos;
out vec2 fragUV;

void main() {
   gl_Position = projMatrix * viewMatrix * modelMatrix * vec4(pos,1.0);
   world_pos = vec3(modelMatrix * vec4(pos,1.0));
   world_normal = transpose(inverse(mat3(modelMatrix))) * normalize((objectSpaceNormal));
   camera_pos = inverse(viewMatrix) * vec4(0,0,0,1);
   fragUV = UV;
}
