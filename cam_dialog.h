#ifndef CAM_DIALOG_H
#define CAM_DIALOG_H

#include <QDialog>

namespace Ui {
class cam_dialog;
}

class cam_dialog : public QDialog
{
    Q_OBJECT

public:
    explicit cam_dialog(QWidget *parent = nullptr);
    ~cam_dialog();

private:
    Ui::cam_dialog *ui;
};

#endif // CAM_DIALOG_H
