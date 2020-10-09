#include "ui_mainwindow.h"
#include "mainwindow.h"

extern void openWindows();
extern void closeWindows();

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->incorrectPassword->setVisible(false);

    settings = new QSettings("/etc/thedm.conf", QSettings::IniFormat);

    //QRect screenGeometry = QApplication::desktop()->screenGeometry();
    ui->Cover->setParent(this);
    ui->Cover->setGeometry(0, 0, this->width(), this->height());
    ui->confirmPowerFrame->setVisible(false);
    ui->confirmPowerFrame->setParent(this);

    //Load sessions
    QMenu* sessionsMenu = new QMenu();
    QDir xsessions("/usr/share/xsessions/");
    for (QString file : xsessions.entryList(QDir::Files | QDir::NoDotAndDotDot)) {
        QSettings xsessionsFile(xsessions.path() + "/" + file, QSettings::IniFormat);
        xsessionsFile.beginGroup("Desktop Entry");
        QAction* action = new QAction;
        action->setText(xsessionsFile.value("Name", xsessions.path() + "/" + file).toString());
        action->setData(xsessionsFile.value("Exec"));
        sessionsMenu->addAction(action);
    }
    if (sessionsMenu->actions().count() != 0) {
        on_sessionSelect_triggered(sessionsMenu->actions().first());
    }
    ui->sessionSelect->setMenu(sessionsMenu);

    //Load users
    for (int i = settings->value("users/uidMin", 1000).toInt(); i < settings->value("users/uidMax", 10000).toInt(); i++) {
        //Get user info
        struct passwd *pw = getpwuid(i);
        if (pw != NULL) {
            QString gecosData = pw->pw_gecos;
            QStringList gecosDataList = gecosData.split(",");
            if (gecosDataList.count() == 0 || gecosData == "") {
                ui->usernameBox->addItem(pw->pw_name);
            } else {
                ui->usernameBox->addItem(gecosDataList.first());
            }

            ui->usernameBox->setItemData(ui->usernameBox->count() - 1, pw->pw_name);
            ui->usernameBox->setItemData(ui->usernameBox->count() - 1, i, Qt::UserRole + 1);
        }
    }
    on_usernameBox_currentIndexChanged(ui->usernameBox->currentIndex());

    QTimer* clockTimer = new QTimer();
    clockTimer->setInterval(1000);
    connect(clockTimer, &QTimer::timeout, [=]() {
        ui->dateLabel->setText(QDateTime::currentDateTime().toString("ddd dd MMM yyyy"));
        ui->timeLabel->setText(QDateTime::currentDateTime().toString("hh:mm:ss"));
        ui->dateLabel_2->setText(QDateTime::currentDateTime().toString("ddd dd MMM yyyy"));
        ui->timeLabel_2->setText(QDateTime::currentDateTime().toString("hh:mm:ss"));
    });
    clockTimer->start();

    ui->dateLabel->setText(QDateTime::currentDateTime().toString("ddd dd MMM yyyy"));
    ui->timeLabel->setText(QDateTime::currentDateTime().toString("hh:mm:ss"));
    ui->dateLabel_2->setText(QDateTime::currentDateTime().toString("ddd dd MMM yyyy"));
    ui->timeLabel_2->setText(QDateTime::currentDateTime().toString("hh:mm:ss"));

    QSettings settings;
    QString background = settings.value("background", "/usr/share/icons/theos/backgrounds/triangle/1920x1080.png").toString();

    image = QImage(background);
    imageSize = image.size();
    QList<bool> lightOrDark;
    for (int w = 0; w < image.width(); w++) {
        for (int h = 0; h < image.height(); h++) {
            QColor col = image.pixelColor(w, h);
            int grey = (col.red() + col.green() + col.blue()) / 3;
            if (grey > 127) {
                lightOrDark.append(true);
            } else {
                lightOrDark.append(false);
            }
        }
    }

    QPalette pal;
    if (lightOrDark.count(true) <= lightOrDark.count() / 2) {
        pal.setColor(QPalette::WindowText, QColor::fromRgb(255, 255, 255));
        ui->downArrow->setPixmap(QIcon(":/icons/downarrowlight.svg").pixmap(32, 32));
        ui->upArrow->setPixmap(QIcon(":/icons/uparrowlight.svg").pixmap(32, 32));
    } else {
        pal.setColor(QPalette::WindowText, QColor::fromRgb(0, 0, 0));
        ui->downArrow->setPixmap(QIcon(":/icons/downarrowdark.svg").pixmap(32, 32));
        ui->upArrow->setPixmap(QIcon(":/icons/uparrowdark.svg").pixmap(32, 32));
    }
    ui->Cover->setPalette(pal);
    ui->dateLabel_2->setPalette(pal);
    ui->timeLabel_2->setPalette(pal);

    ui->ImageLabel->setParent(ui->Cover);
    ui->ImageLabel->setPixmap(QPixmap::fromImage(image));
    ui->ImageLabel->setGeometry(0, 0, this->width(), this->height());
    ui->ImageLabel->lower();

}

