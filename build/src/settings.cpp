// This file is generated by kconfig_compiler from cloudsync.kcfg.
// All changes you do to this file will be lost.

#include "settings.h"

#include <kglobal.h>
#include <QtCore/QFile>

class SettingsHelper
{
  public:
    SettingsHelper() : q(0) {}
    ~SettingsHelper() { delete q; }
    Settings *q;
};
K_GLOBAL_STATIC(SettingsHelper, s_globalSettings)
Settings *Settings::self()
{
  if (!s_globalSettings->q) {
    new Settings;
    s_globalSettings->q->readConfig();
  }

  return s_globalSettings->q;
}

Settings::Settings(  )
  : KConfigSkeleton( QLatin1String( "cloudsyncrc" ) )
{
  Q_ASSERT(!s_globalSettings->q);
  s_globalSettings->q = this;
  setCurrentGroup( QLatin1String( "Preferences" ) );

  KConfigSkeleton::ItemColor  *itemCol_background;
  itemCol_background = new KConfigSkeleton::ItemColor( currentGroup(), QLatin1String( "col_background" ), mCol_background, QColor( "black" ) );
  addItem( itemCol_background, QLatin1String( "col_background" ) );
  KConfigSkeleton::ItemColor  *itemCol_foreground;
  itemCol_foreground = new KConfigSkeleton::ItemColor( currentGroup(), QLatin1String( "col_foreground" ), mCol_foreground, QColor( "yellow" ) );
  addItem( itemCol_foreground, QLatin1String( "col_foreground" ) );
  KConfigSkeleton::ItemInt  *itemVal_time;
  itemVal_time = new KConfigSkeleton::ItemInt( currentGroup(), QLatin1String( "val_time" ), mVal_time, 2 );
  addItem( itemVal_time, QLatin1String( "val_time" ) );
}

Settings::~Settings()
{
  if (!s_globalSettings.isDestroyed()) {
    s_globalSettings->q = 0;
  }
}

