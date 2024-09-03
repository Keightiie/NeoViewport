#ifndef SCENE_DATA_H
#define SCENE_DATA_H

#include "Neo/scene/objects/components/neo_transform.h"

#include <QOpenGLShaderProgram>


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

private:

    NeoTransform *m_LightTransform = nullptr;
    QVector3D m_LightSourceColor = QVector3D(0.6f, 0.6f, 0.6f);
    QVector3D m_LightAmbientColor = QVector3D(0.8f, 0.8f, 0.8f);

};

#endif // SCENE_DATA_H
