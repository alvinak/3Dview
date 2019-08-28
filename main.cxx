#define vtkRenderingCore_AUTOINIT 3(vtkInteractionStyle,vtkRenderingFreeType,vtkRenderingOpenGL2)
#define vtkRenderingVolume_AUTOINIT 1(vtkRenderingVolumeOpenGL)

#include "MainWindow.h" 

#include <QApplication> 
#include <QSurfaceFormat> 

#include <QVTKOpenGLWidget.h> 


int main(int argc, char* argv[])
{
    // Before initializing QApplication, set the default surface format.
    QSurfaceFormat::setDefaultFormat(QVTKOpenGLWidget::defaultFormat());

    QApplication app(argc, argv);

    MainWindow window;
    window.setWindowTitle("3D View");
    window.show();

    return app.exec();
}