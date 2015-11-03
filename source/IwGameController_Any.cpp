/*
 * (C) 2015 Nick Smith. All Rights Reserved.
 */

#include "IwGameController_Any.h"

namespace IwGameController
{

	bool IsSupported(Type::eType type)
	{
		if (type == Type::IOS)
			return s3eIOSControllerAvailable() == S3E_TRUE;
    
		else if (type == Type::ANDROID_GENERIC || type == Type::ANDROID_ANY)
			return s3eAndroidControllerAvailable() == S3E_TRUE;
    
		else if (type == Type::ANDROID_OUYA_EVERYWHERE)
			return (s3eAndroidControllerAvailable() == S3E_TRUE && s3eAndroidControllerIsTypeSupported(S3E_ANDROIDCONTROLLER_TYPE_OUYA_EVERYWHERE));
    
		else if (type == Type::ANDROID_AMAZON)
			return (s3eAndroidControllerAvailable() == S3E_TRUE && s3eAndroidControllerIsTypeSupported(S3E_ANDROIDCONTROLLER_TYPE_AMAZON));
    
		else if (type == Type::DESKTOP_HID)
			return s3eHidControllerAvailable() == S3E_TRUE;
    
		else if (type == Type::ANY)
		{
			int os = s3eDeviceGetInt(S3E_DEVICE_OS);
        
			switch (os)
			{
				case S3E_OS_ID_IPHONE:
					return s3eIOSControllerAvailable() == S3E_TRUE;
				case S3E_OS_ID_ANDROID:
					return s3eAndroidControllerAvailable() == S3E_TRUE;
				case S3E_OS_ID_WINDOWS:
				case S3E_OS_ID_OSX:
					return s3eHidControllerAvailable() == S3E_TRUE;
				default:
					return false;
			}
		}
		else
			return false;
	}

	CIwGameController* Create(Type::eType type)
	{
		if (type == Type::ANY)
		{
			int os = s3eDeviceGetInt(S3E_DEVICE_OS);

			switch (os)
			{
			case S3E_OS_ID_IPHONE:
            case S3E_OS_ID_TVOS:
				type = Type::IOS;
				break;
			case S3E_OS_ID_ANDROID:
				type = Type::ANDROID_ANY; //will pick up Amazon on Amazon devices etc
				break;
			case S3E_OS_ID_WINDOWS:
			case S3E_OS_ID_OSX:
				type = Type::DESKTOP_HID;
				break;
			default:
				return NULL;
			}
		}

		if (!IsSupported(type))
			return NULL;

		switch (type)
		{
		case Type::IOS:
			return new CIwGameControllerIOS();
			break;
		case Type::ANDROID_ANY:
		case Type::ANDROID_GENERIC:
		case Type::ANDROID_OUYA_EVERYWHERE:
		case Type::ANDROID_AMAZON:
			return new CIwGameControllerAndroid(type);
			break;
		case Type::DESKTOP_HID:
			return new CIwGameControllerDesktopHid();
			break;
		default:
			return NULL;
		}
	}

}   // namespace IwGameController
