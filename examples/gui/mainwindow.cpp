#include "mainwindow.h"
#include "ui_mainwindow.h"

using namespace qttp;
using namespace native::http;

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow),
  m_HttpReceiver(parent)
{
  ui->setupUi(this);
  ui->tableView->setModel(&m_HttpReceiver);
}

MainWindow::~MainWindow()
{
  delete ui;
}

HttpReceiver& MainWindow::getModel()
{
  return  m_HttpReceiver;
}

HttpReceiver::HttpReceiver(QWidget* parent) :
  QAbstractTableModel(parent)
{
}

HttpReceiver::~HttpReceiver()
{
}

QVariant HttpReceiver::data(const QModelIndex &index, int role) const
{
  Q_UNUSED(role);

  if(m_Data.isEmpty() || m_Data.size() < index.row())
    return QVariant();

  return QVariant(m_Data[index.row()]);
}

int HttpReceiver::rowCount(const QModelIndex& parent) const
{
  Q_UNUSED(parent);
  return m_Data.size();
}

int HttpReceiver::columnCount(const QModelIndex& parent) const
{
  Q_UNUSED(parent);
  return 1;
}

void HttpReceiver::setHttpData(qttp::HttpData& data)
{
  LOG_TRACE;
  m_Data.push_back(data.getRequest().url().path().c_str());
  emit layoutChanged();
}

