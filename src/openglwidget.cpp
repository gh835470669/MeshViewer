#include "openglwidget.h"


OpenGLWidget::OpenGLWidget(QWidget* parent) : QOpenGLWidget(parent),
    phongShader(0), gourandShader(0), curShader(0), lightProgram(0),
    camera(glm::vec3(0.0f, 0.0f, 6.0f)),
    lights(), builtInMeshes(), builtInObjects()
{
    Light light;
    light.setPosition(2.0f, 2.0f, 2.0f);
    lights.push_back(light);

}

OpenGLWidget::~OpenGLWidget() {
    makeCurrent();

    curShader = 0;
    delete phongShader;
    delete gourandShader;
    delete lightProgram;

    for (unsigned i = 0; i< builtInMeshes.size(); i++)
        delete builtInMeshes.at(i);

    for (unsigned i = 0; i < builtInObjects.size(); i++)
        delete builtInObjects.at(i);

    doneCurrent();
}

void OpenGLWidget::initializeGL() {
#ifdef _DEBUG
    qDebug()<< QDir::currentPath();\
    qDebug("OpenGL Version min %d", context()->format().version().first);\
    qDebug("OpenGL Version major %d", context()->format().version().second);\
    qDebug("OpenGL profile %d", context()->format().profile());
#endif

    glewExperimental = true;
    GLenum err = glewInit();
#ifdef _DEBUG
    if (GLEW_OK != err){
        qDebug("GLEW Error: %s\n", glewGetErrorString(err));
    }
#endif

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

    lightProgram = new ShaderProgram();
    if (!lightProgram->addShaderFromSourceCode(Shader::Vertex, lightVertShaderSource))
        qDebug() << lightProgram->log().data();
    if (!lightProgram->addShaderFromSourceCode(Shader::Fragment, lightFraShaderSource))
        qDebug() << lightProgram->log().data();
    if (!lightProgram->link())
        qDebug() << lightProgram->log().data();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);
    glClearColor(100 / 255.0f, 100 / 255.0f, 200 / 255.0f, 1.0f);

    //for debug
    // sphere.dae nanosuit/nanosuit.obj
    Mesh* mesh = new Mesh();
    GameObject* gameObject = new GameObject();
    //mesh->loadModelFromFile((QDir::currentPath() + "/models/nanosuit/nanosuit.obj").toStdString());
    //mesh->genBuffers();
    gameObject->setMesh(mesh);
    builtInMeshes.push_back(mesh);
    builtInObjects.push_back(gameObject);

    setBuiltInObject();
}

void OpenGLWidget::resizeGL(int w, int h) {
    glViewport(0, 0, w, h);

    matrixProjection.setToIdentity();
    matrixModel.setToIdentity();

    matrixProjection.perspective(camera.fiewOfView, (float)this->width() / this->height(), 0.1f, 100.0f);
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
        builtInMeshes.at(0)->clear();
        builtInMeshes.at(0)->loadModelFromFile(modelFilename.toStdString());
        builtInMeshes.at(0)->genBuffers();
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

    paintLights();

    curShader->bind();

    uploadMatrices();

    //upload ambient
    curShader->setUniform("ambientLight", lightAmbient.x(), lightAmbient.y(),
                          lightAmbient.z());

    //texture or color
    bool texture_flag = false;
    if (textureMode == TEXTURE)
        texture_flag = true;
    else if (textureMode == COLOR)
        texture_flag = false;

    curShader->setUniform("texture_flag", texture_flag);
    curShader->setUniform("material.diffuse", 1.0f, 1.0f, 1.0f);

    //flat or smooth
    curShader->setUniform("flat_flag", flat_flag);


    //fill, wireFrame or fillLine
    switch (displayMode)
    {
        case FILL:
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            for (unsigned i = 0; i < builtInObjects.size(); i++)
            {
                builtInObjects.at(i)->setShaderProgram(curShader);
                builtInObjects.at(i)->paint(lights);
            }
            break;
        case FILLLINES: //draw twice : FILL and LINE
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            for (unsigned i = 0; i < builtInObjects.size(); i++)
            {
                builtInObjects.at(i)->setShaderProgram(curShader);
                builtInObjects.at(i)->paint(lights);
            }

            curShader->setUniform("texture_flag", false);
            curShader->setUniform("material.diffuse", 0.0f, 0.0f, 0.0f);
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            for (unsigned i = 0; i < builtInObjects.size(); i++)
            {
                builtInObjects.at(i)->paint(lights);
            }
            break;
        case WIREFRAME:
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            for (unsigned i = 0; i < builtInObjects.size(); i++)
            {
                builtInObjects.at(i)->setShaderProgram(curShader);
                builtInObjects.at(i)->paint(lights);
            }
            break;
        default:
            break;
    }
    curShader->release();

    update();
}

