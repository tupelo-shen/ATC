/*
*********************************************************************************************************
*	                                  
*	模块名称 : USB。
*	文件名称 : usbh_usr.c
*	版    本 : V1.0
*	说    明 : 用于打印USB信息
*	修改记录 : 
*		版本号  日期         作者        说明
*		v1.0    2013-05-02  armfly  ST固件库V2.1.0版本。
*
*	Copyright (C), 2013-2014
*   QQ超级群：216681322
*   BLOG: http://blog.sina.com.cn/u/2565749395	
*********************************************************************************************************
*/
#include "usbh_bsp_msc.h"


#define usb_printf	printf


USB_OTG_CORE_HANDLE   USB_OTG_Core;
USBH_HOST             USB_Host;


/*  Points to the DEVICE_PROP structure of current device     */
/*  The purpose of this register is to speed up the execution */

USBH_Usr_cb_TypeDef USR_USBH_MSC_cb =
{
  USBH_USR_Init,
  USBH_USR_DeInit,
  USBH_USR_DeviceAttached,
  USBH_USR_ResetDevice,
  USBH_USR_DeviceDisconnected,
  USBH_USR_OverCurrentDetected,
  USBH_USR_DeviceSpeedDetected,
  USBH_USR_Device_DescAvailable,
  USBH_USR_DeviceAddressAssigned,
  USBH_USR_Configuration_DescAvailable,
  USBH_USR_Manufacturer_String,
  USBH_USR_Product_String,
  USBH_USR_SerialNum_String,
  USBH_USR_EnumerationDone,
  USBH_USR_UserInput,
  USBH_USR_MSC_Application,
  USBH_USR_DeviceNotSupported,
  USBH_USR_UnrecoveredError
};

/**
* @}
*/

/** @defgroup USBH_USR_Private_Constants
* @{
*/
/*--------------- LCD Messages ---------------*/
const uint8_t MSG_HOST_INIT[]        = "> Host Library Initialized\r\n";
const uint8_t MSG_DEV_ATTACHED[]     = "> Device Attached \r\n";
const uint8_t MSG_DEV_DISCONNECTED[] = "> Device Disconnected\r\n";
const uint8_t MSG_DEV_ENUMERATED[]   = "> Enumeration completed \r\n";
const uint8_t MSG_DEV_HIGHSPEED[]    = "> High speed device detected\r\n";
const uint8_t MSG_DEV_FULLSPEED[]    = "> Full speed device detected\r\n";
const uint8_t MSG_DEV_LOWSPEED[]     = "> Low speed device detected\r\n";
const uint8_t MSG_DEV_ERROR[]        = "> Device fault \r\n";

const uint8_t MSG_MSC_CLASS[]        = "> Mass storage device connected\r\n";
const uint8_t MSG_HID_CLASS[]        = "> HID device connected\r\n";
const uint8_t MSG_DISK_SIZE[]        = "> Size of the disk in MBytes: \r\n";
const uint8_t MSG_LUN[]              = "> LUN Available in the device:\r\n";
const uint8_t MSG_ROOT_CONT[]        = "> Exploring disk flash ...\r\n";
const uint8_t MSG_WR_PROTECT[]       = "> The disk is write protected\r\n";
const uint8_t MSG_UNREC_ERROR[]      = "> UNRECOVERED ERROR STATE\r\n";

/**
* @}
*/


/** @defgroup USBH_USR_Private_FunctionPrototypes
* @{
*/

/**
* @}
*/


/** @defgroup USBH_USR_Private_Functions
* @{
*/


/**
* @brief  USBH_USR_Init
*         Displays the message on LCD for host lib initialization
* @param  None
* @retval None
*/
void USBH_USR_Init(void)
{
	static uint8_t startup = 0;

	if(startup == 0 )
	{
		startup = 1;

		#ifdef USE_USB_OTG_HS
			usb_printf("> USB OTG HS MSC Host\r\n");
		#else
			usb_printf("> USB OTG FS MSC Host\r\n");
		#endif
		usb_printf("> USB Host library started.\r\n");
		usb_printf ("     USB Host Library v2.1.0\r\n" );
	}
}

/**
* @brief  USBH_USR_DeviceAttached
*         Displays the message on LCD on device attached
* @param  None
* @retval None
*/
void USBH_USR_DeviceAttached(void)
{
	usb_printf((char *)MSG_DEV_ATTACHED);
}


/**
* @brief  USBH_USR_UnrecoveredError
* @param  None
* @retval None
*/
void USBH_USR_UnrecoveredError (void)
{
	usb_printf((char *)MSG_UNREC_ERROR);
}


/**
* @brief  USBH_DisconnectEvent
*         Device disconnect event
* @param  None
* @retval Staus
*/
void USBH_USR_DeviceDisconnected (void)
{
	usb_printf((char *)MSG_DEV_DISCONNECTED);
}
/**
* @brief  USBH_USR_ResetUSBDevice
* @param  None
* @retval None
*/
void USBH_USR_ResetDevice(void)
{
	/* callback for USB-Reset */
	usb_printf("> USBH_USR_ResetDevice \r\n");
}


