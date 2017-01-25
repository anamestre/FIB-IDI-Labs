#include "MyGLWidget.h"

#include <iostream>

MyGLWidget::MyGLWidget (QWidget* parent) : QOpenGLWidget(parent) {
  setFocusPolicy(Qt::ClickFocus);  // per rebre events de teclat
  xClick = yClick = 0;
  angleY = 0.0;
  perspectiva = true;
  DoingInteractive = NONE;
  rotacio = 0.0;
  //radiEsc = sqrt(5);
  teclaL = 1;
}

MyGLWidget::~MyGLWidget () {
  if (program != NULL)
    delete program;
}

void MyGLWidget::initializeGL () {
  // Cal inicialitzar l'ús de les funcions d'OpenGL
  initializeOpenGLFunctions();  
  glClearColor(0.5, 0.7, 1.0, 1.0); // defineix color de fons (d'esborrat)
  glEnable(GL_DEPTH_TEST);


  carregaShaders();
  createBuffers();

  calculaCapsaEscena();
  iniCamera();

  projectTransform ();
  viewTransform ();
}

void MyGLWidget::iniCamera(){
    float dist = radiEsc*2;
    FOV = 2*asin(radiEsc/dist);
    //FOV = float(M_PI/3.0);
    FOVi = FOV;
    ra = 1.0;
    zNear = radiEsc;
    zFar = dist + radiEsc;
    VRP = glm::vec3(1, -0.375, 0);
    teclaX = 1;

}

void MyGLWidget::paintGL () {
  // Esborrem el frame-buffer i el depth-buffer
  glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Activem el VAO per a pintar el terra 
  //pintaVaca = 0;
  //glUniformMatrix4fv (pintaVacaLoc, 1, GL_FALSE, &pintaVaca[0][0]);
  glBindVertexArray (VAO_Terra);
  modelTransformTerra ();
  glDrawArrays(GL_TRIANGLES, 0, 12);

  //pintaVaca = 1;
  glUniform1i(pintaVacaLoc, 1);
  // Pintem la vaca.
  glBindVertexArray (VAO_Vaca);
  modelTransformVaca ();
  glDrawArrays(GL_TRIANGLES, 0, vaca.faces().size()*3);

  //pintaVaca = 0;
  glUniform1i(pintaVacaLoc, 0);
  // Activem el VAO per a pintar el Patricio
  glBindVertexArray (VAO_Patr);
  modelTransformPatricio ();
  glDrawArrays(GL_TRIANGLES, 0, patr.faces().size()*3);
  
  glBindVertexArray(0);
}

void MyGLWidget::resizeGL (int w, int h) {
    glViewport(0, 0, w, h);
    float raV = float (w) / float (h);
    ra = raV;
    if(raV < 1.0) FOV = 2.0*atan(tan(FOVi/2.0)/raV);
    projectTransform();

}

// ------------ CARREGAR ---------------

void MyGLWidget::createBuffers () {
  // Carreguem el model de l'OBJ - Atenció! Abans de crear els buffers!

  carregaPatricio();
  carregaTerra();
  carregaVaca();

  glBindVertexArray(0);
}

