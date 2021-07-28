#include "mainwindow.h" //подключаем нашу библиотеку, содержащую класс главного окна
#include "ui_mainwindow.h" //Подключаем форму главного окна

MainWindow::MainWindow(QWidget *parent) //конструктор класса MainWindow
    : QMainWindow(parent) //Выполняет функционал конструктора стандартного класса QMainWindow
    , ui(new Ui::MainWindow) //Выполняет функционал конструктора UI-класса
{ //Помимо описанного выше после выпоняется следуещее
    ui->setupUi(this); //Устанавливаю Ui-класс для данного экземпляра окна
    int w; //ширина окна
    int h=this->height(); //Заношу высоту окна
    w=h; //Высота экрана не больше ширины, так что такое ухишрение позволяет присвоть ширине высоту
    this->resize(w, h); //Изменяем размер окна
    Figures.reserve(7); //Резервирую в векторе место под 7 элементов
    int k=0; //Счётчик элементов занесённых в вектор фигур
    this->setWindowTitle("Тетрис"); //Устанавливаем имя окна
    BlocksImGL=QImage("Blocks.png"); //Получаем изображение ресурсов из файла
    Ti=new QTimer(this); //Создаю Таймер и привязываю его к этому окну
    SDown=new QTimer(this); //то же самое
    GLOBAL_Timer=new QTimer(this); //то же самое
    //Связываю сигналы прохождения интервала таймеров к соответсвующим им слотам
    connect(Ti, SIGNAL(timeout()), this, SLOT(Tick())); // Ti-указатель элемента передающего сигнал, SIGNAL(timeout()) - показывает какой именно сигнал ждём от него,
  //this-указатель элемента принемающего сигнал, SLOT(Tick())- показывает какой слот элемента, принемающего сигнал, ставим в соответсвие сигналу
    connect(SDown, SIGNAL(timeout()), this, SLOT(FastTick()));
    connect(GLOBAL_Timer, SIGNAL(timeout()), this, SLOT(Faster()));

    B=new QPushButton(this);//создаю кнопку и привязываю её к этому окну
    connect(B, SIGNAL(clicked()), this, SLOT(Start())); //Связываю нажатие на неё со слотом
    B->setText("Начать игру"); //Устанавливаю текст кнопка
    B->setGeometry(this->width()*0.6, this->height()*0.8, this->width()*0.3, this->width()*0.15); //её положение относительно окна и размеры

    SC=new QLabel(this); //создаю надпись и привязываю её к этому окну
    SC->setGeometry(this->width()*0.50, this->height()*0.68, this->width()*0.50, this->height()*0.12); //устанавливаю её положение и размеры
    SC->setAlignment(Qt::AlignCenter); //Устанавливаю выравнивание текста по центру
    SC->setFont(QFont("Comic Sans MS", 20, 12)); //Устанавливаю семейство шрифта и его размер

    ReadScores(); //Вызываю функцию считывания из файла рекордов и занесения в вектор

    //Настройка груп бокса, содержащегося в ui-классе окна, можно посмотреть в mainwindow.ui
    ui->groupBox->setGeometry(0.55*this->width(), 0.4*this->height(), 0.4*this->width(), 0.28*this->height());
    ui->groupBox->setTitle("Таблица рекордов");

    //Настройка области с полосами прокрутки, содержащегося в ui-классе окна, можно посмотреть в mainwindow.ui
    ui->scrollArea->setGeometry(2, 20, ui->scrollArea->parentWidget()->width()-4, ui->scrollArea->parentWidget()->height()-20);
    ui->scrollAreaWidgetContents->resize(ui->scrollArea->width(), ui->scrollArea->height());

    for (int i=0; i<Scores.size(); i++){ //Прохожусь по вектору рекордов
        SCL.push_back(new QLabel(ui->scrollAreaWidgetContents)); //для каждого элемента создаю элемент "Надпись", и привязываю к области внутри поля с полосами прокрутки
        SCL[SCL.size()-1]->setText(QString::number(i+1) +". "+Scores[i].first+":\t"+QString::number(Scores[i].second)); //Устанавливаю текст надписи
        SCL[SCL.size()-1]->setGeometry(2, 15+i*0.4*this->height()/12+(i-1)*2,  SCL[SCL.size()-1]->parentWidget()->width()-4, 0.4*this->height()/12); //устанавливаю положение
        SCL[i]->setFont(QFont("Comic Sans MS", 11, 11, true)); //Устанавливаю семество и размер шрифта
    }
    srand(time(0)); //Устанавливаю семечко для генерации случайных чисел
    thisFigure=new Figure({4,21}, {4,20}, {5,21}, {6,21}, {4, 19, 3, 3}, true, {1, 1}); //выделяю память под элемент структуры "Фигура" и заношу указатель в переменную
    //Заполняю массив возможных фигур
    Figures[k++]=new Figure({4,21}, {4,20}, {5,21}, {6,21}, {4, 19, 3, 3}, true, {1, 1}); //L
    Figures[k++]=new Figure({4,21}, {4,20}, {5,20}, {6,20}, {4, 19, 3, 3}, true, {1, 1}); //J
    Figures[k++]=new Figure({4,21}, {5,21}, {6,21}, {7,21}, {3, 19, 5, 5}, true, {2, 2}); //I
    Figures[k++]=new Figure({4,21}, {5,21}, {6,21}, {5,20}, {4, 19, 3, 3}, true, {1, 1}); //T
    Figures[k++]=new Figure({4,21}, {5,21}, {5,20}, {6,20}, {4, 19, 3, 3}, true, {1, 1}); //Z
    Figures[k++]=new Figure({4,20}, {5,20}, {5,21}, {6,21}, {4, 19, 3, 3}, true, {1, 1}); //S
    Figures[k++]=new Figure({5,21}, {5,20}, {6,20}, {6,21}, {5, 20, 2, 2}, false); //O

    //Устанавливаю активной фигурой случайную фигуру из массива и устанавливаю ей случайный цвет
    *thisFigure=Figures[rand()%7]->MakeCopy();
    thisFigure->UPD_color(rand()%5);

    //прохожусь по всем координатам поля и указываю что они пусты
    for (int i=-1; i<13; i++){
        for (int j=-1; j<=22; j++){
            BB[{i,j}]=nullptr;
        }
    }

    for (int i=21; i>=0; i--){ //создание левой границы
        BB[{0, i}]=new Block(0, i, 5);
    }
    for (int i=1; i<=10; i++){ //создание нижней границы
        BB[{i, 0}]=new Block(i, 0, 5);
    }
    for (int i=0; i<=21; i++){ //создание правой границы
        BB[{11, i}]=new Block(11, i, 5);
    }
    repaint(); //вызов функции отрисовки
}

