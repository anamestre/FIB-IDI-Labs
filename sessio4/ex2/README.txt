COM ES RESOL:

1. Tocar el vertex shader, afegir: uniform mat4 view;
2. Al main del vertex shader, multiplicar proj tal que:
    gl_Position = proj*view*TG * vec4 (vertex, 1.0);
3. Al MyGLWidget.cpp afegir al CARREGARSHADERS:
  viewLoc = glGetUniformLocation(program->programId(), "view");
  (S'ha d'haver declarat viewLoc al ".h")
4. Al MyGLWidget.cpp afegir la funció projectTransform:
	void MyGLWidget::viewTransform(){
	  glm::mat4 View = glm::lookAt(glm::vec3(0,0,1), glm::vec3(0,0,0), glm::vec3(0,1,0));
	  glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &View[0][0]);
}
5. Cridem viewTransform al initialize.

SEGONA PART:
1. Al ".cpp" creem la funcio ini_camera. (declarem totes les variables necessàries i la pròpia funció al ".h").
 Hi inicialitzem les variables.
 Ara cridarem aquí a projectTransform i viewTransform.
2. Caviem el codi a projectTransform i viewTransform perque passin les variables de iniCamera.
3. Cridem iniCamera a initialize