MainWindow::~MainWindow()
{
    delete ui;
    XUngrabKeyboard(QX11Info::display(), CurrentTime);
    XUngrabPointer(QX11Info::display(), CurrentTime);
}

void MainWindow::resizeEvent(QResizeEvent *event) {
    this->resizeSlot();
}

void MainWindow::resizeSlot() {
    ui->paddingFrame->resize(this->width(), 0);
    ui->Cover->setGeometry(0, 0, this->width(), this->height());

    if (imageSize.width() < imageSize.height()) {
        ui->ImageLabel->resize(this->width(), this->width() / ((float) imageSize.width() / (float) imageSize.height()));
        ui->ImageLabel->move(0, this->height() / 2 - ui->ImageLabel->height() / 2);
    } else {
        ui->ImageLabel->resize(this->height() * ((float) imageSize.width() / (float) imageSize.height()), this->height());
        ui->ImageLabel->move(this->width() / 2 - ui->ImageLabel->width() / 2, 0);
    }
}

void MainWindow::keyPressEvent(QKeyEvent *event) {
    hideCover();

    if (event->key() == Qt::Key_PowerOff) {
        //Confirm power off
        on_powerButton_clicked();
    }
}

void MainWindow::on_passwordBox_returnPressed()
{
    if (typePassword) {
        ui->loginButton->click();
    } else {
        hideCover();
    }
}

void MainWindow::on_Cover_clicked()
{

    //ui->Cover->setVisible(false);
}

void MainWindow::on_Cover_MouseDown(QMouseEvent *event)
{
    this->moveY = event->y();
}

void MainWindow::on_Cover_MouseMove(QMouseEvent *event)
{
    QRect geometry = ui->Cover->geometry();
    geometry.translate(0, 0 - this->moveY + event->y());
    ui->Cover->setGeometry(geometry);
}

void MainWindow::on_Cover_MouseUp(QMouseEvent *event)
{
    hideCover();
}

void MainWindow::hideCover() {
    if (!typePassword) {
        QPropertyAnimation* animation = new QPropertyAnimation(ui->Cover, "geometry");
        animation->setStartValue(ui->Cover->geometry());
        if (ui->Cover->geometry().top() > 0) {
            animation->setEndValue(ui->Cover->geometry().translated(0, this->height()));
        } else {
            animation->setEndValue(ui->Cover->geometry().translated(0, -this->height()));
        }
        animation->setDuration(500);
        animation->setEasingCurve(QEasingCurve::OutCubic);
        animation->start();
        typePassword = true;
        ui->passwordBox->setFocus();
    }
}

void MainWindow::showCover() {
    if (typePassword) {
        QPropertyAnimation* animation = new QPropertyAnimation(ui->Cover, "geometry");
        animation->setStartValue(ui->Cover->geometry());
        animation->setEndValue(QRect(0, 0, this->width(), this->height()));
        animation->setDuration(500);
        animation->setEasingCurve(QEasingCurve::OutCubic);
        animation->start();
        connect(animation, SIGNAL(finished()), animation, SLOT(deleteLater()));
        typePassword = false;
        //ui->usernameBox->setText("");
        ui->passwordBox->setText("");
        ui->passwordBox->setFocus();
    }
}

void MainWindow::on_passwordBox_textEdited(const QString &arg1)
{
    if (!typePassword) {
        if (arg1 == " ") {
            ui->passwordBox->setText("");
        }
        hideCover();
    }
}

