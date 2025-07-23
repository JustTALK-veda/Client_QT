#include "cam_dialog.h"
#include "ui_cam_dialog.h"

cam_dialog::cam_dialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::cam_dialog)
{
    ui->setupUi(this);
}

cam_dialog::~cam_dialog()
{
    delete ui;
}
