#include "rewritestyleditemdelegate.h"
#include <QStyleOptionViewItem>
#include <QPainter>


RewriteStyledItemDelegate::RewriteStyledItemDelegate()
{

}

void RewriteStyledItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyleOptionViewItem m_opt = option;
    initStyleOption(&m_opt, index);
    m_opt.displayAlignment = Qt::AlignCenter;  // 设置文字居中对齐
    QStyledItemDelegate::paint(painter, m_opt, index);
}

