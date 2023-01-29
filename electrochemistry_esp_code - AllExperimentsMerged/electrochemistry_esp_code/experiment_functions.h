/* 
    contains the following functions -
    1)store_chronoamperometry_data()
    2)store_chronopotentiometry_data()
    3)store_cyclicvoltametry_data()
    4)store_normalpulsevoltametry_data()
    5)store_squarewavevoltametry_data()
    6)store_chronoOCP_data()
    7)store_chronoVaux_data()
    8)store_linearpulsevoltametry_data()
    9)store_differentialpulsevoltametry_data()
    10)send_chronoamperometry_data()
    11)send_chronopotentiometry_data()
    12)send_cyclicvoltametry_data()
    13)send_normalpulsevoltametry_data()
    14)send_squarepulsevoltametry_data()
    15)send_chronoOCP_data()
    16)send_chronoVaux_data()
    17)send_linearpulsevoltametry_data()
    18)send_differentialpulsevoltametry_data() 
*/


#include "definitions.h"
void send_data_to_controller();
//-------------------------------------------store chronoamperometry_data--------------------------------------------------------------//

void store_chronoamperometry_data()
{
  number_of_steps = byte_data[5];
  //Serial.println(number_of_steps);
  E = (float *)calloc(number_of_steps, sizeof(float));
  T = (float *)calloc(number_of_steps, sizeof(float));
  OCP = (uint8_t *)calloc(number_of_steps, sizeof(uint8_t));
  Collect_data = (uint8_t *)calloc(number_of_steps, sizeof(uint8_t));
  uint8_t j = 6;
  itotalsamplesend = 0;
  for (uint8_t i = 0; i < (number_of_steps); i++)
  {
    Data._char[0] = byte_data[j];
    Data._char[1] = byte_data[j + 1];
    Data._char[2] = byte_data[j + 2];
    Data._char[3] = byte_data[j + 3];
    E[i] = Data._float;
    Serial.println(E[i]);
    j = j + 4;
    Data._char[0] = byte_data[j];
    Data._char[1] = byte_data[j + 1];
    Data._char[2] = byte_data[j + 2];
    Data._char[3] = byte_data[j + 3];
    T[i] = Data._float;
    j = j + 4;
    Serial.println(T[i]);
    OCP[i] = byte_data[j];
    Collect_data[i] = byte_data[j + 1];
    j = j + 2;
    itotalsamplesend = itotalsamplesend + ((uint32_t) T[i]);
  }
  
  Data._char[0] = byte_data[j];
  j=j+1;
  Data._char[1] = byte_data[j];
  sample_rate = Data._int16;
  inosamples = 2;

  Serial.println(sample_rate);
  number_of_cycles = byte_data[j + 1];
  Serial.println(number_of_cycles);
  OCP_measurement_mode = byte_data[j + 2];
  Serial.println(OCP_measurement_mode);
  OCP_measurement_period = byte_data[j + 3];
  Serial.println(OCP_measurement_period);
  j = j + 4;
  
  Data._char[0] = byte_data[j];
  Data._char[1] = byte_data[j + 1];
  Data._char[2] = byte_data[j + 2];
  Data._char[3] = byte_data[j + 3];
  OCP_measurement_slope = Data._float;
  Serial.println(OCP_measurement_slope);
  j = j + 4;

  WE_number = byte_data[j];
  Serial.println(WE_number);
  j = j + 1;
  Data._char[0] = byte_data[j];
  Data._char[1] = byte_data[j + 1];
  Data._char[2] = byte_data[j + 2];
  Data._char[3] = byte_data[j + 3];
  initial_potential = Data._float;
  Serial.println(initial_potential);
  j = j + 4;

  VSOCP = byte_data[j];
  Serial.println(VSOCP);
  j = j + 1;

  Data._char[0] = byte_data[j];
  Data._char[1] = byte_data[j + 1];
  Data._char[2] = byte_data[j + 2];
  Data._char[3] = byte_data[j + 3];
  initial_potential_time = Data._float;
  Serial.println(initial_potential_time);
  j = j + 4;

  current_range = byte_data[j];
  Serial.println(current_range);
  auto_range = byte_data[j + 1];
  Serial.println(auto_range);
  post_exp_cell_state = byte_data[j + 2];
  Serial.println(post_exp_cell_state);
  Serial.print(sample_rate);
  Serial.print(number_of_cycles);

  idd =  byte_data[j + 3];
  imm =  byte_data[j + 4];
  iyy =  byte_data[j + 5];

  ihour =  byte_data[j + 6];
  imin =  byte_data[j + 7];
  isec =  byte_data[j + 8]; 

  itotalsamplesend = itotalsamplesend * sample_rate * number_of_cycles;
  itotalsamplesend = (itotalsamplesend + (sample_rate * (uint32_t) initial_potential_time)) ;//+ (sample_rate * (uint32_t) OCP_measurement_period))*inosamples ;
  Serial.println(itotalsamplesend);

  byte_data_send[0] = SOH;
  byte_data_send[1] = 0x35;
  byte_data_send[2] = STX;
  byte_data_send[3] = DEVICEID;
  byte_data_send[4] = local_packet_id;
  byte_data_send[5] = ACK;
  byte_data_send[6] = ETX;
  byte_data_send[7] = 0;
  byte_data_send[8] = CR;

  Serial.println("Sending Acknowledgement: ");
  for (uint8_t j = 0; j < (9); j++)
  {
    Serial.print(byte_data_send[j], HEX);
    Serial.print(",");
  }

  //now serial print the received data also

  for (uint8_t i = 0; i < MAX_SRV_CLIENTS; i++)
  {
    if (serverClients[i] && serverClients[i].connected())
    {
      serverClients[i].write(byte_data_send, (10));
    }
    delay(1);
  }
  send_data_to_controller();
}

//-------------------------------------------store chronoamperometry data--------------------------------------------------------------//

//-------------------------------------------store chronopotentiometry data--------------------------------------------------------------//

void store_chronopotentiometry_data()
{
  number_of_steps = byte_data[5];
  //Serial.println(number_of_steps);
  I = (float *)calloc(number_of_steps, sizeof(float));
  T = (float *)calloc(number_of_steps, sizeof(float));
  Collect_data = (uint8_t *)calloc(number_of_steps, sizeof(uint8_t));
  uint8_t j = 6;
  itotalsamplesend = 0;
  for (uint8_t i = 0; i < (number_of_steps); i++)
  {
    Data._char[0] = byte_data[j];
    Data._char[1] = byte_data[j + 1];
    Data._char[2] = byte_data[j + 2];
    Data._char[3] = byte_data[j + 3];
    I[i] = Data._float;
    Serial.println(I[i]);
    j = j + 4;
    Data._char[0] = byte_data[j];
    Data._char[1] = byte_data[j + 1];
    Data._char[2] = byte_data[j + 2];
    Data._char[3] = byte_data[j + 3];
    T[i] = Data._float;
    j = j + 4;
    Serial.println(T[i]);
    Collect_data[i] = byte_data[j];
    j = j + 1;
   itotalsamplesend = itotalsamplesend + ((uint32_t) T[i]*number_of_cycles*sample_rate); 
  }
  //        sample_rate = byte_data[j];
  Data._char[0] = byte_data[j];
  j=j+1;
  Data._char[1] = byte_data[j];
  sample_rate = Data._int16;
  //    ifreq = ifreqarray[sample_rate]; //get total samples per packet from thi
  inosamples = 2; //for ocp set the no of parameters

  Serial.println(sample_rate);
  number_of_cycles = byte_data[j + 1];
  Serial.println(number_of_cycles);
  OCP_measurement_mode = byte_data[j + 2];
  Serial.println(OCP_measurement_mode);
  OCP_measurement_period = byte_data[j + 3];
  Serial.println(OCP_measurement_period);
  j = j + 4;

  Data._char[0] = byte_data[j];
  Data._char[1] = byte_data[j + 1];
  Data._char[2] = byte_data[j + 2];
  Data._char[3] = byte_data[j + 3];
  OCP_measurement_slope = Data._float;
  Serial.println(OCP_measurement_slope);
  j = j + 4;

  WE_number = byte_data[j];
  Serial.println(WE_number);
  j = j + 1;
  Data._char[0] = byte_data[j];
  Data._char[1] = byte_data[j + 1];
  Data._char[2] = byte_data[j + 2];
  Data._char[3] = byte_data[j + 3];
  initial_potential = Data._float;
  Serial.println(initial_potential);
  j = j + 4;

  VSOCP = byte_data[j];
  Serial.println(VSOCP);
  j = j + 1;

  Data._char[0] = byte_data[j];
  Data._char[1] = byte_data[j + 1];
  Data._char[2] = byte_data[j + 2];
  Data._char[3] = byte_data[j + 3];
  initial_potential_time = Data._float;
  Serial.println(initial_potential_time);
  j = j + 4;

  current_range = byte_data[j];
  Serial.println(current_range);
  auto_range = byte_data[j + 1];
  Serial.println(auto_range);
  post_exp_cell_state = byte_data[j + 2];
  Serial.println(post_exp_cell_state);

  idd =  byte_data[j + 3];
  imm =  byte_data[j + 4];
  iyy =  byte_data[j + 5];

  ihour =  byte_data[j + 6];
  imin =  byte_data[j + 7];
  isec =  byte_data[j + 8]; 

  itotalsamplesend = itotalsamplesend * sample_rate * number_of_cycles;
  itotalsamplesend = itotalsamplesend + (sample_rate * (uint32_t) initial_potential_time);
  Serial.println(itotalsamplesend);
  Serial.print(sample_rate);
  Serial.print(number_of_cycles);

  byte_data_send[0] = SOH;
  byte_data_send[1] = 0x35;
  byte_data_send[2] = STX;
  byte_data_send[3] = DEVICEID;
  byte_data_send[4] = local_packet_id;
  byte_data_send[5] = ACK;
  byte_data_send[6] = ETX;
  byte_data_send[7] = 0;
  byte_data_send[8] = CR;

  Serial.println("Sending Acknowledgement: ");
  for (uint8_t j = 0; j < (9); j++)
  {
    Serial.print(byte_data_send[j], HEX);
    Serial.print(",");
  }

  //now serial print the received data also


  for (uint8_t i = 0; i < MAX_SRV_CLIENTS; i++)
  {
    if (serverClients[i] && serverClients[i].connected())
    {
      serverClients[i].write(byte_data_send, (10));
    }
    //delay(1);
  }
  send_data_to_controller();
}

