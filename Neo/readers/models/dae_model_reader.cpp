#include "dae_model_reader.h"

#include <QFile>
#include <QDebug>

DAEModelReader::DAEModelReader(QString l_filePath, QString l_fileName)
{
    QFile file(l_filePath + "/" + l_fileName);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Cannot open file:" << l_filePath + "/" + l_fileName;
        return;
    }

    QXmlStreamReader l_XmlReader(&file);
    l_XmlReader.clear();
    file.close();
}
