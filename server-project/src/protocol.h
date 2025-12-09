/*
 * protocol.h   SERVER
 *
 * Shared header file for UDP client and server
 * Contains protocol definitions, data structures, constants and function prototypes
 */

#ifndef PROTOCOL_H_
#define PROTOCOL_H_

// Shared application parameters
#define SERVER_PORT 56700  // Server port (change if needed)
#define BUFFER_SIZE 512    // Buffer size for messages
#define STATUS_SUCCESS 0
#define STATUS_CITY_NOT_AVAILABLE 1
#define STATUS_REQUEST_NOT_VALID 2


typedef struct {
 char type;        // Weather data type: 't', 'h', 'w', 'p'
 char city[64];    // City name (null-terminated string)
} weather_request_t;

typedef struct {
 unsigned int status;  // Response status code
 char type;            // Echo of request type
 float value;          // Weather data value
} weather_response_t;

// Function prototypes
void clearwinsock();

void errorhandler(char *errorMessage);

int valid_type(const char c);

int supported_city(const char *city);

float get_temperature(); //temperatura citta'

float get_humidity(); //umidita' citta'

float get_wind(); //velocita' vento citta'

float get_pressure(); //pressione aria citta'

#endif /* PROTOCOL_H_ */
