/*
 * cloudsyncview.h
 *
 * Copyright (C) 2007 %{AUTHOR} <%{EMAIL}>
 */
#ifndef CLOUDSYNCVIEW_H
#define CLOUDSYNCVIEW_H

#include <QtGui/QWidget>

#include "ui_cloudsyncview_base.h"

class QPainter;
class KUrl;

/**
 * This is the main view class for cloudsync.  Most of the non-menu,
 * non-toolbar, and non-statusbar (e.g., non frame) GUI code should go
 * here.
 *
 * @short Main view
 * @author %{AUTHOR} <%{EMAIL}>
 * @version %{VERSION}
 */

class cloudsyncView : public QWidget, public Ui::cloudsyncview_base
{
    Q_OBJECT
public:
    /**
     * Default constructor
     */
    cloudsyncView(QWidget *parent);

    /**
     * Destructor
     */
    virtual ~cloudsyncView();

private:
    Ui::cloudsyncview_base ui_cloudsyncview_base;

signals:
    /**
     * Use this signal to change the content of the statusbar
     */
    void signalChangeStatusbar(const QString& text);

    /**
     * Use this signal to change the content of the caption
     */
    void signalChangeCaption(const QString& text);

private slots:
    void switchColors();
    void settingsChanged();
};

#endif // cloudsyncVIEW_H