//---------------------------------------- store chronopotentiometry data-----------------------------------------------------------//

//-------------------------------------------store cyclic voltametry data--------------------------------------------------------------//

void store_cyclicvoltametry_data()
{
  float fvolt1;
  float fvolt2;


  inosamples = 2; //for ocp set the no of parameters
  
  Sweep_potential = (float *)calloc(4, sizeof(float));
  VSOCP_arr = (uint8_t *)calloc(4, sizeof(uint8_t));
  number_of_cycles = byte_data[5];
  Serial.println(number_of_cycles);
  OCP_measurement_mode = byte_data[6];
  Serial.println(OCP_measurement_mode);
  OCP_measurement_period = byte_data[7];
  Serial.println(OCP_measurement_period);

  uint8_t j = 8;
  Data._char[0] = byte_data[j];
  Data._char[1] = byte_data[j + 1];
  Data._char[2] = byte_data[j + 2];
  Data._char[3] = byte_data[j + 3];
  OCP_measurement_slope = Data._float;
  Serial.println(OCP_measurement_slope);
  j = j + 4;

  Data._char[0] = byte_data[j];
  Data._char[1] = byte_data[j + 1];
  Data._char[2] = byte_data[j + 2];
  Data._char[3] = byte_data[j + 3];
  initial_potential = Data._float;
  Serial.println(initial_potential);
  j = j + 4;

  Data._char[0] = byte_data[j];
  Data._char[1] = byte_data[j + 1];
  Data._char[2] = byte_data[j + 2];
  Data._char[3] = byte_data[j + 3];
  initial_potential_time = Data._float;
  Serial.println(initial_potential_time);
  j = j + 4;

  VSOCP = byte_data[j];
  Serial.println(VSOCP);
  j = j + 1;

  for (uint8_t i = 0; i < (4); i++)
  {
    Data._char[0] = byte_data[j];
    Data._char[1] = byte_data[j + 1];
    Data._char[2] = byte_data[j + 2];
    Data._char[3] = byte_data[j + 3];
    Sweep_potential[i] = Data._float;
    Serial.println(Sweep_potential[i]);
    VSOCP_arr[i] = byte_data[j + 4];
    Serial.println(VSOCP_arr[i]);
    j = j + 5;
  }

  fvolt1 = abs(Sweep_potential[1]) + abs(Sweep_potential[0]);
  fvolt2 =  abs(Sweep_potential[3]) + abs(Sweep_potential[2]);

  

  Data._char[0] = byte_data[j];
  Data._char[1] = byte_data[j + 1];
  Data._char[2] = byte_data[j + 2];
  Data._char[3] = byte_data[j + 3];
  Sweep_rate = Data._float;
  Serial.println(Sweep_rate);
  j = j + 4;

  Data._char[0] = byte_data[j];
  Data._char[1] = byte_data[j + 1];
  Data._char[2] = byte_data[j + 2];
  Data._char[3] = byte_data[j + 3];
  Volts_per_point = Data._float;
  Serial.println(Volts_per_point);
  j = j + 4;

  current_range = byte_data[j];
  Serial.println(current_range);
  auto_range = byte_data[j + 1];
  Serial.println(auto_range);

  j = j + 2;
  WE_number = byte_data[j];
  Serial.println(WE_number);
  j = j + 1;


  Data._char[0] = byte_data[j];
  Data._char[1] = byte_data[j + 1];
  Data._char[2] = byte_data[j + 2];
  Data._char[3] = byte_data[j + 3];
  Min_autocurrent_range = Data._float;
  Serial.println(Min_autocurrent_range);
  j = j + 4;

  post_exp_cell_state = byte_data[j];
  Serial.println(post_exp_cell_state);

  idd =  byte_data[j + 1];
  imm =  byte_data[j + 2];
  iyy =  byte_data[j + 3];

  ihour =  byte_data[j + 4];
  imin =  byte_data[j + 5];
  isec =  byte_data[j + 6]; 

  
//  itotalsamplesend = itotalsamplesend * sample_rate * number_of_cycles;
//  itotalsamplesend = itotalsamplesend + (sample_rate * (uint32_t) initial_potential_time);
  sample_rate =  (Sweep_rate / Volts_per_point);
  itotalsamplesend = (initial_potential_time)*sample_rate*inosamples + (((fvolt1+fvolt2)/Volts_per_point)*number_of_cycles)*inosamples;
//  itotalsamplesend = itotalsamplesend + 1;
  Serial.print("Sweep_rate:");
  Serial.println(Sweep_rate);
  
  Serial.print("Volts_per_point:");
  Serial.println(Volts_per_point);
  
  Serial.print("fvolt1:");
  Serial.println(fvolt1);
  
  Serial.print("fvolt2:");
  Serial.println(fvolt2);
  
  Serial.print("itotalsamplesend:");
  Serial.println(itotalsamplesend);
  
  Serial.print("sample_rate:");
  Serial.println(sample_rate);
  
  byte_data_send[0] = SOH;
  byte_data_send[1] = 0x35;
  byte_data_send[2] = STX;
  byte_data_send[3] = DEVICEID;
  byte_data_send[4] = local_packet_id;
  byte_data_send[5] = ACK;
  byte_data_send[6] = ETX;
  byte_data_send[7] = 0;
  byte_data_send[8] = CR;

  Serial.println("Sending Acknowledgement: ");
  for (uint8_t j = 0; j < (9); j++)
  {
    Serial.print(byte_data_send[j], HEX);
    Serial.print(",");
  }

  //now serial print the received data also


  for (uint8_t i = 0; i < MAX_SRV_CLIENTS; i++)
  {
    if (serverClients[i] && serverClients[i].connected())
    {
      serverClients[i].write(byte_data_send, (10));
    }
    //delay(1);
  }
  send_data_to_controller();
}

//-------------------------------------------store cyclic voltametry data--------------------------------------------------------------//

//-------------------------------------------store normal pulse voltametry data--------------------------------------------------------------//

