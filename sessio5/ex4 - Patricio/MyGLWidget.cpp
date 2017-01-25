#include "MyGLWidget.h"
#include <cmath>
#include <iostream>
#include <algorithm> // per calcular mínims.

// ANA MESTRE: VERSIÓ FINAL PER L'EXAMEN DE LAB.

MyGLWidget::MyGLWidget (QWidget* parent) : QOpenGLWidget(parent)
{
  setFocusPolicy(Qt::ClickFocus);  // per rebre events de teclat
  scale = 1.0f;
}

MyGLWidget::~MyGLWidget ()
{
  if (program != NULL)
    delete program;
}


void MyGLWidget::initializeGL ()
{
  // Cal inicialitzar l'ús de les funcions d'OpenGL
  initializeOpenGLFunctions();  

  glClearColor(0.5, 0.7, 1.0, 1.0); // defineix color de fons (d'esborrat)
  glEnable(GL_DEPTH_TEST);
  carregaShaders();
  createBuffers();
  iniCamera();
}

void MyGLWidget::paintGL () 
{
  // Esborrem el frame-buffer
  glClear (GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

  // Carreguem la transformació de model
  modelTransform ();
  pintaPatricio();
 // pintaHomer();
 // modelTransformTerra();
  //pintaTerra();

  glBindVertexArray (0);
}

void MyGLWidget::modelTransform () 
{
  // Matriu de transformació de model
  glm::mat4 transform (1.0f);
  transform = glm::scale(transform, glm::vec3(scale));
  transform = glm::rotate(transform, rotacio, glm::vec3(0,1,0));
  transform = glm::translate(transform, -centre);
  glUniformMatrix4fv(transLoc, 1, GL_FALSE, &transform[0][0]);
}

void MyGLWidget::resizeGL (int w, int h) {
    glViewport(0, 0, w, h);
    float raV = float (w) / float (h);
    ra = raV;
    if(raV < 1.0) FOV = 2.0*atan(tan((float)M_PI/4)/raV);
    projectTransform();
}

void MyGLWidget::keyPressEvent(QKeyEvent* event) 
{
  switch (event->key()) {
    makeCurrent();
    case Qt::Key_S: { // escalar a més gran
      scale += 0.05;
      break;
    }
    case Qt::Key_D: { // escalar a més petit
      scale -= 0.05;
      break;
    }
    case Qt::Key_R: {
        rotacio += M_PI/4.0;
        break;
    }
    default: event->ignore(); break;
  }
  update();
}

void MyGLWidget::createBuffers () {
  //homer.load("E:/Documents/EXAMEN IDI/versio neta/sessio5/ex2/models/HomerProves.obj");
  //calculaCapsaContenidoraHomer();
  //carregarModel();
  //carregaTerra();

  patricio.load("E:/Documents/EXAMEN IDI/versio neta/sessio5/ex4 - Patricio/models/Patricio.obj");
  calculaCapsaContenidoraPatricio();
  carregarPatricio();
  glBindVertexArray (0);
}

void MyGLWidget::carregaShaders()
{
  // Creem els shaders per al fragment shader i el vertex shader
  QOpenGLShader fs (QOpenGLShader::Fragment, this);
  QOpenGLShader vs (QOpenGLShader::Vertex, this);

  // Carreguem el codi dels fitxers i els compilem
  //fs.compileSourceFile("shaders/fragshad.frag");
  //vs.compileSourceFile("shaders/vertshad.frag");

  fs.compileSourceFile("E:/Documents/EXAMEN IDI/versio neta/sessio5/ex4 - Patricio/shaders/fragshad.frag");
  vs.compileSourceFile("E:/Documents/EXAMEN IDI/versio neta/sessio5/ex4 - Patricio/shaders/vertshad.vert");
  // Creem el program
  program = new QOpenGLShaderProgram(this);
  // Li afegim els shaders corresponents
  program->addShader(&fs);
  program->addShader(&vs);
  // Linkem el program
  program->link();
  // Indiquem que aquest és el program que volem usar
  program->bind();

  // Obtenim identificador per a l'atribut “vertex” del vertex shader
  vertexLoc = glGetAttribLocation (program->programId(), "vertex");
  // Obtenim identificador per a l'atribut “color” del vertex shader
  colorLoc = glGetAttribLocation (program->programId(), "color");
  // Uniform locations
  transLoc = glGetUniformLocation(program->programId(), "TG");


  // Sessio 4: exercici 1.
  projLoc = glGetUniformLocation(program->programId(), "proj");
  viewLoc = glGetUniformLocation(program->programId(), "view");


}


// --------------------------------
// ----- LES MEVES FUNCIONS -------
// --------------------------------

// Sessio 4: exercici 1.
void MyGLWidget::projectTransform(){
    //glm::mat4 Proj = glm::perspective ((float)M_PI/2.0f, 1.0f, 1.0f, 3.0f); // <-- Sessio 4, exercici 1.
    glm::mat4 Proj = glm::perspective (FOV, ra, zNear, zFar); // <-- Sessio 4, exercici 2.
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, &Proj[0][0]);
}

