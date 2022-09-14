#include "displayimage.hpp"
#include "ui_displayimage.h"

// #### Конструктор класса
// ####
DisplayImage::DisplayImage(QWidget *parent, QByteArray fileBuffer, const char* extension) :
    QDialog             {parent},
    mb_userInterface    {new Ui::DisplayImage}
{
    mb_userInterface->setupUi(this);

    // Загружаем массив QByteArray, содержащий изображение, в объект класса QPixmap
    mb_pixmap.loadFromData(fileBuffer, extension);

    mb_scene = new QGraphicsScene(this);
    mb_scene->addPixmap(mb_pixmap);
    //mb_scene->setSceneRect(mb_scene->itemsBoundingRect());
    mb_scene->setSceneRect(QRectF());

    //mb_userInterface->graphicsViewScreen->showFullScreen();
    mb_userInterface->graphicsViewScreen->setScene(mb_scene);

}


// #### Деструктор класса
// ####
DisplayImage::~DisplayImage()
{
    delete mb_userInterface;
    if (mb_scene != nullptr) {
        delete mb_scene;
    }
    else {} // Nothing to do
}
