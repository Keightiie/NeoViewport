#include "obj_model_reader.h"

#include <QFile>
#include <QDebug>

ObjModelReader::ObjModelReader(QString l_filePath, QString l_fileName)
{
    m_FilePath = l_filePath;
    m_FileName = l_fileName;
    ParseFile();
}

MeshData *ObjModelReader::ParseMeshData(int l_StartingVert)
{
    MeshData *l_NewMesh = new MeshData();

    QList<QVector3D> l_VertexList = {};
    QList<QVector3D> l_NormalsList = {};
    QList<QVector2D> l_TexCordList = {};

    for(QString l_Vert : m_LoadedLinesVertex)
    {
        QStringList parts = l_Vert.split(' ');
        parts.removeAll("");

        if (parts.size() < 4) continue;

        QVector3D l_Transform;
        l_Transform.setX(parts[1].toFloat());
        l_Transform.setY(parts[2].toFloat());
        l_Transform.setZ(parts[3].toFloat());


        l_VertexList.append(l_Transform);
    }

    for(QString l_TexCords : m_LoadedLinesVertexTexCords)
    {
        QVector2D l_UV(0, 0);

        QStringList UVParts = l_TexCords.split(' ');
        UVParts.removeAll("");
        l_UV.setX(UVParts[1].toDouble());
        l_UV.setY(UVParts[2].toDouble());

        l_TexCordList.append(l_UV);
    }


    for(QString l_NormalLine : m_LoadedLinesVertexNorms)
    {
        QStringList l_SplitLine = l_NormalLine.split(' ');
        l_SplitLine.removeAll("");
        l_NormalsList.append(QVector3D(l_SplitLine[1].toDouble(), l_SplitLine[2].toDouble(), l_SplitLine[3].toDouble()));
    }

    QList<FaceData *> l_FacesList = {};

    MaterialData *l_CurrentMaterial = nullptr;

    int l_CurrentFaceIndex = 0;
    for(QString l_face : m_LoadedLinesFaces)
    {
        if(m_FaceMaterialIndex.contains(l_CurrentFaceIndex))
        {
            for(MtlModelReader *r_Reader : m_MtlReaders)
            {
                l_CurrentMaterial = r_Reader->GetMaterial(m_FaceMaterialIndex[l_CurrentFaceIndex]);
            }

        }

        QStringList l_FaceParts = l_face.split(' ');
        l_FaceParts.removeAll("");
        if(l_FaceParts.count() != 4) continue;

        QStringList l_FaceData1 = l_FaceParts[1].split('/');
        QStringList l_FaceData2 = l_FaceParts[2].split('/');
        QStringList l_FaceData3 = l_FaceParts[3].split('/');

        //Vertex Index
        int index1 = l_FaceData1[0].toInt() - l_StartingVert - 1;
        int index2 = l_FaceData2[0].toInt() - l_StartingVert - 1;
        int index3 = l_FaceData3[0].toInt() - l_StartingVert - 1;



        //UV Index

        int TexCordindex1 = -1;
        int TexCordindex2 = -1;
        int TexCordindex3 = -1;

        int NormalIndex1 = -1;
        int NormalIndex2 = -1;
        int NormalIndex3 = -1;

        if(l_FaceData1.count() > 1 && !l_FaceData1[1].trimmed().isEmpty())
        {
            TexCordindex1 = l_FaceData1[1].toInt() - 1;
        }

        if(l_FaceData2.count() > 1 && !l_FaceData2[1].trimmed().isEmpty())
        {
            TexCordindex2 = l_FaceData2[1].toInt() - 1;
        }

        if(l_FaceData3.count() > 2 && !l_FaceData3[1].trimmed().isEmpty())
        {
            TexCordindex3 = l_FaceData3[1].toInt() - 1;
        }

        //Parse the Normals
        if(l_FaceData1.count() > 2 && !l_FaceData1[2].trimmed().isEmpty())
        {
            NormalIndex1 = l_FaceData1[2].toInt() - 1;
        }

        if(l_FaceData2.count() > 2 && !l_FaceData2[2].trimmed().isEmpty())
        {
            NormalIndex2 = l_FaceData2[2].toInt() - 1;
        }

        if(l_FaceData3.count() > 2 && !l_FaceData3[2].trimmed().isEmpty())
        {
            NormalIndex3 = l_FaceData3[2].toInt() - 1;
        }

        FaceData * l_NewFace = new FaceData(QVector4D(index1, index2, index3, 0), l_CurrentMaterial);
        l_NewFace->SetTexCordIndex(QVector4D(TexCordindex1, TexCordindex2, TexCordindex3, -1));
        l_NewFace->SetNormalsIndex(QVector4D(NormalIndex1, NormalIndex2, NormalIndex3, -1));

        l_FacesList.append(l_NewFace);

        l_CurrentFaceIndex += 1;
    }
    l_NewMesh->SetFaceType(faceTri);
    l_NewMesh->SetFaces(l_FacesList);
    l_NewMesh->SetVerticies(l_VertexList);
    l_NewMesh->SetNormals(l_NormalsList);
    l_NewMesh->SetTexCords(l_TexCordList);

    l_NewMesh->CacheVertexBuffer();
    return l_NewMesh;
}