// Sessio 4: exercici 2.
void MyGLWidget::viewTransform(){
  //glm::mat4 View = glm::lookAt(glm::vec3(0,0,1), glm::vec3(0,0,0), glm::vec3(0,1,0)); // <-- Sessio 4, exercici 1.
  glm::mat4 View = glm::lookAt(OBS, VRP, UP); // <-- Sessio 4, exercici 2.
  glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &View[0][0]);
}

// Sessio 4: exercici 2.
// Modificat FOV a Sessio 5, exercici 4.
void MyGLWidget::iniCamera(){
    float dist = radi*1.5;
    //FOV = 2*asin(radi/dist);
    FOV = float(M_PI/2.0);
    ra = 1.0;
    zNear = radi;
    zFar = dist + radi;

    VRP = glm::vec3(0,0,0);
    OBS= VRP + dist*glm::vec3(0.f, 0.f, 1.f);
    UP = glm::vec3(0,1,0);

    modelTransform();
    projectTransform();
    viewTransform();
}

// Sessio 4: exercici 4. [Homer]
void MyGLWidget::carregarModel(){
    glGenVertexArrays(1, &VAOhomer);
    glBindVertexArray(VAOhomer);

    glGenBuffers(1, &VBOhomerVertex);
    glBindBuffer(GL_ARRAY_BUFFER, VBOhomerVertex);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*homer.faces().size()*3*3, homer.VBO_vertices(), GL_STATIC_DRAW);

    // Activem l'atribut vertexLoc
    glVertexAttribPointer(vertexLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(vertexLoc);

    glGenBuffers(1, &VBOhomerMaterial);
    glBindBuffer(GL_ARRAY_BUFFER, VBOhomerMaterial);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*homer.faces().size()*3*3, homer.VBO_matdiff(), GL_STATIC_DRAW);

    // Activem l'atribut colorLoc
    glVertexAttribPointer(colorLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(colorLoc);

}

void MyGLWidget::pintaHomer(){
  // Activem el VAO per a pintar la caseta
  glBindVertexArray (VAOhomer);

  // pintem
  glDrawArrays(GL_TRIANGLES, 0, homer.faces().size()*3);
}

void MyGLWidget::pintaPatricio(){
  // Activem el VAO per a pintar la caseta
  glBindVertexArray (VAOpatricio);

  // pintem
  glDrawArrays(GL_TRIANGLES, 0, patricio.faces().size()*3);
}

void MyGLWidget::pintaTerra(){
  glBindVertexArray (VAOterra);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}


