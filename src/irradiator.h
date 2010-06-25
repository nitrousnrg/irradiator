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

/*aptitude install libqwt5-qt4-dev */

#ifndef IRRADIATOR_H
#define IRRADIATOR_H

#include "calculos.h"

#include <QMainWindow>
#include <QCloseEvent>
#include <QLineEdit>
#include <QRadioButton>
#include "qwt_plot.h"
#include <qwt_plot_curve.h>	//no se si sirve
#include "contour.h"


class QAction;
class QMenu;
class QTextEdit;
class QLineEdit;
class QLabel;
class QRadioButton;

class Irradiator:public QMainWindow
{
	Q_OBJECT

public:
	Irradiator();
	~Irradiator();

protected:
	void closeEvent(QCloseEvent *event);

private slots:
	void newFile();
	void open();
	void addAntenna();
	bool save();
	bool saveAs();
	void newProject();
	void openProject(QString);
	void saveProject();
	void closeProject();
	void setParameters();
	void saveParameters();
	void about();
	void documentWasModified();

private:
	void initializeVariables();
	void createActions();
	void createMenus();
	void createToolBars();
	void createStatusBar();
	void readSettings();
	void writeSettings();
	bool maybeSave();
	void loadFile(const QString &fileName);
	bool saveFile(const QString &fileName);
	void setCurrentFile(const QString &fileName);
	QString strippedName(const QString &fullFileName);

/*engineering stuff here*/
	Plot *plot;

/*******/
	QTextEdit *textEdit;
	QString curFile;
	QString currentProject;
	
	altoCalculamiento *calculador;

	QMenu *fileMenu;
	QMenu *editMenu;
	QMenu *projectMenu;
	QMenu *plotMenu;
	QMenu *helpMenu;
	QToolBar *fileToolBar;
	QToolBar *editToolBar;
	QDialog *paramDialog;
	QDialog *addAntennaDialog;
	QLineEdit *anchoXEdit;
	QLineEdit *largoYEdit;
	QLineEdit *abscisa_xEdit;
	QLineEdit *ordenada_yEdit;
	QLineEdit *alturaEdit;
	QRadioButton *hiRes;
	QRadioButton *normalRes;
	QRadioButton *lowRes;
	QAction *newAct;
	QAction *openAct;
	QAction *addAntennaAct;
	QAction *saveAct;
	QAction *saveAsAct;
	QAction *exitAct;
	QAction *newProjectAct;
	QAction *openProjectAct;
	QAction *saveProjectAct;
	QAction *closeProjectAct;
	QAction *editParamAct;
	QAction *cutAct;
	QAction *copyAct;
	QAction *pasteAct;
	QAction *aboutAct;
	QAction *aboutQtAct;
	QAction *showSpectogramAct;	
	QAction *showContourAct;
	QAction *printPlotAct;

	/* variables globales para los cálculos */
};

#endif
