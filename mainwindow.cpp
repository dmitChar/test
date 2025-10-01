#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    parseFromFile(":/xmlFile.xml");
}

void MainWindow::parseFromFile(const QString &fileName)
{
    QFile file(fileName);

    if (!file.open(QIODevice::ReadOnly | QFile::Text))
    {
        qDebug() << "Невозможно открыть файл" << fileName;
        return;
    }

    QXmlStreamReader xml(&file);
    printCommands(xml);
    file.close();
}

void MainWindow::printCommands(QXmlStreamReader &xml)
{
    int commandCount = 0;

    while(!xml.atEnd())
    {
        QXmlStreamReader::TokenType token = xml.readNext();
        if (token == QXmlStreamReader::StartDocument)
            continue;

        if (token == QXmlStreamReader::StartElement && xml.name() == QString("TContextCMD"))
        {
            if (commandCount % 2 == 0)
                qDebug() << "Request: ";
            else qDebug() << "Response: ";

            QString command = xml.attributes().value("Data").toString();
            qDebug() << "Command:" << command;

            parseElements(xml, command);
            commandCount++;
            qDebug() << "\n";

        }
    }
}

void MainWindow::parseElements(QXmlStreamReader &xml, QString &command)
{
    while (!xml.atEnd() && !xml.hasError()) {
        QXmlStreamReader::TokenType token = xml.readNext();

        if (token == QXmlStreamReader::StartElement) {
            if (xml.name() == QString("TCont")) {
                QString name = xml.attributes().value("Name").toString();
                QString type = xml.attributes().value("Type").toString();
                bool ok = false;
                int storageLen = xml.attributes().value("StorageLen").toInt(&ok);
                if (!ok) storageLen = 0;

                QString realData = formatData(command, type, storageLen);
                qDebug().noquote() << name + ":" << realData;
            }
        } else if (token == QXmlStreamReader::EndElement) {
            if (xml.name() == QString("TContextCMD")) {
                // закончили блок команды — возвращаемся в printCommands
                return;
            }
        }
    }
}

QString MainWindow::formatData(QString &command, const QString &type,  int len)
{
    int hexLength = len * 2;
    QString hexData = command.left(hexLength);
    command = command.mid(hexLength);

    QByteArray bytes = QByteArray::fromHex(hexData.toUtf8());

    if (type == "A")
        return QString::fromLatin1(bytes);

    else if (type == "H")
        return QString::fromLatin1(bytes);

    else if (type == "B")
        return hexData;

    else if (type == "N")
    {
        QString asAscii = QString::fromLatin1(bytes);
        bool allDigits = !asAscii.isEmpty();

        for (auto c : asAscii)
        {
            if (!c.isDigit())
            {
                allDigits = false;
                break;
            }
        }
        if (allDigits) return asAscii;
        else return hexData;
    }
    return hexData;

}

MainWindow::~MainWindow()
{
}

