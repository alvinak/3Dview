#include "MainWindow.h"
#include <QInputDialog>
#include <QFileDialog>

MainWindow::MainWindow()
{
    setupUi(this);
    toolBar->setMovable(false);
    mShadingGroup = new QActionGroup(this);
    mShadingGroup->setEnabled(false);
    mShadingGroup->addAction(actionFlatShading);
    mShadingGroup->addAction(actionSmoothShading);
    actionFlatShading->setChecked(true);
    actionMeshDecimation->setEnabled(false);
    actionMeshSubdivision->setEnabled(false);
    actionFillHoles->setEnabled(false);
    actionBooleanUnion->setEnabled(false);
    actionSave->setEnabled(false);

    connect(qvtkWidget, &MyQVTKWidget::fileLoaded, [this] {enableToolbarButtons(true);});
}

MainWindow::~MainWindow()
{
}

void MainWindow::on_actionImport_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("load OBJ file"), "", tr("OBJ file (*.obj)"));
    if (fileName.isEmpty())
        return;

    qvtkWidget->importFile(fileName);
}

void MainWindow::on_actionFlatShading_triggered()
{
    qvtkWidget->setFlatShading();
}

void MainWindow::on_actionSmoothShading_triggered()
{
    qvtkWidget->setSmoothShading();
}

void MainWindow::on_actionMeshDecimation_triggered()
{
    bool ok = false;
    double targetReduction = QInputDialog::getInt(this, "Decimation", "Target reduction[0..90]", 50, 0, 90, 1, &ok);
    if (ok)
        qvtkWidget->setMeshDecimation(targetReduction / 100);
}

void MainWindow::on_actionMeshSubdivision_triggered()
{
    bool ok = false;
    int numOfSubdivisions = QInputDialog::getInt(this, "Subdivision", "Number of subdivisions[0..4]", 2, 0, 4, 1, &ok);
    if (ok)
        qvtkWidget->setMeshSubdivision(numOfSubdivisions);
}

void MainWindow::on_actionFillHoles_triggered()
{
    qvtkWidget->fillHoles();
}

void MainWindow::on_actionBooleanUnion_triggered()
{
    qvtkWidget->applyBooleanUnion();
}

void MainWindow::on_actionSave_triggered()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save OBJ file"), "", tr("OBJ file (*.obj)"));
    if (fileName.isEmpty())
        return;
    qvtkWidget->saveToFile(fileName);
}

void MainWindow::enableToolbarButtons(bool enable)
{
    actionFlatShading->setChecked(enable);
    actionSave->setEnabled(enable);
    mShadingGroup->setEnabled(enable);
    actionMeshDecimation->setEnabled(enable);
    actionMeshSubdivision->setEnabled(enable);
    actionFillHoles->setEnabled(enable);
    actionBooleanUnion->setEnabled(enable);
}