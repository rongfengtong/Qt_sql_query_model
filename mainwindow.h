#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtSql>
#include <QDataWidgetMapper>


QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
    QSqlDatabase DB;
    QSqlQueryModel *qrymodel;
    QItemSelectionModel *selmodel;
    QDataWidgetMapper * datamapper;

    void open_select();
    void refresh_tableview();

private slots:
    void do_current_row_changed(const QModelIndex &current, const QModelIndex &previous);


public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_actOpenDB_triggered();

    void on_actRecFirst_triggered();

    void on_actRecPrevious_triggered();

    void on_actRecNext_triggered();

    void on_actRecLast_triggered();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
