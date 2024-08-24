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
    void initShaders();
    void initTextures();

    //Camera Functions (TEMP)
    void UpdateFreeCam();

    //Rendering Code
    void RenderBackground(QString l_Background);

    //Other
    bool WithinFOV(QVector3D l_Object);

    double DistanceFromCamera(const QVector3D& t_CameraPosition, const QVector3D& t_VertexPosition);

private slots:
    void RendererLoop();

private:
    QHash<QString, bool> m_QueuedAction = {};

    bool m_IsFreecam = false;
    bool m_InUpdate = false;
    bool m_AutoUpdate = true;

    //Rendering
    QTimer *m_RenderTimer = nullptr;

    //Cameras
    CameraData *m_CurrentCamera = nullptr;
    QVector3D m_CameraForward;
    QVector3D m_CameraTransform = {0, 0, 0};
    QVector3D m_CameraRotation = {0, 0, 0};

    TextureManager *m_TextureLoader = nullptr;

    QString m_OverlayImage = "";
    QOpenGLShaderProgram m_ShaderProgram;

    MeshData *m_DebugMesh = nullptr;


    QList<SceneObject *> m_LoadedOBJ = {};
    QHash<eDebugKeyInputs, bool> m_PressedKeys = {};


    QMatrix4x4 l_CameraProjection;

    int m_DebugValue = 0;
};
#endif // NEO_RENDERER_H
