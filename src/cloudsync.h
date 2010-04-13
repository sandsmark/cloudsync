/*
 * cloudsync.h
 *
 * Copyright (C) 2008 Martin T. Sandsmark <martin.sandsmark@kde.org>
 */
#ifndef CLOUDSYNC_H
#define CLOUDSYNC_H

#include <QSet>

#include <KStatusNotifierItem>

#include "ui_settings.h"

class cloudsyncView;
class QPrinter;
class KToggleAction;
class KUrl;

/**
 * This class serves as the main interface for CloudSync.
 *
 * @short Main window class
 * @author Martin T. Sandsmark <martin.sandsmark@kde.org>
 * @version 1.0
 */
class CloudSync : public KStatusNotifierItem
{
    Q_OBJECT
public:
    /**
     * Default Constructor
     */
    CloudSync();

    /**
     * Default Destructor
     */
    virtual ~CloudSync();

private slots:
    void optionsPreferences();
    void transferring(QString);
    void finished(QString);

private:
    void setupActions();
    void updateTooltip();

private:
    Ui::Settings ui_Settings;

    KToggleAction *m_toolbarAction;
    KToggleAction *m_statusbarAction;

    QSet<QString> m_fileTransfers;
};

#endif // _CLOUDSYNC_H_
