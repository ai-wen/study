#ifndef DBUSSEAT_H
#define DBUSSEAT_H

#include <QObject>
#include <QDBusConnection>
#include <QDebug>

class DBusSeat : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.freedesktop.DisplayManager.Seat")

public:
    explicit DBusSeat(QObject *parent = 0);

signals:

public slots:

public Q_SLOTS:
    Q_SCRIPTABLE void switchToGreeter();
};

#endif // DBUSSEAT_H
