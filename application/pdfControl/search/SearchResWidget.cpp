/*
 * Copyright (C) 2019 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     duanxiaohui
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "SearchResWidget.h"
#include "docview/docummentproxy.h"
#include "DocSheet.h"

SearchResWidget::SearchResWidget(DocSheet *sheet, DWidget *parent)
    : CustomWidget(parent), m_sheet(sheet)
{
    initWidget();
    initConnections();
}

SearchResWidget::~SearchResWidget()
{

}

//  查询有数据, 则填充
void SearchResWidget::handFindContentComming(const stSearchRes &search)
{
    int resultNum = 0;
    QString strText = "";

    int page = static_cast<int>(search.ipage);
    foreach (QString s, search.listtext) {
        strText += s.trimmed();
        strText += QString("    ");
        ++resultNum;
    }

    addSearchsItem(page, strText, resultNum);
}

int  SearchResWidget::handleFindFinished()
{
    int count = m_pSearchList->count();;
    if (count == 0) {      //  无结果
        showTips();     //此处会增加一个count
    }

    return count;
}

void SearchResWidget::clearFindResult()
{
    m_pSearchList->clear();
}

void SearchResWidget::slotSelectItem(QListWidgetItem *item)
{
    if (item == nullptr) {
        return;
    }

    setSelectItemBackColor(item);
}

void SearchResWidget::initWidget()
{
    auto m_pVLayout = new QVBoxLayout;
    m_pVLayout->setContentsMargins(0, 8, 0, 0);
    m_pVLayout->setSpacing(0);
    this->setLayout(m_pVLayout);

    m_pSearchList = new CustomListWidget(m_sheet, this);
    connect(m_pSearchList, SIGNAL(sigSelectItem(QListWidgetItem *)), SLOT(slotSelectItem(QListWidgetItem *)));

    m_pVLayout->addWidget(m_pSearchList);
}

void SearchResWidget::initConnections()
{
}

void SearchResWidget::addSearchsItem(const int &page, const QString &text, const int &resultNum)
{
    if (nullptr == m_sheet)
        return;

    auto item = m_pSearchList->insertWidgetItem(page);
    if (item) {
        item->setFlags(Qt::NoItemFlags);
        int tW = LEFTMINWIDTH;
        int tH = 80;
        item->setSizeHint(QSize(tW, tH));

        auto itemWidget = new SearchItemWidget(this);

        itemWidget->setLabelPage(page, 1);
        itemWidget->setSerchResultText(tr("%1 items found").arg(resultNum));
        tW = LEFTMINWIDTH - 5;
        tH = 80;
        itemWidget->setMinimumSize(QSize(tW, tH));
        itemWidget->setTextEditText(text);

        auto dproxy = m_sheet->getDocProxy();
        if (nullptr != dproxy) {
            QImage image;
            int tW = 146;
            int tH = 174;
            bool bl = dproxy->getImage(page, image, tW, tH /*42, 62*/);
            if (bl) {
                itemWidget->setLabelImage(image);
            }
        }

        m_pSearchList->setItemWidget(item, itemWidget);
    }
}

void SearchResWidget::showTips()
{
    auto hLayout = new QHBoxLayout;
    auto vLayout = new QVBoxLayout;
    auto tipWidget = new DWidget;
    auto tipLab = new DLabel(tr("No search results"));
    tipLab->setForegroundRole(/*DPalette::TextTips*/ QPalette::ToolTipText);
    DFontSizeManager::instance()->bind(tipLab, DFontSizeManager::T6);
    int tW = 226;
    int tH = 528;
    tipWidget->setMinimumSize(QSize(tW, tH));

    hLayout->addStretch(1);
    hLayout->addWidget(tipLab);
    hLayout->addStretch(1);
    vLayout->addStretch(1);
    vLayout->addItem(hLayout);
    vLayout->addStretch(1);
    tipWidget->setLayout(vLayout);

    auto item = new QListWidgetItem;
    item->setSizeHint(QSize(tW, tH));
    item->setFlags(Qt::NoItemFlags);
    m_pSearchList->addItem(item);
    m_pSearchList->setItemWidget(item, tipWidget);
}

void SearchResWidget::setSelectItemBackColor(QListWidgetItem *item)
{
    if (item == nullptr) {
        return;
    }

    clearItemColor();

    m_pSearchList->setCurrentItem(item);

    auto t_widget = reinterpret_cast<SearchItemWidget *>(m_pSearchList->itemWidget(item));
    if (t_widget) {
        t_widget->setBSelect(true);
    }
}

void SearchResWidget::clearItemColor()
{
    if (m_pSearchList == nullptr)
        return;
    auto pCurItem = m_pSearchList->currentItem();
    if (pCurItem) {
        auto pItemWidget =
            reinterpret_cast<SearchItemWidget *>(m_pSearchList->itemWidget(pCurItem));
        if (pItemWidget) {
            pItemWidget->setBSelect(false);
        }
    }
}

SearchItemWidget *SearchResWidget::getItemWidget(QListWidgetItem *item)
{
    if (m_pSearchList == nullptr) {
        return nullptr;
    }
    auto pWidget = qobject_cast<SearchItemWidget *>(m_pSearchList->itemWidget(item));
    if (pWidget) {
        return pWidget;
    }
    return nullptr;
}

/**
 * @brief SearchResWidget::adaptWindowSize
 * @param scale
 */
void SearchResWidget::adaptWindowSize(const double &scale)
{
    m_scale = scale;

    int tW = LEFTMINWIDTH;
    int tH = 80;

    double width = static_cast<double>(tW) * scale;
    double height = static_cast<double>(tH) * scale;

    if (m_pSearchList) {
        int itemCount = 0;
        itemCount = m_pSearchList->count();
        if (itemCount > 0) {
            for (int index = 0; index < itemCount; index++) {
                auto item = m_pSearchList->item(index);
                if (item) {
                    auto itemWidget = getItemWidget(item);
                    if (itemWidget) {
                        item->setSizeHint(QSize(static_cast<int>(width), static_cast<int>(height)));
                        itemWidget->adaptWindowSize(scale);
                    }
                }
            }
        }
    }
}

/**
 * @brief SearchResWidget::updateThumbnail
 * 高亮操作之后要跟换相应的缩略图
 * @param page 页码数，从0开始
 */
void SearchResWidget::updateThumbnail(const int &page)
{
    if (nullptr == m_sheet)
        return;

    if (m_pSearchList == nullptr) {
        return;
    }
    int itemNum = 0;
    itemNum = m_pSearchList->count();
    if (itemNum <= 0) {
        return;
    }
    QImage image;

    int tW = 146;
    int tH = 174;

    auto dproxy = m_sheet->getDocProxy();
    dproxy->getImage(page, image, tW, tH);
    for (int index = 0; index < itemNum; index++) {
        auto item = m_pSearchList->item(index);
        if (item) {
            auto itemWidget = getItemWidget(item);
            if (itemWidget) {
                if (itemWidget->nPageIndex() == page) {
                    if (nullptr == dproxy) {
                        return;
                    }
                    itemWidget->setLabelImage(image);
                }
            }
        }
    }
}
