#version 300 es
layout(location = 0) in vec4 VertexPosition;
uniform mat4 uMVPMatrix;


void main()
{
    gl_Position = uMVPMatrix * VertexPosition;
}
