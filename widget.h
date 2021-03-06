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
#include "stpage.h"
#include <QLabel>
#include <QStackedWidget>
#include <QPushButton>
#include <QComboBox>
#ifdef Q_OS_WIN
#include <QtWinExtras>
#endif

class Widget : public QWidget
{
    Q_OBJECT
    
public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

private:
    //
    QLabel *labelBg;
    QSlider *musicSlider;
    QSlider *sliderVolume;
    QLabel *labelName;
    QLabel *labelTotalTime;
    QPushButton *buttonPlay;
    QPushButton *buttonPre;
    QPushButton *buttonNext;
    QLabel *labelCurrentTime;
    QLabel *labelVolumeSmall;
    QLabel *labelImage;
    QLabel *labelMessage;
    QLabel *labelVersion;
    QLabel *labelAuthor;
    QLabel *labelTotalTime_2;
    QStackedWidget *stackedWidget;

    QWidget *pageList;
    QTableWidget *tablemusiclist;
    QComboBox *comboMusicType;
    QPushButton *buttonPlayMode;
    QPushButton *buttonRefresh;

    QWidget *pageDownload;
    QTableWidget *tableDownloadList;
    QPushButton *pushButtonDownloadDir;
    QPushButton *pushButtonOpenDir;

    //
    QList<STModel> musicLists;
    int musicListSize;

    QMediaPlayer player;
    QSettings *settings;

    int palyNumber;
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
