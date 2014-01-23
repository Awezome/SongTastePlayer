#include "widget.h"
#include "ui_widget.h"
#include <QDebug>


Widget::Widget(QWidget *parent) :QWidget(parent),ui(new Ui::Widget){

    ui->setupUi(this);
    this->songteste =new STPage();

    this->setWindowOpacity(1);
    this->setWindowFlags(Qt::FramelessWindowHint);
    //setWindowFlags(Qt::WindowMinimizeButtonHint);
    this->setAttribute(Qt::WA_TranslucentBackground);
    this->setFixedSize(530,450);

    palyNumber=0;
    buttonModel=false;//是否为点击了下一个或上一个，做为标记，会影响正常下的顺序播放。暂时的。
    scene=new QGraphicsScene();

    //table
    this->loadListView();
    this->loadListData();
    connect(ui->tablemusiclist,SIGNAL(cellDoubleClicked(int,int)),this, SLOT( getTableItem(int,int)) );

    //播放音乐
    connect(this,SIGNAL(signalPlayerMusic(int)),this,SLOT(slotPlayMusic(int)));
    connect(&player,SIGNAL(stateChanged(QMediaPlayer::State)),this, SLOT(playerStateChanged(QMediaPlayer::State)));

    //进度条
    ui->musicSlider->setRange(0, 0);
    connect(ui->musicSlider, SIGNAL(sliderMoved(int)),this, SLOT(setPosition(int)));
    connect(&player, SIGNAL(positionChanged(qint64)), this, SLOT(positionChanged(qint64)));
    connect(&player, SIGNAL(durationChanged(qint64)), this, SLOT(durationChanged(qint64)));

    //按钮
    //ui->buttonPlay->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
    connect(ui->buttonPlay, SIGNAL(clicked()),this, SLOT(slotPlayButton()));
    connect(ui->buttonPre, SIGNAL(clicked()),this, SLOT(slotPreButton()));
    connect(ui->buttonNext, SIGNAL(clicked()),this, SLOT(slotNextButton()));

    //音量
    player.setVolume(50);
    ui->sliderVolume->setRange(0,100);
    ui->sliderVolume->setValue(50);
    connect(ui->sliderVolume, SIGNAL(valueChanged(int)),this, SLOT(updateVolume(int)));
}

Widget::~Widget(){
    delete ui;
}

void Widget::loadListView(){
    ui->tablemusiclist->setSelectionBehavior(QAbstractItemView::SelectRows); //整行选中的方式
    ui->tablemusiclist->setEditTriggers(QAbstractItemView::NoEditTriggers);//禁止编辑
    ui->tablemusiclist->setSelectionMode(QAbstractItemView::SingleSelection); //设置为可以选中多个目标
    ui->tablemusiclist->verticalHeader()->setVisible(false); //隐藏行号
    ui->tablemusiclist->horizontalHeader()->setVisible(false); //隐藏行表头
    ui->tablemusiclist->setColumnWidth(0,360);
    ui->tablemusiclist->setColumnWidth(1,90);
    ui->tablemusiclist->setShowGrid(false);
    ui->tablemusiclist->setFocusPolicy(Qt::NoFocus);
    ui->tablemusiclist->setStyleSheet("selection-background-color:#D8FAA5");  //设置选中行颜色


}

void Widget::loadListData(){
    this->musicLists=this->songteste->musicLists();
    int listsize=this->musicLists.size();
    ui->tablemusiclist->setRowCount(listsize);
    for(int i=0;i<listsize;i++){
        STModel song=this->musicLists.at(i);
        this->ui->tablemusiclist->setItem(i,0,new QTableWidgetItem(song.name));
        this->ui->tablemusiclist->setItem(i,1,new QTableWidgetItem(song.author));
        this->ui->tablemusiclist->setRowHeight(i,22);
    }
}

void Widget::getTableItem(int row, int column){
    emit this->signalPlayerMusic(row);
}

void Widget::setPosition(int position){
    player.setPosition(position);
}

void Widget::positionChanged(qint64 position){
     this->updateTime(position);
     ui->musicSlider->setValue(position);
}

void Widget::durationChanged(qint64 duration){
    ui->musicSlider->setRange(0, duration);
    ui->labelTotalTime->setText(qint64ToTime(duration).toString("mm:ss"));
}

void Widget::updateTime(qint64 currentTimeNumber){
    QString str = qint64ToTime(currentTimeNumber).toString("mm:ss");
    ui->labelCurrentTime->setText(str);
}

QTime Widget::qint64ToTime(qint64 time){
    //3个参数分别代表时，分，秒；60000毫秒为1分钟，所以分钟第二个参数是先除6000,第3个参数是直接除1s
    QTime t(0,(time/60000)%60,(time/1000)%60);
    return t;
}

void Widget::updateVolume(int volume){
    player.setVolume(volume);
}

void Widget::setRowColor(int row, QColor textcolor,QColor backcolor){
    int size=2;//ui->tablemusiclist->columnCount();
    QTableWidgetItem *item;
    for (int col=0; col<size; col++){
        item = ui->tablemusiclist->item(row, col);
        item->setBackgroundColor(backcolor);
        item->setTextColor(textcolor);
    }
}

void Widget::slotPlayMusic(int id){
    int size=musicLists.size()-1;
    id=id>size?0:id;
    id=id<0?size:id;

    STModel song=this->musicLists.at(id);
    QString songurl=this->songteste->songUrl(song.id);

    player.setMedia(QUrl(songurl));
    player.play();

    //
    buttonModel=false;

    //设置歌名
    ui->labelName->setText(song.name);

    //播放时改变列表中的行颜色
    setRowColor(palyNumber,QColor("#000"),QColor("#fff"));
    this->palyNumber=id;
    setRowColor(id,QColor("#fff"),QColor("#81C300"));

    //设置头像
    pixmap.loadFromData(songteste->userImage(song.image));
    scene->addPixmap(pixmap);
    ui->graphicsImage->setScene(scene);
}

void Widget::playerStateChanged(QMediaPlayer::State state){
    switch(state) {
    case QMediaPlayer::PlayingState:
        ui->buttonPlay->setStyleSheet("QPushButton{border-image: url(:/image/button_pause.png);}");
        //ui->buttonPlay->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
        break;
    case QMediaPlayer::PausedState:
        ui->buttonPlay->setStyleSheet("QPushButton{border-image: url(:/image/button_play.png);}");
        //ui->buttonPlay->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
        break;
    default:
        if(!buttonModel){
            emit this->signalPlayerMusic(palyNumber+1);
        }
        break;
    }
}

void Widget::slotPlayButton(){
    switch(player.state()){
        case QMediaPlayer::PlayingState:
            player.pause();
            break;
        case QMediaPlayer::StoppedState:
            if(palyNumber==0){
                emit this->signalPlayerMusic(0);
            }
        default:
            player.play();
            break;
    }
}

void Widget::slotPreButton(){
    buttonModel=true;
    emit this->signalPlayerMusic(palyNumber-1);
}

void Widget::slotNextButton(){
    buttonModel=true;
    emit this->signalPlayerMusic(palyNumber+1);
}

