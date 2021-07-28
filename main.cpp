#include "mainwindow.h" //Подключаем заголовочные файлы содержащие описания классов, структур
#include "regwind.h"

#include <QApplication> //Подключение библиотеки отбъекта "приложение"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv); //Создаём приложение
    MainWindow w; //Создаём главное окно
    RegWind r(&w); //Создаём окно регистрации связывая его с первым
    r.show(); //Выводим на экран окно регистрации
    return a.exec(); //Ожидаем действий пользователя
}
