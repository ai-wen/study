#include "pam.h"

extern QString currentVt;

static pam_handle_t *pamHandle;
QString currentPassword, currentUsername;
bool login(QString username, QString password, QString execFile, pid_t *child_pid, QDBusObjectPath* resumeSession) {
    //Prevent paging to disk
    mlockall(MCL_CURRENT | MCL_FUTURE);

    //Set XDG environment variables
    //Get current user information
    struct passwd *pw = getpwnam(username.toStdString().data());

    //Get Current Session
    QDBusInterface sessionInterface("org.freedesktop.login1", "/org/freedesktop/login1/session/self", "org.freedesktop.login1.Session", QDBusConnection::systemBus());
    qputenv("XDG_SESSION_ID", sessionInterface.property("Id").toString().toUtf8());
    qputenv("XDG_VTNR", sessionInterface.property("VTNr").toString().toUtf8());
    qputenv("XDG_RUNTIME_DIR", QString("/run/user/" + QString::number(pw->pw_uid)).toUtf8());

    //TODO: Correct these environment variables
    qputenv("XDG_SESSION_PATH", "/org/freedesktop/DisplayManager/Session0");
    qputenv("XDG_SEAT_PATH", "/org/freedesktop/DisplayManager/Seat0");
    //qputenv("XDG_SEAT", "seat0");

    const char *data[2];
    data[0] = username.toStdString().data();
    data[1] = password.toStdString().data();
    struct pam_conv pamConversationParams = {
        conversation, data
    };


    currentUsername = username;
    currentPassword = password;

    int result = pam_start("thedm", username.toStdString().data(), &pamConversationParams, &pamHandle);
    if (result != PAM_SUCCESS) {
        //ERROR ERROR
        qCritical() << "pam_start failed";
        return false;
    }

    /*pam_misc_setenv(pamHandle, "XDG_SEAT", "seat0", false);
    pam_misc_setenv(pamHandle, "XDG_VTNR", sessionInterface.property("VTNr").toString().toUtf8(), false);*/

    result = pam_authenticate(pamHandle, 0);
    if (result != PAM_SUCCESS) {
        //ERROR ERROR
        qCritical() << "pam_authenticate failed with error " + QString::number(result);
        return false;
    }

    //Make sure that user is currently allowed to log in
    result = pam_acct_mgmt(pamHandle, 0);
    if (result != PAM_SUCCESS) {
        //ERROR ERROR
        qWarning() << "pam_acct_mgmt failed";
        return false;
    }

    //Get an authentication token
    result = pam_setcred(pamHandle, PAM_ESTABLISH_CRED);
    if (result != PAM_SUCCESS) {
        //ERROR ERROR
        qWarning() << "pam_setcred failed";
        return false;
    }

    //Release keyboard and mouse locks
    XUngrabKeyboard(QX11Info::display(), CurrentTime);
    XUngrabPointer(QX11Info::display(), CurrentTime);

    //At this point, the user is authenticated.
    //If the user currently has a session that should be unlocked, activate that session now.
    if (resumeSession != NULL) {
        //Activate the session.
        QDBusMessage activateMessage = QDBusMessage::createMethodCall("org.freedesktop.login1", resumeSession->path(), "org.freedesktop.login1.Session", "Activate");
        QDBusConnection::systemBus().call(activateMessage, QDBus::NoBlock);

        //Unlock the session
        QDBusMessage unlockMessage = QDBusMessage::createMethodCall("org.freedesktop.login1", resumeSession->path(), "org.freedesktop.login1.Session", "Unlock");
        QDBusConnection::systemBus().call(unlockMessage, QDBus::NoBlock);


        //Log out of this session.
        return logout();
    }

    result = pam_open_session(pamHandle, 0);
    if (result != PAM_SUCCESS) {
        //ERROR ERROR
        //Release the authentication token
        pam_setcred(pamHandle, PAM_DELETE_CRED);
        qWarning() << "pam_open_session failed";
        return false;
    }

    //Set up other environment variables
    qputenv("HOME", pw->pw_dir);
    qputenv("PWD", pw->pw_dir);
    qputenv("SHELL", pw->pw_shell);
    qputenv("USER", pw->pw_name);
    qputenv("PATH", "/usr/local/sbin:/usr/local/bin:/usr/bin");
    qputenv("MAIL", _PATH_MAILDIR);
    qputenv("XAUTHORITY", QByteArray(pw->pw_dir) + "/.Xauthority");
    qputenv("XDG_SESSION_CLASS", "user");

    //Set up environment variables fiven by PAM
    char** pamEnv = pam_getenvlist(pamHandle);
    for (int i = 0; pamEnv[i] != NULL; i++) {
        QString variable = QString::fromLocal8Bit(pamEnv[i]);
        int equalIndex = variable.indexOf("=");
        if (equalIndex != -1) {
            qputenv(variable.left(equalIndex).toUtf8(), variable.mid(equalIndex + 1).toUtf8());
        }

        free(pamEnv[i]);
    }
    free(pamEnv);

    //Blank out the current password
    currentPassword = "";

    //Fork the process and start the desktop environment
    *child_pid = fork();
    //qDebug() << "PID of new session: " + QString::number(*child_pid);
    if (*child_pid == 0) {
        //Change the UID and GID of this new process
        setsid();
        //Set group information
        setgid(pw->pw_gid);
        initgroups(username.toStdString().data(), pw->pw_gid);

        setuid(pw->pw_uid);
        chdir(pw->pw_dir);

        //Start DBus
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

        //Start the new process
        execl(pw->pw_shell, pw->pw_shell, "-c", execFile.toStdString().data(), NULL);
        qDebug() << "execl failed.";
        exit(1);
    }

    //Start the desktop environment
    /*QProcess* sessionProcess = new QProcess();
    sessionProcess->setProgram(execFile);
    sessionProcess->start();
    *child_pid = sessionProcess->processId();*/
    return true;
}

