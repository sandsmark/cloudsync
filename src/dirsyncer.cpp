#include "dirsyncer.h"
#include <KApplication>
#include <KDebug>

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
    while (!localLister.isFinished() || !remoteLister.isFinished()) {
        kapp->processEvents(QEventLoop::ExcludeUserInputEvents);
    }

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
    foreach(KFileItem item, remoteFiles){
        if (!localFiles.contains(item))
            emit download(item.url());
    }

    // Upload new files
    foreach(KFileItem item, localFiles){
        if (!remoteFiles.contains(item))
            emit upload(item.url());
    }
}

