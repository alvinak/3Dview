#include "Mesh.h"

#include <vtkOBJReader.h>
#include <vtkOBJWriter.h>
#include <vtkCleanPolyData.h>
#include <vtkTriangleFilter.h>
#include <vtkPolyDataNormals.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkOutlineFilter.h>

double Mesh::kSelectedColor[] = { 1.0, 1.0, 0.0 };
double Mesh::kDeselectedColor[] = { 128, 128, 128 };

Mesh::Mesh()
{
}

Mesh::Mesh(vtkSmartPointer<vtkPolyData> polyData)
{
    createFromPolyData(polyData);
}

Mesh::~Mesh()
{
}

void Mesh::importFile(const QString& fileName)
{
    // Load from file
    vtkSmartPointer<vtkOBJReader> reader = vtkSmartPointer<vtkOBJReader>::New();
    reader->SetFileName(fileName.toStdString().c_str());
    reader->Update();

    createFromPolyData(reader->GetOutput());
}

void Mesh::addToRenderer(vtkRenderer* renderer)
{
    // Add the actors to the scene
    renderer->AddActor(m_actor);
    renderer->AddActor(m_edgeActor);
    renderer->AddActor(m_outlineActor);
}

void Mesh::removeFromRenderer(vtkRenderer* renderer)
{
    // Remove actors from the scene
    renderer->RemoveActor(m_actor);
    renderer->RemoveActor(m_edgeActor);
    renderer->RemoveActor(m_outlineActor);
}

void Mesh::saveToFile(const QString& fileName)
{
    vtkSmartPointer<vtkOBJWriter> writer = vtkSmartPointer<vtkOBJWriter>::New();
    writer->SetFileName(fileName.toStdString().c_str());
    writer->SetInputData(m_inputPolyData);
    writer->Write();
}

void Mesh::setShading(ShadingType type)
{
    // Set shading to flat
    if (type == ShadingType::Flat)
    {
        m_actor->GetProperty()->SetInterpolationToFlat();
    }
    
    // Set smooth shading
    else if (type == ShadingType::Smooth)
    {
        m_actor->GetProperty()->SetInterpolationToPhong();
    }

    // Show or hide edge actor
    m_edgeActor->SetVisibility(type == ShadingType::Flat ? true : false);
}

void Mesh::update()
{
    // Update edge filter and data
    m_extractEdges->SetInputData(m_inputPolyData);
    m_extractEdges->Update();
    m_featureEdgesData->DeepCopy(m_extractEdges->GetOutput());
}

void Mesh::setSelected(bool state)
{
    if (state)
    {
        m_outlineActor->GetProperty()->SetColor(kSelectedColor);
    }
    else
    {
        m_outlineActor->GetProperty()->SetColor(kDeselectedColor);
    }
}

void Mesh::translate(double tx, double ty, double tz)
{
    double actorPos[3];
    m_actor->GetPosition(actorPos);

    actorPos[0] += tx;
    actorPos[1] += ty;
    actorPos[2] += tz;

    m_actor->SetPosition(actorPos);
    m_edgeActor->SetPosition(actorPos);
    m_outlineActor->SetPosition(actorPos);
}

vtkSmartPointer<vtkPolyData> Mesh::getPolyData()
{
    return m_inputPolyData;
}

vtkSmartPointer<vtkActor> Mesh::getActor()
{
    return m_actor;
}

void Mesh::createFromPolyData(vtkSmartPointer<vtkPolyData> polyData)
{
    vtkSmartPointer<vtkCleanPolyData> cleanPolyData = vtkSmartPointer<vtkCleanPolyData>::New();
    cleanPolyData->SetInputData(polyData);
    cleanPolyData->Update();

    // Subdivision and decimation filters only work on triangles
    vtkSmartPointer<vtkTriangleFilter> triangles = vtkSmartPointer<vtkTriangleFilter>::New();
    triangles->SetInputData(cleanPolyData->GetOutput());
    triangles->Update();

    vtkNew<vtkPolyDataNormals> normals;
    normals->ComputePointNormalsOn();
    normals->ComputeCellNormalsOn();
    normals->ConsistencyOn();
    normals->SplittingOff();
    normals->SetInputData(triangles->GetOutput());
    normals->Update();

    m_inputPolyData = vtkSmartPointer<vtkPolyData>::New();
    m_inputPolyData->DeepCopy(normals->GetOutput());

    // Initialize edge data, extract edge filter and apply input data for it
    m_featureEdgesData = vtkSmartPointer<vtkPolyData>::New();
    m_extractEdges = vtkSmartPointer<vtkExtractEdges>::New();

    // Map edge data to its actor
    vtkSmartPointer<vtkPolyDataMapper> edgeMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    edgeMapper->SetInputData(m_featureEdgesData);
    m_edgeActor = vtkSmartPointer<vtkActor>::New();
    m_edgeActor->GetProperty()->SetColor(1.0, 0.0, 0.0);
    m_edgeActor->SetMapper(edgeMapper);

    // Visualize
    vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper->SetInputData(m_inputPolyData);
    m_actor = vtkSmartPointer<vtkActor>::New();
    m_actor->SetMapper(mapper);

    // Create the outline
    vtkSmartPointer<vtkOutlineFilter> outline = vtkSmartPointer<vtkOutlineFilter>::New();
    outline->SetInputData(m_inputPolyData);
    vtkSmartPointer<vtkPolyDataMapper> outlineMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    outlineMapper->SetInputConnection(outline->GetOutputPort());
    m_outlineActor = vtkSmartPointer<vtkActor>::New();
    m_outlineActor->SetMapper(outlineMapper);
}