bool logout() {
    qputenv("XDG_SESSION_CLASS", "greeter");

    int result = pam_close_session(pamHandle, 0);
    if (result != PAM_SUCCESS) {
        //ERROR ERROR
        //Release the authentication token
        pam_setcred(pamHandle, PAM_DELETE_CRED);
        qWarning() << "pam_close_session failed";
        return false;
    }

    //Release the authentication token
    result = pam_setcred(pamHandle, PAM_DELETE_CRED);
    if (result != PAM_SUCCESS) {
        //ERROR ERROR
        qWarning() << "pam_setcred failed";
        return false;
    }

    pam_end(pamHandle, result);
    pamHandle = NULL;

    //Capture the keyboard and mouse
    XGrabKeyboard(QX11Info::display(), RootWindow(QX11Info::display(), 0), True, GrabModeAsync, GrabModeAsync, CurrentTime);
    XGrabPointer(QX11Info::display(), RootWindow(QX11Info::display(), 0), True, None, GrabModeAsync, GrabModeAsync, RootWindow(QX11Info::display(), 0), None, CurrentTime);

    return true;
}

int conversation(int num_msg, const pam_message **msg, pam_response **resp, void *appdata_ptr) {
    //Check for memory buffer errors
    *resp = (pam_response*) calloc(num_msg, sizeof(struct pam_response));
    if (*resp == NULL) {
        return PAM_BUF_ERR;
    }

    int result = PAM_SUCCESS;
    for (int i = 0; i < num_msg; i++) {
        char *username, *password;
        switch(msg[i]->msg_style) {
        case PAM_PROMPT_ECHO_ON:
            //PAM is looking for the username
            qDebug() << "Passing username to PAM...";
            (*resp)[i].resp = strdup(currentUsername.toStdString().data());
            break;
        case PAM_PROMPT_ECHO_OFF:
            //PAM is looking for the password
            qDebug() << "Passing password to PAM...";
            (*resp)[i].resp = strdup(currentPassword.toStdString().data());
            break;
        case PAM_ERROR_MSG:
            qWarning() << msg[i]->msg;
            result = PAM_CONV_ERR;
            break;
        case PAM_TEXT_INFO:
            qWarning() << msg[i]->msg;
            break;
        }
        if (result != PAM_SUCCESS) {
            break;
        }

    }

    if (result != PAM_SUCCESS) {
        //If there was an issue, free memory pointers
        free(*resp);
        *resp = 0;
    }

    return result;
}
