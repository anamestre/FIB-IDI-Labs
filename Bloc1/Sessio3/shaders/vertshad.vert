#version 330 core

in vec3 vertex;
in vec4 colors;
out vec4 outcolors;

void main()  {
    gl_Position = vec4 (vertex*0.5, 1.0);
    outcolors = colors;
}
