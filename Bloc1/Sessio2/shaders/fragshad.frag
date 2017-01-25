#version 330 core

out vec4 FragColor;
in vec4 outcolors;

void pintarFranges(){
    FragColor = vec4(1.0);
    int num = int(gl_FragCoord.y)/10;
    if(num%2==0){
      if (gl_FragCoord .x < 347.){ // ESQUERRA.
         if(gl_FragCoord .y < 347.) { //ABAIX
           FragColor = vec4 (1. ,1. ,0. ,1.); // vermell costat esquerra.
         }
         if(gl_FragCoord .y >= 347.){ // ADALT
           FragColor = vec4 (1. ,0. ,0. ,1.);
         }
     }

     if ( gl_FragCoord .x >= 347.){ // ESQUERRA.
         if(gl_FragCoord .y < 347.) { //ABAIX
           FragColor = vec4 (0. ,1. ,0. ,1.); // verd
         }
         if(gl_FragCoord .y >= 347.){ // ADALT
           FragColor = vec4 (0. ,0. ,1. ,1.); // blau
         }
     }
    }
    else discard;
}

void main() {
    FragColor = outcolors;
    //pintarFranges();
}
