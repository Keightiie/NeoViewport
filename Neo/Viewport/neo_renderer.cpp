#include "neo_renderer.h"

#include <QOpenGLTexture>

NeoRenderer::NeoRenderer(int t_fps, QWidget *parent) : QOpenGLWidget{parent}
{

    QSurfaceFormat l_SurfaceFormat;
    l_SurfaceFormat.setSwapInterval(1);
    l_SurfaceFormat.setSamples(4);
    setFormat(l_SurfaceFormat);

    m_TextureLoader = new TextureManager();
    m_CurrentCamera = new CameraData();
    if(t_fps != -1)
    {
        m_RenderTimer = new QTimer(this);
        connect(m_RenderTimer, &QTimer::timeout, this, &NeoRenderer::RendererLoop);
        m_RenderTimer->start(t_fps / 1000);
    }
}

void NeoRenderer::LoadSceneObject(SceneObject *t_ScnObject)
{
    m_LoadedOBJ.append(t_ScnObject);
}

void NeoRenderer::ToggleFreecam(bool t_toggle)
{
    m_IsFreecam = t_toggle;
}

void NeoRenderer::ClearDebugKeys()
{
    m_PressedKeys.clear();
}

void NeoRenderer::PressDebugKey(eDebugKeyInputs l_key)
{
    m_PressedKeys[l_key] = true;
}

void NeoRenderer::ReleaseDebugKey(eDebugKeyInputs l_key)
{
    m_PressedKeys[l_key] = false;
}

void NeoRenderer::SetCamera(CameraData *t_Camera)
{
    m_CurrentCamera = t_Camera;
}

void NeoRenderer::initializeGL()
{
    //glEnable(GL_MULTISAMPLE);
    initializeOpenGLFunctions();

    initShaders();
    initTextures();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);


    m_DebugMesh = new MeshData();
    m_DebugMesh->Initialize();
}

void NeoRenderer::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
    m_CurrentCamera->UpdateMatrix(w, h);
}

void NeoRenderer::paintGL()
{
    glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
    glClear(GL_COLOR_BUFFER_BIT);

    RendererUpdate();
}

void NeoRenderer::initShaders()
{
    if (!m_ShaderProgram.addShaderFromSourceFile(QOpenGLShader::Vertex, "base/models/shaders/vshader.glsl"))
        close();

    if (!m_ShaderProgram.addShaderFromSourceFile(QOpenGLShader::Fragment, "base/models/shaders/fshader.glsl"))
        close();

    if (!m_ShaderProgram.link())
        close();

    if (!m_ShaderProgram.bind())
        close();
}

void NeoRenderer::initTextures()
{

}

void NeoRenderer::UpdateFreeCam()
{
    QVector3D l_CameraTranslate = QVector3D(0, 0, 0);
    QVector3D l_CameraRotate = QVector3D(0, 0, 0);

    l_CameraTranslate.setZ(l_CameraTranslate.z() + (m_PressedKeys[DbgKeyForward] ? 0.01 : 0));
    l_CameraTranslate.setZ(l_CameraTranslate.z() + (m_PressedKeys[DbgKeyBack] ? -0.01 : 0));

    l_CameraTranslate.setX(l_CameraTranslate.x() + (m_PressedKeys[DbgKeyLeft] ? 0.01 : 0));
    l_CameraTranslate.setX(l_CameraTranslate.x() + (m_PressedKeys[DbgKeyRight] ? -0.01 : 0));

    l_CameraTranslate.setY(l_CameraTranslate.y() + (m_PressedKeys[DbgKeyDown] ? 0.01 : 0));
    l_CameraTranslate.setY(l_CameraTranslate.y() + (m_PressedKeys[DbgKeyUp] ? -0.01 : 0));

    l_CameraRotate.setX(l_CameraRotate.x() + (m_PressedKeys[DbgCamRotateUp] ? -1 : 0));
    l_CameraRotate.setX(l_CameraRotate.x() + (m_PressedKeys[DbgCamRotateDown] ? 1 : 0));

    l_CameraRotate.setY(l_CameraRotate.y() + (m_PressedKeys[DbgCamRotateRight] ? 1 : 0));
    l_CameraRotate.setY(l_CameraRotate.y() + (m_PressedKeys[DbgCamRotateLeft] ? -1 : 0));

    TranslateTransform(l_CameraTranslate);
    TranslateRotation(l_CameraRotate);
}

void NeoRenderer::RendererLoop()
{
    update();
}

void NeoRenderer::RenderBackground(QString l_Background)
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    m_TextureLoader->GetTexture(l_Background)->bind();

    glBegin(GL_QUADS);
    glTexCoord2f(0, 0); glVertex2f(0, 0);
    glTexCoord2f(1, 0); glVertex2f(width(), 0);
    glTexCoord2f(1, 1); glVertex2f(width(), height());
    glTexCoord2f(0, 1); glVertex2f(0, height());
    glEnd();
}


double NeoRenderer::DistanceFromCamera(const QVector3D &t_CameraPosition, const QVector3D &t_VertexPosition)
{
    return (t_CameraPosition - t_VertexPosition).lengthSquared();
}

void NeoRenderer::TranslateTransform(QVector3D t_Transform)
{
    m_CurrentCamera->GetTransform()->TranslatePosition(t_Transform);
}

void NeoRenderer::TranslateRotation(QVector3D t_Transform)
{
    m_CurrentCamera->GetTransform()->TranslateRotation(t_Transform);
}

void NeoRenderer::SetTransform(QVector3D t_Transform)
{
    m_CurrentCamera->GetTransform()->SetPosition(t_Transform);
}

void NeoRenderer::SetRotation(QVector3D t_Transform)
{
    m_CurrentCamera->GetTransform()->SetRotation(t_Transform);
}

void NeoRenderer::SetOverlay(QString t_overlay)
{
    m_OverlayImage = t_overlay;
}

void NeoRenderer::SetDebugValue(int l_dbg)
{
    m_DebugValue = l_dbg;
}

bool NeoRenderer::IsRendering()
{
    return m_LoadedOBJ.count() > 0;
}

void NeoRenderer::DisableAutoUpdates()
{
    m_AutoUpdate = false;
}

void NeoRenderer::EnableAutoUpdates()
{
    m_AutoUpdate = true;
}

void NeoRenderer::ClearViewport()
{
    m_LoadedOBJ.clear();
}

void NeoRenderer::WaitForUpdateFinish()
{
    while(m_InUpdate){}
    return;
}

void NeoRenderer::RendererUpdate()
{
    if(m_InUpdate) return;
    if(m_IsFreecam) UpdateFreeCam();

    m_InUpdate = true;
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    m_ShaderProgram.setUniformValue("mvp_matrix", m_CurrentCamera->GetProjectionMatrix());// * matrix);
    m_ShaderProgram.setUniformValue("texture", 0);

    m_CurrentCamera->UpdateCamera(width(), height());

    for(SceneObject *l_ScenObj : m_LoadedOBJ)
    {
        if(l_ScenObj == nullptr) return;
        for(MeshData *l_Mesh : l_ScenObj->m_Mesh)
        {
            l_Mesh->DrawMesh(&m_ShaderProgram, m_TextureLoader, m_DebugValue);
        }
    }

    m_InUpdate = false;
}

QVector3D NeoRenderer::GetTransform()
{
    return m_CurrentCamera->GetTransform()->GetPosition();
}

QVector3D NeoRenderer::GetRotation()
{
    return m_CurrentCamera->GetTransform()->GetRotation();
}