MainWindow::~MainWindow() //деструктор класса
{
    for (Figure* now : Figures) //Проходим по всем элементам вектора фигур
        delete now; //очищаем выделенную под них память
    delete ui; //очищаем выделенную память под ui-класс
}

bool GG=false; //Показатель не окончилась ли игра

void MainWindow::Start(){ //функция нажатия на кнопку
    GG=true; //Устанавливаем показатель готовый ко встрече с функциями, игра началась
    //прохожусь по всем покрдинатам поле с запасом в высоту и указываю что они пусты
    for (int i=-1; i<13; i++){
        for (int j=-1; j<=26; j++){
            BB[{i,j}]=nullptr;
        }
    }

    //создание границ поля
    for (int i=21; i>=0; i--){
        BB[{0, i}]=new Block(0, i, 5);
    }
    for (int i=1; i<=10; i++){
        BB[{i, 0}]=new Block(i, 0, 5);
    }
    for (int i=0; i<=21; i++){
        BB[{11, i}]=new Block(11, i, 5);
    }

    Ti->start(400); //запускаю таймер нормального падения фигур с заданным интервалом
    GLOBAL_Timer->start(1300); //запускаю таймер ускорения нормального падения
    srand(time(0)); //Устанавливаю семечко для генерации случайных чисел
    //Устанавливаю следующую фигуру случайной фигурой из массива и устанавливаю ей случайный цвет
    NextFigure=Figures[rand()%7];
    NextFigure->UPD_color(rand()%5);

    Score=0; //обнуляю счёт
    SC->setText("Score: 0"); //устанавливаю соответствующуюю надпись
    B->hide(); //Прячу саму кнопку
    SC->show(); //Показываю надпись со счётом
    SC->setFocus(); //Устанавливаю фокус на счёт, так как после нажатия на кнопку фокус остаётся на ней и нажав на пробел можно нажать на неё ещё раз, хоть она и скрыта
    PostMessage(GetForegroundWindow(), WM_INPUTLANGCHANGEREQUEST, 1, 0x04090409); //устанавливаю английскую раскаладку для окна
}

