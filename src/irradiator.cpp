/***************************************************************************
 *   Copyright (C) 2008 by Marcos Chaparro, Federico Maidana, Cristian   *
 *   nitrousnrg@gmail.com, fedexlacria@hotmail.com   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/


#include <QtGui>
#include "irradiator.h"

//#include <QTextEdit>
#include <QTextStream>
#include <QCloseEvent>
#include <QFileDialog>

Irradiator::Irradiator()
{
	setWindowTitle("Irradiator");

	//calculador es donde se realizan todos los cálculos, en un thread aparte
	calculador = new altoCalculamiento;
	plot = new Plot(calculador->matriz());		//plot es el objeto que graficará todo

// 	al terminar los cálculos, *calculador* llama a *plot* para que dibuje el resultado
	connect(calculador, SIGNAL(done(double **,int,int)), plot, SLOT(replotear(double**,int,int)));

	setCentralWidget(plot);
	
	initializeVariables();
	createActions();
	createMenus();
	createToolBars();
	createStatusBar();
	
	readSettings();
	
	openProject(currentProject);
	//setCurrentFile("");
}

void Irradiator::closeEvent(QCloseEvent *event)
{
	writeSettings();

/*	if (maybeSave()) {
		writeSettings();
		event->accept();
	} else {
		event->ignore();
	}*/
}

void Irradiator::newFile()
{
	if (maybeSave()) {
		textEdit->clear();
		setCurrentFile("");
	}
}

void Irradiator::open()
{
	
	calculador->run();
	plot->setMatriz(calculador->matriz());
//      if (maybeSave()) {
//             QString fileName = QFileDialog::getOpenFileName(this,tr("Open antenna pattern"),
//                                 "/home/marcos", /*había una pavada que recordaba directorios */
//                                 tr("Text (*.txt)"));
//             if (!fileName.isEmpty())
//             loadFile(fileName);
//       }
}

bool Irradiator::save()
{
      if (curFile.isEmpty()) {
            return saveAs();
      } else {
            return saveFile(curFile);
      }
}

bool Irradiator::saveAs()
{
      QString fileName = QFileDialog::getSaveFileName(this);
      if (fileName.isEmpty())
            return false;

      return saveFile(fileName);
}

void Irradiator::about()
{
      QMessageBox::about(this, tr("About Application"),
            tr("The <b>Application</b> example demonstrates how to "
                  "write modern GUI applications using Qt, with a menu bar, "
                  "toolbars, and a status bar."));
}

void Irradiator::documentWasModified()
{
      setWindowModified(true);
}

void Irradiator::initializeVariables()
{
// 	La inicialización suele ser poniendo todo en cero, o leyendo un archivo de configuración
// 	para setear las variables al estado de la última vez.

// 	ufa. estoy leyendo las variables viejas desde readSettings() y writeSettings() por 'compactez'.
}

