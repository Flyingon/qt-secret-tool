#ifndef EDITPASSWORDDIALOG_H
#define EDITPASSWORDDIALOG_H

#include <QDialog>

namespace Ui {
class EditPasswordDialog;
}

class EditPasswordDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit EditPasswordDialog(QWidget *parent = nullptr);
    ~EditPasswordDialog();
    
private:
    Ui::EditPasswordDialog *ui;
};

#endif // EDITPASSWORDDIALOG_H