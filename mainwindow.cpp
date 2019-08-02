#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QtCore/QDebug>
#include <iostream>
#include <QThread>
#include <QMessageBox>

QTextStream cinput(stdin);
QTextStream coutput(stdout);



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_serial(new QSerialPort(this))

{
    ui->setupUi(this);
    this->on_bnt_listPorts_clicked();
    ui->label_status->setVisible(false);

    //connect(m_serial, &QSerialPort::errorOccurred, this, &MainWindow::handleError);
    //connect(m_serial, &QSerialPort::readyRead, this, &MainWindow::readData);
    //connect(m_console, &Console::getData, this, &MainWindow::writeData);


}

MainWindow::~MainWindow()
{
        m_serial->close();
        delete ui;
}

void MainWindow::on_bnt_format_clicked()
{
    // Código para formatar eeprom do Arduino
    QByteArray input;
    if (m_serial->isOpen() && m_serial->isWritable())
       {
        m_serial->flush();
        m_serial->write("FORMAT\r");
        while(m_serial->waitForBytesWritten(500));
        qDebug()<<"Formatando:";

        input.clear();
        while(m_serial->waitForReadyRead(5000) and input != "FORMATED\r\nDONE\r\n" ) // Firt step is very slow so we nedd to wai more time; There are many eeprom address to write
        {
                input.append(m_serial->readAll());

        }
        qDebug()<<input;

       }

}
void MainWindow::on_bnt_readData_clicked()
{
    // Vamos tentar ler o que vem do Arduino!
    QByteArray input;
    input.clear();
    bool endOfConfig = false;
    QString cfg_buffer;

    ui->bnt_readData->setDisabled(true);
    m_serial->flush();

    if (m_serial->isOpen() && m_serial->isWritable())
       {
        m_serial->flush();
        m_serial->write("CFG\r");
        while(m_serial->waitForBytesWritten());
        qDebug()<<">CFG";


        QString tche_command;
        char c;
        //QThread::msleep(500);
        tche_status("Reading.");
        while(m_serial->waitForReadyRead(1000)){ tche_status("Reading."); }

        while(( m_serial->bytesAvailable() >= 1 ) and endOfConfig != true )
        {
            c = char(m_serial->read(1)[0]);
            if(c == 13)
            {
                //Parser Here
                tche_parser(tche_command);
                tche_command.clear();
            }else if( c == 10){ /* Do nothing */ }
            else{
                tche_command.append(c);
            }
            if(tche_command == "####END####") endOfConfig = true;

        }
        m_serial->flush();
        tche_status("done");
      }

    ui->bnt_readData->setEnabled(true);
}

void MainWindow::tche_status(QString option){


    if(option == "Reading."){

        if(ui->label_status->text() == "Reading." ) ui->label_status->setText("Reading..");
        if(ui->label_status->text() == "Reading.." ) ui->label_status->setText("Reading...");
        if(ui->label_status->text() == "Reading..." ) ui->label_status->setText("Reading....");
        if(ui->label_status->text() == "Reading...." ) ui->label_status->setText("Reading.");
        else ui->label_status->setText("Reading.");
        ui->label_status->setVisible(true);

    }else if( option == "done"){
          ui->label_status->setVisible(false);

    }else{
         ui->label_status->setText(option);
         ui->label_status->setVisible(true);

    }
    this->repaint();




}

