#version 300 es

uniform vec2 uvScale;

uniform mat4    ModelViewProjectionMatrix;
uniform mat4    ModelViewMatrix;
uniform mat4    ProjectionMatrix;

layout(location = 0) in vec4  position;
layout(location = 1) in vec2  uv;

out vec2 vUv;

void main()
{
    
    vUv =  uv * uvScale;
    vec4 mvPosition = ModelViewMatrix * position;
    gl_Position = ProjectionMatrix * mvPosition;
    
}

