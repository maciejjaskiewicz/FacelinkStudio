#version 330 core

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec2 aTextureCoordinate;

uniform mat4 uViewProjection;

out vec2 vTextureCoordinate;

void main()
{
    vTextureCoordinate = aTextureCoordinate;
    gl_Position = uViewProjection * vec4(aPosition, 1.0);
}