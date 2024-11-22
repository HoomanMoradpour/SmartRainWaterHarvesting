#ifndef CSVPARSER_H
#define CSVPARSER_H

#include <QString>
#include <QMap>
#include <QList>
#include <QStringList>

struct Location {
    QString city;
    double latitude;
    double longitude;
};

class CSVParser {
public:
    CSVParser(const QString &filePath);
    QMap<QString, QList<Location>> parse();

private:
    QString m_filePath;
};

#endif // CSVPARSER_H
