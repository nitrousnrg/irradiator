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

#include <QThread>
#include <QFile>
#include <QTextStream>
#include "malloc.h"
#include <math.h>
#define PI 3.14159265357


class QThread;		//esto misteriosamente mejora los tiempos de compilación
class QFile;
class QTextStream;

class altoCalculamiento : public QThread
{
	Q_OBJECT

public:
	altoCalculamiento();
	void addAntenna(QString);

	void run();
	double ** matriz() {return S;}
	int dimension_h;
	int dimension_v;
	int numero_d_puntos;
	int saveData(char *fileName);
	double abscisa_x;
	double ordenada_y;
	double altura_del_piso;
	~altoCalculamiento();
	signals:
	void done(double**,int,int);



private:
	void establecerLimites();
	void establecerDefinicion();
	void crearMatriz();
	void liberarMatriz(double **);
	void abrirArchivo(FILE *f);
	void calcular();

	int cantidad_de_antenas;
	QString antennaFileName[80];	//80 = numero de maximo de antenas que podrá ingresarse. Modificar sin miedo ;-)

	double **S;	//matriz de densidad de potencia electromagnetica
	int ordenada_mas;
	int ordenada_menos;
	
	int abscisa_mas;
	int abscisa_menos;

	int cant_filas;
	int cant_columnas;

	int x_antena;
	int y_antena;

	double definicion_eje_x;
	double definicion_eje_y;

	double angulo_antena;
	double potencia_antena;	//potencia en [dB]
	double h;	// altura (altura de antena - altura piso)
	double A[721][4];	//matriz con los valores del txt
	double Gaz,Gvert;

};

