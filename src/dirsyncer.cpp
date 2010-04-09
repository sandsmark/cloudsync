#include "dirsyncer.h"
#include "settings.h"
#include <KApplication>
#include <KDebug>
#include <KIO/StatJob>
#include <KIO/Job>

DirSyncer::DirSyncer(KUrl localPath, KUrl remotePath)
    : m_localPath(localPath), m_remotePath(remotePath)
{
}

void DirSyncer::compareDirs(QString subdir)
{
    qDebug() << "Comparing subdir: " << m_localPath.url() << subdir;
    
    KDirLister localLister, remoteLister;
    localLister.openUrl(m_localPath.url() + subdir);
    remoteLister.openUrl(m_remotePath.url() + subdir);

    // Spin and wait, async is not good when drunk
    while (!localLister.isFinished() || !remoteLister.isFinished())
        kapp->processEvents(QEventLoop::ExcludeUserInputEvents);

    KFileItemList localFiles = localLister.items(KDirLister::AllItems),
                  remoteFiles = remoteLister.items(KDirLister::AllItems);

    // Synchronise existing files
    foreach(KFileItem local, localFiles){
        foreach(KFileItem remote, remoteFiles){
            if (local.name() != remote.name()) continue;
            if (local.isDir() && remote.isDir()) {
                compareDirs(subdir + "/" + local.name());
                continue;
            }
            if (local.time(KFileItem::ModificationTime) > remote.time(KFileItem::ModificationTime)) // TODO: checksumming
                emit upload(local.url());
            else
                emit download(remote.url());
        }
    }

    // Download new files
    foreach(KFileItem file, remoteFiles){
        if (!localFiles.contains(file)) {
            // This is stupid. Caching, maybe?
            KUrl relativeUrl = KUrl::relativeUrl(Settings::localUrl(), KUrl(file.url().directory()));
            KUrl localUrl = Settings::localUrl().url(KUrl::AddTrailingSlash) + relativeUrl.url();
            KUrl remoteUrl = Settings::remoteUrl().url(KUrl::AddTrailingSlash) + relativeUrl.url();

            KIO::StatJob *statJob = KIO::stat(remoteUrl);
            statJob->exec();
            KDateTime remoteTime;
            remoteTime.setTime_t(statJob->statResult().numberValue(KIO::UDSEntry::UDS_MODIFICATION_TIME));
            statJob->deleteLater();

            statJob = KIO::stat(localUrl);
            statJob->exec();
            KDateTime localTime;
            localTime.setTime_t(statJob->statResult().numberValue(KIO::UDSEntry::UDS_MODIFICATION_TIME));
            statJob->deleteLater();

            if (remoteTime > localTime) // If the local dir is changed last, we assume we deleted something
                emit download(file.url());
        }
    }

    // Upload new files
    foreach(KFileItem item, localFiles){
        if (!remoteFiles.contains(item)) {
            KUrl relativeUrl = KUrl::relativeUrl(Settings::localUrl(), KUrl(file.url().directory()));
            KUrl localUrl = Settings::localUrl().url(KUrl::AddTrailingSlash) + relativeUrl.url();
            KUrl remoteUrl = Settings::remoteUrl().url(KUrl::AddTrailingSlash) + relativeUrl.url();

            KIO::StatJob *statJob = KIO::stat(remoteUrl);
            statJob->exec();
            KDateTime remoteTime;
            remoteTime.setTime_t(statJob->statResult().numberValue(KIO::UDSEntry::UDS_MODIFICATION_TIME));
            statJob->deleteLater();

            statJob = KIO::stat(localUrl);
            statJob->exec();
            KDateTime localTime;
            localTime.setTime_t(statJob->statResult().numberValue(KIO::UDSEntry::UDS_MODIFICATION_TIME));
            statJob->deleteLater();

            if (remoteTime < localTime) // If the local dir is changed last, we assume we deleted something
                emit upload(item.url());
        }
    }
}