void store_normalpulsevoltametry_data()
{
  Sweep_potential = (float *)calloc(2, sizeof(float));
  VSOCP_arr = (uint8_t *)calloc(2, sizeof(uint8_t));
  OCP_measurement_mode = byte_data[5];
  inosamples = 2;
  Serial.println(OCP_measurement_mode);
  OCP_measurement_period = byte_data[6];
  Serial.println(OCP_measurement_period);

  uint8_t j = 7;
  Data._char[0] = byte_data[j];
  Data._char[1] = byte_data[j + 1];
  Data._char[2] = byte_data[j + 2];
  Data._char[3] = byte_data[j + 3];
  OCP_measurement_slope = Data._float;
  Serial.println(OCP_measurement_slope);
  j = j + 4;

  Data._char[0] = byte_data[j];
  Data._char[1] = byte_data[j + 1];
  Data._char[2] = byte_data[j + 2];
  Data._char[3] = byte_data[j + 3];
  initial_potential = Data._float;
  Serial.println(initial_potential);
  j = j + 4;

  Data._char[0] = byte_data[j];
  Data._char[1] = byte_data[j + 1];
  Data._char[2] = byte_data[j + 2];
  Data._char[3] = byte_data[j + 3];
  initial_potential_time = Data._float;
  Serial.println(initial_potential_time);
  j = j + 4;

  VSOCP = byte_data[j];
  Serial.println(VSOCP);
  j = j + 1;

  for (uint8_t i = 0; i < (2); i++)
  {
    Data._char[0] = byte_data[j];
    Data._char[1] = byte_data[j + 1];
    Data._char[2] = byte_data[j + 2];
    Data._char[3] = byte_data[j + 3];
    Sweep_potential[i] = Data._float;
    Serial.println(Sweep_potential[i]);
    VSOCP_arr[i] = byte_data[j + 4];
    Serial.println(VSOCP_arr[i]);
    j = j + 5;
  }

  Data._char[0] = byte_data[j];
  Data._char[1] = byte_data[j + 1];
  Data._char[2] = byte_data[j + 2];
  Data._char[3] = byte_data[j + 3];
  Pulse_height = Data._float;
  Serial.println(Pulse_height);
  j = j + 4;

  Data._char[0] = byte_data[j];
  Data._char[1] = byte_data[j + 1];
  Data._char[2] = byte_data[j + 2];
  Data._char[3] = byte_data[j + 3];
  Frequency = Data._float;
  Serial.println(Frequency);
  j = j + 4;

  current_range = byte_data[j];
  Serial.println(current_range);
  j = j + 1;
  WE_number = byte_data[j];
  Serial.println(WE_number);
  j = j + 1;
  post_exp_cell_state = byte_data[j];
  Serial.println(post_exp_cell_state);
  j = j + 1;

  idd =  byte_data[j];
  imm =  byte_data[j + 1];
  iyy =  byte_data[j + 2];

  ihour =  byte_data[j + 3];
  imin =  byte_data[j + 4];
  isec =  byte_data[j + 5]; 

  byte_data_send[0] = SOH;
  byte_data_send[1] = 0x35;
  byte_data_send[2] = STX;
  byte_data_send[3] = DEVICEID;
  byte_data_send[4] = local_packet_id;
  byte_data_send[5] = ACK;
  byte_data_send[6] = ETX;
  byte_data_send[7] = 0;
  byte_data_send[8] = CR;

  Serial.println("Sending Acknowledgement: ");
  for (uint8_t j = 0; j < (9); j++)
  {
    Serial.print(byte_data_send[j], HEX);
    Serial.print(",");
  }

  //now serial print the received data also


  for (uint8_t i = 0; i < MAX_SRV_CLIENTS; i++)
  {
    if (serverClients[i] && serverClients[i].connected())
    {
      serverClients[i].write(byte_data_send, (10));
    }
    //delay(1);
  }
}

//-------------------------------------------store normal pulse voltametry data--------------------------------------------------------------//

//-------------------------------------------store squarewave voltametry data--------------------------------------------------------------//

void store_squarewavevoltametry_data()
{
  Sweep_potential = (float *)calloc(2, sizeof(float));
  VSOCP_arr = (uint8_t *)calloc(2, sizeof(uint8_t));
  OCP_measurement_mode = byte_data[5];
  Serial.println(OCP_measurement_mode);
  OCP_measurement_period = byte_data[6];
  Serial.println(OCP_measurement_period);
  inosamples = 2;

  uint8_t j = 7;
  Data._char[0] = byte_data[j];
  Data._char[1] = byte_data[j + 1];
  Data._char[2] = byte_data[j + 2];
  Data._char[3] = byte_data[j + 3];
  OCP_measurement_slope = Data._float;
  Serial.println(OCP_measurement_slope);
  j = j + 4;

  Data._char[0] = byte_data[j];
  Data._char[1] = byte_data[j + 1];
  Data._char[2] = byte_data[j + 2];
  Data._char[3] = byte_data[j + 3];
  initial_potential = Data._float;
  Serial.println(initial_potential);
  j = j + 4;

  Data._char[0] = byte_data[j];
  Data._char[1] = byte_data[j + 1];
  Data._char[2] = byte_data[j + 2];
  Data._char[3] = byte_data[j + 3];
  initial_potential_time = Data._float;
  Serial.println(initial_potential_time);
  j = j + 4;

  VSOCP = byte_data[j];
  Serial.println(VSOCP);
  j = j + 1;

  for (uint8_t i = 0; i < (2); i++)
  {
    Data._char[0] = byte_data[j];
    Data._char[1] = byte_data[j + 1];
    Data._char[2] = byte_data[j + 2];
    Data._char[3] = byte_data[j + 3];
    Sweep_potential[i] = Data._float;
    Serial.println(Sweep_potential[i]);
    VSOCP_arr[i] = byte_data[j + 4];
    Serial.println(VSOCP_arr[i]);
    j = j + 5;
  }

  Data._char[0] = byte_data[j];
  Data._char[1] = byte_data[j + 1];
  Data._char[2] = byte_data[j + 2];
  Data._char[3] = byte_data[j + 3];
  Pulse_height = Data._float;
  Serial.println(Pulse_height);
  j = j + 4;

  Data._char[0] = byte_data[j];
  Data._char[1] = byte_data[j + 1];
  Data._char[2] = byte_data[j + 2];
  Data._char[3] = byte_data[j + 3];
  Frequency = Data._float;
  Serial.println(Frequency);
  j = j + 4;
  Data._char[0] = byte_data[j];
  Data._char[1] = byte_data[j + 1];
  Data._char[2] = byte_data[j + 2];
  Data._char[3] = byte_data[j + 3];
  Scan_increment = Data._float;
  Serial.println(Scan_increment,4);
  j = j + 4;
  
  current_range = byte_data[j];
  Serial.println(current_range);
  j = j + 1;
  WE_number = byte_data[j];
  Serial.println(WE_number);
  j = j + 1;
  post_exp_cell_state = byte_data[j];
  Serial.println(post_exp_cell_state);
  j = j + 1;

  idd =  byte_data[j];
  imm =  byte_data[j + 1];
  iyy =  byte_data[j + 2];

  ihour =  byte_data[j + 3];
  imin =  byte_data[j + 4];
  isec =  byte_data[j + 5]; 

  sample_rate = Frequency;
  itotalsamplesend = abs((Sweep_potential[1] - Sweep_potential[0] + Pulse_height) / Scan_increment);
  itotalsamplesend = (itotalsamplesend)*sample_rate + (sample_rate * (uint32_t) initial_potential_time);
  Serial.println(itotalsamplesend);


  byte_data_send[0] = SOH;
  byte_data_send[1] = 0x35;
  byte_data_send[2] = STX;
  byte_data_send[3] = DEVICEID;
  byte_data_send[4] = local_packet_id;
  byte_data_send[5] = ACK;
  byte_data_send[6] = ETX;
  byte_data_send[7] = 0;
  byte_data_send[8] = CR;

  Serial.println("Sending Acknowledgement: ");
  for (uint8_t j = 0; j < (9); j++)
  {
    Serial.print(byte_data_send[j], HEX);
    Serial.print(",");
  }

  //now serial print the received data also


  for (uint8_t i = 0; i < MAX_SRV_CLIENTS; i++)
  {
    if (serverClients[i] && serverClients[i].connected())
    {
      serverClients[i].write(byte_data_send, (10));
    }
    //delay(1);
  }
  send_data_to_controller();
}

//-------------------------------------------store square wave voltametry data----------------------------------------------------------//

//-------------------------------------------store chrono OCP data--------------------------------------------------------------//