void MainWindow::paintEvent(QPaintEvent *event){ //Функция отрисовки
    //создаю некоторую сетку для приложения, для удобного позиционированния
    int w=this->width()/23;
    int h=this->height()/22;
    int s=std::min(w, h);

    QPainter p(this);//Создаю элемент для рисования и привязываю его к главному окну
    p.drawImage(QRect(0,0, this->width(), this->height()), BlocksImGL, QRect(0, 145, 600, 393)); //отрисовка заднего фона из изображения ресурсов
    p.setPen(Qt::black); //установка painter-у чёрного цвета кисти
    for (int i=0; i<12; i++){ //проходим по всему полю
        for (int j=0; j<21; j++){

            if (BB[{i, j}]!=nullptr){ //если по некоторым координаты находится блок
               p.drawImage(QRect(i*s, this->height()-s-j*s, s, s), BlocksImGL, QRect(BB[{i, j}]->color*144.66, 0, 144, 145)); //То рисуем этот блок в соответствии с его координатами и цветов
            }
        }
    }

    if (thisFigure!=nullptr){ //если у нас есть активный элемент
        for (int i=0; i<4; i++) //то проходим по его блокам
            //Рисуем блоки в соответствии с их координатами и цветом
            p.drawImage(QRect(thisFigure->Elem[i]->posX*s, this->height()-s-thisFigure->Elem[i]->posY*s, s, s), BlocksImGL, QRect(thisFigure->Elem[0]->color*144.66, 0, 144, 145));
    }
    if (NextFigure!=nullptr){//если у нас есть следующий элемент
        s=std::min(0.5*this->width()/6, 0.5*this->height()/5); //создание ещё одной сетки
        for (int i=0; i<4; i++) //проходим по всем его блокам
            //Рисуем блоки в соответствии с их координатами, но вынесенное в отдельную область и цветом
            p.drawImage(QRect((NextFigure->Elem[i]->posX-3)*s+this->width()*0.5, s/2+(21-NextFigure->Elem[i]->posY)*s, s, s), BlocksImGL, QRect(NextFigure->Elem[0]->color*144.66, 0, 144, 145));
        p.drawRect(s+this->width()*0.5, s/2, s*4, s*4); //рисование границы для следующей фигуры
    }
}

bool Busy=false;

