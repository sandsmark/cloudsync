/*
 * cloudsync.h
 *
 * Copyright (C) 2010 Martin T. Sandsmark <martin.sandsmark@kde.org>
 */

#ifndef DIRSYNCER_H
#define DIRSYNCER_H
#include <QDir>
#include <KUrl>
#include <KDirLister>
#include <KFileItem>
#include <KDateTime>

class DirSyncer : public QObject {
    Q_OBJECT;

public:
    DirSyncer(KUrl localPath, KUrl remotePath);

signals:
    void idle();
    void downloading(QString file);
    void uploading(QString file);
    void finished(QString file);

public slots:
    void compareDirs(QString subdir = "");

private slots:
    void cleanJobs(KJob*);

private:
    void launchTransfer(KUrl from, KUrl to);
    void download(KUrl file); // Fires off async calls
    void upload(KUrl file);

    KDateTime getModificationTime(KUrl url);
    KUrl m_localPath;
    KUrl m_remotePath;

    QSet<KJob*> m_copyJobs;
};

#endif//DIRSYNCER_H
