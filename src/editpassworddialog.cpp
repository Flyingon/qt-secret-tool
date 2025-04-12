#include "editpassworddialog.h"
#include "ui_editpassword.h"

EditPasswordDialog::EditPasswordDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EditPasswordDialog)
{
    ui->setupUi(this);
}

EditPasswordDialog::~EditPasswordDialog()
{
    delete ui;
}