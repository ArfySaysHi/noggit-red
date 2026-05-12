// This file is part of Noggit3, licensed under GNU General Public License
// (version 3).

#pragma once
#include <QFile>
#include <QObject>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>
#include <QtWidgets/QDialog>
#include <ui_DownloadFileDialog.h>

namespace Noggit::Ui {
class DownloadFileDialog : public QDialog {
  Q_OBJECT
public:
  DownloadFileDialog(QString url, QString fileDownloadPath,
                     QWidget *parent = nullptr);

private:
  ::Ui::DownloadFileDialog *ui;
  QNetworkAccessManager manager;
  QFile *localFile;
  QNetworkReply *_response;

public slots:
  void downloadFinished(QNetworkReply *data);
  void updateProgress(qint64 recieved, qint64 total);
  void readingReadyBytes();
};
} // namespace Noggit::Ui