void Irradiator::createActions()
{
	newAct = new QAction(QIcon(":/filenew.xpm"), tr("&New"), this);
	newAct->setShortcut(tr("Ctrl+N"));
	newAct->setStatusTip(tr("Create a new file"));
	connect(newAct, SIGNAL(triggered()), this, SLOT(newFile()));
	
	openAct = new QAction(QIcon(":/fileopen.xpm"), tr("&Open..."), this);
	openAct->setShortcut(tr("Ctrl+O"));
	openAct->setStatusTip(tr("Open an existing file"));
	connect(openAct, SIGNAL(triggered()), this, SLOT(open()));
	
	saveAct = new QAction(QIcon(":/filesave.xpm"), tr("&Save"), this);
	saveAct->setShortcut(tr("Ctrl+S"));
	saveAct->setStatusTip(tr("Save the document to disk"));
	connect(saveAct, SIGNAL(triggered()), this, SLOT(save()));
	
	saveAsAct = new QAction(tr("Save &As..."), this);
	saveAsAct->setStatusTip(tr("Save the document under a new name"));
	connect(saveAsAct, SIGNAL(triggered()), this, SLOT(saveAs()));
	
	exitAct = new QAction(tr("E&xit"), this);
	exitAct->setShortcut(tr("Ctrl+Q"));
	exitAct->setStatusTip(tr("Exit the application"));
	connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));

	editParamAct = new QAction(tr("&Parameters"), this);
	connect(editParamAct, SIGNAL(triggered()), this, SLOT(setParameters()));

	addAntennaAct = new QAction(tr("&Add Antenna"), this);
	connect(addAntennaAct, SIGNAL(triggered()), this, SLOT(addAntenna()));
	
	/* Project-related */
	newProjectAct =  new QAction(tr("&New Project"), this);
	connect(newProjectAct, SIGNAL(triggered()), this, SLOT(addAntenna()));
	
	openProjectAct = new QAction(tr("&Open Project"), this);
	connect(openProjectAct, SIGNAL(triggered()), this, SLOT(addAntenna()));

	saveProjectAct = new QAction(tr("&Save Project"), this);
	connect(saveProjectAct, SIGNAL(triggered()), this, SLOT(addAntenna()));

	closeProjectAct = new QAction(tr("&Close Project"), this);
	connect(closeProjectAct, SIGNAL(triggered()), this, SLOT(addAntenna()));


	cutAct = new QAction(QIcon(":/editcut.xpm"), tr("Cu&t"), this);
	cutAct->setShortcut(tr("Ctrl+X"));
	cutAct->setStatusTip(tr("Cut the current selection's contents to the "
				"clipboard"));
//	connect(cutAct, SIGNAL(triggered()), textEdit, SLOT(cut()));
	
	copyAct = new QAction(QIcon(":/editcopy.xpm"), tr("&Copy"), this);
	copyAct->setShortcut(tr("Ctrl+C"));
	copyAct->setStatusTip(tr("Copy the current selection's contents to the "
				"clipboard"));
//	connect(copyAct, SIGNAL(triggered()), textEdit, SLOT(copy()));
	
	pasteAct = new QAction(QIcon(":/editpaste.xpm"), tr("&Paste"), this);
	pasteAct->setShortcut(tr("Ctrl+V"));
	pasteAct->setStatusTip(tr("Paste the clipboard's contents into the current "
				"selection"));
//	connect(pasteAct, SIGNAL(triggered()), textEdit, SLOT(paste()));
	
	aboutAct = new QAction(tr("&About"), this);
	aboutAct->setStatusTip(tr("Show the application's About box"));
	connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));
	
	aboutQtAct = new QAction(tr("About &Qt"), this);
	aboutQtAct->setStatusTip(tr("Show the Qt library's About box"));
	connect(aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
	
	cutAct->setEnabled(false);
	copyAct->setEnabled(false);
//	connect(textEdit, SIGNAL(copyAvailable(bool)), cutAct, SLOT(setEnabled(bool)));
// 	connect(textEdit, SIGNAL(copyAvailable(bool)), copyAct, SLOT(setEnabled(bool)));
	
	showSpectogramAct = new QAction(tr("Spectogram"), this);
	showSpectogramAct->setCheckable(TRUE);
	showSpectogramAct->setChecked(TRUE);
	connect(showSpectogramAct, SIGNAL(toggled(bool)), plot, SLOT(showSpectrogram(bool)));


	showContourAct = new QAction(tr("Contour"), this);
	showContourAct->setCheckable(TRUE);
	connect(showContourAct, SIGNAL(toggled(bool)), plot, SLOT(showContour(bool)));

	printPlotAct = new QAction(tr("Print Plot"), this);

}

void Irradiator::createMenus()
{
	fileMenu = menuBar()->addMenu(tr("&File"));
	fileMenu->addAction(newAct);
	fileMenu->addAction(openAct);
	fileMenu->addAction(addAntennaAct);
	fileMenu->addAction(saveAct);
	fileMenu->addAction(saveAsAct);
	fileMenu->addSeparator();
	fileMenu->addAction(exitAct);
	
	editMenu = menuBar()->addMenu(tr("&Edit"));

	editMenu->addAction(editParamAct);
	editMenu->addAction(cutAct);
	editMenu->addAction(copyAct);
	editMenu->addAction(pasteAct);
	
	projectMenu = menuBar()->addMenu(tr("&Project"));

	projectMenu->addAction(newProjectAct);
	projectMenu->addAction(openProjectAct);
// 	projectMenu->addAction(openRecentProjectAct);
	projectMenu->addAction(saveProjectAct);
	projectMenu->addAction(closeProjectAct);

	menuBar()->addSeparator();

	plotMenu = menuBar()->addMenu(tr("&Plot"));
	plotMenu->addAction(showSpectogramAct);
	plotMenu->addAction(showContourAct);	
	
	helpMenu = menuBar()->addMenu(tr("&Help"));
	helpMenu->addAction(aboutAct);
	helpMenu->addAction(aboutQtAct);
}

