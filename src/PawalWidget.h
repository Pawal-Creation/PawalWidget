#pragma once
#include "ui_PawalWidget.h"
#include <QWidget>

#include <pawal/IPawalApi.hpp>

class PawalWidget : public QWidget {
    Q_OBJECT
    
private:
    Ui_PawalWidget* ui;
    std::unique_ptr<pawal::IPawalApi> api_;
    qreal lastHeight_;
    qreal lastWidth_;

    void ChangeUi();

    void OnHorLookupClicked();

    void OnLookupClicked();

    void OnShowImage(std::vector<char> image);

    void InvokeApiAndShow(std::string keyword);

    void SaveFile(QPixmap pixmap);

    void OnSaveClicked();

    void OnSavingCompledted(QString path);

signals: 
    void ShowImage(std::vector<char> imageBytes);

    void SavingCompleted(QString path);
public:
    PawalWidget(QWidget* parent = nullptr);
    ~PawalWidget();
};