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

cloudsync::cloudsync()
    : KStatusNotifierItem()
{
    setIconByName("weather-many-clouds");

    // then, setup our actions
    setupActions();

    DirSyncer *syncer = new DirSyncer(KUrl("/home/sandsmark/tmp/caek"), KUrl("/home/sandsmark/tmp/lol"));
    connect(syncer, SIGNAL(download(KUrl)), this, SLOT(download(KUrl)));
    connect(syncer, SIGNAL(upload(KUrl)), this, SLOT(upload(KUrl)));
    syncer->compareDirs();
}

cloudsync::~cloudsync()
{
}

void cloudsync::setupActions()
{
    KStandardAction::quit(qApp, SLOT(quit()), actionCollection());

    KStandardAction::preferences(this, SLOT(optionsPreferences()), actionCollection());

    // custom menu and menu item - the slot is in the class cloudsyncView
    KAction *custom = new KAction(KIcon("colorize"), i18n("Swi&tch Colors"), this);
    actionCollection()->addAction( QLatin1String("switch_action"), custom );
}

void cloudsync::download(KUrl file)
{
    qDebug() << "DOWNLOAD: " << file.url();
}


void cloudsync::upload(KUrl file)
{
    qDebug() << "UPLOAD: " << file.url();
}

void cloudsync::optionsPreferences()
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
    ui_prefs_base.setupUi(generalSettingsDlg);
    dialog->addPage(generalSettingsDlg, i18n("General"), "package_setting");
    dialog->setAttribute( Qt::WA_DeleteOnClose );
    dialog->show();
}

#include "cloudsync.moc"