void Irradiator::createToolBars()
{
	fileToolBar = addToolBar(tr("File"));
	fileToolBar->addAction(newAct);
	fileToolBar->addAction(openAct);
	fileToolBar->addAction(saveAct);
	fileToolBar->addAction(addAntennaAct);
	
	editToolBar = addToolBar(tr("Edit"));
	editToolBar->addAction(cutAct);
	editToolBar->addAction(copyAct);
	editToolBar->addAction(pasteAct);
	
}

void Irradiator::createStatusBar()
{
      statusBar()->showMessage(tr("Ready"));
}

void Irradiator::readSettings()
{
	QSettings settings("GEBI", "Irradiator");
	QPoint pos = settings.value("pos", QPoint(200, 200)).toPoint();
	QSize size = settings.value("size", QSize(400, 400)).toSize();
	resize(size);
	move(pos);
	
	calculador->dimension_h = settings.value("calculador->dimension_h", 200).toInt();	//200 será el default
	calculador->dimension_v = settings.value("calculador->dimension_v", 200).toInt();
	calculador->numero_d_puntos = settings.value("calculador->numero_d_puntos", 300).toInt();
	calculador->abscisa_x = settings.value("calculador->abscisa_x", 0).toDouble();
	calculador->ordenada_y = settings.value("calculador->ordenada_y", 0).toDouble();
	calculador->altura_del_piso = settings.value("altura_del_piso", 2).toDouble();
	currentProject = settings.value("current project filename", "").toString();

	//qDebug("calculador->numero_d_puntos en irradiator %d",calculador->numero_d_puntos);

}

void Irradiator::writeSettings()
{
	QSettings settings("GEBI", "Irradiator");
	settings.setValue("pos", pos());
	settings.setValue("size", size());
// guardo el valor de los parametros de los calculos para la proxima vez
	settings.setValue("calculador->dimension_h", calculador->dimension_h);
	settings.setValue("calculador->dimension_v", calculador->dimension_v);
	settings.setValue("calculador->numero_d_puntos", calculador->numero_d_puntos);
	settings.setValue("calculador->abscisa_x", calculador->abscisa_x);
	settings.setValue("calculador->ordenada_y", calculador->ordenada_y);
	settings.setValue("altura_del_piso", calculador->altura_del_piso);
	settings.setValue("current project filename", currentProject);
}

bool Irradiator::maybeSave()
{
      if (textEdit->document()->isModified()) {
            int ret = QMessageBox::warning(this, tr("Irradiator"),
                        tr("The document has been modified.\n"
                        "Do you want to save your changes?"),
                        QMessageBox::Yes | QMessageBox::Default,
                        QMessageBox::No,
                        QMessageBox::Cancel | QMessageBox::Escape);
            if (ret == QMessageBox::Yes)
            return save();
            else if (ret == QMessageBox::Cancel)
            return false;
      }
      return true;
}

void Irradiator::loadFile(const QString &fileName)
{
/*	QFile file(fileName);
	if (!file.open(QFile::ReadOnly | QFile::Text)) {
		QMessageBox::warning(this, tr("Irradiator"),
				tr("Cannot read file %1:\n%2.")
				.arg(fileName)
				.arg(file.errorString()));
		return;
	}
	
	QTextStream in(&file);
	QApplication::setOverrideCursor(Qt::WaitCursor);
	textEdit->setPlainText(in.readAll());
	QApplication::restoreOverrideCursor();
	
	setCurrentFile(fileName);
	statusBar()->showMessage(tr("File loaded"), 2000);
*/
}

