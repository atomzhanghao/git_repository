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

    printf("modbus_writer_registers: ");
    modbus_get_response_timeout(ctx, &old_response_timeout);

    /* Define a new and too short timeout */
    response_timeout.tv_sec = 4;
    response_timeout.tv_usec = 0;
    modbus_set_response_timeout(ctx, &response_timeout);
    modbus_rtu_set_serial_mode(ctx, 0);
    while(1)
    {
	    uint16_t init_meter_tab_registers[2] = {0,0};

	    /* Many registers */
	    rc = modbus_write_registers(ctx, 12,
	                               2, init_meter_tab_registers);
	    printf("modbus_writer_registers: \n");

	    if (rc != -1) {
	        printf("congratulation, init meter sucessfully\n");
	        goto close;
	    }
    }
    printf("\n");
    printf("OK\n");


close:

    /* Close the connection */
    modbus_close(ctx);
    modbus_free(ctx);

    return 0;
}
