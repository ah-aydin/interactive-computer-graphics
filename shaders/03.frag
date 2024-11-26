#version 330 core

layout(location=0) out vec4 color;

in vec3 vNormal;

uniform vec3 light_dir;
uniform vec3 camera_forward_dir;

vec3 ambientColor = vec3(0.0215, 0.1745, 0.0215);
vec3 diffuseColor = vec3(0.07568, 0.61424, 0.07568);
vec3 specularColor = vec3(0.633, 0.727811, 0.633);
float shininess = 40;

float ambientIntensity = 1;
float intensity = 0.7;

vec4 calculateAmbientColor() {
  return vec4(ambientIntensity * ambientColor, 1.0);
}

vec4 calculateDiffuseColor() {
  float geometryTerm = max(0, dot(vNormal, -light_dir));
  return geometryTerm * vec4(diffuseColor, 1.0);
}

vec4 calculatePhongSpecularColor() {
  vec3 reflectionDir = normalize(reflect(light_dir, vNormal));
  float specAngle = max(0, dot(reflectionDir, -camera_forward_dir));
  float specularFactor = pow(specAngle, shininess);
  return specularFactor * vec4(specularColor, 1.0);
}

vec4 calculateBlinnSpecularColor() {
  vec3 halfVector = normalize(-light_dir - camera_forward_dir);
  float specAngle = max(0, dot(halfVector, vNormal));
  float specularFactor = pow(specAngle, shininess);
  return specularFactor * vec4(specularColor, 1.0);
}

void main(){
  color = intensity * (calculatePhongSpecularColor() + calculateDiffuseColor()) + calculateAmbientColor();
  //color = intensity * (calculateBlinnSpecularColor() + calculateDiffuseColor()) + calculateAmbientColor();
}
