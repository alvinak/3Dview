#ifndef MESH_H
#define MESH_H

#include <QString>
#include <vtkSmartPointer.h>
#include <QSharedPointer>
#include <vtkRenderer.h>
#include <vtkPolyData.h>
#include <vtkExtractEdges.h>
#include <vtkActor.h>

enum class ShadingType
{
    Flat, Smooth
};

class Mesh
{
public:
    Mesh();
    Mesh(vtkSmartPointer<vtkPolyData> polyData);
    ~Mesh();

    using MeshPtr = QSharedPointer<Mesh>;

    void importFile(const QString& fileName);
    void addToRenderer(vtkRenderer* renderer);
    void removeFromRenderer(vtkRenderer* renderer);
    void saveToFile(const QString& fileName);
    void setShading(ShadingType type);
    void update();
    void setSelected(bool state);
    void translate(double tx, double ty, double tz);
    
    vtkSmartPointer<vtkPolyData> getPolyData();
    vtkSmartPointer<vtkActor> getActor();

private:
    void createFromPolyData(vtkSmartPointer<vtkPolyData> polyData);

    vtkSmartPointer<vtkPolyData> m_inputPolyData;
    vtkSmartPointer<vtkPolyData> m_featureEdgesData;
    vtkSmartPointer<vtkExtractEdges> m_extractEdges;
    vtkSmartPointer<vtkActor> m_actor;
    vtkSmartPointer<vtkActor> m_edgeActor;
    vtkSmartPointer<vtkActor> m_outlineActor;

    // Not using a const because VTK has a problem with const-correctness
    static double kSelectedColor[];
    static double kDeselectedColor[];
};

#endif

