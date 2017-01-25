#version 330 core

in vec3 fcolor;
out vec4 FragColor;
uniform vec3 colFocus;
uniform vec3 posFocus;

void main()
{	
	FragColor = vec4(fcolor,1);	
}
