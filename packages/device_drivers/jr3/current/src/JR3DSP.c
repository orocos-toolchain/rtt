//----------------------------------------------------------------------------- 
// Includes
#include <asm/io.h>  // for writeb/w/l, readb/w/l
#include <asm/uaccess.h>
#include <linux/module.h>
#include <linux/pci.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/time.h>

#include "JR3DSP.h"

#ifndef CONFIG_PCI
#   error "This driver needs PCI support to be available"
#endif


MODULE_LICENSE("GPL");


//----------------------------------------------------------------------------- 
// The symbols that are available outside this module
EXPORT_SYMBOL (JR3DSP_check_sensor_and_DSP);
EXPORT_SYMBOL (JR3DSP_set_offsets_scaled);
EXPORT_SYMBOL (JR3DSP_set_offsets);
EXPORT_SYMBOL (JR3DSP_get_error_word);
EXPORT_SYMBOL (JR3DSP_get_warning_word);
EXPORT_SYMBOL (JR3DSP_get_transform_nr);
EXPORT_SYMBOL (JR3DSP_isUnits_lbs_inLbs_mils);
EXPORT_SYMBOL (JR3DSP_isUnits_N_dNm_mmX10);
EXPORT_SYMBOL (JR3DSP_isUnits_kgF_kgFcm_mmX10);
EXPORT_SYMBOL (JR3DSP_isUnits_klbs_kinlbs_mils);
EXPORT_SYMBOL (JR3DSP_setUnits_lbs_inLbs_mils);
EXPORT_SYMBOL (JR3DSP_setUnits_N_dNm_mmX10);
EXPORT_SYMBOL (JR3DSP_setUnits_kgF_kgFcm_mmX10);
EXPORT_SYMBOL (JR3DSP_setUnits_klbs_kinlbs_mils);
EXPORT_SYMBOL (JR3DSP_getDataFromFilter0_scaled);
EXPORT_SYMBOL (JR3DSP_getDataFromFilter1_scaled);
EXPORT_SYMBOL (JR3DSP_getDataFromFilter2_scaled);
EXPORT_SYMBOL (JR3DSP_getDataFromFilter3_scaled);
EXPORT_SYMBOL (JR3DSP_getDataFromFilter4_scaled);
EXPORT_SYMBOL (JR3DSP_getDataFromFilter5_scaled);
EXPORT_SYMBOL (JR3DSP_getDataFromFilter6_scaled);
EXPORT_SYMBOL (JR3DSP_getDataFromFilter0);
EXPORT_SYMBOL (JR3DSP_getDataFromFilter1);
EXPORT_SYMBOL (JR3DSP_getDataFromFilter2);
EXPORT_SYMBOL (JR3DSP_getDataFromFilter3);
EXPORT_SYMBOL (JR3DSP_getDataFromFilter4);
EXPORT_SYMBOL (JR3DSP_getDataFromFilter5);
EXPORT_SYMBOL (JR3DSP_getDataFromFilter6);
EXPORT_SYMBOL (JR3DSP_transformCoordinateSystem);
//----------------------------------------------------------------------------- 
// Local defines

#define COPYRIGHT_SIZE     0x18