void OpenGLWidget::keyPressEvent(QKeyEvent* event)
{
    const int deltaTime = 1;

    switch (event->key())
    {
    case Qt::Key_Up:
    case Qt::Key_Space:
        camera.move(Camera::UP, deltaTime);
        break;
    case Qt::Key_Down:
    case Qt::Key_Control:
        camera.move(Camera::DOWN, deltaTime);
        break;
    case Qt::Key_W:
        camera.move(Camera::FORWARD, deltaTime);
        break;
    case Qt::Key_S:
        camera.move(Camera::BACKWARD, deltaTime);
        break;
    case Qt::Key_A:
        camera.move(Camera::LEFT, deltaTime);
        break;
    case Qt::Key_D:
        camera.move(Camera::RIGHT, deltaTime);
        break;
    default:
        return;
    }
    event->accept();
}

void OpenGLWidget::keyReleaseEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape)
    {
        isCaptureAllEvent = false;
        this->setCursor(Qt::ArrowCursor);
    }

}

void OpenGLWidget::mouseMoveEvent(QMouseEvent *event)
{
    //event->button() in mouseMoveEvent is always  Qt::NoButton
    if (mouseButton == Qt::LeftButton)
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

        camera.rotate(yoffset * camRotSensitivity, xoffset * camRotSensitivity);
        event->accept();
    }
//    else if (isControlPressing && mouseButton == Qt::RightButton)
//    {
//        if (isFirstMouseClick) {
//            lastX = event->x();
//            isFirstMouseClick = false;
//        }

//        int xoffset = event->x() - lastX;

//        lastX = event->x();

//        rotationAroundY += xoffset * rot1Sensitivity;

//        event->accept();
//    }

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
//    if (isControlPressing) {
//        scaler *= (1 - event->angleDelta().y() / 120 * scaSensitivity);
//        qDebug() << scaler;
//        event->accept();
//    }
}

bool OpenGLWidget::eventFilter(QObject *object, QEvent *event)
{
    if (isCaptureAllEvent == true)
        return true;
    else
        return false;
}

void OpenGLWidget::uploadMatrices()
{
    //update matrix
    matrixProjection.setToIdentity();
    matrixProjection.perspective(camera.fiewOfView, (float)this->width() / this->height(), 0.1f, 100);
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

void OpenGLWidget::setBuiltInObject()
{
    float vertices[] = {
        // positions          // normals           // texture coords
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
    };

    int size = sizeof(vertices) / sizeof(float);
    std::vector<float> v(vertices, vertices + size);
    unsigned indSize = size / 8;
    std::vector<unsigned> indices(indSize);
    for (unsigned i = 0; i < indSize; i++) indices[i] = i;
    std::vector<unsigned> texIndices;
    texIndices.push_back(0);
    texIndices.push_back(1);
    SubMesh sm(v, indices, texIndices, 8);

    initLightVAO(v);

    Mesh* m = new Mesh();
    m->addSubMesh(sm);
    Texture t;
    t.fileName = "models/textures/container2.png";
    t.type = TextureType::diffuse;
    m->addTexture(t);
    t.fileName = "models/textures/container2_specular.png";
    t.type = TextureType::specular;
    m->addTexture(t);
    builtInMeshes.push_back(m);

    for(unsigned i = 0; i < builtInMeshes.size(); i++)
        builtInMeshes.at(i)->genBuffers();


    // positions all containers
    glm::vec3 cubePositions[] = {
        glm::vec3( 0.0f,  0.0f,  0.0f),
        glm::vec3( 2.0f,  5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3( 2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f,  3.0f, -7.5f),
        glm::vec3( 1.3f, -2.0f, -2.5f),
        glm::vec3( 1.5f,  2.0f, -2.5f),
        glm::vec3( 1.5f,  0.2f, -1.5f),
        glm::vec3(-1.3f,  1.0f, -1.5f)
    };


    for (unsigned int i = 0; i < 10; i++)
    {
        GameObject* gameObject = new GameObject();
        gameObject->setMesh(builtInMeshes.back());
        gameObject->setPosition(cubePositions[i]);
        float angle = 20.0f * i;
        gameObject->rotate(angle, glm::vec3(1.0f, 0.3f, 0.5f));
        builtInObjects.push_back(gameObject);
    }


}

void OpenGLWidget::initLightVAO(const std::vector<float> &v)
{
    unsigned int VBO;
    glGenVertexArrays(1, &lightVAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(lightVAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, v.size() * sizeof(float), &v[0], GL_STATIC_DRAW);

    // note that we update the lamp's position attribute's stride to reflect the updated buffer data
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
}

void OpenGLWidget::paintLights()
{
    lightProgram->bind();
    lightProgram->setUniformMatrix4("projection", matrixProjection.data(), 1, GL_FALSE);
    lightProgram->setUniform("view", camera.GetViewMatrix());
    glBindVertexArray(lightVAO);
    for (unsigned i = 0 ; i < lights.size(); i++) {
        glm::mat4 model;
        model = glm::translate(model, glm::vec3(lights.at(i).position()));
        model = glm::scale(model, glm::vec3(1.0f)); // a smaller cube
        lightProgram->setUniform("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }

    glBindVertexArray(0);
    lightProgram->release();
}


