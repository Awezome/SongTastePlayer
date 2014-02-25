#include "widget.h"
#include <QDebug>
#include <QMouseEvent>
#include <QDesktopServices>
#include <QDir>
#include <QFileDialog>
#include <QDesktopWidget>
#include <QMessageBox>
#include <QTimer>
#include <QShortcut>
#include <QApplication>
#include "config/config.h"
#include "util/download.h"
#include "util/tool.h"
#include "ui.h"

Widget::Widget(QWidget *parent) :QWidget(parent){
    this->setWindowOpacity(1);
    this->setWindowFlags(Qt::FramelessWindowHint);
    this->setAttribute(Qt::WA_TranslucentBackground);
    this->setWindowTitle(ZTitle);
    this->setFixedSize(420,360);
    this->setFocus();
    this->setMouseTracking(true);

    //ui
    setUi();
    this->labelBg->setMouseTracking(true);
    this->musicSlider->setMouseTracking(true);
    this->sliderVolume->setMouseTracking(true);
    this->labelVersion->setText(ZTitle+" "+ZVersion);

    //titleShow
    titleShow();
    createKeys();

    //
    if (!player.isAvailable()) {
        QMessageBox::warning(this,"无法正常播放音乐","系统可能缺少Player必要的组件");
    }

    //get config
    this->settings =new QSettings (QDir::homePath()+"/"+ZConfig, QSettings ::IniFormat);
    if(!settings->contains("Player/downloadDir")){
        settings->setValue("Player/downloadDir",QDir::homePath());
        settings->setValue("Player/musicType",0);
        settings->setValue("Player/volume",50);
    }
    downloadDir=settings->value("Player/downloadDir").toString();
    musicOrder=0;
    this->buttonPlayMode->setToolTip("顺序播放");

    //player
    palyNumber=0;
    musicListSize=0;
    scene=new QGraphicsScene();
    dragPosition=QPoint(-1, -1);//防止鼠标在控件上拖动窗口失效

    //table
    connect(this->tablemusiclist,&QTableWidget::customContextMenuRequested, this, &Widget::tableContentMenu);
    connect(this->tablemusiclist,&QTableWidget::cellDoubleClicked,[this](int row,int){
        emit this->signalPlayerMusic(row);
    });
    connect(this,&Widget::signalLoadList,&Widget::slotLoadList);

    //播放音乐
    connect(this,&Widget::signalPlayerMusic,&Widget::slotPlayMusic);
    connect(&player,&QMediaPlayer::stateChanged,this, &Widget::playerStateChanged);
    connect(&player, &QMediaPlayer::mediaStatusChanged, this, &Widget::playerMediaStatus);

    //进度条
    this->musicSlider->setRange(0, 0);
    connect(this->musicSlider,&QSlider::sliderMoved,[this](int position){
        player.setPosition(position);
    });
    connect(&player, &QMediaPlayer::positionChanged, [this](qint64 position){
        this->musicSlider->setValue(position);
        this->labelCurrentTime->setText(Tool::qint64ToTime(position).toString("mm:ss"));
    });
    connect(&player,  &QMediaPlayer::durationChanged, [this](qint64 duration){
        this->musicSlider->setRange(0, duration);
        this->labelTotalTime->setText(Tool::qint64ToTime(duration).toString("mm:ss"));
    });

    //按钮
    //this->buttonPlay->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
    connect(this->buttonPlay,&QPushButton::clicked,this, &Widget::slotPlayButton);
    connect(this->buttonPre, &QPushButton::clicked,this, &Widget::slotPreButton);
    connect(this->buttonNext,&QPushButton::clicked,this, &Widget::slotNextButton);
    //download dir
    this->pushButtonOpenDir->setText(downloadDir);
    connect(this->pushButtonDownloadDir,&QPushButton::clicked,this, &Widget::slotSetDir);
    connect(this->pushButtonOpenDir,&QPushButton::clicked,[this](){
        QDesktopServices::openUrl(this->downloadDir);
    });
    connect(this->buttonPlayMode,&QPushButton::clicked,this,[this](){
        if(musicOrder==1){
            musicOrder=0;//顺序播放
            this->buttonPlayMode->setStyleSheet(UI::pushBotton("playmode_sequence"));
            this->buttonPlayMode->setToolTip("顺序播放");
        }else{
            musicOrder=1;//单曲循环
            this->buttonPlayMode->setStyleSheet(UI::pushBotton("playmode_repeatone"));
            this->buttonPlayMode->setToolTip("单曲循环");
        }
    });
    this->buttonRefresh->setToolTip("刷新列表");
    connect(this->buttonRefresh,&QPushButton::clicked,this,[this](){
        emit slotLoadList(this->comboMusicType->currentIndex());
    });

    //音量
    this->sliderVolume->setRange(0,100);
    this->sliderVolume->setValue(0);
    connect(this->sliderVolume, &QSlider::valueChanged,[this](int value){
        player.setVolume(value);
        this->settings->setValue("Player/volume",value);
    });
    this->sliderVolume->setValue(settings->value("Player/volume").toInt());
    //contentmenu
    this->contentMenu();
    this->showTrayIcon();
#ifdef Q_OS_WIN
    createTaskbar();
#endif
    //list type
    this->comboMusicType->insertItems(0,STPage::typeLists());
    this->comboMusicType->setCurrentIndex(-1);
    connect(this->comboMusicType,static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),[this](int i){
        settings->setValue("Player/musicType",i);
        slotLoadList(i);
    });//不明白为什么QComboBox要类型转换，其它的都不用，static_cast<void (QComboBox::*)(int)>

    //download
    downloadingRow=-1;//默认-1当大于-1即有文件下载

    //load musiclists
    QTimer::singleShot(0,this, SLOT(initional()));
}

