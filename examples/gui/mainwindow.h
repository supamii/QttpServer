#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <httpserver.h>
#include <QtCore>
#include <QtGui>
#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class HttpReceiver : public QAbstractTableModel
{
    Q_OBJECT

  public:
    HttpReceiver(QWidget *parent = 0);
    ~HttpReceiver();

    QVariant data(const QModelIndex &index, int role) const;
    int rowCount(const QModelIndex& parent = QModelIndex()) const;
    int columnCount(const QModelIndex& parent = QModelIndex()) const;

    void setHttpData(qttp::HttpData& data);

  private:
    QVector<QString> m_Data;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

  public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    HttpReceiver& getModel();

  private:
    Ui::MainWindow *ui;
    HttpReceiver m_HttpReceiver;
};


#endif // MAINWINDOW_H
