#ifndef DISPLAYIMAGE_HPP
#define DISPLAYIMAGE_HPP

#include <QDialog>
#include <QGraphicsScene>


namespace Ui {
class DisplayImage;
}

class DisplayImage : public QDialog
{
    Q_OBJECT

public:

    explicit DisplayImage(QWidget *parent = nullptr, QByteArray fileBuffer = 0, const char* extension = nullptr);
    ~DisplayImage();

private:
    Ui::DisplayImage* mb_userInterface {};
    QGraphicsScene*   mb_scene {};
    QPixmap           mb_pixmap {};
};

#endif // DISPLAYIMAGE_HPP