// All the offsets (see manual pp 7- 18 and pp 47)
#define RAW_CHANNELS       0x0000
#define COPYRIGHT          0x0040
#define RESERVED1          0x0058
#define SHUNTS             0x0060
#define RESERVED2          0x0066
#define DEFAULT_FS         0x0068
#define RESERVED3          0x006e
#define LOAD_ENVELOPE_NUM  0x006f
#define MIN_FULL_SCALE     0x0070
#define RESERVED4          0x0076
#define TRANSFORM_NUM      0x0077
#define MAX_FULL_SCALE     0x0078
#define RESERVED5          0x007e
#define PEAK_ADDRESS       0x007f
#define FULL_SCALE         0x0080
#define OFFSETS            0x0088
#define OFFSET_NUM         0x008e
#define VECT_AXES          0x008f
#define FILTER0            0x0090
#define FILTER1            0x0098
#define FILTER2            0x00a0
#define FILTER3            0x00a8
#define FILTER4            0x00b0
#define FILTER5            0x00b8
#define FILTER6            0x00c0
#define RATE_DATA          0x00c8
#define MINIMUM_DATA       0x00d0
#define MAXIMUM_DATA       0x00d8
#define NEAR_SAT_VALUE     0x00e0
#define SAT_VALUE          0x00e1
#define RATE_ADDRESS       0x00e2
#define RATE_DIVISOR       0x00e3
#define RATE_COUNT         0x00e4
#define COMMAND_WORD2      0x00e5
#define COMMAND_WORD1      0x00e6
#define COMMAND_WORD0      0x00e7
#define COUNT1             0x00e8
#define COUNT2             0x00e9
#define COUNT3             0x00ea
#define COUNT4             0x00eb
#define COUNT5             0x00ec
#define COUNT6             0x00ed
#define ERROR_COUNT        0x00ee
#define COUNT_X            0x00ef
#define WARNINGS           0x00f0
#define ERRORS             0x00f1
#define THRESHOLD_BITS     0x00f2
#define LAST_CRC           0x00f3
#define EEPROM_VER_NO      0x00f4
#define SOFTWARE_VER_NO    0x00f5
#define SOFTWARE_DAY       0x00f6
#define SOFTWARE_YEAR      0x00f7
#define SERIAL_NO          0x00f8
#define MODEL_NO           0x00f9
#define CAL_DAY            0x00fa
#define CAL_YEAR           0x00fb
#define UNITS              0x00fc
#define BITS               0x00fd
#define CHANNELS           0x00fe
#define THICKNESS          0x00ff
#define LOAD_ENVELOPES     0x0100
#define TRANSFORMS         0x0200

#define JR3OFFSET          0x10000
#define JR3VEND_ID         0x1762   /* JR3 Vendor ID */
#define JR3DEV_ID          0x3112   /* 0x3112: Device ID double channel PCI (for 2 devices or 12DoF)
                                     * 0x1111: Device ID single channel PCI
                                     * 0x211n: Device ID double channel Compact PCI */


enum UnitTypes {FORCE, MOMENTS, DISTANCE};


//----------------------------------------------------------------------------- 
// Local variables

static u8* jr3_baseaddr;

static unsigned short jr3_program[] __devinitdata = JR3DSP_PROGRAM;


static struct pci_device_id jr3pci_id_table[] __devinitdata = {
 {
  vendor:JR3VEND_ID,
  device:JR3DEV_ID,
  subvendor:PCI_ANY_ID,
  subdevice:PCI_ANY_ID,
  class:0,
  class_mask:0,
  driver_data:0
 },
 {0, 0, 0, 0, 0, 0, 0}      
};
MODULE_DEVICE_TABLE(pci, jr3pci_id_table);

//----------------------------------------------------------------------------- 
// Local functions

static void __devinit JR3DSP_download_code(u8* jr3, int pnum)
{
	if (jr3)
	{
		int pos = 0;
        
		while ( (pos < (sizeof(jr3_program) / sizeof(unsigned short)) )  && (jr3_program[pos] != 0xffff) )
		{
			int count, addr;
			
			count = jr3_program[pos++];
			addr  = jr3_program[pos++];
		  
			while (count > 0)
			{
				if (addr & 0x4000) 
				{
					/* 16 bit data, not used!! */
					int data;

					data = jr3_program[pos++];
					count--;
					
					printk("jr3 DSP data memory, not tested\n");
					writew(data, jr3 + ( addr + 0x20000 * pnum ) * 4 );
				} 
				else
				{
				    /* Download 24 bit program */
					int data1, data2;
                    
					data1 = jr3_program[pos++];
					data2 = jr3_program[pos++];
					count -= 2;
					
					writew(data1, jr3 + (addr           + 0x20000 * pnum ) * 4 );
					writew(data2, jr3 + (addr + 0x10000 + 0x20000 * pnum ) * 4 );
					
				}
				addr++;
			}
		}
	}
}


int JR3DSP_read_word(unsigned int offset, unsigned int dsp)
// Function to read a word off the DSP.
// This function takes the offset from the manual, multiplies it
// by four and adds JR3OFFSET (as described in the appendix).
// dsp is the DSP number (0, 1, 2 or 3).
{
  return readw(jr3_baseaddr + offset * 4 + JR3OFFSET + 0x80000 * dsp);
}