void MyGLWidget::carregaPatricio(){
    patr.load("models/Patricio.obj");

    // Calculem la capsa contenidora del model
    calculaCapsaModel ();
    //calculaCapsaEscena();

    // Creació del Vertex Array Object del Patricio
    glGenVertexArrays(1, &VAO_Patr);
    glBindVertexArray(VAO_Patr);

    // Creació dels buffers del model patr
    // Buffer de posicions
    glGenBuffers(1, &VBO_PatrPos);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_PatrPos);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*patr.faces().size()*3*3, patr.VBO_vertices(), GL_STATIC_DRAW);

    // Activem l'atribut vertexLoc
    glVertexAttribPointer(vertexLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(vertexLoc);

    // Buffer de normals
    glGenBuffers(1, &VBO_PatrNorm);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_PatrNorm);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*patr.faces().size()*3*3, patr.VBO_normals(), GL_STATIC_DRAW);

    glVertexAttribPointer(normalLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(normalLoc);

    // En lloc del color, ara passem tots els paràmetres dels materials
    // Buffer de component ambient
    glGenBuffers(1, &VBO_PatrMatamb);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_PatrMatamb);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*patr.faces().size()*3*3, patr.VBO_matamb(), GL_STATIC_DRAW);

    glVertexAttribPointer(matambLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(matambLoc);

    // Buffer de component difusa
    glGenBuffers(1, &VBO_PatrMatdiff);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_PatrMatdiff);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*patr.faces().size()*3*3, patr.VBO_matdiff(), GL_STATIC_DRAW);

    glVertexAttribPointer(matdiffLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(matdiffLoc);

    // Buffer de component especular
    glGenBuffers(1, &VBO_PatrMatspec);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_PatrMatspec);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*patr.faces().size()*3*3, patr.VBO_matspec(), GL_STATIC_DRAW);

    glVertexAttribPointer(matspecLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(matspecLoc);

    // Buffer de component shininness
    glGenBuffers(1, &VBO_PatrMatshin);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_PatrMatshin);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*patr.faces().size()*3, patr.VBO_matshin(), GL_STATIC_DRAW);

    glVertexAttribPointer(matshinLoc, 1, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(matshinLoc);

}

void MyGLWidget::carregaTerra(){

    // Dades del terra
    // VBO amb la posició dels vèrtexs
    glm::vec3 posterra[12] = {
      glm::vec3(-2.0, -1.0, 2.0),
      glm::vec3(2.0, -1.0, 2.0),
      glm::vec3(-2.0, -1.0, -2.0),
      glm::vec3(-2.0, -1.0, -2.0),
      glm::vec3(2.0, -1.0, 2.0),
      glm::vec3(2.0, -1.0, -2.0),
      glm::vec3(-2.0, -1.0, -2.0),
      glm::vec3(2.0, -1.0, -2.0),
      glm::vec3(-2.0, 1.0, -2.0),
      glm::vec3(-2.0, 1.0, -2.0),
      glm::vec3(2.0, -1.0, -2.0),
      glm::vec3(2.0, 1.0, -2.0)
    };

    // VBO amb la normal de cada vèrtex
    glm::vec3 norm1 (0,1,0);
    glm::vec3 norm2 (0,0,1);
    glm::vec3 normterra[12] = {
      norm1, norm1, norm1, norm1, norm1, norm1, // la normal (0,1,0) per als primers dos triangles
      norm2, norm2, norm2, norm2, norm2, norm2  // la normal (0,0,1) per als dos últims triangles
    };

    // Definim el material del terra
    glm::vec3 amb(0.2,0,0.2);
    glm::vec3 diff(0.8,0,0.8);
    glm::vec3 spec(0,0,0);
    float shin = 100;

    // Fem que aquest material afecti a tots els vèrtexs per igual
    glm::vec3 matambterra[12] = {
      amb, amb, amb, amb, amb, amb, amb, amb, amb, amb, amb, amb
    };
    glm::vec3 matdiffterra[12] = {
      diff, diff, diff, diff, diff, diff, diff, diff, diff, diff, diff, diff
    };
    glm::vec3 matspecterra[12] = {
      spec, spec, spec, spec, spec, spec, spec, spec, spec, spec, spec, spec
    };
    float matshinterra[12] = {
      shin, shin, shin, shin, shin, shin, shin, shin, shin, shin, shin, shin
    };

  // Creació del Vertex Array Object del terra
    glGenVertexArrays(1, &VAO_Terra);
    glBindVertexArray(VAO_Terra);

    glGenBuffers(1, &VBO_TerraPos);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_TerraPos);
    glBufferData(GL_ARRAY_BUFFER, sizeof(posterra), posterra, GL_STATIC_DRAW);

    // Activem l'atribut vertexLoc
    glVertexAttribPointer(vertexLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(vertexLoc);

    glGenBuffers(1, &VBO_TerraNorm);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_TerraNorm);
    glBufferData(GL_ARRAY_BUFFER, sizeof(normterra), normterra, GL_STATIC_DRAW);

    // Activem l'atribut normalLoc
    glVertexAttribPointer(normalLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(normalLoc);

    // En lloc del color, ara passem tots els paràmetres dels materials
    // Buffer de component ambient
    glGenBuffers(1, &VBO_TerraMatamb);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_TerraMatamb);
    glBufferData(GL_ARRAY_BUFFER, sizeof(matambterra), matambterra, GL_STATIC_DRAW);

    glVertexAttribPointer(matambLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(matambLoc);

    // Buffer de component difusa
    glGenBuffers(1, &VBO_TerraMatdiff);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_TerraMatdiff);
    glBufferData(GL_ARRAY_BUFFER, sizeof(matdiffterra), matdiffterra, GL_STATIC_DRAW);

    glVertexAttribPointer(matdiffLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(matdiffLoc);

    // Buffer de component especular
    glGenBuffers(1, &VBO_TerraMatspec);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_TerraMatspec);
    glBufferData(GL_ARRAY_BUFFER, sizeof(matspecterra), matspecterra, GL_STATIC_DRAW);

    glVertexAttribPointer(matspecLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(matspecLoc);

    // Buffer de component shininness
    glGenBuffers(1, &VBO_TerraMatshin);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_TerraMatshin);
    glBufferData(GL_ARRAY_BUFFER, sizeof(matshinterra), matshinterra, GL_STATIC_DRAW);

    glVertexAttribPointer(matshinLoc, 1, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(matshinLoc);

}

