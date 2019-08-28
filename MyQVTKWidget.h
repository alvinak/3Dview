#ifndef MYQVTKWIDGET_H
#define MYQVTKWIDGET_H

#include <QVTKOpenGLWidget.h>
#include "Mesh.h"
#include "MeshOperations.h"

#include <vtkSmartPointer.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>

#include <QVector>

class MyQVTKWidget : public QVTKOpenGLWidget
{
	Q_OBJECT

public:
    MyQVTKWidget(QWidget* parent);
	~MyQVTKWidget();

    void importFile(const QString& fileName);
    void saveToFile(const QString& fileName);
    void setFlatShading();
    void setSmoothShading();
    void setMeshDecimation(double reduction);
    void setMeshSubdivision(int numOfSubdivisions);
    void fillHoles();
    void applyBooleanUnion();

signals:
    void fileLoaded();

private slots:
    void onMeshSelected(const vtkActor* actor);
    void onKeyPressed(const QString& key);

private:
    vtkSmartPointer<vtkRenderer> m_renderer;
    vtkSmartPointer<vtkRenderWindowInteractor> m_renderWindowInteractor;
    QVector<Mesh::MeshPtr> m_meshes;
    Mesh::MeshPtr m_selectedMesh;
};

#endif
