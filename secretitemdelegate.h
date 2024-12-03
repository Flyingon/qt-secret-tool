#ifndef SECRETITEMDELEGATE_H
#define SECRETITEMDELEGATE_H

#include <QStyledItemDelegate>
#include <QPainter>
#include <QPushButton>
#include <QApplication>
#include <QMouseEvent>
#include <QStyle>
#include <QStyleOptionButton>

class SecretItemDelegate : public QStyledItemDelegate {
    Q_OBJECT

public:
    explicit SecretItemDelegate(QObject *parent = nullptr);

    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index) override;

signals:
    void editClicked(const QModelIndex &index);
    void deleteClicked(const QModelIndex &index);
};

#endif // SECRETITEMDELEGATE_H
