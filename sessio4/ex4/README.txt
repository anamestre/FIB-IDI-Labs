COM ES RESOL:

Hem de carregar el Homer:

1. Afegir el path de models al ".pro".
2. Crear al ".h" el model i els VAO's i VBO's corresponents: un VBO pels vèrtexs i un pel material de l'objecte.
  #include "model.h" al ".h".
3. Carregar el model a CreateBuffers. Posar tots els VAO's i VBO's. -> Esta a la funcio carregarModel().
4. S'han de canviar atributs a les funcions:
  - paintGL:
	glClear (GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
     glDrawArrays(GL_TRIANGLES, 0, homer.faces().size()*3);
  - initialize:
	glEnable(GL_DEPTH_TEST);