void MainWindow::StopFallFigure(){ //функция вызываемая при встрече активной фигуры с блоками поля
    if (GG){ //Если игра не завершилась
        delete thisFigure; //то осовобождаем память выделенную под активную фигуру
        thisFigure=nullptr; //Удаляем указатель на эту память
    thBeg:{} //Точка останова для прыжков через goto, нужна для циклической проверки на сбор целого ряда
        int firstJ=21; //индекс высоты на которой встретился целый ряд, изначально 21 чтобы было ясно что целых рядов нету
        for (int j=1; j<26; j++){ //начинаем цикл с 1 высоты и идём вверх
            int i=1; //Устанавливаем временный показатель координаты х, начинаем с 1
            for  (; i<11; i++){ //проходим до 11
                if (BB[{i,j}]==nullptr) break; //если на j-ом ряду встретиться пустой блок на i-ой позиции, то прерываем цикл
            }
            if (i==11){ //по результату цикла смотрим дошло ли i до конца (является ли ряд заполненным)
                firstJ=j;  //заношу в переменную высоту первого ряда
                break; //выхожу из цикла
            }//если условие не выполненно перехожу к следующей высоте
        }
        int j, sc=0; //создаю переменные: 1-показатель координаты y, 2-увеличение счёта от уничтожения рядов
        //проверим на целостность ряды стоящие выше или равнво
        for (j=firstJ; j<std::min(firstJ+5, 21); j++){ //Максимальное кол-во подряд идущих собранных рядов = 4(тетрис), однако выше 21 высоты они не могут находиться
      //, если не было собранных рядов ничего не происходит
            for (int i=0; i<12; i++){ //проходим по х
                if (BB[{i,j}]==nullptr){ //если встречается пустой блок
                    goto ECheckB; //выходим из циклов
                }
            }
            sc=sc*2+100; //Увеличиваем счёт по формуле за уничтоженный ряд

        }//по итогу выполнения фунции j хранит высоту последнего целого ряда+1, который относиться к одной и той же собранной области с рядом на высоте firstJ
        ECheckB: //Точка останова для перехода с помощью goto
        Score+=sc; //Увеличиваем счёт
        SC->setText("Score: "+QString::number(Score)); //Устанавливаем новый текст надпись, соответствующий счёту
        for (int i=firstJ; i<j; i++){ //проходим по всем элементам в собранной области(множеству подрядидущих собранных рядов), если не было собранных рядов ничего не происходит
            for (int q=1; q<11; q++){
                BB[{q, i}] = nullptr; //Указываем что они пусты
            }
        }
        int sm=j-firstJ; //находим кол-во рядов в области
        for (j=firstJ; j<21; j++){ //Проходим по всем блокам выше ряда, которые являлся первым собранным, если не было собранных рядов ничего не происходит
            for (int i=1; i<11; i++){
                BB[{i, j}]=BB[{i, j+sm}]; //смещаем блоки на sm строк вниз
            }
        }
        repaint(); //вызываем функцию отрисовки
        if (firstJ!=21) //если был хоть один собранный, то перейдём на точку thBeg, для повторной проверки, может ещё остались собранные ряды
        goto thBeg;
    }
}



void MainWindow::GameOver(){ //Функция вызываемая когда активный блок пересекается блоками с блоками поля (когда игра окончена)
    if (GG){ //если игра не окончена, эта проверка нужна для того что может быть ситуация что 2 разных таймера допустим ускоренный
    //и обычной просчитают проигрыш за тот-же такт, тогда вызвалось бы сразу 2 проигрыша, а нужен только 1
        GG=false; //устанавливаем что игра окончена
        Ti->stop(); //Останавливаем таймер нормального падения
        SDown->stop(); //Останавливаем таймер ускоренного падения
        Scores.push_back({this->Nickname, Score}); //Добавляем в конец вектора рекордов, запись игрока
        SortScores(); //Сортируем рекорды, удаляем лишние, выводим их на экран
        WriteScores();  //заносим обновлённые рекорды в файл
        QMessageBox::information(this, "", "Вы проиграли!"); //Вызываем плавующее окно которое уведомляет о проигрыше
        B->show(); //Проявляем кнопку для начала игры
        thisFigure=nullptr; //обнуляем указатель на активную фигуру
        Cr=true; //Указываем что при возможности нужно сгененрировать следующую фигуру
    }
}