void JR3DSP_write_word(unsigned int offset, unsigned short data, unsigned int dsp)
// Function to write a word to the DSP.
// This function takes the offset from the manual, multiplies it
// by four and adds JR3OFFSET (as described in the appendix). 
// dsp is the DSP number (0, 1, 2 or 3).
{
  writew(data, jr3_baseaddr + offset * 4 + JR3OFFSET + 0x80000 * dsp);
}


void JR3DSP_print_units(unsigned int type, unsigned int dsp)
// Prints the current units. 'type' defines which of the units is meant
// 0:force, 1:moments, 2:distance
{
  
  switch ( JR3DSP_read_word(UNITS, dsp) )
  {
    case 0:
      switch (type)
      {
        case FORCE:    printk ("lbs"); break;
        case MOMENTS:  printk ("inches*lbs"); break;
        case DISTANCE: printk ("minches"); break;
    };
    break;
    case 1:
      switch (type)
      {
        case FORCE:    printk ("N"); break;
        case MOMENTS:  printk ("Nm*10"); break;
        case DISTANCE: printk ("*100 um"); break;
    };
    break;
    case 2:
      switch (type)
      {
        case FORCE:    printk ("kg"); break;
        case MOMENTS:  printk ("kg*cm"); break;
        case DISTANCE: printk ("*100 um"); break;
    };
    break;
    case 3:
      switch (type)
      {
        case FORCE:    printk ("1000*lbs"); break;
        case MOMENTS:  printk ("1000*inches*lbs"); break;
        case DISTANCE: printk ("1000*inches"); break;
    };
    break;
    default:
      printk("undefined");
      break;
  }
}


void JR3DSP_print_errors(unsigned int dsp)
{
  unsigned short errorWord, mask;
  unsigned int i;

  errorWord = JR3DSP_read_word(ERRORS, dsp);
  mask = 1;
  for (i = 0; i < 16; i++)
  {
    if (errorWord & mask)
    switch (i)
    {
    case 0:
      printk("- Force X saturated\n");
      break;
    case 1:
      printk("- Force Y saturated\n");
      break;
    case 2:
      printk("- Force Z saturated\n");
      break;
    case 3:
      printk("- Moment X saturated\n");
      break;
    case 4:
      printk("- Moment Y saturated\n");
      break;
    case 5:
      printk("- Moment Z saturated\n");
      break;
    case 6: case 7: case 8: case 9:
      break;
    case 10:
      printk("- Memory Error\n");
      break;
    case 11:
      printk("- Sensor change\n");
      break;
    case 12:
      printk("- System busy\n");
      break;
    case 13:
      printk("- Calibration CRC not zero\n");
      break;
    case 14:
      printk("- Watchdog 2\n");
      break;
    case 15:
      printk("- Watchdog 1\n");
      break;
    }
    mask = mask << 1;
  }
}


