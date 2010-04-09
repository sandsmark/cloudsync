/*
 * cloudsync.cpp
 *
 * Copyright (C) 2008 %{AUTHOR} <%{EMAIL}>
 */
#include "cloudsync.h"
#include "settings.h"
#include "dirsyncer.h"

#include <QtGui/QDropEvent>
#include <QtGui/QPainter>
#include <QtGui/QPrinter>

#include <kconfigdialog.h>
#include <kstatusbar.h>

#include <kaction.h>
#include <kactioncollection.h>
#include <kstandardaction.h>


#include <KDE/KLocale>

CloudSync::CloudSync()
    : KStatusNotifierItem()
{
    setIconByName("weather-many-clouds");
    setCategory(KStatusNotifierItem::Communications);

    setToolTip("weather-many-clouds", "Idle", "CloudSync is currently idle.");
    setStatus(KStatusNotifierItem::Passive);

    // then, setup our actions
    setupActions();

    DirSyncer *syncer = new DirSyncer(KUrl("/home/sandsmark/tmp/caek"), KUrl("/home/sandsmark/tmp/lol"));
    connect(syncer, SIGNAL(download(KUrl)), this, SLOT(download(KUrl)));
    connect(syncer, SIGNAL(upload(KUrl)), this, SLOT(upload(KUrl)));
    syncer->compareDirs();
}

CloudSync::~CloudSync()
{
}

void CloudSync::setupActions()
{
    KStandardAction::quit(qApp, SLOT(quit()), actionCollection());
    KStandardAction::preferences(this, SLOT(optionsPreferences()), actionCollection());
}

void CloudSync::download(KUrl file)
{
    KUrl localPath = KUrl(Settings::localUrl().url() +
                     KUrl::relativeUrl(Settings::remoteUrl(), KUrl(file.directory())) + "/" +
                     file.fileName());

    localPath.cleanPath();


    //qDebug() << "DOWNLOAD:" << file << "->" << localPath;

    //KIO::CopyJob job = KIO::copy(file, localPath, KIO::Overwrite);
    //connect(job, SIGNAL(result(KJob*), SLOT(cleanJobs(KJob*))));
    //m_copyJobs.append(job);
}

void CloudSync::upload(KUrl file)
{
    KUrl remotePath = KUrl(Settings::remoteUrl().url() +
                      KUrl::relativeUrl(Settings::localUrl(), KUrl(file.directory())) + "/" +
                      file.fileName());

    remotePath.cleanPath();
    //qDebug() << "UPLOAD:" << file << "->" << remotePath;

    //KIO::CopyJob job = KIO::copy(file, Settings::remoteUrl(), KIO::Overwrite);
    //connect(job, SIGNAL(result(KJob*), SLOT(cleanJobs(KJob*))));
    //m_copyJobs.append(job);
}

void CloudSync::cleanJobs(KJob *job)
{
    m_copyJobs.remove(job);
}

void CloudSync::optionsPreferences()
{
    // The preference dialog is derived from prefs_base.ui
    //
    // compare the names of the widgets in the .ui file
    // to the names of the variables in the .kcfg file
    //avoid to have 2 dialogs shown
    if ( KConfigDialog::showDialog( "settings" ) )  {
        return;
    }
    KConfigDialog *dialog = new KConfigDialog(NULL, "settings", Settings::self());
    QWidget *generalSettingsDlg = new QWidget;
    ui_Settings.setupUi(generalSettingsDlg);
    dialog->addPage(generalSettingsDlg, i18n("General"), "package_setting");
    dialog->setAttribute( Qt::WA_DeleteOnClose );
    dialog->show();
}

#include "cloudsync.moc"
