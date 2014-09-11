
//#include <string.h>

#define BUF_LEN 100
#define MAX_FIELDS 30

// Project specific includes

#include "line_parser.h"

// Private variables

    static char line_buf[BUF_LEN];
    static char field_offsets[MAX_FIELDS];

/**
 * @brief This function serves as a parser strings.
 *
 * \b Example:
 *    @code
 *          sprintf(line, "%s", "$GPGGA,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,*47");
 *          num_fields =  line_parser(line, ',');
 *          printf("\nThe string \"%s\" contains %d fields.  The individual fields are: \n", line, num_fields);
 *          for (i = 0; i < num_fields; i++){
 *              num_char_in_field = get_field(i, line);
 *              printf("\tfield %d with length %d = %s\n",i, num_char_in_field, line );
 *          }
 *    @endcode
 * 
 * return
 * 
 *      The string "$GPGGA,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,*47" contains 15 fields.  The individual fields are: 
 *          field 0 with length 6 = $GPGGA
 *          field 1 with length 6 = 123519
 *          field 2 with length 8 = 4807.038
 *          field 3 with length 1 = N
 *          field 4 with length 9 = 01131.000
 *          field 5 with length 1 = E
 *          field 6 with length 1 = 1
 *          field 7 with length 2 = 08
 *          field 8 with length 3 = 0.9
 *          field 9 with length 5 = 545.4
 *          field 10 with length 1 = M
 *          field 11 with length 4 = 46.9
 *          field 12 with length 1 = M
 *          field 13 with length 0 = 
 *          field 14 with length 3 = *47
 *
 * @warning Do not insert spaces after the comma.
 *
 * @Note This function could have been designed using an array of pointers to
 *       hold the location of each field.  This would have made the results
 *       easier to use.  This was not done to keep the array as small as 
 *       possible - an important decision for use with smaller computers such
 *       as Arduino.
 *
 * @param *line a pointer to the input string
 * 
 * @param *line_field_offsets a pointer to an array that will be used to hold the
 *        starting indices of each field in the array
 *
 * @return int containing the number of fields in the string
 *
 * \b Example:
 *    @code
 *
 *      // NMEA GPS test
 *
 *          sprintf(line, "%s", "$GPGGA,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,*47");
 *          num_fields =  line_parser(line, ',');
 *          printf("\nThe string \"%s\" contains %d fields.  The individual fields are: \n", line, num_fields);
 *          for (i = 0; i < num_fields; i++){
 *              num_char_in_field = get_field(i, line);
 *          printf("\tfield %d with length %d = %s\n",i, num_char_in_field, line );
 *          }
 *    @endcode
 * 
 */

uint8_t line_parser(char *line, char delim){

    uint8_t i = 0;
    uint8_t field_ctr = 1;

    field_offsets[0] = 0;                           // assume at least one field starting at position zero

    if(*line == 0x00){
        line_buf[0] = 0x00;
        return 0;
    }

    while(i < BUF_LEN - 1){                         // Save space for null termination
        if(*line == delim){
            line_buf[i] = 0x00;
            field_offsets[field_ctr] = i + 1;       // You are at the delimiter, field start on i
            field_ctr++;
        }
        else{
            line_buf[i] = *line;
        }
        line++;
        i++;
    if(*line == 0x00)
        break;
    }

    line_buf[i] = 0x00;
    return field_ctr;

}



uint8_t get_field(uint8_t N, char *field){

    uint8_t i = field_offsets[N];
    uint8_t cnt = 0;

    while(i < BUF_LEN - 1){                         // Save space for null termination
        *field = line_buf[i];
        if (line_buf[i] == 0x00){
            break;
        }
        cnt++;
        field++;
        i++;
    }
    *field = 0x00;
    return cnt;
}




//uint8_t is_number(char *P){
    uint8_t is_line_field_num(uint8_t N){

    uint8_t result = 0x01;
/*
    while(*P){
        if ((!isdigit(*P)) && (*P != '-') && (*P != '+')){
            result = 0x00;
            break;
        }
       P++;
    }
    */
    return result;
}






/*
char *field_in_line(uint8_t field){
    extern *line;
    extern *line_field_ofsets;
    return line + line_field_offsets[field];
}

*/


