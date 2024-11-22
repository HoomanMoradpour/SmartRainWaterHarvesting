#include "CSVParser.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>

CSVParser::CSVParser(const QString &filePath)
    : m_filePath(filePath)
{
}

QMap<QString, QList<Location>> CSVParser::parse() {
    QMap<QString, QList<Location>> stateCityMap;

    QFile file(m_filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "Error opening file!";
        return stateCityMap;
    }

    QTextStream in(&file);
    bool firstLine = true;

    while (!in.atEnd()) {
        QString line = in.readLine();
        if (firstLine) {
            firstLine = false;
            continue;  // Skip header
        }

        QStringList columns = line.split(",");
        if (columns.size() < 4) continue;  // Ignore invalid rows

        QString state = columns[0].trimmed();
        QString city = columns[1].trimmed();
        double latitude = columns[2].trimmed().toDouble();
        double longitude = columns[3].trimmed().toDouble();

        Location location = {city, latitude, longitude};
        stateCityMap[state].append(location);
    }

    return stateCityMap;
}
