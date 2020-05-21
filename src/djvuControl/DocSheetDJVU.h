#ifndef DOCSHEETDJVU_H
#define DOCSHEETDJVU_H

#include "DocSheet.h"

class SheetBrowserDJVU;
class DocSheetDJVU : public DocSheet
{
    Q_OBJECT
    Q_DISABLE_COPY(DocSheetDJVU)

public:
    explicit DocSheetDJVU(QString filePath, QWidget *parent = nullptr);

    ~DocSheetDJVU()override;

    bool openFileExec()override;

    bool saveAsData(QString filePath)override;

    int pagesNumber()override;

    int currentPage()override;      //从1开始

    int currentIndex()override;     //从0开始

    void jumpToPage(int page)override;

    void jumpToIndex(int index)override;

    void jumpToFirstPage()override;

    void jumpToLastPage()override;

    void jumpToNextPage()override;

    void jumpToPrevPage()override;

    void rotateLeft()override;

    void rotateRight()override;

    void setLayoutMode(Dr::LayoutMode mode)override;

    void setScaleFactor(qreal scaleFactor)override;

    void setMouseShape(Dr::MouseShape shape)override;

    void setScaleMode(Dr::ScaleMode mode)override;

    bool getImage(int index, QImage &image, double width, double height) override;

    bool getImageMax(int index, QImage &image, double max)override;

    bool fileChanged()override;

    void openMagnifier()override;

    bool closeMagnifier()override;

    void docBasicInfo(stFileInfo &info)override;

    QString filter()override;

private slots:
    void onBrowserPageChanged(int page);

    void onBrowserSizeChanged();

    void onBrowserWheelUp();

    void onBrowserWheelDown();

private:
    SheetBrowserDJVU *m_browser = nullptr;
};

#endif // DOCSHEETDJVU_H