Widget::~Widget(){}

void Widget::initional(){
    this->comboMusicType->setCurrentIndex(settings->value("Player/musicType").toInt());
}

void Widget::slotLoadList(int type){
    this->labelMessage->setText(this->comboMusicType->currentText()+"列表加载中...");

    this->musicLists=STPage::musicLists(type);
    this->musicListSize=this->musicLists.size();
    if(musicListSize>0){
        this->tablemusiclist->setRowCount(musicListSize);
        for(int i=0;i<musicListSize;i++){
            STModel song=this->musicLists.at(i);
            this->tablemusiclist->setItem(i,0,new QTableWidgetItem(song.name));
            this->tablemusiclist->setItem(i,1,new QTableWidgetItem(song.author));
            this->tablemusiclist->setRowHeight(i,22);
        }
        this->labelMessage->setText(this->comboMusicType->currentText()+"列表加载完成");
    }else{
        this->labelMessage->setText(this->comboMusicType->currentText()+"列表加载失败");
    }
    this->palyNumber=0;//刷新列表后重新计数为-1，播放完后会加1，重新开始播放
}

void Widget::slotPlayMusic(int id){
    if(id<0||id>=this->musicListSize){
        return ;
    }

    STModel song=this->musicLists.at(id);qDebug()<<2;
    QString songurl=STPage::songUrl(song.id);
    player.setMedia(QUrl(songurl));
    player.play();

    //设置歌名
    this->labelName->setText(song.name);
    this->labelAuthor->setText(song.author);

    //播放时改变列表中的行颜色
    UI::tableWidgetRowColor(this->tablemusiclist,palyNumber,QColor("#999"),QColor("#fff"));
    this->palyNumber=id;
    UI::tableWidgetRowColor(this->tablemusiclist,id,QColor("#fff"),QColor("#0579C7"));

    //设置头像
    pixmap.loadFromData(STPage::userImage(song.image));
    this->labelImage->setPixmap(pixmap);
}

