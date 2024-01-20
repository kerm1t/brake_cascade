R"(
#version 120
uniform mat4 MVP;
attribute vec3 vCol;
attribute vec3 vPos;
varying vec3 color;
uniform vec3 T; // translate
void main()
{
    gl_Position = MVP * vec4((vPos + T), 1.0);
//    color = vCol;
};
)"