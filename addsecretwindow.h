#ifndef ADDSECRETWINDOW_H
#define ADDSECRETWINDOW_H

#include <QWidget>
#include <QAbstractButton>

namespace Ui {
class AddSecretWindow;
}

class AddSecretWindow : public QWidget
{
    Q_OBJECT

public:
    explicit AddSecretWindow(QWidget *parent = nullptr);
    ~AddSecretWindow();

private slots:
    void on_buttonBox_accepted();

    void on_buttonBox_clicked(QAbstractButton *button);

private:
    Ui::AddSecretWindow *ui;
};

#endif // ADDSECRETWINDOW_H
