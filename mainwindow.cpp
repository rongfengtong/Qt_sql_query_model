#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QMessageBox>


void MainWindow::open_select()
{
    QString str = "SELECT empNo,Name,Gender,Birthday,Province,Department,"
                  " Salary FROM employee ORDER BY empNo";
    qrymodel = new QSqlQueryModel(this);
    qrymodel->setQuery(str);
    if(qrymodel->lastError().isValid()){
        QMessageBox::critical(this,"错误","数据库查询错误/n"+qrymodel->lastError().text());
        return;
    }

    ui->tableView->setModel(qrymodel);
    selmodel = new QItemSelectionModel(qrymodel, this);
    ui->tableView->setSelectionModel(selmodel);

    //处理表头
    QSqlRecord rec = qrymodel->record();
    qrymodel->setHeaderData(rec.indexOf("empNo"), Qt::Horizontal, "工号");
    qrymodel->setHeaderData(rec.indexOf("Name"), Qt::Horizontal, "姓名");
    qrymodel->setHeaderData(rec.indexOf("Gender"), Qt::Horizontal, "性别");
    qrymodel->setHeaderData(rec.indexOf("Birthday"), Qt::Horizontal, "出生日期");
    qrymodel->setHeaderData(rec.indexOf("Province"), Qt::Horizontal, "省份");
    qrymodel->setHeaderData(rec.indexOf("Department"), Qt::Horizontal, "部门");
    qrymodel->setHeaderData(rec.indexOf("Salary"), Qt::Horizontal, "工资");

    //映射
    datamapper = new QDataWidgetMapper(this);
    datamapper->setSubmitPolicy(QDataWidgetMapper::AutoSubmit);
    datamapper->setModel(qrymodel);

    datamapper->addMapping(ui->dbSpinEmpNo, rec.indexOf("empNo"));
    datamapper->addMapping(ui->dbEditName, rec.indexOf("Name"));
    datamapper->addMapping(ui->dbComboSex, rec.indexOf("Gender"));
    datamapper->addMapping(ui->dbEditBirth, rec.indexOf("Birthday"));
    datamapper->addMapping(ui->dbComboProvince, rec.indexOf("Province"));
    datamapper->addMapping(ui->dbComboDep, rec.indexOf("Department"));
    datamapper->addMapping(ui->dbSpinSalary, rec.indexOf("Salary"));
    datamapper->addMapping(ui->dbEditMemo, rec.indexOf("Memo"));
    datamapper->addMapping(ui->dbSpinHeight, rec.indexOf("Height"));
    datamapper->addMapping(ui->dbEditMobile, rec.indexOf("Mobile"));
    datamapper->addMapping(ui->dbComboEdu, rec.indexOf("Education"));
    datamapper->addMapping(ui->dbEditCity, rec.indexOf("City"));

    datamapper->toFirst();
    ui->actOpenDB->setEnabled(false);

    connect(selmodel, &QItemSelectionModel::currentRowChanged, this, &MainWindow::do_current_row_changed);
}

void MainWindow::refresh_tableview()
{
    int index = datamapper->currentIndex();
    QModelIndex cur_index = qrymodel->index(index, 1);
    selmodel->clearSelection();
    selmodel->setCurrentIndex(cur_index, QItemSelectionModel::Select);
}

void MainWindow::do_current_row_changed(const QModelIndex &current, const QModelIndex &previous)
{
    Q_UNUSED(previous);
    if(!current.isValid()){
        ui->dbLabPhoto->clear();
        ui->dbEditMemo->clear();
        return;
    }
    datamapper->setCurrentModelIndex(current);  //修改映射

    //设置按钮
    bool first_row = (current.row() == 0);
    bool last_row = (current.row() == qrymodel->rowCount()-1);
    ui->actRecPrevious->setEnabled(!first_row);
    ui->actRecNext->setEnabled(!last_row);
    ui->actRecFirst->setEnabled(!first_row);
    ui->actRecLast->setEnabled(!last_row);

    //显示照片photo和备注memo (没有放入qrymodel)
    QSqlRecord cur_rec = qrymodel->record(selmodel->currentIndex().row());
    int empNo = cur_rec.value("EmpNo").toInt(); //主键

    QSqlQuery query;
    query.prepare("select Memo,Photo from employee where EmpNo= :ID");
    query.bindValue(":ID", empNo);
    query.exec();
    query.first();      //得到的第一条记录

    QVariant va = query.value("Photo");
    if(!va.isValid())
        ui->dbLabPhoto->clear();
    else{
        QPixmap pic;
        pic.loadFromData(va.toByteArray());
        ui->dbLabPhoto->setPixmap(pic.scaledToWidth(ui->dbLabPhoto->size().width()));
    }

    QVariant va2 = query.value("Memo");
    ui->dbEditMemo->setPlainText(va2.toString());
}


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setCentralWidget(ui->splitter);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_actOpenDB_triggered()
{
    QString file_dir = QFileDialog::getOpenFileName(this, "选择文件", "", "数据库(*.db3)");
    if(file_dir.isEmpty())
        return;
    DB = QSqlDatabase::addDatabase("QSQLITE");  //添加驱动
    DB.setDatabaseName(file_dir);

    if(!DB.open())
        QMessageBox::warning(this, "警告", "数据库未打开");
    else{
        open_select();
    }
}


void MainWindow::on_actRecFirst_triggered()
{
    datamapper->toFirst();
    refresh_tableview();
}


void MainWindow::on_actRecPrevious_triggered()
{
    datamapper->toPrevious();
    refresh_tableview();
}


void MainWindow::on_actRecNext_triggered()
{
    datamapper->toNext();
    refresh_tableview();
}


void MainWindow::on_actRecLast_triggered()
{
    datamapper->toLast();
    refresh_tableview();
}

