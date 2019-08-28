#ifndef MOUSEINTERACTORSTYLE_H
#define MOUSEINTERACTORSTYLE_H

#include <QObject>

#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkActor.h>

class InteractorStyle :public QObject, public vtkInteractorStyleTrackballCamera
{
    Q_OBJECT
public:
    InteractorStyle();
    ~InteractorStyle();

    static InteractorStyle* New();
    vtkTypeMacro(InteractorStyle, vtkInteractorStyleTrackballCamera);

    virtual void OnLeftButtonDown() override;
    virtual void OnKeyPress() override;

signals:
    void actorSelected(const vtkActor* actor);
    void keyPressed(const QString& key);
};

#endif