void Widget::playerStateChanged(QMediaPlayer::State state){
    switch(state) {
    case QMediaPlayer::PlayingState:
        this->buttonPlay->setStyleSheet(UI::pushBotton("button_pause"));
        //this->buttonPlay->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
        break;
    case QMediaPlayer::PausedState:
        this->buttonPlay->setStyleSheet(UI::pushBotton("button_play"));
        //this->buttonPlay->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
        break;
    default:
        this->buttonPlay->setStyleSheet(UI::pushBotton("button_stop"));
        break;
    }
}

void Widget::slotPlayButton(){
    switch(player.state()){
        case QMediaPlayer::PlayingState:
            player.pause();
            break;
        case QMediaPlayer::StoppedState:
            if(palyNumber==0&&this->musicListSize>0){
                emit this->signalPlayerMusic(0);
            }
        default:
            player.play();
            break;
    }
}

void Widget::slotPreButton(){
    if(this->musicListSize>0){
        emit this->signalPlayerMusic(palyNumber-1);
    }
}

void Widget::slotNextButton(){    
    if(this->musicListSize>0){
        emit this->signalPlayerMusic(palyNumber+1);
    }
}

void Widget::playerMediaStatus(QMediaPlayer::MediaStatus stats){
    switch(stats){
        case QMediaPlayer::EndOfMedia:
            if(musicOrder==1){
                emit this->signalPlayerMusic(palyNumber);
            }else{
                emit this->signalPlayerMusic(palyNumber+1);
            }
            break;
        case QMediaPlayer::LoadingMedia:
            this->labelMessage->setText("正在缓冲...");
            break;
        case QMediaPlayer::InvalidMedia:
            player.stop();
            this->labelMessage->setText("无法连接当前音乐");
            break;
        case QMediaPlayer::BufferedMedia:
            this->labelMessage->setText("正在播放...");
            break;
        default:

            break;
    }
    qDebug()<<stats;
}

void Widget::slotHideList(){
    if(this->stackedWidget->isHidden()){
        this->setFixedHeight(360);
        this->labelBg->setFixedHeight(360);
        this->stackedWidget->show();
    }else{
        this->setFixedHeight(60);
        this->labelBg->setFixedHeight(60);
        this->stackedWidget->hide();
    }
}

void Widget::titleShow(){
    this->labelName->show();
    this->labelAuthor->show();
    this->buttonNext->hide();
    this->buttonPre->hide();
    this->buttonPlay->hide();
    this->sliderVolume->hide();
    this->labelVolumeSmall->hide();
}

void Widget::titleHide(){
    this->labelAuthor->hide();
    this->labelName->hide();
    this->labelAuthor->hide();
    this->buttonNext->show();
    this->buttonPre->show();
    this->buttonPlay->show();
    this->sliderVolume->show();
    this->labelVolumeSmall->show();
}

void Widget::downloadMusic(int i){
    STModel song=musicLists.at(i);

    int tsize=this->tableDownloadList->rowCount();

    this->tableDownloadList->setRowCount(tsize+1);
    this->tableDownloadList->setItem(tsize,0,new QTableWidgetItem(song.name));
    this->tableDownloadList->setItem(tsize,1,new QTableWidgetItem("等待下载"));
    this->tableDownloadList->setItem(tsize,2,new QTableWidgetItem(song.id));
    this->tableDownloadList->setItem(tsize,3,new QTableWidgetItem("wait"));
    this->tableDownloadList->setRowHeight(tsize,22);

    if(downloadingRow<0){
        this->downloadManager();
    }
}

