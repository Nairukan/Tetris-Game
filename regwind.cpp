#include "regwind.h" //подключаем нашу библиотеку, содержащую класс окна для входа
#include "ui_regwind.h" //подключение формы окна для входа

RegWind::RegWind(MainWindow* NextGen) : //конструктор класса RegWind
    QMainWindow(nullptr), //Выполняет функционал конструктора стандартного класса QMainWindow
    ui(new Ui::RegWind) //Выполняет функционал конструктора UI-класса
{ //Помимо описанного выше после выпоняется следуещее
    ui->setupUi(this);//Устанавливаю Ui-класс для данного экземпляра окна
    this->setWindowTitle("Ввод Никнейма. Тетрис"); //Устанавливаем имя окна
    RES=QImage("Blocks.png"); //Получаем изображение ресурсов из файла
    this->NextGen=NextGen; //Установка связанного окна
    this->setFixedSize(404, 330); //Установка фиксированного размера
    this->ui->lineEdit->setGeometry(0.118*this->width(), 0.449*this->height(), 0.4*this->width(), 0.086*this->height()); //Установка геометрии поля для ввода текста
    ui->lineEdit->setFocus(); //установка фокуса на него
    this->ui->pushButton->setGeometry(0.587*this->width(), 0.449*this->height(), 0.29*this->width(), 0.086*this->height()); //установка геомертии кнопки входа
    connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(Push())); //Связывание нажатия на кнопку и слотом Push()
    ui->pushButton->setText("Войти"); //Установка надписи на кнопке
    this->ui->label->setGeometry(0.105*this->width(), 0.362*this->height(), 0.415*this->width(), 0.077*this->height()); //Установка геометрии надписи
    ui->label->setText("Введите Никнейм"); //Установка текста
    ui->label->setStyleSheet("color: rgb(255, 255, 255)"); //Установка цвета шрифта
    ui->label->setFont(QFont("Calibri", 11, 11)); //Установка семейста в и размера шрифта
    ui->label_2->setGeometry(0, 0.19*this->height(), this->width(), 0.11*this->height());
    ui->label_2->setAlignment(Qt::AlignCenter);
    ui->label_2->setFont(QFont("Calibri", 20, 11));
    ui->label_2->setText("Тетрис");
    ui->label_2->setStyleSheet("color: rgb(255, 255, 255)");
    ui->label_3->setGeometry(0.436*this->width(), 0.863*this->height(), 0.551*this->width(), 0.053*this->height());
    ui->label_4->setGeometry(0.436*this->width(), 0.921*this->height(), 0.551*this->width(), 0.053*this->height());
    ui->label_3->setAlignment(Qt::AlignRight);
    ui->label_4->setAlignment(Qt::AlignRight);
    ui->label_3->setFont(QFont("Calibri", 9, 11));
    ui->label_4->setFont(QFont("Calibri", 9, 11));
    ui->label_3->setText("Желаю приятной игры)");
    ui->label_3->setStyleSheet("color: rgb(255, 255, 255)");
    ui->label_4->setText("https://github.com/Nairukan");
    ui->label_4->setStyleSheet("color: rgb(255, 255, 255)");
}

RegWind::~RegWind()
{
    delete ui;
}

void RegWind::paintEvent(QPaintEvent *event){
    QPainter p(this);
    p.drawImage(QRect(0,0, this->width(), this->height()), RES, QRect(600, 145, 948, 534));
}

void RegWind::Push(){
    if (ui->lineEdit->text()!=""){ //Если никнейм не пустой
        NextGen->TakeNickname(ui->lineEdit->text()); //Передаём в главное окно никнейм
        this->hide(); //Прячем это окно
        NextGen->show(); //показываем главное окно
    }else{
        QMessageBox::critical(this, "Ошибка", "Введите не пустой никнейм"); //если пустой выводим ошибку
    }
}
