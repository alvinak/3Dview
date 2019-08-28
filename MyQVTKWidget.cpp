#include "MyQVTKWidget.h"
#include "InteractorStyle.h"

#include <vtkRenderWindow.h>
#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkProperty.h>

#include <QMessageBox>

MyQVTKWidget::MyQVTKWidget(QWidget* parent) :
    QVTKOpenGLWidget(parent)
{
    // Setup the window
    m_renderer = vtkSmartPointer<vtkRenderer>::New();
    vtkSmartPointer<vtkGenericOpenGLRenderWindow> window = vtkSmartPointer<vtkGenericOpenGLRenderWindow>::New();
    window->AddRenderer(m_renderer);

    SetRenderWindow(window);

    m_renderWindowInteractor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
    m_renderWindowInteractor->SetRenderWindow(GetRenderWindow());

    m_renderer->SetBackground(.3, .6, .3); // Background color green
    GetRenderWindow()->AddRenderer(m_renderer);

    // Specify a particular interactor style
    auto style = vtkSmartPointer<InteractorStyle>::New();
    style->SetDefaultRenderer(m_renderer);
    m_renderWindowInteractor->SetInteractorStyle(style);

    connect(style, &InteractorStyle::actorSelected, this, &MyQVTKWidget::onMeshSelected);
    connect(style, &InteractorStyle::keyPressed, this, &MyQVTKWidget::onKeyPressed);
}

MyQVTKWidget::~MyQVTKWidget()
{
}

void MyQVTKWidget::importFile(const QString& fileName)
{
    Mesh::MeshPtr mesh = Mesh::MeshPtr(new Mesh);
    mesh->importFile(fileName);

    // Update edge filter and data
    mesh->update();

    mesh->addToRenderer(m_renderer);

    m_meshes.push_back(mesh);

    // Initially let's apply flat shading
    for (auto mesh : m_meshes)
        mesh->setShading(ShadingType::Flat);

    GetRenderWindow()->Render();

    // Emit signal
    emit fileLoaded();
}

void MyQVTKWidget::saveToFile(const QString& fileName)
{
    if (m_selectedMesh)
    {
        m_selectedMesh->saveToFile(fileName);
    }
    else
    {
        QMessageBox mBox;
        mBox.setText("Please select mesh to save..");
        mBox.exec();
    }
}

void MyQVTKWidget::setFlatShading()
{
    for (auto mesh : m_meshes)
        mesh->setShading(ShadingType::Flat);

    GetRenderWindow()->Render();
}

void MyQVTKWidget::setSmoothShading()
{
    for (auto mesh : m_meshes)
        mesh->setShading(ShadingType::Smooth);

    GetRenderWindow()->Render();
}

void MyQVTKWidget::setMeshDecimation(double reduction)
{
    if (m_selectedMesh)
    {
        MeshOperations::decimation(m_selectedMesh, reduction);
        GetRenderWindow()->Render();
    }
}

void MyQVTKWidget::setMeshSubdivision(int numOfSubdivisions)
{
    if (m_selectedMesh)
    {
        MeshOperations::subdivision(m_selectedMesh, numOfSubdivisions);
        GetRenderWindow()->Render();
    }
}

void MyQVTKWidget::fillHoles()
{
    if (m_selectedMesh)
    {
        MeshOperations::fillHoles(m_selectedMesh);
        GetRenderWindow()->Render();
    }
}

void MyQVTKWidget::applyBooleanUnion()
{
    Mesh::MeshPtr mesh = MeshOperations::booleanUnion(m_meshes);

    if (!mesh)
    {
        return;
    }

    for (auto mesh : m_meshes)
        mesh->removeFromRenderer(m_renderer);

    m_meshes.clear();
    
    // Update edge filter and data
    mesh->update();
    mesh->addToRenderer(m_renderer);

    m_meshes.push_back(mesh);

    // Initially let's apply flat shading
    for (auto mesh : m_meshes)
        mesh->setShading(ShadingType::Flat);

    // Select mesh
    m_selectedMesh = m_meshes[0];
    m_selectedMesh->setSelected(true);
    
    GetRenderWindow()->Render();
}

void MyQVTKWidget::onKeyPressed(const QString& key)
{
    if (m_selectedMesh)
    {
        double trans[3] = { 0, 0, 0 };

        if (key == 'x')
        {
            trans[0]--;
        }
        else if (key == 'X')
        {
            trans[0]++;
        }
        else if (key == 'y')
        {
            trans[1]--;
        }
        else if (key == 'Y')
        {
            trans[1]++;
        }
        else if (key == 'z')
        {
            trans[2]--;
        }
        else if (key == 'Z')
        {
            trans[2]++;
        }

        m_selectedMesh->translate(trans[0], trans[1], trans[2]);

        m_renderer->ResetCameraClippingRange();
        GetRenderWindow()->Render();
    }
}

void MyQVTKWidget::onMeshSelected(const vtkActor* actor)
{
    m_selectedMesh = nullptr;

    for (int i = 0; i < m_meshes.size(); i++)
    {
        if (m_meshes[i]->getActor() == actor)
        {
            m_meshes[i]->setSelected(true);
            m_selectedMesh = m_meshes[i];
        }
        else
        {
            m_meshes[i]->setSelected(false);
        }
    }
}