void MainWindow::on_goBack_clicked()
{
    showCover();
}

void MainWindow::on_sessionSelect_triggered(QAction *arg1)
{
    ui->sessionSelect->setText(arg1->text());
    sessionExec = arg1->data().toString();
}

void MainWindow::on_loginButton_clicked()
{
    pid_t processId;
    if (login(ui->usernameBox->itemData(ui->usernameBox->currentIndex(), Qt::UserRole).toString(), ui->passwordBox->text(), sessionExec, &processId, sessionPath)) {
        if (sessionPath == NULL) {
            closeWindows();
            showCover();

            QTimer* checkRunTimer = new QTimer();
            checkRunTimer->setInterval(1000);
            connect(checkRunTimer, &QTimer::timeout, [=]() {
                int status;
                if (waitpid(processId, &status, WNOHANG) != 0) {
                    checkRunTimer->stop();
                    checkRunTimer->deleteLater();
                    logout();
                    openWindows();
                }
            });
            checkRunTimer->start();

            int status;
            waitpid(processId, &status, 0);
        } else {
            //We're just switching sessions. Show the cover again.
            showCover();
        }
    } else {
        ui->incorrectPassword->setVisible(true);
    }
}

void MainWindow::on_turnOffScreen_clicked()
{
    showCover();
    QProcess::startDetached("xset dpms force off");
}

void MainWindow::on_pushButton_clicked()
{
    QApplication::exit();
}

void MainWindow::on_suspendButton_clicked()
{
    showCover();
    QList<QVariant> arguments;
    arguments.append(true);

    QDBusMessage message = QDBusMessage::createMethodCall("org.freedesktop.login1", "/org/freedesktop/login1", "org.freedesktop.login1.Manager", "Suspend");
    message.setArguments(arguments);
    QDBusConnection::systemBus().send(message);
}

void MainWindow::on_usernameBox_currentIndexChanged(int index)
{
    //Determine if user is logged in
    bool isLoggedIn = false;
    int uid = ui->usernameBox->itemData(index, Qt::UserRole + 1).toInt();
    if (uid != 0) {
        QDBusMessage GetUserMessage = QDBusMessage::createMethodCall("org.freedesktop.login1", "/org/freedesktop/login1", "org.freedesktop.login1.Manager", "GetUser");
        QVariantList args;
        args.append((uint) uid);
        GetUserMessage.setArguments(args);
        QDBusReply<QDBusObjectPath> userPath = QDBusConnection::systemBus().call(GetUserMessage);

        if (userPath.isValid()) {
            QDBusMessage GetSessionMessage = QDBusMessage::createMethodCall("org.freedesktop.login1", userPath.value().path(), "org.freedesktop.DBus.Properties", "Get");
            QVariantList getSessionArgs;
            getSessionArgs.append("org.freedesktop.login1.User");
            getSessionArgs.append("Display");
            GetSessionMessage.setArguments(getSessionArgs);
            QDBusMessage GetSessionReply = QDBusConnection::systemBus().call(GetSessionMessage);
            QDBusVariant GetSessionReplyVariant = GetSessionReply.arguments().first().value<QDBusVariant>();
            QDBusArgument sessionListArgument = GetSessionReplyVariant.variant().value<QDBusArgument>();
            QStringDBusObjectPathMap sessionList;
            sessionListArgument >> sessionList;

            QDBusInterface sessionInterface("org.freedesktop.login1", sessionList.objectPath.path(), "org.freedesktop.login1.Session", QDBusConnection::systemBus());
            if (sessionInterface.property("Display").toString() != "") {
                isLoggedIn = true;
                sessionPath = new QDBusObjectPath(sessionList.objectPath.path());
            }
        }
    }

    if (isLoggedIn) {
        ui->sessionSelect->setVisible(false);
        ui->loginButton->setText("Unlock");
    } else {
        ui->sessionSelect->setVisible(true);
        ui->loginButton->setText("Log In");
        sessionPath = NULL;
    }
}

