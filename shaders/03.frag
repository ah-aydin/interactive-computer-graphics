#version 330 core

layout(location=0) out vec4 color;

in vec3 vNormal;

void main(){
  color = vec4(vNormal, 1.0);
}
