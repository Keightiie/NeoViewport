#include "scene_object.h"

SceneObject::SceneObject()
{
    m_CompArmature = new NeoArmature();
    m_CompAnimator = new NeoAnimator(m_CompArmature);
}

NeoAnimator *SceneObject::GetAnimator()
{
    return m_CompAnimator;
}

NeoArmature *SceneObject::GetArmature()
{
    return m_CompArmature;
}

void SceneObject::DrawObject(QOpenGLShaderProgram *t_shaderProgram, TextureManager *t_textureManager)
{
    m_CompAnimator->RunAnimation();
    t_shaderProgram->setUniformValueArray("jointTransforms", m_CompArmature->m_BoneMatrices, 512);
    for(MeshData *l_Mesh : m_Mesh)
    {
        l_Mesh->DrawMesh(t_shaderProgram, t_textureManager);
    }
}
