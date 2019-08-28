#pragma once

#include "ui_MainWindow.h"
#include "MyQVTKWidget.h"

class MainWindow : public QMainWindow, public Ui::MainWindow
{
    Q_OBJECT
public:
    MainWindow();
    ~MainWindow();

public slots:
    void on_actionImport_triggered();
    void on_actionFlatShading_triggered();
    void on_actionSmoothShading_triggered();
    void on_actionMeshDecimation_triggered();
    void on_actionMeshSubdivision_triggered();
    void on_actionFillHoles_triggered();
    void on_actionBooleanUnion_triggered();
    void on_actionSave_triggered();
    
    void enableToolbarButtons(bool enable);

private:
    QActionGroup* mShadingGroup;
};

