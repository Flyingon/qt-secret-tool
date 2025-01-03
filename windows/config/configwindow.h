#pragma once

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QSettings>
#include <QMessageBox>
#include "../../config/settingmanager.h"

class ConfigWindow : public QWidget
{
Q_OBJECT

public:
    explicit ConfigWindow(QWidget *parent = nullptr);

private slots:
    void onSelectDatabasePath();
    void onSaveConfig();

private:
    void loadConfig();

    QLabel *dbPathLabel;
    QLineEdit *dbPathLineEdit;
    QPushButton *dbPathButton;

    QPushButton *saveButton;
};