void Widget::downloadManager(){
    int tsize=0;
    Download *download=new Download();
    connect(download,&Download::progress,this,&Widget::downloadProgress);
    while(1){
        if(tsize>=this->tableDownloadList->rowCount()){
            break;
        }
        if(this->tableDownloadList->item(tsize,3)->text()=="wait"){
            downloadingRow=tsize;

            QString url=STPage::songUrl(this->tableDownloadList->item(tsize,2)->text());
            QString filename=(this->downloadDir)+"/"+this->tableDownloadList->item(tsize,0)->text().trimmed()+".mp3";
            qDebug()<<"downloading : "<<filename;
            download->run(url,filename);
            this->tableDownloadList->setItem(tsize,1,new QTableWidgetItem("下载完成"));
            this->tableDownloadList->setItem(tsize,3,new QTableWidgetItem("downloaded"));
            UI::tableWidgetRowColor(this->tableDownloadList,tsize,QColor("#999"),QColor("#fff"));
        }
        tsize++;
    }
    download->deleteLater();
    downloadingRow=-1;
}

void Widget::downloadProgress(qint64 recieved, qint64 total){
    QString a=Tool::qint64ToStringKb(recieved)+"KB/"+Tool::qint64ToStringKb(total)+"KB";
    this->tableDownloadList->setItem(downloadingRow,1,new QTableWidgetItem(a));
    UI::tableWidgetRowColor(this->tableDownloadList,downloadingRow,QColor("#fff"),QColor("#0579C7"));
}

void Widget::slotSetDir(){
    QString dir = QFileDialog::getExistingDirectory(this,tr("打开目录"),this->downloadDir,QFileDialog::ShowDirsOnly);
    if(dir!=""){
        downloadDir=dir;
        settings->setValue("Player/downloadDir",dir);
        this->pushButtonOpenDir->setText(dir);
    }
}

//system
void Widget::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton){
        dragPosition = QPoint(-1, -1);
        event->accept();
    }
}

void Widget::mousePressEvent(QMouseEvent * event){
    if (event->button() == Qt::LeftButton){
         dragPosition    = event->globalPos() - frameGeometry().topLeft();
         //globalPos()获取根窗口的相对路径，frameGeometry().topLeft()获取主窗口左上角的位置
         event->accept();   //鼠标事件被系统接收
    }
}

void Widget::mouseMoveEvent(QMouseEvent * event){
    if(1<event->y()&&event->y()<40&&61<event->x()&&event->x()<415){
       titleHide();
    }else{
       titleShow();
    }

    if (event->buttons()==Qt::LeftButton && dragPosition != QPoint(-1, -1)){
        move(event->globalPos() - dragPosition);
        event->accept();
    }
}

//双击显示隐藏列表
void Widget::mouseDoubleClickEvent(QMouseEvent *event){
    slotHideList();
    event->accept();
}

void Widget::contentMenu(){
    trayMenu = new QMenu(this);//创建菜单
    QAction *Tray_quit = new QAction("退出", this);
    Tray_quit->setIcon(UI::icon("power"));
    connect(Tray_quit,&QAction::triggered, [this](){
        this->close();
        QApplication::quit();
    });

    QAction *Tray_homepage = new QAction("检查更新", this);
    Tray_homepage->setIcon(UI::icon("plane-2"));
    connect(Tray_homepage,&QAction::triggered, [this](){
        QDesktopServices::openUrl(QUrl(ZHomepage));
    });

    QAction *menuWindowsMinimized = new QAction("隐藏主界面", this);
    menuWindowsMinimized->setIcon(UI::icon("expand-2"));
    connect(menuWindowsMinimized,&QAction::triggered, [this](){
        this->hide();
    });

    QAction *menuHideList = new QAction("隐藏/显示列表", this);
    menuHideList->setIcon(UI::icon("copy_paste-2"));
    connect(menuHideList,&QAction::triggered, this,&Widget::slotHideList);

    QAction *menuMusiclist = new QAction("音乐列表", this);
    menuMusiclist->setIcon(UI::icon("music-note"));
    connect(menuMusiclist,&QAction::triggered,[this](){
        this->stackedWidget->setCurrentIndex(0);
    });

    QAction *menuDownload = new QAction("下载列表", this);
    menuDownload->setIcon(UI::icon("download"));
    connect(menuDownload,&QAction::triggered, [this](){
        this->stackedWidget->setCurrentIndex(1);
    });

    trayMenu->addAction(menuWindowsMinimized);
    trayMenu->addAction(menuHideList);
    trayMenu->addSeparator();
    trayMenu->addAction(menuMusiclist);
    trayMenu->addAction(menuDownload);
    trayMenu->addSeparator();
    trayMenu->addAction(Tray_homepage);
    trayMenu->addAction(Tray_quit);
}

