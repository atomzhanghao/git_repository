/*
 * Copyright © 2008-2010 Stéphane Raimbault <stephane.raimbault@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include "../modbus.h"
#include <time.h>

#include "unit-test.h"

enum {
    TCP,
    TCP_PI,
    RTU
};

int main(int argc, char *argv[])
{
    FILE * fp;
    uint8_t *tab_rp_bits;
    uint16_t *tab_rp_registers;
    //uint16_t *tab_rp_registers_bad;
    modbus_t *ctx;
    int i;
    int nb_points;
    int rc;
    struct timeval old_response_timeout;
    struct timeval response_timeout;
    int use_backend;

    time_t rawtime;
    struct tm * timeinfo;
    char buffer[128];
    

    if (argc > 1) {
        if (strcmp(argv[1], "tcp") == 0) {
            use_backend = TCP;
	} else if (strcmp(argv[1], "tcppi") == 0) {
            use_backend = TCP_PI;
        } else if (strcmp(argv[1], "rtu") == 0) {
            use_backend = RTU;
        } else {
            printf("Usage:\n  %s [tcp|tcppi|rtu] - Modbus client for unit testing\n\n", argv[0]);
            exit(1);
        }
    } else {
        /* By default */
        use_backend = TCP;
    }

    if (use_backend == TCP) {
        ctx = modbus_new_tcp("127.0.0.1", 1502);
    } else if (use_backend == TCP_PI) {
        ctx = modbus_new_tcp_pi("::1", "1502");
    } else {
        ctx = modbus_new_rtu("/dev/ttyUSB0", 4800, 'N', 8, 1);
    }
    if (ctx == NULL) {
        fprintf(stderr, "Unable to allocate libmodbus context\n");
        return -1;
    }
    modbus_set_debug(ctx, TRUE);
    modbus_set_error_recovery(ctx,
                              MODBUS_ERROR_RECOVERY_LINK |
                              MODBUS_ERROR_RECOVERY_PROTOCOL);

    if (use_backend == RTU) {
          modbus_set_slave(ctx, 1);
    }

    if (modbus_connect(ctx) == -1) {
        fprintf(stderr, "Connection failed: %s\n",
                modbus_strerror(errno));
        modbus_free(ctx);
        return -1;
    }

    /* Allocate and initialize the memory to store the bits */
    nb_points = (UT_BITS_NB > UT_INPUT_BITS_NB) ? UT_BITS_NB : UT_INPUT_BITS_NB;
    tab_rp_bits = (uint8_t *) malloc(nb_points * sizeof(uint8_t));
    memset(tab_rp_bits, 0, nb_points * sizeof(uint8_t));

    /* Allocate and initialize the memory to store the registers */
    nb_points = (UT_REGISTERS_NB > UT_INPUT_REGISTERS_NB) ?
        UT_REGISTERS_NB : UT_INPUT_REGISTERS_NB;
    tab_rp_registers = (uint16_t *) malloc(nb_points * sizeof(uint16_t));
    memset(tab_rp_registers, 0, nb_points * sizeof(uint16_t));

    printf("** UNIT TESTING **\n");

    printf("\nTEST WRITE/READ:\n");
	
    modbus_get_response_timeout(ctx, &old_response_timeout);

    /* Define a new and too short timeout */
    response_timeout.tv_sec = 4;
    response_timeout.tv_usec = 0;
    modbus_set_response_timeout(ctx, &response_timeout);
    modbus_rtu_set_serial_mode(ctx, 0);
    fp=fopen("enery_monitor.txt","a+");
    if(!fp)  
	{
    		printf("create file error! %s\n", strerror(errno));
    		return 0;
  	}
    while(1)
    {
	    

	    /* Many registers */
	    rc = modbus_read_registers(ctx, 72,
	                               10, tab_rp_registers);
	    printf("modbus_read_registers: ");
	    time(&rawtime);
    	    timeinfo=localtime(&rawtime);
    	    strftime (buffer,sizeof(buffer),"%Y-%m-%d %H:%M:%S",timeinfo);
	    if (rc != UT_REGISTERS_NB) {
	        printf("FAILED (nb points %d)\n", rc);
	        goto close;
	    }
	    printf("\n");
	    for(i=0;i<rc;i++)
	    {
		printf("<%.2X>",tab_rp_registers[i]);
	    }  
	   uint32_t  sum_kwh = (tab_rp_registers[3] << 16 | tab_rp_registers[4])/32;
	    
	fprintf(fp,"{\"time\":\"%s\",\"voltage\":\"%d\",\"amp\":\"%d\",\"watt\":\"%d\",\"kwh\":\"%d\"}\n",buffer,tab_rp_registers[0],tab_rp_registers[1]/10,tab_rp_registers[2]/10,sum_kwh);

    	fflush(fp);
	sleep(600);
    }
    fclose(fp);
    printf("\n");
    printf("OK\n");


close:
    /* Free the memory */
    free(tab_rp_bits);
    free(tab_rp_registers);

    /* Close the connection */
    modbus_close(ctx);
    modbus_free(ctx);

    return 0;
}