void MainWindow::tche_parser(QString tche_command){
    qDebug()<<"Parser## " + tche_command;

    QStringList Command_value;

    Command_value = tche_command.split(":");
   //Basics
   if(Command_value[0]=="CALL") ui->lineCall->setText(Command_value[1]);
   if(Command_value[0]=="SSID") ui->lineSSID->setText(Command_value[1]);
   if(Command_value[0]=="SYMBOL") ui->lineSymbol->setText(Command_value[1]);
   if(Command_value[0]=="SYMBOLTABLE") Command_value[1] == "1" ? ui->combo_symbolTable->setCurrentText("Alternate") : ui->combo_symbolTable->setCurrentText("Normal");
   if(Command_value[0]=="PATH1SSID") Command_value[1] == "1" ? ui->combo_path1->setCurrentText("WIDE1-1") : ui->combo_path1->setCurrentText("WIDE1-2");
   if(Command_value[0]=="PATH2SSID") Command_value[1] == "1" ? ui->combo_path2->setCurrentText("WIDE2-1") : ui->combo_path2->setCurrentText("WIDE2-2");
   if(Command_value[0]=="MESSAGE") ui->lineMessage->setText(Command_value[1]);

   //Course Trigger Options
   if(Command_value[0] == "COURSEANGLE"){
       if(Command_value[1] == "0"){
           ui->checkBox_course->setChecked(true);
           ui->checkBox_course->clicked(true);
       }else{
           ui->checkBox_course->setChecked(false);
           ui->checkBox_course->clicked(false);

       }
    }
   if(Command_value[0] == "COURSEANGLE" ) { ui->combo_cangle->setCurrentText(Command_value[1]); }
   if(Command_value[0] == "COURSETIME"  ) { ui->combo_ctime->setCurrentText(Command_value[1]);  }
   if(Command_value[0] == "COURSESPEED" ) { ui->combo_cspeed->setCurrentText(Command_value[1]); }


   //Speed Trigger Oprions
   if(Command_value[0] == "SPEED1" ) { ui->combo_speed1->setCurrentText(Command_value[1]); }
   if(Command_value[0] == "SPEED2" ) { ui->combo_speed2->setCurrentText(Command_value[1]); }
   if(Command_value[0] == "SPEED3" ) { ui->combo_speed3->setCurrentText(Command_value[1]); }
   if(Command_value[0] == "DELAY1" ) { ui->combo_delay1->setCurrentText(Command_value[1]); }
   if(Command_value[0] == "DELAY2" ) { ui->combo_delay2->setCurrentText(Command_value[1]); }
   if(Command_value[0] == "DELAY3" ) { ui->combo_delay3->setCurrentText(Command_value[1]); }
   if(Command_value[0] == "DELAY4" ) { ui->combo_delay4->setCurrentText(Command_value[1]); }



   //Radio settings
   if(Command_value[0]=="PREAMBLE") ui->line_txpreamble->setText(Command_value[1]);
   if(Command_value[0]=="TAIL") ui->line_txtail->setText(Command_value[1]);

   //User Signaling
   if(Command_value[0]=="TXBEEP") ui->line_txbeep->setText(Command_value[1]);
   if(Command_value[0]=="RXBEEP") ui->line_rxbeep->setText(Command_value[1]);
   if(Command_value[0]=="REPBEEP") ui->line_repbeep->setText(Command_value[1]);

   //Fixed Beacon ( No GPS )
   if(Command_value[0] == "SPEED1" ) speed1 = Command_value[1];
   if(Command_value[0] == "SPEED2" ) speed2 = Command_value[1];
   if(Command_value[0] == "SPEED3" ) speed3 = Command_value[1];

   if(Command_value[0] == "SPEED2" ) { ui->combo_speed2->setCurrentText(Command_value[1]); }
   if(Command_value[0] == "SPEED3" ) {
             ui->combo_speed3->setCurrentText(Command_value[1]);
             // If all speed = Fixed Becon is enabled
             if((speed1 == "0") and (speed2 =="0") and (speed3 == "0")){
                 ui->checkBox_beacon->setChecked(true);
                 ui->checkBox_beacon->clicked(true);
                 speed1 = "1";
             }else{
                 ui->checkBox_beacon->setChecked(false);
                 ui->checkBox_beacon->clicked(false);
             }
   }

   if(Command_value[0] == "FIXLAT" ) ui->line_fixlat->setText(Command_value[1]);
   if(Command_value[0] == "FIXLAT" ) Command_value[1].right(1) == "S" ? ui->combo_South->setCurrentText("South") : ui->combo_South->setCurrentText("North");


   if(Command_value[0] == "FIXLON" ) ui->line_fixlon->setText(Command_value[1]);
   if(Command_value[0] == "FIXLON" ) Command_value[1].right(1) == "W" ? ui->combo_West->setCurrentText("West") : ui->combo_West->setCurrentText("East");

   if(Command_value[0] == "DELAY1" ) { ui->combo_fixdelay->setCurrentText(Command_value[1]); }

   if(Command_value[0] == "PHGDP" ) ui->comboPower->setCurrentIndex(Command_value[1].toInt());
   if(Command_value[0] == "PHGDH" ) ui->comboHeight->setCurrentIndex(Command_value[1].toInt());
   if(Command_value[0] == "PHGDG" ) ui->comboGain->setCurrentIndex(Command_value[1].toInt());
   if(Command_value[0] == "PHGDD" ) ui->comboDirection->setCurrentIndex(Command_value[1].toInt());

   this->repaint();

}

