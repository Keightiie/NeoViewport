#include "neo_renderer.h"

#include <QOpenGLTexture>

NeoRenderer::NeoRenderer(int t_fps, QWidget *parent) : QOpenGLWidget{parent}
{

    //QSurfaceFormat l_SurfaceFormat;
    //l_SurfaceFormat.setSwapInterval(1);
    //l_SurfaceFormat.setSamples(4);
    //setFormat(l_SurfaceFormat);

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
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);
}

void NeoRenderer::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
}

void NeoRenderer::paintGL()
{
    RendererUpdate();
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
    //Should probably call update here.
    update();
}


void NeoRenderer::RenderMesh(MeshData *l_Mesh)
{
    QList<FaceData *> l_FacesNoAlpha = l_Mesh->getFacesFiltered(false);
    QList<FaceData *> l_FacesAlpha = l_Mesh->getFacesFiltered(true);

    //std::sort(l_FacesAlpha.begin(), l_FacesAlpha.end(), [&](FaceData * a, FaceData * b)
    //{
    //    return DistanceFromCamera(m_CameraTransform, a->GetCenter()) > DistanceFromCamera(m_CameraTransform, b->GetCenter());
    //});

    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);

    // Render opaque faces first
    RenderFaces(l_Mesh, l_FacesNoAlpha);

    // Render transparent faces
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    RenderFaces(l_Mesh, l_FacesAlpha);
    glDisable(GL_BLEND);


    glDisable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);

}

void NeoRenderer::RenderFaces(MeshData *l_Mesh, QList<FaceData *> l_Faces)
{
    MaterialData * l_LastMaterial = nullptr;

    int l_FaceType = (l_Mesh->getFaceType() == faceTri) ? GL_TRIANGLES : GL_QUADS;

    glBegin(l_FaceType);

    for(FaceData *l_Vertex : l_Faces)
    {
        MaterialData * l_NewMaterial = l_Vertex->getMaterial();
        if(l_LastMaterial != l_NewMaterial && l_NewMaterial != nullptr)
        {
            l_LastMaterial = l_NewMaterial;
            glEnd();

            QOpenGLTexture *l_Texture = m_TextureLoader->GetTexture(l_NewMaterial->getTexturePath());
            if(l_Texture != nullptr) l_Texture->bind();
            else glBindTexture(GL_TEXTURE_2D, 0);

            glBegin(l_FaceType);

        }
        bool l_RenderTexCords = l_Vertex->GetTexCords().count() == l_Vertex->GetVerticies().count();
        for(int i = 0; i < l_Vertex->GetVerticies().count(); i++)
        {
            if(l_RenderTexCords)
            {
                glTexCoord2f(l_Vertex->GetTexCords().at(i).x(), l_Vertex->GetTexCords().at(i).y());
            }
            glVertex3f(l_Vertex->GetVerticies().at(i).x(), l_Vertex->GetVerticies().at(i).y(), l_Vertex->GetVerticies().at(i).z());
        }

    }

    glEnd();
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

bool NeoRenderer::WithinFOV(QVector3D l_Object)
{
    //This function doesn't currently behave as intended. Will redo it eventually.
    QVector3D directionToObject = l_Object - m_CameraTransform;

    QVector3D l_NormForward = m_CameraForward.normalized();
    QVector3D l_NormDirection = directionToObject.normalized();

    float dotProduct = QVector3D::dotProduct(l_NormForward, l_NormDirection);

    if (dotProduct >= 0)
    {
        return true;
    }
    return false;
}

double NeoRenderer::DistanceFromCamera(const QVector3D &t_CameraPosition, const QVector3D &t_VertexPosition)
{
    return (t_CameraPosition - t_VertexPosition).lengthSquared();
}

void NeoRenderer::TranslateTransform(QVector3D t_Transform)
{
    m_CameraTransform += t_Transform;
}

void NeoRenderer::TranslateRotation(QVector3D t_Transform)
{
    m_CameraRotation += t_Transform;
}

void NeoRenderer::SetTransform(QVector3D t_Transform)
{
    m_CameraTransform = t_Transform;
}

void NeoRenderer::SetRotation(QVector3D t_Transform)
{
    m_CameraRotation = t_Transform;
}

void NeoRenderer::SetOverlay(QString t_overlay)
{
    m_OverlayImage = t_overlay;
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

    //CameraSetPerspective();
    m_CurrentCamera->UpdateCamera(width(), height());

    glRotatef(m_CameraRotation.x(), 1.0f, 0.0f, 0.0f);
    glRotatef(m_CameraRotation.y(), 0.0f, 1.0f, 0.0f);
    glRotatef(m_CameraRotation.z(), 0.0f, 0.0f, 1.0f);
    glTranslatef(m_CameraTransform.x(), m_CameraTransform.y(), m_CameraTransform.z());


    for(SceneObject *l_ScenObj : m_LoadedOBJ)
    {
        if(l_ScenObj == nullptr) return;
        for(MeshData *l_Mesh : l_ScenObj->m_Mesh)
        {
            if(l_Mesh == nullptr) return;
            glPushMatrix();

            glTranslatef(l_ScenObj->m_Transform.x(), l_ScenObj->m_Transform.y(), l_ScenObj->m_Transform.z());

            if(l_ScenObj->m_IsBillboard)
                glRotatef(-m_CameraRotation.y(), 0.0f, 1.0f, 0.0f);
            else
            {
                glRotatef(l_ScenObj->m_Rotation.x(), 1.0f, 0.0f, 0.0f);
                glRotatef(l_ScenObj->m_Rotation.y(), 0.0f, 1.0f, 0.0f);
                glRotatef(l_ScenObj->m_Rotation.z(), 0.0f, 0.0f, 1.0f);
            }

            RenderMesh(l_Mesh);

            glPopMatrix();
        }
    }

    m_CurrentCamera->CameraCleanup();

    //Replace this with a user-interface system.
    m_CurrentCamera->UpdateCameraOrtho(width(), height());

    if(!m_OverlayImage.trimmed().isEmpty()) RenderBackground(m_OverlayImage);

    m_CurrentCamera->CameraCleanupOrtho();
    m_InUpdate = false;
}

QVector3D NeoRenderer::GetTransform()
{
    return m_CameraTransform;
}

QVector3D NeoRenderer::GetRotation()
{
    return m_CameraRotation;
}
