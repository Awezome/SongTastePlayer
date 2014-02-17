#include "widget.h"
#include "ui_widget.h"
#include <QDebug>
#include <QMouseEvent>
#include <QDesktopServices>
#include <QDir>
#include <QFileDialog>
#include <QDesktopWidget>
#include <QMessageBox>
#include <QTimer>
#include <QShortcut>
#include "config.h"
#include "download.h"
#include "tool.h"

Widget::Widget(QWidget *parent) :QWidget(parent),ui(new Ui::Widget),
    taskbarButton(0), taskbarProgress(0)
{
    ui->setupUi(this);
    this->setWindowOpacity(1);
    this->setWindowFlags(Qt::FramelessWindowHint);
    this->setAttribute(Qt::WA_TranslucentBackground);
    this->setWindowTitle(ZTitle);
    this->setFixedSize(420,360);
    this->setFocus();
    this->setMouseTracking(true);
    ui->labelBg->setMouseTracking(true);
    ui->musicSlider->setMouseTracking(true);
    ui->sliderVolume->setMouseTracking(true);
    ui->labelVersion->setText(ZTitle+" "+ZVersion);

    //ui
    setUi();

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
    ui->buttonPlayMode->setToolTip("顺序播放");

    //player
    palyNumber=0;
    musicListSize=0;
    buttonModel=false;//是否为点击了下一个或上一个，做为标记，会影响正常下的顺序播放。暂时的。
    scene=new QGraphicsScene();
    dragPosition=QPoint(-1, -1);//防止鼠标在控件上拖动窗口失效

    //table
    connect(ui->tablemusiclist,&QTableWidget::customContextMenuRequested, this, &Widget::tableContentMenu);
    connect(ui->tablemusiclist,&QTableWidget::cellDoubleClicked,[this](int row,int){
        emit this->signalPlayerMusic(row);
    });
    connect(this,&Widget::signalLoadList,&Widget::slotLoadList);

    //播放音乐
    connect(this,&Widget::signalPlayerMusic,&Widget::slotPlayMusic);
    connect(&player,&QMediaPlayer::stateChanged,this, &Widget::playerStateChanged);
    connect(&player, &QMediaPlayer::mediaStatusChanged, this, &Widget::playerMediaStatus);

    //进度条
    ui->musicSlider->setRange(0, 0);
    connect(ui->musicSlider,&QSlider::sliderMoved,[this](int position){
        player.setPosition(position);
    });
    connect(&player, &QMediaPlayer::positionChanged, [this](qint64 position){
        ui->musicSlider->setValue(position);
        ui->labelCurrentTime->setText(Tool::qint64ToTime(position).toString("mm:ss"));
    });
    connect(&player,  &QMediaPlayer::durationChanged, [this](qint64 duration){
        ui->musicSlider->setRange(0, duration);
        ui->labelTotalTime->setText(Tool::qint64ToTime(duration).toString("mm:ss"));
    });

    //按钮
    //ui->buttonPlay->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
    connect(ui->buttonPlay,&QPushButton::clicked,this, &Widget::slotPlayButton);
    connect(ui->buttonPre, &QPushButton::clicked,this, &Widget::slotPreButton);
    connect(ui->buttonNext,&QPushButton::clicked,this, &Widget::slotNextButton);
    //download dir
    ui->pushButtonOpenDir->setText(downloadDir);
    connect(ui->pushButtonDownloadDir,&QPushButton::clicked,this, &Widget::slotSetDir);
    connect(ui->pushButtonOpenDir,&QPushButton::clicked,[this](){
        QDesktopServices::openUrl(this->downloadDir);
    });
    connect(ui->buttonPlayMode,&QPushButton::clicked,this,[this](){
        if(musicOrder==1){
            musicOrder=0;//顺序播放
            ui->buttonPlayMode->setStyleSheet(UI::pushBotton("playmode_sequence"));
            ui->buttonPlayMode->setToolTip("顺序播放");
        }else{
            musicOrder=1;//单曲循环
            ui->buttonPlayMode->setStyleSheet(UI::pushBotton("playmode_repeatone"));
            ui->buttonPlayMode->setToolTip("单曲循环");
        }
    });
    ui->buttonRefresh->setToolTip("刷新列表");
    connect(ui->buttonRefresh,&QPushButton::clicked,this,[this](){
        emit slotLoadList(ui->comboMusicType->currentIndex());
    });

    //音量
    ui->sliderVolume->setRange(0,100);
    ui->sliderVolume->setValue(0);
    connect(ui->sliderVolume, &QSlider::valueChanged,[this](int value){
        player.setVolume(value);
        this->settings->setValue("Player/volume",value);
    });
    ui->sliderVolume->setValue(settings->value("Player/volume").toInt());
    //contentmenu
    this->contentMenu();
    this->showTrayIcon();
#ifdef Q_OS_WIN
    createTaskbar();
#endif
    //list type
    ui->comboMusicType->insertItems(0,STPage::typeLists());
    ui->comboMusicType->setCurrentIndex(-1);
    connect(ui->comboMusicType,static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),[this](int i){
        settings->setValue("Player/musicType",i);
        slotLoadList(i);
    });//不明白为什么QComboBox要类型转换，其它的都不用，static_cast<void (QComboBox::*)(int)>

    //download
    downloadingRow=-1;//默认-1当大于-1即有文件下载

    //load musiclists
    QTimer::singleShot(0,this, SLOT(initional()));
}