void Widget::tableContentMenu(const QPoint &pos){
    QAction *downMusic = new QAction("下载歌曲", this);
    downMusic->setIcon(UI::icon("iCloud-down"));
    QAction *playMusic = new QAction("播放歌曲", this);
    playMusic->setIcon(UI::icon("handset"));
    QAction *openUrl = new QAction("打开网页", this);
    openUrl->setIcon(UI::icon("link"));

    QMenu menu(this);
    menu.addAction(playMusic);
    menu.addAction(downMusic);
    menu.addAction(openUrl);

    int row=this->tablemusiclist->itemAt(pos)->row();
    QAction *m=menu.exec(this->tablemusiclist ->viewport()->mapToGlobal(pos));
    if(m==downMusic){
        downloadMusic(row);
    }else if(m==playMusic){
        emit this->signalPlayerMusic(row);
    }else if(m==openUrl){
        QDesktopServices::openUrl(STPage::songWeb(this->musicLists.at(row).id));
    }else{}
}

void Widget::contextMenuEvent(QContextMenuEvent *){
    trayMenu->exec(this->cursor().pos()); //关联到光标
}

void Widget::showTrayIcon(){
    QIcon icon(":/image/logo.png");
    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setIcon(icon);
    trayIcon->show();
    trayIcon->setContextMenu(trayMenu);//将创建菜单作为系统托盘菜单
    trayIcon->setToolTip(ZTitle);
    trayIcon->showMessage(ZTitle,"我在这~~", QSystemTrayIcon::Information, 5000);
    connect(trayIcon,&QSystemTrayIcon::activated, [this](QSystemTrayIcon::ActivationReason reason){
        if (reason == QSystemTrayIcon::Trigger&&this->isHidden()) {
            this->showNormal();
        }
    });
}

void Widget::createKeys(){
    QShortcut *increaseShortcut = new QShortcut(Qt::Key_Up, this);
    connect(increaseShortcut,&QShortcut::activated, [this](){
        this->sliderVolume->triggerAction(QSlider::SliderPageStepAdd);
    });

    QShortcut *decreaseShortcut = new QShortcut(Qt::Key_Down, this);
    connect(decreaseShortcut,&QShortcut::activated, [this](){
        this->sliderVolume->triggerAction(QSlider::SliderPageStepSub);
    });

    QShortcut *toggleShortcut = new QShortcut(Qt::Key_Space, this);
    connect(toggleShortcut, &QShortcut::activated,this,&Widget::slotPlayButton);

    QShortcut *nextShortcut = new QShortcut(Qt::CTRL+Qt::Key_Right, this);
    connect(nextShortcut, &QShortcut::activated,this,&Widget::slotNextButton);

    QShortcut *preShortcut = new QShortcut(Qt::CTRL+Qt::Key_Left, this);
    connect(preShortcut, &QShortcut::activated,this,&Widget::slotPreButton);
}

