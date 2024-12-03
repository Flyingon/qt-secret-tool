#include "SecretListModel.h"

SecretListModel::SecretListModel(QObject *parent)
    : QAbstractListModel(parent) {}

int SecretListModel::rowCount(const QModelIndex &parent) const {
    return parent.isValid() ? 0 : items.size();
}

QVariant SecretListModel::data(const QModelIndex &index, int role) const {
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

bool SecretListModel::setData(const QModelIndex &index, const QVariant &value, int role) {
    if (index.isValid() && role == Qt::EditRole) {
        items[index.row()].name = value.toString();
        emit dataChanged(index, index, {role});
        return true;
    }
    return false;
}

Qt::ItemFlags SecretListModel::flags(const QModelIndex &index) const {
    return QAbstractListModel::flags(index) | Qt::ItemIsEditable;
}

void SecretListModel::addItem(const SecretItem &item) {
    beginInsertRows(QModelIndex(), items.size(), items.size());
    items.append(item);
    endInsertRows();
}

void SecretListModel::removeItem(int row) {
    if (row < 0 || row >= items.size()) return;
    beginRemoveRows(QModelIndex(), row, row);
    items.removeAt(row);
    endRemoveRows();
}