Widget::~Widget(){
    delete ui;
}

void Widget::initional(){
    ui->comboMusicType->setCurrentIndex(settings->value("Player/musicType").toInt());
}

void Widget::slotLoadList(int type){
    ui->labelMessage->setText(ui->comboMusicType->currentText()+"列表加载中...");

    this->musicLists=STPage::musicLists(type);
    this->musicListSize=this->musicLists.size();
    if(musicListSize>0){
        ui->tablemusiclist->setRowCount(musicListSize);
        for(int i=0;i<musicListSize;i++){
            STModel song=this->musicLists.at(i);
            this->ui->tablemusiclist->setItem(i,0,new QTableWidgetItem(song.name));
            this->ui->tablemusiclist->setItem(i,1,new QTableWidgetItem(song.author));
            this->ui->tablemusiclist->setRowHeight(i,22);
        }
        ui->labelMessage->setText(ui->comboMusicType->currentText()+"列表加载完成");
    }else{
        ui->labelMessage->setText(ui->comboMusicType->currentText()+"列表加载失败");
    }
    this->palyNumber=0;//刷新列表后重新计数为-1，播放完后会加1，重新开始播放
}

void Widget::slotPlayMusic(int id){
    if(id<0||id>=this->musicListSize){
        return ;
    }

    STModel song=this->musicLists.at(id);
    QString songurl=STPage::songUrl(song.id);

    player.setMedia(QUrl(songurl));
    player.play();

    //
    buttonModel=false;

    //设置歌名
    ui->labelName->setText(song.name);
    ui->labelAuthor->setText(song.author);

    //播放时改变列表中的行颜色
    UI::tableWidgetRowColor(ui->tablemusiclist,palyNumber,QColor("#999"),QColor("#fff"));
    this->palyNumber=id;
    UI::tableWidgetRowColor(ui->tablemusiclist,id,QColor("#fff"),QColor("#0579C7"));

    //设置头像
    pixmap.loadFromData(STPage::userImage(song.image));
    ui->labelImage->setPixmap(pixmap);
}

void Widget::playerStateChanged(QMediaPlayer::State state){
    switch(state) {
    case QMediaPlayer::PlayingState:
        ui->buttonPlay->setStyleSheet(UI::pushBotton("button_pause"));
        //ui->buttonPlay->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
        break;
    case QMediaPlayer::PausedState:
        ui->buttonPlay->setStyleSheet(UI::pushBotton("button_play"));
        //ui->buttonPlay->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
        break;
    default:
        ui->buttonPlay->setStyleSheet(UI::pushBotton("button_stop"));
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
        buttonModel=true;
        emit this->signalPlayerMusic(palyNumber-1);
    }
}

