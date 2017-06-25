#ifndef OPENGLWIDGET_H
#define OPENGLWIDGET_H

#include <GL/glew.h>
#include <glm/gtc/type_ptr.hpp>

#include <QOpenGLWidget>
#include <QAction>
#include <QMatrix4x4>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QFileDialog>
#include <QDir>
#include <QOpenGLContext>

#include "mesh.h"
#include "shaderprogram.h"
#include "camera.h"
#include "light.h"
#include "gameobject.h"

using namespace makai;

class OpenGLWidget : public QOpenGLWidget
{
public:
    OpenGLWidget(QWidget *parent = 0);
    ~OpenGLWidget();

    //modes
    enum DISPLAYMODE {
        FILL,
        FILLLINES,
        WIREFRAME
    } displayMode = FILL;

    enum SHADINGMODE {
        PHONG,
        GOURAUD
    } shadingMode = GOURAUD;

    enum TEXTUREMODE {
        TEXTURE,
        COLOR
    } textureMode = COLOR;

    bool flat_flag = true;
protected:
    //Qt OpenGL functions
    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();

    //control varibles for event
    bool isControlPressing = false;
    bool isAltPressing = false;
    bool isFirstMouseClick = true;
    int lastX = 0, lastY = 0;
    int mouseButton;

    //input events
    void keyPressEvent(QKeyEvent* event);
    void keyReleaseEvent(QKeyEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void mousePressEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);
    void wheelEvent(QWheelEvent* event);


    float t = 0.0f;
private:
    const GLfloat transSensitivity = 0.05f;
    const GLfloat rotSensitivity = 0.5f;
    const GLfloat scaSensitivity = 0.05f;
    GLfloat scaler = 1.0f;
    GLfloat transX = 0.0f;
    GLfloat transY = 0.0f;
    GLfloat rotationAroundY = 0.0f;

    QMatrix4x4 matrixProjection;
    QMatrix4x4 matrixModel;
    Camera camera;

    Mesh mesh;
    ShaderProgram* phongShader;
    ShaderProgram* gourandShader;
    ShaderProgram* curShader;

    QVector3D lightAmbient = QVector3D(0.3f, 0.3f, 0.3f);
    std::vector<Light> lights;

    void uploadMatrices();

    void setBuiltInObject();
    std::vector<Mesh*> builtInMeshes;
    std::vector<GameObject*> builtInObjects;

    const char* lightVertShaderSource =
            "#version 330 core\n"
            "layout (location = 0) in vec3 aPos;\n"
            "uniform mat4 model;\n"
            "uniform mat4 view;\n"
            "uniform mat4 projection;\n"
            "void main() {\n"
            "    gl_Position = projection * view * model * vec4(aPos, 1.0);\n"
            "}\n";

    const char* lightFraShaderSource =
        "#version 330 core\n"
        "out vec4 FragColor;\n"
        "void main()\n"
        "{\n"
        "    FragColor = vec4(1.0);\n"
        "}\n";
    ShaderProgram* lightProgram;
    unsigned lightVAO = 0;
    void initLightVAO(const std::vector<float> &v);
    void paintLights();
public slots:
    void openfile();
    void onDisplayModeChanged(QAction *mode);
    void onShadingModeChanged(QAction *mode);
    void onFlatShadingModeChanged(QAction *mode);
    void onTextureModeChanged(QAction *mode);
};

#endif // OPENGLWIDGET_H
