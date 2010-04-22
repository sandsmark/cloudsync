#include "cloudsync.h"
#include <kapplication.h>
#include <kaboutdata.h>
#include <kcmdlineargs.h>
#include <KDE/KLocale>

static const char description[] =
    I18N_NOOP("An automated directory synchroniser using KIO.");

static const char version[] = "%{VERSION}";

int main(int argc, char **argv)
{
    KAboutData about("CloudSync", 0, ki18n("CloudSync"), version, ki18n(description),
                     KAboutData::License_GPL, ki18n("(C) 2010 Martin T. Sandsmark"), KLocalizedString(), 0, "martin.sandsmark@kde.org");
    KCmdLineArgs::init(argc, argv, &about);

    KCmdLineOptions options;
    options.add("+[URL]", ki18n( "Document to open" ));
    KCmdLineArgs::addCmdLineOptions(options);
    KApplication app;

    CloudSync *widget = new CloudSync;

    
    app.setQuitOnLastWindowClosed(false);
    return app.exec();
}
