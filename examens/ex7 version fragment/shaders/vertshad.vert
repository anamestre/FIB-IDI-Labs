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
uniform int pintaVaca;
uniform int teclaL;


/*
// Valors per als components que necessitem dels focus de llum
vec3 colFocus = vec3(0.0, 0.8, 0.8); //focus blau
//vec3 colFocus= vec3(1,1,1); 
vec3 llumAmbient = vec3(0.2, 0.2, 0.2);
vec3 posFocus = vec3(1, 1, 1);  // en SCA

vec3 matambaux = matamb;
vec3 matdiffaux = matdiff;
vec3 matspecaux = matspec;
vec3 posFocusAux = posFocus;
vec3 colFocusAux = colFocus; */

out vec3 yvertex;
out vec3 ynormal;

out vec3 ymatamb;
out vec3 ymatdiff;
out vec3 ymatspec;
out float ymatshin;

//out vec3 fcolor;

/*
vec3 Lambert (vec3 NormSCO, vec3 L) 
{
    // S'assumeix que els vectors que es reben com a paràmetres estan normalitzats

    // Inicialitzem color a component ambient
    vec3 colRes = llumAmbient * matambaux;

    // Afegim component difusa, si n'hi ha
    if (dot (L, NormSCO) > 0)
      colRes = colRes + colFocusAux * matdiffaux * dot (L, NormSCO);
    return (colRes);
}

vec3 Phong (vec3 NormSCO, vec3 L, vec4 vertSCO) 
{
    // Els vectors estan normalitzats

    // Inicialitzem color a Lambert
    vec3 colRes = Lambert (NormSCO, L);

    // Calculem R i V
    if (dot(NormSCO,L) < 0)
      return colRes;  // no hi ha component especular

    vec3 R = reflect(-L, NormSCO); // equival a: normalize (2.0*dot(NormSCO,L)*NormSCO - L);
    vec3 V = normalize(-vertSCO.xyz);

    if ((dot(R, V) < 0) || (matshin == 0))
      return colRes;  // no hi ha component especular
    
    // Afegim la component especular
    float shine = pow(max(0.0, dot(R, V)), matshin);
    return (colRes + matspecaux * colFocusAux * shine);
}
*/

void main() { 
    yvertex = vertex;
    ynormal = normal;
    ymatamb = matamb;
    ymatdiff = matdiff;
    ymatspec = matspec;
    ymatshin = matshin;
    
    vec4 vertSCO = view * TG * vec4 (vertex, 1.0);
    gl_Position = proj * vertSCO;     
    
}
