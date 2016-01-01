#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <httpserver.h>
#include <QtCore>
#include <QtGui>
#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class HttpDataModel : public QAbstractTableModel
{
    Q_OBJECT

  public:
    HttpDataModel(QWidget *parent = 0);
    ~HttpDataModel();

    QVariant data(const QModelIndex &index, int role) const;
    int rowCount(const QModelIndex& parent = QModelIndex()) const;
    int columnCount(const QModelIndex& parent = QModelIndex()) const;

    void addData(qttp::HttpData& data);

  private:
    QVector<QString> m_Data;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

  public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    HttpDataModel& getModel();

  private:
    Ui::MainWindow *ui;
    HttpDataModel m_HttpDataModel;
};


#endif // MAINWINDOW_H
