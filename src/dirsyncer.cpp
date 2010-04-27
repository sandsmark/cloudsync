/*
 * dirsyncer.cpp
 *
 * This contains the implementation of the class that is responsible for
 * synchronising two directories.
 *
 * Copyright (C) 2010 Martin T. Sandsmark <martin.sandsmark@kde.org>
 */

#include "dirsyncer.h"
#include "settings.h"

#include <KApplication>
#include <KDebug>
#include <KIO/StatJob>
#include <KIO/Job>
#include <KIO/CopyJob>
#include <KIO/DeleteJob>

DirSyncer::DirSyncer(KUrl localPath, KUrl remotePath)
    : m_localPath(localPath), m_remotePath(remotePath)
{
    qDebug() << "remote dir" << remotePath;
    qDebug() << "local dir" << localPath;
    connect(&m_dirWatcher, SIGNAL(dirty(QString)), SLOT(checkDirty(KUrl)));
    connect(&m_dirWatcher, SIGNAL(created(QString)), SLOT(checkCreated(KUrl)));
    connect(&m_dirWatcher, SIGNAL(deleted(QString)), SLOT(checkDeleted(KUrl)));

    //TODO: Check for remote folder, and add magic so we avoid stating all the time
    m_dirWatcher.addDir(localPath.url(), KDirWatch::WatchSubDirs);
    m_dirWatcher.addDir(remotePath.url(), KDirWatch::WatchSubDirs);

    // Do initial scan after we return, which also starts continous watch
    QMetaObject::invokeMethod(this, SLOT(compareDirs()), Qt::QueuedConnection);
}

void DirSyncer::checkDeleted(KUrl url)
{
    KUrl parentDir;
    bool isLocal;
    if (m_localPath.isParentOf(url)) {
        parentDir = m_localPath;
        isLocal = true;
    } else {
        parentDir = m_remotePath;
        isLocal = false;
    }

    KUrl relative = KUrl::relativeUrl(parentDir, url);
    KUrl toDelete;
    if (isLocal)
        toDelete = m_localPath.url() + relative.url();
    else
        toDelete = m_remotePath.url() + relative.url();

    //KIO::del(toDelete);//TODO when you're sure, really sure
    qDebug() << "I almost this dir: " << toDelete;

}

void DirSyncer::checkCreated(KUrl url)
{
    //TODO
    checkDirty(url);
}

void DirSyncer::checkDirty(KUrl url)
{
    qDebug() << "FILTHY ANIMAL: " << url;
    KUrl parent;

    if (m_localPath.isParentOf(url)) {
        parent = m_localPath;
    } else {
        parent = m_remotePath;
    }

    if (url.fileName().isNull()) {
        // IT IS LE DIRECTORY!
        compareDirs(KUrl::relativeUrl(parent, url));
    } else {
        KUrl relative = KUrl::relativeUrl(parent, url);
        KDateTime remote = getModificationTime(m_remotePath.url() + relative.url());
        KDateTime local = getModificationTime(m_localPath.url() + relative.url());
        if (local > remote) {
            upload(m_localPath.url() + relative.url());
        } else if (local > remote) {
            download(m_remotePath.url() + relative.url());
        }
    }
}

