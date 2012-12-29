#include "MainWindow.h"
#include <QApplication>
#include <time.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
	a.setApplicationName("BrawlTool");
	a.setApplicationVersion("1.1");
	qsrand(time(NULL));
    MainWindow w;
    w.show();
    
    return a.exec();
}
