#ifndef NEO_RENDERER_H
#define NEO_RENDERER_H

#include <QObject>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QWidget>
#include <QMatrix4x4>
#include <QTimer>
#include <QOpenGLShaderProgram>

#include <Neo/OpenGL/camera_data.h>
#include <Neo/OpenGL/mesh_data.h>
#include <Neo/Managers/texture_manager.h>
#include <Neo/scene/scene_data.h>
#include <Neo/scene/scene_object.h>

enum eDebugKeyInputs
{
    DbgKeyForward,
    DbgKeyBack,
    DbgKeyLeft,
    DbgKeyRight,
    DbgKeyUp,
    DbgKeyDown,
    DbgCamRotateLeft,
    DbgCamRotateRight,
    DbgCamRotateUp,
    DbgCamRotateDown
};

class MeshData;

class NeoRenderer : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    using QOpenGLWidget::QOpenGLWidget;
    NeoRenderer(int t_fps, QWidget *parent = nullptr);

    void RendererUpdate();
    void DisableAutoUpdates();
    void EnableAutoUpdates();

    //Remove?
    void WaitForUpdateFinish();

    //To-Do: Create a UserInterface system instead.
    void SetOverlay(QString t_overlay);
    void SetDebugValue(int l_dbg);

    //To-Do: Rename to ClearScene();
    void ClearViewport();
    void LoadSceneObject(SceneObject *t_ScnObject);

    // Input (Temporary)
    void ToggleFreecam(bool t_toggle);
    void ClearDebugKeys();
    void PressDebugKey(eDebugKeyInputs l_key);
    void ReleaseDebugKey(eDebugKeyInputs l_key);

    void SetCamera(CameraData *t_Camera);
    void SetBoneMatrix(int l_BoneId, QMatrix4x4 l_matrix);

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

    SceneData *GetScene();
    CameraData *GetCamera();

protected:
    //OpenGL
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

private:
    void initShaders();
    void initTextures();

    //Camera Functions (TEMP)
    void UpdateFreeCam();

    //Rendering Code
    void RenderBackground(QString l_Background);
    void RenderBackground(QMatrix4x4 l_BoneTransform);

    //Other
    bool WithinFOV(QVector3D l_Object);

    double DistanceFromCamera(const QVector3D& t_CameraPosition, const QVector3D& t_VertexPosition);

private slots:
    void RendererLoop();

private:
    QMap<QString, bool> m_QueuedAction = {};

    bool m_IsFreecam = false;
    bool m_InUpdate = false;
    bool m_AutoUpdate = true;

    //Rendering
    QTimer *m_RenderTimer = nullptr;

    //Cameras
    SceneData *m_CurrentScene = nullptr;
    CameraData *m_CurrentCamera = nullptr;

    TextureManager *m_TextureLoader = nullptr;

    QString m_OverlayImage = "";
    QOpenGLShaderProgram m_ShaderProgram;

    MeshData *m_DebugMesh = nullptr;


    QMatrix4x4 m_JointTransforms[100];
    QList<SceneObject *> m_LoadedOBJ = {};
    QMap<eDebugKeyInputs, bool> m_PressedKeys = {};

    int m_DebugValue = 0;
};
#endif // NEO_RENDERER_H
