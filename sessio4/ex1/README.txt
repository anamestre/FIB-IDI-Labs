COM ES RESOL:

1. Tocar el vertex shader, afegir: uniform mat4 proj;
2. Al main del vertex shader, multiplicar proj tal que:
    gl_Position = proj*TG * vec4 (vertex, 1.0);
3. Al MyGLWidget.cpp afegir al CARREGARSHADERS:
  projLoc = glGetUniformLocation(program->programId(), "proj");
  (S'ha d'haver declarat projLoc al ".h")
4. Al MyGLWidget.cpp afegir la funció projectTransform:
	void MyGLWidget::projectTransform(){
	    glm::mat4 Proj = glm::perspective ((float)M_PI/2.0f, 	1.0f, 1.0f, 3.0f);
	    glUniformMatrix4fv(projLoc, 1, GL_FALSE, &Proj[0][0]);
}