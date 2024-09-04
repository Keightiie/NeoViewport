#ifndef SCENE_DATA_H
#define SCENE_DATA_H

#include "Neo/scene/objects/components/neo_transform.h"

#include "scene_object.h"

#include <QOpenGLShaderProgram>

#include <Neo/OpenGL/camera_data.h>


class SceneData
{
public:
    SceneData();

    void UpdateShader(QOpenGLShaderProgram *t_shaderProgram);
    QVector3D GetAmbientColor();
    QVector3D GetLightColor();
    NeoTransform *GetLightTransform();

    void SetAmbientColor(QVector3D t_amb);
    void SetLightColor(QVector3D t_amb);

    //Cameras
    void CameraSet(CameraData * l_camera);
    CameraData * getCamera(int l_id = -1);

    //Game Objects
    void GameObjectsClear();
    void GameObjectsAdd(SceneObject *l_Object);
    QList<SceneObject *> getGameObjects();
    int getGameObjectsCount();

    virtual void Render(QOpenGLShaderProgram *t_shaderProgram, TextureManager *t_textureManager);
    virtual void Update();

private:

    NeoTransform *m_LightTransform = nullptr;
    QVector3D m_LightSourceColor = QVector3D(0.6f, 0.6f, 0.6f);
    QVector3D m_LightAmbientColor = QVector3D(0.8f, 0.8f, 0.8f);

    QList<SceneObject *> m_GameObjects = {};

    CameraData * m_MainCamera = nullptr;

};

#endif // SCENE_DATA_H
