#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "logic.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QPainter>

QPixmap *pix = nullptr;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;

    delete pix;

    Request* request = new Request;
    request->operation = Operations::FREE_MEMORY;

    Response* response = execute(request);

    delete request;
    delete response;
}

QString fileName;

void MainWindow::on_fileDialog_bttn_clicked()
{
    fileName = QFileDialog::getOpenFileName(this, "Выберите файл", QDir::currentPath());

    ui->fileName_label->setText(fileName);
}

void MainWindow::on_fileLoad_bttn_clicked()
{
    Request* request = new Request;
    request->operation = Operations::LOAD_DATA;
    request->fileName = fileName.toStdString();

    Response* response = execute(request);

    if (!response->done)
    {
        displayError(response->message);
        return;
    }

    delete request;
    delete response;
}

void MainWindow::on_draw_bttn_clicked()
{
    Request* request = new Request;
    request->operation = Operations::DRAW;

    Response* response = execute(request);

    if (!response->done)
    {
        displayError(response->message);
        return;
    }

    drawSurface(response->lines, response->lineCount);

    delete request;
    delete response;
}

void MainWindow::on_rotate_x_bttn_clicked()
{
    rotationHandler(Axis::X);
}

void MainWindow::on_rotate_y_bttn_clicked()
{
    rotationHandler(Axis::Y);
}

void MainWindow::on_rotate_z_bttn_clicked()
{
    rotationHandler(Axis::Z);
}

void MainWindow::on_rotate_x_back_bttn_clicked()
{
    rotationHandler(Axis::X, -1);
}

void MainWindow::on_rotate_y_back_bttn_clicked()
{
    rotationHandler(Axis::Y, -1);
}

void MainWindow::on_rotate_z_back_bttn_clicked()
{
    rotationHandler(Axis::Z, -1);
}

void MainWindow::on_offset_x_bttn_clicked()
{
    offsetHandler(Axis::X);
}

void MainWindow::on_offset_y_bttn_clicked()
{
    offsetHandler(Axis::Y);
}

void MainWindow::on_offset_z_bttn_clicked()
{
    offsetHandler(Axis::Z);
}

void MainWindow::on_normalization_bttn_clicked()
{
    Request* request = new Request;
    request->operation = Operations::NORMALIZE;

    request->normalization[0] = atof(ui->normalization_a->toPlainText().toStdString().c_str());
    request->normalization[1] = atof(ui->normalization_b->toPlainText().toStdString().c_str());

    Response* response = execute(request);

    if (!response->done)
    {
        displayError(response->message);
        return;
    }

    drawSurface(response->lines, response->lineCount);

    delete request;
    delete response;
}

void MainWindow::drawSurface(Line* lines, int count)
{
    delete pix;
    pix = new QPixmap(450, 450);
    QPainter paint(pix);
    paint.fillRect(0, 0, 450, 450, QBrush(QColor(Qt::GlobalColor::white)));
    paint.setPen(Qt::blue);

    for (int i = 0; i < count; i++)
    {
        drawLine(&paint, &(lines[i]));
    }

    ui->draw_label->setPixmap(*pix);
}

void MainWindow::drawLine(QPainter* paint, Line* line)
{
    paint->drawLine(line->start.x, line->start.y, line->end.x, line->end.y);
}

void MainWindow::displayError(string message)
{
    QMessageBox::about(this, "Ошибка", QString::fromStdString(message));
}

void MainWindow::rotationHandler(Axis axis, int direction)
{
    Request* request = new Request;

    request->operation = Operations::ROTATE;
    request->axis = axis;

    switch (axis)
    {
        case Axis::X:
            request->rotationAngle = atof(ui->rotation_x->toPlainText().toStdString().c_str());;
            break;
        case Axis::Y:
            request->rotationAngle = atof(ui->rotation_y->toPlainText().toStdString().c_str());;
            break;
        case Axis::Z:
            request->rotationAngle = atof(ui->rotation_z->toPlainText().toStdString().c_str());;
            break;
    }

    request->rotationAngle *= direction;

    Response* response = execute(request);

    if (!response->done)
    {
        displayError(response->message);
        return;
    }

    drawSurface(response->lines, response->lineCount);

    delete request;
    delete response;
}

void MainWindow::offsetHandler(Axis axis)
{
    Request* request = new Request;

    request->operation = Operations::OFFSET;
    request->axis = axis;

    switch (axis)
    {
        case Axis::X:
            request->offsetValue = atof(ui->offset_x->toPlainText().toStdString().c_str());;
            break;
        case Axis::Y:
            request->offsetValue = atof(ui->offset_y->toPlainText().toStdString().c_str());;
            break;
        case Axis::Z:
            request->offsetValue = atof(ui->offset_z->toPlainText().toStdString().c_str());;
            break;
    }

    Response* response = execute(request);

    if (!response->done)
    {
        displayError(response->message);
        return;
    }

    drawSurface(response->lines, response->lineCount);

    delete request;
    delete response;
}
