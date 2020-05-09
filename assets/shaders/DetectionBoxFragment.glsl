#version 330 core

in vec2 vTextureCoordinate;
out vec4 color;

uniform float uBoxOutlineWidth;
uniform float uBoxAspect;
uniform vec4 uBoxOutlineColor;
uniform vec4 uBoxFillColor;

void main()
{
    float maxX = 1.0 - uBoxOutlineWidth;
    float minX = uBoxOutlineWidth;
    float maxY = 1.0 - uBoxOutlineWidth;
    float minY = uBoxOutlineWidth;

    if (vTextureCoordinate.x < maxX && vTextureCoordinate.x > minX && 
        vTextureCoordinate.y < maxY && vTextureCoordinate.y > minY)
    {
        color = uBoxFillColor;
    }
    else
    {
        color = uBoxOutlineColor;
    }
}