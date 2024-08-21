#include "neo_renderer.h"

#include <QOpenGLTexture>

NeoRenderer::NeoRenderer(QWidget *parent) : QOpenGLWidget{parent}
{
    m_TextureLoader = new TextureManager();

    m_RenderTimer = new QTimer(this);
    connect(m_RenderTimer, &QTimer::timeout, this, &NeoRenderer::RendererLoop);
    m_RenderTimer->start(33);
}

void NeoRenderer::LoadSceneObject(SceneObject *t_ScnObject)
{
    m_LoadedOBJ.append(t_ScnObject);
}

void NeoRenderer::initializeGL()
{
    initializeOpenGLFunctions();
    glEnable(GL_DEPTH_TEST);
}

void NeoRenderer::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
}

void NeoRenderer::paintGL()
{
    RendererUpdate();
}

void NeoRenderer::CameraSetOrtho()
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, width(), 0, height(), -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void NeoRenderer::CameraSetPerspective()
{
    QMatrix4x4 projection;
    projection.perspective(45.0f, float(width()) / float(height()), 0.1f, 100.0f);
    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(projection.constData());
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void NeoRenderer::RendererLoop()
{
    //Should probably call update here.
    update();
}


void NeoRenderer::RenderMesh(MeshData *l_Mesh)
{
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);

    QList<FaceData *> l_FacesNoAlpha = l_Mesh->getFacesFiltered(false);
    QList<FaceData *> l_FacesAlpha = l_Mesh->getFacesFiltered(true);

    std::sort(l_FacesAlpha.begin(), l_FacesAlpha.end(), [&](FaceData * a, FaceData * b)
    {
        return DistanceFromCamera(m_CameraTransform, a->GetCenter()) > DistanceFromCamera(m_CameraTransform, b->GetCenter());
    });

    // Render opaque faces first
    RenderFaces(l_Mesh, l_FacesNoAlpha);

    // Render transparent faces
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    RenderFaces(l_Mesh, l_FacesAlpha);
    glDisable(GL_BLEND);


    // Cleanup
    glDisable(GL_BLEND);
    glDisable(GL_TEXTURE_2D);
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
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    m_TextureLoader->GetTexture(l_Background)->bind();

    glBegin(GL_QUADS);
    glTexCoord2f(0, 0); glVertex2f(0, 0);
    glTexCoord2f(1, 0); glVertex2f(width(), 0);
    glTexCoord2f(1, 1); glVertex2f(width(), height());
    glTexCoord2f(0, 1); glVertex2f(0, height());
    glEnd();

    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_TEXTURE_2D);
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

    m_InUpdate = true;
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    CameraSetPerspective();

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
    CameraSetOrtho();

    if(!m_OverlayImage.trimmed().isEmpty()) RenderBackground(m_OverlayImage);

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