void MainWindow::Tick(){ //Функция вызываемая таймером нормального падения по прохождению некоторого интервала
    if (Cr){ //Если необходимо Сгенерировать новую фигуру
        thisFigure=new Figure({4,21}, {4,20}, {5,21}, {6,21}, {4, 19, 3, 3}, true, {1, 1}); //Выделяем память под фигуру
        *thisFigure=NextFigure->MakeCopy(); //Активной фигуре присваиваем клона следующей фигуры
        thisFigure->UPD_color(NextFigure->Elem[0]->color); //Обновляем соответствующий цвет фигуры
        NextFigure=Figures[rand()%7]; //Генерация следующей фигуры
        thisFigure->UpdatePos(); //Переносим активную фигуру к началу падения, востанавливаем начальную геометрию
        NextFigure->UPD_color(rand()%5); //Установка случайного цвета следующей фигуре
        Cr=false; //Устанавливаем показатель что генерировать новую фигуру не надо
    }else{ //Если же генерировать фигуру не надо
        for (int i=0; i<4; i++){ //проходим по состовляющим активной фигуры
            if (BB[{thisFigure->Elem[i]->posX, thisFigure->Elem[i]->posY}]!=nullptr){ //проверяя есть ли на поле блок с координатами блока активной фигуры
                GameOver(); //Вызываем окончание игры
                goto EndTick; //переходим в конец функции
            }
        }//если мы дошли до сюда то игра не прекратилась
        for (int i=0; i<4; i++){ //Проходим по состовляющим активной фигуры
            if (BB[{thisFigure->Elem[i]->posX, thisFigure->Elem[i]->posY-1}]!=nullptr){ //Если же на поле есть блок ниже блока активной фигуры
                //то создаём блоки фигуры на поле и заносим их в словарь элементов по координатам
                for (int j=0; j<4; j++){
                    Block* BL=new Block(thisFigure->Elem[j]->posX, thisFigure->Elem[j]->posY, thisFigure->Elem[0]->color);
                    BB[{thisFigure->Elem[j]->posX, thisFigure->Elem[j]->posY}]=BL;
                }

                thisFigure=nullptr; //обнуляем указатель на активную фигуру
                StopFallFigure(); //вызываем функцию остановки активной фигуры
                Cr=true; //Указываем что необходима генерация фигуры
                goto EndTick; //переход в конец функции
            }
        }
        //иначе перемещаем фигуру на позицию вниз и её область действия
        for (int i=0; i<4; i++){
            thisFigure->Elem[i]->posY--;
        }
        thisFigure->RecBoundsModel.setY(thisFigure->RecBoundsModel.y()-1);

    }
    EndTick:{}
    repaint(); //Вызвов функции отрисовки
}

unsigned short killFastTick=0; //счётчик определяющий сколько раз сработает таймер быстрого падения

void MainWindow::FastTick(){ //функция вызываемая таймером быстрого падения с некоторым интервалом
    if (killFastTick==7){ //если счётчик равен 7, примерно столько нужно чтобы началась циклическая отправка сигнала о отпускании клавиши S
        killFastTick=0; //Обнуляем счётчик для следующих разов
        Ti->start(); //Запускаем таймер обычного падения
        SDown->stop(); //Останавливаем таймер быстрого падения
    }
    else {
        killFastTick++; //Иначе увеличиваем счётчик
        Tick(); //и вызываем обработку падения фигуры
    }
}

void MainWindow::keyPressEvent(QKeyEvent *event){ //Встроеная функция вызывается при нажатии клавиши клавиатуры
  if (event->key()==Qt::Key_Escape){ //если нажатая клавиша равна скан коду клавиши esc
      this->close(); //закрываем приложение
  }
  if (thisFigure!=nullptr){ //если существует активная фигура
      if (event->key()==Qt::Key_A){ //сравниваем нажатую клавишу с клавишей A
          //Проверяем можно ли сдвинуть фигуру влево
          for (int i=0; i<4; i++){
              if (BB[{thisFigure->Elem[i]->posX-1, thisFigure->Elem[i]->posY}]!=nullptr){
                goto EndPress;
              }
          }
          //если программа не перешла на отметку EndPress, то сдвигаем фигуру влево и её область действия
          for (int i=0; i<4; i++){
              thisFigure->Elem[i]->posX--;
          }
          thisFigure->RecBoundsModel.setX(thisFigure->RecBoundsModel.x()-1);

          repaint();
      }else if (event->key()==Qt::Key_D){ //по аналогии с движением вправо
          for (int i=0; i<4; i++){
              if (BB[{thisFigure->Elem[i]->posX+1, thisFigure->Elem[i]->posY}]!=nullptr){
                goto EndPress;
              }
          }
          for (int i=0; i<4; i++){
              thisFigure->Elem[i]->posX++;
          }
          thisFigure->RecBoundsModel.setX(thisFigure->RecBoundsModel.x()+1);
          repaint();
      }else if (event->key()==Qt::Key_S){ //обработка ускорения падения
          Ti->stop(); //останавливаем таймер обычного падения
          killFastTick=0; //Обнуляем счётчик жизни ускоренного таймера
          Suport=true; //устанавлиаем показатель нажатия на клавишу S равной true
          SDown->start(50); //Запускаем ускоренный таймер
      }else if (event->key()==Qt::Key_Space){ //Обработка вращения фигуры
          RotateActiveFigure(); //Вызываем функцию вращения активной фигуры
          repaint();
      }
    EndPress:{}
  }
}