void MyGLWidget::carregaVaca() {
    vaca.load("models/cow.obj");

    // Calculem la capsa contenidora del model
    calculaCapsaVaca ();
    //calculaCapsaEscena();

    // Creació del Vertex Array Object del Patricio
    glGenVertexArrays(1, &VAO_Vaca);
    glBindVertexArray(VAO_Vaca);

    // Creació dels buffers del model patr
    // Buffer de posicions
    glGenBuffers(1, &VBO_VacaPos);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_VacaPos);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*vaca.faces().size()*3*3, vaca.VBO_vertices(), GL_STATIC_DRAW);

    // Activem l'atribut vertexLoc
    glVertexAttribPointer(vertexLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(vertexLoc);

    // Buffer de normals
    glGenBuffers(1, &VBO_VacaNorm);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_VacaNorm);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*vaca.faces().size()*3*3, vaca.VBO_normals(), GL_STATIC_DRAW);

    glVertexAttribPointer(normalLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(normalLoc);

    // En lloc del color, ara passem tots els paràmetres dels materials
    // Buffer de component ambient
    glGenBuffers(1, &VBO_VacaMatamb);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_VacaMatamb);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*vaca.faces().size()*3*3, vaca.VBO_matamb(), GL_STATIC_DRAW);

    glVertexAttribPointer(matambLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(matambLoc);

    // Buffer de component difusa
    glGenBuffers(1, &VBO_VacaMatdiff);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_VacaMatdiff);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*vaca.faces().size()*3*3, vaca.VBO_matdiff(), GL_STATIC_DRAW);

    glVertexAttribPointer(matdiffLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(matdiffLoc);

    // Buffer de component especular
    glGenBuffers(1, &VBO_VacaMatspec);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_VacaMatspec);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*vaca.faces().size()*3*3, vaca.VBO_matspec(), GL_STATIC_DRAW);

    glVertexAttribPointer(matspecLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(matspecLoc);

    // Buffer de component shininness
    glGenBuffers(1, &VBO_VacaMatshin);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_VacaMatshin);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*vaca.faces().size()*3, vaca.VBO_matshin(), GL_STATIC_DRAW);

    glVertexAttribPointer(matshinLoc, 1, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(matshinLoc);
}

