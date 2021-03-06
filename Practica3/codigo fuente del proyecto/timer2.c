/*********************************************************************************************
* Fichero:		timer.c
* Autor:
* Descrip:		funciones de control del timer0 del s3c44b0x
* Version:
*********************************************************************************************/

/*--- ficheros de cabecera ---*/
#include "timer2.h"
#include "44b.h"

volatile static int timer2_num_int;

/**
 * declaración de función que es rutina de servicio de interrupción
 * https://gcc.gnu.org/onlinedocs/gcc/ARM-Function-Attributes.html
 * */
void timer2_ISR(void) __attribute__((interrupt("FIQ")));

void timer2_ISR(void) {
    // Hacer lo que sea
    timer2_num_int = timer2_num_int + 1;

    /* borrar bit en I_ISPC para desactivar la solicitud de interrupci�n*/
    rI_ISPC |= BIT_TIMER2; // BIT_TIMER2 est� definido en 44b.h y pone un uno en el bit 13 que correponde al Timer2
}

/**
 * Pre: ---
 * Post: Inicializa el timer 2
 * */
void timer2_inicializar(void) {
    /* Establece la rutina de servicio para TIMER2 */
    pISR_FIQ = (unsigned) timer2_ISR;

    /* Configura el Timer2 */
    rTCFG0 = rTCFG0 & ~(0xff00); // factor de preescalado minimo, para aumentar la precisión
    rTCFG1 = rTCFG1 &
             ~(0xf00); // selecciona la entrada del mux que proporciona el reloj. La 00 corresponde a un divisor de 1/2.
    rTCMPB2 = 0; // valor de comparaci�n
    rTCNTB2 = 65535; // valor inicial de cuenta (la cuenta es descendente)
    rINTMSK = rINTMSK & ~(BIT_GLOBAL | BIT_TIMER2); // Emascara todas las lineas excepto Timer2 y el bit global
}

/**
 * Pre: El timer 2 ha de estar inicializado
 * Post: Comienza la cuenta del timer 2
 * */
void timer2_empezar(void) {
    rTCON = (rTCON & ~(0xf000)) | 0x2000;
    /* iniciar timer (bit 0) con auto-reload (bit 3)*/
    rTCON = (rTCON & ~(0xf000)) | 0x9000;
    timer2_num_int = 0;
}

/**
 * Pre: Se ha de haber comenzado una cuenta con el timer 2
 * Post: Devuelve la cuenta del timer 2
 * */
int timer2_leer(void) {
    int timer_actual = (timer2_num_int * 65535) + (65535 - rTCNTO2);
    /* Si durante la operación ha cambiado timer2_num_int, implica que se ha terminado
       la cuenta y que se ha activado la subrutina */
    if (timer2_num_int * 65535 > timer_actual) return (timer2_num_int * 65535) / 32;
    else return timer_actual / 32;
}
