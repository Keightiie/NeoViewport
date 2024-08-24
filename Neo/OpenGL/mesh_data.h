#ifndef MESH_DATA_H
#define MESH_DATA_H

#include <Neo/Managers/texture_manager.h>
#include <QList>
#include <QOpenGLBuffer>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>
#include <QVector2D>
#include <QVector3D>
#include <QVector4D>
#include <gl/GL.h>

enum eFacesType
{
    faceQuad,
    faceTri
};

class MaterialData
{
public:
    MaterialData(){};
    MaterialData(QString l_texturePath)
    {
        m_TexturePath = l_texturePath;
    };

    void setTexturePath(QString l_Path) { m_TexturePath = l_Path; }
    void setAlphaPath(QString l_Path) { m_AlphaPath = l_Path; m_UsesAlpha = true; }
    QString getTexturePath() { return m_TexturePath; }
    bool getUsesAlpha() { return m_UsesAlpha;}

private:
    QString m_TexturePath = "";
    QString m_AlphaPath = "";
    bool m_UsesAlpha = false;
};

struct GLVertexData
{
    QVector3D m_Position;
    QVector2D m_TexCoord;
};

struct GLMeshData
{
    int m_StartingIndex;
    int m_IndexLength;
};


class FaceData
{
public:
    FaceData(QVector4D l_Index) { m_VerticiesIndex = l_Index; };
    FaceData(QVector4D l_Index, MaterialData *l_mat) { m_VerticiesIndex = l_Index; m_Material = l_mat; };

    QVector4D getVerticiesIndex() { return m_VerticiesIndex; };
    QVector4D getTexCordsIndex() { return m_TexCordsIndex; };
    QString getMaterialPath() { if (m_Material == nullptr) return ""; else return m_Material->getTexturePath(); };
    MaterialData *getMaterial() {return m_Material;};


    QList<QVector3D> GetVerticies() { return m_VerticiesCords; };
    QList<QVector3D> GetTexCords() { return m_TexCords; };

    void SetTexCordIndex(QVector4D t_index){m_TexCordsIndex = t_index;};

    void SetVerticies(QList<QVector3D> l_Verts)
    {
        m_VerticiesCords = l_Verts;
        if(l_Verts.count() == 3) m_Center = (l_Verts.at(0) + l_Verts.at(1) + l_Verts.at(2)) / 3.0f;
    }

    QVector3D GetCenter(){return m_Center;};

    void SetTexCords(QList<QVector3D> l_Verts) {m_TexCords = l_Verts;};


private:
    QList<QVector3D> m_VerticiesCords = {};
    QList<QVector3D> m_TexCords = {};

    QVector3D m_Center;

    MaterialData *m_Material = nullptr;
    QVector4D m_VerticiesIndex = {0, 0, 0, 0};
    QVector4D m_TexCordsIndex = {-1, -1, -1, -1};

};


class MeshData : protected QOpenGLFunctions
{
public:
    MeshData();

    QList<FaceData*> getFaces() { return m_Faces; }
    QList<FaceData*> getFacesFiltered(bool m_alpha)
    {
        if(m_alpha)
        {
            return m_FacesFilterAlpha;
        };
        return m_FacesFilterNoAlpha;
    }
    QList<QVector3D> getVertices() { return m_Vertices; }
    QList<QVector2D> getTexCords() { return m_TextureCords; }
    eFacesType getFaceType(){ return m_FaceType; }

    void SetVerticies(QList<QVector3D>  t_verts)
    {
        m_Vertices = t_verts;
    }

    void SetTexCords(QList<QVector2D>  t_cords) { m_TextureCords = t_cords; }
    void SetFaces(QList<FaceData*>  t_faces){ m_Faces = t_faces; }
    void SetFaceType(eFacesType t_FaceType) { m_FaceType = t_FaceType; }

    void CreateDebugCube();

    void CacheVertexBuffer();

    void Initialize();
    void DrawMesh(QOpenGLShaderProgram *t_shaderProgram, TextureManager *t_textureManager, int t_debug = 0);

    QString GetMaterialPath();

private:
    MaterialData *m_Material = nullptr;
    QList<FaceData*> m_Faces = {};
    QList<FaceData*> m_FacesFilterAlpha = {};
    QList<FaceData*> m_FacesFilterNoAlpha = {};
    QHash<int, QList<GLMeshData>> m_LoadOrder = {};

    QList<QVector3D> m_Vertices = {};
    QList<QVector2D> m_TextureCords = {};
    QStringList m_TextureNames = {};
    QList<QOpenGLTexture *> m_Textures = {};

    eFacesType m_FaceType = faceQuad;

    QVector<GLVertexData> m_GLVertices;
    QVector<GLushort> m_GLIndices;
    std::vector<GLuint> m_GLTextures;
    QOpenGLFunctions *m_GLFunctions;

    QOpenGLBuffer arrayBuf;
    QOpenGLBuffer indexBuf;

};


class SceneObject
{
public:
    SceneObject(){};

    QList<MeshData*> m_Mesh = {};

    bool m_IsBillboard = false;
    QVector3D m_Transform = QVector3D(0, 0, 0);
    QVector3D m_Rotation = QVector3D(0, 0, 0);
};


#endif // MESH_DATA_H
