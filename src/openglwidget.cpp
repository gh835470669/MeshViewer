#include "openglwidget.h"


OpenGLWidget::OpenGLWidget(QWidget* parent) : QOpenGLWidget(parent),
    phongShader(0), gourandShader(0), curShader(0),
    camera(glm::vec3(0.0f, 8.0f, 20.0f))
{

}

OpenGLWidget::~OpenGLWidget(){
    makeCurrent();

    curShader = 0;
    delete phongShader;
    delete gourandShader;
    mesh.deleteBuffers();

    doneCurrent();
}

void OpenGLWidget::initializeGL() {
    qDebug()<< QDir::currentPath();

    qDebug("%d", context()->format().version().first);
    qDebug("%d", context()->format().version().second);
    qDebug("%d", context()->format().profile());

    glewExperimental = true;
    GLenum err = glewInit();
    if (GLEW_OK != err){
        qDebug("GLEW Error: %s\n", glewGetErrorString(err));
    }


    phongShader = new ShaderProgram();
    if (!phongShader->addShaderFromFile(Shader::Vertex, "shaders/shader.vert"))
        qDebug() << phongShader->log().data();
    if (!phongShader->addShaderFromFile(Shader::Fragment, "shaders/shader.frag"))
        qDebug() << phongShader->log().data();
    if (!phongShader->link())
        qDebug() << phongShader->log().data();

    gourandShader = new ShaderProgram();
    if (!gourandShader->addShaderFromFile(Shader::Vertex, "shaders/gourandshader.vert"))
        qDebug() << gourandShader->log().data();
    if (! gourandShader->addShaderFromFile(Shader::Fragment, "shaders/gourandshader.frag"))
        qDebug() << gourandShader->log().data();
    if (!gourandShader->link())
        qDebug() << gourandShader->log().data();

    glEnable(GL_DEPTH_TEST);
    glClearColor(100 / 255.0f, 100 / 255.0f, 200 / 255.0f, 1.0f);

    //for debug
    mesh.loadModelFromFile((QDir::currentPath() + "/models/nanosuit/nanosuit.obj").toStdString());
    mesh.genBuffers();
}

void OpenGLWidget::resizeGL(int w, int h) {
    glViewport(0, 0, w, h);

    matrixProjection.setToIdentity();
    matrixModel.setToIdentity();

    matrixProjection.perspective(camera.Zoom, (float)this->width() / this->height(), 0.1f, 100.0f);
}

void OpenGLWidget::openfile()
{

    QFileDialog fileDialog(this, tr("Select A Model File"));
    fileDialog.setViewMode(QFileDialog::List);
    fileDialog.setDirectory(QDir::currentPath() + "/models");
    QStringList filters;
    filters << "All Model files (*.obj *.dae)"
            << "Alias WaveFront Objects (*.obj)"
            << "Collada File Format (*.dae)";
    fileDialog.setNameFilters(filters);

    QString modelFilename;
    if (fileDialog.exec() == QDialog::Accepted) {
        modelFilename = fileDialog.selectedFiles().at(0);
    }

    if (!modelFilename.isEmpty()) {
        makeCurrent();
        mesh.clear();
        mesh.loadModelFromFile(modelFilename.toStdString());
        mesh.genBuffers();
    }
}

void OpenGLWidget::onDisplayModeChanged(QAction *mode)
{
    QString actionName = mode->objectName();
    if (actionName.compare(tr("actionFill")) == 0) {
        displayMode = FILL;
    } else if (actionName.compare(tr("actionFilllines")) == 0) {
        displayMode = FILLLINES;
    } else if (actionName.compare(tr("actionWireframe")) == 0) {
        displayMode = WIREFRAME;
    } else {
        displayMode = FILL;
    }
}

void OpenGLWidget::onShadingModeChanged(QAction *mode)
{
    QString actionName = mode->objectName();
    if (actionName.compare(tr("actionGouraud")) == 0) {
        shadingMode = GOURAUD;
    } else if (actionName.compare(tr("actionPhong")) == 0) {
        shadingMode = PHONG;
    } else {
        shadingMode = PHONG;
    }
}

void OpenGLWidget::onFlatShadingModeChanged(QAction *mode)
{
    QString actionName = mode->objectName();
    if (actionName.compare(tr("actionFlat")) == 0)
        flat_flag = true;
    else if (actionName.compare(tr("actionSmooth")) == 0)
        flat_flag = false;
    else
        flat_flag = true;
}

void OpenGLWidget::onTextureModeChanged(QAction *mode)
{
    QString actionName = mode->objectName();
    if (actionName.compare(tr("actionTexture")) == 0)
        textureMode = TEXTURE;
    else if (actionName.compare(tr("actionColor")) == 0)
        textureMode = COLOR;
    else
        textureMode = COLOR;
}

