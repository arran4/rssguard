// For license of this file, see <project-root-folder>/LICENSE.md.

#ifndef LITEHTMLVIEWER_H
#define LITEHTMLVIEWER_H

#include "3rd-party/qlitehtml/qlitehtmlwidget.h"
#include "gui/webviewer.h"

#include "network-web/adblock/adblockmanager.h"

class Downloader;
class QWheelEvent;
class QMenu;

class LiteHtmlViewer : public QLiteHtmlWidget, public WebViewer {
  Q_OBJECT

  public:
    explicit LiteHtmlViewer(QWidget* parent = nullptr);

  public:
    virtual void bindToBrowser(WebBrowser* browser);
    virtual void findText(const QString& text, bool backwards);
    virtual void setUrl(const QUrl& url);
    virtual void setHtml(const QString& html, const QUrl& base_url = {});
    virtual QString html() const;
    virtual QUrl url() const;
    virtual void clear();
    virtual void loadMessages(const QList<Message>& messages, RootItem* root);
    virtual double verticalScrollBarPosition() const;
    virtual void setVerticalScrollBarPosition(double pos);
    virtual void applyFont(const QFont& fon);
    virtual bool canZoomIn() const;
    virtual bool canZoomOut() const;
    virtual qreal zoomFactor() const;
    virtual void zoomIn();
    virtual void zoomOut();
    virtual void setZoomFactor(qreal zoom_factor);

  private slots:
    void selectedTextChanged(bool available);
    void showContextMenu(const QPoint& pos, const QUrl& url);

  signals:
    void zoomFactorChanged();
    void titleChanged(const QString& new_title);
    void urlChanged(const QUrl& url);
    void loadStarted();
    void loadProgress(int progress);
    void loadFinished(bool success);

  protected:
    virtual void keyPressEvent(QKeyEvent* event);

  private:
    BlockingResult blockedWithAdblock(const QUrl& url);
    QByteArray handleResource(const QUrl& url);

  private:
    QScopedPointer<Downloader> m_downloader;
    QScopedPointer<QMenu> m_contextMenu;
};

#endif // LITEHTMLVIEWER_H
