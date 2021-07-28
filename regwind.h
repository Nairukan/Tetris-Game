#ifndef REGWIND_H
#define REGWIND_H

#include <QMainWindow>
#include "mainwindow.h" //Подключаем нашу библиотеку хранящую класс главного окна

namespace Ui {
class RegWind; // Определение UI-класса наперёд, чтобы визуальное редактирование не приводило к крупным перекомпиляциям
}

class RegWind : public QMainWindow //Класс окна входа, наследующие свойства и функции из стандартного класса окна
{
    Q_OBJECT //Макрос, определяющий данных класс как объект Qt, и добавляет необходимые функции для компиляции

public: //Флаг доступа "публичный", к данным членам можно обращаться извне класса
    RegWind(MainWindow* NextGen); //Конструктор, принемающий указатель на наше главное окно
    MainWindow* NextGen; //Указатель на главное окно
    ~RegWind(); //Деструктор
    void paintEvent(QPaintEvent* event); //Переопеределяем функцию отрисовки окна
    QImage RES; //Изображение ресурсов для отрисовки, текстур
public slots:
    void Push(); //Нажатие на кнопку

private:
    Ui::RegWind *ui;
};

#endif // REGWIND_H
