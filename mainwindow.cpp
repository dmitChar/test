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
    QString xmlData = file.readAll();
    file.close();
    printCommands(xmlData);

}

void MainWindow::printCommands(QString xmlData)
{
    QXmlStreamReader xml(xmlData);
    bool request = true;
    int commandCount = 0;

    while(!xml.atEnd() && !xml.hasError())
    {
        QXmlStreamReader::TokenType token = xml.readNext();
        if (token == QXmlStreamReader::StartDocument)
            continue;

        if (token == QXmlStreamReader::StartElement)
        {
            if (xml.name() == QString("TContextCMD"))
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
}

void MainWindow::parseElements(QXmlStreamReader &xml, QString &command)
{
    while (!xml.atEnd() && !xml.hasError())
    {
        QXmlStreamReader::TokenType token = xml.readNext();
        if (token == QXmlStreamReader::StartElement)
        {
            if (xml.name() == QString("TCont"))
            {
                QString name = xml.attributes().value("Name").toString();
                QString type = xml.attributes().value("Type").toString();
                QString len = xml.attributes().value("StorageLen").toString();

                QString realData = formatData(command, type, len);

                qDebug() << name << ":" << realData;
            }
        }
        else if (token == QXmlStreamReader::EndElement)
        {
            if (xml.name() == QString("TContextCMD"))
                return;
        }
    }
}

QString MainWindow::formatData(QString &command, const QString &type, const QString &len)
{
    int length = len.toInt() * 2;
    QString realData = "";

    if (type == "B")
    {
        realData = command.left(length);
        command = command.mid(length);
    }
    else if (type == "A" || type == "H" || type == "N")
    {
        for (int i = 0; i < length; i += 2)
        {
            QString byte = command.mid(2);
            bool ok;
            realData.append(char(byte.toUInt()));
            command = command.mid(2);
        }
    }
//    else if (type == "N")
//    {
//        QString asciiStr = command.left(length);
//        for (auto ch : asciiStr)
//        {
//            realData.append(QString::number(ch.unicode()));
//        }
//        command.mid(length);
//    }

    return realData;
}

MainWindow::~MainWindow()
{
}

