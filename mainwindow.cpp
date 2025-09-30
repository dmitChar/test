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
    int hexLength = len.toInt() * 2; // Длина в hex символах (байты * 2)
    QString hexData = command.left(hexLength); // Берем hex-данные

    // УДАЛЯЕМ обработанные данные из исходной команды
    command = command.mid(hexLength);

    QString realData;

    if (type == "A" || type == "H") {
        for (int i = 0; i < hexData.length(); i += 2)
        {
            QString byte = hexData.mid(i, 2);
            bool ok;
            char ch = static_cast<char>(byte.toUShort(&ok, 16));
            if (ok) realData.append(ch);
        }
    }
    else if (type == "N") {

        for (int i = 0; i < hexData.length(); i += 2) {
            QString byte = hexData.mid(i, 2);
            bool ok;
            char ch = static_cast<char>(byte.toUShort(&ok, 16));
            if (ok) {
                realData.append(ch);
            }
        }
    }
    else if (type == "B") {
        // Bytes - форматируем hex с пробелами
        for (int i = 0; i < hexData.length(); i += 2) {
            if (!realData.isEmpty()) realData.append(" ");
            realData.append(hexData.mid(i, 2));
        }
    }

    return realData;
}

MainWindow::~MainWindow()
{
}

