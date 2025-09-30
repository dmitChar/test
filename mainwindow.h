#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFile>
#include <QXmlStreamReader>
#include <QDebug>
#include <QString>
#include <QTextStream>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    void parseFromFile(const QString &fileName);

    ~MainWindow();
private:
    void printCommands(QString xmlData);
    void parseElements(QXmlStreamReader &xml, QString &command);
    QString formatData(QString &command, const QString &type, const QString &len);

};
#endif // MAINWINDOW_H
