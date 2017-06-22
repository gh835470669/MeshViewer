#include <GL/glew.h>
#include "openglwidget.h"
#include <QFileDialog>
#include <QDir>
#include <QOpenGLContext>
#include <glm/gtc/type_ptr.hpp>
#include "mesh.h"

OpenGLWidget::OpenGLWidget(QWidget* parent) : QOpenGLWidget(parent),
    model(0), phongShader(0), gourandShader(0), curShader(0),
    camera(glm::vec3(0.0f, 8.0f, 20.0f))
{

}

OpenGLWidget::~OpenGLWidget(){
    curShader = 0;
    delete phongShader;
    delete gourandShader;
    clearModel();
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
    //The current path is different, the path should be changed appropriately
    phongShader = new Shader("shaders/shader.vert", "shaders/shader.frag");
    gourandShader = new Shader("shaders/gourandshader.vert", "shaders/gourandshader.frag");
    if (phongShader == nullptr || gourandShader == nullptr) {
        qDebug() << "Shader is null";
    }
    glEnable(GL_DEPTH_TEST);
    glClearColor(100 / 255.0f, 100 / 255.0f, 200 / 255.0f, 1.0f);
}

void OpenGLWidget::resizeGL(int w, int h) {
    glViewport(0, 0, w, h);

    matrixProjection.setToIdentity();
    matrixModel.setToIdentity();

    matrixProjection.perspective(camera.Zoom, (float)this->width() / this->height(), 0.1f, 100.0f);
}

void OpenGLWidget::openfile()
{
    QStringList filters;
      filters << "Alias WaveFront Objects (*.obj)"
              << "Collada File Format (*.dae)"
              << "3D-Studio File Format (*.3ds)"
              << "All files (*)";
    QFileDialog fileDialog(this);
    fileDialog.setViewMode(QFileDialog::List);
    fileDialog.setDirectory(QDir::currentPath());
    fileDialog.setNameFilters(filters);

    QString modelFilename;
    if (fileDialog.exec() == QDialog::Accepted) {
        modelFilename = fileDialog.selectedFiles().at(0);
    }

    if (!modelFilename.isEmpty()) {
        clearModel();
        model = new Model(modelFilename.toStdString());
        setupModel();
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
        qDebug() << "shadingMode = GOURAUD;";
        shadingMode = GOURAUD;
    } else if (actionName.compare(tr("actionPhong")) == 0) {
        qDebug() << "shadingMode = PHONG;";
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
            //qDebug() << "shadingMode = GOURAUD;";
            break;
        case PHONG:
            curShader = phongShader;
            //qDebug() << "shadingMode = PHONG;";
            break;
        default:
            curShader = phongShader;
            break;
    }

    if (model != 0)
    {
        //update matrix
        matrixProjection.setToIdentity();
        matrixProjection.perspective(camera.Zoom, (float)this->width() / this->height(), 0.1f, 100);
        matrixModel.setToIdentity();
        matrixModel.scale(QVector3D(scaler, scaler, scaler));
        matrixModel.translate(QVector3D(transX, transY, 0.0f));
        matrixModel.rotate(rotationAroundY, QVector3D(0, 1, 0));

        curShader->Use();

        GLint lightAmbientLoc = glGetUniformLocationARB(curShader->Program, "light.ambient");
        GLint lightDiffuseLoc = glGetUniformLocationARB(curShader->Program, "light.diffuse");
        GLint lightSpecularLoc = glGetUniformLocationARB(curShader->Program, "light.specular");
        GLint lightPosLoc = glGetUniformLocationARB(curShader->Program, "light.position");
        glUniform3fARB(lightAmbientLoc, lightAmbient.x(), lightAmbient.y(), lightAmbient.z());
        glUniform3fARB(lightDiffuseLoc, lightDiffus.x(), lightDiffus.y(), lightDiffus.z());
        glUniform3fARB(lightSpecularLoc, lightSpecular.x(), lightSpecular.y(), lightSpecular.z());
        glUniform3fARB(lightPosLoc, 10.0f, 5.0f, 5.0f);

        bool texture_flag = false;
        if (textureMode == TEXTURE)
            texture_flag = true;
        else if (textureMode == COLOR)
            texture_flag = false;
        glUniform1i(glGetUniformLocationARB(curShader->Program, "texture_flag"), texture_flag);

        GLuint objectColorLoc = glGetUniformLocationARB(curShader->Program, "objectColor");
        glUniform3fARB(objectColorLoc, 1.0f, 1.0f, 1.0f);
        switch (displayMode)
        {
            case FILL:
                glPolygonMode(GL_FRONT, GL_FILL);
                drawModel();
                break;
            case FILLLINES: //draw twice : FILL and LINE
                glPolygonMode(GL_FRONT, GL_FILL);
                drawModel();

                glUniform1i(glGetUniformLocationARB(curShader->Program, "texture_flag"), false);
                glUniform3fARB(objectColorLoc, 0.0f, 0.0f, 0.0f);
                glPolygonMode(GL_FRONT, GL_LINE);
                drawModel();
                break;
            case WIREFRAME:
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                drawModel();
                break;
            default:
                break;
        }
    }

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

void OpenGLWidget::setupModel()
{
    for (size_t i = 0; i < model->meshes.size(); i++) {
        setupMesh(model->meshes.at(i));
    }
    for (size_t i = 0; i < model->textures_loaded.size(); i++) {
        setupTex(model->textures_loaded.at(i));
    }
}

void OpenGLWidget::setupMesh(Mesh &mesh)
{
    makeCurrent();

    // Create buffers/arrays
    glGenVertexArrays(1, &mesh.VAO);
    glGenBuffersARB(1, &mesh.VBO);
    glGenBuffersARB(1, &mesh.EBO);

    glBindVertexArray(mesh.VAO);

    // Load data into vertex buffers
    glBindBufferARB(GL_ARRAY_BUFFER, mesh.VBO);
    // A great thing about structs is that their memory layout is sequential for all its items.
    // The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
    // again translates to 3/2 floats which translates to a byte array.
    glBufferDataARB(GL_ARRAY_BUFFER, mesh.vertices.size() * sizeof(Vertex), &(mesh.vertices[0]), GL_STATIC_DRAW_ARB);

    glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER, mesh.EBO);
    glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER, mesh.indices.size() * sizeof(GLuint), &(mesh.indices[0]), GL_STATIC_DRAW_ARB);

    // Set the vertex attribute pointers

    // Vertex Positions
    glEnableVertexAttribArrayARB(0);
    glVertexAttribPointerARB(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, Position));
    // Vertex Normals
    glEnableVertexAttribArrayARB(1);
    glVertexAttribPointerARB(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, Normal));
    // Vertex Texture Coords
    glEnableVertexAttribArrayARB(2);
    glVertexAttribPointerARB(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, TexCoords));

    glBindVertexArray(0);

    auto e = glGetError();
    if (e != GL_NO_ERROR) {
        qDebug() << e;
    } else {
        qDebug() << __FILE__ << __FUNCTION__ << " NO ERROR";
    }
}

