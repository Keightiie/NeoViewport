#include "scene_data.h"

SceneData::SceneData()
{
    CameraSet(new CameraData());
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

void SceneData::CameraSet(CameraData *l_camera)
{
    m_MainCamera = l_camera;
}

CameraData *SceneData::getCamera(int l_id)
{
    if(l_id == -1)
    {
        return m_MainCamera;
    }

    return m_MainCamera;
}

void SceneData::GameObjectsClear()
{
    m_GameObjects.clear();
}

void SceneData::GameObjectsAdd(SceneObject *l_Object)
{
    m_GameObjects.append(l_Object);
}

int SceneData::getGameObjectsCount()
{
    return m_GameObjects.count();
}

void SceneData::Render(QOpenGLShaderProgram *t_shaderProgram, TextureManager *t_textureManager)
{


    QMatrix4x4 l_ViewMatrix = getCamera()->GetProjectionMatrix(true);

    t_shaderProgram->setUniformValue("mvp_matrix", l_ViewMatrix);

    QMatrix3x3 l_normalMatrix = getCamera()->GetProjectionMatrix(false).normalMatrix();
    t_shaderProgram->setUniformValue("normal_matrix", l_normalMatrix);
    t_shaderProgram->setUniformValue("texture", 0);

    t_shaderProgram->setUniformValue("view_position", QVector3D(0.0f, 0.0f, 5.0f));
    UpdateShader(t_shaderProgram);

    for(SceneObject *l_ScenObj : m_GameObjects)
    {
        if(l_ScenObj == nullptr) return;
        l_ScenObj->DrawObject(t_shaderProgram, t_textureManager);
    }
}

void SceneData::Update()
{

}
