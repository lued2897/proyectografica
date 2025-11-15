#version 330 core
out uvec4 outID;

uniform int objectID;

void main()
{
    outID = uvec4(objectID, 0, 0, 1);
}
