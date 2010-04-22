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
#include <kmenu.h>

#include <KDE/KLocale>

CloudSync::CloudSync()
    : KStatusNotifierItem()
{
    setIconByName("weather-many-clouds");
    setCategory(KStatusNotifierItem::Communications);

    updateTooltip();

    // then, setup our actions
    setupActions();

    contextMenu()->setTitle("CloudSync");

    DirSyncer *syncer = new DirSyncer(Settings::localUrl(), Settings::remoteUrl());
    connect(syncer, SIGNAL(downloading(QString)), this, SLOT(transferring(QString)));
    connect(syncer, SIGNAL(finished(QString)), this, SLOT(finished(QString)));

    syncer->compareDirs();
}

CloudSync::~CloudSync()
{
}

void CloudSync::transferring(QString file)
{
    m_fileTransfers.insert(file);
}

void CloudSync::finished(QString file)
{
    m_fileTransfers.remove(file);
}

void CloudSync::updateTooltip()
{
    if (m_fileTransfers.isEmpty()) {
        setToolTip("weather-many-clouds", "Idle", "CloudSync is currently idle.");
        setStatus(KStatusNotifierItem::Passive);
    } else if (m_fileTransfers.size() < 3) {
        QString text = "CloudSync is currently transferring the following files:\n";
        foreach(QString file, m_fileTransfers)
            text.append(file + "\n");

        setToolTip("weather-many-clouds", "Transferring", text);
        setStatus(KStatusNotifierItem::Active);
    } else{
        QString text = QString("CloudSync is currently transferring %1 files.").arg(m_fileTransfers.size());
        setToolTip("weather-many-clouds", "Transferring", text);
        setStatus(KStatusNotifierItem::Active);
    }
}

void CloudSync::setupActions()
{
    contextMenu()->addAction(KStandardAction::quit(qApp, SLOT(quit()), actionCollection()));
    contextMenu()->addAction(KStandardAction::preferences(this, SLOT(optionsPreferences()), actionCollection()));
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
    dialog->addPage(generalSettingsDlg, i18n("Paths"), "document-open-remote");
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    dialog->show();
}

#include "cloudsync.moc"
