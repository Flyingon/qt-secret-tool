#ifndef ADDPASSWORDDIALOG_H
#define ADDPASSWORDDIALOG_H

#include <QDialog>

namespace Ui {
class AddPasswordDialog;
}

class AddPasswordDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit AddPasswordDialog(QWidget *parent = nullptr);
    ~AddPasswordDialog();
    
private:
    Ui::AddPasswordDialog *ui;
};

#endif // ADDPASSWORDDIALOG_H