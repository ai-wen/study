#ifndef PAM_H
#define PAM_H

#include <QString>
#include <QProcess>
#include <QDebug>
#include <QX11Info>
#include <security/pam_appl.h>
#include <security/pam_misc.h>
#include <pwd.h>
#include <grp.h>
#include <paths.h>
#include <unistd.h>
#include <sys/mman.h>
#include <X11/Xlib.h>
#include <QDBusConnection>
#include <QDBusMessage>
#include <QDBusObjectPath>
#include <QDBusInterface>
#include <QDBusArgument>

struct QStringDBusObjectPathMap {
    QString sessionText;
    QDBusObjectPath objectPath;
};
Q_DECLARE_METATYPE(QStringDBusObjectPathMap)

inline QDBusArgument &operator<<(QDBusArgument &arg, const QStringDBusObjectPathMap &map) {
    arg.beginStructure();
    arg << map.sessionText << map.objectPath;
    arg.endStructure();
    return arg;
}

inline const QDBusArgument &operator>>(const QDBusArgument &arg, QStringDBusObjectPathMap &map) {
    qDebug() << arg.currentType();
    arg.beginStructure();
    arg >> map.sessionText >> map.objectPath;
    arg.endStructure();
    return arg;
}

bool login(QString username, QString password, QString exec, pid_t *child_pid, QDBusObjectPath* resumeSession = NULL);
bool logout();
int conversation(int num_msg, const struct pam_message **msg, struct pam_response **resp, void *appdata_ptr);

#endif // PAM_H