void MyGLWidget::carregaShaders()
{
  // Creem els shaders per al fragment shader i el vertex shader
  QOpenGLShader fs (QOpenGLShader::Fragment, this);
  QOpenGLShader vs (QOpenGLShader::Vertex, this);
  // Carreguem el codi dels fitxers i els compilem
  fs.compileSourceFile("shaders/fragshad.frag");
  vs.compileSourceFile("shaders/vertshad.vert");
  //fs.compileSourceFile("E:/Documents/EXAMEN IDI/versio neta/examens/ex1/shaders/fragshad.frag");
  //vs.compileSourceFile("E:/Documents/EXAMEN IDI/versio neta/examens/ex1/shaders/vertshad.vert");

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
  // Obtenim identificador per a l'atribut “normal” del vertex shader
  normalLoc = glGetAttribLocation (program->programId(), "normal");
  // Obtenim identificador per a l'atribut “matamb” del vertex shader
  matambLoc = glGetAttribLocation (program->programId(), "matamb");
  // Obtenim identificador per a l'atribut “matdiff” del vertex shader
  matdiffLoc = glGetAttribLocation (program->programId(), "matdiff");
  // Obtenim identificador per a l'atribut “matspec” del vertex shader
  matspecLoc = glGetAttribLocation (program->programId(), "matspec");
  // Obtenim identificador per a l'atribut “matshin” del vertex shader
  matshinLoc = glGetAttribLocation (program->programId(), "matshin");

  // Demanem identificadors per als uniforms del vertex shader
  transLoc = glGetUniformLocation (program->programId(), "TG");
  projLoc = glGetUniformLocation (program->programId(), "proj");
  viewLoc = glGetUniformLocation (program->programId(), "view");
  pintaVacaLoc = glGetUniformLocation (program->programId(), "pintaVaca");
  teclaXLoc = glGetUniformLocation (program->programId(), "canviFranjes");
  teclaLloc = glGetUniformLocation (program->programId(), "teclaL");
}

// ------ TRANSFORMS --------

void MyGLWidget::modelTransformPatricio () {
  glm::mat4 TG(1.f);  // Matriu de transformació

  TG = glm::rotate(TG, rotacio, glm::vec3(0,1,0)); // Canviat d'ordre perque giri correctament com demana l'enunciat.
  TG = glm::translate(TG, glm::vec3(1, -0.375,0));
  TG = glm::scale(TG, glm::vec3(escala, escala, escala));
  //TG = glm::rotate(TG, rotacio, glm::vec3(0,1,0));  Ha d'anar aquí perquè giri "sobre si mateix".
  TG = glm::translate(TG, -centrePatr);
  glUniformMatrix4fv (transLoc, 1, GL_FALSE, &TG[0][0]);
}

void MyGLWidget::modelTransformTerra () {
  glm::mat4 TG(1.f);  // Matriu de transformació
  glUniformMatrix4fv (transLoc, 1, GL_FALSE, &TG[0][0]);
}

void MyGLWidget::modelTransformVaca(){
  glm::mat4 TG(1.f);  // Matriu de transformació
  TG = glm::rotate(TG, rotacio, glm::vec3(0,1,0));
  TG = glm::translate(TG, glm::vec3(1,-0.75,0));
  TG = glm::scale(TG, glm::vec3(escalaVaca, escalaVaca, escalaVaca));

  // TG = glm::rotate(TG, rotacio, glm::vec3(0,1,0));
  TG = glm::rotate(TG, -float(M_PI/2.0), glm::vec3(0,1,0));
  TG = glm::rotate(TG, - float(M_PI/2.0), glm::vec3(1,0,0));

  TG = glm::translate(TG, -centreVaca);
  glUniformMatrix4fv (transLoc, 1, GL_FALSE, &TG[0][0]);
}

void MyGLWidget::projectTransform () {
  glm::mat4 Proj;  // Matriu de projecció
  if (perspectiva)
    Proj = glm::perspective(FOV, ra, zNear, zFar);
  else
    Proj = glm::ortho(-radiEsc, radiEsc, -radiEsc, radiEsc, radiEsc, 3.0f*radiEsc);

  glUniformMatrix4fv (projLoc, 1, GL_FALSE, &Proj[0][0]);
}