void MainWindow::on_powerButton_clicked()
{
    /*ui->confirmPowerFrame->setGeometry(0, this->height(), this->width(), ui->confirmPowerFrame->sizeHint().height());
    ui->confirmPowerFrame->setVisible(true);
    //ui->powerOptionsFrame->setVisible(false);
    QPropertyAnimation* anim = new QPropertyAnimation(ui->confirmPowerFrame, "geometry");
    anim->setStartValue(ui->confirmPowerFrame->geometry());
    anim->setEndValue(QRect(0, this->height() - ui->confirmPowerFrame->height(), this->width(), ui->confirmPowerFrame->sizeHint().height()));
    anim->setDuration(500);
    anim->setEasingCurve(QEasingCurve::OutCubic);
    connect(anim, SIGNAL(finished()), anim, SLOT(deleteLater()));
    anim->start();*/

    tToast* toast = new tToast();
    QMap<QString, QString> actions;
    actions.insert("reboot", "Reboot");
    actions.insert("off", "Power Off");
    actions.insert("no", "Cancel");

    toast->setTitle("Power Off");
    toast->setText("You're about to power off your PC. Are you sure?");
    toast->setActions(actions);
    toast->setTimeout(30000);

    QString* action = new QString();
    connect(toast, &tToast::doDefaultOption, [=, &action] {
        toast->announceAction("Powering off...");
        action = new QString("off");
    });
    connect(toast, &tToast::actionClicked, [=, &action](QString key) {
        if (key == "no") return;

        if (key == "reboot") {
            toast->announceAction("Rebooting...");
        } else if (key == "off") {
            toast->announceAction("Powering off...");
        }
        QGraphicsOpacityEffect* effect = new QGraphicsOpacityEffect;
        ui->lockFrame->setGraphicsEffect(effect);

        tPropertyAnimation* anim = new tPropertyAnimation(effect, "opacity");
        anim->setStartValue((float) 1);
        anim->setEndValue((float) 0);
        anim->setDuration(250);
        anim->start();

        action = new QString(key);
    });
    connect(toast, &tToast::dismissed, [=] {
        /*if (action == "reboot") {
            //Reboot the PC
            QDBusMessage message = QDBusMessage::createMethodCall("org.freedesktop.login1", "/org/freedesktop/login1", "org.freedesktop.login1.Manager", "Reboot");
            QList<QVariant> arguments;
            arguments.append(true);
            message.setArguments(arguments);
            QDBusConnection::systemBus().send(message);
        } else if (action == "off") {
            //Power off the PC
            QDBusMessage message = QDBusMessage::createMethodCall("org.freedesktop.login1", "/org/freedesktop/login1", "org.freedesktop.login1.Manager", "PowerOff");
            QList<QVariant> arguments;
            arguments.append(true);
            message.setArguments(arguments);
            QDBusConnection::systemBus().send(message);
        }*/
    });
    toast->show(this);
}

void MainWindow::on_cancelPowerOptions_clicked()
{
    ui->powerOptionsFrame->setVisible(true);

    QPropertyAnimation* anim = new QPropertyAnimation(ui->confirmPowerFrame, "geometry");
    anim->setStartValue(ui->confirmPowerFrame->geometry());
    anim->setEndValue(QRect(0, this->height(), this->width(), ui->confirmPowerFrame->sizeHint().height()));
    anim->setDuration(500);
    anim->setEasingCurve(QEasingCurve::InCubic);
    connect(anim, SIGNAL(finished()), anim, SLOT(deleteLater()));
    anim->start();

}

void MainWindow::on_powerOffButton_clicked()
{
    //Power off the PC
    QDBusMessage message = QDBusMessage::createMethodCall("org.freedesktop.login1", "/org/freedesktop/login1", "org.freedesktop.login1.Manager", "PowerOff");
    QList<QVariant> arguments;
    arguments.append(true);
    message.setArguments(arguments);
    QDBusConnection::systemBus().send(message);
}

void MainWindow::on_rebootButton_clicked()
{
    //Reboot the PC
    QDBusMessage message = QDBusMessage::createMethodCall("org.freedesktop.login1", "/org/freedesktop/login1", "org.freedesktop.login1.Manager", "Reboot");
    QList<QVariant> arguments;
    arguments.append(true);
    message.setArguments(arguments);
    QDBusConnection::systemBus().send(message);
}
