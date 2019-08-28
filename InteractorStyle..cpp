#include "InteractorStyle.h"

#include <vtkObjectFactory.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkCellPicker.h>
#include <vtkSmartPointer.h>

vtkStandardNewMacro(InteractorStyle);

InteractorStyle::InteractorStyle()
{
}


InteractorStyle::~InteractorStyle()
{
}

void InteractorStyle::OnLeftButtonDown()
{
    // Get the location of the click (in window coordinates)
    int* pos = GetInteractor()->GetEventPosition();

    vtkSmartPointer<vtkCellPicker> picker = vtkSmartPointer<vtkCellPicker>::New();
    picker->SetTolerance(0.0005);

    // Pick from this location.
    picker->Pick(pos[0], pos[1], 0, GetDefaultRenderer());

    auto actor = vtkSmartPointer<vtkActor>::New();
    actor = picker->GetActor();
    emit actorSelected(actor);

    // Let the base class handle the event
    vtkInteractorStyleTrackballCamera::OnLeftButtonDown();
}

void InteractorStyle::OnKeyPress()
{
    // Get the keypress
    vtkRenderWindowInteractor *rwi = this->Interactor;
    QString key = rwi->GetKeySym();

    emit keyPressed(key);

    // Let the base class handle the event
    vtkInteractorStyleTrackballCamera::OnKeyPress();
}
