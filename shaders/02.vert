#version 330 core

layout(location=0) in vec3 pos;
layout(location=1) in vec3 texCoord;
layout(location=2) in vec3 normal;

out vec3 vColor;
uniform mat4 mvp;

void main(){
  gl_Position = mvp * vec4(pos, 1.0);
  vColor = normal;
}