void OpenGLWidget::paintGL() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    switch (shadingMode)
    {
        case GOURAUD:
            curShader = gourandShader;
            break;
        case PHONG:
            curShader = phongShader;
            break;
        default:
            curShader = phongShader;
            break;
    }

    curShader->bind();

    uploadMatrices();

    //texture or color
    bool texture_flag = false;
    if (textureMode == TEXTURE)
        texture_flag = true;
    else if (textureMode == COLOR)
        texture_flag = false;

    curShader->setUniform("texture_flag", texture_flag);
    curShader->setUniform("objectColor", 1.0f, 1.0f, 1.0f);

    //flat or smooth
    curShader->setUniform("flat_flag", flat_flag);

    Light light;
    //fill, wireFrame or fillLine
    switch (displayMode)
    {
        case FILL:
            glPolygonMode(GL_FRONT, GL_FILL);
            mesh.paint(curShader, &light);
            break;
        case FILLLINES: //draw twice : FILL and LINE
            glPolygonMode(GL_FRONT, GL_FILL);
            mesh.paint(curShader, &light);

            curShader->setUniform("texture_flag", false);
            curShader->setUniform("objectColor", 0.0f, 0.0f, 0.0f);
            glPolygonMode(GL_FRONT, GL_LINE);
            mesh.paint(curShader, &light);
            break;
        case WIREFRAME:
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            mesh.paint(curShader, &light);
            break;
        default:
            break;
    }

    curShader->release();

    update();
}

void OpenGLWidget::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Control)
    {
        qDebug() << "Qt::Key_Control Pressed in" << this->objectName();
        isControlPressing = true;
        event->accept();
    }
    else if (event->key() == Qt::Key_Alt)
    {
        isAltPressing = true;
        event->accept();
    }
}

void OpenGLWidget::keyReleaseEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Control)
    {
        qDebug() << "Qt::Key_Control Released in" << this->objectName();
        isControlPressing = false;
        event->accept();
    }
    else if (event->key() == Qt::Key_Alt)
    {
        isAltPressing = false;
        event->accept();
    }
}

void OpenGLWidget::mouseMoveEvent(QMouseEvent *event)
{

    if (isControlPressing && mouseButton == Qt::LeftButton) //event->button() in mouseMoveEvent is always  Qt::NoButton
    {
        if (isFirstMouseClick) {
            lastX = event->x();
            lastY = event->y();
            isFirstMouseClick = false;
        }

        int xoffset = event->x() - lastX;
        int yoffset = lastY - event->y();  // Reversed since y-coordinates go from bottom to left

        lastX = event->x();
        lastY = event->y();

        transX += xoffset * transSensitivity;
        transY += yoffset * transSensitivity;

        event->accept();
    }
    else if (isControlPressing && mouseButton == Qt::RightButton)
    {
        if (isFirstMouseClick) {
            lastX = event->x();
            isFirstMouseClick = false;
        }

        int xoffset = event->x() - lastX;

        lastX = event->x();

        rotationAroundY += xoffset * rotSensitivity;

        event->accept();
    }
}

void OpenGLWidget::mousePressEvent(QMouseEvent *event)
{
    qDebug() << "Mouse Press in" << this->objectName();
    mouseButton = event->button();

    event->accept();
}

void OpenGLWidget::mouseReleaseEvent(QMouseEvent *event)
{
    qDebug() << "Mouse Release in" << this->objectName();
    isFirstMouseClick = true;
    event->accept();
}

void OpenGLWidget::wheelEvent(QWheelEvent *event)
{
    //my mouse y is 120 or -120
    if (isControlPressing) {
        scaler *= (1 - event->angleDelta().y() / 120 * scaSensitivity);
        qDebug() << scaler;
        event->accept();
    }
}

void OpenGLWidget::uploadMatrices()
{
    //update matrix
    matrixProjection.setToIdentity();
    matrixProjection.perspective(camera.Zoom, (float)this->width() / this->height(), 0.1f, 100);
    matrixModel.setToIdentity();
    matrixModel.scale(QVector3D(scaler, scaler, scaler));
    matrixModel.translate(QVector3D(transX, transY, 0.0f));
    matrixModel.rotate(rotationAroundY, QVector3D(0, 1, 0));

    //upload matrix
    curShader->setUniformMatrix4("view", glm::value_ptr(camera.GetViewMatrix()), 1, GL_FALSE);
    curShader->setUniformMatrix4("view_inv", glm::value_ptr(glm::inverse(camera.GetViewMatrix())), 1, GL_FALSE);
    curShader->setUniformMatrix4("projection", matrixProjection.data(), 1, GL_FALSE);
    curShader->setUniformMatrix4("model", matrixModel.data(), 1, GL_FALSE);
}


