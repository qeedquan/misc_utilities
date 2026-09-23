precision mediump float;
precision mediump int;

uniform float fraction;

varying vec4 vertColor;
varying vec3 vertNormal;
varying vec3 vertLightDir;

void main() {
  float intensity;
  vec4 color;
  intensity = max(0.0, dot(vertLightDir, vertNormal));

  if (intensity > pow(0.95, fraction)) {
    color = vec4(vec3(0, 1.0, 0), 1.0);
  } else if (intensity > pow(0.5, fraction)) {
    color = vec4(vec3(0, 0.6, 0), 1.0);
  } else if (intensity > pow(0.25, fraction)) {
    color = vec4(vec3(0, 0.4, 0), 1.0);
  } else {
    color = vec4(vec3(0, 0.2, 0), 1.0);
  }

  gl_FragColor = color * vertColor;
}
