#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QWidget;
class RenderWindow;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    QWidget *mRenderWindowContainer{nullptr};

private slots:
    void on_actionGrid_toggled(bool arg);

    void on_actionAxis_toggled(bool arg);

    void on_actionCorner_Gizmo_toggled(bool arg);

    void on_actionWireframe_toggled(bool arg1);

    void on_actionDraw_Normals_toggled(bool arg);

    void on_actionScene_1_triggered();

    void on_actionScene_2_triggered();

    void on_actionScene_3_triggered();

    void on_actionStart_Stop_Simulation_triggered();

    void on_actionRestart_Simulation_triggered();

private:
    void init();
    Ui::MainWindow *ui;

    RenderWindow *mRenderWindow{nullptr};
};

#endif // MAINWINDOW_H
