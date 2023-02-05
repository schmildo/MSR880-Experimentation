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

#pragma warning(disable : 4996) //disables worrying about depreciated code (libusb_init/libusb_init_context)... because the context version isnt in the library file.. wtf!

#include <stdio.h>
//#include <stdlib.h>
#include <string>
#include<iostream>

#include "../LibUSB/libusb/libusb.h"

#include <bitset>
#include <iomanip>
#include <ctime>
#include <windows.h>
#include <regex>
#include <sstream>
#include <vector>



#include <array>
//#include <stdint.h>
//#include <chrono>
//#include <thread>


	//Declarations
libusb_context* MyDamnContext = nullptr;
libusb_device** MyDamnDevices = nullptr;
int NumberOfDevices = -1;
int FoundDeviceDescriptor = 42;
libusb_device_handle* MyDamnHandle = nullptr;
libusb_config_descriptor* MyDamnConfigDescriptor = nullptr;

libusb_init_option MyDamnOptions[1];
libusb_device* MyDamnDevice = nullptr;
libusb_device_descriptor MyDamnDescriptor;

libusb_interface_descriptor* interface = nullptr;
libusb_endpoint_descriptor* endpoint = nullptr;


std::string utf16_to_utf8(const std::vector<uint16_t>& utf16) {
	std::string utf8;
	utf8.reserve(utf16.size());
	for (uint16_t c : utf16) {
		if (c < 0x80) {
			utf8.push_back(c & 0x7f);
		}
		else if (c < 0x800) {
			utf8.push_back(0xc0 | (c >> 6));
			utf8.push_back(0x80 | (c & 0x3f));
		}
		else {
			utf8.push_back(0xe0 | (c >> 12));
			utf8.push_back(0x80 | ((c >> 6) & 0x3f));
			utf8.push_back(0x80 | (c & 0x3f));
		}
	}
	return utf8;
}


std::string uint8ToBCDandHEX(uint8_t value) {
	std::stringstream result;
	result << std::hex << std::setfill('0') << std::setw(2) << (int)value;
	return "0x" + result.str();
}

std::string uint16ToBCDandHEX(uint16_t value) {
	std::stringstream result;
	result << std::hex << std::setfill('0') << std::setw(4) << (int)value;
	return "0x" + result.str();
}



std::string getEnumString_DescriptorType(int i)
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
		std::cout<<" NOPE! your enum was out of range"<<std::endl;
		
		exit(0);
	}
}


std::string getEnumString_ClassCode(int i)
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
		std::cout<<" NOPEIES! your enum was out of range"<<std::endl;
		exit(0);
	}
}




static void print_devs(libusb_device** devs)
{
	libusb_device* dev;
	int i = 0, j = 0;
	uint8_t path[8];

	while ((dev = devs[i++]) != nullptr) {
		struct libusb_device_descriptor desc;
		int r = libusb_get_device_descriptor(dev, &desc);
		if (r < 0) {
			std::cout<<"failed to get device descriptor"<<std::endl;
			return;
		}

		std::cout<<"%04x:%04x (bus %d, device %d)",desc.idVendor, desc.idProduct,libusb_get_bus_number(dev), libusb_get_device_address(dev);

		r = libusb_get_port_numbers(dev, path, sizeof(path));
		if (r > 0) {
			std::cout<<" path: %d"<<path[0]<<std::endl;
			for (j = 1; j < r; j++)
				std::cout<<".%d"<<path[j]<<std::endl;
		}
		std::cout<<"\n"<<std::endl;
	}
}

libusb_device_handle *TIM_GetDeviceHandlePointer(libusb_context* USB_Context, uint16_t IDVendor = 0x0802, uint16_t IDProduct = 0x0005)
{
	libusb_device_handle *returnValue;
	returnValue = libusb_open_device_with_vid_pid(USB_Context, IDVendor, 0x0005);
	if (returnValue != nullptr)
	{
		std::cout << "Found handle.." << std::endl;
	}
	else
	{
		std::cout << "Did not find handle.. device probably not plugged in. Exiting safely." << std::endl;
		exit(0);
	}
	return returnValue;
}


