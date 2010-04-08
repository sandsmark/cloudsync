/*
 * cloudsync.h
 *
 * Copyright (C) 2008 %{AUTHOR} <%{EMAIL}>
 */
#ifndef CLOUDSYNC_H
#define CLOUDSYNC_H


#include <KStatusNotifierItem>

#include "ui_prefs_base.h"

class cloudsyncView;
class QPrinter;
class KToggleAction;
class KUrl;

/**
 * This class serves as the main window for cloudsync.  It handles the
 * menus, toolbars, and status bars.
 *
 * @short Main window class
 * @author %{AUTHOR} <%{EMAIL}>
 * @version %{VERSION}
 */
class cloudsync : public KStatusNotifierItem
{
    Q_OBJECT
public:
    /**
     * Default Constructor
     */
    cloudsync();

    /**
     * Default Destructor
     */
    virtual ~cloudsync();

private slots:
    void optionsPreferences();
    void download(KUrl file);
    void upload(KUrl file);

private:
    void setupActions();

private:
    Ui::prefs_base ui_prefs_base;

    KToggleAction *m_toolbarAction;
    KToggleAction *m_statusbarAction;
};

#endif // _CLOUDSYNC_H_
