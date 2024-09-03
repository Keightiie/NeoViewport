#ifndef SCENE_OBJECT_H
#define SCENE_OBJECT_H

#include <QVector3D>
#include <Neo/OpenGL/mesh_data.h>
#include <Neo/scene/objects/components/neo_animator.h>
#include <Neo/scene/objects/components/neo_armature.h>
#include <Neo/scene/objects/components/neo_transform.h>


class SceneObject
{
public:
    SceneObject();
    NeoAnimator *GetAnimator();
    NeoArmature *GetArmature();
    void DrawObject(QOpenGLShaderProgram *t_shaderProgram, TextureManager *t_textureManager);

    QList<MeshData*> m_Mesh = {};
    NeoTransform *m_CompTransform = nullptr;

private:
    NeoAnimator *m_CompAnimator = nullptr;
    NeoArmature *m_CompArmature = nullptr;
};

#endif