void TIM_InitContextPointer(libusb_context* OUT_Context)
{
	//r = libusb_init_context(&MyDamnContext,MyDamnOptions,0);
	int returnValue = libusb_init(&OUT_Context);
	if (returnValue ==0)
	{
		std::cout << "Initialised Context..." << std::endl;
	}
	else
	{
		std::cout << "Could not initialise context. Exiting safely." << std::endl;
		exit(0);
	}
}

int TIM_GetDeviceList(libusb_context* IN_Context, libusb_device** OUT_DeviceList)
{
	ssize_t cnt = libusb_get_device_list(IN_Context, &OUT_DeviceList);
	if (cnt < 0)
	{
		libusb_exit(IN_Context);
		std::cout << "Could not obtain device list. Exiting safely." << std::endl;
		exit(0);
	}
	return (int)cnt;
}

libusb_device* TIM_GetDevicePointer(libusb_device_handle* Devicehandle)
{
	libusb_device* ReturnValue = nullptr;
	ReturnValue = libusb_get_device(Devicehandle);
	if (ReturnValue != nullptr)
	{
		std::cout << "Found device..." << std::endl;
	}
	else
	{
		std::cout << "Did not find device" << std::endl;
		exit(0);
	}
	return ReturnValue;
}


libusb_config_descriptor* TIM_GetActiveConfigDescriptorPointer(libusb_device* Device,libusb_config_descriptor* ConfigDescriptor)
{
	libusb_config_descriptor* ReturnValue = nullptr;
	int success = libusb_get_active_config_descriptor(Device, &ConfigDescriptor);
	if (success==0)
	{
		std::cout << "Found device config descriptor" << std::endl;
		ReturnValue = ConfigDescriptor;
	}
	else
	{
		std::cout << "Did not find active config descriptor" << std::endl;
		exit(0);
	}
	return ReturnValue;
}

void TIM_PrintInterfacesAndEndpoints(libusb_config_descriptor* ConfigDesc,const libusb_interface_descriptor* InterfaceDesc,const libusb_endpoint_descriptor* EndpointDesc)
{
	// Loop through the interfaces in the configuration
	for (int i = 0; i < ConfigDesc->bNumInterfaces; i++) {
		InterfaceDesc = &(ConfigDesc->interface[i].altsetting[0]);
		std::cout << "Interface " << i << " has " << (int)InterfaceDesc->bNumEndpoints<< " endpoint(s)" << std::endl;

		// Loop through the endpoints in the interface
		for (int j = 0; j < InterfaceDesc->bNumEndpoints; j++) {
			EndpointDesc = &InterfaceDesc->endpoint[j];
			std::cout << "  Endpoint " << j << " address: 0x" << std::setfill('0') << std::setw(2) << std::hex<<(int)EndpointDesc->bEndpointAddress << std::dec << std::endl;
		}
	}
}



void TIM_SHUTITDOWN()
{
	libusb_release_interface(MyDamnHandle, 0);
	libusb_close(MyDamnHandle);
	libusb_exit(MyDamnContext);
	libusb_free_device_list(MyDamnDevices, 1);
	libusb_exit(nullptr);
}


int TIM_ClaimInterface(libusb_device_handle* Devicehandle)
{
	
	int ReturnValue = libusb_claim_interface(Devicehandle, 0);
	if (ReturnValue < 0) {
		std::cerr << "Error claiming interface: " << libusb_error_name(ReturnValue) << std::endl;
		TIM_SHUTITDOWN();
		
		return 1;
	}
	else
	{
		std::cout << "Claimed interface succesfullly..." << std::endl;
	}

	return ReturnValue;
}