void MainWindow::on_bnt_listPorts_clicked()
{
    // Código poara listar portas no combobox
    ui->combo_ports->clear();
    const auto infos = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo &info : infos) {
         ui->combo_ports->addItem(info.portName());
    }
}


void MainWindow::on_bnt_writeData_clicked()
{
     tche_status("Writing.");
    m_serial->flush();
    if (m_serial->isOpen() && m_serial->isWritable())
    {

        //QString CALL = ;
        //QString::to SSID = ui->lineSSID->text();

        m_serial->write("CALL "); m_serial->write(ui->lineCall->text().toUtf8()); m_serial->write("\r");
        m_serial->write("SSID "); m_serial->write(ui->lineSSID->text().toUtf8()); m_serial->write("\r");
        tche_status("Writing..");
        m_serial->write("SYMBOL "); m_serial->write(ui->lineSymbol->text().toUtf8()); m_serial->write("\r");

        if(ui->combo_symbolTable->currentText() == "Normal")
        {
            m_serial->write("SYMBOLTABLE 0\r");
        }else{
            m_serial->write("SYMBOLTABLE 1\r");
        }



        if(ui->combo_path1->currentText() == "WIDE1-1")
        {
            m_serial->write("PATH1 WIDE1\r");
            m_serial->write("PATH1SSID 1\r");
        }else{
            m_serial->write("PATH1 WIDE1\r");
            m_serial->write("PATH1SSID 2\r");
        }
        tche_status("Writing...");

        if(ui->combo_path2->currentText() == "WIDE2-1")
        {
                m_serial->write("PATH2 WIDE2\r");
                m_serial->write("PATH2SSID 1\r");
        }else{
                m_serial->write("PATH2 WIDE2\r");
                m_serial->write("PATH2SSID 2\r");
        }
        tche_status("Writing....");
        m_serial->write("MESSAGE "); m_serial->write(ui->lineMessage->text().toUtf8()); m_serial->write("\r");

        // Radio Settings
        m_serial->write("TXBEEP "); m_serial->write(ui->line_txbeep->text().toUtf8()); m_serial->write("\r");
        m_serial->write("REPBEEP "); m_serial->write(ui->line_repbeep->text().toUtf8()); m_serial->write("\r");
        m_serial->write("RXBEEP "); m_serial->write(ui->line_rxbeep->text().toUtf8()); m_serial->write("\r");



        // Smart course
        if(ui->checkBox_course->isChecked())
        {
            // Para curso ser desabilitado deve ser igual a zero
            m_serial->write("COURSEANGLE 0\r");

        }else{
            m_serial->write("COURSEANGLE "); m_serial->write(ui->combo_cangle->currentText().toUtf8()); m_serial->write("\r");
            m_serial->write("COURSETIME ");  m_serial->write( ui->combo_ctime->currentText().toUtf8()); m_serial->write("\r");
            m_serial->write("COURSESPEED "); m_serial->write(ui->combo_cspeed->currentText().toUtf8()); m_serial->write("\r");
        }
        tche_status("Writing.....");
        //Smart Speed fixed beacon
        if(ui->checkBox_beacon->isChecked())
        {
            // Beacon Mode is enable when all speed are = 0
            m_serial->write("SPEED1 0\r");
            m_serial->write("SPEED2 0\r");
            m_serial->write("SPEED3 0\r");

            // Fixed Coordenates
            if(ui->combo_South->currentText()=="South")
            {
                m_serial->write("FIXLAT ");m_serial->write(ui->line_fixlat->text().toUtf8()); m_serial->write("S\r");
            }else
            {
                m_serial->write("FIXLAT ");m_serial->write(ui->line_fixlat->text().toUtf8()); m_serial->write("N\r");
            }

            if(ui->combo_West->currentText()=="West")
            {
                m_serial->write("FIXLON ");m_serial->write(ui->line_fixlon->text().toUtf8()); m_serial->write("W\r");
            }else
            {
                m_serial->write("FIXLON ");m_serial->write(ui->line_fixlon->text().toUtf8()); m_serial->write("E\r");
            }



            // When in beacon mode the delay betwen packeges is based on delay1
            m_serial->write("DELAY1 "); m_serial->write(ui->combo_fixdelay->currentText().toUtf8()); m_serial->write("\r");

            // POWER
            if(ui->comboPower->currentText()=="0"){
                m_serial->write("PHGDP 0\r");
            }else if(ui->comboPower->currentText()=="1"){
                m_serial->write("PHGDP 1\r");
            }else if(ui->comboPower->currentText()=="4"){
                m_serial->write("PHGDP 2\r");
            }else if(ui->comboPower->currentText()=="9"){
                m_serial->write("PHGDP 3\r");
            }else if(ui->comboPower->currentText()=="16"){
                m_serial->write("PHGDP 4\r");
            }else if(ui->comboPower->currentText()=="25"){
                m_serial->write("PHGDP 5\r");
            }else if(ui->comboPower->currentText()=="36"){
                m_serial->write("PHGDP 6\r");
            }else if(ui->comboPower->currentText()=="49"){
                m_serial->write("PHGDP 7\r");
            }else if(ui->comboPower->currentText()=="64"){
                m_serial->write("PHGDP 8\r");
            }else{
                m_serial->write("PHGDP 9\r");
            }

            //Height
            if(ui->comboHeight->currentText()=="10"){
                m_serial->write("PHGDH 0\r");
            }else if(ui->comboHeight->currentText()=="20"){
                m_serial->write("PHGDH 1\r");
            }else if(ui->comboHeight->currentText()=="40"){
                m_serial->write("PHGDH 2\r");
            }else if(ui->comboHeight->currentText()=="80"){
                m_serial->write("PHGDH 3\r");
            }else if(ui->comboHeight->currentText()=="160"){
                m_serial->write("PHGDH 4\r");
            }else if(ui->comboHeight->currentText()=="320"){
                m_serial->write("PHGDH 5\r");
            }else if(ui->comboHeight->currentText()=="640"){
                m_serial->write("PHGDH 6\r");
            }else if(ui->comboHeight->currentText()=="1280"){
                m_serial->write("PHGDH 7\r");
            }else if(ui->comboHeight->currentText()=="2560"){
                m_serial->write("PHGDG 8\r");
            }else{
                m_serial->write("PHGDP 9\r");
            }

            //Gain
            m_serial->write("PHGDG "); m_serial->write(ui->comboGain->currentText().toUtf8()); m_serial->write("\r");

            //Direction
            if(ui->comboDirection->currentText()=="Omni"){
                m_serial->write("PHGDD 0\r");
            }else if(ui->comboDirection->currentText()=="45 NE"){
                m_serial->write("PHGDD 1\r");
            }else if(ui->comboDirection->currentText()=="90 E"){
                m_serial->write("PHGDD 2\r");
            }else if(ui->comboDirection->currentText()=="135 SE"){
                m_serial->write("PHGDD 3\r");
            }else if(ui->comboDirection->currentText()=="180 S"){
                m_serial->write("PHGDD 4\r");
            }else if(ui->comboDirection->currentText()=="225 SW"){
                m_serial->write("PHGDD 5\r");
            }else if(ui->comboDirection->currentText()=="270 W"){
                m_serial->write("PHGDD 6\r");
            }else if(ui->comboDirection->currentText()=="315 NW"){
                m_serial->write("PHGDD 7\r");
            }else if(ui->comboDirection->currentText()=="360 N"){
                m_serial->write("PHGDD 8\r");
            }else{
                m_serial->write("PHGDD 0\r");
            }

        tche_status("Writing......");
        }else{
            m_serial->write("SPEED1 "); m_serial->write(ui->combo_speed1->currentText().toUtf8()); m_serial->write("\r");
            m_serial->write("SPEED2 "); m_serial->write(ui->combo_speed2->currentText().toUtf8()); m_serial->write("\r");
            m_serial->write("SPEED3 "); m_serial->write(ui->combo_speed3->currentText().toUtf8()); m_serial->write("\r");
            m_serial->write("DELAY1 "); m_serial->write(ui->combo_delay1->currentText().toUtf8()); m_serial->write("\r");
            m_serial->write("DELAY2 "); m_serial->write(ui->combo_delay2->currentText().toUtf8()); m_serial->write("\r");
            m_serial->write("DELAY3 "); m_serial->write(ui->combo_delay3->currentText().toUtf8()); m_serial->write("\r");
            m_serial->write("DELAY4 "); m_serial->write(ui->combo_delay4->currentText().toUtf8()); m_serial->write("\r");
        }
        tche_status("Writing.......");
        m_serial->write("PREAMBLE "); m_serial->write(ui->line_txpreamble->text().toUtf8()); m_serial->write("\r");
        m_serial->write("TAIL ");     m_serial->write(ui->line_txtail->text().toUtf8());     m_serial->write("\r");



    }
    m_serial->flush();
    tche_status("done");
}