QList<MeshData *> ObjModelReader::GetMeshData()
{
    return m_ParsedMeshData;
}

SceneObject *ObjModelReader::GenerateSceneObject()
{
    SceneObject* l_NewObject = new SceneObject();
    l_NewObject->m_Mesh = m_ParsedMeshData;
    return l_NewObject;
}

void ObjModelReader::ParseFile()
{
    QFile file(m_FilePath + "/" + m_FileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qWarning() << "Failed to open file:" << m_FilePath + "/" + m_FileName;
        return;
    }

    QString l_currentMatFile = "";

    QString l_ObjectName = "";

    QTextStream in(&file);
    int l_StartingVert = 1;
    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (line.startsWith("v ")) m_LoadedLinesVertex.append(line);
        if (line.startsWith("f ")) m_LoadedLinesFaces.append(line);
        if (line.startsWith("vn ")) m_LoadedLinesVertexNorms.append(line);
        if (line.startsWith("vt ")) m_LoadedLinesVertexTexCords.append(line);
        if (line.startsWith("usemtl "))
        {
            QString l_MatName = line.remove("usemtl ");
            m_FaceMaterialIndex[m_LoadedLinesFaces.count()] = l_MatName.trimmed();
        }
        if (line.startsWith("mtllib "))
        {
            QString l_MtlFileName = line.remove("mtllib ");
            m_MtlReaders.append(new MtlModelReader(m_FilePath, l_MtlFileName));
        }
    }

    MeshData *l_MeshData = ParseMeshData((l_StartingVert - 1));
    m_ParsedMeshData.append(l_MeshData);
}



MtlModelReader::MtlModelReader(QString l_filePath, QString l_fileName)
{

    QFile file(l_filePath + "/" + l_fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qWarning() << "Failed to open file:" << l_fileName;
        return;
    }

    MaterialData * l_NewMat = nullptr;

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (line.startsWith("newmtl "))
        {
            QStringList l_Parts = line.split(' ');
            l_Parts.removeAll("");
            l_Parts.removeFirst();
            QString l_MatName = l_Parts.join(' ');

            if(l_MatName.trimmed().isEmpty()) continue;

            l_NewMat = new MaterialData();
            m_Materials[l_MatName] = l_NewMat;

        }
        else if (line.startsWith("map_d "))
        {
            QStringList l_Parts = line.split(' ');
            l_Parts.removeAll("");
            l_Parts.removeFirst();
            QString l_Path = l_Parts.join(' ');
            if(l_NewMat != nullptr)
            {
                l_NewMat->setAlphaPath(l_filePath + "/" + l_Path.trimmed());
            }
        }
        else if (line.startsWith("map_Kd "))
        {
            QStringList l_Parts = line.split(' ');
            l_Parts.removeAll("");
            l_Parts.removeFirst();
            QString l_Path = l_Parts.join(' ');
            if(l_NewMat != nullptr)
            {
                l_NewMat->setTexturePath(l_filePath + "/" + l_Path.trimmed());
            }
        }
    }

}

MaterialData *MtlModelReader::GetMaterial(QString m_MatName)
{
    if(m_Materials.contains(m_MatName)) return m_Materials[m_MatName];
    return nullptr;
}

