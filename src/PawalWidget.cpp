#include "PawalWidget.h"

#include <QThreadPool>
#include <QFile>
#include <QGraphicsPixmapItem>
#include <QMessageBox>

#include <pawal/AnosuApi.hpp>

PawalWidget::PawalWidget(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui_PawalWidget)
    , api_(new (std::nothrow) pawal::AnosuApi{})
{
    if(!this->api_)
    {
        throw std::bad_alloc{};
    }
    ui->setupUi(this);
    QIcon icon{tr("./favicon.ico")};
    this->setWindowIcon(icon);
#ifndef Q_OS_ANDROID
    this->resize(640,640);
    ui->gridLayoutWidget->resize(640,640);
#endif
    ui->keywordEdit_->hide();
    ui->lookupButton_->hide();
    QMargins margins{ui->verticalLayout->contentsMargins()};
    margins.setTop(this->height() - 50);
    ui->verticalLayout->setContentsMargins(margins);
    ui->imageView_->hide();
    ui->saveButton_->hide();
    connect(ui->horLookupButton_,&QPushButton::clicked,this,&PawalWidget::OnHorLookupClicked);
    connect(ui->lookupButton_,&QPushButton::clicked,this,&PawalWidget::OnLookupClicked);
    connect(this,&PawalWidget::ShowImage,this,&PawalWidget::OnShowImage);
    connect(ui->saveButton_,&QPushButton::clicked,this,&PawalWidget::OnSaveClicked);
    connect(this,&PawalWidget::SavingCompleted,this,&PawalWidget::OnSavingCompledted);
}

void PawalWidget::OnHorLookupClicked()
{
    QString text{this->ui->horKeywordEdit_->toPlainText()};
    std::string keyword{text.toStdString()};
    this->ui->horLookupButton_->setEnabled(false);
    this->ui->horLookupButton_->setText(tr("Loading"));
    QThreadPool::globalInstance()->start(std::bind(&PawalWidget::InvokeApiAndShow,this,keyword));
}

void PawalWidget::OnLookupClicked()
{
    QString text{this->ui->keywordEdit_->toPlainText()};
    std::string keyword{text.toStdString()};
    this->ui->lookupButton_->setEnabled(false);
    this->ui->lookupButton_->setText(tr("Loading"));
    QThreadPool::globalInstance()->start(std::bind(&PawalWidget::InvokeApiAndShow,this,keyword));
}

void PawalWidget::ChangeUi()
{
    QMargins margins{ui->verticalLayout->contentsMargins()};
    margins.setTop(5);
    ui->verticalLayout->setContentsMargins(margins);
    ui->imageView_->setTransformationAnchor(QGraphicsView::AnchorViewCenter);
    ui->imageView_->show();
    ui->keywordEdit_->show();
    ui->lookupButton_->show();
    ui->horKeywordEdit_->hide();
    ui->horLookupButton_->hide();
    ui->saveButton_->show();
}

void PawalWidget::OnShowImage(std::vector<char> imageBytes)
{
    if(imageBytes.empty())
    {
        if(!this->ui->imageView_->isVisible())
        {
            this->ui->horLookupButton_->setText(tr("Lookup"));
            this->ui->horLookupButton_->setEnabled(true);
        }
        else
        {
            this->ui->lookupButton_->setText(tr("Lookup"));
            this->ui->lookupButton_->setEnabled(true);
        }
        QMessageBox::warning(this,tr("Failed to lookup image"),tr("Cannot find any image about keyword"));
        return;
    }
    if(!this->ui->imageView_->isVisible())
    {
        this->ChangeUi();
    }
    uchar *data{reinterpret_cast<uchar*>(imageBytes.data())};
    QPixmap bitmap;
    bitmap.loadFromData(data,static_cast<uint>(imageBytes.size()));
    QGraphicsScene *scene{this->ui->imageView_->scene()};
    if(!scene)
    {
        scene = new (std::nothrow) QGraphicsScene{};
        this->ui->imageView_->setScene(scene);
        scene->addPixmap(bitmap);
        qreal height{static_cast<qreal>(this->ui->imageView_->height())};
        qreal widght{static_cast<qreal>(this->ui->imageView_->width())};
        this->ui->imageView_->scale(widght/bitmap.width(),height/bitmap.height());
    }
    else
    {
        scene->clear();
        scene->addPixmap(bitmap);
    }
    this->ui->lookupButton_->setEnabled(true);
    this->ui->lookupButton_->setText(tr("Lookup"));
    this->ui->keywordEdit_->clear();
}

void PawalWidget::OnSavingCompledted(QString path)
{
    this->ui->saveButton_->setEnabled(true);
    this->ui->saveButton_->setText(tr("Save"));
    if(path.isEmpty())
    {
        QMessageBox::warning(this,tr("Save Failed"),tr("Unable to save file"));
    }
}

void PawalWidget::SaveFile(QPixmap pixmap)
{
    for(std::uint32_t i = 0;i != (std::numeric_limits<std::uint32_t>::max)();++i)
    {
        QString name{tr("./")};
        name = name + QString::number(i) + tr(".png");
        QFile file{name};
        if(!file.exists())
        {
            pixmap.save(name);
            emit SavingCompleted(std::move(name));
            return;
        }
    }
    emit SavingCompleted(QString{});
}

void PawalWidget::OnSaveClicked()
{
    QGraphicsScene *scene{this->ui->imageView_->scene()};
    if(!scene || scene->items().size() != 1)
    {
        return;
    }
    QGraphicsPixmapItem *pixmap{qgraphicsitem_cast<QGraphicsPixmapItem*>(scene->items().front())};
    if(pixmap)
    {
        this->ui->saveButton_->setText(tr("Saving"));
        this->ui->saveButton_->setEnabled(false);
        QThreadPool::globalInstance()->start(std::bind(&PawalWidget::SaveFile,this,pixmap->pixmap()));
    }
}

void PawalWidget::InvokeApiAndShow(std::string keyword)
{
    std::vector<char> imageBytes{this->api_->LookupImage(keyword)};
    emit ShowImage(std::move(imageBytes));
}

PawalWidget::~PawalWidget()
{
    QGraphicsScene *scene{this->ui->imageView_->scene()};
    if(scene)
    {
        delete scene;
    }
    delete ui; 
}