bool Irradiator::saveFile(const QString &fileName)
{
      QFile file(fileName);
      if (!file.open(QFile::WriteOnly | QFile::Text)) {
            QMessageBox::warning(this, tr("Application"),
                              tr("Cannot write file %1:\n%2.")
                              .arg(fileName)
                              .arg(file.errorString()));
            return false;
      }

      QTextStream out(&file);
      QApplication::setOverrideCursor(Qt::WaitCursor);
      out << textEdit->toPlainText();
      QApplication::restoreOverrideCursor();

      setCurrentFile(fileName);
      statusBar()->showMessage(tr("File saved"), 2000);
      return true;
}

void Irradiator::setCurrentFile(const QString &fileName)	//no se usa
{
	curFile = fileName;
	textEdit->document()->setModified(false);
	setWindowModified(false);
	
}

QString Irradiator::strippedName(const QString &fullFileName)
{
      return QFileInfo(fullFileName).fileName();
}

void Irradiator::addAntenna()
{
/*	addAntennaDialog = new QFileDialog(this);
	addAntennaDialog->setModal(false);	//un paramDialogo modal puede ser notoriamente molesto
	addAntennaDialog->setSizeGripEnabled(false);
	addAntennaDialog->resize(380,360);
	addAntennaDialog->setWindowTitle(tr("Parameters"));


// 	Creo y conecto los botones para aceptar y cancelar
	QDialogButtonBox *accept = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

	connect(accept, SIGNAL(accepted()), this, SLOT(saveParameters()));
	connect(accept, SIGNAL(rejected()), addAntennaDialog, SLOT(reject()));
*/
	
// 	Creo los dialogos

	QString newAntennaFileName = QFileDialog::getOpenFileName(this, tr("Open antenna"), "",  tr("Antenna data (*.txt)"));	
	calculador->addAntenna(newAntennaFileName);
// 	addAntennaDialog->show();

}


void Irradiator::newProject()
{
// 	cierro el anterior (guardar?)
// 	pregunto nombre



}
void Irradiator::openProject(QString lastProject)
{
// 	pregunto nombre
	
	if(lastProject == "")
		currentProject = QFileDialog::getOpenFileName(this, tr("Open project"), "",  tr("Project config (*.pro)"));
	else
		currentProject = lastProject;

	FILE *file;
	if(!(file = fopen(currentProject, "r")))
	{
		qDebug(tr("Error opening project file"));
		return;
	}

// 	calculador->reset();

	QString antennaFileName;
	char fakeString[70];
	int cant_antenas;
	fscanf(file, "%[^:]:%d",fakeString, &cant_antenas);	//leo la cantidad de antenas del proyecto
 	fscanf(file, "%[^:]:\n",fakeString);			//simplemente me salto la info irrelevante


	for(int i = 0; i < cant_antenas; ++i)
	{
	 	fscanf(file, "%[^\n]\n",fakeString);
		qDebug("%s", fakeString);

 		antennaFileName = fakeString;
 		calculador->addAntenna(antennaFileName);
	}
	fclose(file);
}

void Irradiator::saveProject()
{

}

void Irradiator::closeProject()
{

}




