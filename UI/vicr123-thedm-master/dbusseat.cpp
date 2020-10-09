#include "dbusseat.h"
#include "seat_adaptor.h"

DBusSeat::DBusSeat(QObject *parent) : QObject(parent)
{
    new SeatAdaptor(this);
    QDBusConnection::systemBus().registerObject("/org/freedesktop/DisplayManager/Seat0", "org.freedesktop.DisplayManager.Seat", this);
}

void DBusSeat::switchToGreeter() {
    qDebug() << "Switch to Greeter";
}
