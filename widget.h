#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QMediaPlayer>
#include "stpage.h"
#include <QTime>
#include <QGraphicsScene>
#include <QMenu>

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT
    
public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

    void contextMenuEvent(QContextMenuEvent *);
    
private:
    Ui::Widget *ui;
    QList<STModel> musicLists;
    int musicListSize;
    STPage *songteste;

    void loadListView();


    QMediaPlayer player;

    void updateTime(qint64 currentTimeNumber);
    QTime qint64ToTime(qint64 time);

    int palyNumber;
    bool buttonModel;
    QPixmap pixmap;
    QGraphicsScene *scene;

    QPoint dragPosition;

    QMenu *trayMenu;
    void contentMenu();
private slots:
    void slotPlayMusic(int id);

    void getTableItem(int row,int column);

    void setPosition(int position);
    void positionChanged(qint64 position);
    void durationChanged(qint64 duration);
    void playerStateChanged(QMediaPlayer::State state);

    void slotPlayButton();
    void slotPreButton();
    void slotNextButton();

    void updateVolume(int volume);
    void setRowColor(int row, QColor textcolor, QColor backcolor);

    void slotLoadList();

    void slotChangelog();
    void slotHomepage();
    void slotQuit();
    void playerError();

    void playerMediaStatus(QMediaPlayer::MediaStatus stats);
signals:
    void signalPlayerMusic(int);
    void signalLoadList();
protected:
    void mousePressEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);
};

#endif // WIDGET_H