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

const libusb_interface_descriptor* MyDamnInterfaceDescriptor = nullptr;
const libusb_interface* MyDamnInterface = nullptr;
const libusb_endpoint_descriptor* MyDamnEndpointDescriptor = nullptr;







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

void CHECKIFBASTARDSAREUSINGUSBWEBAPI()
{
	std::cout << "START USB WEB API CHECKING" << std::endl;
	libusb_device_handle* device_handle;
	libusb_device* device = nullptr;
	struct libusb_config_descriptor* config;
	const struct libusb_interface* interface;
	const struct libusb_interface_descriptor* interface_desc;
	int r;

	r = libusb_get_active_config_descriptor(MyDamnDevice, &config);
	std::cout << "BICKIE port number:" << uint8ToBCDandHEX(libusb_get_port_number(MyDamnDevice)) << std::endl;
	if (r == LIBUSB_SUCCESS) {
		for (int i = 0; i < config->bNumInterfaces; i++) {
			interface = &config->interface[i];
			for (int j = 0; j < interface->num_altsetting; j++) {
				interface_desc = &interface->altsetting[j];
				if (interface_desc->bInterfaceClass == 0x38) {
					// WebUSB interface descriptor found
					// ...
					std::cout << "HOLY SHIT THATS BAD this device has usb web api!" << std::endl;
				}
			}
		}
	}

	

	std::cout << "FINISHED USB WEB API CHECKING" << std::endl;

}
std::string getEnumString_USBSpeed(int i)
{
	switch (i)
	{
	case libusb_speed::LIBUSB_SPEED_FULL:
			return "LIBUSB_SPEED_FULL (12Mb/s)";
			break;
	case libusb_speed::LIBUSB_SPEED_HIGH:
		return "LIBUSB_SPEED_HIGH (480Mb/s)";
			break;
	case libusb_speed::LIBUSB_SPEED_LOW:
		return "LIBUSB_SPEED_LOW (1.5Mb/s)";
		break;
	case libusb_speed::LIBUSB_SPEED_SUPER:
		return "LIBUSB_SPEED_SUPER (5000Mb/s)";
		break;
	case libusb_speed::LIBUSB_SPEED_SUPER_PLUS:
		return "LIBUSB_SPEED_SUPER_PLUS (10000Mb/s)";
		break;
	case libusb_speed::LIBUSB_SPEED_UNKNOWN:
		return "LIBUSB_SPEED_UNKNOWN (???Mb/s)";
		break;
	default:
		std::cout << " NOPE! your enum was out of range" << std::endl;
		exit(0);
	}
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
	libusb_device* dev = nullptr;
	int i = 0, j = 0;
	uint8_t path[8];
	std::cout << "A" << std::endl;
	//std::cout << &devs[0];
	//if (devs[0] != nullptr)std::cout << "first dev is not null" << std::endl;
	/****************/
	for (i = 0; i < 2; i++)
	{
		std::cout << "B" << std::endl;
		struct libusb_device_descriptor Ddescriptor;
		int r = libusb_get_device_descriptor(devs[i], &Ddescriptor);
		std::cout << "%04x:%04x (bus %d, device %d)", Ddescriptor.idVendor, Ddescriptor.idProduct, libusb_get_bus_number(dev), libusb_get_device_address(dev);
	}
	/****************/
	while ((dev = devs[i++]) != nullptr)
	{
		struct libusb_device_descriptor desc;
		int r = libusb_get_device_descriptor(dev, &desc);
		if (r < 0)
		{
			std::cout<<"failed to get device descriptor"<<std::endl;
			return;
		}

		std::cout<<"%04x:%04x (bus %d, device %d)",desc.idVendor, desc.idProduct,libusb_get_bus_number(dev), libusb_get_device_address(dev);

		r = libusb_get_port_numbers(dev, path, sizeof(path));
		if (r > 0)
		{
			std::cout<<" path: %d"<<path[0]<<std::endl;
			for (j = 1; j < r; j++)
				std::cout<<".%d"<<path[j]<<std::endl;
		}
		std::cout<<"\n"<<std::endl;
	}
	std::cout << "Z" << std::endl;
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

int TIM_GetDeviceList(libusb_context* IN_Context, libusb_device**& OUT_DeviceList)
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

void TIM_PrintInterfacesAndEndpoints(libusb_config_descriptor* ConfigDesc)
{
	// Loop through the interfaces in the configuration
	for (int i = 0; i < ConfigDesc->bNumInterfaces; i++) {
		MyDamnInterfaceDescriptor = &(ConfigDesc->interface[i].altsetting[0]);
		std::cout << "Interface " << i << " has " << (int)MyDamnInterfaceDescriptor->bNumEndpoints<< " endpoint(s)" << std::endl;

		// Loop through the endpoints in the interface
		for (int j = 0; j < MyDamnInterfaceDescriptor->bNumEndpoints; j++) {
			MyDamnEndpointDescriptor = &(MyDamnInterfaceDescriptor->endpoint[j]);
			std::cout << "  Endpoint " << j << " address: 0x" << std::setfill('0') << std::setw(2) << std::hex<<(int)MyDamnEndpointDescriptor->bEndpointAddress << std::dec << std::endl;
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
	std::cout << "SAFE EXIT" << std::endl;
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

void TIM_PlayWithDeviceDescriptor()
{

	FoundDeviceDescriptor = libusb_get_device_descriptor(MyDamnDevice, &MyDamnDescriptor);
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
			//return 1;
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
		//	return 1;
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
		//	return 1;
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
			//return 1;
		}

		std::cout << "Serial number: " << utf16_to_utf8(MyDamnSerialData);


		/***************************************/

		std::cout << "Found device descriptor..." << std::endl;
		//std::cout <<"0x" << std::setfill('0') << std::setw(2) << std::hex << +a << " -- ";
		//std::cout<<"(bus %d, device %d)\n" << std::setfill('0') << std::setw(2) << std::hex <<libusb_get_bus_number(MyDamnDevice)<< libusb_get_device_address(MyDamnDevice);



		std::cout << "	(bus: " << uint8ToBCDandHEX(libusb_get_bus_number(MyDamnDevice)) << " Device: " << uint8ToBCDandHEX(libusb_get_device_address(MyDamnDevice)) << std::endl;
		std::cout << "	SerialNumber:					" << uint8ToBCDandHEX(MyDamnDescriptor.iSerialNumber) << std::endl;
		std::cout << "	BCD Device:					" << uint16ToBCDandHEX(MyDamnDescriptor.bcdDevice) << std::endl; // Device release number - USB Specification Release Number in Binary - Coded Decimal(i.e., 2.10 is 210h). This field identifies the release of the USB Specification with which the device and its descriptors are compliant.
		std::cout << "	BCD USB:					" << uint16ToBCDandHEX(MyDamnDescriptor.bcdUSB) << std::endl;
		std::cout << "	DescriptorType:					" << getEnumString_DescriptorType(MyDamnDescriptor.bDescriptorType) << std::endl;
		std::cout << "	DeviceClass:					" << getEnumString_ClassCode(MyDamnDescriptor.bDeviceClass) << std::endl;
		std::cout << "	Device Protocol:				" << uint8ToBCDandHEX(MyDamnDescriptor.bDeviceProtocol) << std::endl;
		std::cout << "	DeviceSubclass:					" << getEnumString_ClassCode(MyDamnDescriptor.bDeviceSubClass) << std::endl;
		std::cout << "	Length (bytes):					" << (int)MyDamnDescriptor.bLength << std::endl;
		std::cout << "	Max packet size (for endpoint 0):		" << (int)MyDamnDescriptor.bMaxPacketSize0 << std::endl;
		std::cout << "	Number of configurations:			" << (int)MyDamnDescriptor.bNumConfigurations << std::endl;
		std::cout << "	Product ID:					" << uint16ToBCDandHEX(MyDamnDescriptor.idProduct) << std::endl;
		std::cout << "	Vendor ID:					" << uint16ToBCDandHEX(MyDamnDescriptor.idVendor) << std::endl;
		std::cout << "	Manufacturer: (index)				" << (int)MyDamnDescriptor.iManufacturer << std::endl;
		std::cout << "	Product: (index)				" << (int)MyDamnDescriptor.iProduct << std::endl;
		std::cout << "	Serial number: (index)				" << (int)MyDamnDescriptor.iSerialNumber << std::endl;
		int shit;
		//shit = libusb_descriptor_type::LIBUSB_DT_BOS;
		char* MyDamnData;
		//libusb_get_descriptor(MyDamnHandle, libusb_descriptor_type::LIBUSB_DT_STRING, MyDamnDescriptor.iManufacturer, MyDamnData, 2);

		int* int_Config = nullptr;
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
			std::cout << "pretest" << std::endl;
			shesh = libusb_get_config_descriptor(MyDamnDevice, 0, &MyDamnConfigDescriptor);
			std::cout << "posttest" << std::endl;

			//shesh = libusb_get_active_config_descriptor(MyDamnDevice, &MyDamnConfigDescriptor);

			if (shesh == 0)
			{
				std::cout << "%i	ActiveConfigDescriptor - Config Value:" << MyDamnConfigDescriptor->bConfigurationValue << std::endl;
				std::cout << "%i	ActiveConfigDescriptor - Descriptor Type:" << MyDamnConfigDescriptor->bDescriptorType << std::endl;
				std::cout << "%i	ActiveConfigDescriptor - length:" << MyDamnConfigDescriptor->bLength << std::endl;
				std::cout << "%i	ActiveConfigDescriptor - attributes:" << MyDamnConfigDescriptor->bmAttributes << std::endl;
				std::cout << "%i	ActiveConfigDescriptor - num interfaces:" << uint8ToBCDandHEX(MyDamnConfigDescriptor->bNumInterfaces) << std::endl;
				//std::cout<<"%i	ActiveConfigDescriptor - extra:" << MyDamnConfigDescriptor->extra << std::endl;
				//std::cout<<"%i	ActiveConfigDescriptor - extra length:" << MyDamnConfigDescriptor->extra_length << std::endl;
				//std::cout<<"%i	ActiveConfigDescriptor - iconfiguration:" << MyDamnConfigDescriptor->iConfiguration << std::endl;
				//std::cout<<"%i	ActiveConfigDescriptor - maxpower:" << MyDamnConfigDescriptor->MaxPower << std::endl;
				//std::cout<<"%i	ActiveConfigDescriptor - totallength:" << MyDamnConfigDescriptor->wTotalLength << std::endl;

				//std::cout<<"%i	ActiveConfigDescriptor - interface num altsetting:\n", MyDamnConfigDescriptor.interface->num_altsetting);

			}
			else
			{
				std::cout << "awwwww:   %s   %s" << libusb_error_name(shesh) << "  " << libusb_strerror(shesh) << std::endl;
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
		std::cout << "Did not find device descriptor" << std::endl;
		exit(0);
	}


}

void TIM_Current(std::array<uint8_t, 8> Data,libusb_device_handle* DeviceHandle, uint8_t Endpoint)
{
	int* bytes_transferred_cnt = nullptr;
	int rc = 0;
	//std::cout << "Size in bytes of the input before:" << Data.size() << std::endl;
	rc = libusb_interrupt_transfer(MyDamnHandle, Endpoint, Data.data(), Data.size(), bytes_transferred_cnt, 0);

	if (rc != libusb_error::LIBUSB_SUCCESS)
	{
		std::cerr << "ERROR: Couldn't send magstripe read sequence initiation command, rc=" << rc << std::endl;
		//return -1;
	}
	else
	{
		//std::cout << "Size in bytes of the input after:" << Data.size() << std::endl;
		if(bytes_transferred_cnt!=nullptr)std::cout << "testicles:" << *bytes_transferred_cnt << std::endl;
	}
}


void TIM_Current2(unsigned char* Data, libusb_device_handle* DeviceHandle, uint8_t Endpoint)
{
	int* bytes_transferred_cnt = nullptr;
	int rc = 0;
	//std::cout << "Size in bytes of the input before:" << Data.size() << std::endl;
	rc = libusb_interrupt_transfer(MyDamnHandle, Endpoint, Data, sizeof(Data), bytes_transferred_cnt, 0);

	if (rc != libusb_error::LIBUSB_SUCCESS)
	{
		std::cerr << "ERROR: Couldn't send magstripe read sequence initiation command, rc=" << rc << std::endl;
		//return -1;
	}
	else
	{
		//std::cout << "Size in bytes of the input after:" << Data.size() << std::endl;
		if (bytes_transferred_cnt != nullptr)std::cout << "testicles:" << *bytes_transferred_cnt << std::endl;
	}
}

void TIM_PlayWithInterrupts()
{

	/*************************/
		// Harcoded, got from Wireshark capture of doing a Magnetic stripe read

	std::array<uint8_t, 8> magstripeReadReq_1 = { 0x1b, 0x61, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }; //goes green
	std::array<uint8_t, 8> magstripeReadReq_2 = { 0x1b, 0x72, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }; //goes blue
	std::array<uint8_t, 8> DeviceVersionNumber = { 0x02, 0x00, 0x02, 0x0c, 0x01, 0x0f , 0x00 , 0x00 };
	std::array<uint8_t, 8> response = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
	unsigned char MyDamnResponse[256];

	//TIM_Current(magstripeReadReq_1,MyDamnHandle,0x01);
	//TIM_Current(magstripeReadReq_2, MyDamnHandle,0x01);
	TIM_Current(DeviceVersionNumber, MyDamnHandle,0x01);
	TIM_Current2(MyDamnResponse, MyDamnHandle, 0x82);
	std::cout << response.data() << std::endl;



	
	/*
	// Receive the response from endpoint 0x81
	int rc = libusb_interrupt_transfer(MyDamnHandle, 0x82, MyDamnResponse, sizeof(MyDamnResponse), bytes_transferred_cnt, 0);
	if (rc != 0) {
		std::cerr << "Error receiving interrupt transfer: " << libusb_error_name(rc) << std::endl;
	}
	else {
		std::cout << "Interrupt transfer received successfully, " << bytes_transferred_cnt << " bytes transferred. >>>>:" << MyDamnResponse << std::endl;
	}
	*/

	/********************/
}

void TIM_PrintWebAPICheck(libusb_device* device)
{
	const libusb_interface* Inteface;
	libusb_config_descriptor* ConfigDesc;
	const libusb_interface_descriptor* InterfaceDesc;
	int s = libusb_get_active_config_descriptor(device, &ConfigDesc);
	if (s == LIBUSB_SUCCESS)
	{
		for (int i = 0; i < ConfigDesc->bNumInterfaces; i++) {
			Inteface = &ConfigDesc->interface[i];
			for (int j = 0; j < Inteface->num_altsetting; j++) {
				InterfaceDesc = &Inteface->altsetting[j];
				if (InterfaceDesc->bInterfaceClass == 0x38) {
					// WebUSB interface descriptor found
					// ...
					std::cout << "HOLY SHIT THATS BAD this device has usb web api!" << std::endl;
				}
			}
		}
	}
}

void TIM_PrintParentDevice(libusb_device* device)
{
	//print parent crap
	if (int(libusb_get_device_address(device)) != 0)
	{
		ParentDevice = libusb_get_parent(device);
		std::cout << "Parent:" << (int)libusb_get_port_number(ParentDevice) << "." << int(libusb_get_device_address(ParentDevice)) << "." << int(libusb_get_bus_number(ParentDevice)) << std::endl;
	}
	else
	{
		//this is a root device
	}	//return ParentDevice;
}

void print_device_info(libusb_device* device) {
	libusb_device_descriptor DeviceDesc;
	libusb_config_descriptor *ConfigDesc;
	const libusb_interface_descriptor *InterfaceDesc;
	const libusb_interface* Inteface;
	libusb_device* ParentDevice;
	

	int ret = libusb_get_device_descriptor(device, &DeviceDesc);
	if (ret != 0) {
		std::cerr << "Error getting device descriptor: "
			<< libusb_error_name(ret) << std::endl;
		return;
	}

	std::cout << "idVendor: 0x" << std::hex << DeviceDesc.idVendor << std::dec << std::endl;
	std::cout << "idProduct: 0x" << std::hex << DeviceDesc.idProduct << std::dec << std::endl;
	std::cout << "Bus number: " << int(libusb_get_bus_number(device)) << std::endl;
	std::cout << "Device address: " << int(libusb_get_device_address(device)) << std::endl;
	std::cout << "Device speed: " << getEnumString_USBSpeed(libusb_get_device_speed(device)) << std::endl;
	
	/*****************************/
	int r = libusb_get_active_config_descriptor(device, &ConfigDesc);
	std::cout << "Port number:" << (int)libusb_get_port_number(device) << std::endl;
	
	//WTF - wireshark calls it a usb.bus_id but in libusb its actually a port number????
	std::cout << "Wireshark Stuff - (port.address.bus): " << (int)libusb_get_port_number(device) << "." << int(libusb_get_device_address(device)) << "." << int(libusb_get_bus_number(device)) << "     -  Filter: usb.bus_id== " << (int)libusb_get_port_number(device) << " and usb.device_address == " << int(libusb_get_device_address(device)) << std::endl;
	
	TIM_PrintParentDevice(device);
	TIM_PrintWebAPICheck(device);
	
}

int main(void)
{

	std::cout << "libusbverion:describe:" << libusb_get_version()->describe << std::endl;
	std::cout << "Major:" << libusb_get_version()->major << std::endl;


	TIM_InitContextPointer(MyDamnContext);
	MyDamnHandle = TIM_GetDeviceHandlePointer(MyDamnContext);
	MyDamnDevice = TIM_GetDevicePointer(MyDamnHandle);

	//print_devs(MyDamnDevices);
	NumberOfDevices = TIM_GetDeviceList(MyDamnContext, MyDamnDevices);
	std:: cout << "numdevcie:" <<NumberOfDevices<< std::endl;



	MyDamnConfigDescriptor = TIM_GetActiveConfigDescriptorPointer(MyDamnDevice, MyDamnConfigDescriptor);
	TIM_PrintInterfacesAndEndpoints(MyDamnConfigDescriptor);
	TIM_PlayWithDeviceDescriptor();
	TIM_PlayWithInterrupts();
	CHECKIFBASTARDSAREUSINGUSBWEBAPI();

	/**********************************************************/


	//libusb_device** device_list;
	ssize_t num_devices;
	// Loop through the list of devices and print the info for each one
	for (ssize_t i = 0; i < NumberOfDevices; i++) {
		std::cout << "Device " << i << ":" << std::endl;
		MyDamnDevice = MyDamnDevices[i];
		print_device_info(MyDamnDevice);
		

	
		std::cout << std::endl;
	}
	


	TIM_SHUTITDOWN();
	
	return 0;
}
