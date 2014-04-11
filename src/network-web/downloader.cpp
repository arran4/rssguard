#include "network-web/downloader.h"

#include "network-web/silentnetworkaccessmanager.h"

#include <QTimer>


Downloader::Downloader(QObject *parent)
  : QObject(parent),
    m_activeReply(NULL),
    m_downloadManager(new SilentNetworkAccessManager(this)),
    m_timer(new QTimer(this)) {

  m_timer->setInterval(2000);
  m_timer->setSingleShot(true);

  connect(m_timer, SIGNAL(timeout()), this, SLOT(timeout()));
  connect(m_downloadManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(finished(QNetworkReply*)));
}

Downloader::~Downloader() {
  m_downloadManager->deleteLater();
}

void Downloader::downloadFile(const QString &url, bool protected_contents,
                              const QString &username, const QString &password) {
  QNetworkRequest request;
  QObject originatingObject;

  // Set credential information as originating object.
  originatingObject.setProperty("protected", protected_contents);
  originatingObject.setProperty("username", username);
  originatingObject.setProperty("password", password);
  request.setOriginatingObject(&originatingObject);

  // Set url for this reques.
  request.setUrl(url);

  runRequest(request);
}

void Downloader::finished(QNetworkReply *reply) {
  m_timer->stop();

  // In this phase, some part of downloading process is completed.
  QUrl redirection_url = reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();

  if (redirection_url.isValid()) {
    // Communication indicates that HTTP redirection is needed.
    // Setup redirection URL and download again.
    QNetworkRequest request = reply->request();
    request.setUrl(redirection_url);

    m_activeReply->deleteLater();
    m_activeReply = NULL;

    runRequest(request);
  }
  else {
    // No redirection is indicated. Final file is obtained
    // in our "reply" object.

    // Read the data into output buffer.
    QByteArray output = reply->readAll();
    QNetworkReply::NetworkError reply_error = reply->error();

    m_activeReply->deleteLater();
    m_activeReply = NULL;

    emit completed(reply_error, output);
  }
}

void Downloader::progressInternal(qint64 bytes_received, qint64 bytes_total) {
  if (m_timer->interval() > 0) {
    m_timer->start();
  }

  emit progress(bytes_received, bytes_total);
}

void Downloader::timeout() {
  if (m_activeReply != NULL) {
    m_activeReply->abort();
  }
}

void Downloader::runRequest(const QNetworkRequest &request) {
  m_timer->start();
  m_activeReply = m_downloadManager->get(request);

  connect(m_activeReply, SIGNAL(downloadProgress(qint64,qint64)),
          this, SLOT(progressInternal(qint64,qint64)));
}
