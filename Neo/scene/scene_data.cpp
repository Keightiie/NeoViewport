#include "scene_data.h"

SceneData::SceneData()
{
    m_LightTransform = new NeoTransform();
    m_LightTransform->SetPosition({0, 1.0f, 0});
}

void SceneData::UpdateShader(QOpenGLShaderProgram *t_shaderProgram)
{
    t_shaderProgram->setUniformValue("light_position", m_LightTransform->GetPosition());
    t_shaderProgram->setUniformValue("light_color", m_LightSourceColor);
    t_shaderProgram->setUniformValue("ambient_color", m_LightAmbientColor);
}

QVector3D SceneData::GetAmbientColor()
{
    return m_LightAmbientColor;
}

QVector3D SceneData::GetLightColor()
{
    return m_LightSourceColor;
}

NeoTransform *SceneData::GetLightTransform()
{
    return m_LightTransform;
}

void SceneData::SetAmbientColor(QVector3D t_amb)
{
    m_LightAmbientColor = t_amb;
}

void SceneData::SetLightColor(QVector3D t_amb)
{
    m_LightSourceColor = t_amb;
}
