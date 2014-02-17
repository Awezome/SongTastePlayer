#ifndef WIDGET_H
#define WIDGET_H
#include <QWidget>
#include <QMediaPlayer>
#include <QTime>
#include <QGraphicsScene>
#include <QMenu>
#include <QSystemTrayIcon>
#include <QTableWidget>
#include <QSettings>
#include <QtWinExtras>
#include "stpage.h"
#include "ui.h"
namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT
    
public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

private:
    Ui::Widget *ui;
    QList<STModel> musicLists;
    int musicListSize;

    QMediaPlayer player;
    QSettings *settings;

    int palyNumber;
    bool buttonModel;
    QPixmap pixmap;
    QGraphicsScene *scene;

    QPoint dragPosition;
    QString downloadDir;
    int musicOrder;

    QMenu *trayMenu;
    QSystemTrayIcon* trayIcon;
#ifdef Q_OS_WIN
    QWinTaskbarButton* taskbarButton;
    QWinTaskbarProgress* taskbarProgress;
    void createTaskbar();
    void updateTaskbar();
#endif
    int downloadingRow;
    void contentMenu();
    void tableContentMenu(const QPoint &pos);
    void showTrayIcon();
    void createKeys();

    void titleShow();
    void titleHide();

    void downloadMusic(int i);
    void downloadProgress(qint64 recieved, qint64 total);
    void downloadManager();

    void slotSetDir();
    void slotPlayMusic(int id);
    void playerStateChanged(QMediaPlayer::State state);

    void slotPlayButton();
    void slotPreButton();
    void slotNextButton();
    void slotLoadList(int type);
    void playerMediaStatus(QMediaPlayer::MediaStatus stats);
    void slotHideList();
    void setUi();
signals:
    void signalPlayerMusic(int);
    void signalLoadList(int);
private slots:
    void initional();
protected:
    void contextMenuEvent(QContextMenuEvent *);
    void mousePressEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *);
    void mouseDoubleClickEvent(QMouseEvent *event);
};

#endif // WIDGET_H
