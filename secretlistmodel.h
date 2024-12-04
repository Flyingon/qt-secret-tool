#ifndef SECRETLISTMODEL_H
#define SECRETLISTMODEL_H

#include <QAbstractListModel>
#include <QVariant>

struct SecretItem {
    QString key;
    QString secret;
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
    void clearAllItems();  // 新增清理所有item的方法声明
private:
    QList<SecretItem> items;
};

#endif // SECRETLISTMODEL_H
