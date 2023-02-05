/*
 * libusb example program to list devices on the bus
 * Copyright © 2007 Daniel Drake <dsd@gentoo.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <stdio.h>

#include "libusb.h"




char* getEnumString_DescriptorType(int i)
{
	switch (i) {
	case 0x01:
		return "LIBUSB_DT_DEVICE";
		break;
	case 0x02:
		return "LIBUSB_DT_CONFIG";
		break;
	case 0x03:
		return "LIBUSB_DT_STRING";
		break;
	case 0x04:
		return "LIBUSB_DT_INTERFACE";
		break;
	case 0x05:
		return "LIBUSB_DT_ENDPOINT";
		break;
	case 0x0f:
		return "LIBUSB_DT_BOS";
		break;
	case 0x10:
		return "LIBUSB_DT_DEVICE_CAPABILITY";
		break;
	case 0x21:
		return "LIBUSB_DT_HID";
		break;
	case 0x22:
		return "LIBUSB_DT_REPORT";
		break;
	case 0x23:
		return "LIBUSB_DT_PHYSICAL";
		break;
	case 0x29:
		return "LIBUSB_DT_HUB";
		break;
	case 0x2a:
		return "LIBUSB_DT_SUPERSPEED_HUB";
		break;
	case 0x30:
		return "LIBUSB_DT_SS_ENDPOINT_COMPANION";
		break;

	default:
		printf(" NOPE! your enum was out of range");
		exit(0);
	}
}


char* getEnumString_ClassCode(int i)
{
	switch (i) {
	case 0xfe:
		return "LIBUSB_CLASS_APPLICATION";
		break;
	case 0x01:
		return "LIBUSB_CLASS_AUDIO";
		break;
	case 0x02:
		return "LIBUSB_CLASS_COMM";
		break;
	case 0x0d:
		return "LIBUSB_CLASS_CONTENT_SECURITY";
		break;
	case 0x0a:
		return "LIBUSB_CLASS_DATA";
		break;
	case 0xdc:
		return "LIBUSB_CLASS_DIAGNOSTIC_DEVICE";
		break;
	case 0x03:
		return "LIBUSB_CLASS_HID";
		break;
	case 0x09:
		return "LIBUSB_CLASS_HUB";
		break;
	case 0x06:
		return "LIBUSB_CLASS_IMAGE (formerly LIBUSB_CLASS_PTP)";
		break;
	case 0x08:
		return "LIBUSB_CLASS_MASS_STORAGE";
		break;
	case 0xef:
		return "LIBUSB_CLASS_MISCELLANEOUS";
		break;
	case 0x00:
		return "LIBUSB_CLASS_PER_INTERFACE";
		break;
	case 0x0f:
		return "LIBUSB_CLASS_PERSONAL_HEALTHCARE";
		break;
	case 0x05:
		return "LIBUSB_CLASS_PHYSICAL";
		break;
	case 0x07:
		return "LIBUSB_CLASS_PRINTER";
		break;
	case 0x0b:
		return "LIBUSB_CLASS_SMART_CARD";
		break;
	case 0xff:
		return "LIBUSB_CLASS_VENDOR_SPEC";
		break;
	case 0x0e:
		return "LIBUSB_CLASS_VIDEO";
		break;
	case 0xe0:
		return "LIBUSB_CLASS_WIRELESS";
		break;
	default:
		printf(" NOPEIES! your enum was out of range");
		exit(0);
	}
}
	



static void print_devs(libusb_device **devs)
{
	libusb_device *dev;
	int i = 0, j = 0;
	uint8_t path[8]; 

	while ((dev = devs[i++]) != NULL) {
		struct libusb_device_descriptor desc;
		int r = libusb_get_device_descriptor(dev, &desc);
		if (r < 0) {
			fprintf(stderr, "failed to get device descriptor");
			return;
		}

		printf("%04x:%04x (bus %d, device %d)",
			desc.idVendor, desc.idProduct,
			libusb_get_bus_number(dev), libusb_get_device_address(dev));

		r = libusb_get_port_numbers(dev, path, sizeof(path));
		if (r > 0) {
			printf(" path: %d", path[0]);
			for (j = 1; j < r; j++)
				printf(".%d", path[j]);
		}
		printf("\n");
	}
}



int main(void)
{
	libusb_device **devs;
	int r;
	ssize_t cnt;
	libusb_context *MyDamnContext;

	r = libusb_init(&MyDamnContext);
	if (r < 0)
		return r;

	cnt = libusb_get_device_list(MyDamnContext, &devs);
	if (cnt < 0){
		libusb_exit(MyDamnContext);
		return (int) cnt;
	}
	//a
	
	libusb_device_handle* MyDamnHandle = NULL;
	libusb_device* MyDamnDevice = NULL;
	struct libusb_device_descriptor MyDamnDescriptor;
	struct libusb_config_descriptor MyDamnConfigDescriptor;

	MyDamnHandle = libusb_open_device_with_vid_pid(MyDamnContext, 0x0802, 0x0005);
	if (MyDamnHandle != NULL)
	{
		printf("Found handle...\n");
	}
	else
	{
		printf("Did not find handle.. device probably not plugged in.\n");
		exit(0);
	}
	MyDamnDevice = libusb_get_device(MyDamnHandle);

	if (MyDamnDevice != NULL)
	{
		printf("Found device...\n");
	}
	else
	{
		printf("Did not find device");
		exit(0);
	}

	int FoundDeviceDescriptor = 42;
	printf("a\n");
	FoundDeviceDescriptor = libusb_get_device_descriptor(MyDamnDevice, &MyDamnDescriptor);
	printf("b\n");
	
	if (FoundDeviceDescriptor == 0)
	{
		printf("Found device descriptor...\n");
		printf("(bus %d, device %d)\n", libusb_get_bus_number(MyDamnDevice), libusb_get_device_address(MyDamnDevice));
		printf("SerialNumber:		%c\n", MyDamnDescriptor.iSerialNumber);
		printf("BCD Device:			0x%04x\n", MyDamnDescriptor.bcdDevice);
		printf("BCD USB:			0x%04x\n", MyDamnDescriptor.bcdUSB);
		printf("DescriptorType:		%s\n", getEnumString_DescriptorType(MyDamnDescriptor.bDescriptorType));
		printf("DeviceClass:		%s\n", getEnumString_ClassCode(MyDamnDescriptor.bDeviceClass));
		printf("Device Protocol:	%04x\n", MyDamnDescriptor.bDeviceProtocol);
		printf("DeviceSubclass:		%s\n", getEnumString_ClassCode(MyDamnDescriptor.bDeviceSubClass));
		printf("Length:				%i bytes?\n", MyDamnDescriptor.bLength);
		printf("Max packet size:	%i bytes?\n", MyDamnDescriptor.bMaxPacketSize0);
		printf("Number of configurations:	%i\n", MyDamnDescriptor.bNumConfigurations);
		printf("Product ID:			0x%04x\n", MyDamnDescriptor.idProduct);
		printf("Vendor ID:			0x%04x\n", MyDamnDescriptor.idVendor);
		printf("Manufacturer: (index)		0x%04x\n", MyDamnDescriptor.iManufacturer);
		printf("Product: (index)			0x%04x\n", MyDamnDescriptor.iProduct);
		printf("Serial number: (index)		0x%04x\n", MyDamnDescriptor.iSerialNumber);
		int shit;
		//shit = libusb_descriptor_type::LIBUSB_DT_BOS;
		char* MyDamnData;
		//libusb_get_descriptor(MyDamnHandle, libusb_descriptor_type::LIBUSB_DT_STRING, MyDamnDescriptor.iManufacturer, MyDamnData, 2);

		int nah = 42;
		int meh = 42;
		int shesh = 42;
		
		//meh = libusb_get_config_descriptor(MyDamnDevice, 0, configDescriptor);
		meh = libusb_get_configuration(MyDamnHandle, &nah);
		
		if (meh == 0)
		{
			printf("%04x		:	x\n",nah);
			printf("pretest\n");
			//shesh = libusb_get_config_descriptor(MyDamnDevice, nah, &MyDamnConfigDescriptor);
			shesh = libusb_get_active_config_descriptor(MyDamnDevice, &MyDamnConfigDescriptor);
			printf("posttest:\n");
			if (shesh == 0)
			{
				printf("%i	ActiveConfigDescriptor - Config Value:\n", MyDamnConfigDescriptor.bConfigurationValue);
				printf("%i	ActiveConfigDescriptor - Descriptor Type:\n", MyDamnConfigDescriptor.bDescriptorType);
				printf("%i	ActiveConfigDescriptor - length:\n", MyDamnConfigDescriptor.bLength);
				printf("%i	ActiveConfigDescriptor - attributes:\n", MyDamnConfigDescriptor.bmAttributes);
				printf("%i	ActiveConfigDescriptor - num interfaces:\n", MyDamnConfigDescriptor.bNumInterfaces);
				printf("%i	ActiveConfigDescriptor - extra:\n", MyDamnConfigDescriptor.extra);
				printf("%i	ActiveConfigDescriptor - extra length:\n", MyDamnConfigDescriptor.extra_length);
				printf("%i	ActiveConfigDescriptor - iconfiguration:\n", MyDamnConfigDescriptor.iConfiguration);
				printf("%i	ActiveConfigDescriptor - maxpower:\n", MyDamnConfigDescriptor.MaxPower);
				printf("%i	ActiveConfigDescriptor - totallength:\n", MyDamnConfigDescriptor.wTotalLength);
				
				//printf("%i	ActiveConfigDescriptor - interface num altsetting:\n", MyDamnConfigDescriptor.interface->num_altsetting);
				
			}
			else
			{
				printf("awwwww:   %s   %s\n", libusb_error_name(shesh), libusb_strerror(shesh));
				libusb_error_name(shesh);
				exit(0);
			}
		}
		else {
			exit(0);
		}

		 

			//printf("Found device configuration...\n");
		//libusb_fill_interrupt_transfer()
		//  
		//  //
		//  
		//  
		//int LIBUSB_CALL libusb_get_configuration				(libusb_device_handle *dev,			int* config);
		//int LIBUSB_CALL libusb_get_active_config_descriptor	(libusb_device *dev,				struct libusb_config_descriptor** config);
		//int LIBUSB_CALL libusb_get_config_descriptor			(libusb_device *dev,				uint8_t config_index, struct libusb_config_descriptor** config);
		//int LIBUSB_CALL libusb_get_config_descriptor_by_value	(libusb_device *dev,				uint8_t bConfigurationValue, struct libusb_config_descriptor** config);
	   //void LIBUSB_CALL libusb_free_config_descriptor			(struct libusb_config_descriptor* config);
		//int LIBUSB_CALL libusb_set_configuration				(libusb_device_handle *dev_handle,	int configuration);
	}
	else
	{
		printf("Did not find devicedescriptor");
		exit(0);
	}
	



	libusb_close(MyDamnHandle);
	
	
	//print_devs(devs);
	libusb_free_device_list(devs, 1);

	

	libusb_exit(NULL);
	return 0;
}
