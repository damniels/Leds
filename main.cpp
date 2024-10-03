/* mbed Microcontroller Library
 * Copyright (c) 2019 ARM Limited
 * SPDX-License-Identifier: Apache-2.0
 */
#include "mbed.h"

//Alias
#define DS D4 // Salida de datos
#define STCP D3 // Bajada de datos
#define SHCP D2 // Registro de desplazamiento 

// Declaración de funciones
void shift_data(uint32_t data);// Función para enviar datos al registro de desplazamiento
void load_data(uint32_t data); // Función para cargar datos en el registro de almacenamiento
void clear_reg(void);// Función para limpiar los registros
void send_data(void); // Función llamada desde el hilo

void sc1(void); // Funcion secuencia 1
void sc2(void); //  Funcion secuencia 2
void sc3(void); // Funcion secuencia 3
void sc4(void); // Funcion secuencia 4
void sc5(void); // Funcion secuencia 5
void sc6(void); //  Funcion secuencia 6

// Hilos y pines
Thread T_send_data(osPriorityNormal, 4096, NULL, NULL);

DigitalOut Serial_data(DS);// Controla el pin de salida de datos para enviar bits al registro de desplazamiento
DigitalOut Reg_Clk(SHCP);  // Controla el registro de desplazamiento
DigitalOut Latch_Clk(STCP);// Controla la bajada de los datos 

//Inicializacion de variables y se inician procesos

int main(void)
{
    Serial_data = 0; 
    Reg_Clk = 0;
    Latch_Clk = 0;
    clear_reg();
    T_send_data.start(send_data);

  // Buble infinito que mantiene el programa en ejecucion
    while (true) {
    }
}
 //Funcion shift_data la cual envia un byte de datos al registro de dezplasamiento
void shift_data(uint32_t data)
{
    for(int i = 0; i < 24; i++)
    {
        Serial_data = (data & 0x01) ? 1 : 0;
        Reg_Clk = 0;
        wait_us(1);
        Reg_Clk = 1;
        wait_us(1);
        data = data >> 1;
    }
}
//Funicon load_data la cual envia un byte al registro de desplazamiento y luego trasfiere eso a la salida
void load_data(uint32_t data)
{
    Latch_Clk = 0;
    shift_data(data);
    Latch_Clk = 1;
}
 //Funcion clear reg la cual limpia el registro de desplazamiento poniendo todo en 0
void clear_reg(void)
{
    uint32_t data = 0x000000;
    load_data(data);
}

void send_data(void)
{
    while(true)
    {
        sc1();
        sc2();
        sc3();
        sc4();
        sc5();
        sc6();
    }
}

// Función para la primera secuencia
void sc1(void)
{
    
    uint32_t data = 0xAAAAAA; // Ejemplo de patrón alternado
    for(int i = 0; i < 10; i++) // Repetir 10 veces
    {
        load_data(data);
        data = ~data; // Invierte los bits de xAAAAAA
        load_data(data); // Llama a load_data para cargar los datos invertidos en el registro de desplazamiento.
        ThisThread::sleep_for(200ms); // Hace que el hilo se detenga por 200 milis segundo antes de repetir el proceso.

}
}

// Función para la segunda secuencia
void sc2(void)
{
    uint32_t data = 0x000001; // Iniciar con el primer bit encendido
    for (int i = 0; i < 24; i++) // Recorre 24 posiciones para los 24 LEDs
    {
        load_data(data); // Envía el dato completo de 24 bits

        ThisThread::sleep_for(200ms); // Espera 200ms antes de encender el siguiente LED

        data = data << 1; // Desplaza el bit encendido una posición a la izquierda
    }

   
}



// Función para la tercera secuencia
void sc3(void)
{
    
    uint32_t data = 0xFFFFFF; // Ejemplo de patrón alternado inverso
    for(int i = 0; i < 10; i++) // Repetir 10 veces
    {
        load_data(data);
        data = ~data; // Invierte los bits 0xFFFFFF
        load_data(data);
        ThisThread::sleep_for(200ms);
    }

}



// Función para la cuarta secuencia
void sc4(void)
{
    uint32_t data = 0x800000; // Iniciar con el primer bit encendido
    for (int i = 24; i > 0; i--) // Recorre 24 posiciones para los 24 LEDs
    {
        load_data(data); // Envía el dato completo de 24 bits

        ThisThread::sleep_for(200ms); // Espera 200ms antes de encender el siguiente LED

        data = data >> 1; // Desplaza el bit encendido una posición a la derecha
    }


}

void sc5(void)
{
    uint32_t data = 0xFFFFFF; //Inicia con todos los bits encendidos
    for (int i = 24; i > 0; i--) // Recorre 24 posiciones para los 24 LEDs
    {
        load_data(data); // Envía el dato completo de 24 bits

        ThisThread::sleep_for(200ms); // Espera 500ms antes de encender el siguiente LED

        data = data >> 1; // Desplaza el bit encendido una posición a la izquierda
    }

    data = 0xFFFFFF; // Reinicia para empezar de nuevo con el primer LED encendido

}

void sc6(void)
{
uint32_t data = 0x000000; // Inicialmente, todos los LEDs están apagados

    for (int i = 0; i < 24; i++) // Recorre 24 posiciones para los 24 LEDs
    {
        data |= (0x000001 << i); // Realiza Or bit a bit  Enciende el LED actual y deja los anteriores encendidos
        
        load_data(data); // Carga el dato completo al registro de desplazamiento

        ThisThread::sleep_for(200ms); // Espera 200ms para visualizar el LED encendido
    }
}