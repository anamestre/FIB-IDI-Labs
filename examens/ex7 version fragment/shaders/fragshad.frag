#version 330 core

//in vec3 fcolor;
in vec3 yvertex;
in vec3 ynormal;

in vec3 ymatamb;
in vec3 ymatdiff;
in vec3 ymatspec;
in float ymatshin;

out vec4 FragColor;


uniform int canviFranjes;
uniform mat4 proj;
uniform mat4 view;
uniform mat4 TG;
uniform int pintaVaca;
uniform int teclaL;

vec3 matambaux = ymatamb;
vec3 matdiffaux = ymatdiff;
vec3 matspecaux = ymatspec;


vec3 colFocus = vec3(0.0, 0.8, 0.8); //focus blau
//vec3 colFocus= vec3(1,1,1); 
vec3 llumAmbient = vec3(0.2, 0.2, 0.2);
vec3 posFocus = vec3(1, 1, 1);  // en SCA

vec3 posFocusAux = posFocus;
vec3 colFocusAux = colFocus;

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

    if ((dot(R, V) < 0) || (ymatshin == 0))
      return colRes;  // no hi ha component especular
    
    // Afegim la component especular
    float shine = pow(max(0.0, dot(R, V)), ymatshin);
    return (colRes + matspecaux * colFocusAux * shine);
}


void main() {
/*
   if(canviFranjes == 1){
      int num = int(gl_FragCoord.y)/10;
      if(num%2==0)FragColor = vec4 (1. ,1. ,1. ,1.); // vermell costat esquerra 
      else FragColor = vec4 (0. ,0. ,0. ,1.);
   }
   else FragColor = vec4(fcolor,1);
   */
   
   if(pintaVaca == 1){
        matspecaux = vec3(1,1,1);
        matdiffaux = vec3(0.3, 0.3, 0.3);
        matambaux = vec3(0.3, 0.3, 0.3);
    }

    vec4 vertSCO = view * TG * vec4 (yvertex, 1.0);
    mat3 normalMatrix = inverse (transpose (mat3 (view * TG)));
    vec3 normalSCO = normalize (normalMatrix * ynormal);
    //vec4 focusSCO = view * vec4 (posFocus, 1.0);
    
    vec4 focusSCO;
    
    if(teclaL == 1){
      posFocusAux = vec3(1,1,1);
      colFocusAux = vec3(1,1,1);
      focusSCO = view*(vec4(posFocusAux, 1.0));
      
    }
    else focusSCO = vec4 (posFocusAux, 1.0);
    vec3 L = normalize (focusSCO.xyz - vertSCO.xyz);
    FragColor = vec4 (Phong(normalSCO, L, vertSCO), 1.0f);
}
