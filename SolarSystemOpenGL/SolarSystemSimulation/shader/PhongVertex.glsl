#version 300 es
// Vertex information
layout(location = 0) in vec4  VertexPosition;
layout(location = 1) in vec3  Normal;
layout(location = 2) in vec2  texCoord;

// Model View Project matrix
uniform mat4    ModelViewProjectionMatrix;
uniform mat4    ModelViewMatrix;
uniform mat4    ModelMatrix;
uniform mat3    NormalMatrix;

out vec3    normalCoord;
out vec3    eyeCoord;
out vec2    textureCoord;

void main()
{
    normalCoord = NormalMatrix * Normal;
    eyeCoord    = vec3 ( ModelViewMatrix * VertexPosition );
    textureCoord = texCoord;
    
    gl_Position = ModelViewProjectionMatrix * VertexPosition;
}
