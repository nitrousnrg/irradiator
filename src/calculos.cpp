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
#include "calculos.h"

#define FILA_X_ANT 1
#define FILA_Y_ANT 2
#define FILA_ANGULO_ANT 3
#define FILA_ALTURA_ANT 4
#define FILA_GANANCIA_ANT 5
#define FILA_POTENCIA_ANT 6
#define COLUMNA_DATOS 1
#define COLUMNA_GAZ 2
#define COLUMNA_GVERT 3
#define COLUMNA_GRADOS 4

altoCalculamiento::altoCalculamiento()
{
Gaz = 0;
Gvert = 0;

cantidad_de_antenas = 0;
/*antennaFileName[0] = "antena_numero1.txt";
antennaFileName[1] = "antena_numero2.txt";
antennaFileName[2] = "antena_numero3.txt";
antennaFileName[3] = "antena_numero4.txt";*/
}

void altoCalculamiento::run()
{
/*	getFieldSize();
	getGraphDefinition();
	getCoordinates();*/
	qDebug("Working...");
	establecerLimites();
	establecerDefinicion();
	crearMatriz();

	FILE *f;

	for(int i=0; i<cantidad_de_antenas; ++i)
	{
		if(!(f = fopen(antennaFileName[i], "r")))
		{	qDebug("no puedo abrirlo che ");
			return;
		}
	
		abrirArchivo(f);
		calcular();
	}



        emit done(S, cant_filas, cant_columnas);
	saveData("out.dat");

        //liberarMatriz(S);	//nooooo!! lo hace el destructor
	return;	// salir de run() es como salir de main(), el thread se cierra.

//	exec();		//exec permite continuar el loop del thread, para usar signals
}

void altoCalculamiento::establecerLimites()
{
    //establezco los límites tanto para x como para y del gráfico para fijar
    //las dimensiones de la matriz

// 	ordenada_y: posicion de la antena
// 	dimension_v: tamaño de la superficie /2.
// 	el mapa irá de - dimension a +dimension
    ordenada_mas   = ordenada_y + dimension_v;
    ordenada_menos = ordenada_y - dimension_v;

    abscisa_mas   = abscisa_x + dimension_h;
    abscisa_menos = abscisa_x - dimension_h;
}

void altoCalculamiento::establecerDefinicion()
{
   //calculamos la definición óptima para no realizar excesiva cantidad de
   //cálculos. cantidad max 1000.
    definicion_eje_x = ((double)(abscisa_mas - abscisa_menos))/numero_d_puntos; //defini en metros
    definicion_eje_y = ((double)(ordenada_mas - ordenada_menos))/numero_d_puntos; //defini en metros

}

void altoCalculamiento::crearMatriz()
{
	//Calculo la cantidad de filas y columnas que tengo que cargarle a la 
	//matriz densidad de potencia electromagnetica.

// 	qDebug("abscisa_mas = %d\nabscisa_menos = %d",abscisa_mas, abscisa_menos);
// 	qDebug("ordenada_mas = %d\nordenada_menos = %d",ordenada_mas, ordenada_menos);
// 	qDebug("numero_d_puntos = %d",numero_d_puntos);

	cant_filas   = ((double)(abscisa_mas  -  abscisa_menos)) / definicion_eje_x +1; 
	cant_columnas = ((double)(ordenada_mas - ordenada_menos)) / definicion_eje_y +1;

    //Matriz densidad de potencia electromagnetica eqivalente a la 
    //superficie en metros cuadrados correspondiente a la ciudad de B.Blanca.
    //La cargo con todos ceros. 

    //S = zeros(cant_filas,cant_columnas);

	//reservo memoria dinámicamente
	S = (double**) malloc(cant_filas* sizeof(double*));
	for(int i = 0,j; i< cant_filas;++i)
	{
		S[i] = (double*) malloc(cant_columnas* sizeof(double));

		for(j=0;j < cant_columnas;++j)	//las voy inicialiando en cero
			S[i][j] = 0;
	}
}

void altoCalculamiento::liberarMatriz(double** M)
{
	for(int i = 0; i<cant_filas;++i)
		free(M[i]);
	free(M);
}