void JR3DSP_check_sensor_and_DSP( unsigned int dsp )
// Checks for copyright, software date and year on the DSP and the eeprom, 
// software version, serial and model number of the sensor (unique identifiers)
// and calibration date of sensor
{
  unsigned char copyrightTxt[COPYRIGHT_SIZE + 1];
  unsigned int i;
  unsigned short w;
  
  for (i = 0x0; i < COPYRIGHT_SIZE; i++)
  { // The bytes seem to be the character with a space, so the 'C' is stored 
    // int he EEPROM of the receiver card as 'C '. The shift gets rid of
	// the space.
    w = JR3DSP_read_word ( COPYRIGHT + i, dsp) >> 8;
    copyrightTxt[i] = w;
  }

  // Set the final char to 0. If everything is ok, it should be no problem
  // (the last char of the copyright statement is a '0'), but if an error
  // should occur, this solves the problem of an unterminated string
  copyrightTxt[i]=0;
  printk ("\n\n%s\n", copyrightTxt);

  // Print some DSP information
  printk("DSP nr %i\n", dsp);
  printk("Software version nr: %d.%02d (released on day %d of %d)\n",
    JR3DSP_read_word ( SOFTWARE_VER_NO, dsp )/100,
    JR3DSP_read_word ( SOFTWARE_VER_NO, dsp )%100,
    JR3DSP_read_word ( SOFTWARE_DAY, dsp ), JR3DSP_read_word ( SOFTWARE_YEAR, dsp ));

  // Print some information from the connected sensor
  printk("Information from the attached sensor:\n");
  printk("  Eeprom version nr:   %d\n",
    JR3DSP_read_word ( EEPROM_VER_NO, dsp ));
  printk("  Serial nr:           %d\n",
    JR3DSP_read_word ( SERIAL_NO, dsp ));
  printk("  Model nr:            %d\n",
    JR3DSP_read_word ( MODEL_NO, dsp ));
  printk("  Thickness:           %d ", JR3DSP_read_word ( THICKNESS, dsp ));
  JR3DSP_print_units(DISTANCE, dsp ); printk("\n");
  printk("  Calibration:         day %d of %d\n",
    JR3DSP_read_word ( CAL_DAY, dsp ), JR3DSP_read_word ( CAL_YEAR, dsp ));

  if (JR3DSP_read_word(ERRORS, dsp) != 0)
  {
    printk("There seem to be errors!\n");
    printk("Error code : 0x%x\n", JR3DSP_read_word(ERRORS, dsp));
    JR3DSP_print_errors(dsp);
    printk("Check the manual for further explanation.\n");
  }
}


void JR3DSP_getDataFromFilter(struct ForceArrayScaled* data, 
                              unsigned int filteroffset,
							  unsigned int dsp)
// Function to get the data from a filter into the array. The filter is defined
// by its offset
{
  data->Fx =  JR3DSP_read_word ( filteroffset + 0, dsp );
  data->Fy =  JR3DSP_read_word ( filteroffset + 1, dsp );
  data->Fz =  JR3DSP_read_word ( filteroffset + 2, dsp );
  data->Tx =  JR3DSP_read_word ( filteroffset + 3, dsp );
  data->Ty =  JR3DSP_read_word ( filteroffset + 4, dsp );
  data->Tz =  JR3DSP_read_word ( filteroffset + 5, dsp );
  data->V1 =  JR3DSP_read_word ( filteroffset + 6, dsp );
  data->V2 =  JR3DSP_read_word ( filteroffset + 7, dsp );
}
#ifdef DEBUG
unsigned int JR3counter;
#endif

void JR3DSP_getDataFromFilterAndRescale(struct ForceArray* data,
                                        unsigned int filteroffset,
										unsigned int dsp)
// Function to get the data from a filter into the array and also take
// the fullscale into consideration. The filter is defined
// by its offset
{
  struct ForceArrayScaled m;

  // Read the measurement of the requested filter
  JR3DSP_getDataFromFilter(&m, filteroffset, dsp);

  // Take the scaling factor into account to attain the float values
  data->Fx = (float)m.Fx * (float)(JR3DSP_read_word (FULL_SCALE + 0, dsp)) / 16384.0;
  data->Fy = (float)m.Fy * (float)(JR3DSP_read_word (FULL_SCALE + 1, dsp)) / 16384.0;
  data->Fz = (float)m.Fz * (float)(JR3DSP_read_word (FULL_SCALE + 2, dsp)) / 16384.0;
  data->Tx = (float)m.Tx * (float)(JR3DSP_read_word (FULL_SCALE + 3, dsp)) / 16384.0;
  data->Ty = (float)m.Ty * (float)(JR3DSP_read_word (FULL_SCALE + 4, dsp)) / 16384.0;
  data->Tz = (float)m.Tz * (float)(JR3DSP_read_word (FULL_SCALE + 5, dsp)) / 16384.0;
  
#ifdef DEBUG
  JR3counter++;
  if (JR3counter == 100)
  {
      JR3counter = 0;
      printk("JR3DSP: Fx: %i, Fy: %i, Fz: %i, Tx: %i, Ty: %i, Tz: %i\n",
              (int)data->Fx, (int)data->Fy, (int)data->Fz, (int)data->Tx, (int)data->Ty, (int)data->Tz);
  }
#endif
}

