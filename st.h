#include "nestypes.h"

extern ushort pc_reg;
extern ushort sp_reg;
extern ushort a_reg;
extern ushort x_reg;
extern ushort y_reg;

ushort get_value_reg( byte id ) {
    switch( id ) {
        case PC:
            return pc_reg;
            break;
        case A:
            return a_reg;
            break;
        case X:
            return x_reg;
            break;
        case Y:
            return y_reg;
            break;
        case SP:
            return sp_reg;
            break;
    }
}

void set_value_reg( byte id, ushort value ) {
    switch( id ) {
        case PC:
            pc_reg = value;
            break;
        case A:
            a_reg = value;
            break;
        case X:
            x_reg = value;
            break;
        case Y:
            y_reg = value;
            break;
        case SP:
            sp_reg = value;
            break;
        default:
            printf("Error onn set value reg\n");
    }
}

void print_reg( byte id ) {
    switch( id ) {
        case PC:
            printf("PC : %d\n", pc_reg );
            break;
        case A:
            printf("A  : %d\n", a_reg );
            break;
        case X:
            printf("X : %d\n", x_reg );
            break;
        case Y:
            printf("Y  : %d\n", y_reg );
            break;
        case SP:
            printf("SP : %d\n", sp_reg );
            break;
    }

}

