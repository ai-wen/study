#include <QApplication>
#include <QWindow>
#include <QProcess>
#include <QX11Info>
#include <QDBusMetaType>
#include "mainwindow.h"
#include "dbusseat.h"
#include <X11/extensions/Xrandr.h>
#include <iostream>

QList<MainWindow*> windows;

void closeWindows() {
    for (MainWindow* window : windows) {
        window->close();
    }
    windows.clear();
}

void openWindows() {
    closeWindows();
    for (int i = 0; i < QApplication::desktop()->screenCount(); i++) {
        MainWindow* w = new MainWindow();
        w->show();
        w->setGeometry(QApplication::desktop()->screenGeometry(i));
        w->showFullScreen();
        windows.append(w);
    }
}


int main(int argc, char *argv[])
{
    qputenv("XDG_SESSION_CLASS", "greeter"); //Qt设置环境变量_penghuilater的博客-CSDN博客
    qputenv("QT_QPA_PLATFORMTHEME", "ts");
    QProcess* XServerProcess = NULL;

    if (argc > 1)
    {
        QString firstArg = argv[1];
        if (firstArg == "--help" || firstArg == "-h")
        {
            std::cout << "theDM Display Manager\n";
            std::cout << "Usage: thedm [vtx|--help]\n";
            std::cout << "       vtx: The virtual terminal to start the X server on, in the form of vtx.\n";
            std::cout << "\n";
            std::cout << "  -h, --help                   Show this help output\n";
            std::cout << "                               This must be the first argument passed to theDM.\n";
            std::cout << "\n";
            std::cout << "If vtx is not passed, and $DISPLAY=\"\", theDM will start an X server on the "
                         "current virtual terminal.\n";
            return 0;
        }
    }

    if (qgetenv("DISPLAY") == "")
    {
        //Start the X server
        QString currentVt = "";

        if (argc > 1)
        {
            QString firstArg = argv[1];
            if (firstArg.contains("vt"))
            {
                currentVt = firstArg;

                //Switch to the required VT
                QDBusMessage message = QDBusMessage::createMethodCall("org.freedesktop.login1", "/org/freedesktop/login1/seat/self", "org.freedesktop.login1.Seat", "SwitchTo");
                QList<QVariant> args;
                args.append(firstArg.remove("vt").toUInt());
                message.setArguments(args);
                QDBusConnection::systemBus().call(message);
            }
        }

        if (currentVt == "")
        {
            QProcess vtGet;
            vtGet.start("fgconsole");
            vtGet.waitForFinished();
            currentVt = "vt" + QString(vtGet.readAll()); //Linux fgconsole命令显示活动的虚拟终端数量-Linux实验室
        }

        bool serverStarted = false;
        int display = 0;
        do {
            qDebug() << QString("Starting the X Server as display :" + QString::number(display) + " on " + currentVt).toStdString().data();

            XServerProcess = new QProcess();
            XServerProcess->setProcessChannelMode(QProcess::ForwardedChannels);
            XServerProcess->start("/usr/bin/X :" + QString::number(display) + " " + currentVt);
            XServerProcess->waitForFinished(1000);

            if (XServerProcess->state() != QProcess::Running) {
                display++;
            } else {
                if (!qputenv("DISPLAY", QString(":" + QString::number(display)).toUtf8())) {
                    qDebug() << "Could not set DISPLAY environment variable.";
                    XServerProcess->kill();
                    return 1;
                }

                serverStarted = true;
            }
        } while (!serverStarted);
    }

    QApplication a(argc, argv);
    a.setQuitOnLastWindowClosed(false);
    qRegisterMetaType<QStringDBusObjectPathMap>();
    qDBusRegisterMetaType<QStringDBusObjectPathMap>();

    a.setOrganizationName("theSuite");
    a.setOrganizationDomain("");
    a.setApplicationName("theDM");

    new DBusSeat();
    if (!QDBusConnection::systemBus().interface()->registeredServiceNames().value().contains("org.thesute.thedm")) {
        QDBusConnection::systemBus().registerService("org.thesuite.thedm");
    }
    if (!QDBusConnection::systemBus().interface()->registeredServiceNames().value().contains("org.freedesktop.DisplayManager")) {
        QDBusConnection::systemBus().registerService("org.freedesktop.DisplayManager");
    }

    //Start DBus so that we can use KDED
    QProcess* dbusLaunch = new QProcess;
    dbusLaunch->start("dbus-launch");
    dbusLaunch->waitForFinished();
    for (QString env : QString(dbusLaunch->readAll()).split("\n")) {
        QStringList parts = env.split("=");
        if (parts.length() >= 2) {
            QString key = parts.first();
            parts.removeFirst();
            qputenv(key.toStdString().data(), parts.join("=").toUtf8());
        }
    }

    {
        QProcess* kded = new QProcess;
        kded->start("kded5");
        kded->waitForStarted();

        QDBusMessage kscreen = QDBusMessage::createMethodCall("org.kde.kded5", "/kded", "org.kde.kded5", "loadModule");
        QVariantList args;
        args.append("kscreen");
        kscreen.setArguments(args);
        QDBusConnection::sessionBus().call(kscreen);
    }

    /*{
        int numberOfScreens = QApplication::screens().count();
        for (int i = 0; i < numberOfScreens; i++) {
            int numberOfSizes;
            int currentWidth = 0, currentHeight = 0, currentMWidth = 0, currentMHeight = 0;
            SizeID newSizeId = 0;
            XRRScreenSize *screenSize = XRRSizes(QX11Info::display(), numberOfScreens, &numberOfSizes);
            for (int sizes = 0; sizes < numberOfSizes; sizes++) {
                if ((screenSize[sizes].height * screenSize[sizes].width) > (currentWidth * currentHeight)) {
                    //This is a bigger screen.
                    currentWidth = screenSize[sizes].width;
                    currentHeight = screenSize[sizes].height;
                    currentMWidth = screenSize[sizes].mwidth;
                    currentMHeight = screenSize[sizes].mheight;
                    newSizeId = sizes;
                }

            }



            XRRScreenConfiguration *currentConfig = XRRGetScreenInfo(QX11Info::display(), DefaultRootWindow(QX11Info::display()));
            XRRSetScreenConfig(QX11Info::display(), currentConfig, DefaultRootWindow(QX11Info::display()), newSizeId, RR_Rotate_0, CurrentTime);

            XRRFreeScreenConfigInfo(currentConfig);
        }*/

        /*int numberOfMonitors;
        XRRMonitorInfo* monitorInfoArray = XRRGetMonitors(QX11Info::display(), DefaultRootWindow(QX11Info::display()), 1, &numberOfMonitors);
        for (int i = 0; i < numberOfMonitors; i++) {
            /*
            XRRMonitorInfo monitorInfo = monitorInfoArray[i];
            monitorInfo.automatic = 1;
            XRRSetMonitor(QX11Info::display(), DefaultRootWindow(QX11Info::display()), &monitorInfo);
            XRRFreeMonitors(monitorInfo);


            XRRMonitorInfo *m = XRRAllocateMonitor(QX11Info::display(), monitorInfoArray[i].noutput);

            m->name = monitorInfoArray[i].name;
            m->primary = monitorInfoArray[i].primary;
            m->x = monitorInfoArray[i].x;
            m->y = monitorInfoArray[i].y;
            m->width = monitorInfoArray[i].width;
            m->height = monitorInfoArray[i].height;
            m->mwidth = monitorInfoArray[i].mwidth;
            m->mheight = monitorInfoArray[i].mheight;
            for (int out = 0; out < monitorInfoArray[i].noutput; out++) {
                m->outputs[out] = monitorInfoArray[i].outputs[out];
            }
            m->automatic = 1;
            XRRSetMonitor(QX11Info::display(), DefaultRootWindow(QX11Info::display()), m);
            XRRFreeMonitors(m);
        }
        XRRFreeMonitors(monitorInfoArray);
        XSync(QX11Info::display(), false);
    }*/

    //XGrabKeyboard(QX11Info::display(), RootWindow(QX11Info::display(), 0), True, GrabModeAsync, GrabModeAsync, CurrentTime);
    //XGrabPointer(QX11Info::display(), RootWindow(QX11Info::display(), 0), True, None, GrabModeAsync, GrabModeAsync, RootWindow(QX11Info::display(), 0), None, CurrentTime);


    //Listen out for screen changes
    QObject::connect(QApplication::desktop(), &QDesktopWidget::resized, &openWindows);
    QObject::connect(QApplication::desktop(), &QDesktopWidget::screenCountChanged, &openWindows);

    openWindows();

    int ret = a.exec();

    for (MainWindow* w : windows) {
        w->close();
    }

    XUngrabKeyboard(QX11Info::display(), CurrentTime);
    XUngrabPointer(QX11Info::display(), CurrentTime);
    if (XServerProcess != NULL) {
        XServerProcess->terminate();
    }

    return ret;
}

