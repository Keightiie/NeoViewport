#include "mesh_data.h"
#include "qdebug.h"

#include <QOpenGLFunctions>

MeshData::MeshData() : indexBuf(QOpenGLBuffer::IndexBuffer)
{
    initializeOpenGLFunctions();
    arrayBuf.create();
    indexBuf.create();
}

void MeshData::CreateDebugCube()
{
    //This is function no longer works correctly due to API changes.

    m_Vertices.clear();
    m_FaceType = faceQuad;

    //Top
    m_Vertices.append(QVector3D(-1, -1, 1));
    m_Vertices.append(QVector3D(1, -1, 1));
    m_Vertices.append(QVector3D(1, 1, 1));
    m_Vertices.append(QVector3D(-1, 1, 1));

    //Bottom
    m_Vertices.append(QVector3D(-1, -1, -1));
    m_Vertices.append(QVector3D(1, -1, -1));
    m_Vertices.append(QVector3D(1, 1, -1));
    m_Vertices.append(QVector3D(-1, 1, -1));


    MaterialData *l_DebugMat = new MaterialData("debug.png");

    //Top and Bottom Faces
    m_Faces.append(new FaceData(QVector4D(0, 1, 2, 3), l_DebugMat));
    m_Faces.append(new FaceData(QVector4D(4, 5, 6, 7), l_DebugMat));

    m_Faces.append(new FaceData(QVector4D(0, 1, 5, 4), l_DebugMat));
    m_Faces.append(new FaceData(QVector4D(2, 3, 7, 6), l_DebugMat));

    m_Faces.append(new FaceData(QVector4D(1, 2, 6, 5), l_DebugMat));
    m_Faces.append(new FaceData(QVector4D(0, 3, 7, 4), l_DebugMat));

    //Side Faces
}

void MeshData::CacheVertexBuffer()
{
    m_GLVertices.clear();
    m_GLIndices.clear();


    QList<QVector3D> l_verticies = getVertices();
    QList<FaceData*> l_faces = getFaces();

    GLushort l_FaceId = 0;

    int l_FaceCount = 0;
    int l_FaceStartingValue = 0;


    int l_CurrentTextureId = -1;
    QString l_CurrentMat = "";

    for(FaceData *r_face : l_faces)
    {
        if (r_face->getMaterial() != nullptr)
        {
            m_Material = r_face->getMaterial();
            QString l_texture = m_Material->getTexturePath();
            if (l_CurrentMat != l_texture)
            {
                m_LoadOrder[l_CurrentTextureId].append({l_FaceStartingValue, l_FaceCount * 3});
                l_FaceStartingValue += l_FaceCount * 3;
                l_FaceCount = 0;
                l_CurrentMat = l_texture;
                if (m_TextureNames.contains(l_texture))
                {
                    l_CurrentTextureId = m_TextureNames.indexOf(l_texture);
                }
                else
                {
                    if (l_texture.trimmed().isEmpty())
                    {
                        l_CurrentTextureId = -1;
                    }
                    else
                    {
                        m_TextureNames.append(l_texture);
                        l_CurrentTextureId = m_TextureNames.indexOf(l_texture);
                    }
                }
            }
        }
        else
        {
            l_CurrentTextureId = -1;
        }

        QVector4D r_index = r_face->getVerticiesIndex();
        QVector4D r_TexIndex = r_face->getTexCordsIndex();

        QVector3D l_Vert1 = l_verticies.at(r_index.x());
        QVector3D l_Vert2 = l_verticies.at(r_index.y());
        QVector3D l_Vert3 = l_verticies.at(r_index.z());

        QVector2D l_VertUV1 = (r_TexIndex.x() != -1) ? getTexCords().at(r_TexIndex.x()) : QVector2D();
        QVector2D l_VertUV2 = (r_TexIndex.y() != -1) ? getTexCords().at(r_TexIndex.y()) : QVector2D();
        QVector2D l_VertUV3 = (r_TexIndex.z() != -1) ? getTexCords().at(r_TexIndex.z()) : QVector2D();


        //VBO
        m_GLVertices.append({l_Vert1, l_VertUV1});
        m_GLVertices.append({l_Vert2, l_VertUV2});
        m_GLVertices.append({l_Vert3, l_VertUV3});

        l_FaceCount += 1;
    }

    Initialize();

}

void MeshData::Initialize()
{
    arrayBuf.bind();
    arrayBuf.allocate(m_GLVertices.constData(), m_GLVertices.size() * sizeof(GLVertexData));
}

void MeshData::DrawMesh(QOpenGLShaderProgram *t_shaderProgram, TextureManager *t_textureManager, int t_debug)
{
    if(m_Textures.isEmpty())
    {
        for(QString l_TexturePath : m_TextureNames)
        {
            m_Textures.append(t_textureManager->GetTexture(l_TexturePath));
        }
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    arrayBuf.bind();

    quintptr offset = 0;

    int vertexLocation = t_shaderProgram->attributeLocation("a_position");
    t_shaderProgram->enableAttributeArray(vertexLocation);
    t_shaderProgram->setAttributeBuffer(vertexLocation, GL_FLOAT, offset, 3, sizeof(GLVertexData));

    offset += sizeof(QVector3D);

    int texcoordLocation = t_shaderProgram->attributeLocation("a_texcoord");
    t_shaderProgram->enableAttributeArray(texcoordLocation);
    t_shaderProgram->setAttributeBuffer(texcoordLocation, GL_FLOAT, offset, 2, sizeof(GLVertexData));

    //glDrawArrays(GL_TRIANGLES, t_debug, m_GLVertices.size());

    for (int i = 0; i < m_Textures.count(); i++)
    {
        if (m_LoadOrder.contains(i))
        {
            QOpenGLTexture *l_texture = m_Textures[i];
            if(l_texture != nullptr) l_texture->bind();

            for (GLMeshData m_Mesh : m_LoadOrder[i])
            {
                glDrawArrays(GL_TRIANGLES, m_Mesh.m_StartingIndex, m_Mesh.m_IndexLength);
            }
        }
    }
    if (m_LoadOrder.contains(-1))
    {
        for (GLMeshData m_Mesh : m_LoadOrder[-1])
        {
            glDrawArrays(GL_TRIANGLES, m_Mesh.m_StartingIndex, m_Mesh.m_IndexLength);
        }
    }


    glDisable(GL_BLEND);
}

QString MeshData::GetMaterialPath()
{
    if(m_Material != nullptr) return m_Material->getTexturePath();
    return "";
}
