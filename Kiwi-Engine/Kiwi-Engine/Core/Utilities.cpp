#include "Utilities.h"

#include "../Graphics/DirectX.h"

#include <Windows.h>

namespace Kiwi
{

	std::wstring s2ws(const std::string& s1)
	{
		int size_needed = MultiByteToWideChar(CP_UTF8, 0, &s1[0], (int)s1.size(), NULL, 0);
		std::wstring wstr( size_needed, 0 );
		MultiByteToWideChar(CP_UTF8, 0, &s1[0], (int)s1.size(), &wstr[0], size_needed);
		return wstr;
	}

	std::wstring c2ws(const char cString[], int length)
	{
		int size_needed = MultiByteToWideChar(CP_UTF8, 0, cString, length, NULL, 0);
		std::wstring wstr( size_needed, 0 );
		MultiByteToWideChar(CP_UTF8, 0, cString, length, &wstr[0], size_needed);
		return wstr;
	}

	//takes the hresult error from a D3D call and converts it to a string representation
	std::wstring GetD3DErrorString(HRESULT hr)
	{

		std::wstring error;
		switch (hr)
		{
			case D3D11_ERROR_FILE_NOT_FOUND: 
			{
				//The file was not found.
				error = L"D3D11_ERROR_FILE_NOT_FOUND";
				break;
			}
			case D3D11_ERROR_TOO_MANY_UNIQUE_STATE_OBJECTS: 
			{
				//There are too many unique instances of a particular type of state object.
				error = L"D3D11_ERROR_TOO_MANY_UNIQUE_STATE_OBJECTS";
				break;
			}
			case D3D11_ERROR_TOO_MANY_UNIQUE_VIEW_OBJECTS:
			{
				//There are too many unique instances of a particular type of view object.
				error = L"D3D11_ERROR_TOO_MANY_UNIQUE_VIEW_OBJECTS";
				break;
			}
			case D3D11_ERROR_DEFERRED_CONTEXT_MAP_WITHOUT_INITIAL_DISCARD: 
			{
				/*The first call to ID3D11DeviceContext::Map after either
				ID3D11Device::CreateDeferredContext or ID3D11DeviceContext::FinishCommandList per
				Resource was not D3D11_MAP_WRITE_DISCARD.*/
				error = L"D3D11_ERROR_DEFERRED_CONTEXT_MAP_WITHOUT_INITIAL_DISCARD";
				break;
			}
			case D3DERR_INVALIDCALL:
			case DXGI_ERROR_INVALID_CALL: 
			{
				//The method call is invalid.For example, a method's parameter may not be a 
				//valid pointer.
				error = L"DXGI_ERROR_INVALID_CALL";
				break;
			}
			case D3DERR_WASSTILLDRAWING:
			case DXGI_ERROR_WAS_STILL_DRAWING: 
			{
				//The previous blit operation that is transferring information to or 
				//from this surface is incomplete.
				error = L"DXGI_ERROR_WAS_STILL_DRAWING";
				break;
			}
			case E_FAIL: 
			{
				//Attempted to create a device with the debug layer enabled and the 
				//layer is not installed.
				error = L"E_FAIL";
				break;
			}
			case E_INVALIDARG: 
			{
				//An invalid parameter was passed to the returning function.
				error = L"E_INVALIDARG";
				break;
			}
			case E_OUTOFMEMORY: 
			{
				//Direct3D could not allocate sufficient memory to complete the call.
				error = L"E_OUTOFMEMORY";
				break;
			}
			case E_NOTIMPL: 
			{
				//The method call isn't implemented with the passed parameter combination.
				error = L"E_NOTIMPL";
				break;
			}
			case DXGI_ERROR_DEVICE_HUNG:
			{
				/*The application's device failed due to badly formed commands sent by
				the application. This is an design-time issue that should be investigated and fixed*/
				error = L"DXGI_ERROR_DEVICE_HUNG";
				break;
			}
			case DXGI_ERROR_DEVICE_REMOVED:
			{
				/*The video card has been physically removed from the system,
				or a driver upgrade for the video card has occurred.The application
				should destroy and recreate the device.For help debugging the problem,
				call ID3D10Device::GetDeviceRemovedReason.*/
				error = L"DXGI_ERROR_DEVICE_REMOVED";
				break;
			}
			case DXGI_ERROR_DEVICE_RESET:
			{
				/*The device failed due to a badly formed command.This is a run - time
				issue; The application should destroy and recreate the device.*/
				error = L"DXGI_ERROR_DEVICE_RESET";
				break;
			}
			case DXGI_ERROR_DRIVER_INTERNAL_ERROR:
			{
				/*The driver encountered a problem and was put into the device removed state.*/
				error = L"DXGI_ERROR_DRIVER_INTERNAL_ERROR";
				break;
			}
			case DXGI_ERROR_FRAME_STATISTICS_DISJOINT:
			{
				/*An event(for example, a power cycle) interrupted the gathering of
				presentation statistics.*/
				error = L"DXGI_ERROR_FRAME_STATISTICS_DISJOINT";
				break;
			}
			case DXGI_ERROR_GRAPHICS_VIDPN_SOURCE_IN_USE:
			{
				/*The application attempted to acquire exclusive ownership of an output,
				but failed because some other application(or device within the application)
				already acquired ownership.*/
				error = L"DXGI_ERROR_GRAPHICS_VIDPN_SOURCE_IN_USE";
				break;
			}
			case DXGI_ERROR_MORE_DATA:
			{
				/*The buffer supplied by the application is not big enough to hold
				the requested data.*/
				error = L"DXGI_ERROR_MORE_DATA";
				break;
			}

			case DXGI_ERROR_NONEXCLUSIVE:
			{
				/*A global counter resource is in use, and the Direct3D device
				can't currently use the counter resource.*/
				error = L"DXGI_ERROR_NONEXCLUSIVE";
				break;
			}
			case DXGI_ERROR_NOT_CURRENTLY_AVAILABLE:
			{
				/*The resource or request is not currently available, but it
				might become available later.*/
				error = L"DXGI_ERROR_NOT_CURRENTLY_AVAILABLE";
				break;
			}
			case DXGI_ERROR_NOT_FOUND:
			{
				/*When calling IDXGIObject::GetPrivateData, the GUID passed in is not
				recognized as one previously passed to IDXGIObject::SetPrivateData or
				IDXGIObject::SetPrivateDataInterface.When calling IDXGIFactory::EnumAdapters
				or IDXGIAdapter::EnumOutputs, the enumerated ordinal is out of range.*/
				error = L"DXGI_ERROR_NOT_FOUND";
				break;
			}
			case DXGI_ERROR_REMOTE_CLIENT_DISCONNECTED:
			{
				//Reserved
				error = L"DXGI_ERROR_REMOTE_CLIENT_DISCONNECTED";
				break;
			}
			case DXGI_ERROR_REMOTE_OUTOFMEMORY:
			{
				//Reserved
				error = L"DXGI_ERROR_REMOTE_OUTOFMEMORY";
				break;
			}
			case DXGI_ERROR_UNSUPPORTED:
			{
				//The requested functionality is not supported by the device or the driver.
				error = L"DXGI_ERROR_UNSUPPORTED";
				break;
			}
			case DXGI_ERROR_ACCESS_LOST:
			{
				/*The desktop duplication interface is invalid.The desktop duplication
				interface typically becomes invalid when a different type of image is
				displayed on the desktop.*/
				error = L"DXGI_ERROR_ACCESS_LOST";
				break;
			}
			case DXGI_ERROR_WAIT_TIMEOUT:
			{
				//The time - out interval elapsed before the next desktop frame was available.
				error = L"DXGI_ERROR_WAIT_TIMEOUT";
				break;
			}
			case DXGI_ERROR_SESSION_DISCONNECTED:
			{
				//The Remote Desktop Services session is currently disconnected.
				error = L"DXGI_ERROR_SESSION_DISCONNECTED";
				break;
			}
			case DXGI_ERROR_RESTRICT_TO_OUTPUT_STALE:
			{
				/*The DXGI output(monitor) to which the swap chain content was restricted is
				now disconnected or changed.*/
				error = L"DXGI_ERROR_RESTRICT_TO_OUTPUT_STALE";
				break;
			}

			case DXGI_ERROR_CANNOT_PROTECT_CONTENT:
			{
				/*DXGI can't provide content protection on the swap chain. This error is
				typically caused by an older driver, or when you use a swap chain that
				is incompatible with content protection.*/
				error = L"DXGI_ERROR_CANNOT_PROTECT_CONTENT";
				break;
			}

			case DXGI_ERROR_ACCESS_DENIED:
			{
				/*You tried to use a resource to which you did not have the required
				access privileges.This error is most typically caused when you write
				to a shared resource with read - only access.*/
				error = L"DXGI_ERROR_ACCESS_DENIED";
				break;
			}

			case DXGI_ERROR_NAME_ALREADY_EXISTS:
			{
				/*The supplied name of a resource in a call to
				IDXGIResource1::CreateSharedHandle is already associated with some
				other resource.*/
				error = L"DXGI_ERROR_NAME_ALREADY_EXISTS";
				break;
			}

			case DXGI_ERROR_SDK_COMPONENT_MISSING:
			{
				//The operation depends on an SDK component that is missing or mismatched.
				error = L"DXGI_ERROR_SDK_COMPONENT_MISSING";
				break;
			}
			default:
			{
				error = L"Unknown error";
				break;
			}
		}

		return error;

	}

};