void MainWindow::on_bnt_connect_clicked()
{
    tche_status("Connecting.");
    this->repaint();


    //if(ui->combo_ports->currentText() != ""){

        m_serial->setPortName(ui->combo_ports->currentText());
        m_serial->open(QIODevice::ReadWrite);
        m_serial->setBaudRate(QSerialPort::Baud9600);
        m_serial->setDataBits(QSerialPort::Data8);
        m_serial->setParity(QSerialPort::NoParity);
        m_serial->setStopBits(QSerialPort::OneStop);
        m_serial->setFlowControl(QSerialPort::NoFlowControl);



        while(!m_serial->isOpen()) m_serial->open(QIODevice::ReadWrite);

        QByteArray input;
        while (m_serial->waitForReadyRead(3500)) {
                input.append(m_serial->readAll());
        }
        qDebug()<<input;
        if(input == "CFG\r\n"){

            tche_status("done");
            ui->bnt_desconnect->setEnabled(true);
            ui->bnt_format->setEnabled(true);
            ui->bnt_writeData->setEnabled(true);
            ui->bnt_readData->setEnabled(true);
            this->repaint();


        }else{

            QMessageBox msgBox;
            msgBox.setText("Cannot open TcheTracker! Verify if Jumper 11 is in place.");
            msgBox.exec();

            this->on_bnt_desconnect_clicked();


        }






    //}// else{ Alert box port not selected }
}

void MainWindow::on_bnt_desconnect_clicked()
{
    m_serial->close();
    ui->bnt_connect->setEnabled(true);
    ui->bnt_desconnect->setEnabled(false);
    ui->label_status->setVisible(false);
    ui->bnt_format->setEnabled(false);
    ui->bnt_writeData->setEnabled(false);
    ui->bnt_readData->setEnabled(false);
    this->repaint();

}