/**
* @brief  USBH_USR_DeviceSpeedDetected
*         Displays the message on LCD for device speed
* @param  Device speed
* @retval None
*/
void USBH_USR_DeviceSpeedDetected(uint8_t DeviceSpeed)
{
	if (DeviceSpeed == HPRT0_PRTSPD_HIGH_SPEED)
	{
		usb_printf((char *)MSG_DEV_HIGHSPEED);
	}
	else if(DeviceSpeed == HPRT0_PRTSPD_FULL_SPEED)
	{
		usb_printf((char *)MSG_DEV_FULLSPEED);
	}
	else if(DeviceSpeed == HPRT0_PRTSPD_LOW_SPEED)
	{
		usb_printf((char *)MSG_DEV_LOWSPEED);
	}
	else
	{
		usb_printf((char *)MSG_DEV_ERROR);
	}
}

/**
* @brief  USBH_USR_Device_DescAvailable
*         Displays the message on LCD for device descriptor
* @param  device descriptor
* @retval None
*/
void USBH_USR_Device_DescAvailable(void *DeviceDesc)
{
	USBH_DevDesc_TypeDef *hs;
	hs = DeviceDesc;

	usb_printf("> VID : %04Xh\r\n" , (uint32_t)(*hs).idVendor);
	usb_printf("> PID : %04Xh\r\n" , (uint32_t)(*hs).idProduct);
}

/**
* @brief  USBH_USR_DeviceAddressAssigned
*         USB device is successfully assigned the Address
* @param  None
* @retval None
*/
void USBH_USR_DeviceAddressAssigned(void)
{

}


/**
* @brief  USBH_USR_Conf_Desc
*         Displays the message on LCD for configuration descriptor
* @param  Configuration descriptor
* @retval None
*/
void USBH_USR_Configuration_DescAvailable(USBH_CfgDesc_TypeDef * cfgDesc,
                                          USBH_InterfaceDesc_TypeDef *itfDesc,
                                          USBH_EpDesc_TypeDef *epDesc)
{
	USBH_InterfaceDesc_TypeDef *id;

	id = itfDesc;

	if((*id).bInterfaceClass  == 0x08)
	{
		usb_printf((char *)MSG_MSC_CLASS);
	}
	else if((*id).bInterfaceClass  == 0x03)
	{
		usb_printf((char *)MSG_HID_CLASS);
	}
}

/**
* @brief  USBH_USR_Manufacturer_String
*         Displays the message on LCD for Manufacturer String
* @param  Manufacturer String
* @retval None
*/
void USBH_USR_Manufacturer_String(void *ManufacturerString)
{
	usb_printf("> Manufacturer : %sr\r\n", (char *)ManufacturerString);
}

/**
* @brief  USBH_USR_Product_String
*         Displays the message on LCD for Product String
* @param  Product String
* @retval None
*/
void USBH_USR_Product_String(void *ProductString)
{
	usb_printf("> Product : %s\r\n", (char *)ProductString);
}

/**
* @brief  USBH_USR_SerialNum_String
*         Displays the message on LCD for SerialNum_String
* @param  SerialNum_String
* @retval None
*/
void USBH_USR_SerialNum_String(void *SerialNumString)
{
	usb_printf( "> Serial Number : %s\r\n", (char *)SerialNumString);
}

/**
* @brief  EnumerationDone
*         User response request is displayed to ask application jump to class
* @param  None
* @retval None
*/
void USBH_USR_EnumerationDone(void)
{
	/* Enumeration complete */
	usb_printf((void *)MSG_DEV_ENUMERATED);
}


/**
* @brief  USBH_USR_DeviceNotSupported
*         Device is not supported
* @param  None
* @retval None
*/
void USBH_USR_DeviceNotSupported(void)
{
	usb_printf ("> Device not supported.\r\n");
}

/**
* @brief  USBH_USR_UserInput
*         User Action for application state entry
* @param  None
* @retval USBH_USR_Status : User response for key button
*/
USBH_USR_Status USBH_USR_UserInput(void)
{
#if 1
	/* HOST_ENUMERATION 和 HOST_CLASS_REQUEST
		在枚举成功和类请求之间等待用户输入。
		此处直接返回OK，无需等待。
	*/
	return USBH_USR_RESP_OK;
#else
	USBH_USR_Status usbh_usr_status;

	usbh_usr_status = USBH_USR_NO_RESP;

	#if 0
	/*Key B3 is in polling mode to detect user action */
	if(STM_EVAL_PBGetState(Button_KEY) == RESET)
	{
		usbh_usr_status = USBH_USR_RESP_OK;
	}
	#endif
	return usbh_usr_status;
#endif
}

/**
* @brief  USBH_USR_OverCurrentDetected
*         Over Current Detected on VBUS
* @param  None
* @retval Staus
*/
void USBH_USR_OverCurrentDetected (void)
{
	usb_printf("> Overcurrent detected.\r\n");
}

/**
* @brief  USBH_USR_MSC_Application
*         Demo application for mass storage
* @param  None
* @retval Staus
*/
int USBH_USR_MSC_Application(void)
{
	return 0;
}

/**
* @brief  USBH_USR_DeInit
*         Deint User state and associated variables
* @param  None
* @retval None
*/
void USBH_USR_DeInit(void)
{
	//USBH_USR_ApplicationState = USH_USR_FS_INIT;
}


/**
* @}
*/

/**
* @}
*/

/**
* @}
*/

/**
* @}
*/

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

