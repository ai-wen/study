#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QX11Info>
#include <QProcess>
#include <QTimer>
#include <QDateTime>
#include <QDesktopWidget>
#include <QPropertyAnimation>
#include <QMouseEvent>
#include <QDBusMessage>
#include <QDBusConnection>
#include <QSettings>
#include <QDBusReply>
#include <QDBusConnectionInterface>
#include <QDBusInterface>
#include <QFrame>
#include <QDebug>
#include <QMenu>
#include <QDir>
#include <QToolButton>
#include <QLineEdit>
#include <QApplication>
#include <QComboBox>
#include <QPushButton>
#include <ttoast.h>
#include <QGraphicsOpacityEffect>
#include "pam.h"

#include <sys/wait.h>
#include <X11/Xlib.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_passwordBox_returnPressed();

    void on_Cover_clicked();

    void on_Cover_MouseDown(QMouseEvent *);

    void on_Cover_MouseMove(QMouseEvent *);

    void on_Cover_MouseUp(QMouseEvent *);

    void hideCover();

    void showCover();

    void on_passwordBox_textEdited(const QString &arg1);
    void on_goBack_clicked();

    void resizeSlot();

    void on_sessionSelect_triggered(QAction *arg1);

    void on_loginButton_clicked();

    void on_turnOffScreen_clicked();

    void on_pushButton_clicked();

    void on_suspendButton_clicked();

    void on_usernameBox_currentIndexChanged(int index);

    void on_powerButton_clicked();

    void on_cancelPowerOptions_clicked();

    void on_powerOffButton_clicked();

    void on_rebootButton_clicked();

private:
    Ui::MainWindow *ui;

    QImage image;

    void resizeEvent(QResizeEvent* event);
    void keyPressEvent(QKeyEvent* event);

    int moveY;
    bool typePassword = false;
    QSize imageSize;

    QString sessionExec;
    QDBusObjectPath* sessionPath;

    QSettings* settings;
};

#endif // MAINWINDOW_H