void MainWindow::keyReleaseEvent(QKeyEvent *event){ //Встроеная функция вызывается при отпускании клавиши клавиатуры
    if (thisFigure!=nullptr && event->key()==Qt::Key_S){ //Если активная фигура существует и отпущеная клавиша является клавишей S
        SDown->stop();  //останавливаем таймер ускоренного падения
        Suport=false;   //устанавлиаем показатель нажатия на клавишу S равной true
        if (!Suport){ //Если отпускание клавишы было вызвано но показатель Support показывает что отпускание было обработано,
            //то текущий вызов является циклическим вызванным удерживанием клавиши нажатой
            Tick(); //Вызываем обработчик падения фигуры
        }
        Ti->start(); //Запускаем таймер нормального падения
    }
}

void MainWindow::RotateActiveFigure(){ //Функцию вращения активной фигуры
    Figure TFig(thisFigure->MakeCopy()); //Создаём копию активной фигуры с теми же координатами
    TFig.Rotate(); //Вызываем вращение копии фигуры
    //Проходим по всем элементам копии активной фигуры и проверяем пересекается ли она с блоками, если пересекается
   //то вращение активной фигуры невозможно и заканчиваем выполнение функции
    for (int i=0; i<4; i++){
        if (BB[{TFig.Elem[i]->posX, TFig.Elem[i]->posY}]!=nullptr) goto EndRot;
    }
    thisFigure->Rotate(); //Если же вращение копии возможно, то вращаем активную фигуру
    EndRot:{}
}

void MainWindow::Faster(){ //функция вызываемая таймером ускорения падения с некоторым интервалом
    Ti->setInterval(Ti->interval()-1); //Уменьшаем интервал таймера обычного падения
}

void MainWindow::TakeNickname(QString Nick){ //функция получения Никнейма игрока, вызывается из окна входа, для передачи от туда Никнейма
    Nickname=Nick; //Запоминаем
}

int str_to_int(QString x){ //Функция перевода Строки содержащей число к числу
    int ans=0; //Ответ изначально 0
    for (int i=0; i<x.length(); i++){ //Проходим по всем символам строки
        ans*=10; //Умножаем ответ на 10
        ans+=(x[i].digitValue()); //Прибавляем число соответсвующее этому символу
    }
    return ans; //Возвращаем ответ
}

void MainWindow::ReadScores(){ //Считывание рекордов из файла
    QString path = QCoreApplication::applicationDirPath(); //Создаём строку равную пути в файловой системе к исполняемому файлу
    QFile FileW; //Создаём файл
    QTextCodec* defaultTextCodec = QTextCodec::codecForName("UTF-8"); //Создаём кодек текста UTF-8, для поддержки хранения в файле русских символов
    FileW.setFileName(path+"/SCORES.sc"); //Устанавливаем путь к файлу рекордов
    QTextDecoder *decoder = new QTextDecoder(defaultTextCodec);
    if (FileW.open(QIODevice::ReadOnly)){ //Открываем файл с флагом только для чтения, и смотрим успешно ли открытия файла
        QString buffer=defaultTextCodec->toUnicode(FileW.readAll()); //Заносим в строку весь текст из файла рекордов, пройдённый через кодек UTF-8, приведённый к юникоду
        QStringList BALlist =buffer.split(QRegExp("\\s+")); //Определяем список строк, равный множеству строк полученный при разделении буфера на слова
        Scores.reserve(BALlist.size()/2); //Резервируем в векторе рекордов память под кол-во записей
        for (int i=0; i<BALlist.size()/2; i++){ //проходим по списку строк
            Scores.push_back({BALlist[i*2], str_to_int(BALlist[i*2+1])}); //заносим в вектор рекордов записи из файла
        }
        FileW.close(); //Закрываем файл
    }//если неуспешно откртие то не выполняем код выше
}