// ------------------------------------
// A rather specific tranformation: rotation and translation around the
// z-axis; (deg, m)
void JR3DSP_transformCoordinateSystem(double angle, double distance, unsigned int dsp)
{
  short force_units;
  
  // rotate around the z-axis
  JR3DSP_write_word( TRANSFORMS+0, 6, dsp); 
  // amount calculated using program alignforcesensor
  JR3DSP_write_word( TRANSFORMS+1, (int)(angle*32768.0/180.0), dsp );
  printk("Setting rotation to %d units for dsp %u\n", (int)(angle*32768.0/180.0), dsp );
  // Remember the current units mode
  force_units = JR3DSP_read_word(UNITS, dsp);
  // Set distance units to mmX10
  JR3DSP_write_word(UNITS, 1, dsp);
  // translate along the z-axis
  JR3DSP_write_word( TRANSFORMS+2, 3, dsp); 
  // amount calculated using program alignforcesensor
  // Here, it is assumed that the translation is less than 3.2768 m
  JR3DSP_write_word( TRANSFORMS+3, (int)(distance*-10000.0), dsp );
  // The End
  JR3DSP_write_word( TRANSFORMS+4, 0, dsp);
  
  JR3DSP_write_word(COMMAND_WORD0, 0x0500, dsp); // use transform #0
}

//----------------------------------------------------------------------------- 
// Accessable functions functions
void JR3DSP_set_offsets_scaled(const struct SixAxisArray* offsets, unsigned int dsp)
{
  JR3DSP_write_word(OFFSETS+0, offsets->axis_1, dsp);
  JR3DSP_write_word(OFFSETS+1, offsets->axis_2, dsp);
  JR3DSP_write_word(OFFSETS+2, offsets->axis_3, dsp);
  JR3DSP_write_word(OFFSETS+3, offsets->axis_4, dsp);
  JR3DSP_write_word(OFFSETS+4, offsets->axis_5, dsp);
  JR3DSP_write_word(OFFSETS+5, offsets->axis_6, dsp);

  // And immediately change the offset
  JR3DSP_write_word(COMMAND_WORD0, 0x0700, dsp);
}

void JR3DSP_set_offsets(const struct ForceArray* offsets, unsigned int dsp)
{
  struct SixAxisArray offsets_scaled;
  // First scale to fullscale. I'm counting on the fact that
  // the offsets will not exceed full scale!
  offsets_scaled.axis_1 = (short)(offsets->Fx * 16384.0 / (float)(JR3DSP_read_word (FULL_SCALE + 0, dsp)) );
  offsets_scaled.axis_2 = (short)(offsets->Fy * 16384.0 / (float)(JR3DSP_read_word (FULL_SCALE + 1, dsp)) );
  offsets_scaled.axis_3 = (short)(offsets->Fz * 16384.0 / (float)(JR3DSP_read_word (FULL_SCALE + 2, dsp)) );
  offsets_scaled.axis_4 = (short)(offsets->Tx * 16384.0 / (float)(JR3DSP_read_word (FULL_SCALE + 3, dsp)) );
  offsets_scaled.axis_5 = (short)(offsets->Ty * 16384.0 / (float)(JR3DSP_read_word (FULL_SCALE + 4, dsp)) );
  offsets_scaled.axis_6 = (short)(offsets->Tz * 16384.0 / (float)(JR3DSP_read_word (FULL_SCALE + 5, dsp)) );

  JR3DSP_set_offsets_scaled(&offsets_scaled, dsp);
}

unsigned int JR3DSP_get_transform_nr(unsigned int dsp)
{
  return JR3DSP_read_word(TRANSFORM_NUM, dsp);
}

unsigned short JR3DSP_get_error_word(unsigned int dsp)
{
  return JR3DSP_read_word(ERRORS, dsp);
}

unsigned short JR3DSP_get_warning_word(unsigned int dsp)
{
  return JR3DSP_read_word(WARNINGS, dsp);
}

int JR3DSP_isUnits_lbs_inLbs_mils(unsigned int dsp)
{
  return (JR3DSP_read_word(UNITS, dsp) == 0);
}

