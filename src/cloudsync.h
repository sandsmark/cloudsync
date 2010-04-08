/*
 * cloudsync.h
 *
 * Copyright (C) 2008 Martin T. Sandsmark <martin.sandsmark@kde.org>
 */
#ifndef CLOUDSYNC_H
#define CLOUDSYNC_H


#include <KStatusNotifierItem>
#include <QSet>
#include <KIO/CopyJob>

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
    void download(KUrl file);
    void upload(KUrl file);
    void cleanJobs(KJob*);

private:
    void setupActions();

private:
    Ui::Settings ui_Settings;

    KToggleAction *m_toolbarAction;
    KToggleAction *m_statusbarAction;
    QSet<KJob*> m_copyJobs;
};

#endif // _CLOUDSYNC_H_