void store_chronoOCP_data()
{
  inosamples = 2;
  Data._char[0] = byte_data[5];
  Data._char[1] = byte_data[6];
  sample_rate = Data._int16;
  Serial.print("sample_rate");
  Serial.println(sample_rate);
  hours = byte_data[7];
  Serial.print("hours");
  Serial.println(hours);
  minutes = byte_data[8];
  Serial.print("minutes");
  Serial.println(minutes);
  seconds = byte_data[9];
  Serial.print("seconds");
  Serial.println(seconds);
  WE_number =  byte_data[10];
  Serial.println(WE_number);
  post_exp_cell_state = byte_data[11];
  Serial.println(post_exp_cell_state);
  uint8_t j=12;
  
  Data._char[0] = byte_data[j];
  Data._char[1] = byte_data[j + 1];
  Data._char[2] = byte_data[j + 2];
  Data._char[3] = byte_data[j + 3];
  WE_Area = Data._float;
  Serial.println(WE_Area);
  j = j+4;

  Data._char[0] = byte_data[j];
  Data._char[1] = byte_data[j + 1];
  Data._char[2] = byte_data[j + 2];
  Data._char[3] = byte_data[j + 3];
  WE_material_density = Data._float;
  Serial.println(WE_material_density);
  j = j+4;

  Data._char[0] = byte_data[j];
  Data._char[1] = byte_data[j + 1];
  Data._char[2] = byte_data[j + 2];
  Data._char[3] = byte_data[j + 3];
  Equivalent_weight = Data._float;
  Serial.println(Equivalent_weight);
  j = j+4;

  idd =  byte_data[j];
  imm =  byte_data[j + 1];
  iyy =  byte_data[j + 2];

  ihour =  byte_data[j + 3];
  imin =  byte_data[j + 4];
  isec =  byte_data[j + 5]; 

  itotalsamplesend = itotalsamplesend + (sample_rate * (uint32_t) initial_potential_time);
  itotalsamplesend = (OCP_measurement_period + initial_potential_time)*sample_rate*inosamples + (hours*3600 + minutes*60 + seconds)*sample_rate*inosamples;
  Serial.print("itotalsamplesend");
  Serial.print(" ");
  Serial.println(itotalsamplesend);

  
  byte_data_send[0] = SOH;
  byte_data_send[1] = 0x35;
  byte_data_send[2] = STX;
  byte_data_send[3] = DEVICEID;
  byte_data_send[4] = local_packet_id;
  byte_data_send[5] = ACK;
  byte_data_send[6] = ETX;
  byte_data_send[7] = 0;
  byte_data_send[8] = CR;

  Serial.println("Sending Acknowledgement: ");
  for (uint8_t j = 0; j < (9); j++)
  {
    Serial.print(byte_data_send[j], HEX);
    Serial.print(",");
  }

  //now serial print the received data also


  for (uint8_t i = 0; i < MAX_SRV_CLIENTS; i++)
  {
    if (serverClients[i] && serverClients[i].connected())
    {
      serverClients[i].write(byte_data_send, (10));
    }
    //delay(1);
  }
  send_data_to_controller();   
}

//-------------------------------------------store chrono OCP data--------------------------------------------------------------//

//-------------------------------------------store chronoVAUX data--------------------------------------------------------------//

void store_chronoVaux_data()
{
  inosamples = 2;
  sample_rate = byte_data[5];
  Serial.println(sample_rate);
  hours = byte_data[6];
  Serial.println(hours);
  minutes = byte_data[7];
  Serial.println(minutes);
  seconds = byte_data[8];
  Serial.println(seconds);
  WE_number =  byte_data[9];
  Serial.println(WE_number);

    idd =  byte_data[10];
  imm =  byte_data[11];
  iyy =  byte_data[12];

  ihour =  byte_data[13];
  imin =  byte_data[14];
  isec =  byte_data[15]; 

  itotalsamplesend = itotalsamplesend * sample_rate * number_of_cycles;
  itotalsamplesend = itotalsamplesend + (sample_rate * (uint32_t) initial_potential_time);
  Serial.println(itotalsamplesend);


  byte_data_send[0] = SOH;
  byte_data_send[1] = 0x35;
  byte_data_send[2] = STX;
  byte_data_send[3] = DEVICEID;
  byte_data_send[4] = local_packet_id;
  byte_data_send[5] = ACK;
  byte_data_send[6] = ETX;
  byte_data_send[7] = 0;
  byte_data_send[8] = CR;

  Serial.println("Sending Acknowledgement: ");
  for (uint8_t j = 0; j < (9); j++)
  {
    Serial.print(byte_data_send[j], HEX);
    Serial.print(",");
  }

  //now serial print the received data also


  for (uint8_t i = 0; i < MAX_SRV_CLIENTS; i++)
  {
    if (serverClients[i] && serverClients[i].connected())
    {
      serverClients[i].write(byte_data_send, (10));
    }
    //delay(1);
  }
}

//-------------------------------------------store chronoVAUX data--------------------------------------------------------------//

//-------------------------------------------store linear pulse voltametry data--------------------------------------------------------//

void store_linearpulsevoltametry_data()
{ 
  inosamples = 2;
  Sweep_potential = (float *)calloc(2, sizeof(float));
  VSOCP_arr = (uint8_t *)calloc(2, sizeof(uint8_t));
  OCP_measurement_mode = byte_data[5];
  Serial.println(OCP_measurement_mode);
  OCP_measurement_period = byte_data[6];
  Serial.println(OCP_measurement_period);

  uint8_t j = 7;
  Data._char[0] = byte_data[j];
  Data._char[1] = byte_data[j + 1];
  Data._char[2] = byte_data[j + 2];
  Data._char[3] = byte_data[j + 3];
  OCP_measurement_slope = Data._float;
  Serial.println(OCP_measurement_slope);
  j = j + 4;

  Data._char[0] = byte_data[j];
  Data._char[1] = byte_data[j + 1];
  Data._char[2] = byte_data[j + 2];
  Data._char[3] = byte_data[j + 3];
  initial_potential = Data._float;
  Serial.println(initial_potential);
  j = j + 4;

  Data._char[0] = byte_data[j];
  Data._char[1] = byte_data[j + 1];
  Data._char[2] = byte_data[j + 2];
  Data._char[3] = byte_data[j + 3];
  initial_potential_time = Data._float;
  Serial.println(initial_potential_time);
  j = j + 4;

  VSOCP = byte_data[j];
  Serial.println(VSOCP);
  j = j + 1;

  for (uint8_t i = 0; i < (2); i++)
  {
    Data._char[0] = byte_data[j];
    Data._char[1] = byte_data[j + 1];
    Data._char[2] = byte_data[j + 2];
    Data._char[3] = byte_data[j + 3];
    Sweep_potential[i] = Data._float;
    Serial.println(Sweep_potential[i]);
    VSOCP_arr[i] = byte_data[j + 4];
    Serial.println(VSOCP_arr[i]);
    j = j + 5;
  }

  Data._char[0] = byte_data[j];
  Data._char[1] = byte_data[j + 1];
  Data._char[2] = byte_data[j + 2];
  Data._char[3] = byte_data[j + 3];
  Sweep_rate = Data._float;
  Serial.println(Sweep_rate);
  j = j + 4;

  Data._char[0] = byte_data[j];
  Data._char[1] = byte_data[j + 1];
  Data._char[2] = byte_data[j + 2];
  Data._char[3] = byte_data[j + 3];
  Volts_per_point = Data._float;
  Serial.println(Volts_per_point);
  j = j + 4;

  current_range = byte_data[j];
  Serial.println(current_range);
  
  auto_range = byte_data[j + 1];
  Serial.println(auto_range);

  j = j + 2;
  WE_number = byte_data[j];
  Serial.println(WE_number);
  j = j + 1;
  post_exp_cell_state = byte_data[j];
  Serial.println(post_exp_cell_state);
  j = j + 1;

   idd =  byte_data[j];
  imm =  byte_data[j + 1];
  iyy =  byte_data[j + 2];

  ihour =  byte_data[j + 3];
  imin =  byte_data[j + 4];
  isec =  byte_data[j + 5]; 

  sample_rate = (Sweep_rate)/(Volts_per_point);
  itotalsamplesend = ((initial_potential_time + OCP_measurement_period)*sample_rate)*inosamples + ((Sweep_potential[1] - Sweep_potential[0])/(Volts_per_point))*inosamples;
 
  Serial.print("Sweep_potential[0]:");
  Serial.println(Sweep_potential[0]);
  
  Serial.print("Sweep_potential[1]:");
  Serial.println(Sweep_potential[1]);
  
  Serial.print("Volts_per_point:");
  Serial.println(Volts_per_point);
  
  Serial.print("itotalsamplesend:");
  Serial.println(itotalsamplesend);
  
  Serial.print("Sweep_rate:");
  Serial.println(Sweep_rate);
  
  Serial.print("sample rate:");
  Serial.println(sample_rate);
  Serial.print("number of cycles");
  Serial.println(number_of_cycles);

  
  byte_data_send[0] = SOH;
  byte_data_send[1] = 0x35;
  byte_data_send[2] = STX;
  byte_data_send[3] = DEVICEID;
  byte_data_send[4] = local_packet_id;
  byte_data_send[5] = ACK;
  byte_data_send[6] = ETX;
  byte_data_send[7] = 0;
  byte_data_send[8] = CR;

  Serial.println("Sending Acknowledgement: ");
  for (uint8_t j = 0; j < (9); j++)
  {
    Serial.print(byte_data_send[j], HEX);
    Serial.print(",");
  }

  //now serial print the received data also


  for (uint8_t i = 0; i < MAX_SRV_CLIENTS; i++)
  {
    if (serverClients[i] && serverClients[i].connected())
    {
      serverClients[i].write(byte_data_send, (10));
    }
    //delay(1);
  }
  send_data_to_controller();
}