int JR3DSP_isUnits_N_dNm_mmX10(unsigned int dsp)
{
  return (JR3DSP_read_word(UNITS, dsp) == 1);
}

int JR3DSP_isUnits_kgF_kgFcm_mmX10(unsigned int dsp)
{
  return (JR3DSP_read_word(UNITS, dsp) == 2);
}

int JR3DSP_isUnits_klbs_kinlbs_mils(unsigned int dsp)
{
  return (JR3DSP_read_word(UNITS, dsp) == 3);
}


void JR3DSP_setUnits_lbs_inLbs_mils(unsigned int dsp)
{
  JR3DSP_write_word(UNITS, 0, dsp);
}

void JR3DSP_setUnits_N_dNm_mmX10(unsigned int dsp)
{
  JR3DSP_write_word(UNITS, 1, dsp);
}

void JR3DSP_setUnits_kgF_kgFcm_mmX10(unsigned int dsp)
{
  JR3DSP_write_word(UNITS, 2, dsp);
}

void JR3DSP_setUnits_klbs_kinlbs_mils(unsigned int dsp)
{
  JR3DSP_write_word(UNITS, 3, dsp);
}



// -----------------------------------
// Function to get the current offset that the DSP assumes
void JR3DSP_getOffset(struct ForceArrayScaled*data, unsigned int dsp)
{
  data->Fx = JR3DSP_read_word(OFFSETS + 0, dsp);
  data->Fy = JR3DSP_read_word(OFFSETS + 1, dsp);
  data->Fz = JR3DSP_read_word(OFFSETS + 2, dsp);
  data->Tx = JR3DSP_read_word(OFFSETS + 3, dsp);
  data->Ty = JR3DSP_read_word(OFFSETS + 4, dsp);
  data->Tz = JR3DSP_read_word(OFFSETS + 5, dsp);
}
// -----------------------------------
// Functions to get data directly from the filters
void JR3DSP_getDataFromFilter0_scaled(struct ForceArrayScaled* data, unsigned int dsp)
{
  JR3DSP_getDataFromFilter( data, FILTER0, dsp );
}

void JR3DSP_getDataFromFilter1_scaled(struct ForceArrayScaled* data, unsigned int dsp)
{
  JR3DSP_getDataFromFilter( data, FILTER1, dsp );
}

void JR3DSP_getDataFromFilter2_scaled(struct ForceArrayScaled* data, unsigned int dsp)
{
  JR3DSP_getDataFromFilter( data, FILTER2, dsp );
}

void JR3DSP_getDataFromFilter3_scaled(struct ForceArrayScaled* data, unsigned int dsp)
{
  JR3DSP_getDataFromFilter( data, FILTER3, dsp );
}

void JR3DSP_getDataFromFilter4_scaled(struct ForceArrayScaled* data, unsigned int dsp)
{
  JR3DSP_getDataFromFilter( data, FILTER4, dsp );
}

void JR3DSP_getDataFromFilter5_scaled(struct ForceArrayScaled* data, unsigned int dsp)
{
  JR3DSP_getDataFromFilter( data, FILTER5, dsp );
}

void JR3DSP_getDataFromFilter6_scaled(struct ForceArrayScaled* data, unsigned int dsp)
{
  JR3DSP_getDataFromFilter( data, FILTER6, dsp );
}

// -----------------------------------
// Functions to get the data from the filter, but also recalculate to 
// SI units
void JR3DSP_getDataFromFilter0(struct ForceArray* data, unsigned int dsp)
{
  JR3DSP_getDataFromFilterAndRescale( data, FILTER0, dsp );
}

void JR3DSP_getDataFromFilter1(struct ForceArray* data, unsigned int dsp)
{
  JR3DSP_getDataFromFilterAndRescale( data, FILTER1, dsp );
}

void JR3DSP_getDataFromFilter2(struct ForceArray* data, unsigned int dsp)
{
  JR3DSP_getDataFromFilterAndRescale( data, FILTER2, dsp );
}

void JR3DSP_getDataFromFilter3(struct ForceArray* data, unsigned int dsp)
{
  JR3DSP_getDataFromFilterAndRescale( data, FILTER3, dsp );
}