void altoCalculamiento::abrirArchivo(FILE *f)
{
	
	//abro archivo, leo toodos los datos, cargandolos a las respectivas variables
	 //y al array Gaz[grados/2] y Gvert[grados/2], y lo cierro 

	//datos: ubicacion espacial de la antena (x,y,z), ganancia azimutal y vertical para cada angulo 

	for(int i = 0 ; i<720 ; ++i)	//cargo la matriz A con los datos del txt
		fscanf(f,"  %lf\t\t%lf\t\t%lf\t\t%lf",&A[i][0],&A[i][1],&A[i][2],&A[i][3]);
	
	fclose(f);	
		
	potencia_antena=A[FILA_POTENCIA_ANT-1][COLUMNA_DATOS-1]/*[5][0]*/+A[FILA_GANANCIA_ANT-1][COLUMNA_DATOS-1]/*[4][0]*/; 
	h = A[FILA_ALTURA_ANT-1][COLUMNA_DATOS-1]/*[3][0]*/ - altura_del_piso;	
	
	//posicion x,y de la antena
	x_antena = A[FILA_X_ANT-1][COLUMNA_DATOS-1]/*[0][0]*/;
	y_antena = A[FILA_Y_ANT-1][COLUMNA_DATOS-1]/*[1][0]*/;

	//ángulo de la antena (0° sería en la dirección del eje X(+))
	angulo_antena = A[FILA_ANGULO_ANT-1][COLUMNA_DATOS-1];
// 	qDebug("angulo antena= %lf", angulo_antena);
}

void altoCalculamiento::calcular()
{

	int x_punto,y_punto;	//coordenadas que irán cubriendo todo el plano a calcular
	int y_ant_punto,x_ant_punto;	
	int posicion_ant,posicion_sig;

	double dist_antena_punto,hipot_alt_p,alt_engrados;
	double grados, angulo_final;
	int fila, columna;

 	for(fila = 0, x_punto = abscisa_menos ; fila < cant_filas ; x_punto += definicion_eje_x, ++fila)
	{
 		for(columna = 0, y_punto = ordenada_menos ; columna < cant_columnas; y_punto += definicion_eje_y, ++columna)
		{
			y_ant_punto  =  y_punto - y_antena;	//dist relativa entre antena y punto a calcular
			x_ant_punto  =  x_punto - x_antena;

			//distancia entre la antena y el punto:
                        dist_antena_punto = (double)sqrtl((long double)(powl(x_ant_punto,2) + powl(y_ant_punto,2)));

			//calculo el ángulo y de paso las ganancias
			if(dist_antena_punto == 0)	//el punto esta en la antena
				{
				Gaz = A[0][COLUMNA_GAZ-1];
     				Gvert = A[180][COLUMNA_GVERT-1];
				}
 			else
				{
				grados = atan2(y_ant_punto, -x_ant_punto)*180/PI;	//esta devuelve el cuadrante correcto

				//ubico angulo_final con respecto al angulo de la antena
				angulo_final = grados - angulo_antena +270;
				

				while(angulo_final < 0)
					angulo_final += 360;
				while(angulo_final >= 360)
					angulo_final -= 360;
         /*                       if((columna == 50) && (fila==50))
				{
					qDebug("angulo_final = %lf", angulo_final);
					qDebug("y_ant_punto = %d", y_ant_punto);
					qDebug("y_punto = %d", y_punto);
					qDebug("y_antena = %d", y_antena);
					qDebug("x_ant_punto = %d", x_ant_punto);
					qDebug("x_punto = %d", x_punto);
					qDebug("x_antena = %d", x_antena);
				}
        */                                //suavización
				posicion_sig = ceil (angulo_final * 2 + 1 );
				posicion_ant = floor (angulo_final * 2 + 1 );
	
	
		/*		if (posicion_sig != posicion_ant)
				{
					//interpolación con una recta: y= y0 + (y1-y0)*(x-x0)/(x1-x0)
					Gaz = (A[posicion_sig-1][2-1]-A[posicion_ant-1][2-1])*(angulo_final-A[posicion_ant-1][4-1]); //(y1-y0)*(x-x0)
					Gaz /= A[posicion_sig-1][4-1]-A[posicion_ant-1][4-1];  //  /(x1-x0)
					Gaz += A[posicion_ant-1][2-1]; //+y0
				}
				else*/
					Gaz = A[posicion_ant-1][COLUMNA_GAZ-1];
	
					//Objetivo: obtener del array la ganancia vertical correspondiente al punto
	
					//angulo theta en radianes
				hipot_alt_p = sqrt(powl(dist_antena_punto,2) + powl(h,2));
				alt_engrados =  asin ( h / hipot_alt_p )*180/PI;
	
	
	
					//otra suavización
				posicion_sig = ceil (alt_engrados * 2 + 1 );
				posicion_ant = floor (alt_engrados * 2 + 1 );
	
	
				if (posicion_sig != posicion_ant)
				{
					//interpolación con una recta: y= y0 + (y1-y0)*(x-x0)/(x1-x0)
					Gvert = (A[posicion_sig-1][COLUMNA_GVERT-1]-A[posicion_ant-1][COLUMNA_GVERT-1]) * (alt_engrados-A[posicion_ant-1][4-1]); //(y1-y0)*(x-x0)
					Gvert /= (A[posicion_sig-1][4-1]-A[posicion_ant-1][4-1]);  //  /(x1-x0)
					Gvert += A[posicion_ant-1][COLUMNA_GVERT-1]; //+y0
				}
				else
					Gvert = A[posicion_ant-1][COLUMNA_GVERT-1];


					//Calculo el numerador de la formula de la densidad de potencia
 				double numerador = powl(10,((potencia_antena + Gaz + Gvert)/10));

         /*                       if((columna == 50) && (fila==50))
				{                                    
                                        qDebug("A[posicion_sig-1][COLUMNA_GVERT-1] = %lf", A[posicion_sig-1][COLUMNA_GVERT-1]);
					qDebug("A[posicion_ant-1][COLUMNA_GVERT-1] = %lf", A[posicion_ant-1][COLUMNA_GVERT-1]);

                                        qDebug("hipot_alt_p = %lf", hipot_alt_p);
                                        qDebug("alt_engrados = %lf", alt_engrados);	//16.999
                                        qDebug("A[posicion_ant-1][4-1] = %lf", A[posicion_ant-1][4-1]);

					qDebug("A[posicion_sig-1][4-1] = %lf", A[posicion_sig-1][4-1]);
					qDebug("A[posicion_ant-1][4-1] = %lf", A[posicion_ant-1][4-1]);

					qDebug("A[posicion_ant-1][COLUMNA_GVERT-1] = %lf", A[posicion_ant][COLUMNA_GVERT-1]);

					qDebug("numerador = %lf", numerador);	//16.999
					qDebug("potencia_antena = %lf", potencia_antena);	//16.999
					qDebug("Gaz = %lf", Gaz);
					qDebug("Gvert = %lf", Gvert);
					qDebug("dist_antena_punto = %lf", dist_antena_punto);
					qDebug("h = %lf", h);
				}
*/




  					//Calculo el denominador de la formula de la densidad de potencia
				double denominador = 4 * PI * (powl(h,2) + powl(dist_antena_punto,2));
				if (denominador == 0)
				{qDebug("División por cero!"); break;}
				else
					//cargo los valores a la matriz. sólo queda graficarla.
					S[columna][fila] += (numerador / (denominador * 10));

			}
		}
	}


// qDebug("\nx_antena = %d y_antena = %d",x_antena, y_antena);

}

