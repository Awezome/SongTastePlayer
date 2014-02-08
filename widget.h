#ifndef WIDGET_H
#define WIDGET_H
#include <QWidget>
#include <QMediaPlayer>
#include "stpage.h"
#include <QTime>
#include <QGraphicsScene>
#include <QMenu>
#include <QSystemTrayIcon>
#include <QTableWidget>
#include <QSettings>
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
    STPage *songteste;

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
    Http *http;
    QSystemTrayIcon* trayIcon;

    int downloadingRow;
    void contentMenu();
    void tableContentMenu(const QPoint &pos);
    void showTrayIcon();
    void loadListView();

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

    void setRowColor(QTableWidget *table,int row, QColor textcolor, QColor backcolor);

    void slotLoadList(int type);
    void playerMediaStatus(QMediaPlayer::MediaStatus stats);
    void slotHideList();
signals:
    void signalPlayerMusic(int);
    void signalLoadList(int);
protected:
    void contextMenuEvent(QContextMenuEvent *);
    void mousePressEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *);
    void keyPressEvent(QKeyEvent *k );
};

#endif // WIDGET_H
