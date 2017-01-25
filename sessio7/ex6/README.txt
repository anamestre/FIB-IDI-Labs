


Exercici 5
Pas a uniforms de la posició i el color del focus de
llum:
- Convertir la posició i el color en uniforms en el VS
- Inicialitzar aquests uniforms al MyGLWidget
- Fixem-nos que ara podríem passar el uniform de posició
directament ja en SCO
Podem també passar a uniform el color de la llum
ambient

COM ES FA:
--------------------------------------------------------------
- Anar al vertex shader i canviar: 
vec3 colFocus = vec3(0.8, 0.8, 0.8);
   per  uniform vec3 colFocus;
vec3 posFocus = vec3(1, 0, -1);  
   per uniform vec3 posFocus;

- Anar al MyGLWidget.cpp:

  (prèviament declarar al ".h": colFocusLoc, posFocusLoc, colorFocus i posiFocus.

  FUNCIÓ carregarShaders();
  colFocusLoc = glGetUniformLocation(program->programId(), "colFocus");
  posFocusLoc = glGetUniformLocation(program->programId(), "posFocus");
  
  FUNCIÓ initializeGL(); (al final de la funció).
  colorFocus = glm::vec3(0.8, 0.8, 0.8);
  posiFocus = glm::vec3(1, 1, 1);
  glUniform3fv(colFocusLoc, 1, &colorFocus[0]);
  glUniform3fv(posFocusLoc, 1, &posiFocus[0]);