#include "coverframe.h"

CoverFrame::CoverFrame(QWidget *parent) : QFrame(parent)
{

}

void CoverFrame::mousePressEvent(QMouseEvent *event) {
    emit MouseDown(event);
    //emit clicked();
}

void CoverFrame::mouseReleaseEvent(QMouseEvent *event) {
    emit MouseUp(event);
}

void CoverFrame::mouseMoveEvent(QMouseEvent *event) {
    emit MouseMove(event);
}
