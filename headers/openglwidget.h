#ifndef OPENGLWIDGET_H
#define OPENGLWIDGET_H

#include "model.h"
#include <QOpenGLWidget>
#include <QAction>
#include <QMatrix4x4>
#include <QKeyEvent>
#include <QMouseEvent>
#include "shader.h"
#include "camera.h"

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

    Model* model;
    Shader* phongShader;
    Shader* gourandShader;
    Shader* curShader;

    QVector3D lightAmbient = QVector3D(0.1f, 0.1f, 0.1f);
    QVector3D lightDiffus = QVector3D(1.0f, 1.0f, 1.0f);
    QVector3D lightSpecular= QVector3D(1.0f, 1.0f, 1.0f);

    void setupModel();
    void setupMesh(Mesh& mesh);
    void drawModel();
    void clearModel();
    void setupTex(Texture& texture);
    uint textureFromFile(string path, string directory);

public slots:
    void openfile();
    void onDisplayModeChanged(QAction *mode);
    void onShadingModeChanged(QAction *mode);
    void onFlatShadingModeChanged(QAction *mode);
    void onTextureModeChanged(QAction *mode);
};

#endif // OPENGLWIDGET_H