//-------------------------------------------store linear pulse voltametry data--------------------------------------------------------//

//-------------------------------------------store differential pulse voltametry data--------------------------------------------------------//

void store_differentialpulsevoltametry_data()
{
  inosamples = 2;
  Sweep_potential = (float *)calloc(2, sizeof(float));
  VSOCP_arr = (uint8_t *)calloc(2, sizeof(uint8_t));
  OCP_measurement_mode = byte_data[5];
  Serial.println(OCP_measurement_mode);
  OCP_measurement_period = byte_data[6];
  Serial.println(OCP_measurement_period);

  uint8_t j = 7;
  Data._char[0] = byte_data[j];
  Data._char[1] = byte_data[j + 1];
  Data._char[2] = byte_data[j + 2];
  Data._char[3] = byte_data[j + 3];
  OCP_measurement_slope = Data._float;
  Serial.println(OCP_measurement_slope);
  j = j + 4;

  Data._char[0] = byte_data[j];
  Data._char[1] = byte_data[j + 1];
  Data._char[2] = byte_data[j + 2];
  Data._char[3] = byte_data[j + 3];
  initial_potential = Data._float;
  Serial.println(initial_potential);
  j = j + 4;

  Data._char[0] = byte_data[j];
  Data._char[1] = byte_data[j + 1];
  Data._char[2] = byte_data[j + 2];
  Data._char[3] = byte_data[j + 3];
  initial_potential_time = Data._float;
  Serial.println(initial_potential_time);
  j = j + 4;

  VSOCP = byte_data[j];
  Serial.println(VSOCP);
  j = j + 1;

  for (uint8_t i = 0; i < (2); i++)
  {
    Data._char[0] = byte_data[j];
    Data._char[1] = byte_data[j + 1];
    Data._char[2] = byte_data[j + 2];
    Data._char[3] = byte_data[j + 3];
    Sweep_potential[i] = Data._float;
    Serial.println(Sweep_potential[i]);
    VSOCP_arr[i] = byte_data[j + 4];
    Serial.println(VSOCP_arr[i]);
    j = j + 5;
  }

  Data._char[0] = byte_data[j];
  Data._char[1] = byte_data[j + 1];
  Data._char[2] = byte_data[j + 2];
  Data._char[3] = byte_data[j + 3];
  Scan_rate = Data._float;
  Serial.println(Scan_rate);
  j = j + 4;

  Data._char[0] = byte_data[j];
  Data._char[1] = byte_data[j + 1];
  Data._char[2] = byte_data[j + 2];
  Data._char[3] = byte_data[j + 3];
  Scan_increment = Data._float;
  Serial.println(Scan_increment);
  j = j + 4;

  Data._char[0] = byte_data[j];
  Data._char[1] = byte_data[j + 1];
  Data._char[2] = byte_data[j + 2];
  Data._char[3] = byte_data[j + 3];
  Step_time = Data._float;
  Serial.println(Step_time);
  j = j + 4;

  Data._char[0] = byte_data[j];
  Data._char[1] = byte_data[j + 1];
  Data._char[2] = byte_data[j + 2];
  Data._char[3] = byte_data[j + 3];
  Pulse_height = Data._float;
  Serial.println(Pulse_height);
  j = j + 4;

  Data._char[0] = byte_data[j];
  Data._char[1] = byte_data[j + 1];
  Data._char[2] = byte_data[j + 2];
  Data._char[3] = byte_data[j + 3];
  Pulse_width = Data._float;
  Serial.println(Pulse_width);
  j = j + 4;

  current_range = byte_data[j];
  Serial.println(current_range);
  j = j + 1;
  WE_number = byte_data[j];
  Serial.println(WE_number);
  j = j + 1;
  post_exp_cell_state = byte_data[j];
  Serial.println(post_exp_cell_state);
  j = j + 1;

  idd =  byte_data[j];
  imm =  byte_data[j + 1];
  iyy =  byte_data[j + 2];

  ihour =  byte_data[j + 3];
  imin =  byte_data[j + 4];
  isec =  byte_data[j + 5]; 

   sample_rate = 1/(Step_time*0.1);
  itotalsamplesend = ((initial_potential_time + OCP_measurement_period)*sample_rate)*inosamples + ((Sweep_potential[1] - Sweep_potential[0])/(Scan_increment))*Step_time*sample_rate*inosamples;
  Serial.print("itotalsamplesend");
  Serial.print(": ");
  Serial.println(itotalsamplesend);


  byte_data_send[0] = SOH;
  byte_data_send[1] = 0x35;
  byte_data_send[2] = STX;
  byte_data_send[3] = DEVICEID;
  byte_data_send[4] = local_packet_id;
  byte_data_send[5] = ACK;
  byte_data_send[6] = ETX;
  byte_data_send[7] = 0;
  byte_data_send[8] = CR;

  Serial.println("Sending Acknowledgement: ");
  for (uint8_t j = 0; j < (9); j++)
  {
    Serial.print(byte_data_send[j], HEX);
    Serial.print(",");
  }

  //now serial print the received data also


  for (uint8_t i = 0; i < MAX_SRV_CLIENTS; i++)
  {
    if (serverClients[i] && serverClients[i].connected())
    {
      serverClients[i].write(byte_data_send, (10));
    }
    //delay(1);
  }
  send_data_to_controller();
}

//-------------------------------------------store differential pulse voltametry data--------------------------------------------------------//

//-------------------------------------------send chronoamperometry data--------------------------------------------------------//

void send_chronoamperometry_data()
{
  uint8_t j = 6;
  //Serial.println("sending settings packet");
  byte_data_send[0] = SOH;
  byte_data_send[1] = 0x36;
  byte_data_send[2] = STX;
  byte_data_send[3] = DEVICEID;
  byte_data_send[4] = experiment_no;
  byte_data_send[5] = number_of_steps;
  for (int i = 0; i < (number_of_steps); i ++)
  {
    Data._float = E[i];
    byte_data_send[j] = Data._char[0];
    byte_data_send[j + 1] = Data._char[1];
    byte_data_send[j + 2] = Data._char[2];
    byte_data_send[j + 3] = Data._char[3];
    j = j + 4;

    Data._float = T[i];
    byte_data_send[j] = Data._char[0];
    byte_data_send[j + 1] = Data._char[1];
    byte_data_send[j + 2] = Data._char[2];
    byte_data_send[j + 3] = Data._char[3];
    j = j + 4;

    byte_data_send[j] = OCP[i];
    byte_data_send[j + 1] = Collect_data[i];
    j = j + 2;
    temp = i;
  }

  byte_data_send[j] = sample_rate;
  byte_data_send[j + 1] = number_of_cycles;
  byte_data_send[j + 2] = OCP_measurement_mode;
  byte_data_send[j + 3] = OCP_measurement_period;

  j = j + 4;

  Data._float = OCP_measurement_slope;
  byte_data_send[j] = Data._char[0];
  byte_data_send[j + 1] = Data._char[1];
  byte_data_send[j + 2] = Data._char[2];
  byte_data_send[j + 3] = Data._char[3];

  byte_data_send[j + 4] = WE_number;

  j = j + 5;

  Data._float = initial_potential;
  byte_data_send[j] = Data._char[0];
  byte_data_send[j + 1] = Data._char[1];
  byte_data_send[j + 2] = Data._char[2];
  byte_data_send[j + 3] = Data._char[3];

  byte_data_send[j + 4] = VSOCP;

  j = j + 5;

  Data._float = initial_potential_time;
  byte_data_send[j] = Data._char[0];
  byte_data_send[j + 1] = Data._char[1];
  byte_data_send[j + 2] = Data._char[2];
  byte_data_send[j + 3] = Data._char[3];

  byte_data_send[j + 4] = current_range;
  byte_data_send[j + 5] = auto_range;
  byte_data_send[j + 6] = post_exp_cell_state;

  byte_data_send[j + 7] = ETX;
  byte_data_send[j + 8] = 0;
  byte_data_send[j + 9] = CR;
  j = j + 10;
  temp = j;

  Serial.println("Sent Data: ");
  for (uint8_t j = 0; j < temp; j++)
  {
    Serial.print(byte_data_send[j], HEX);
    Serial.print(",");
  }
  Serial.println();
  Serial.print("Sending Time(ms) : ");
  Serial.println(millis());
  for (uint8_t i = 0; i < MAX_SRV_CLIENTS; i++)
  {
    if (serverClients[i] && serverClients[i].connected())
    {
      serverClients[i].write(byte_data_send, temp);
      //delay(1)
    }
  }
}