void OpenGLWidget::drawModel()
{
    GLint viewLoc = glGetUniformLocationARB(curShader->Program, "view");
    GLint view_invPosLoc = glGetUniformLocationARB(curShader->Program, "view_inv");
    GLuint projectionLoc = glGetUniformLocationARB(curShader->Program, "projection");
    GLuint modelLoc = glGetUniformLocationARB(curShader->Program, "model");
    glUniformMatrix4fvARB(viewLoc, 1, GL_FALSE,  glm::value_ptr(camera.GetViewMatrix()));
    glUniformMatrix4fvARB(view_invPosLoc, 1, GL_FALSE, glm::value_ptr(glm::inverse(camera.GetViewMatrix())));
    glUniformMatrix4fvARB(projectionLoc, 1, GL_FALSE,  matrixProjection.data());
    glUniformMatrix4fvARB(modelLoc, 1, GL_FALSE,  matrixModel.data());

    GLint flat_flagLoc = glGetUniformLocationARB(curShader->Program, "flat_flag");
    glUniform1ui(flat_flagLoc, flat_flag);

    for (size_t i = 0; i < model->meshes.size(); i++) {
        // Bind appropriate textures
        GLuint diffuseNr = 1;
        GLuint specularNr = 1;
        for(GLuint j = 0; j < model->meshes.at(i).textures.size(); j++)
        {
            GLuint index = model->meshes.at(i).textures.at(j);
            glActiveTextureARB(GL_TEXTURE0 + j); // Active proper texture unit before binding

            // Retrieve texture number (the N in diffuse_textureN)
            stringstream ss;
            string number;
            string name = model->textures_loaded.at(index).type;
            if(name == "texture_diffuse")
                ss << diffuseNr++; // Transfer GLuint to stream
            else if(name == "texture_specular")
                ss << specularNr++; // Transfer GLuint to stream
            number = ss.str();
            // Now set the sampler to the correct texture unit
            glUniform1iARB(glGetUniformLocationARB(curShader->Program, (name + number).c_str()), j);
            // And finally bind the texture
            glBindTexture(GL_TEXTURE_2D, model->textures_loaded.at(index).id);

        }
        //Also set each mesh's shininess property to a default value (if you want you could extend this to another mesh property and possibly change this value)
        glUniform1fARB(glGetUniformLocationARB(curShader->Program, "shininess"), 32.0f);

        glBindVertexArray(model->meshes.at(i).VAO);
        glDrawElements(GL_TRIANGLES, model->meshes.at(i).indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        // Always good practice to set everything back to defaults once configured.
        for (GLuint i = 0; i < model->meshes.at(i).textures.size(); i++)
        {
            glActiveTextureARB(GL_TEXTURE0 + i);
            glBindTexture(GL_TEXTURE_2D, 0);
        }
    }

}

void OpenGLWidget::clearModel()
{
    if (model == 0) {
        return;
    }

    makeCurrent();
    for (size_t i = 0; i < model->meshes.size(); i++) {
        glDeleteBuffersARB(1, &(model->meshes.at(i).VBO));
        glDeleteBuffersARB(1, &(model->meshes.at(i).EBO));
        glDeleteVertexArrays(1, &(model->meshes.at(i).VAO));
    }
    for (size_t i = 0; i < model->textures_loaded.size(); i++) {
        glDeleteTextures(1, &(model->textures_loaded.at(i).id));
    }
    doneCurrent();
}

void OpenGLWidget::setupTex(Texture& texture)
{
    texture.id = textureFromFile(texture.path, model->directory);
}

uint OpenGLWidget::textureFromFile(string path, string directory)
{
    makeCurrent();
     //Generate texture ID and load texture data
    string filename = path;
    filename = directory + '/' + filename;
    GLuint textureID;
    glGenTextures(1, &textureID);
    int width,height;
    unsigned char* image = SOIL_load_image(filename.c_str(), &width, &height, 0, SOIL_LOAD_RGB);
    // Assign texture to ID
    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    glGenerateMipmap(GL_TEXTURE_2D);

    // Parameters
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
    SOIL_free_image_data(image);

    auto e = glGetError();
    if (e != GL_NO_ERROR) {
        qDebug()   << "e "  << e << __FUNCTION__  << " INVALID_OPERATION";
    }

    return textureID;
}
