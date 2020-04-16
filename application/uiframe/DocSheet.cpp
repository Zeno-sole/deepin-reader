/*
 * Copyright (C) 2019 ~ 2020 UOS Technology Co., Ltd.
 *
 * Author:     wangzhxiaun
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
#include "DocSheet.h"
#include "docview/commonstruct.h"
#include "pdfControl/SheetSidebarPDF.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QStackedWidget>
#include <QMimeData>
#include <QUuid>

#include "widgets/SpinnerWidget.h"
#include "pdfControl/SheetBrowserPDF.h"
#include "CentralDocPage.h"
#include "app/ProcessController.h"
#include "docview/docummentproxy.h"

QMap<QString, DocSheet *> DocSheet::g_map;
DocSheet::DocSheet(DocType_EM type, DWidget *parent)
    : DSplitter(parent), m_type(type)
{
    if (DocType_PDF == m_type) {
        initPDF();
    }

    connect(this, SIGNAL(splitterMoved(int, int)), this, SLOT(onSplitterMoved(int, int)));

    m_uuid = QUuid::createUuid().toString();
    g_map[m_uuid] = this;
}

DocSheet::~DocSheet()
{
    g_map.remove(m_uuid);
}

void DocSheet::handleShortcut(QString shortcut)
{
    CentralDocPage *doc = static_cast<CentralDocPage *>(parent());
    if (nullptr == doc)
        return;

    doc->OnAppShortCut(shortcut);
}

void DocSheet::openFile(const QString &filePath)
{
    if (DocType_PDF == m_type)
        static_cast<SheetBrowserPDF *>(m_browser)->OpenFilePath(filePath);
}

void DocSheet::pageJump(int page)
{
    if (DocType_PDF == m_type) {
        DocummentProxy *_proxy =  static_cast<SheetBrowserPDF *>(m_browser)->GetDocProxy();
        if (_proxy) {
            int nPageSize = _proxy->getPageSNum();      //  总页数
            if (page < 0 || page == nPageSize) {
                return;
            }

            int nCurPage = _proxy->currentPageNo();
            if (nCurPage != page) {
                _proxy->pageJump(page);
            }
        }
    }
}

void DocSheet::pageFirst()
{
    if (DocType_PDF == m_type) {
        pageJump(0);
    }
}

void DocSheet::pageLast()
{
    if (DocType_PDF == m_type) {
        pageJump(getDocProxy()->getPageSNum() - 1);
    }
}

void DocSheet::pageNext()
{
    if (DocType_PDF == m_type) {

        bool isDoubleShow = static_cast<SheetBrowserPDF *>(m_browser)->isDoubleShow();

        int nCurPage = getDocProxy()->currentPageNo();

        int page = nCurPage + (isDoubleShow ? 2 : 1);

        page = (page >= (getDocProxy()->getPageSNum() - 1) ? (getDocProxy()->getPageSNum() - 1) : page);

        pageJump(page);
    }
}

void DocSheet::pagePrev()
{
    if (DocType_PDF == m_type) {

        bool isDoubleShow = static_cast<SheetBrowserPDF *>(m_browser)->isDoubleShow();

        int nCurPage = getDocProxy()->currentPageNo();

        int page = nCurPage - (isDoubleShow ? 2 : 1);

        pageJump(page);
    }
}

void DocSheet::setDoubleShow(bool isShow)
{
    if (DocType_PDF == m_type)
        static_cast<SheetBrowserPDF *>(m_browser)->setDoubleShow(isShow);
}

void DocSheet::setRotateLeft()
{
    if (DocType_PDF == m_type)
        static_cast<SheetBrowserPDF *>(m_browser)->setRotateLeft();
}

void DocSheet::setRotateRight()
{
    if (DocType_PDF == m_type)
        static_cast<SheetBrowserPDF *>(m_browser)->setRotateRight();
}

void DocSheet::setFileChanged(bool hasChanged)
{
    if (DocType_PDF == m_type)
        static_cast<SheetBrowserPDF *>(m_browser)->setFileChanged(hasChanged);
}

void DocSheet::setMouseDefault()
{
    if (DocType_PDF == m_type)
        static_cast<SheetBrowserPDF *>(m_browser)->setMouseDefault();
}

void DocSheet::setMouseHand()
{
    if (DocType_PDF == m_type)
        static_cast<SheetBrowserPDF *>(m_browser)->setMouseHand();
}

void DocSheet::setScale(double scale)
{
    if (DocType_PDF == m_type)
        static_cast<SheetBrowserPDF *>(m_browser)->setScale(scale);
}

void DocSheet::setFit(int fit)
{
    if (DocType_PDF == m_type)
        static_cast<SheetBrowserPDF *>(m_browser)->setFit(fit);
}

void DocSheet::setBookMark(int page, int state)
{
    if (DocType_PDF == m_type) {
        static_cast<SheetSidebarPDF *>(m_sidebar)->setBookMark(page, state);
        static_cast<SheetBrowserPDF *>(m_browser)->setBookMark(page, state);
    }
}

void DocSheet::showNoteWidget(int page, const QString &uuid, const int &type)
{
    if (DocType_PDF == m_type) {
        static_cast<SheetBrowserPDF *>(m_browser)->showNoteWidget(page, uuid, type);
    }
}

bool DocSheet::isMouseHand()
{
    if (DocType_PDF == m_type)
        return static_cast<SheetBrowserPDF *>(m_browser)->isMouseHand();
    return false;
}

bool DocSheet::isDoubleShow()
{
    if (DocType_PDF == m_type)
        return static_cast<SheetBrowserPDF *>(m_browser)->isDoubleShow();
    return false;
}

void DocSheet::initPDF()
{
    m_pRightWidget = new QStackedWidget(this);
    m_pSpinnerWidget = new SpinnerWidget(this);

    setHandleWidth(5);
    setChildrenCollapsible(false);  //  子部件不可拉伸到 0

    SheetBrowserPDF *browser = new SheetBrowserPDF(this, this);
    SheetSidebarPDF *sidebar = new SheetSidebarPDF(this);

    m_sidebar = sidebar;
    m_browser = browser;

    connect(sidebar, SIGNAL(sigDeleteAnntation(const int &, const QString &)), browser, SIGNAL(sigDeleteAnntation(const int &, const QString &)));
    connect(sidebar, SIGNAL(sigFindNone()), browser, SLOT(onFindNone()));
    connect(browser, SIGNAL(sigFileOpenResult(const QString &, const bool &)), SLOT(SlotFileOpenResult(const QString &, const bool &)));
    connect(browser, SIGNAL(sigFindOperation(const int &)), this, SIGNAL(sigFindOperation(const int &)));
    connect(browser, SIGNAL(sigFindOperation(const int &)), sidebar, SLOT(onSearch(const int &)));
    connect(browser, SIGNAL(sigAnntationMsg(const int &, const QString &)), sidebar, SIGNAL(sigAnntationMsg(const int &, const QString &)));
    connect(browser, SIGNAL(sigUpdateThumbnail(const int &)), sidebar, SIGNAL(sigUpdateThumbnail(const int &)));
    connect(browser, SIGNAL(sigFileChanged()), this, SLOT(onFileChanged()));
    connect(browser, SIGNAL(sigRotateChanged(int)), sidebar, SLOT(onRotate(int)));
    connect(browser, SIGNAL(sigPageChanged(int)), sidebar, SLOT(onPageChanged(int)));

    int tW = 36;
    int tH = 36;
    dApp->adaptScreenView(tW, tH);
    m_pSpinnerWidget->setSpinnerSize(QSize(tW, tH));
    m_pSpinnerWidget->startSpinner();

    m_pRightWidget->addWidget(m_pSpinnerWidget);
    m_pRightWidget->addWidget(browser);

    addWidget(sidebar);
    addWidget(m_pRightWidget);

    QList<int> list_src;
    tW = LEFTNORMALWIDTH;
    dApp->adaptScreenView(tW, tH);
    list_src.append(tW);
    tW = 1000 - LEFTNORMALWIDTH;
    dApp->adaptScreenView(tW, tH);
    list_src.append(tW);

    setSizes(list_src);

    setAcceptDrops(true);

    //文档刚打开时，模拟鼠标点击文档区域事件
    QPoint pos(m_pRightWidget->geometry().x() + 10, m_pRightWidget->geometry().y() + 10);
    QMouseEvent event0(QEvent::MouseButtonPress, pos, Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
    QApplication::sendEvent(m_pRightWidget, &event0);
}

void DocSheet::SlotFileOpenResult(const QString &s, const bool &res)
{
    if (res) {
        if (m_pRightWidget && m_pSpinnerWidget) {
            m_pRightWidget->removeWidget(m_pSpinnerWidget);

            delete  m_pSpinnerWidget;
            m_pSpinnerWidget = nullptr;
        }

        handleOpenSuccess();
    }

    emit sigOpened(this, res);
    emit sigOpenFileResult(s, res);
}

void DocSheet::handleOpenSuccess()
{
    if (DocType_PDF == m_type) {
        SheetSidebarPDF *sidebar = static_cast<SheetSidebarPDF *>(m_sidebar);
        if (sidebar) sidebar->handleOpenSuccess();
    }
}

void DocSheet::setSidebarVisible(bool isVisible)
{
    if (DocType_PDF == m_type) {
        SheetSidebarPDF *sidebar = static_cast<SheetSidebarPDF *>(m_sidebar);
        if (sidebar) sidebar->setVisible(isVisible);
    }
}

void DocSheet::onShowTips(const QString &tips, int index)
{
    showTips(tips, index);
}

void DocSheet::onFileChanged()
{
    emit sigFileChanged(this);
}

void DocSheet::onSplitterMoved(int a, int b)
{
    setFit(0);
    emit sigFileChanged(this);
}

void DocSheet::onTitleShortCut(QString shortCut)
{
    if (shortCut == KeyStr::g_ctrl_b) {
        auto side = this->findChild<SheetSidebarPDF *>();
        if (side) {
            side->qDealWithShortKey(shortCut);
        }
    } else {
        auto brower = this->findChild<SheetBrowserPDF *>();
        if (brower) {
            brower->qDealWithShortKey(shortCut);
        }
    }
}

QUuid DocSheet::getUuid(DocSheet *sheet)
{
    return g_map.key(sheet);
}

DocSheet *DocSheet::getSheet(QString uuid)
{
    if (g_map.contains(uuid))
        return g_map[uuid];

    return nullptr;
}

QString DocSheet::filePath()
{
    if (DocType_PDF == m_type)
        return static_cast<SheetBrowserPDF *>(m_browser)->getFilePath();

    return "";
}

int DocSheet::qGetFileChange()
{
    int istatus = -1;
    if (nullptr != static_cast<SheetBrowserPDF *>(m_browser))
        istatus = static_cast<SheetBrowserPDF *>(m_browser)->getFileChange() ? 1 : 0;
    return  istatus;
}

void DocSheet::saveOper()
{
    if (DocType_PDF == m_type)
        static_cast<SheetBrowserPDF *>(m_browser)->saveOper();
}

bool DocSheet::saveData()
{
    if (DocType_PDF == m_type)
        return static_cast<SheetBrowserPDF *>(m_browser)->saveData();

    return false;
}

bool DocSheet::saveAsData(QString filePath)
{
    if (DocType_PDF == m_type)
        return static_cast<SheetBrowserPDF *>(m_browser)->saveAsData(filePath);

    return false;
}

void DocSheet::setData(const int &type, const QVariant &value)
{
    if (DocType_PDF == m_type)
        static_cast<SheetBrowserPDF *>(m_browser)->setOper(type, value);
}

QVariant DocSheet::getOper(int type)
{
    if (DocType_PDF == m_type)
        return static_cast<SheetBrowserPDF *>(m_browser)->getOper(type);

    return -1;
}

DocummentProxy *DocSheet::getDocProxy()
{
    if (DocType_PDF == m_type) {
        SheetBrowserPDF *browser = static_cast<SheetBrowserPDF *>(m_browser);
        if (browser)
            return browser->GetDocProxy();
    }

    return nullptr;
}

void DocSheet::OnOpenSliderShow()
{
    if (DocType_PDF == m_type) {
        m_bOldState = static_cast<SheetSidebarPDF *>(m_sidebar)->isVisible();
        static_cast<SheetSidebarPDF *>(m_sidebar)->setVisible(false);
    }
}

void DocSheet::OnExitSliderShow()
{
    if (DocType_PDF == m_type)
        static_cast<SheetSidebarPDF *>(m_sidebar)->setVisible(m_bOldState);
}

void DocSheet::ShowFindWidget()
{
    if (DocType_PDF == m_type)
        static_cast<SheetBrowserPDF *>(m_browser)->ShowFindWidget();
}

DocType_EM DocSheet::type()
{
    return m_type;
}

void DocSheet::showTips(const QString &tips, int iconIndex)
{
    CentralDocPage *doc = static_cast<CentralDocPage *>(parent());
    if (nullptr == doc)
        return;

    doc->showTips(tips, iconIndex);
}

void DocSheet::setCurrentState(int state)
{
    CentralDocPage *doc = static_cast<CentralDocPage *>(parent());
    if (nullptr == doc)
        return;

    return doc->setCurrentState(state);
}

int DocSheet::getCurrentState()
{
    CentralDocPage *doc = static_cast<CentralDocPage *>(parent());
    if (nullptr == doc)
        return Default_State;

    return doc->getCurrentState();
}

void DocSheet::openSlide()
{
    CentralDocPage *doc = static_cast<CentralDocPage *>(parent());
    if (nullptr == doc)
        return;

    doc->OnOpenSliderShow();
}

void DocSheet::showControl()
{
    CentralDocPage *doc = static_cast<CentralDocPage *>(parent());
    if (nullptr == doc)
        return;

    doc->showPlayControlWidget();
}

void DocSheet::quitSlide()
{
    CentralDocPage *doc = static_cast<CentralDocPage *>(parent());
    if (nullptr == doc)
        return;

    doc->OnExitSliderShow();
}

void DocSheet::openMagnifer()
{
    CentralDocPage *doc = static_cast<CentralDocPage *>(parent());
    if (nullptr == doc)
        return;

    doc->OnOpenMagnifer();
}

void DocSheet::quitMagnifer()
{
    CentralDocPage *doc = static_cast<CentralDocPage *>(parent());
    if (nullptr == doc)
        return;

    doc->OnExitMagnifer();
}