void Widget::setUi(){
    //global style
    this->setStyleSheet(UI::font()+UI::style());

    labelBg=new QLabel(this);
    labelBg->setGeometry(0,0,420,360);

    musicSlider=new QSlider(this);
    musicSlider->setOrientation(Qt::Horizontal);
    musicSlider->setGeometry(70,40,331,20);

    sliderVolume=new QSlider(this);
    sliderVolume->setOrientation(Qt::Horizontal);
    sliderVolume->setGeometry(340,12,61,16);

    labelName=new QLabel(this);
    labelName->setGeometry(70,10,331,16);
    labelName->setText("SongTeste Player");

    labelTotalTime=new QLabel(this);
    labelTotalTime->setGeometry(370,30,40,16);
    labelTotalTime->setText("00:00");

    labelCurrentTime=new QLabel(this);
    labelCurrentTime->setGeometry(320,30,40,16);
    labelCurrentTime->setText("00:00");

    buttonPlay=new QPushButton(this);
    buttonPlay->setGeometry(150,8,30,30);
    buttonPlay->setCursor(Qt::PointingHandCursor);

    buttonPre=new QPushButton(this);
    buttonPre->setGeometry(110,13,18,20);
    buttonPre->setCursor(Qt::PointingHandCursor);

    buttonNext=new QPushButton(this);
    buttonNext->setGeometry(200,13,18,20);
    buttonNext->setCursor(Qt::PointingHandCursor);

    labelVolumeSmall=new QLabel(this);
    labelVolumeSmall->setGeometry(320,10,17,18);

    labelImage=new QLabel(this);
    labelImage->setGeometry(0,0,60,60);

    labelMessage=new QLabel(this);
    labelMessage->setGeometry(4,336,151,20);

    labelVersion=new QLabel(this);
    labelVersion->setGeometry(203,336,211,20);
    labelVersion->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

    labelAuthor=new QLabel(this);
    labelAuthor->setGeometry(70,30,201,16);

    labelTotalTime_2=new QLabel(this);
    labelTotalTime_2->setGeometry(360,29,16,16);
    labelTotalTime_2->setText("|");

    stackedWidget=new QStackedWidget(this);
    stackedWidget->setGeometry(0,60,420,270);
    stackedWidget->setCurrentIndex(0);

    pageList=new QWidget();

    tablemusiclist=new QTableWidget(pageList);
    tablemusiclist->setGeometry(0,30,420,240);
    tablemusiclist->setColumnCount(2);

    comboMusicType=new QComboBox(pageList);
    comboMusicType->setGeometry(0,0,41,30);
    comboMusicType->setToolTip("更改音乐列表");

    buttonPlayMode=new QPushButton(pageList);
    buttonPlayMode->setGeometry(50,5,20,20);
    buttonPlayMode->setCursor(Qt::PointingHandCursor);

    buttonRefresh=new QPushButton(pageList);
    buttonRefresh->setGeometry(75,5,20,20);
    buttonRefresh->setCursor(Qt::PointingHandCursor);

    pageDownload=new QWidget();

    tableDownloadList=new QTableWidget(pageDownload);
    tableDownloadList->setGeometry(0,30,420,240);
    tableDownloadList->setColumnCount(4);

    pushButtonDownloadDir=new QPushButton(pageDownload);
    pushButtonDownloadDir->setGeometry(0,0,75,30);
    pushButtonDownloadDir->setCursor(Qt::PointingHandCursor);
    pushButtonDownloadDir->setText("更改下载位置");
    pushButtonDownloadDir->setToolTip("更改下载位置");

    pushButtonOpenDir=new QPushButton(pageDownload);
    pushButtonOpenDir->setGeometry(80,0,311,30);
    pushButtonOpenDir->setCursor(Qt::PointingHandCursor);
    pushButtonOpenDir->setToolTip("打开下载位置");

    stackedWidget->addWidget(pageList);
    stackedWidget->addWidget(pageDownload);

    labelBg->setStyleSheet("QLabel{background-color:rgb(0,0,0,210);border-radius:4px;}");
    labelImage->setStyleSheet("QLabel{background: #0863C5 url(:/image/logo.png);}");
    labelName->setStyleSheet("QLabel{color: #fff;font-weight:bold;}");
    labelCurrentTime->setStyleSheet("QLabel{color: #fff;font-weight:bold;}");
    labelVersion->setStyleSheet("QLabel{color: #fff;font-weight:bold;}");
    labelAuthor->setStyleSheet("QLabel{color: #999;font-weight:bold;}");
    labelTotalTime->setStyleSheet("QLabel{color: #999;font-weight:bold;}");
    labelMessage->setStyleSheet("QLabel{color: #0579C7;font-weight:bold;}");
    labelTotalTime_2->setStyleSheet("QLabel{color: #666;font-weight:bold;}");
    labelVolumeSmall->setStyleSheet("QLabel{background-image: url(:/image/volume_small.png);}");

    buttonPre->setStyleSheet("QPushButton{border-image: url(:/image/button_pre.png);}");
    buttonNext->setStyleSheet("QPushButton{border-image: url(:/image/button_next.png);}");
    buttonPlay->setStyleSheet("QPushButton{border-image: url(:/image/button_play.png);}");
    buttonPlayMode->setStyleSheet("QPushButton{border-image: url(:/image/playmode_sequence.png);}");
    buttonRefresh->setStyleSheet("QPushButton{border-image: url(:/image/refresh.png);}"
                                 "QPushButton:hover{border-image: url(:/image/refresh_mouseover.png);}");

    pushButtonDownloadDir->setStyleSheet("QPushButton{border: none;color:#fff;background:transparent;font-weight:bold;}");
    pushButtonOpenDir->setStyleSheet("QPushButton{border: none;color:#fff;background:transparent;text-align:left;font-weight:bold;}");

    //center
    int x=(QApplication::desktop()->width()-width())/2;
    int y=(QApplication::desktop()->height()-height())/2;
    move (x,y);

    UI::tableWidgetView(this->tableDownloadList);
    UI::tableWidgetView(this->tablemusiclist);
    this->tablemusiclist->setColumnWidth(0,340);
    this->tablemusiclist->setColumnWidth(1,70);
    this->tableDownloadList->setColumnWidth(0,310);
    this->tableDownloadList->setColumnWidth(1,100);
    this->tableDownloadList->setColumnWidth(2,60);
    this->tableDownloadList->setColumnWidth(3,30);//下载状态 1,等待，2正在下载，3下载完成 ，下载出错
    this->tableDownloadList->setColumnHidden(2,true);
    this->tableDownloadList->setColumnHidden(3,true);
}