void MyGLWidget::viewTransform () {
 // glm::mat4 View;  // Matriu de posició i orientació
  //View = glm::translate(glm::mat4(1.f), glm::vec3(0, 0, -2*radiEsc));

   glm::vec3 OBS(-1,1,-1);
   glm::vec3 UP(0,1,0);

   float dist = zFar - zNear;

   //zNear = sqrt(pow(OBS[1] - VRP[1], 2) + pow(OBS[0] - VRP[0],2)) -1 ;
   zNear = 0.1;

   zFar = zNear + dist;

   glm::mat4 View = glm::lookAt(OBS, VRP, UP);

  //View = glm::translate(View, -VRP);
  View = glm::rotate(View, -angleY, glm::vec3(0, 1, 0));
  View = glm::translate(View, -centreEscena);
  View = glm::rotate(View,-angleX,glm::vec3(1,0,0));

  glUniformMatrix4fv (viewLoc, 1, GL_FALSE, &View[0][0]);
}



// ------- CAPSES CONTENIDORES ----


void MyGLWidget::calculaCapsaModel () {
  // Càlcul capsa contenidora i valors transformacions inicials
  float minx, miny, minz, maxx, maxy, maxz;
  minx = maxx = patr.vertices()[0];
  miny = maxy = patr.vertices()[1];
  minz = maxz = patr.vertices()[2];
  for (unsigned int i = 3; i < patr.vertices().size(); i+=3)
  {
    if (patr.vertices()[i+0] < minx)
      minx = patr.vertices()[i+0];
    if (patr.vertices()[i+0] > maxx)
      maxx = patr.vertices()[i+0];
    if (patr.vertices()[i+1] < miny)
      miny = patr.vertices()[i+1];
    if (patr.vertices()[i+1] > maxy)
      maxy = patr.vertices()[i+1];
    if (patr.vertices()[i+2] < minz)
      minz = patr.vertices()[i+2];
    if (patr.vertices()[i+2] > maxz)
      maxz = patr.vertices()[i+2];
  }

  escala = 0.25/(maxy-miny); // El Patricio ha de ser d'alçada 0.25
  centrePatr[0] = (minx+maxx)/2.0; centrePatr[1] = (miny+maxy)/2.0; centrePatr[2] = (minz+maxz)/2.0;
}

void MyGLWidget::calculaCapsaVaca(){
    float minx, miny, minz, maxx, maxy, maxz;
    minx = maxx = vaca.vertices()[0];
    miny = maxy = vaca.vertices()[1];
    minz = maxz = vaca.vertices()[2];
    for (unsigned int i = 3; i < vaca.vertices().size(); i+=3)     {
      if (vaca.vertices()[i+0] < minx) minx = vaca.vertices()[i+0];
      if (vaca.vertices()[i+0] > maxx) maxx = vaca.vertices()[i+0];

      if (vaca.vertices()[i+1] < miny) miny = vaca.vertices()[i+1];
      if (vaca.vertices()[i+1] > maxy) maxy = vaca.vertices()[i+1];

      if (vaca.vertices()[i+2] < minz) minz = vaca.vertices()[i+2];
      if (vaca.vertices()[i+2] > maxz) maxz = vaca.vertices()[i+2];
    }

    escalaVaca = 0.5/(maxz-minz);   // Em demanen que l'alçada de la vaca sigui de 0.5
                                    // Com que està girada, he d'escalar segons les "z".

    centreVaca[0] = (minx+maxx)/2.0;
    centreVaca[1] = (miny+maxy)/2.0;
    centreVaca[2] = (minz+maxz)/2.0;

}