//-------------------------------------------send chronoamperometry data--------------------------------------------------------//

//-------------------------------------------send chronopotentiometry data--------------------------------------------------------//

void send_chronopotentiometry_data()
{
  uint8_t j = 6;
  Serial.println("sending settings packet");
  byte_data_send[0] = SOH;
  byte_data_send[1] = 0x36;
  byte_data_send[2] = STX;
  byte_data_send[3] = DEVICEID;
  byte_data_send[4] = experiment_no;
  byte_data_send[5] = number_of_steps;
  for (int i = 0; i < (number_of_steps); i ++)
  {
    Data._float = I[i];
    byte_data_send[j] = Data._char[0];
    byte_data_send[j + 1] = Data._char[1];
    byte_data_send[j + 2] = Data._char[2];
    byte_data_send[j + 3] = Data._char[3];
    j = j + 4;

    Data._float = T[i];
    byte_data_send[j] = Data._char[0];
    byte_data_send[j + 1] = Data._char[1];
    byte_data_send[j + 2] = Data._char[2];
    byte_data_send[j + 3] = Data._char[3];
    j = j + 4;

    byte_data_send[j] = Collect_data[i];
    j = j + 1;
    temp = i;
  }

  byte_data_send[j] = sample_rate;
  byte_data_send[j + 1] = number_of_cycles;
  byte_data_send[j + 2] = OCP_measurement_mode;
  byte_data_send[j + 3] = OCP_measurement_period;

  j = j + 4;

  Data._float = OCP_measurement_slope;
  byte_data_send[j] = Data._char[0];
  byte_data_send[j + 1] = Data._char[1];
  byte_data_send[j + 2] = Data._char[2];
  byte_data_send[j + 3] = Data._char[3];

  byte_data_send[j + 4] = WE_number;

  j = j + 5;

  Data._float = initial_potential;
  byte_data_send[j] = Data._char[0];
  byte_data_send[j + 1] = Data._char[1];
  byte_data_send[j + 2] = Data._char[2];
  byte_data_send[j + 3] = Data._char[3];

  byte_data_send[j + 4] = VSOCP;

  j = j + 5;

  Data._float = initial_potential_time;
  byte_data_send[j] = Data._char[0];
  byte_data_send[j + 1] = Data._char[1];
  byte_data_send[j + 2] = Data._char[2];
  byte_data_send[j + 3] = Data._char[3];

  byte_data_send[j + 4] = current_range;
  byte_data_send[j + 5] = auto_range;
  byte_data_send[j + 6] = post_exp_cell_state;

  byte_data_send[j + 7] = ETX;
  byte_data_send[j + 8] = 0;
  byte_data_send[j + 9] = CR;
  j = j + 10;
  temp = j;


  Serial.println("Sent Data: ");
  for (uint8_t j = 0; j < temp; j++)
  {
    Serial.print(byte_data_send[j], HEX);
    Serial.print(",");
  }
  Serial.println();
  Serial.print("Sending Time(ms) : ");
  Serial.println(millis());
  for (uint8_t i = 0; i < MAX_SRV_CLIENTS; i++)
  {
    if (serverClients[i] && serverClients[i].connected())
    {
      serverClients[i].write(byte_data_send, temp);
      //delay(1)
    }
  }
}

//-------------------------------------------send chronopotentiometry data--------------------------------------------------------//

//-------------------------------------------send cyclic voltametry data--------------------------------------------------------//

void send_cyclicvoltametry_data()
{
          uint8_t j = 8;
          Serial.println("sending settings packet");
          byte_data_send[0] = SOH;
          byte_data_send[1] = 0x36;
          byte_data_send[2] = STX;
          byte_data_send[3] = DEVICEID;
          byte_data_send[4] = experiment_no;
          byte_data_send[5] = number_of_cycles;
          byte_data_send[6] = OCP_measurement_mode;
          byte_data_send[7] = OCP_measurement_period;
        
          Data._float = OCP_measurement_slope;
          byte_data_send[j] = Data._char[0];
          byte_data_send[j + 1] = Data._char[1];
          byte_data_send[j + 2] = Data._char[2];
          byte_data_send[j + 3] = Data._char[3];
          j = j + 4;
        
          Data._float = initial_potential;
          byte_data_send[j] = Data._char[0];
          byte_data_send[j + 1] = Data._char[1];
          byte_data_send[j + 2] = Data._char[2];
          byte_data_send[j + 3] = Data._char[3];
          j = j + 4;
        
          Data._float = initial_potential_time;
          byte_data_send[j] = Data._char[0];
          byte_data_send[j + 1] = Data._char[1];
          byte_data_send[j + 2] = Data._char[2];
          byte_data_send[j + 3] = Data._char[3];
          j = j + 4;
        
          byte_data_send[j] = VSOCP;
          j = j + 1;
        
          for (int i = 0; i < (4); i ++)
          {
            Data._float = Sweep_potential[i];
            byte_data_send[j] = Data._char[0];
            byte_data_send[j + 1] = Data._char[1];
            byte_data_send[j + 2] = Data._char[2];
            byte_data_send[j + 3] = Data._char[3];
            byte_data_send[j + 4] = VSOCP_arr[i]; 
            j=j+5; 
          }

          Data._float = Sweep_rate;
          byte_data_send[j] = Data._char[0];
          byte_data_send[j + 1] = Data._char[1];
          byte_data_send[j + 2] = Data._char[2];
          byte_data_send[j + 3] = Data._char[3];
          j = j + 4;

          Data._float = Volts_per_point;
          byte_data_send[j] = Data._char[0];
          byte_data_send[j + 1] = Data._char[1];
          byte_data_send[j + 2] = Data._char[2];
          byte_data_send[j + 3] = Data._char[3];
          j = j + 4;

          byte_data_send[j] = current_range;
          byte_data_send[j + 1] = auto_range;
          j=j+2;

          byte_data_send[j] = WE_number;
          j=j+1;

          Data._float = Min_autocurrent_range;
          byte_data_send[j] = Data._char[0];
          byte_data_send[j + 1] = Data._char[1];
          byte_data_send[j + 2] = Data._char[2];
          byte_data_send[j + 3] = Data._char[3];
          j = j + 4;
          byte_data_send[j] = post_exp_cell_state;
          
           byte_data_send[j + 1] = ETX;
           byte_data_send[j + 2] = 0;
           byte_data_send[j + 3] = CR;

           j=j+4;
           temp=j;


        Serial.println("Sent Data: ");
        for (uint8_t j = 0; j < temp; j++)
        {
          Serial.print(byte_data_send[j], HEX);
          Serial.print(",");
        }
        Serial.println();
        Serial.print("Sending Time(ms) : ");
        Serial.println(millis());
        for (uint8_t i = 0; i < MAX_SRV_CLIENTS; i++)
        {
          if (serverClients[i] && serverClients[i].connected())
          {
            serverClients[i].write(byte_data_send, temp);
            //delay(1)
          }
        }
}

//-------------------------------------------send cyclic voltametry data--------------------------------------------------------//

//-------------------------------------------send normal pulse voltametry data--------------------------------------------------------//

