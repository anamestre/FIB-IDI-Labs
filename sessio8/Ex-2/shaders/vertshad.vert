#version 330 core

in vec3 vertex;
in vec3 normal;

in vec3 matamb;
in vec3 matdiff;
in vec3 matspec;
in float matshin;

uniform mat4 proj;
uniform mat4 view;
uniform mat4 TG;

//Pasamos el color y la posición de la luz como uniforms
uniform vec3 colFocus;
uniform vec3 posFocus;

out vec3 matambfragment;
out vec3 matdifffragment;
out vec3 matspecfragment;
out float matshinfragment;
out mat4 viewmatrix;

// Valors per als components que necessitem dels focus de llum
//vec3 colFocus = vec3(0.8, 0.8, 0.8);
vec3 llumAmbient = vec3(0.2, 0.2, 0.2);
//vec3 posFocus = vec3(1, 1, 1);  // en SCA

out vec3 fcolor;
out vec4 vertSCO;
out vec3 normalSCO;


void main()
{	
    //fcolor = matdiff;
    //gl_Position = proj * view * TG * vec4 (vertex, 1.0);
    vertSCO = (view * TG * vec4 (vertex, 1.0));
    mat3 normalMatrix = inverse (transpose (mat3 (view * TG)));
    normalSCO =  normalize(normalMatrix * normal);
    fcolor = colFocus;

    gl_Position = proj * vertSCO;

    matambfragment = matamb;
    matdifffragment = matdiff;
    matspecfragment = matspec;
    matshinfragment = matshin;
    viewmatrix = view;
}
