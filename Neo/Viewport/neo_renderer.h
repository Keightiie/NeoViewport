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
    NeoRenderer(SceneData *l_Scene, int t_fps, QWidget *parent = nullptr);

    void RendererUpdate();

    //Remove?
    void WaitForUpdateFinish();

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

    void setScene(SceneData * l_scene);
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

    double DistanceFromCamera(const QVector3D& t_CameraPosition, const QVector3D& t_VertexPosition);

private slots:
    void RendererLoop();

private:
    bool m_InUpdate = false;
    QMap<QString, bool> m_QueuedAction = {};

    SceneData *m_CurrentScene = nullptr;
    TextureManager *m_TextureLoader = nullptr;
    QOpenGLShaderProgram m_ShaderProgram;

    //Rendering
    QTimer *m_RenderTimer = nullptr;

    //TO-DO: Move to a seperate "DebugCamera" class.
    bool m_IsFreecam = false;
    QMap<eDebugKeyInputs, bool> m_PressedKeys = {};
};
#endif // NEO_RENDERER_H