void send_normalpulsevoltametry_data()
{
          uint8_t j = 7;
          Serial.println("sending settings packet");
          byte_data_send[0] = SOH;
          byte_data_send[1] = 0x36;
          byte_data_send[2] = STX;
          byte_data_send[3] = DEVICEID;
          byte_data_send[4] = experiment_no;
          byte_data_send[5] = OCP_measurement_mode;
          byte_data_send[6] = OCP_measurement_period;
        
          Data._float = OCP_measurement_slope;
          byte_data_send[j] = Data._char[0];
          byte_data_send[j + 1] = Data._char[1];
          byte_data_send[j + 2] = Data._char[2];
          byte_data_send[j + 3] = Data._char[3];
          j = j + 4;
        
          Data._float = initial_potential;
          byte_data_send[j] = Data._char[0];
          byte_data_send[j + 1] = Data._char[1];
          byte_data_send[j + 2] = Data._char[2];
          byte_data_send[j + 3] = Data._char[3];
          j = j + 4;
        
          Data._float = initial_potential_time;
          byte_data_send[j] = Data._char[0];
          byte_data_send[j + 1] = Data._char[1];
          byte_data_send[j + 2] = Data._char[2];
          byte_data_send[j + 3] = Data._char[3];
          j = j + 4;
        
          byte_data_send[j] = VSOCP;
          j = j + 1;
        
          for (int i = 0; i < (2); i ++)
          {
            Data._float = Sweep_potential[i];
            byte_data_send[j] = Data._char[0];
            byte_data_send[j + 1] = Data._char[1];
            byte_data_send[j + 2] = Data._char[2];
            byte_data_send[j + 3] = Data._char[3];
            byte_data_send[j + 4] = VSOCP_arr[i]; 
            j=j+5; 
          }

          Data._float = Pulse_height;
          byte_data_send[j] = Data._char[0];
          byte_data_send[j + 1] = Data._char[1];
          byte_data_send[j + 2] = Data._char[2];
          byte_data_send[j + 3] = Data._char[3];
          j = j + 4;

          Data._float = Frequency;
          byte_data_send[j] = Data._char[0];
          byte_data_send[j + 1] = Data._char[1];
          byte_data_send[j + 2] = Data._char[2];
          byte_data_send[j + 3] = Data._char[3];
          j = j + 4;

          byte_data_send[j] = current_range;
          j=j+1;

          byte_data_send[j] = WE_number;
          j=j+1;

          byte_data_send[j] = post_exp_cell_state;
          
          
           byte_data_send[j + 1] = ETX;
           byte_data_send[j + 2] = 0;
           byte_data_send[j + 3] = CR;

           j=j+4;
           temp=j;


        Serial.println("Sent Data: ");
        for (uint8_t j = 0; j < temp; j++)
        {
          Serial.print(byte_data_send[j], HEX);
          Serial.print(",");
        }
        Serial.println();
        Serial.print("Sending Time(ms) : ");
        Serial.println(millis());
        for (uint8_t i = 0; i < MAX_SRV_CLIENTS; i++)
        {
          if (serverClients[i] && serverClients[i].connected())
          {
            serverClients[i].write(byte_data_send, temp);
            //delay(1)
          }
        }
}

//-------------------------------------------send normal pulse voltametry data--------------------------------------------------------//

//-------------------------------------------send square pulse voltametry data--------------------------------------------------------//

void send_squarepulsevoltametry_data()
{
  
          uint8_t j = 7;
          Serial.println("sending settings packet");
          byte_data_send[0] = SOH;
          byte_data_send[1] = 0x36;
          byte_data_send[2] = STX;
          byte_data_send[3] = DEVICEID;
          byte_data_send[4] = experiment_no;
          byte_data_send[5] = OCP_measurement_mode;
          byte_data_send[6] = OCP_measurement_period;
        
          Data._float = OCP_measurement_slope;
          byte_data_send[j] = Data._char[0];
          byte_data_send[j + 1] = Data._char[1];
          byte_data_send[j + 2] = Data._char[2];
          byte_data_send[j + 3] = Data._char[3];
          j = j + 4;
        
          Data._float = initial_potential;
          byte_data_send[j] = Data._char[0];
          byte_data_send[j + 1] = Data._char[1];
          byte_data_send[j + 2] = Data._char[2];
          byte_data_send[j + 3] = Data._char[3];
          j = j + 4;
        
          Data._float = initial_potential_time;
          byte_data_send[j] = Data._char[0];
          byte_data_send[j + 1] = Data._char[1];
          byte_data_send[j + 2] = Data._char[2];
          byte_data_send[j + 3] = Data._char[3];
          j = j + 4;
        
          byte_data_send[j] = VSOCP;
          j = j + 1;
        
          for (int i = 0; i < (2); i ++)
          {
            Data._float = Sweep_potential[i];
            byte_data_send[j] = Data._char[0];
            byte_data_send[j + 1] = Data._char[1];
            byte_data_send[j + 2] = Data._char[2];
            byte_data_send[j + 3] = Data._char[3];
            byte_data_send[j + 4] = VSOCP_arr[i]; 
            j=j+5; 
          }

          Data._float = Pulse_height;
          byte_data_send[j] = Data._char[0];
          byte_data_send[j + 1] = Data._char[1];
          byte_data_send[j + 2] = Data._char[2];
          byte_data_send[j + 3] = Data._char[3];
          j = j + 4;

          Data._float = Frequency;
          byte_data_send[j] = Data._char[0];
          byte_data_send[j + 1] = Data._char[1];
          byte_data_send[j + 2] = Data._char[2];
          byte_data_send[j + 3] = Data._char[3];
          j = j + 4;

          Data._float = Scan_increment;
          byte_data_send[j] = Data._char[0];
          byte_data_send[j + 1] = Data._char[1];
          byte_data_send[j + 2] = Data._char[2];
          byte_data_send[j + 3] = Data._char[3];
          j = j + 4;

          byte_data_send[j] = current_range;
          j=j+1;

          byte_data_send[j] = WE_number;
          j=j+1;

          byte_data_send[j] = post_exp_cell_state;
          
          
           byte_data_send[j + 1] = ETX;
           byte_data_send[j + 2] = 0;
           byte_data_send[j + 3] = CR;

           j=j+4;
           temp=j;


        Serial.println("Sent Data: ");
        for (uint8_t j = 0; j < temp; j++)
        {
          Serial.print(byte_data_send[j], HEX);
          Serial.print(",");
        }
        Serial.println();
        Serial.print("Sending Time(ms) : ");
        Serial.println(millis());
        for (uint8_t i = 0; i < MAX_SRV_CLIENTS; i++)
        {
          if (serverClients[i] && serverClients[i].connected())
          {
            serverClients[i].write(byte_data_send, temp);
            //delay(1)
          }
        }
}

//-------------------------------------------send square pulse voltametry data--------------------------------------------------------//

//-------------------------------------------send chronoOCP data--------------------------------------------------------//

void send_chronoOCP_data()
{
      uint8_t j = 8;
      Serial.println("sending settings packet");
      byte_data_send[0] = SOH;
      byte_data_send[1] = 0x36;
      byte_data_send[2] = STX;
      byte_data_send[3] = DEVICEID;
      byte_data_send[4] = experiment_no;
      byte_data_send[5] = sample_rate;
      byte_data_send[6] = hours;
      byte_data_send[7] = minutes;
      byte_data_send[8] = seconds;
      byte_data_send[9] = WE_number;
      byte_data_send[10] = post_exp_cell_state;
      j=11;
      
       Data._float = WE_Area;
       byte_data_send[j] = Data._char[0];
       byte_data_send[j + 1] = Data._char[1];
       byte_data_send[j + 2] = Data._char[2];
       byte_data_send[j + 3] = Data._char[3];
       j = j + 4;

       Data._float = WE_material_density;
       byte_data_send[j] = Data._char[0];
       byte_data_send[j + 1] = Data._char[1];
       byte_data_send[j + 2] = Data._char[2];
       byte_data_send[j + 3] = Data._char[3];
       j = j + 4;

       Data._float = Equivalent_weight;
       byte_data_send[j] = Data._char[0];
       byte_data_send[j + 1] = Data._char[1];
       byte_data_send[j + 2] = Data._char[2];
       byte_data_send[j + 3] = Data._char[3];
       j = j + 4;
       
       temp=j;

       byte_data_send[temp+1] = ETX;
      byte_data_send[temp+2] = 0;
      byte_data_send[temp+3] = CR;
      temp = temp + 3;
      
       
      Serial.println("Sent Data: ");
      for (uint8_t j = 0; j < temp; j++)
      {
        Serial.print(byte_data_send[j], HEX);
        Serial.print(",");
      }
      Serial.println();
      Serial.print("Sending Time(ms) : ");
      Serial.println(millis());
      for (uint8_t i = 0; i < MAX_SRV_CLIENTS; i++)
      {
        if (serverClients[i] && serverClients[i].connected())
        {
          serverClients[i].write(byte_data_send, temp);
          //delay(1)
        }
      }  
}

//-------------------------------------------send chronoOCP data--------------------------------------------------------//

//-------------------------------------------send chronoVAUX data--------------------------------------------------------//


