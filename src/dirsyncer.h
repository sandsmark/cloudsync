#ifndef DIRSYNCER_H
#define DIRSYNCER_H
#include <QDir>
#include <KUrl>
#include <KDirLister>
#include <KFileItem>

class DirSyncer : public QObject {
    Q_OBJECT;

public:
    DirSyncer(KUrl localPath, KUrl remotePath);

signals:
    void download(KUrl file);
    void upload(KUrl file);

public slots:
    void compareDirs(QString subdir = "");

private:
    KUrl m_localPath;
    KUrl m_remotePath;

    KFileItemList m_localFiles;
    KFileItemList m_remoteFiles;
};

#endif//DIRSYNCER_H