int altoCalculamiento::saveData(char *fileName)
{
//	QFile file(fileName);
//	if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
//		return 1;
	FILE *file;

	int fila, columna;
	double x_punto, y_punto;

	x_punto = abscisa_menos;
	y_punto = ordenada_menos;

	if(!(file = fopen("graf_x.dat","w")))
		return 1;

	for(fila = 0;fila < cant_filas;++fila)
	{
		fprintf(file, " %le", y_punto);	
		y_punto+=definicion_eje_y;
	}

	fclose(file);

	if(!(file = fopen("graf_y.dat","w")))
		return 1;

	for(columna = 0;columna < cant_columnas;++columna)
	{
		fprintf(file, " %le", x_punto);	
		x_punto+=definicion_eje_x;
	}

	fclose(file);

	if(!(file = fopen("S.dat","w")))
		return 1;

	
	for(fila = 0;fila < cant_filas;++fila)
	{
		for(columna = 0;columna < cant_columnas;++columna)
		{
// 			if(columna)
// 				fprintf(file, " ");	
// 			else
// 			{
// 			}
			fprintf(file, " %1.8e", S[columna][fila]);	
		}
		fprintf(file, "\n");	

	}

	fclose(file);

               qDebug("filas = %d, columnas = %d, S[50][50]=%lf",fila,columna,S[50][50]*10000000);

qDebug("saved");

	return 0;
}

altoCalculamiento::~altoCalculamiento()
{
	liberarMatriz(S);
}


void altoCalculamiento::addAntenna(QString AntennaFileName)
{
	antennaFileName[cantidad_de_antenas] = AntennaFileName;
	++cantidad_de_antenas;

}






/*
double ** altoCalculamiento::matriz()
{
	return S;
}
*/