void JR3DSP_getDataFromFilter4(struct ForceArray* data, unsigned int dsp)
{
  JR3DSP_getDataFromFilterAndRescale( data, FILTER4, dsp );
}

void JR3DSP_getDataFromFilter5(struct ForceArray* data, unsigned int dsp)
{
  JR3DSP_getDataFromFilterAndRescale( data, FILTER5, dsp );
}

void JR3DSP_getDataFromFilter6(struct ForceArray* data, unsigned int dsp)
{
  JR3DSP_getDataFromFilterAndRescale( data, FILTER6, dsp );
}




static int __devinit jr3pci_probe(struct pci_dev* dev, const struct pci_device_id* id)
{
	int i;
	printk("JR3dsp: Initializing pci board.\n\n");

	i = pci_enable_device(dev);
	if (i) 
    {
        printk("pci_enable_device returned: %d", i);
        return i;
    }
    //printk("ioremap\n"); 
	jr3_baseaddr = ioremap( pci_resource_start(dev, 0), pci_resource_len(dev, 0) );
    
	//printk("request_mem_region\n");
	if ( request_mem_region( (unsigned long) jr3_baseaddr, pci_resource_len(dev, 0), "jr3pci") == NULL)
	{	
		printk("mem region 0x%x @ 0x%x busy\n\n", (unsigned int) pci_resource_len(dev, 0), (unsigned int)jr3_baseaddr);
		return -EBUSY;
	}

    printk("JR3dsp: Resetting card.\n");
	writew(0, jr3_baseaddr + 0x18000 * 4 );        // reset the board

    printk("JR3dsp: Downloading code for DSP 0.\n");
	JR3DSP_download_code( jr3_baseaddr, 0);
    printk("JR3dsp: Downloading code for DSP 1.\n");
	JR3DSP_download_code( jr3_baseaddr, 1); 
	
	printk("\nJR3dsp: Card initialized.\n\n");
	// The card seems to need some time after a resetting and freshly downloading
	// the code, before it is operational. This waits for 5 seconds and
	// draws nice dots in the mean time to show progress ;)
	//set_current_state(TASK_INTERRUPTIBLE); schedule_timeout( 1 * HZ); 	printk(".");
	//set_current_state(TASK_INTERRUPTIBLE); schedule_timeout( 1 * HZ); 	printk(".");
	//set_current_state(TASK_INTERRUPTIBLE); schedule_timeout( 1 * HZ); 	printk(".");
	//set_current_state(TASK_INTERRUPTIBLE); schedule_timeout( 1 * HZ); 	printk(".");	
	//set_current_state(TASK_INTERRUPTIBLE); schedule_timeout( 1 * HZ); 	printk(".\n\n");
	
	// After waiting, put some general and error info on the screen
	//JR3DSP_check_sensor_and_DSP( 0 );
   	//printk("\n\n");
	//JR3DSP_check_sensor_and_DSP( 1 );
	//printk("\n\n");
	
	return 0;
}



void __devexit jr3pci_remove(struct pci_dev* dev)
{
	printk("jr3pci_remove\n");
	
    printk("release_mem_region\n");
    release_mem_region( (unsigned long) jr3_baseaddr, pci_resource_len(dev, 0));
	
    printk("iounmap\n");
    iounmap(jr3_baseaddr);
    
	printk("pci_disable_device\n");
    pci_disable_device(dev);
}



static struct pci_driver jr3pci_driver =
{
  name:"jr3pci_driver",
  id_table:jr3pci_id_table,
  probe:jr3pci_probe,
  remove:__devexit_p(jr3pci_remove),
  suspend:NULL,
  resume:NULL,
  enable_wake:NULL
};


int __init jr3pci_init_module(void)
{
	//printk("\njr3pci_init_module\n");
	return pci_module_init(&jr3pci_driver);
}


void __exit jr3pci_cleanup_module(void)
{
	//printk("jr3pci_cleanup_module\n");
	pci_unregister_driver(&jr3pci_driver);
}


module_init(jr3pci_init_module);
module_exit(jr3pci_cleanup_module);


