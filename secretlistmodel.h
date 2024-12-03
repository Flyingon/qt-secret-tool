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
    explicit SecretListModel(QObject *parent = nullptr) : QAbstractListModel(parent) {}

    int rowCount(const QModelIndex &parent = QModelIndex()) const override {
        return parent.isValid() ? 0 : items.size();
    }

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override {
        if (!index.isValid() || index.row() >= items.size())
            return {};

        const SecretItem &item = items.at(index.row());
        if (role == Qt::DisplayRole) {
            return item.name;
        } else if (role == Qt::UserRole) {
            return item.details;
        }
        return {};
    }

    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override {
        if (index.isValid() && role == Qt::EditRole) {
            items[index.row()].name = value.toString();
            emit dataChanged(index, index, {role});
            return true;
        }
        return false;
    }

    Qt::ItemFlags flags(const QModelIndex &index) const override {
        return QAbstractListModel::flags(index) | Qt::ItemIsEditable;
    }

    void addItem(const SecretItem &item) {
        beginInsertRows(QModelIndex(), items.size(), items.size());
        items.append(item);
        endInsertRows();
    }

    void removeItem(int row) {
        if (row < 0 || row >= items.size()) return;
        beginRemoveRows(QModelIndex(), row, row);
        items.removeAt(row);
        endRemoveRows();
    }

private:
    QList<SecretItem> items;
};


#endif // SECRETLISTMODEL_H
