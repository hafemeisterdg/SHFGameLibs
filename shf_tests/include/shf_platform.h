#if !defined(SHF_PLATFORM_H)
#define SHF_PLATFORM_H

// API Decls
// ================================================
#if defined(_WIN32)
#if defined(BUILD_LIBYTPE_SHARED)
#define SHF_PLATFORM_API __declspec(dllexport)
#elif defined(USE_LIBTYPE_SHARED)
#define SHF_PLATFORM_API __declspec(dllimport)
#else
#define SHF_PLATFORM_API 
#endif // dll check
#else
#define SHF_PLATFORM_API 
#endif // API decls
// ================================================

// Platform Detection
// ================================================
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#define SHF_PLATFORM_WINDOWS
#ifdef _WIN64
#define SHF_PLATFORM_WINDOWS64
#else
#define SHF_PLATFORM_WINDOWS32
#endif
#elif defined(__APPLE__)
#include <TargetConditionals.h>
#if TARGET_OS_MAC
#define SHF_PLATFORM_MAC
#else
// Unsupported apple
#endif
#elif defined(__linux__)
#define SHF_PLATFORM_LINUX
#endif // platform detection
// ================================================

#include <stdint.h>

namespace shf {
	namespace platform {
		typedef void* Platform_Api_Object;

		enum File_Permissions {
			File_Permissions_Execute,
			File_Permissions_Read,
			File_Permissions_Write
		};

		struct File {
			Platform_Api_Object platform_file;

			uint32_t            size;
			File_Permissions    perms;
		};

		enum Key_Action {
			Key_Action_Pressed,
			Key_Action_Released
		};

		enum Key_Code {
			Key_Code_Undefined,

			// Alphabetical
			Key_Code_A,
			Key_Code_B,
			Key_Code_C,
			Key_Code_D,
			Key_Code_E,
			Key_Code_F,
			Key_Code_G,
			Key_Code_H,
			Key_Code_I,
			Key_Code_J,
			Key_Code_K,
			Key_Code_L,
			Key_Code_M,
			Key_Code_N,
			Key_Code_O,
			Key_Code_P,
			Key_Code_Q,
			Key_Code_R,
			Key_Code_S,
			Key_Code_T,
			Key_Code_U,
			Key_Code_V,
			Key_Code_W,
			Key_Code_X,
			Key_Code_Y,
			Key_Code_Z,

			// Function Keys
			Key_Code_F1,
			Key_Code_F2,
			Key_Code_F3,
			Key_Code_F4,
			Key_Code_F5,
			Key_Code_F6,
			Key_Code_F7,
			Key_Code_F8,
			Key_Code_F9,
			Key_Code_F10,
			Key_Code_F11,
			Key_Code_F12,

			// Numeric
			Key_Code_1,
			Key_Code_2,
			Key_Code_3,
			Key_Code_4,
			Key_Code_5,
			Key_Code_6,
			Key_Code_7,
			Key_Code_8,
			Key_Code_9,
			Key_Code_0,

			// Utility
			Key_Code_Caps_Lock,
			Key_Code_Delete,
			Key_Code_End,
			Key_Code_Insert,
			Key_Code_Num_Lock,
			Key_Code_Page_Down,
			Key_Code_Page_Up,
			Key_Code_Print_Screen,
			Key_Code_Scroll_Lock,
		};

		enum Key_Modifier {
			Key_Modifier_Alt,
			Key_Modifier_Control,
			Key_Modifier_Shift,
		};

		typedef uint8_t Key_Modifiers;

		typedef void (*PFN_keyboard_callback)(Key_Code, Key_Modifiers, Key_Action);

		enum Socket_Protocol {
			Socket_Protocol_Tcp,
			Socket_Protocol_Udp
		};

		struct Socket {
			Platform_Api_Object platform_socket;

			Socket_Protocol     protocol;
		};

		struct Window {
			Platform_Api_Object platform_window;

			struct {
				uint8_t  buffer[256];
				uint32_t length;
			} title;

			PFN_keyboard_callback keyboard_callback;

			uint32_t width;
			uint32_t height;

			bool should_close;

			void SHF_PLATFORM_API center_on_display();
			void SHF_PLATFORM_API hide();
			void SHF_PLATFORM_API poll_input();
			void SHF_PLATFORM_API resize(uint32_t width, uint32_t height);
			void SHF_PLATFORM_API set_keyboard_callback(PFN_keyboard_callback callback);
			void SHF_PLATFORM_API set_position(uint32_t x, uint32_t y);
			void SHF_PLATFORM_API set_title(const char* title);
			void SHF_PLATFORM_API show();
		};

		SHF_PLATFORM_API Window* create_window(const char* title, uint32_t width, uint32_t height);
		SHF_PLATFORM_API void    destroy_window(Window* window);

	}
}

#endif // SHF_PLATFORM_H

#if defined(SHF_PLATFORM_IMPL)
#include <stdlib.h>

