#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
#include <QSerialPortInfo>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:

    void on_bnt_format_clicked();
    void on_bnt_listPorts_clicked();
    void on_bnt_readData_clicked();
    void on_bnt_writeData_clicked();
    void on_bnt_connect_clicked();
    void on_bnt_desconnect_clicked();
    void tche_parser(QString);
    void tche_status(QString);

private:
    Ui::MainWindow *ui;
    //Console *m_console = nullptr;
    QSerialPort *m_serial = nullptr;
    QString speed1 = "1";
    QString speed2 = "2";
    QString speed3 = "3";
};

#endif // MAINWINDOW_H
