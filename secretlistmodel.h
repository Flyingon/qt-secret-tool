#ifndef SECRETLISTMODEL_H
#define SECRETLISTMODEL_H

#include <QAbstractListModel>
#include <QVariant>

struct SecretItem {
    QString name;
    QString details;
};

class SecretListModel : public QAbstractListModel {
    Q_OBJECT

public:
    explicit SecretListModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;

    void addItem(const SecretItem &item);
    void removeItem(int row);

private:
    QList<SecretItem> items;
};

#endif // SECRETLISTMODEL_H