void MainWindow::WriteScores(){ //Запись рекордов из файла
    QString path = QCoreApplication::applicationDirPath(); //Создаём строку равную пути в файловой системе к исполняемому файлу
    QFile FileW; //Создаём файл
    FileW.setFileName(path+"/SCORES.sc");//Устанавливаем путь к файлу рекордов
    if (FileW.open(QIODevice::WriteOnly)){ //Открываем файл с флагом только для записи, и смотрим успешно ли открытия файла
        for (int i=0; i<Scores.size(); i++){ //проходим по элементам вектора рекордов
            FileW.write(Scores[i].first.toUtf8()+" "+QString::number(Scores[i].second).toUtf8()+"\n"); //Записываем в файл рекорды приведённые к тексту и к кодировке UTF-8
        }
        FileW.close(); //Закрываем файл
    }//если неуспешно откртие то не выполняем код выше
}

bool rule(std::pair<QString, int> t1, std::pair<QString, int> t2){ //Правило сортировки рекордов
    return t1.second>t2.second; //Сравниваются именно баллы, без внимания на Никнеймы
}

void MainWindow::SortScores(){ //Функция Сортировки записей
    std::sort(Scores.begin(), Scores.end(), rule); //Сортируем элементы вектора рекордов по правилу rule
    if (Scores.size()>6){ //если размер вектора > 6
        Scores.erase(Scores.begin()+6); //то удаляем 7 элемент
    }else //иначе
        SCL.push_back(new QLabel(this->ui->scrollAreaWidgetContents)); //Добавляем к вектору элементов типа "надпись" новую напдпись привязанную к области с полосами прокрутки
    SCL[SCL.size()-1]->show(); //Проявляем последнюю надпись рекордов
    for (int i=0; i<Scores.size(); i++){ //Проходим по всем надписям рекордов
        SCL[i]->setText(QString::number(i+1) +". "+Scores[i].first+":         "+QString::number(Scores[i].second)); //Устанавливаем текст надписи
        SCL[i]->setGeometry(2, 15+i*0.12*ui->groupBox->height()+(i-1)*2,  SCL[i]->parentWidget()->width()-10, 0.4*this->height()/12); //Позицию
        SCL[i]->setFont(QFont("Comic Sans MS", 11, 11, true)); //семейство шрифтов и размер
    }
    repaint();
}

void MainWindow::resizeEvent(QResizeEvent * event){ //ствндартная функция вызываемая при изменении размера окна
    //Меняем геометрию объектов ui-класса
    ui->groupBox->setGeometry(0.55*this->width(), 0.4*this->height(), 0.4*this->width(), 0.28*this->height());
    ui->scrollArea->resize(ui->scrollArea->parentWidget()->width()-4, ui->scrollArea->parentWidget()->height()-20);

    SortScores(); //Сортируем записи так как там обновятся положения надписей рекордов
    this->B->setGeometry(this->width()*0.6, this->height()*0.8, this->width()*0.3, this->width()*0.15); //обновляем геометрию кнопки
    this->SC->setGeometry(this->width()*0.50, this->height()*0.68, this->width()*0.50, this->height()*0.12); //надписи счёта
}

void MainWindow::on_action_triggered()
{
    QMessageBox::information(this, "Управление", "Управление игрой Тетрис клавиатурой:\nПробел - Поворот фигуры;\nКлавиши A/D - Передвижение фигуры;\nКлавиша S - быстрое падение.");
}

void MainWindow::on_action_2_triggered()
{
    QMessageBox::information(this, "Правила", "Правила игры Тетрис:\nСтарайтесь сложить падающие блоки так, чтобы они заполнили форму уровень за уровнем и как можно меньше пустот.\nЦвет блоков тетриса не имеет значения.");
}