int main(void)
{


	//Initialisations
	TIM_InitContextPointer(MyDamnContext);
	NumberOfDevices = TIM_GetDeviceList(MyDamnContext, MyDamnDevices);
	MyDamnHandle = TIM_GetDeviceHandlePointer(MyDamnContext);
	MyDamnDevice = TIM_GetDevicePointer(MyDamnHandle);
	MyDamnConfigDescriptor = TIM_GetActiveConfigDescriptorPointer(MyDamnDevice, MyDamnConfigDescriptor);
	TIM_PrintInterfacesAndEndpoints(MyDamnConfigDescriptor, interface,endpoint);
	
	
	FoundDeviceDescriptor = libusb_get_device_descriptor(MyDamnDevice, &MyDamnDescriptor);
	
	
	
	
	
	
	
	int num_interfaces = (int)MyDamnDescriptor.bNumConfigurations;
	std::cout << "SLURRY: Number of interfaces: " << num_interfaces << std::endl;

	if (FoundDeviceDescriptor == 0)
	{
		/***************************************/

		unsigned char manufacturer[256];
		int length = libusb_get_string_descriptor_ascii(
			MyDamnHandle,
			MyDamnDescriptor.iManufacturer,
			manufacturer, sizeof(manufacturer));
		if (length < 0) {
			std::cerr << "Error getting string descriptor: " << libusb_error_name(length) << std::endl;
			libusb_close(MyDamnHandle);
			libusb_exit(MyDamnContext);
			return 1;
		}

		std::cout << "Manufacturer: " << manufacturer << std::endl;
		/***************************************/

		unsigned char Product[256];
		length = libusb_get_string_descriptor_ascii(
			MyDamnHandle,
			MyDamnDescriptor.iProduct,
			Product, sizeof(Product));
		if (length < 0) {
			std::cerr << "Error getting string descriptor: " << libusb_error_name(length) << std::endl;
			libusb_close(MyDamnHandle);
			libusb_exit(MyDamnContext);
			return 1;
		}

		std::cout << "Product: " << Product << std::endl;
		/***************************************/
		unsigned char Serial[256];
		length = libusb_get_string_descriptor_ascii(
			MyDamnHandle,
			MyDamnDescriptor.iSerialNumber,
			Serial, sizeof(Serial));
		if (length < 0) {
			std::cerr << "Error getting string descriptor: " << libusb_error_name(length) << std::endl;
			libusb_close(MyDamnHandle);
			libusb_exit(MyDamnContext);
			return 1;
		}

		std::cout << "Serial: " << Serial << std::endl;
		//sizeof(manufacturer)

				/***************************************/

		libusb_set_debug(MyDamnContext, LIBUSB_LOG_LEVEL_WARNING);
		

		std::vector<uint16_t> MyDamnSerialData(256);
		
		unsigned char* ptr = reinterpret_cast<unsigned char*>(MyDamnSerialData.data());
		std::cout << "ptr: " << static_cast<void*>(ptr) << std::endl;


		length = libusb_get_string_descriptor(MyDamnHandle,
			MyDamnDescriptor.iSerialNumber,
			0x0409, ptr, MyDamnSerialData.size());
		if (length < 0) {
			std::cerr << "Error getting string descriptor: " << libusb_error_name(length) << std::endl;
			libusb_close(MyDamnHandle);
			libusb_exit(MyDamnContext);
			return 1;
		}

		std::cout << "Serial number: " << utf16_to_utf8(MyDamnSerialData);


		/***************************************/

		std::cout<<"Found device descriptor..."<<std::endl;
		//std::cout <<"0x" << std::setfill('0') << std::setw(2) << std::hex << +a << " -- ";
		//std::cout<<"(bus %d, device %d)\n" << std::setfill('0') << std::setw(2) << std::hex <<libusb_get_bus_number(MyDamnDevice)<< libusb_get_device_address(MyDamnDevice);

		
		
		std::cout<<"	(bus: " << uint8ToBCDandHEX(libusb_get_bus_number(MyDamnDevice)) <<" Device: "<< uint8ToBCDandHEX(libusb_get_device_address(MyDamnDevice))<<std::endl;
		std::cout<<"	SerialNumber:					" << uint8ToBCDandHEX(MyDamnDescriptor.iSerialNumber)<<std::endl;
		std::cout<<"	BCD Device:					"<< uint16ToBCDandHEX(MyDamnDescriptor.bcdDevice) << std::endl; // Device release number - USB Specification Release Number in Binary - Coded Decimal(i.e., 2.10 is 210h). This field identifies the release of the USB Specification with which the device and its descriptors are compliant.
		std::cout<<"	BCD USB:					"<< uint16ToBCDandHEX(MyDamnDescriptor.bcdUSB)<< std::endl;
		std::cout<<"	DescriptorType:					"<<getEnumString_DescriptorType(MyDamnDescriptor.bDescriptorType)<<std::endl;
		std::cout<<"	DeviceClass:					" <<getEnumString_ClassCode(MyDamnDescriptor.bDeviceClass) << std::endl;
		std::cout<<"	Device Protocol:				" << uint8ToBCDandHEX(MyDamnDescriptor.bDeviceProtocol)<< std::endl;
		std::cout<<"	DeviceSubclass:					" << getEnumString_ClassCode(MyDamnDescriptor.bDeviceSubClass) << std::endl;
		std::cout<<"	Length (bytes):					" << (int)MyDamnDescriptor.bLength << std::endl;
		std::cout<<"	Max packet size (for endpoint 0):		" << (int)MyDamnDescriptor.bMaxPacketSize0 << std::endl;
		std::cout<<"	Number of configurations:			" << (int)MyDamnDescriptor.bNumConfigurations << std::endl;
		std::cout<<"	Product ID:					" << uint16ToBCDandHEX(MyDamnDescriptor.idProduct) << std::endl;
		std::cout<<"	Vendor ID:					" << uint16ToBCDandHEX(MyDamnDescriptor.idVendor) << std::endl;
		std::cout<<"	Manufacturer: (index)				" << (int)MyDamnDescriptor.iManufacturer << std::endl;
		std::cout<<"	Product: (index)				" << (int)MyDamnDescriptor.iProduct << std::endl;
		std::cout<<"	Serial number: (index)				"<<(int)MyDamnDescriptor.iSerialNumber << std::endl;
		int shit;
		//shit = libusb_descriptor_type::LIBUSB_DT_BOS;
		char* MyDamnData;
		//libusb_get_descriptor(MyDamnHandle, libusb_descriptor_type::LIBUSB_DT_STRING, MyDamnDescriptor.iManufacturer, MyDamnData, 2);

		int *int_Config =nullptr;
		int balls = 1;
		int_Config = &balls;
		int int_FoundConfig = 42;
		int shesh = 42;
		libusb_config_descriptor* flaps;
		//flaps = &MyDamnConfigDescriptor;
		//int_FoundConfig = libusb_get_config_descriptor(MyDamnDevice, 0, configDescriptor);
		int_FoundConfig = libusb_get_configuration(MyDamnHandle, int_Config);

		
		if (int_FoundConfig == 0)
		{
			std::cout << "int_Config:		" << *int_Config << std::endl;
			std::cout<<"pretest"<<std::endl;
			shesh = libusb_get_config_descriptor(MyDamnDevice, 0,&MyDamnConfigDescriptor);
			std::cout << "posttest" << std::endl;
			
			//shesh = libusb_get_active_config_descriptor(MyDamnDevice, &MyDamnConfigDescriptor);
			
			if (shesh == 0)
			{
				std::cout<<"%i	ActiveConfigDescriptor - Config Value:"<<MyDamnConfigDescriptor->bConfigurationValue<<std::endl;
				std::cout<<"%i	ActiveConfigDescriptor - Descriptor Type:" << MyDamnConfigDescriptor->bDescriptorType << std::endl;
				std::cout<<"%i	ActiveConfigDescriptor - length:" << MyDamnConfigDescriptor->bLength << std::endl;
				std::cout<<"%i	ActiveConfigDescriptor - attributes:" << MyDamnConfigDescriptor->bmAttributes << std::endl;
				std::cout<<"%i	ActiveConfigDescriptor - num interfaces:" << uint8ToBCDandHEX(MyDamnConfigDescriptor->bNumInterfaces) << std::endl;
				//std::cout<<"%i	ActiveConfigDescriptor - extra:" << MyDamnConfigDescriptor->extra << std::endl;
				//std::cout<<"%i	ActiveConfigDescriptor - extra length:" << MyDamnConfigDescriptor->extra_length << std::endl;
				//std::cout<<"%i	ActiveConfigDescriptor - iconfiguration:" << MyDamnConfigDescriptor->iConfiguration << std::endl;
				//std::cout<<"%i	ActiveConfigDescriptor - maxpower:" << MyDamnConfigDescriptor->MaxPower << std::endl;
				//std::cout<<"%i	ActiveConfigDescriptor - totallength:" << MyDamnConfigDescriptor->wTotalLength << std::endl;

				//std::cout<<"%i	ActiveConfigDescriptor - interface num altsetting:\n", MyDamnConfigDescriptor.interface->num_altsetting);

			}
			else
			{
				std::cout<<"awwwww:   %s   %s"<<libusb_error_name(shesh)<<"  "<<libusb_strerror(shesh)<<std::endl;
				libusb_error_name(shesh);
				exit(0);
			}
			
		}
		else
		{
			exit(0);
		}



		//std::cout<<"Found device configuration...\n"<<std::endl;
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
		std::cout<<"Did not find device descriptor"<<std::endl;
		exit(0);
	}


	/*************************/
		// Harcoded, got from Wireshark capture of doing a Magnetic stripe read

		std::array<uint8_t, 8> magstripeReadReq_1 = { 0x1b, 0x61, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }; //goes green
		std::array<uint8_t, 8> magstripeReadReq_2 = { 0x1b, 0x72, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }; //goes blue
		int rc = 0;
		int* bytes_transferred_cnt = nullptr;
		rc = libusb_interrupt_transfer(MyDamnHandle, 0x01, magstripeReadReq_1.data(), magstripeReadReq_1.size(), bytes_transferred_cnt, 0);

		if (rc != libusb_error::LIBUSB_SUCCESS)
		{
			std::cerr << "ERROR: Couldn't send magstripe read sequence initiation command, rc=" << rc << std::endl;
			return -1;
		}
		else
		{
			std::cout << "holy crap it worked." << std::endl;
		}



		//02 00 02 0c 01 0f 00 00
		std::array<uint8_t, 8> DeviceVersionNumber = { 0x02, 0x00, 0x02, 0x0c, 0x01, 0x0f , 0x00 , 0x00 };
		rc = libusb_interrupt_transfer(MyDamnHandle, 0x01, DeviceVersionNumber.data(), DeviceVersionNumber.size(), bytes_transferred_cnt, 0);

		if (rc != libusb_error::LIBUSB_SUCCESS)
		{
			std::cerr << "ERROR: Couldn't send magstripe read sequence initiation command, rc=" << rc << std::endl;
			return -1;
		}
		else
		{
			std::cout << "holy crap it worked.  :" << DeviceVersionNumber.size()<<std::endl;
		}


		std::array<uint8_t, 8> response;
		unsigned char MyDamnResponse[256];
		// Receive the response from endpoint 0x81
		rc = libusb_interrupt_transfer(MyDamnHandle, 0x82, MyDamnResponse, sizeof(MyDamnResponse), bytes_transferred_cnt, 0);
		if (rc != 0) {
			std::cerr << "Error receiving interrupt transfer: " << libusb_error_name(rc)<< std::endl;
		}
		else {
			std::cout << "Interrupt transfer received successfully, "<< bytes_transferred_cnt << " bytes transferred. >>>>:" << MyDamnResponse<<std::endl;
		}
	/********************/

	
	TIM_SHUTITDOWN();
	
	return 0;
}