#define bit_set(a,b) ((a) |= (1ULL<<(b)))
#define bit_clear(a,b) ((a) &= ~(1ULL<<(b)))
#define bit_flip(a,b) ((a) ^= (1ULL<<(b)))
#define bit_check(a,b) (!!((a) & (1ULL<<(b)))) 

#define bitmask_set(x,y) ((x) |= (y))
#define bitmask_clear(x,y) ((x) &= (~(y)))
#define bitmask_flip(x,y) ((x) ^= (y))
#define bitmask_check_all(x,y) (!(~(x) & (y)))
#define bitmask_check_any(x,y) ((x) & (y))
inline uint64_t unpack_bits_from_offset(uint64_t num, uint8_t num_bits, uint8_t offset) {
	return (num >> offset) & ((1 << num_bits) - 1);
}

#if defined(SHF_PLATFORM_WINDOWS)
#include <windows.h>

namespace shf {
	namespace platform {
		struct _Win_Keypress_Lparam_Data {
			uint16_t repeat_count;
			uint8_t  scan_code;
			uint8_t  extended_key;
			uint8_t  context_code;
			uint8_t  previous_state;
			uint8_t  transition_state;
		};

		static bool _win_is_window_class_registered = false;
		static bool _win_register_window_class();
		static LRESULT CALLBACK _win_window_callback(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
		static void _win_default_keyboard_callback(Key_Code code, Key_Modifiers mods, Key_Action action);
		static _Win_Keypress_Lparam_Data _win_breakdown_keypress_lparam(LPARAM lparam);
		static Key_Code _win_covert_keycode(WPARAM wparam);

		Window* create_window(const char* title, uint32_t width, uint32_t height) {
			if (!title || width <= 0 || height <= 0) return 0;

			if (!_win_is_window_class_registered) {
				if (!_win_register_window_class()) return 0;

				_win_is_window_class_registered = true;
			}

			HWND win_window_handle = CreateWindowExA(0, "shf_window_class", title, WS_OVERLAPPEDWINDOW, 0, 0, 1, 1, 0, 0, GetModuleHandleA(0), 0);
			if (!win_window_handle) {
				DWORD error_code = GetLastError();

				// Handling per error code optionally.
				switch (error_code) {

					default: return 0;
				}
			}

			Window* window = (Window*)malloc(sizeof(Window));
			if (!window) {
				DestroyWindow(win_window_handle);

				return 0;
			}

			SetPropA(win_window_handle, "shf_window", (HANDLE)window);
			window->should_close = false;
			window->keyboard_callback = &_win_default_keyboard_callback;

			window->platform_window = (Platform_Api_Object)win_window_handle;
			window->set_title(title);
			window->resize(width, height);
			window->center_on_display();
			window->show();

			return window;
		}

		void destroy_window(Window* window) {
			if (!window || !window->platform_window) return;

			DestroyWindow((HWND)window->platform_window);
			free(window);
		}

		void Window::center_on_display() {
			uint32_t screen_width  = GetSystemMetrics(SM_CXSCREEN);
			uint32_t screen_height = GetSystemMetrics(SM_CYSCREEN);

			uint32_t screen_center_x = screen_width / 2;
			uint32_t screen_center_y = screen_height / 2;

			set_position(screen_center_x - (width / 2), screen_center_y - (height / 2));
		}

		void Window::hide() {
			ShowWindow((HWND)platform_window, SW_HIDE);
		}

		void Window::poll_input() {
			MSG window_msg = {};

			while (PeekMessageA(&window_msg, (HWND)platform_window, 0, 0, PM_REMOVE)) {
				if (window_msg.message == WM_QUIT) {
					should_close = true;

					break;
				}
				else {
					TranslateMessage(&window_msg);
					DispatchMessageA(&window_msg);
				}
			}
		}

		void Window::resize(uint32_t width, uint32_t height) {
			this->width  = width;
			this->height = height;

			SetWindowPos((HWND)platform_window, HWND_TOP, 0, 0, width, height, SWP_NOMOVE);
		}

		void Window::show() {
			ShowWindow((HWND)platform_window, SW_SHOW);
		}

		void Window::set_keyboard_callback(PFN_keyboard_callback callback) {
			this->keyboard_callback = callback;
		}

		void Window::set_position(uint32_t x, uint32_t y) {
			SetWindowPos((HWND)platform_window, HWND_TOP, x, y, x + width, y + height, SWP_NOSIZE);
		}

		void Window::set_title(const char* title) {
			uint32_t title_length = strlen(title);

			this->title.length = title_length;
			memset(&this->title.buffer[0], 0, 256);
			memcpy(&this->title.buffer[0], title, title_length);
			SetWindowTextA((HWND)platform_window, (LPCSTR)title);
		}

		bool _win_register_window_class() {
			if (_win_is_window_class_registered) return true;

			WNDCLASSEXA window_class   = {};
			window_class.cbSize        = sizeof(window_class);
			window_class.cbClsExtra    = 0;
			window_class.cbWndExtra    = 0;
			window_class.style         = CS_OWNDC | CS_VREDRAW | CS_HREDRAW | CS_GLOBALCLASS;
			window_class.lpfnWndProc   = _win_window_callback;
			window_class.hInstance     = GetModuleHandle(0);
			window_class.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
			window_class.hCursor       = LoadCursor(NULL, IDC_ARROW);
			window_class.lpszClassName = "shf_window_class";

			if (RegisterClassExA(&window_class) == 0) return false;

			return true;
		}

		LRESULT CALLBACK _win_window_callback(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
			Window* window = (Window*)GetPropA(hwnd, "shf_window");

			switch (msg) {
				case WM_CLOSE: {
					window->should_close = true;
				} break;

				case WM_KEYDOWN: {
					_Win_Keypress_Lparam_Data keypress_data = _win_breakdown_keypress_lparam(lparam);
					Key_Code key_code = _win_covert_keycode(wparam);

					window->keyboard_callback(key_code, 0, Key_Action_Pressed);
				} break;

				case WM_KEYUP: {
					_Win_Keypress_Lparam_Data keypress_data = _win_breakdown_keypress_lparam(lparam);
					Key_Code key_code = _win_covert_keycode(wparam);

					window->keyboard_callback(key_code, 0, Key_Action_Released);
				} break;

				case WM_SYSKEYDOWN: {

				} break;

				case WM_SIZE: {
					uint16_t new_width = lparam;
					uint16_t new_height = lparam >> 16;

					window->width  = new_width;
					window->height = new_height;
				} break;

				case WM_SYSKEYUP: {

				} break;
			}

			return DefWindowProcA(hwnd, msg, wparam, lparam);
		}

		static void _win_default_keyboard_callback(Key_Code code, Key_Modifiers mods, Key_Action action) {
			// Do nothing
		}

		static _Win_Keypress_Lparam_Data _win_breakdown_keypress_lparam(LPARAM lparam) {
			_Win_Keypress_Lparam_Data data;

			data.repeat_count     = unpack_bits_from_offset(lparam, 16, 0);
			data.scan_code        = unpack_bits_from_offset(lparam, 8, 16);
			data.extended_key     = unpack_bits_from_offset(lparam, 1, 24);
			data.context_code     = unpack_bits_from_offset(lparam, 1, 29);
			data.previous_state   = unpack_bits_from_offset(lparam, 1, 30);
			data.transition_state = unpack_bits_from_offset(lparam, 1, 31);

			return data;
		}
		
		static Key_Code _win_covert_keycode(WPARAM wparam) {
			switch (wparam) {
				// Alphabetical Keys
				case 0x41: return Key_Code_A;
				case 0x42: return Key_Code_B;
				case 0x43: return Key_Code_C;
				case 0x44: return Key_Code_D;
				case 0x45: return Key_Code_E;
				case 0x46: return Key_Code_F;
				case 0x47: return Key_Code_G;
				case 0x48: return Key_Code_H;
				case 0x49: return Key_Code_I;
				case 0x4A: return Key_Code_J;
				case 0x4B: return Key_Code_K;
				case 0x4C: return Key_Code_L;
				case 0x4D: return Key_Code_M;
				case 0x4E: return Key_Code_N;
				case 0x4F: return Key_Code_O;
				case 0x50: return Key_Code_P;
				case 0x51: return Key_Code_Q;
				case 0x52: return Key_Code_R;
				case 0x53: return Key_Code_S;
				case 0x54: return Key_Code_T;
				case 0x55: return Key_Code_U;
				case 0x56: return Key_Code_V;
				case 0x57: return Key_Code_W;
				case 0x58: return Key_Code_X;
				case 0x59: return Key_Code_Y;
				case 0x5A: return Key_Code_Z;

				// Function Keys
				case 0x70: return Key_Code_F1;
				case 0x71: return Key_Code_F2;
				case 0x72: return Key_Code_F3;
				case 0x73: return Key_Code_F4;
				case 0x74: return Key_Code_F5;
				case 0x75: return Key_Code_F6;
				case 0x76: return Key_Code_F7;
				case 0x77: return Key_Code_F8;
				case 0x78: return Key_Code_F9;
				case 0x79: return Key_Code_F10;
				case 0x7A: return Key_Code_F11;
				case 0x7B: return Key_Code_F12;

				// Numerical Keys
				case 0x31: return Key_Code_1;
				case 0x32: return Key_Code_2;
				case 0x33: return Key_Code_3;
				case 0x34: return Key_Code_4;
				case 0x35: return Key_Code_5;
				case 0x36: return Key_Code_6;
				case 0x37: return Key_Code_7;
				case 0x38: return Key_Code_8;
				case 0x39: return Key_Code_9;
				case 0x30: return Key_Code_0;

				default: return Key_Code_Undefined;
			}
		}
	}
}
#elif defined(SHF_PLATFORM_MAC)
namespace shf {
	namespace platform {
	
	}
}
#elif defined(SHF_PLATFORM_LINUX)
namespace shf {
	namespace platform {
	
	}
}
#else

#endif // platform detection

#endif // SHF_PLATFORM_IMPL