void Widget::slotNextButton(){    
    if(this->musicListSize>0){
        buttonModel=true;
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
            ui->labelMessage->setText("正在缓冲...");
            break;
        case QMediaPlayer::InvalidMedia:
            player.stop();
            ui->labelMessage->setText("无法连接当前音乐");
            break;
        case QMediaPlayer::BufferedMedia:
            ui->labelMessage->setText("正在播放...");
            break;
        default:

            break;
    }
    qDebug()<<stats;
}

void Widget::slotHideList(){
    if(ui->stackedWidget->isHidden()){
        this->setFixedHeight(360);
        ui->labelBg->setFixedHeight(360);
        ui->stackedWidget->show();
    }else{
        this->setFixedHeight(60);
        ui->labelBg->setFixedHeight(60);
        ui->stackedWidget->hide();
    }
}

void Widget::titleShow(){
    ui->labelName->show();
    ui->labelAuthor->show();
    ui->buttonNext->hide();
    ui->buttonPre->hide();
    ui->buttonPlay->hide();
    ui->sliderVolume->hide();
    ui->labelVolumeSmall->hide();
}

void Widget::titleHide(){
    ui->labelAuthor->hide();
    ui->labelName->hide();
    ui->labelAuthor->hide();
    ui->buttonNext->show();
    ui->buttonPre->show();
    ui->buttonPlay->show();
    ui->sliderVolume->show();
    ui->labelVolumeSmall->show();
}

void Widget::downloadMusic(int i){
    STModel song=musicLists.at(i);

    int tsize=ui->tableDownloadList->rowCount();

    ui->tableDownloadList->setRowCount(tsize+1);
    ui->tableDownloadList->setItem(tsize,0,new QTableWidgetItem(song.name));
    ui->tableDownloadList->setItem(tsize,1,new QTableWidgetItem("等待下载"));
    ui->tableDownloadList->setItem(tsize,2,new QTableWidgetItem(song.id));
    ui->tableDownloadList->setItem(tsize,3,new QTableWidgetItem("wait"));
    ui->tableDownloadList->setRowHeight(tsize,22);

    if(downloadingRow<0){
        this->downloadManager();
    }
}

void Widget::downloadManager(){
    int tsize=0;
    Download *download=new Download();
    connect(download,&Download::progress,this,&Widget::downloadProgress);
    while(1){
        if(tsize>=ui->tableDownloadList->rowCount()){
            break;
        }
        if(ui->tableDownloadList->item(tsize,3)->text()=="wait"){
            downloadingRow=tsize;

            QString url=STPage::songUrl(ui->tableDownloadList->item(tsize,2)->text());
            QString filename=(this->downloadDir)+"/"+ui->tableDownloadList->item(tsize,0)->text().trimmed()+".mp3";
            qDebug()<<"downloading : "<<filename;
            download->run(url,filename);
            ui->tableDownloadList->setItem(tsize,1,new QTableWidgetItem("下载完成"));
            ui->tableDownloadList->setItem(tsize,3,new QTableWidgetItem("downloaded"));
            UI::tableWidgetRowColor(ui->tableDownloadList,tsize,QColor("#999"),QColor("#fff"));
        }
        tsize++;
    }
    download->deleteLater();
    downloadingRow=-1;
}

void Widget::downloadProgress(qint64 recieved, qint64 total){
    QString a=Tool::qint64ToStringKb(recieved)+"KB/"+Tool::qint64ToStringKb(total)+"KB";
    ui->tableDownloadList->setItem(downloadingRow,1,new QTableWidgetItem(a));
    UI::tableWidgetRowColor(ui->tableDownloadList,downloadingRow,QColor("#fff"),QColor("#0579C7"));
}

