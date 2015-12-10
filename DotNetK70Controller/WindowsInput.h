#pragma once

std::set<int> KeysDown; // Keys that are currently down
std::set<int> KeysDownSent; // Keys that were just pressed and need to be send
std::set<int> KeysUpToSend;

LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	BOOL fEatKeystroke = FALSE;

	if (nCode == HC_ACTION)
	{
		PKBDLLHOOKSTRUCT p = (PKBDLLHOOKSTRUCT)lParam;
		switch (wParam)
		{
		case WM_KEYDOWN:
		case WM_SYSKEYDOWN:
			if (std::find(KeysDown.begin(), KeysDown.end(), p->vkCode) == KeysDown.end())
			{
				KeysDown.insert(p->vkCode);
			}
			break;
		case WM_KEYUP:
		case WM_SYSKEYUP:
			KeysUpToSend.insert(p->vkCode);
			break;
		}
	}
	return(fEatKeystroke ? 1 : CallNextHookEx(NULL, nCode, wParam, lParam));
}

HHOOK hhkLowLevelKybd;
void SetKeyboardHook()
{
	hhkLowLevelKybd = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, 0, 0);
}

void UnhookKeyboard()
{
	UnhookWindowsHookEx(hhkLowLevelKybd); // releases low level hook to keyboard
}
