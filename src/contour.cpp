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

#include "contour.h"
#include <qprinter.h>
#if QT_VERSION >= 0x040000
#include <qprintdialog.h>
#endif



class MyZoomer: public QwtPlotZoomer
{
public:
    MyZoomer(QwtPlotCanvas *canvas):
        QwtPlotZoomer(canvas)
    {
        setTrackerMode(AlwaysOn);
    }

    virtual QwtText trackerText(const QwtDoublePoint &pos) const
    {
        QColor bg(Qt::white);
#if QT_VERSION >= 0x040300
        bg.setAlpha(200);
#endif

        QwtText text = QwtPlotZoomer::trackerText(pos);
        text.setBackgroundBrush( QBrush( bg ));
        return text;
    }
};

SpectrogramData::SpectrogramData(double **S, bool conMatriz, int filas_in, int columnas_in): QwtRasterData(QwtDoubleRect(-300, -300, 600, 600))	//-ancho/2, -alto/2, ancho*2, alto*2
{
	M=S;
        hayMatriz = conMatriz;

        columnas = columnas_in;
        filas = filas_in;

        qDebug("replotio, %d", hayMatriz);
        if(hayMatriz)
            qDebug("fiiiilas = %d, columnasss = %d, S[50][50]=%lf",filas,columnas,M[50][50]*10000000);

}
	
QwtRasterData * SpectrogramData::copy() const
{
	return new SpectrogramData(M);
}
	
QwtDoubleInterval SpectrogramData::range() const
{
// A class representing an interval.
// The interval is represented by 2 doubles, the lower and the upper limit. 
	return QwtDoubleInterval(0.0, 10.0);
}

void SpectrogramData::setMatriz(double ** S)
{
	M=S;
// qDebug("b= %d, %d",filas, columnas);
       // qDebug("\nM[50][50] = %lf",M[50][50]*10000000);
hayMatriz =1;

}

void SpectrogramData::hayMatriz_f(bool hayMatrizInput)
{
hayMatriz = 1;
}
double SpectrogramData::value(double x, double y) const
{
	const double c = 0.542;

	const double v1 = x * x + (y-c) * (y+c);
	const double v2 = x * (y+c) + x * (y+c);
//		qDebug("x=%lf\n",x);

        if(hayMatriz)
	{
		if((int)x >= 0 && (int)x < filas)	//que se vea todo
		{	if((int)y >= 0 && (int)y < columnas)
			{
				const double v3 = M[(int)x][(int)y];
				return v3;
			}
                        else return 1.05;
		}
                else return 1.05;
			//return v3;
	}
	else
                return 0.1 / (v1 * v1 + v2 * v2);
}