void Widget::slotSetDir(){
    QString dir = QFileDialog::getExistingDirectory(this,tr("打开目录"),this->downloadDir,QFileDialog::ShowDirsOnly);
    if(dir!=""){
        downloadDir=dir;
        settings->setValue("Player/downloadDir",dir);
        ui->pushButtonOpenDir->setText(dir);
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
    //Tray_quit->setIcon(QIcon(":/image/image/delete.png"));
    connect(Tray_quit,&QAction::triggered, [this](){
        this->close();
        QApplication::quit();
    });

    QAction *Tray_homepage = new QAction("检查更新", this);
    //Tray_flux_day->setIcon(QIcon(":/image/image/checkmark.png"));
    connect(Tray_homepage,&QAction::triggered, [this](){
        QDesktopServices::openUrl(QUrl(ZHomepage));
    });

    QAction *menuWindowsMinimized = new QAction("隐藏主界面", this);
    connect(menuWindowsMinimized,&QAction::triggered, [this](){
        this->hide();
    });

    QAction *menuHideList = new QAction("隐藏/显示列表", this);
    connect(menuHideList,&QAction::triggered, this,&Widget::slotHideList);

    QAction *menuMusiclist = new QAction("音乐列表", this);
    connect(menuMusiclist,&QAction::triggered,[this](){
        ui->stackedWidget->setCurrentIndex(0);
    });

    QAction *menuDownload = new QAction("下载列表", this);
    connect(menuDownload,&QAction::triggered, [this](){
        ui->stackedWidget->setCurrentIndex(1);
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
    QAction *playMusic = new QAction("播放歌曲", this);
    QAction *openUrl = new QAction("打开网页", this);

    QMenu menu(this);
    menu.addAction(playMusic);
    menu.addAction(downMusic);
    menu.addAction(openUrl);

    int row=ui->tablemusiclist->itemAt(pos)->row();
    QAction *m=menu.exec(ui->tablemusiclist ->viewport()->mapToGlobal(pos));
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
        ui->sliderVolume->triggerAction(QSlider::SliderPageStepAdd);
    });

    QShortcut *decreaseShortcut = new QShortcut(Qt::Key_Down, this);
    connect(decreaseShortcut,&QShortcut::activated, [this](){
        ui->sliderVolume->triggerAction(QSlider::SliderPageStepSub);
    });

    QShortcut *toggleShortcut = new QShortcut(Qt::Key_Space, this);
    connect(toggleShortcut, &QShortcut::activated,this,&Widget::slotPlayButton);

    QShortcut *nextShortcut = new QShortcut(Qt::CTRL+Qt::Key_Right, this);
    connect(nextShortcut, &QShortcut::activated,this,&Widget::slotNextButton);

    QShortcut *preShortcut = new QShortcut(Qt::CTRL+Qt::Key_Left, this);
    connect(preShortcut, &QShortcut::activated,this,&Widget::slotPreButton);
}

void Widget::setUi(){
    this->setStyleSheet(UI::font());
    //center
    int x=(QApplication::desktop()->width()-width())/2;
    int y=(QApplication::desktop()->height()-height())/2;
    move (x,y);

    UI::tableWidgetView(ui->tableDownloadList);
    UI::tableWidgetView(ui->tablemusiclist);
    ui->tablemusiclist->setColumnWidth(0,340);
    ui->tablemusiclist->setColumnWidth(1,70);
    ui->tableDownloadList->setColumnWidth(0,310);
    ui->tableDownloadList->setColumnWidth(1,100);
    ui->tableDownloadList->setColumnWidth(2,60);
    ui->tableDownloadList->setColumnWidth(3,30);//下载状态 1,等待，2正在下载，3下载完成 ，下载出错
    ui->tableDownloadList->setColumnHidden(2,true);
    ui->tableDownloadList->setColumnHidden(3,true);
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
    connect(ui->musicSlider,&QSlider::valueChanged, taskbarProgress,&QWinTaskbarProgress::setValue);
    connect(ui->musicSlider,&QSlider::rangeChanged, taskbarProgress,&QWinTaskbarProgress::setRange);

    connect(&player,&QMediaPlayer::stateChanged, this,&Widget::updateTaskbar);
}
#endif
