#include "dirsyncer.h"
#include "settings.h"
#include <KApplication>
#include <KDebug>
#include <KIO/StatJob>
#include <KIO/Job>
#include <KIO/CopyJob>

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
            } else {
                if (local.time(KFileItem::ModificationTime) > remote.time(KFileItem::ModificationTime)) // TODO: checksumming
                    upload(local.url());
                else
                    download(remote.url());
            }
        }
    }

    // Download new files
    foreach(KFileItem file, remoteFiles){
        if (!localFiles.contains(file)) {
            // This is stupid. Caching, maybe?
            KUrl relativeUrl = KUrl::relativeUrl(Settings::localUrl(), KUrl(file.url().directory()));
            KUrl localUrl = Settings::localUrl().url(KUrl::AddTrailingSlash) + relativeUrl.url();
            KUrl remoteUrl = Settings::remoteUrl().url(KUrl::AddTrailingSlash) + relativeUrl.url();

            KDateTime remoteTime = getModificationTime(remoteUrl);
            KDateTime localTime = getModificationTime(localUrl);

            if (remoteTime > localTime) // If the local dir is changed last, we assume we deleted something
                download(file.url());
        }
    }

    // Upload new files
    foreach(KFileItem item, localFiles){
        if (!remoteFiles.contains(item)) {
            KUrl relativeUrl = KUrl::relativeUrl(Settings::localUrl(), KUrl(item.url().directory()));
            KUrl localUrl = Settings::localUrl().url(KUrl::AddTrailingSlash) + relativeUrl.url();
            KUrl remoteUrl = Settings::remoteUrl().url(KUrl::AddTrailingSlash) + relativeUrl.url();


            KDateTime remoteTime = getModificationTime(remoteUrl);
            KDateTime localTime = getModificationTime(localUrl);

            if (remoteTime < localTime) // If the remote dir is changed last, we assume we deleted something
                upload(item.url());
        }
    }
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
    KUrl localPath = KUrl(Settings::localUrl().url() +
                     KUrl::relativeUrl(Settings::remoteUrl(), KUrl(file.directory())) + "/" +
                     file.fileName());

    localPath.cleanPath();
    launchTransfer(file, localPath);
    qDebug() << "DOWNLOAD:" << file << "->" << localPath;
    emit downloading(file.url());
}

void DirSyncer::upload(KUrl file)
{
    KUrl remotePath = KUrl(Settings::remoteUrl().url() +
                      KUrl::relativeUrl(Settings::localUrl(), KUrl(file.directory())) + "/" +
                      file.fileName());

    remotePath.cleanPath();
    launchTransfer(file, remotePath);
    qDebug() << "UPLOAD:" << file << "->" << remotePath;
    emit uploading(file.url());
}

void DirSyncer::launchTransfer(KUrl from, KUrl to)
{
    KIO::CopyJob *job = KIO::copy(from, to, KIO::Overwrite);
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

    KIO::setModificationTime(job->destUrl(), getModificationTime(job->srcUrls().first()).dateTime()); // FIXME: isn't kio supposed to do this?
    qDebug() << getModificationTime(job->srcUrls().first());
    qDebug() << getModificationTime(job->destUrl());

    if (m_copyJobs.isEmpty())
        emit idle();
}