void MyGLWidget::carregaTerra(){
   glm::vec3 Vertexs[4];
   Vertexs[0] = glm::vec3(-1.0, -1.0, -1.0);
   Vertexs[1] = glm::vec3(-1.0, -1.0, 1.0);
   Vertexs[2] = glm::vec3(1.0, -1.0, -1.0);
   Vertexs[3] = glm::vec3(1.0, -1.0, 1.0);


   glGenVertexArrays(1, &VAOterra);
   glBindVertexArray(VAOterra);

   glGenBuffers(1, &VBOterraVertex);
   glBindBuffer(GL_ARRAY_BUFFER, VBOterraVertex);
   glBufferData(GL_ARRAY_BUFFER, sizeof(Vertexs), Vertexs, GL_STATIC_DRAW);

   // Activem l'atribut vertexLoc
   glVertexAttribPointer(vertexLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
   glEnableVertexAttribArray(vertexLoc);

   glm::vec3 Colors[4];
   Colors[0] = glm::vec3(1, 0, 0);
   Colors[1] = glm::vec3(0, 1, 0);
   Colors[2] = glm::vec3(0, 0, 1);
   Colors[3] = glm::vec3(1, 0, 1);

   glGenBuffers(1, &VBOterraColor);
   glBindBuffer(GL_ARRAY_BUFFER, VBOterraColor);
   glBufferData(GL_ARRAY_BUFFER, sizeof(Colors), Colors, GL_STATIC_DRAW);

   // Activem l'atribut colorLoc
   glVertexAttribPointer(colorLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
   glEnableVertexAttribArray(colorLoc);
}


void MyGLWidget::modelTransformTerra ()
{
  // Matriu de transformació de model
  glm::mat4 transform (1.0f);
  transform = glm::scale(transform, glm::vec3(scale));
  glUniformMatrix4fv(transLoc, 1, GL_FALSE, &transform[0][0]);
}


// Sessio 5: exercici 2.
void MyGLWidget::calculaCapsaContenidoraHomer(){
    // Inicialització del punt mínim.
    modelMinim.x = homer.vertices()[0];
    modelMinim.y = homer.vertices()[1];
    modelMinim.z = homer.vertices()[2];

    // Inicialització del punt màxim.
    modelMaxim.x = homer.vertices()[0];
    modelMaxim.y = homer.vertices()[1];
    modelMaxim.z = homer.vertices()[2];

    int mida = homer.vertices().size();

    for(int i = 3; i < mida; i+=3){
        float auxX = homer.vertices()[i];
        float auxY = homer.vertices()[i+1];
        float auxZ = homer.vertices()[i+2];

        modelMinim.x = std::min(modelMinim.x, auxX);
        modelMaxim.x = std::max(modelMaxim.x, auxX);

        modelMinim.y = std::min(modelMinim.y, auxY);
        modelMaxim.y = std::max(modelMaxim.y, auxY);

        modelMinim.z = std::min(modelMinim.z, auxZ);
        modelMaxim.z = std::max(modelMaxim.z, auxZ);
    }

    calculaCentreRadi();
}

// Sessio 5: exercici 4.
void MyGLWidget::calculaCapsaContenidoraPatricio(){
    // Inicialització del punt mínim.
    modelMinim.x = patricio.vertices()[0];
    modelMinim.y = patricio.vertices()[1];
    modelMinim.z = patricio.vertices()[2];

    // Inicialització del punt màxim.
    modelMaxim.x = patricio.vertices()[0];
    modelMaxim.y = patricio.vertices()[1];
    modelMaxim.z = patricio.vertices()[2];

    int mida = patricio.vertices().size();

    for(int i = 3; i < mida; i+=3){
        float auxX = patricio.vertices()[i];
        float auxY = patricio.vertices()[i+1];
        float auxZ = patricio.vertices()[i+2];

        modelMinim.x = std::min(modelMinim.x, auxX);
        modelMaxim.x = std::max(modelMaxim.x, auxX);

        modelMinim.y = std::min(modelMinim.y, auxY);
        modelMaxim.y = std::max(modelMaxim.y, auxY);

        modelMinim.z = std::min(modelMinim.z, auxZ);
        modelMaxim.z = std::max(modelMaxim.z, auxZ);
    }

    calculaCentreRadi();
}

// Sessió 5, ex 2: Calcula el centre i el radi pel model Homer.
void MyGLWidget::calculaCentreRadi(){
    centre = glm::vec3((modelMaxim.x + modelMinim.x)/2,(modelMaxim.y - modelMinim.y)/2, (modelMaxim.z + modelMinim.z)/2);

    float modulX = pow(modelMaxim.x - centre.x, 2);
    float modulY = pow(modelMaxim.y - centre.y, 2);
    float modulZ = pow(modelMaxim.z - centre.z, 2);
    radi = sqrt(modulX + modulY + modulZ);

}

void MyGLWidget::carregarPatricio(){

    glGenVertexArrays(1, &VAOpatricio);
    glBindVertexArray(VAOpatricio);

    glGenBuffers(1, &VBOpatricioVertex);
    glBindBuffer(GL_ARRAY_BUFFER, VBOpatricioVertex);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*patricio.faces().size()*3*3, patricio.VBO_vertices(), GL_STATIC_DRAW);

    // Activem l'atribut vertexLoc
    glVertexAttribPointer(vertexLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(vertexLoc);

    glGenBuffers(1, &VBOpatricioMaterial);
    glBindBuffer(GL_ARRAY_BUFFER, VBOpatricioMaterial);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*patricio.faces().size()*3*3, patricio.VBO_matdiff(), GL_STATIC_DRAW);

    // Activem l'atribut colorLoc
    glVertexAttribPointer(colorLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(colorLoc);
}