//only for windows taskbar
#ifdef Q_OS_WIN
void Widget::updateTaskbar(){
    switch (player.state()) {
    case QMediaPlayer::PlayingState:
        taskbarButton->setOverlayIcon(style()->standardIcon(QStyle::SP_MediaPlay));
        taskbarProgress->show();
        taskbarProgress->resume();
        break;
    case QMediaPlayer::PausedState:
        taskbarButton->setOverlayIcon(style()->standardIcon(QStyle::SP_MediaPause));
        taskbarProgress->show();
        taskbarProgress->pause();
        break;
    case QMediaPlayer::StoppedState:
        taskbarButton->setOverlayIcon(style()->standardIcon(QStyle::SP_MediaStop));
        taskbarProgress->hide();
        break;
    }
}

void Widget::createTaskbar(){
    if (QtWin::isCompositionEnabled()) {
        QtWin::enableBlurBehindWindow(trayMenu);
    } else {
        QtWin::disableBlurBehindWindow(trayMenu);
    }

    taskbarButton = new QWinTaskbarButton(this);
    taskbarButton->setWindow(windowHandle());

    taskbarProgress = taskbarButton->progress();
    connect(this->musicSlider,&QSlider::valueChanged, taskbarProgress,&QWinTaskbarProgress::setValue);
    connect(this->musicSlider,&QSlider::rangeChanged, taskbarProgress,&QWinTaskbarProgress::setRange);

    connect(&player,&QMediaPlayer::stateChanged, this,&Widget::updateTaskbar);
}
#endif
