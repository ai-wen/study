#ifndef COVERFRAME_H
#define COVERFRAME_H

#include <QWidget>
#include <QFrame>

class CoverFrame : public QFrame
{
    Q_OBJECT
public:
    explicit CoverFrame(QWidget *parent = 0);

signals:
    void clicked();
    void MouseDown(QMouseEvent* event);
    void MouseMove(QMouseEvent* event);
    void MouseUp(QMouseEvent* event);

public slots:

private:
    void mousePressEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);
};

#endif // COVERFRAME_H
