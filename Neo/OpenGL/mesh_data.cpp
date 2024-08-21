#include "mesh_data.h"

MeshData::MeshData() {}

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
    m_CachedVertex.clear();

    QList<QVector3D> l_verticies = getVertices();
    QList<FaceData*> l_faces = getFaces();

    for(FaceData *r_face : l_faces)
    {
        QVector4D r_index = r_face->getVerticiesIndex();
        QVector4D r_TexIndex = r_face->getTexCordsIndex();

        QVector3D l_Vert1 = l_verticies.at(r_index.x());
        QVector3D l_Vert2 = l_verticies.at(r_index.y());
        QVector3D l_Vert3 = l_verticies.at(r_index.z());
        QVector3D l_Vert4 = l_verticies.at(r_index.w());

        QVector2D l_VertUV1 = (r_TexIndex.x() != -1) ? getTexCords().at(r_TexIndex.x()) : QVector2D();
        QVector2D l_VertUV2 = (r_TexIndex.y() != -1) ? getTexCords().at(r_TexIndex.y()) : QVector2D();
        QVector2D l_VertUV3 = (r_TexIndex.z() != -1) ? getTexCords().at(r_TexIndex.z()) : QVector2D();
        QVector2D l_VertUV4 = (r_TexIndex.w() != -1) ? getTexCords().at(r_TexIndex.w()) : QVector2D();

        r_face->SetVerticies({l_Vert1, l_Vert2, l_Vert3});
        r_face->SetTexCords({l_VertUV1, l_VertUV2, l_VertUV3});

        m_CachedVertex.push_back({ l_Vert1, l_VertUV1, r_face->getMaterial()});
        m_CachedVertex.push_back({ l_Vert2, l_VertUV2, nullptr });
        m_CachedVertex.push_back({ l_Vert3, l_VertUV3, nullptr});

        if(getFaceType() == faceQuad)
        {
            m_CachedVertex.push_back({ l_Vert4, l_VertUV4, nullptr});
        }

        if(r_face->getMaterial() != nullptr)
        {
            if(r_face->getMaterial()->getUsesAlpha())
            {
                m_FacesFilterAlpha.append(r_face);
            }
            else
            {
                m_FacesFilterNoAlpha.append(r_face);
            }
        }
        else
        {
            m_FacesFilterNoAlpha.append(r_face);
        }
    }

}
