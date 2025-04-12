#include "addpassworddialog.h"
#include "ui_addpassword.h"

AddPasswordDialog::AddPasswordDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddPasswordDialog)
{
    ui->setupUi(this);
}

AddPasswordDialog::~AddPasswordDialog()
{
    delete ui;
}