Plot::Plot(double **S,QWidget *parent): QwtPlot(parent)
{
        M = S;
        filas = 301;
        columnas = 301;
        d_spectrogram = new QwtPlotSpectrogram();
	
	QwtLinearColorMap colorMap(Qt::darkCyan, Qt::red);
	colorMap.addColorStop(0.00001, Qt::cyan);
	colorMap.addColorStop(0.00006, Qt::green);
	colorMap.addColorStop(0.000095, Qt::yellow);
	
	plotData = new SpectrogramData(M);
        plotData->setMatriz(M);
        plotData->setSize(filas,columnas);

	d_spectrogram->setColorMap(colorMap);
	d_spectrogram->setData(*plotData);
	d_spectrogram->attach(this);
	

   //     qDebug("filas = %d, columnas = %d, S[50][50]=%lf",filas,columnas,M[50][50]);



	QwtValueList contourLevels;
	for ( double level = 0.5; level < 10.0; level += 1.0 )
		contourLevels += level;
	d_spectrogram->setContourLevels(contourLevels);
	
	// A color bar on the right axis
	QwtScaleWidget *rightAxis = axisWidget(QwtPlot::yRight);
	rightAxis->setTitle("Intensity");
	rightAxis->setColorBarEnabled(true);
	rightAxis->setColorMap(d_spectrogram->data().range(),
		d_spectrogram->colorMap());
	
	setAxisScale(QwtPlot::yRight,
		d_spectrogram->data().range().minValue(),
		d_spectrogram->data().range().maxValue() );
	enableAxis(QwtPlot::yRight);
	
	plotLayout()->setAlignCanvasToScales(true);
	replot();
	
	// LeftButton for the zooming
	// MidButton for the panning
	// RightButton: zoom out by 1
	// Ctrl+RighButton: zoom out to full size
	
	QwtPlotZoomer* zoomer = new MyZoomer(canvas());
	#if QT_VERSION < 0x040000
	zoomer->setMousePattern(QwtEventPattern::MouseSelect2,
		Qt::RightButton, Qt::ControlButton);
	#else
	zoomer->setMousePattern(QwtEventPattern::MouseSelect2,
		Qt::RightButton, Qt::ControlModifier);
	#endif
	zoomer->setMousePattern(QwtEventPattern::MouseSelect3,
		Qt::RightButton);
	
	QwtPlotPanner *panner = new QwtPlotPanner(canvas());
	panner->setAxisEnabled(QwtPlot::yRight, false);
	panner->setMouseButton(Qt::MidButton);
	
	// Avoid jumping when labels with more/less digits
	// appear/disappear when scrolling vertically
	
	const QFontMetrics fm(axisWidget(QwtPlot::yLeft)->font());
	QwtScaleDraw *sd = axisScaleDraw(QwtPlot::yLeft);
	sd->setMinimumExtent( fm.width("100.00") );
	
	const QColor c(Qt::darkBlue);
	zoomer->setRubberBandPen(c);
	zoomer->setTrackerPen(c);
}

void Plot::showContour(bool on)
{
    d_spectrogram->setDisplayMode(QwtPlotSpectrogram::ContourMode, on);
    replot();

}

void Plot::showSpectrogram(bool on)
{
    d_spectrogram->setDisplayMode(QwtPlotSpectrogram::ImageMode, on);
    d_spectrogram->setDefaultContourPen(on ? QPen() : QPen(Qt::NoPen));
    replot();
}

void Plot::setMatriz(double **S)
{
	plotData->setMatriz(S);
        qDebug("\nM[50][50] = %lf",M[50][50]*10000000);
}

void Plot::replotear(double **S,int filasTemp, int columnasTemp)
{
        filas =filasTemp;
        columnas = columnasTemp;
        M=S;

        qDebug("filas = %d, columnuuuuas = %d, S[150][150]=%lf",filas,columnas,M[150][150]*10000000);

        delete plotData;
        QwtLinearColorMap colorMap(Qt::darkCyan, Qt::red);
        colorMap.addColorStop(0.00001, Qt::cyan);
        colorMap.addColorStop(0.00006, Qt::green);
        colorMap.addColorStop(0.000095, Qt::yellow);

        plotData = new SpectrogramData(M,true, filas, columnas);
        plotData->setMatriz(M);
        //setMatriz(M);
        plotData->hayMatriz_f(true);
        plotData->setSize(filas,columnas);

        d_spectrogram->setColorMap(colorMap);
        d_spectrogram->setData(*plotData);
        d_spectrogram->attach(this);
/*
	filas =filasTemp;
	columnas = columnasTemp;
	M=S;

        plotData->setMatriz(M);
        plotData->hayMatriz_f(TRUE);

	plotData->setSize(filas, columnas);
*/
//	d_spectrogram->setColorMap(colorMap);
	
        d_spectrogram->setData(*plotData);
	d_spectrogram->attach(this);
        qDebug("replotio, %d", plotData->hayMatriz);
        qDebug("filas = %d, columnasss = %d, S[50][50]=%lf",filas,columnas,S[50][50]*10000000);
        replot();
}


void Plot::printPlot()
{
    QPrinter printer;
    printer.setOrientation(QPrinter::Landscape);
#if QT_VERSION < 0x040000
    printer.setColorMode(QPrinter::Color);
    printer.setOutputFileName("/tmp/spectrogram.ps");
    if (printer.setup())
#else
    printer.setOutputFileName("/tmp/spectrogram.pdf");
    QPrintDialog dialog(&printer);
    if ( dialog.exec() )
#endif
    {
        print(printer);
    }
}
