#include "mainwindow.h"
#include "ui_mainwindow.h"

using namespace qttp;
using namespace native::http;

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow),
  m_HttpDataModel(parent)
{
  ui->setupUi(this);
  ui->tableView->setModel(&m_HttpDataModel);
}

MainWindow::~MainWindow()
{
  delete ui;
}

HttpDataModel& MainWindow::getModel()
{
  return m_HttpDataModel;
}

HttpDataModel::HttpDataModel(QWidget* parent) :
  QAbstractTableModel(parent)
{
}

HttpDataModel::~HttpDataModel()
{
}

QVariant HttpDataModel::data(const QModelIndex &index, int role) const
{
  Q_UNUSED(role);

  if(m_Data.isEmpty() || m_Data.size() < index.row())
    return QVariant();

  return QVariant(m_Data[index.row()]);
}

int HttpDataModel::rowCount(const QModelIndex& parent) const
{
  Q_UNUSED(parent);
  return m_Data.size();
}

int HttpDataModel::columnCount(const QModelIndex& parent) const
{
  Q_UNUSED(parent);
  return 1;
}

void HttpDataModel::addData(qttp::HttpData& data)
{
  LOG_TRACE;
  m_Data.push_back(QDateTime::currentDateTime().toString() + " - " + data.getRequest().getUrl().getPath());
  emit layoutChanged();
}

