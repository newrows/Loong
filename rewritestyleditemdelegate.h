#ifndef REWRITESTYLEDITEMDELEGATE_H
#define REWRITESTYLEDITEMDELEGATE_H

#include <QStyledItemDelegate>

class RewriteStyledItemDelegate : public QStyledItemDelegate
{
public:
    RewriteStyledItemDelegate();

    //重写一个绘制函数，实现单元格的自定义目标
    void paint(QPainter* painter, const QStyleOptionViewItem& option,
               const QModelIndex& index) const override;

};

#endif // REWRITESTYLEDITEMDELEGATE_H
