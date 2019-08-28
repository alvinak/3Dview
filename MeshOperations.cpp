#include "MeshOperations.h"

#include <vtkQuadricDecimation.h>
#include <vtkButterflySubdivisionFilter.h>
#include <vtkFillHolesFilter.h>
#include <vtkPolyDataNormals.h>
#include <vtkBooleanOperationPolyDataFilter.h>
#include <vtkTransform.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkCleanPolyData.h>
#include <vtkTriangleFilter.h>

void MeshOperations::decimation(Mesh::MeshPtr mesh, double reduction)
{
    // Apply QuadricDecimation
    vtkSmartPointer<vtkQuadricDecimation> decimate = vtkSmartPointer<vtkQuadricDecimation>::New();
    auto polyData = mesh->getPolyData();
    decimate->SetInputData(polyData);
    decimate->AttributeErrorMetricOn();
    decimate->SetTargetReduction(reduction);
    decimate->VolumePreservationOn();
    decimate->Update();

    // Update mesh
    polyData->DeepCopy(decimate->GetOutput());

    // Update edge filter and data
    mesh->update();
}

void MeshOperations::subdivision(Mesh::MeshPtr mesh, int numOfSubdivisions)
{
    vtkSmartPointer<vtkButterflySubdivisionFilter>  subdivisionFilter = vtkSmartPointer<vtkButterflySubdivisionFilter>::New();
    auto polyData = mesh->getPolyData();
    subdivisionFilter->SetNumberOfSubdivisions(numOfSubdivisions);
    subdivisionFilter->SetInputData(polyData);
    subdivisionFilter->Update();

    // Update mesh
    polyData->DeepCopy(subdivisionFilter->GetOutput());

    // Update edge filter and data
    mesh->update();
}

void MeshOperations::fillHoles(Mesh::MeshPtr mesh)
{
    vtkSmartPointer<vtkFillHolesFilter> fillHolesFilter = vtkSmartPointer<vtkFillHolesFilter>::New();
    auto polyData = mesh->getPolyData();
    fillHolesFilter->SetInputData(polyData);
    fillHolesFilter->SetHoleSize(100000.0);
    fillHolesFilter->Update();

    // Make the triangle winding order consistent
    vtkSmartPointer<vtkPolyDataNormals> normals = vtkSmartPointer<vtkPolyDataNormals>::New();
    normals->SetInputData(fillHolesFilter->GetOutput());
    normals->ConsistencyOn();
    normals->SplittingOff();
    normals->Update();

    // Update mesh
    polyData->DeepCopy(normals->GetOutput());

    // Update edge filter and data
    mesh->update();
}

Mesh::MeshPtr MeshOperations::booleanUnion(const QVector<Mesh::MeshPtr>& meshes)
{
    // Should have at least two meshes
    if (meshes.size() < 2)
    {
        return nullptr;
    }

    // Boolean operation doesn't take the actor transform into account
    // so we need to physically transform the mesh (i.e. modify vertex positions)
    for (auto mesh : meshes)
    {
        auto matrix = mesh->getActor()->GetMatrix();
        auto transform = vtkSmartPointer<vtkTransform>::New();
        transform->SetMatrix(matrix);

        auto transformFilter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
        transformFilter->SetInputData(mesh->getPolyData());
        transformFilter->SetTransform(transform);
        transformFilter->Update();

        mesh->getPolyData()->DeepCopy(transformFilter->GetOutput());
    }

    auto booleanUnionFunction = [](vtkSmartPointer<vtkPolyData> first,
        vtkSmartPointer<vtkPolyData> second) -> vtkSmartPointer<vtkPolyData>
    {
        auto booleanOperation = vtkSmartPointer<vtkBooleanOperationPolyDataFilter>::New();
        booleanOperation->SetOperationToUnion();
        booleanOperation->AddInputData(0, first);
        booleanOperation->AddInputData(1, second);
        booleanOperation->Update();

        // Clean and triangulate mesh
        vtkSmartPointer<vtkCleanPolyData> cleanPolyData = vtkSmartPointer<vtkCleanPolyData>::New();
        cleanPolyData->SetInputData(booleanOperation->GetOutput());
        cleanPolyData->Update();
        vtkSmartPointer<vtkTriangleFilter> triangles = vtkSmartPointer<vtkTriangleFilter>::New();
        triangles->SetInputData(cleanPolyData->GetOutput());
        triangles->Update();

        return triangles->GetOutput();
    };

    auto booleanResult = meshes[0]->getPolyData();
    for (int i = 1; i < meshes.size(); i++)
    {
        booleanResult = booleanUnionFunction(booleanResult, meshes[i]->getPolyData());
    }

    return Mesh::MeshPtr(new Mesh(booleanResult));
}
