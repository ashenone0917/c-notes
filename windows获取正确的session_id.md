```cpp
	DWORD GetCurrSessionId() {
		DWORD dwLogonSuccSessionId = WTSGetActiveConsoleSessionId();

		VOID* pSessionBuf = NULL;
		DWORD dwSessionCount = 0;
		if (WTSEnumerateSessionsW(WTS_CURRENT_SERVER, 0, 1, (PWTS_SESSION_INFOW*)&pSessionBuf, &dwSessionCount) && pSessionBuf != NULL)
		{
			for (DWORD index = 0; index < dwSessionCount; index++) {
				auto pSessionInfo = ((WTS_SESSION_INFOW*)pSessionBuf) + index;
				if (pSessionInfo->State == WTSActive) {
					dwLogonSuccSessionId = pSessionInfo->SessionId;
					break;
				}
			}
			WTSFreeMemory(pSessionBuf);
		}
		return dwLogonSuccSessionId;
	}
```
