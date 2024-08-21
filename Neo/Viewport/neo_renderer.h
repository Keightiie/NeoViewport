#ifndef NEO_RENDERER_H
#define NEO_RENDERER_H

#include <QObject>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QWidget>
#include <QMatrix4x4>
#include <QTimer>

#include <Neo/OpenGL/mesh_data.h>

#include <Neo/Managers/texture_manager.h>

class NeoRenderer : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    NeoRenderer(QWidget *parent = nullptr);

    void RendererUpdate();
    void DisableAutoUpdates();
    void EnableAutoUpdates();

    //Remove?
    void WaitForUpdateFinish();

    //To-Do: Create a UserInterface system instead.
    void SetOverlay(QString t_overlay);

    //To-Do: Rename to ClearScene();
    void ClearViewport();
    void LoadSceneObject(SceneObject *t_ScnObject);

public:
    //Setting
    void SetTransform(QVector3D t_Transform);
    void SetRotation(QVector3D t_Transform);

    //Translation
    void TranslateTransform(QVector3D t_Transform);
    void TranslateRotation(QVector3D t_Transform);

public:
    bool IsRendering();

    QVector3D GetTransform();
    QVector3D GetRotation();

protected:
    //OpenGL
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

private:
    //Camera Functions (TEMP)
    void CameraSetOrtho();
    void CameraSetPerspective();

    //Rendering Code
    void RenderMesh(MeshData *l_Mesh);
    void RenderFaces(MeshData *l_Mesh, QList<FaceData*> l_Faces);
    void RenderBackground(QString l_Background);

    //Other
    bool WithinFOV(QVector3D l_Object);

    double DistanceFromCamera(const QVector3D& t_CameraPosition, const QVector3D& t_VertexPosition);

private slots:
    void RendererLoop();

private:
    QHash<QString, bool> m_QueuedAction = {};

    bool m_InUpdate = false;
    bool m_AutoUpdate = true;

    //Rendering
    QTimer *m_RenderTimer = nullptr;

    //Cameras
    QVector3D m_CameraForward;
    QVector3D m_CameraTransform = {0, 0, 0};
    QVector3D m_CameraRotation = {0, 0, 0};

    TextureManager *m_TextureLoader = nullptr;

    QString m_OverlayImage = "";


    QList<SceneObject *> m_LoadedOBJ = {};
};

#endif // NEO_RENDERER_H
