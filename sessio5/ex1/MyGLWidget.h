#define GLM_FORCE_RADIANS
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLWidget>
#include <QOpenGLShader>
#include <QOpenGLShaderProgram>
#include <QKeyEvent>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "model.h"

class MyGLWidget : public QOpenGLWidget, protected QOpenGLFunctions_3_3_Core 
{
  Q_OBJECT

  public:
    MyGLWidget (QWidget *parent=0);
    ~MyGLWidget ();

  protected:
    // initializeGL - Aqui incluim les inicialitzacions del contexte grafic.
    virtual void initializeGL ( );
    // paintGL - Mètode cridat cada cop que cal refrescar la finestra.
    // Tot el que es dibuixa es dibuixa aqui.
    virtual void paintGL ( );
    // resizeGL - És cridat quan canvia la mida del widget
    virtual void resizeGL (int width, int height);
    // keyPressEvent - Es cridat quan es prem una tecla
    virtual void keyPressEvent (QKeyEvent *event);

  private:
    void createBuffers ();
    void carregaShaders ();
    void modelTransform ();

    // --------------------------------
    // ----- LES MEVES FUNCIONS -------
    // --------------------------------

    void projectTransform();
    void viewTransform();
    void iniCamera();
    void carregarModel();
    void pintaHomer();
    void pintaTerra();
    void carregaTerra();
    void modelTransformTerra();


    GLuint projLoc, viewLoc;
    glm::vec3 VRP, OBS, UP;
    float ra, zNear, zFar, FOV;

    Model homer;
    GLuint VAOhomer, VBOhomerVertex, VBOhomerMaterial;

    float rotacio;

    GLuint VAOterra, VBOterraVertex, VBOterraColor;


    // --------------------------------

    // attribute locations
    GLuint vertexLoc, colorLoc;
    // uniform locations
    GLuint transLoc;
    // VAO i VBO names
    //GLuint VAO_Casa, VBO_CasaPos, VBO_CasaCol;
    // Program
    QOpenGLShaderProgram *program;
    // Internal vars
    float scale;
    glm::vec3 pos;
};

