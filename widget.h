#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QMediaPlayer>
#include "stpage.h"
#include <QTime>
#include <QGraphicsScene>

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
    STPage *songteste;

    QTime totalTime;

    void loadListView();
    void loadListData();

    QMediaPlayer *player;

    void updateTime(qint64 currentTimeNumber);
    QTime qint64ToTime(qint64 time);

    int palyNumber;
    bool buttonModel;
    QPixmap pixmap;
    QGraphicsScene *scene;
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
    void setRowColor(int row, QColor color);

signals:
    void signalPlayerMusic(int id);

};

#endif // WIDGET_H