void Irradiator::setParameters()
{
	paramDialog = new QDialog(this);
	paramDialog->setModal(false);	//un paramDialogo modal puede ser notoriamente molesto
	paramDialog->setSizeGripEnabled(false);
	paramDialog->resize(380,360);
	paramDialog->setWindowTitle(tr("Parameters"));

	QDialogButtonBox *accept = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

	connect(accept, SIGNAL(accepted()), this, SLOT(saveParameters()));
	connect(accept, SIGNAL(rejected()), paramDialog, SLOT(reject()));

	QTabWidget *tabBar = new QTabWidget();
	QGridLayout *grid0 = new QGridLayout;	//esta es la mas 'externa'
	QGridLayout *grid1 = new QGridLayout;	// y esta esta adentro del primer tab


	grid1->setColumnStretch(0,100);
	grid1->setRowStretch(1,100);
	QLabel *labelAnchoX = new QLabel(tr("Ancho de la region (metros)"));	//calculador->dimension_h
	QLabel *labelLargoY = new QLabel(tr("Largo de la region (metros)"));	//calculador->dimension_v

	QString string;
	string.setNum(calculador->dimension_h);		//hago que el default sea el valor de la variable
	anchoXEdit = new QLineEdit(string);	//variable global, será vista desde cualquier lado
	anchoXEdit->setAlignment(Qt::AlignRight);

	string.setNum(calculador->dimension_v);
	largoYEdit = new QLineEdit(string);	//variable global, será vista desde cualquier lado
	largoYEdit->setAlignment(Qt::AlignRight);

// 	botones para definir la reslocuion de los calculos
	hiRes = new QRadioButton("Alta resolucion", this);	//problemas con acentos..
	normalRes = new QRadioButton("Media resolucion", this);
	lowRes = new QRadioButton("Baja resolucion", this);
	switch(calculador->numero_d_puntos)
	{
		case 300:
		{	lowRes->toggle(); break;}
		case 800:
		{	normalRes->toggle(); break;}
		case 1200:
		{	hiRes->toggle(); break;}
	}

// 	define las coordenadas de la antena

	QLabel *labelAntenaX = new QLabel(tr("Coordenada X del centro (mts)"));	//calculador->abscisa_x
	QLabel *labelAntenaY = new QLabel(tr("Coordenada Y del centro (mts)"));	//calculador->ordenada_y
	QLabel *labelAlturaPiso = new QLabel(tr("Altura (mts)"));	//altura_del_piso

	string.setNum(calculador->abscisa_x);
	abscisa_xEdit = new QLineEdit(string);
	abscisa_xEdit->setAlignment(Qt::AlignRight);

	string.setNum(calculador->ordenada_y);
	ordenada_yEdit = new QLineEdit(string);	
	ordenada_yEdit->setAlignment(Qt::AlignRight);

	string.setNum(calculador->altura_del_piso);
	alturaEdit = new QLineEdit(string);	
	alturaEdit->setAlignment(Qt::AlignRight);

	grid1->addWidget(labelAnchoX,0,0);	
	grid1->addWidget(anchoXEdit,0,1);
	grid1->addWidget(labelLargoY,1,0);
	grid1->addWidget(largoYEdit,1,1);	//estos nombres son mas confusos q la m?erd@
	grid1->addWidget(hiRes,2,0);
	grid1->addWidget(normalRes,3,0);
	grid1->addWidget(lowRes,4,0);
	grid1->addWidget(labelAntenaX,5,0);
	grid1->addWidget(abscisa_xEdit,5,1);
	grid1->addWidget(labelAntenaY,6,0);
	grid1->addWidget(ordenada_yEdit,6,1);
	grid1->addWidget(labelAlturaPiso,7,0);
	grid1->addWidget(alturaEdit,7,1);



	QWidget *param1 = new QWidget();
	param1->setLayout(grid1);

	tabBar->addTab(param1,"Escribir algo coherente aca");

	grid0->addWidget(tabBar,1,0,1,2);
	grid0->addWidget(accept,3,1);

	paramDialog->setLayout(grid0);

	paramDialog->show();
}

void Irradiator::saveParameters()
{
	QString string = anchoXEdit->text();
	calculador->dimension_h = string.toUInt();

	string = largoYEdit->text();
	calculador->dimension_v = string.toUInt();

	if(hiRes->isChecked())	//QRadioButtons! defienen la resolución del calculo.
		calculador->numero_d_puntos = 1200;
	if(normalRes->isChecked())	//QRadioButtons! defienen la resolución del calculo.
		calculador->numero_d_puntos = 800;
	if(lowRes->isChecked())	//QRadioButtons! defienen la resolución del calculo.
		calculador->numero_d_puntos = 300;
	
	string = abscisa_xEdit->text();
	calculador->abscisa_x = string.toDouble();

	string = ordenada_yEdit->text();
	calculador->ordenada_y = string.toDouble();

	string = alturaEdit->text();
	calculador->altura_del_piso = string.toDouble();

	paramDialog->accept();
}

Irradiator::~Irradiator()
{

}

