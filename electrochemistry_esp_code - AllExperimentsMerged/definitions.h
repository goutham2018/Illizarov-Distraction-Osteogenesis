#include <SPI.h>
#include <WiFi.h>
#include <WiFiMulti.h>

//-----------------PIN DEFINITIONS-----------------//
#define MO   22
#define MI   23
#define MCLK 19
#define MS   18

#define SO   (gpio_num_t)21
#define SI   (gpio_num_t)19
#define SCLK (gpio_num_t)18
#define SS   (gpio_num_t)5

//-------------------------------------------------//

SPISettings spi_setting(80000000, MSBFIRST, SPI_MODE);
SPIClass master(HSPI);      // HSPI
SlaveSPI slave(VSPI_HOST);  // VSPI_HOST

//-------------------------------------------------//

WiFiMulti wifiMulti;
//how many clients should be able to telnet to this ESP32
#define MAX_SRV_CLIENTS 1
const char* ssid = "SECP";
const char* password = "123456789";
WiFiServer server(8080);
WiFiClient serverClients[MAX_SRV_CLIENTS];
uint8_t i = 0, k = 0;
bool slave_config = 0;

// ------------------------------------------------//

#include "SimpleArray.h"
typedef SimpleArray<uint8_t, int> array_t;

array_t master_msg(SPI_DEFAULT_MAX_BUFFER_SIZE);
array_t slave_msg(SPI_DEFAULT_MAX_BUFFER_SIZE);
uint8_t slave_msg1[] = {'S', 'T', 'A', 'R', 'T'};
uint8_t byte_data_send_1[64] = {0};
uint8_t byte_data_send_2[64] = {0};
uint8_t array2[8000];
extern uint8_t array1[64];

#define SOH 0x01
#define SOH2 0x36
#define STX 0x02
#define ETX 0x03
#define CRC 0x00
#define CR  0x0D
#define DEVICEID 0xAB
#define ACK 0x06
#define NACK 0x15
uint8_t start = 0;
uint8_t byte_data[1000] = {0}, dataReady = 0, experiment_no = 0x01;
uint8_t byte_data_send[1000] = {0};
uint8_t param1_int[256]={0};
uint8_t square_data[480] = {0};
uint16_t sine_index = 0;
uint16_t local_sine_index = 0;
uint8_t local_packet_id = 0;
uint32_t data_index = 0;
uint32_t clk_time = 0;
uint8_t number_of_steps;
uint8_t number_of_cycles;
uint8_t sample_rate;
uint8_t OCP_measurement_mode;
uint8_t OCP_measurement_period;
uint8_t post_exp_cell_state;
float OCP_measurement_slope;
uint8_t WE_number;
float initial_potential;
uint8_t VSOCP;
float initial_potential_time;
uint8_t current_range;
uint8_t auto_range;
uint32_t itimeref;
uint8_t ifreq;
uint8_t inosamples;
uint8_t inos;
uint8_t count_index = 0;
uint8_t count_ptr = 0; 
uint8_t start_controller_cycle = 0;
float *E, *I, *T, *Sweep_potential;
float Sweep_rate, Volts_per_point, Min_autocurrent_range;
float Frequency,Pulse_height,Scan_increment;
float Pulse_width;
float Scan_rate;
float Scan_time;
float Step_time;
float WE_Area,WE_material_density,Equivalent_weight;
float param1[64] ;
float param2[64] ;
uint32_t icount;
uint32_t iexpdatasize;
uint32_t itotalsamplesend;
uint32_t icurrentpacket;

void store_chronoamperometry_data();
void store_chronopotentiometry_data();
void store_cyclicvoltametry_data();
void store_param_data();
void store_normalpulsevoltametry_data();
void store_squarewavevoltametry_data();
void store_chronoOCP_data();
void store_chronoVaux_data();
void store_differentialpulsevoltametry_data();
void store_linearpulsevoltametry_data();
//void store_experiment_data();

void send_chronoamperometry_data();
void send_chronopotentiometry_data();
void send_cyclicvoltametry_data();
void send_normalpulsevoltametry_data();
void send_squarewavevoltametry_data();
void send_chronoOCP_data();
void send_chronoVaux_data();
void send_linearpulsevoltametry_data();
void send_differentialpulsevoltametry_data();
void send_squarepulsevoltametry_data();


uint8_t *OCP, *Collect_data,*VSOCP_arr;
uint8_t hours,minutes,seconds;
uint8_t idd,imm,iyy;
uint8_t ihour,imin,isec;
int* myArray = 0;
int myArraySize = 0;
bool flag = 1;
float lower = -300.0, upper = 300.0, count = 240;
time_t t;
uint8_t temp;
uint8_t rem_samples;


 static uint8_t rec_frame = 0;
 uint8_t ack_ready[64] = {'Y','E','S','!','!'};

union {
  unsigned char _char[4];
  float _float;
  uint32_t _int32;
  uint16_t _int16;
  uint8_t _int8;
} Data;

union {
  unsigned char _char[4];
  float _float;
  uint32_t _int32;
  uint16_t _int16;
  uint8_t _int8;
} Data_converted; 