void DirSyncer::compareDirs(QString subdir)
{    
    KDirLister localLister, remoteLister;
    localLister.openUrl(m_localPath.url() + subdir);
    remoteLister.openUrl(m_remotePath.url() + subdir);

    // Spin and wait, async is not good when drunk
    while (!localLister.isFinished() || !remoteLister.isFinished())
        kapp->processEvents(QEventLoop::ExcludeUserInputEvents);

    KFileItemList localFileItemList = localLister.items(KDirLister::AllItems),
                  remoteFileItemList = remoteLister.items(KDirLister::AllItems);

    KUrl::List localUrlList = localFileItemList.urlList(),
               remoteUrlList = remoteFileItemList.urlList();

    // Synchronise existing files
    foreach(KFileItem local, localFileItemList){
        foreach(KFileItem remote, remoteFileItemList){
            if (local.name() != remote.name()) continue;

            if (local.isDir() && remote.isDir()) {
                compareDirs(subdir + "/" + local.name());
                continue;
            } else {
                if (local.time(KFileItem::ModificationTime) > remote.time(KFileItem::ModificationTime)) // TODO: checksumming
                    upload(local.url());
                else if (local.time(KFileItem::ModificationTime) < remote.time(KFileItem::ModificationTime)) // TODO: checksumming
                    download(remote.url());
            }
        }
    }

    // Download new files
    foreach(KUrl item, remoteUrlList){
        if (!localUrlList.contains(item)) {
            // This is stupid. Caching, maybe?
            KUrl relativeUrl = KUrl::relativeUrl(m_remotePath, KUrl(item.directory()));
            KUrl localUrl = m_localPath.url(KUrl::AddTrailingSlash) + relativeUrl.url();
            KUrl remoteUrl = m_remotePath.url(KUrl::AddTrailingSlash) + relativeUrl.url();

            KDateTime remoteTime = getModificationTime(remoteUrl);
            KDateTime localTime = getModificationTime(localUrl);

            if (remoteTime.toTime_t() > localTime.toTime_t()) // If the local dir is changed last, we assume we deleted something
                download(item.url());
            else {
                //KIO::del(remoteUrl); UNCOMMENT WHEN ABSOLUFREAKINGLUTELY SURE IT IS CORRECT!
                qWarning() << "I almost deleted " << remoteUrl;
            }
        }
    }

    // Upload new files
    foreach(KUrl item, localUrlList){
        if (!remoteUrlList.contains(item)) {
            KUrl relativeUrl = KUrl::relativeUrl(m_localPath, item.directory());
            KUrl localUrl = m_localPath.url(KUrl::AddTrailingSlash) + relativeUrl.url();
            KUrl remoteUrl = m_remotePath.url(KUrl::AddTrailingSlash) + relativeUrl.url();

            KDateTime remoteTime = getModificationTime(remoteUrl);
            KDateTime localTime = getModificationTime(localUrl);

            if (remoteTime.toTime_t() < localTime.toTime_t()) // If the remote dir is changed last, we assume we deleted something
                upload(item.url());
            else {
                //KIO::del(localUrl); UNCOMMENT WHEN ABSOLUFREAKINGLUTELY SURE IT IS CORRECT!
                qWarning() << "I almost deleted " << localUrl;
            }
        }
    }

    //TODO: naughty sandsmark, more late-night ugly hacks
    while (!m_copyJobs.isEmpty()) // make sure we've copied in all files from initial sync before we start monitoring
        kapp->processEvents(QEventLoop::ExcludeUserInputEvents);

    qDebug() << "initial synchronization finished";

    // Start scanning and watching folders
    m_dirWatcher.startScan();
}

KDateTime DirSyncer::getModificationTime(KUrl url) 
{
    KIO::StatJob *statJob = KIO::stat(url, KIO::HideProgressInfo);
    statJob->exec();
    KDateTime r;
    r.setTime_t(statJob->statResult().numberValue(KIO::UDSEntry::UDS_MODIFICATION_TIME));
    statJob->deleteLater();

    return r;
}

void DirSyncer::download(KUrl file)
{
    KUrl localPath = KUrl(m_localPath.url(KUrl::AddTrailingSlash) +
                     KUrl::relativeUrl(m_remotePath, KUrl(file.directory())) + "/" +
                     file.fileName());

    localPath.cleanPath();
    launchTransfer(file, localPath);
    qDebug() << "DOWNLOAD:" << file << "->" << localPath;
    emit downloading(file.url());
}

void DirSyncer::upload(KUrl file)
{
    KUrl remotePath = KUrl(m_remotePath.url(KUrl::AddTrailingSlash) +
                      KUrl::relativeUrl(m_localPath, KUrl(file.directory())) + "/" +
                      file.fileName());

    remotePath.cleanPath();
    launchTransfer(file, remotePath);
    qDebug() << "UPLOAD:" << file << "->" << remotePath;
    emit uploading(file.url());
}

void DirSyncer::launchTransfer(KUrl from, KUrl to)
{
    //TODO: queue these internally
    KIO::CopyJob *job = KIO::copy(from, to, KIO::Overwrite | KIO::HideProgressInfo);
    job->setWriteIntoExistingDirectories(true);
    connect(job, SIGNAL(result(KJob*)), this, SLOT(cleanJobs(KJob*)));
    m_copyJobs.insert(job);
}

void DirSyncer::cleanJobs(KJob *j)
{
    KIO::CopyJob *job = static_cast<KIO::CopyJob*> (j);

    m_copyJobs.remove(job);
    job->deleteLater();

    emit finished(job->srcUrls().first().url());

    if (m_copyJobs.isEmpty())
        emit idle();
}
