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
#include <qwt_plot.h>
#include <qwt_plot_spectrogram.h>
#include <qwt_color_map.h>
#include <qwt_scale_widget.h>
#include <qwt_scale_draw.h>
#include <qwt_plot_zoomer.h>
#include <qwt_plot_panner.h>
#include <qwt_plot_layout.h>

class SpectrogramData: public QwtRasterData
{
public:
        SpectrogramData(double **S,bool conMatriz=false, int filas_in = 301, int columnas_in = 301);
	virtual QwtRasterData *copy() const;
	virtual QwtDoubleInterval range() const;
	virtual double value(double x, double y) const;
	void setMatriz(double **);
	void hayMatriz_f(bool);
	void setSize(int a,int b){filas = a;columnas=b;}	//La 'ñ' de tamaño no anda
        bool hayMatriz;

private:
	double **M;

	int filas;
	int columnas;
};








class Plot: public QwtPlot
{
    Q_OBJECT

public:
	Plot(double **S,QWidget * = NULL);
	void setMatriz(double **);

public slots:
	void showContour(bool on);
	void showSpectrogram(bool on);
	void printPlot();
	void replotear(double **,int,int);

private:
	QwtPlotSpectrogram *d_spectrogram;
	SpectrogramData *plotData;
	double **M;	//matriz a graficar
	int filas;
	int columnas;
};
