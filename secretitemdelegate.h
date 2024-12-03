#ifndef SECRETITEMDELEGATE_H
#define SECRETITEMDELEGATE_H

#include <QStyledItemDelegate>
#include <QPainter>
#include <QPushButton>
#include <QApplication>  // 确保 QApplication 被包含
#include <QMouseEvent>
#include <QPainter>
#include <QStyle>
#include <QStyleOptionButton>

class SecretItemDelegate : public QStyledItemDelegate {
    Q_OBJECT

public:
    explicit SecretItemDelegate(QObject *parent = nullptr) : QStyledItemDelegate(parent) {}

    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override
    {
        // 设置每个item的高度像素
        return QSize(option.rect.width(), 40);
    }

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override {
        QStyledItemDelegate::paint(painter, option, index);

        QStyleOptionButton editButton;
        editButton.rect = QRect(option.rect.right() - 100, option.rect.top(), 40, option.rect.height());
        editButton.text = "Edit";
        editButton.state = QStyle::State_Enabled;
        // 设置按钮文样式
        editButton.features |= QStyleOptionButton::Flat;
        editButton.state |= QStyle::State_Raised;
        editButton.state |= QStyle::State_KeyboardFocusChange;
        editButton.palette.setColor(QPalette::Button, Qt::white);
        editButton.palette.setColor(QPalette::ButtonText, Qt::black);

        QStyleOptionButton deleteButton;
        deleteButton.rect = QRect(option.rect.right() - 50, option.rect.top(), 40, option.rect.height());
        deleteButton.text = "Del";
        deleteButton.state = QStyle::State_Enabled;
        // 设置按钮文样式
        deleteButton.features |= QStyleOptionButton::Flat;
        deleteButton.state |= QStyle::State_Raised;
        deleteButton.state |= QStyle::State_KeyboardFocusChange;
        deleteButton.palette.setColor(QPalette::Button, Qt::white);
        deleteButton.palette.setColor(QPalette::ButtonText, Qt::black);

        QStyle *style = QApplication::style();
        style->drawControl(QStyle::CE_PushButton, &editButton, painter);
        style->drawControl(QStyle::CE_PushButton, &deleteButton, painter);
    }

    bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index) override {
        if (event->type() == QEvent::MouseButtonPress) {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
            QRect editRect(option.rect.right() - 100, option.rect.top(), 40, option.rect.height());
            QRect deleteRect(option.rect.right() - 50, option.rect.top(), 40, option.rect.height());

            if (editRect.contains(mouseEvent->pos())) {
                emit editClicked(index);
                return true;
            } else if (deleteRect.contains(mouseEvent->pos())) {
                emit deleteClicked(index);
                return true;
            }
        } else if (event->type() == QEvent::MouseButtonDblClick) {
            emit editClicked(index);
            return true;
        }
        return false;
    }

signals:
    void editClicked(const QModelIndex &index);
    void deleteClicked(const QModelIndex &index);
};

#endif // SECRETITEMDELEGATE_H
