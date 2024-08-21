#ifndef OBJ_MODEL_READER_H
#define OBJ_MODEL_READER_H

#include <QHash>
#include <QString>

#include <Neo/OpenGL/mesh_data.h>

class MtlModelReader
{
public:
    MtlModelReader(QString l_filePath, QString l_fileName);
    MaterialData *GetMaterial(QString m_MatName);

private:
    QHash<QString, MaterialData*> m_Materials = {};
};

class ObjModelReader //: public ModelReader
{
public:
    ObjModelReader(QString l_fileName);
    ObjModelReader(QString l_filePath, QString l_fileName);

    QList<MeshData*> GetMeshData();

    SceneObject* GenerateSceneObject();

private:
    void ParseFile();

    MeshData *ParseMeshData(QStringList l_verts, QStringList l_faces, QStringList l_uvs, int l_StartingVert);

    QString m_FilePath = "";
    QString m_FileName = "";

    QList<MeshData*> m_ParsedMeshData = {};

    QList<MtlModelReader *> m_MtlReaders = {};
    QHash<int, QString> m_FaceMaterialIndex = {};
};

#endif // OBJ_MODEL_READER_H
