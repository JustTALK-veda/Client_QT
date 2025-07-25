#include "meetinglocal.h"
#include "ui_meetinglocal.h"

meetingLocal::meetingLocal(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::meetingLocal)
{
    ui->setupUi(this);
}

meetingLocal::~meetingLocal()
{
    delete ui;
}