void send_chronoVaux_data()
{
      uint8_t j = 8;
      Serial.println("sending settings packet");
      byte_data_send[0] = SOH;
      byte_data_send[1] = 0x36;
      byte_data_send[2] = STX;
      byte_data_send[3] = DEVICEID;
      byte_data_send[4] = experiment_no;
      byte_data_send[5] = sample_rate;
      byte_data_send[6] = hours;
      byte_data_send[7] = minutes;
      byte_data_send[8] = seconds;
      byte_data_send[9] = WE_number;
//      byte_data_send[10] = post_exp_cell_state;
      j=10;
       
       temp=j;
       byte_data_send[temp+1] = ETX;
       byte_data_send[temp+2] = 0;
       byte_data_send[temp+3] = CR;
       temp = temp + 3;
       
      Serial.println("Sent Data: ");
      for (uint8_t j = 0; j < temp; j++)
      {
        Serial.print(byte_data_send[j], HEX);
        Serial.print(",");
      }
      Serial.println();
     Serial.print("Sending Time(ms) : ");
      Serial.println(millis());
      for (uint8_t i = 0; i < MAX_SRV_CLIENTS; i++)
      {
        if (serverClients[i] && serverClients[i].connected())
        {
          serverClients[i].write(byte_data_send, temp);
          //delay(1)
        }
      }     
}

//-------------------------------------------send chronoVAUX data--------------------------------------------------------//

//-------------------------------------------send linear pulse voltametry data--------------------------------------------------------//


void send_linearpulsevoltametry_data()
{
          uint8_t j = 7;
          Serial.println("sending settings packet");
          byte_data_send[0] = SOH;
          byte_data_send[1] = 0x36;
          byte_data_send[2] = STX;
          byte_data_send[3] = DEVICEID;
          byte_data_send[4] = experiment_no;
          byte_data_send[5] = OCP_measurement_mode;
          byte_data_send[6] = OCP_measurement_period;
        
          Data._float = OCP_measurement_slope;
          byte_data_send[j] = Data._char[0];
          byte_data_send[j + 1] = Data._char[1];
          byte_data_send[j + 2] = Data._char[2];
          byte_data_send[j + 3] = Data._char[3];
          j = j + 4;
        
          Data._float = initial_potential;
          byte_data_send[j] = Data._char[0];
          byte_data_send[j + 1] = Data._char[1];
          byte_data_send[j + 2] = Data._char[2];
          byte_data_send[j + 3] = Data._char[3];
          j = j + 4;
        
          Data._float = initial_potential_time;
          byte_data_send[j] = Data._char[0];
          byte_data_send[j + 1] = Data._char[1];
          byte_data_send[j + 2] = Data._char[2];
          byte_data_send[j + 3] = Data._char[3];
          j = j + 4;
        
          byte_data_send[j] = VSOCP;
          j = j + 1;
        
          for (int i = 0; i < (2); i ++)
          {
            Data._float = Sweep_potential[i];
            byte_data_send[j] = Data._char[0];
            byte_data_send[j + 1] = Data._char[1];
            byte_data_send[j + 2] = Data._char[2];
            byte_data_send[j + 3] = Data._char[3];
            byte_data_send[j + 4] = VSOCP_arr[i]; 
            j=j+5; 
          }

          Data._float = Sweep_rate;
          byte_data_send[j] = Data._char[0];
          byte_data_send[j + 1] = Data._char[1];
          byte_data_send[j + 2] = Data._char[2];
          byte_data_send[j + 3] = Data._char[3];
          j = j + 4;

          Data._float = Volts_per_point;
          byte_data_send[j] = Data._char[0];
          byte_data_send[j + 1] = Data._char[1];
          byte_data_send[j + 2] = Data._char[2];
          byte_data_send[j + 3] = Data._char[3];
          j = j + 4;

          byte_data_send[j] = current_range;
          byte_data_send[j + 1] = auto_range;
          j=j+2;

          byte_data_send[j] = WE_number;
          j=j+1;

          byte_data_send[j] = post_exp_cell_state;
          
           byte_data_send[j + 1] = ETX;
           byte_data_send[j + 2] = 0;
           byte_data_send[j + 3] = CR;

           j=j+4;
           temp=j;


        Serial.println("Sent Data: ");
        for (uint8_t j = 0; j < temp; j++)
        {
          Serial.print(byte_data_send[j], HEX);
          Serial.print(",");
        }
        Serial.println();
        Serial.print("Sending Time(ms) : ");
        Serial.println(millis());
        for (uint8_t i = 0; i < MAX_SRV_CLIENTS; i++)
        {
          if (serverClients[i] && serverClients[i].connected())
          {
            serverClients[i].write(byte_data_send, temp);
            //delay(1)
          }
        }
}

//-------------------------------------------send linear pulse voltametry data--------------------------------------------------------//

//-------------------------------------------send differential pulse voltametry data--------------------------------------------------------//

void send_differentialpulsevoltametry_data()
{
  
          uint8_t j = 7;
          if(experiment_no == 6) //if normal pulse voltametry data then pulse height value is 0
            Pulse_height = 0.0;
          Serial.println("sending settings packet");
          byte_data_send[0] = SOH;
          byte_data_send[1] = 0x36;
          byte_data_send[2] = STX;
          byte_data_send[3] = DEVICEID;
          byte_data_send[4] = experiment_no;
          byte_data_send[5] = OCP_measurement_mode;
          byte_data_send[6] = OCP_measurement_period;
        
          Data._float = OCP_measurement_slope;
          byte_data_send[j] = Data._char[0];
          byte_data_send[j + 1] = Data._char[1];
          byte_data_send[j + 2] = Data._char[2];
          byte_data_send[j + 3] = Data._char[3];
          j = j + 4;
        
          Data._float = initial_potential;
          byte_data_send[j] = Data._char[0];
          byte_data_send[j + 1] = Data._char[1];
          byte_data_send[j + 2] = Data._char[2];
          byte_data_send[j + 3] = Data._char[3];
          j = j + 4;
        
          Data._float = initial_potential_time;
          byte_data_send[j] = Data._char[0];
          byte_data_send[j + 1] = Data._char[1];
          byte_data_send[j + 2] = Data._char[2];
          byte_data_send[j + 3] = Data._char[3];
          j = j + 4;
        
          byte_data_send[j] = VSOCP;
          j = j + 1;
        
          for (int i = 0; i < (2); i ++)
          {
            Data._float = Sweep_potential[i];
            byte_data_send[j] = Data._char[0];
            byte_data_send[j + 1] = Data._char[1];
            byte_data_send[j + 2] = Data._char[2];
            byte_data_send[j + 3] = Data._char[3];
            byte_data_send[j + 4] = VSOCP_arr[i]; 
            j=j+5; 
          }

          Data._float = Scan_rate;
          byte_data_send[j] = Data._char[0];
          byte_data_send[j + 1] = Data._char[1];
          byte_data_send[j + 2] = Data._char[2];
          byte_data_send[j + 3] = Data._char[3];
          j = j + 4;

          Data._float = Scan_increment;
          byte_data_send[j] = Data._char[0];
          byte_data_send[j + 1] = Data._char[1];
          byte_data_send[j + 2] = Data._char[2];
          byte_data_send[j + 3] = Data._char[3];
          j = j + 4;

          Data._float = Step_time;
          byte_data_send[j] = Data._char[0];
          byte_data_send[j + 1] = Data._char[1];
          byte_data_send[j + 2] = Data._char[2];
          byte_data_send[j + 3] = Data._char[3];
          j = j + 4;
          
          Data._float = Pulse_height;
          byte_data_send[j] = Data._char[0];
          byte_data_send[j + 1] = Data._char[1];
          byte_data_send[j + 2] = Data._char[2];
          byte_data_send[j + 3] = Data._char[3];
          j = j + 4;

          Data._float = Pulse_width;
          byte_data_send[j] = Data._char[0];
          byte_data_send[j + 1] = Data._char[1];
          byte_data_send[j + 2] = Data._char[2];
          byte_data_send[j + 3] = Data._char[3];
          j = j + 4;

          byte_data_send[j] = current_range;
          j=j+1;

          byte_data_send[j] = WE_number;
          j=j+1;

          byte_data_send[j] = post_exp_cell_state;
          j=j+1;
          
           byte_data_send[j] = ETX;
           byte_data_send[j + 1] = 0;
           byte_data_send[j + 2] = CR;

           j=j+3;
           temp=j;

        Serial.println("Sent Data: ");
        for (uint8_t j = 0; j < temp; j++)
        {
          Serial.print(byte_data_send[j], HEX);
          Serial.print(",");
        }
        Serial.println();
        Serial.print("Sending Time(ms) : ");
        Serial.println(millis());
        for (uint8_t i = 0; i < MAX_SRV_CLIENTS; i++)
        {
          if (serverClients[i] && serverClients[i].connected())
          {
            serverClients[i].write(byte_data_send, temp);
            //delay(1)
          }
        }
}

//-------------------------------------------send differential pulse voltametry data--------------------------------------------------------//