void MyGLWidget::calculaCapsaEscena(){

    // Hardcoded perquè sabem els valors que té l'escena.
    escenaMaxim.x = 2.0;
    escenaMinim.x = -2.0;

    escenaMaxim.y = -0.25;
    escenaMinim.y = -1.0;

    escenaMaxim.z = 2.0;
    escenaMinim.z = -2.0;

    centreEscena = glm::vec3((escenaMaxim.x + escenaMinim.x)/2, (escenaMaxim.y + escenaMinim.y)/2, (escenaMaxim.z + escenaMinim.z)/2);
    float modulX = pow(escenaMaxim.x - centreEscena.x, 2);
    float modulY = pow(escenaMaxim.y - centreEscena.y, 2);
    float modulZ = pow(escenaMaxim.z - centreEscena.z, 2);
    radiEsc = sqrt(modulX + modulY + modulZ);
}


// --- INTERACCIONS TECLAT I RATOLÍ ----


void MyGLWidget::keyPressEvent(QKeyEvent* event) {
  makeCurrent();
  switch (event->key()) {
    case Qt::Key_O: { // canvia òptica entre perspectiva i axonomètrica
      perspectiva = !perspectiva;
      projectTransform ();
      break;
    }
    case Qt::Key_R: { // canvia òptica entre perspectiva i axonomètrica
      rotacio -= M_PI/6.0;
      glm::mat4 TG(1.f);
     //glm::vec4 rotaVRP = glm::rotate(TG, float(-M_PI/6.0f), glm::vec3(0,1,0))*glm::vec4(VRP, 1.0f);
      glm::vec3 rotaVRP(-1*cos(rotacio), -0.375, -1*sin(rotacio));
      //VRP = glm::vec3(rotaVRP);
      VRP[0] = rotaVRP[0];
      VRP[1] = rotaVRP[1];
      VRP[2] = rotaVRP[2];
      viewTransform();
      //modelTransformPatricio();
      //modelTransformVaca();
      break;
    }
    case Qt::Key_X:{
      glUniform1i(teclaXLoc, teclaX);
      if(teclaX == 1) teclaX = 0;
      else teclaX = 1;
      break;
  }
    case Qt::Key_L:{
      glUniform1i(teclaLloc, teclaL);
      if(teclaL == 1) teclaL = 0;
      else teclaL = 1;
      break;
   }
    default: event->ignore(); break;
  }

  update();
}

void MyGLWidget::mousePressEvent (QMouseEvent *e) {
  xClick = e->x();
  yClick = e->y();

  if (e->button() & Qt::LeftButton &&
      ! (e->modifiers() & (Qt::ShiftModifier|Qt::AltModifier|Qt::ControlModifier))) {
    DoingInteractive = ROTATE;
  }
  if(e->button() & Qt::RightButton &&
          ! (e->modifiers() & (Qt::ShiftModifier|Qt::AltModifier|Qt::ControlModifier))) {
    DoingInteractive = ZOOM;
  }

}

void MyGLWidget::mouseReleaseEvent( QMouseEvent *) {
  DoingInteractive = NONE;
}

void MyGLWidget::mouseMoveEvent(QMouseEvent *e) {
  makeCurrent();
  // Aqui cal que es calculi i s'apliqui la rotacio o el zoom com s'escaigui...
  if (DoingInteractive == ROTATE) {
    // Fem la rotació
    angleY += (e->x() - xClick) * M_PI / 180.0;
    angleX += (e->y() - yClick) * M_PI / 180.0;

    viewTransform ();
  }

  if(DoingInteractive == ZOOM){
     float FOVauxiliar = FOV + (e->y() - yClick)/10.0;

     if(FOVauxiliar < (float) M_PI && FOVauxiliar > 0.0) {
         FOV = FOVauxiliar;

         emit actualitzaBarra((FOV/ float(M_PI))*100.0);
     }

     projectTransform();
  }
  xClick = e->x();
  yClick = e->y();

  update ();
}


// -------------------- QT ----------------


void MyGLWidget::aplicarZoom(int slider){
    makeCurrent();
    float FOVauxiliar = ((float)slider/100.0)*M_PI;
    if(FOVauxiliar < (float) M_PI && FOVauxiliar > 0.0) {
        FOV = FOVauxiliar;
        projectTransform();
    }

    update();
}


