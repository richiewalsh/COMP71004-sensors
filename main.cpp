
/* Includes */
#include "mbed.h"
#include "HTS221Sensor.h"
#include "LPS22HBSensor.h"
#include "LSM6DSLSensor.h"
#include "lis3mdl_class.h"
#include "VL53L0X.h"

/*
Error while trying ascii value of char for scan on repeated attempt
++ MbedOS Error Info ++
Error Status: 0x80010133 Code: 307 Module: 1
Error Message: Mutex: 0x20000B88, Not allowed in ISR context
Location: 0x80130A9
Error Value: 0x20000B88
Current Thread: main Id: 0x20002598 Entry: 0x80102C7 StackSize: 0x1000 StackMem: 0x20000D98 SP: 0x2009FE74 
For more info, visit: https://mbed.com/s/error?error=0x80010133&tgt=B_L4S5I_IOT01A
-- MbedOS Error Info --


*/

// objects for various sensors
static DevI2C devI2c(PB_11,PB_10);
static LPS22HBSensor press_temp(&devI2c);
static HTS221Sensor hum_temp(&devI2c);
static LSM6DSLSensor acc_gyro(&devI2c,0xD4,D4,D5); // high address
static LIS3MDL magnetometer(&devI2c, 0x3C);
static DigitalOut shutdown_pin(PC_6);
static VL53L0X range(&devI2c, &shutdown_pin, PC_7, 0x52);
UnbufferedSerial usb(USBTX, USBRX, 9600);


// functions to print sensor data
void print_t_rh(){
    float value1, value2;
    hum_temp.get_temperature(&value1);
    hum_temp.get_humidity(&value2);

    value1=value2=0;    
    press_temp.get_temperature(&value1);
    press_temp.get_pressure(&value2);
    printf("LPS22HB: [temp] %.2f C, [press] %.2f mbar\r\n", value1, value2);
}

void print_mag(){
    int32_t axes[3];
    magnetometer.get_m_axes(axes);
    printf("LIS3MDL [mag/mgauss]:    %6ld, %6ld, %6ld\r\n", axes[0], axes[1], axes[2]);

}

void print_accel(){
    int32_t axes[3];
    acc_gyro.get_x_axes(axes);
    printf("LSM6DSL [acc/mg]:        %6ld, %6ld, %6ld\r\n", axes[0], axes[1], axes[2]);
}

void print_gyro(){
    int32_t axes[3];
    acc_gyro.get_g_axes(axes);
    printf("LSM6DSL [gyro/mdps]:     %6ld, %6ld, %6ld\r\n", axes[0], axes[1], axes[2]);
}

void print_distance(){
    uint32_t distance;
    int status = range.get_distance(&distance);
    if (status == VL53L0X_ERROR_NONE) {
        printf("VL53L0X [mm]:            %6ld\r\n", distance);
    } else {
        printf("VL53L0X [mm]:                --\r\n");
    }
}

/* Simple main function */
int main() {
    char c;

    uint8_t id;
    float value1, value2;

    int32_t axes[3];

    hum_temp.init(NULL);

    press_temp.init(NULL);
    magnetometer.init(NULL);
    acc_gyro.init(NULL);

    range.init_sensor(0x52);

    hum_temp.enable();
    press_temp.enable();

    acc_gyro.enable_x();
    acc_gyro.enable_g();
  
    printf("\033[2J\033[20A");
    printf ("\r\n--- Starting new run ---\r\n\r\n");

    hum_temp.read_id(&id);
    printf("HTS221  humidity & temperature    = 0x%X\r\n", id);

    press_temp.read_id(&id);
    printf("LPS22HB pressure & temperature    = 0x%X\r\n", id);
    magnetometer.read_id(&id);
    printf("LIS3MDL magnetometer              = 0x%X\r\n", id);
    acc_gyro.read_id(&id);
    printf("LSM6DSL accelerometer & gyroscope = 0x%X\r\n", id);
    
    printf("\n\r--- Reading sensor values ---\n\r"); ;
    print_t_rh();
    print_mag();
    print_accel();
    print_gyro();
    print_distance();
    printf("\r\n");
    
    while(1) {
        printf("Choice menu\r\n1: accel\r\n2: dist\r\n3: gyro\r\n4: mag\r\n5: temp+pres\r\n");
        printf("Enter valid number: \r\n");
    scanf("%c", &c);
    //printf("%d\r\n", c);
    if(c == 97){
        usb.attach(&print_accel);
    }
    else if (c == 100) {
        usb.attach(&print_distance);
    }
    else if(c == 103){
        usb.attach(&print_gyro);
    }
    else if(c == 109){
        usb.attach(&print_mag);
    }
    else if(c == 116){
        usb.attach(&print_t_rh);
    }
    else{
        printf("Invalid choice. Restart program to try again.\r\n");
        //kill loop in case of bad choice
        return 0;
    }
        wait_us(500000);
    }
}