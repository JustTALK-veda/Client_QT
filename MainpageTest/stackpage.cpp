#include "stackpage.h"
#include "ui_stackpage.h"

Stackpage::Stackpage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Stackpage)
{
    ui->setupUi(this);
}

Stackpage::~Stackpage()
{
    delete ui;
}
